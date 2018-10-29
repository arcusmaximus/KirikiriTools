#pragma once

class Directory
{
public:
    static std::vector<std::wstring>        GetFiles        (const std::wstring& folderPath, const std::wstring& pattern);
};
