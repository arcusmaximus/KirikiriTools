#include "stdafx.h"

using namespace std;

void CompilerHelper::Init()
{
    HMODULE hGame = GetModuleHandle(nullptr);
    vector<PE::Section> gameSections = PE::GetSections(hGame);

    const PE::Section& textSection = gameSections[0];
    if (MemoryUtil::FindData(textSection.Start, textSection.Size, "Borland", 7) != nullptr)
        CompilerType = CompilerType::Borland;
    else
        CompilerType = CompilerType::Msvc;
}

void** CompilerHelper::FindVTable(const string& className)
{
    return FindVTable(GetModuleHandle(nullptr), CompilerType, className);
}

void** CompilerHelper::FindVTable(HMODULE hModule, ::CompilerType compilerType, const std::string& className)
{
    void* pModuleStart = (void*)hModule;
    void* pModuleEnd = (BYTE*)pModuleStart + DetourGetModuleSize(hModule);

    vector<PE::Section> sections = PE::GetSections(hModule);
    const PE::Section& textSection = sections[0];
    void* pCodeStart = textSection.Start;
    void* pCodeEnd = (BYTE*)textSection.Start + textSection.Size;

    string typeDescriptorClassName;
    switch (compilerType)
    {
        case CompilerType::Borland:
            typeDescriptorClassName = className;
            break;

        case CompilerType::Msvc:
            vector<string> parts = StringUtil::Split<char>(className, "::");
            std::ranges::reverse(parts);
            typeDescriptorClassName = StringUtil::Join<char>(parts, "@");
            break;
    }

    for (int i = 1; i < sections.size(); i++)
    {
        const PE::Section& section = sections[i];
        void* pSectionStart = section.Start;
        void* pSectionEnd = (BYTE*)section.Start + section.Size;
        for (void** ppFunc = (void**)pSectionStart + 3; ppFunc < pSectionEnd; ppFunc++)
        {
            if (*ppFunc < pCodeStart || *ppFunc >= pCodeEnd)
                continue;

            if (compilerType == CompilerType::Borland && HasBorlandTypeDescriptor(ppFunc, typeDescriptorClassName, pModuleStart, pModuleEnd))
                return ppFunc;

            if (compilerType == CompilerType::Msvc && HasMsvcTypeDescriptor(ppFunc, typeDescriptorClassName, pModuleStart, pModuleEnd))
                return ppFunc;
        }
    }
    return nullptr;
}

bool CompilerHelper::HasBorlandTypeDescriptor(void** pVTable, const string& className, void* pModuleStart, void* pModuleEnd)
{
    BorlandTypeDescriptor* pTypeDescriptor = (BorlandTypeDescriptor*)pVTable[-3];
    if (pTypeDescriptor < pModuleStart || pTypeDescriptor >= pModuleEnd || pTypeDescriptor + 1 + className.size() > pModuleEnd)
        return false;

    return memcmp(pTypeDescriptor->Name, className.c_str(), className.size() + 1) == 0;
}

bool CompilerHelper::HasMsvcTypeDescriptor(void** pVTable, const string& className, void* pModuleStart, void* pModuleEnd)
{
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
