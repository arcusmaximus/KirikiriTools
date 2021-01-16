#include "stdafx.h"

using namespace std;

string StringUtil::Format(const char* pFormat, ...)
{
    va_list args;

    va_start(args, pFormat);
    int length = snprintf(nullptr, 0, pFormat, args);
    va_end(args);

    va_start(args, pFormat);
    string result;
    result.resize(length);
    sprintf(const_cast<char*>(result.c_str()), pFormat, args);
    va_end(args);

    return result;
}

wstring StringUtil::Format(const wchar_t* pFormat, ...)
{
    va_list args;

    va_start(args, pFormat);
    int length = _vsnwprintf(nullptr, 0, pFormat, args);
    va_end(args);

    va_start(args, pFormat);
    wstring result;
    result.resize(length);
    wvsprintf(const_cast<wchar_t*>(result.c_str()), pFormat, args);
    va_end(args);

    return result;
}

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
