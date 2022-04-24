#pragma once

class Directory
{
public:
    static void                             Create              (const std::wstring& folderPath);
    static std::vector<std::wstring>        GetFiles            (const std::wstring& folderPath, const std::wstring& pattern);
    static std::wstring                     GetTempDirectory    ();
};
