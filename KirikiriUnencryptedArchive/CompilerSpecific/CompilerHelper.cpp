#include "stdafx.h"

using namespace std;

void CompilerHelper::Init()
{
    GameModuleHandle = GetModuleHandle(nullptr);
    GameModuleSize = DetourGetModuleSize(GameModuleHandle);
    GameSections = PE::GetSections(GameModuleHandle);

    const PE::Section& textSection = GameSections[0];
    if (MemoryUtil::FindData(textSection.Start, textSection.Size, "Borland", 7) != nullptr)
        CompilerType = CompilerType::Borland;
    else
        CompilerType = CompilerType::Msvc;
}

void** CompilerHelper::FindVTable(const string& className)
{
    const PE::Section& textSection = GameSections[0];
    void* pCodeStart = textSection.Start;
    void* pCodeEnd = (BYTE*)textSection.Start + textSection.Size;

    for (int i = 1; i < GameSections.size(); i++)
    {
        const PE::Section& section = GameSections[i];
        void* pSectionStart = section.Start;
        void* pSectionEnd = (BYTE*)section.Start + section.Size;
        for (void** ppFunc = (void**)pSectionStart + 3; ppFunc < pSectionEnd; ppFunc++)
        {
            if (*ppFunc < pCodeStart || *ppFunc >= pCodeEnd)
                continue;

            if (CompilerType == CompilerType::Borland && HasBorlandTypeDescriptor(ppFunc, className))
                return ppFunc;

            if (CompilerType == CompilerType::Msvc && HasMsvcTypeDescriptor(ppFunc, className))
                return ppFunc;
        }
    }
    return nullptr;
}

bool CompilerHelper::HasBorlandTypeDescriptor(void** pVTable, const string& className)
{
    void* pModuleStart = GameModuleHandle;
    void* pModuleEnd = (BYTE*)GameModuleHandle + GameModuleSize;

    BorlandTypeDescriptor* pTypeDescriptor = (BorlandTypeDescriptor*)pVTable[-3];
    if (pTypeDescriptor < pModuleStart || pTypeDescriptor >= pModuleEnd || pTypeDescriptor + 1 + className.size() > pModuleEnd)
        return false;

    return memcmp(pTypeDescriptor->Name, className.c_str(), className.size() + 1) == 0;
}

bool CompilerHelper::HasMsvcTypeDescriptor(void** pVTable, const string& className)
{
    void* pModuleStart = GameModuleHandle;
    void* pModuleEnd = (BYTE*)GameModuleHandle + GameModuleSize;

    MsvcRttiCompleteObjectLocator* pLocator = (MsvcRttiCompleteObjectLocator*)pVTable[-1];
    if (pLocator < pModuleStart || pLocator >= pModuleEnd || pLocator + 1 > pModuleEnd ||
        pLocator->Signature != 0 ||
        pLocator->pTypeDescriptor < pModuleStart || pLocator->pTypeDescriptor >= pModuleEnd || pLocator->pTypeDescriptor + 1 > pModuleEnd)
    {
        return false;
    }

    const char* pRttiClassName = pLocator->pTypeDescriptor->raw_name();
    if (pRttiClassName < pModuleStart || pRttiClassName >= pModuleEnd || pRttiClassName + className.size() + 7 > pModuleEnd)
        return false;

    return memcmp(pRttiClassName, ".?A", 3) == 0 &&
           memcmp(pRttiClassName + 4, className.c_str(), className.size()) == 0 &&
           memcmp(pRttiClassName + 4 + className.size(), "@@\0", 3) == 0;
}
