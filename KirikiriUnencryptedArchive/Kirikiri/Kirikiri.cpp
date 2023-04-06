#include "stdafx.h"

using namespace std;

void Kirikiri::Init(const function<void()>& callback)
{
    if (!IsKirikiriExe())
    {
        Debugger::Log(L"Not a Kirikiri .exe - initialization aborted");
        return;
    }

    Debugger::RegisterDllLoadHandler(HandleDllLoaded);
    InitializationCallback = callback;
}

wstring Kirikiri::FilePathToUrl(const wstring& path)
{
    wstring url = Path::GetFullPath(path);
    if (url.size() >= 2 && url[1] == L':')
    {
        url[0] = towlower(url[0]);
        url.erase(1, 1);
    }

    url = wstring(L"file://./") + StringUtil::Replace(url, L'\\', L'/');
    return url;
}

bool Kirikiri::IsKirikiriExe()
{
    wstring exePath = Path::GetModuleFilePath(nullptr);

    DWORD handle;
    DWORD versionInfoSize = Proxy::OriginalGetFileVersionInfoSizeW(exePath.c_str(), &handle);
    if (versionInfoSize == 0)
        return false;

    vector<byte> versionInfo(versionInfoSize);
    if (!Proxy::OriginalGetFileVersionInfoW(exePath.c_str(), 0, versionInfo.size(), versionInfo.data()))
        return false;

    void* pLanguages;
    UINT languagesSize;
    if (!Proxy::OriginalVerQueryValueW(versionInfo.data(), L"\\VarFileInfo\\Translation", &pLanguages, &languagesSize) || languagesSize == 0)
        return false;

    UINT language = ((USHORT*)pLanguages)[0];
    UINT codepage = ((USHORT*)pLanguages)[1];
    wstring subBlockName = StringUtil::Format(L"\\StringFileInfo\\%04x%04x\\LegalCopyright", language, codepage);
    void* pProductName;
    UINT productNameSize;
    if (!Proxy::OriginalVerQueryValueW(versionInfo.data(), subBlockName.c_str(), &pProductName, &productNameSize))
        return false;

    wstring copyright((wchar_t*)pProductName, productNameSize);
    return copyright.find(L"KIRIKIRI", 0) != string::npos;
}

void Kirikiri::HandleDllLoaded(const wchar_t* pwszDllPath, HMODULE hDll)
{
    void* pLink = Debugger::FindExport(hDll, "V2Link");
    if (pLink == nullptr)
        return;

    OriginalV2Link = pLink;
    DetourTransactionBegin();
    DetourAttach(&OriginalV2Link, V2LinkHook);
    DetourTransactionCommit();
}

__declspec(naked) void Kirikiri::V2LinkHook()
{
    __asm
    {
        lea eax, [esp+4]
        push eax
        call HandleV2Link
        add esp, 4

        jmp OriginalV2Link
    }
}

void Kirikiri::HandleV2Link(iTVPFunctionExporter** ppExporter)
{
    DetourTransactionBegin();
    DetourDetach(&OriginalV2Link, V2LinkHook);
    DetourTransactionCommit();

    HMODULE hPlugin = DetourGetContainingModule(OriginalV2Link);

    if (ProxyFunctionExporter == nullptr)
    {
        RealFunctionExporter = *ppExporter;
        ProxyFunctionExporter = new ::ProxyFunctionExporter(RealFunctionExporter);

        tTJSString::Init();
        tTJSVariant::Init();

        ResolveScriptExport(L"void ::TVPExecuteExpression(const ttstr &,tTJSVariant *)", TVPExecuteExpression);
        ResolveScriptExport(L"int ::ZLIB_uncompress(unsigned char *,unsigned long *,const unsigned char *,unsigned long)", ZLIB_uncompress);
        ResolveScriptExport(L"bool ::TVPIsExistentStorageNoSearch(const ttstr &)", TVPIsExistentStorageNoSearch);
        ResolveScriptExport(L"bool ::TVPIsExistentStorageNoSearchNoNormalize(const ttstr &)", TVPIsExistentStorageNoSearchNoNormalize);
        ResolveScriptExport(L"IStream * ::TVPCreateIStream(const ttstr &,tjs_uint32)", TVPCreateIStream);
        ResolveScriptExport(L"tTJSBinaryStream * ::TVPCreateBinaryStreamAdapter(IStream *)", TVPCreateBinaryStreamAdapter);
    }

    if (InitializationCallback != nullptr)
    {
        if (ImportHooker::Hook(hPlugin, "ImageUnload", CustomImageUnload))
        {
            Debugger::Log(L"Hooked ImageUnload() to wait for plugin to finish verifying the game's code");
        }
        else
        {
            Debugger::Log(L"Running initialization");
            InitializationCallback();
            InitializationCallback = nullptr;
        }
    }

    ImportHooker::Hook(hPlugin, "GetProcAddress", CustomGetProcAddress);

    *ppExporter = ProxyFunctionExporter;
}

FARPROC Kirikiri::CustomGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    if (strcmp(lpProcName, "GetSystemWow64DirectoryA") == 0)
    {
        // Some games have the following buggy check for a proxy kernel32.dll:
        //
        //     GetModuleFileName(GetModuleHandle("kernel32")) == GetSystemWow64Directory() + "\\kernel32.dll"
        //
        // This of course doesn't work because GetModuleFileName() returns "C:\Windows\System32\kernel32.dll",
        // not "C:\Windows\SysWOW64\kernel32.dll" like the game expects. The result is that even the
        // unmodified game thinks it's being hacked and refuses to start.
        // 
        // The fix is to act as though GetSystemWow64DirectoryA() doesn't exist, which should make the game
        // fall back to the correct GetSystemDirectory().
        return nullptr;
    }

    return GetProcAddress(hModule, lpProcName);
}

BOOL Kirikiri::CustomImageUnload(PLOADED_IMAGE LoadedImage)
{
    if (InitializationCallback != nullptr)
    {
        wstring imageName = Path::GetFileNameWithoutExtension(StringUtil::ToUTF16(LoadedImage->ModuleName));
        wstring exeName = Path::GetFileNameWithoutExtension(Path::GetModuleFilePath(nullptr));
        if (imageName == exeName)
        {
            Debugger::Log(L"ImageUnload() called - running initialization");
            InitializationCallback();
            InitializationCallback = nullptr;
        }
    }

    return ImageUnload(LoadedImage);
}

void* Kirikiri::GetTrampoline(void* pTarget)
{
    // Certain Kirikiri plugins really don't like it if an exported function pointer
    // is not inside the engine. The solution is easy: write a jmp instruction at the end of
    // the engine's .text section (to make sure we're not clobbering any existing code)
    // and use that as the function pointer instead.
	
    static BYTE* pPrevJump = nullptr;
    
    if (pPrevJump == nullptr)
    {
        vector<PE::Section> sections = PE::GetSections(GetModuleHandle(nullptr));
        pPrevJump = sections[1].Start;
    }

    BYTE* pJump = pPrevJump - 5;
    pPrevJump = pJump;

    PageUnprotector unprotect(pJump, 5);
    *pJump = 0xE9;
    *(int*)(pJump + 1) = (BYTE*)pTarget - (pJump + 5);

    return pJump;
}
