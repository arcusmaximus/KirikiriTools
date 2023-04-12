#include "stdafx.h"

using namespace std;

bool Patcher::PatchSignatureCheck(HMODULE hModule)
{
    void** pVerifierVTable = CompilerHelper::FindVTable(hModule, CompilerType::Msvc, "KrkrSign::VerifierImpl");
    if (pVerifierVTable == nullptr)
        return false;

    Debugger::Log(L"Patching KrkrSign::VerifierImpl");
    MemoryUtil::WritePointer(pVerifierVTable + 4, CustomGetSignatureVerificationResult);
    return true;
}

void Patcher::PatchXP3StreamCreation()
{
    void** pXP3ArchiveVTable = CompilerHelper::FindVTable("tTVPXP3Archive");
    if (pXP3ArchiveVTable == nullptr)
    {
        Debugger::Log(L"Failed to find tTVPXP3Archive vtable");
        return;
    }

    Debugger::Log(L"Located tTVPXP3Archive vtable (%08X - %s)", pXP3ArchiveVTable, CompilerHelper::CompilerType == CompilerType::Borland ? L"Borland" : L"MSVC");
    OriginalCreateStreamByIndex = pXP3ArchiveVTable[3];

    MemoryUtil::WritePointer(pXP3ArchiveVTable + 3, CompilerHelper::WrapAsInstanceMethod<CustomCreateStreamByIndex>());
}

void Patcher::PatchAutoPathExports()
{
    Kirikiri::HookScriptExport(L"void ::TVPAddAutoPath(const ttstr &)", &OriginalTVPAddAutoPath, CustomTVPAddAutoPath);
    Kirikiri::HookScriptExport(L"void ::TVPRemoveAutoPath(const ttstr &)", &OriginalTVPRemoveAutoPath, CustomTVPRemoveAutoPath);
}

void Patcher::PatchStorageMediaRegistration()
{
    Kirikiri::HookScriptExport(L"void ::TVPRegisterStorageMedia(iTVPStorageMedia *)", &OriginalTVPRegisterStorageMedia, CustomTVPRegisterStorageMedia);
    Kirikiri::HookScriptExport(L"void ::TVPUnregisterStorageMedia(iTVPStorageMedia *)", &OriginalTVPUnregisterStorageMedia, CustomTVPUnregisterStorageMedia);
}

void Patcher::CustomTVPAddAutoPath(const ttstr& url)
{
    if (&url == nullptr)
        return;

    if (!CxdecHelper::IsCxdecUrl(url))
    {
        OriginalTVPAddAutoPath(url);
        return;
    }

    ttstr filePath = CxdecHelper::CxdecUrlToXp3FilePath(url);
    if (CxdecHelper::IsCxdecArchive(filePath))
    {
        OriginalTVPAddAutoPath(url);
    }
    else
    {
        Debugger::Log(L"CustomTVPAddAutoPath(): Changing Cxdec URL %s to %s", url.c_str(), filePath.c_str());
        OriginalTVPAddAutoPath(filePath + L">");
    }
}

void Patcher::CustomTVPRemoveAutoPath(const ttstr& url)
{
    if (&url == nullptr)
        return;

    if (!CxdecHelper::IsCxdecUrl(url))
    {
        OriginalTVPRemoveAutoPath(url);
        return;
    }

    ttstr filePath = CxdecHelper::CxdecUrlToXp3FilePath(url);
    if (CxdecHelper::IsCxdecArchive(filePath))
    {
        OriginalTVPRemoveAutoPath(url);
    }
    else
    {
        Debugger::Log(L"CustomTVPRemoveAutoPath(): Changing Cxdec URL %s to %s", url.c_str(), filePath.c_str());
        OriginalTVPRemoveAutoPath(filePath + L">");
    }
}

void Patcher::CustomTVPRegisterStorageMedia(iTVPStorageMedia* pMedia)
{
    ttstr mediaName;
    pMedia->GetName(mediaName);

    if (mediaName != ttstr(L"steam")) {
        Debugger::Log(L"Hooking storage media \"%s\"", mediaName.c_str());

        void** pVtable = *(void***)pMedia;
        OriginalStorageMediaOpen[pMedia] = (decltype(CustomStorageMediaOpen)*)pVtable[6];
        MemoryUtil::WritePointer(&pVtable[6], CustomStorageMediaOpen);
    }

    OriginalTVPRegisterStorageMedia(pMedia);
}

void Patcher::CustomTVPUnregisterStorageMedia(iTVPStorageMedia* pMedia)
{
    auto it = OriginalStorageMediaOpen.find(pMedia);
    if (it != OriginalStorageMediaOpen.end())
    {
        void** pVtable = *(void***)pMedia;
        MemoryUtil::WritePointer(&pVtable[6], it->second);

        OriginalStorageMediaOpen.erase(it);
    }

    OriginalTVPUnregisterStorageMedia(pMedia);
}

tTJSBinaryStream* Patcher::CustomStorageMediaOpen(iTVPStorageMedia* pMedia, const ttstr& name, tjs_uint32 flags)
{
    static wstring folderPath = Path::GetModuleFolderPath(nullptr);

    const wchar_t* pFilePath = wcschr(name.c_str(), L'/') + 1;
    wstring looseFilePath = Path::Combine(Path::Combine(folderPath, L"unencrypted"), StringUtil::Replace<wchar_t>(pFilePath, L'/', L'\\'));
    wstring unencryptedXp3Path = Path::Combine(folderPath, L"unencrypted.xp3");

    vector<wstring> urls = { Kirikiri::FilePathToUrl(looseFilePath) };
    if (GetFileAttributes(unencryptedXp3Path.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        wstring unencryptedXp3Url = Kirikiri::FilePathToUrl(unencryptedXp3Path);
        urls.push_back(unencryptedXp3Url + L">" + pFilePath);

        const wchar_t* pFileName = wcsrchr(name.c_str(), L'/') + 1;
        if (pFileName != pFilePath)
            urls.push_back(unencryptedXp3Url + L">" + pFileName);
    }

    for (wstring& url : urls)
    {
        if (Kirikiri::TVPIsExistentStorageNoSearchNoNormalize(url.c_str()))
        {
            ttstr mediaName;
            pMedia->GetName(mediaName);
            Debugger::Log(L"Redirecting %s://%s to %s", mediaName.c_str(), name.c_str(), url.c_str());

            void* pComStream = Kirikiri::TVPCreateIStream(url.c_str(), flags);
            return Kirikiri::TVPCreateBinaryStreamAdapter(pComStream);
        }
    }

    tTJSBinaryStream* pStream = OriginalStorageMediaOpen[pMedia](pMedia, name, flags);

    static bool extractionRequested = GetFileAttributes(Path::Combine(folderPath, L"extract-unencrypted.txt").c_str()) != INVALID_FILE_ATTRIBUTES;
    if (pStream != nullptr && extractionRequested)
    {
        Debugger::Log(L"Extracting %s", pFilePath);
        WriteStreamToFile(pStream, looseFilePath);
    }

    return pStream;
}

void Patcher::WriteStreamToFile(tTJSBinaryStream* pStream, const std::wstring& filePath)
{
    vector<BYTE> data;
    data.resize(pStream->GetSize());
    pStream->Read(data.data(), data.size());
    pStream->Seek(0, SEEK_SET);

    Directory::Create(Path::GetDirectoryName(filePath));
    FileStream fileStream(filePath, L"wb+");
    fileStream.Write(data);
}

bool Patcher::CustomGetSignatureVerificationResult()
{
    return true;
}
