#include "stdafx.h"

using namespace std;

vector<PE::Section> PE::GetSections(HMODULE hModule)
{
    IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)hModule;
    IMAGE_NT_HEADERS* pNtHeaders = (IMAGE_NT_HEADERS*)((BYTE*)hModule + pDosHeader->e_lfanew);
    IMAGE_SECTION_HEADER* pSectionHeaders = IMAGE_FIRST_SECTION(pNtHeaders);

    vector<Section> sections;
    for (int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++)
    {
        Section section
        {
            .Start = (BYTE*)hModule + pSectionHeaders[i].VirtualAddress,
            .Size = (int)pSectionHeaders[i].SizeOfRawData,
            .Characteristics = pSectionHeaders[i].Characteristics
        };
        memcpy(section.Name, pSectionHeaders[i].Name, 8);
        sections.push_back(section);
    }
    return sections;
}
