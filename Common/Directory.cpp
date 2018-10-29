#include "stdafx.h"

using namespace std;

vector<wstring> Directory::GetFiles(const wstring& folderPath, const wstring& pattern)
{
    vector<wstring> files;

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(Path::Combine(folderPath, pattern).c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return files;

    do
    {
        files.push_back(Path::Combine(folderPath, findData.cFileName));
    } while (FindNextFile(hFind, &findData));
    FindClose(hFind);
    return files;
}
