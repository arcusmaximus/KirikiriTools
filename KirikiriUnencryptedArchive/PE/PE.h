#pragma once

class PE
{
public:
    struct Section
    {
        char    Name[8];
        BYTE*   Start;
        int     Size;
        DWORD   Characteristics;
    };

    static std::vector<Section>         GetSections         (HMODULE hModule);
};
