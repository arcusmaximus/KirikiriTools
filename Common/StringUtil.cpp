#include "stdafx.h"

using namespace std;

wstring StringUtil::ToUTF16(const string& str)
{
    std::wstring wstr;
    wstr.resize(MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0));
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), const_cast<wchar_t*>(wstr.c_str()), wstr.size());
    return wstr;
}

string StringUtil::ToUTF8(const wstring& wstr)
{
    std::string str;
    str.resize(WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr));
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), const_cast<char*>(str.c_str()), str.size(), nullptr, nullptr);
    return str;
}
