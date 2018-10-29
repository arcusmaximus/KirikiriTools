#include "stdafx.h"

using namespace std;

void* PluginPatcher::ReturnInstruction;
HMODULE PluginPatcher::PatcherPluginHandle;
wchar_t PluginPatcher::PatcherPluginName[MAX_PATH];

tTVPXP3ArchiveExtractionFilter PluginPatcher::OriginalXP3ArchiveExtractionFilter;
int (__stdcall* PluginPatcher::OriginalTVPGetAutoLoadPluginCount)();
void (__stdcall* PluginPatcher::OriginalTVPSetXP3ArchiveExtractionFilter)(tTVPXP3ArchiveExtractionFilter pFilter);
void (__stdcall* PluginPatcher::OriginalTVPAddAutoPath)(const ttstr& path);
void (__stdcall* PluginPatcher::OriginalTVPRemoveAutoPath)(const ttstr& path);

void PluginPatcher::Patch()
{
    Init();
    LoadAndPatchPlugins();
    HookTvpFunctions();
}

void PluginPatcher::Init()
{
    ReturnInstruction = MemoryUtil::FindByte(Kirikiri::TextSection.Start, Kirikiri::TextSection.Length, 0xC3);

    PatcherPluginHandle = DetourGetContainingModule(&LoadAndPatchPlugins);
    
    wchar_t wszPatcherPluginPath[MAX_PATH];
    GetModuleFileName(PatcherPluginHandle, wszPatcherPluginPath, MAX_PATH);
    wchar_t* pwszFileName = wcsrchr(wszPatcherPluginPath, L'\\') + 1;
    wcscpy(PatcherPluginName, pwszFileName);
}

void PluginPatcher::LoadAndPatchPlugins()
{
    wchar_t path[MAX_PATH];
    GetModuleFileName(nullptr, path, MAX_PATH);
    wchar_t* pwszSubfolder = wcsrchr(path, L'\\') + 1;
    wcscpy(pwszSubfolder, L"plugin\\*.tpm");
    wchar_t* pwszFilename = wcsrchr(path, L'\\') + 1;

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(path, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do
    {
        wcscpy(pwszFilename, findData.cFileName);
        HMODULE hPlugin = LoadLibrary(path);
        if (hPlugin != PatcherPluginHandle)
            PatchPlugin(hPlugin);
    } while (FindNextFile(hFind, &findData));
    FindClose(hFind);
}

void PluginPatcher::PatchPlugin(HMODULE hPlugin)
{
    DetourEnumerateImportsEx(hPlugin, nullptr, nullptr, HandleFunctionImport);
}

BOOL PluginPatcher::HandleFunctionImport(PVOID pContext, ULONG ordinal, PCSTR pszName, PVOID* ppFunc)
{
    if (pszName == nullptr)
        return TRUE;

    if (strcmp(pszName, "FindFirstFileW") == 0)
        MemoryUtil::WritePointer(ppFunc, CustomFindFirstFile);
    else if (strcmp(pszName, "FindNextFileW") == 0)
        MemoryUtil::WritePointer(ppFunc, CustomFindNextFile);

    return TRUE;
}

HANDLE PluginPatcher::CustomFindFirstFile(PCWSTR pwszFileName, PWIN32_FIND_DATA pFindData)
{
    HANDLE hFind = FindFirstFile(pwszFileName, pFindData);
    if (hFind == INVALID_HANDLE_VALUE)
        return INVALID_HANDLE_VALUE;

    if (_wcsicmp(pFindData->cFileName, PatcherPluginName) == 0)
    {
        if (!FindNextFile(hFind, pFindData))
        {
            FindClose(hFind);
            return INVALID_HANDLE_VALUE;
        }
    }

    return hFind;
}

BOOL PluginPatcher::CustomFindNextFile(HANDLE hFindFile, PWIN32_FIND_DATA pFindData)
{
    if (!FindNextFile(hFindFile, pFindData))
        return FALSE;

    if (_wcsicmp(pFindData->cFileName, PatcherPluginName) == 0)
        return FindNextFile(hFindFile, pFindData);

    return TRUE;
}

void PluginPatcher::HookTvpFunctions()
{
    Kirikiri::ReplaceScriptExport(L"tjs_int ::TVPGetAutoLoadPluginCount()", CustomTVPGetAutoLoadPluginCount, OriginalTVPGetAutoLoadPluginCount);
    Kirikiri::ReplaceScriptExport(L"void ::TVPSetXP3ArchiveExtractionFilter(tTVPXP3ArchiveExtractionFilter)", CustomTVPSetXP3ArchiveExtractionFilter, OriginalTVPSetXP3ArchiveExtractionFilter);
    Kirikiri::ReplaceScriptExport(L"void ::TVPAddAutoPath(const ttstr &)", CustomTVPAddAutoPath, OriginalTVPAddAutoPath);
    Kirikiri::ReplaceScriptExport(L"void ::TVPRemoveAutoPath(const ttstr &)", CustomTVPRemoveAutoPath, OriginalTVPRemoveAutoPath);
}

int PluginPatcher::CustomTVPGetAutoLoadPluginCount()
{
    return OriginalTVPGetAutoLoadPluginCount() - 1;
}

void PluginPatcher::CustomTVPSetXP3ArchiveExtractionFilter(tTVPXP3ArchiveExtractionFilter pFilter)
{
    OriginalXP3ArchiveExtractionFilter = pFilter;
    OriginalTVPSetXP3ArchiveExtractionFilter(CustomXP3ArchiveExtractionFilter);
}

void PluginPatcher::CustomXP3ArchiveExtractionFilter(tTVPXP3ExtractionFilterInfo* pInfo)
{
    if (pInfo->FileHash == 0)
        return;

    __asm
    {
        push retaddr
        push pInfo
        push ReturnInstruction
        jmp OriginalXP3ArchiveExtractionFilter

    retaddr:
    }
}

void PluginPatcher::CustomTVPAddAutoPath(const ttstr& url)
{
    if (!CxdecHelper::IsCxdecUrl(url))
    {
        OriginalTVPAddAutoPath(url);
        return;
    }

    ttstr filePath = CxdecHelper::CxdecUrlToXp3FilePath(url);
    if (CxdecHelper::IsCxdecArchive(filePath))
        OriginalTVPAddAutoPath(url);
    else
        OriginalTVPAddAutoPath(filePath + L">");
}

void PluginPatcher::CustomTVPRemoveAutoPath(const ttstr& url)
{
    if (!CxdecHelper::IsCxdecUrl(url))
    {
        OriginalTVPRemoveAutoPath(url);
        return;
    }

    ttstr filePath = CxdecHelper::CxdecUrlToXp3FilePath(url);
    if (CxdecHelper::IsCxdecArchive(filePath))
        OriginalTVPRemoveAutoPath(url);
    else
        OriginalTVPRemoveAutoPath(filePath + L">");
}
