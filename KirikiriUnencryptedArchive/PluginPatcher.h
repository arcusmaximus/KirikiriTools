#pragma once

class PluginPatcher
{
public:
    static void             Patch                                   ();

private:
    static void             Init                                    ();

    static void             LoadAndPatchPlugins                     ();

    static void             PatchPlugin                             (HMODULE hPlugin);
    static BOOL __stdcall   HandleFunctionImport                    (PVOID pContext, ULONG ordinal, PCSTR pszName, PVOID* ppFunc);
    static HANDLE __stdcall CustomFindFirstFile                     (PCWSTR pwszFileName, PWIN32_FIND_DATA pFindData);
    static BOOL __stdcall   CustomFindNextFile                      (HANDLE hFind, PWIN32_FIND_DATA pFindData);

    static void             HookTvpFunctions                        ();
    static int __stdcall    CustomTVPGetAutoLoadPluginCount         ();
    static void __stdcall   CustomTVPSetXP3ArchiveExtractionFilter  (tTVPXP3ArchiveExtractionFilter pFilter);
    static void __stdcall   CustomXP3ArchiveExtractionFilter        (tTVPXP3ExtractionFilterInfo* pInfo);
    static void __stdcall   CustomTVPAddAutoPath                    (const ttstr& url);
    static void __stdcall   CustomTVPRemoveAutoPath                 (const ttstr& url);

    static void* ReturnInstruction;
    static HMODULE PatcherPluginHandle;
    static wchar_t PatcherPluginName[MAX_PATH];

    static int  (__stdcall* OriginalTVPGetAutoLoadPluginCount)();
    static void (__stdcall* OriginalTVPSetXP3ArchiveExtractionFilter)(tTVPXP3ArchiveExtractionFilter pFilter);
    static tTVPXP3ArchiveExtractionFilter OriginalXP3ArchiveExtractionFilter;
    static void (__stdcall* OriginalTVPAddAutoPath)(const ttstr& path);
    static void (__stdcall* OriginalTVPRemoveAutoPath)(const ttstr& path);
};
