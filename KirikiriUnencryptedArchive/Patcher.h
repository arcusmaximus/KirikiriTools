#pragma once

class Patcher
{
public:
    static void             Patch                                   ();

private:
    static void             Init                                    ();

    static void             HookTvpFunctions                        ();
    static void __stdcall   CustomTVPSetXP3ArchiveExtractionFilter  (tTVPXP3ArchiveExtractionFilter pFilter);
    static void __stdcall   CustomXP3ArchiveExtractionFilter        (tTVPXP3ExtractionFilterInfo* pInfo);
    static void __stdcall   CustomTVPAddAutoPath                    (const ttstr& url);
    static void __stdcall   CustomTVPRemoveAutoPath                 (const ttstr& url);

    static void* ReturnInstruction;

    static void (__stdcall* OriginalTVPSetXP3ArchiveExtractionFilter)(tTVPXP3ArchiveExtractionFilter pFilter);
    static tTVPXP3ArchiveExtractionFilter OriginalXP3ArchiveExtractionFilter;
    static void (__stdcall* OriginalTVPAddAutoPath)(const ttstr& path);
    static void (__stdcall* OriginalTVPRemoveAutoPath)(const ttstr& path);
};
