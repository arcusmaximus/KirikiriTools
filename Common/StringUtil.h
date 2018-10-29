#pragma once

class StringUtil
{
public:
    static std::string      Format          (const char* pFormat, ...);
    static std::wstring     Format          (const wchar_t* pFormat, ...);

    static std::wstring     ToUTF16         (const std::string& str);
    static std::string      ToUTF8          (const std::wstring& wstr);
};
