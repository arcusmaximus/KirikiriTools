#pragma once

class StringUtil
{
private:
    struct CFunctions
    {
        template<typename TChar>
        static TChar ToLower(TChar c);

        template<>
        static char ToLower(char c)
        {
            return (char)tolower(c);
        }

        template<>
        static wchar_t ToLower(wchar_t c)
        {
            return (wchar_t)towlower(c);
        }

        template<typename TChar>
        static TChar ToUpper(TChar c);

        template<>
        static char ToUpper(char c)
        {
            return (char)toupper(c);
        }

        template<>
        static wchar_t ToUpper(wchar_t c)
        {
            return (wchar_t)towupper(c);
        }

        static int StringPrintf(char* pBuffer, int bufferSize, const char* pFormat, va_list args)
        {
            return vsnprintf(pBuffer, bufferSize, pFormat, args);
        }

        static int StringPrintf(wchar_t* pBuffer, int bufferSize, const wchar_t* pFormat, va_list args)
        {
            return _vsnwprintf(pBuffer, bufferSize, pFormat, args);
        }
    };

public:
    static std::wstring         ToUTF16         (const std::string& str);
    static std::string          ToUTF8          (const std::wstring& wstr);

    template<typename TChar>
    static std::basic_string<TChar> Format(const TChar* pFormat, va_list args)
    {
        va_list argsForLength = args;
        int length = CFunctions::StringPrintf(nullptr, 0, pFormat, argsForLength);

        va_list argsForText = args;
        std::basic_string<TChar> result;
        result.resize(length);
        CFunctions::StringPrintf(result.data(), result.size(), pFormat, argsForText);

        return result;
    }

    template<typename TChar>
    static std::basic_string<TChar> Format(const TChar* pFormat, ...)
    {
        va_list args;
        va_start(args, pFormat);
        return Format(pFormat, args);
    }

    template<typename TChar>
    static std::basic_string<TChar> ToLower(const std::basic_string<TChar>& str)
    {
        std::basic_string<TChar> result;
        result.resize(str.size());
        std::ranges::transform(str, result.begin(), CFunctions::ToLower<TChar>);
        return result;
    }

    template<typename TChar>
    static std::basic_string<TChar> ToUpper(const std::basic_string<TChar>& str)
    {
        std::basic_string<TChar> result;
        result.resize(str.size());
        std::ranges::transform(str, result.begin(), CFunctions::ToUpper<TChar>);
        return result;
    }

    template<typename TChar>
    static std::vector<std::basic_string<TChar>> Split(const std::basic_string<TChar>& str, const std::basic_string<TChar>& delimiter)
    {
        std::vector<std::basic_string<TChar>> result;
        int start = 0;
        while (start < str.size())
        {
            int end = str.find(delimiter.c_str(), start);
            if (end < 0)
                end = str.size();

            result.push_back(str.substr(start, end - start));
            start = end + delimiter.size();
        }
        return result;
    }

    template<typename TChar>
    static std::basic_string<TChar> Join(const std::vector<std::basic_string<TChar>>& elements, const std::basic_string<TChar>& delimiter)
    {
        std::basic_string<TChar> result;
        for (int i = 0; i < elements.size(); i++)
        {
            if (i > 0)
                result += delimiter;

            result += elements[i];
        }
        return result;
    }

    template<typename TChar>
    static std::basic_string<TChar> Replace(const std::basic_string<TChar>& str, TChar oldChar, TChar newChar)
    {
        std::basic_string<TChar> result(str);
        std::ranges::replace(result, oldChar, newChar);
        return result;
    }
};
