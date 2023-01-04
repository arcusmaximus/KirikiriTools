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

        InitializationCallback();
        InitializationCallback = nullptr;
    }

    *ppExporter = ProxyFunctionExporter;
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
