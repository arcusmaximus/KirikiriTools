#include "stdafx.h"

using namespace std;

void* Patcher::ReturnInstruction;

tTVPXP3ArchiveExtractionFilter Patcher::OriginalXP3ArchiveExtractionFilter;
void (__stdcall* Patcher::OriginalTVPSetXP3ArchiveExtractionFilter)(tTVPXP3ArchiveExtractionFilter pFilter);
void (__stdcall* Patcher::OriginalTVPAddAutoPath)(const ttstr& path);
void (__stdcall* Patcher::OriginalTVPRemoveAutoPath)(const ttstr& path);

void Patcher::Patch()
{
    Init();
    HookTvpFunctions();
}

void Patcher::Init()
{
    ReturnInstruction = MemoryUtil::FindByte(Kirikiri::GameTextSection.Start, Kirikiri::GameTextSection.Size, 0xC3);
    if (ReturnInstruction != nullptr)
        OutputDebugString(L"Successfully found return instruction");
}

void Patcher::HookTvpFunctions()
{
    Kirikiri::ReplaceScriptExport(L"void ::TVPSetXP3ArchiveExtractionFilter(tTVPXP3ArchiveExtractionFilter)", CustomTVPSetXP3ArchiveExtractionFilter, OriginalTVPSetXP3ArchiveExtractionFilter);
    Kirikiri::ReplaceScriptExport(L"void ::TVPAddAutoPath(const ttstr &)", CustomTVPAddAutoPath, OriginalTVPAddAutoPath);
    Kirikiri::ReplaceScriptExport(L"void ::TVPRemoveAutoPath(const ttstr &)", CustomTVPRemoveAutoPath, OriginalTVPRemoveAutoPath);
}

void Patcher::CustomTVPSetXP3ArchiveExtractionFilter(tTVPXP3ArchiveExtractionFilter pFilter)
{
    Debugger::Log(L"CustomTVPSetXP3ArchiveExtractionFilter() called with %08X", pFilter);
    OriginalXP3ArchiveExtractionFilter = pFilter;
    OriginalTVPSetXP3ArchiveExtractionFilter(CustomXP3ArchiveExtractionFilter);
}

void Patcher::CustomXP3ArchiveExtractionFilter(tTVPXP3ExtractionFilterInfo* pInfo)
{
	// If we encounter an unencrypted file, skip the decryption
    if (pInfo->FileHash == 0)
        return;

	// Otherwise call the decryption routine, using a bit of a roundabout
	// because the routine may check that its return address is
	// inside the Kirikiri .text section.
    __asm
    {
        push retaddr
        push pInfo
        push ReturnInstruction
        jmp OriginalXP3ArchiveExtractionFilter

    retaddr:
    }
}

void Patcher::CustomTVPAddAutoPath(const ttstr& url)
{
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
