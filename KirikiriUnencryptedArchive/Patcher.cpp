#include "stdafx.h"

using namespace std;

void Patcher::Patch()
{
    PatchXP3StreamCreation();
    PatchAutoPathExports();
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

    PageUnprotector unprotect(pXP3ArchiveVTable, 16);
    pXP3ArchiveVTable[3] = CompilerHelper::WrapAsInstanceMethod<CustomCreateStreamByIndex>();
}

void Patcher::PatchAutoPathExports()
{
    Kirikiri::ReplaceScriptExport(L"void ::TVPAddAutoPath(const ttstr &)", CustomTVPAddAutoPath, OriginalTVPAddAutoPath);
    Kirikiri::ReplaceScriptExport(L"void ::TVPRemoveAutoPath(const ttstr &)", CustomTVPRemoveAutoPath, OriginalTVPRemoveAutoPath);
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
