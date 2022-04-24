#include "stdafx.h"
#include "Directory.h"

using namespace std;

void Directory::Create(const std::wstring& folderPath)
{
    CreateDirectory(folderPath.c_str(), nullptr);
}

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

wstring Directory::GetTempDirectory()
{
    DWORD pathLength = GetTempPath(0, nullptr);
    if (pathLength == 0)
        return L"";

    wstring path;
    path.resize(pathLength);
    GetTempPath(path.size(), path.data());

    path.resize(path.size() - 1);
    return path;
}
