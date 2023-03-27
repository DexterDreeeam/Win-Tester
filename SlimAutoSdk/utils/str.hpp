#pragma once

inline string Utf8(const wstring& wstr)
{
    const wchar_t* ch = wstr.c_str();
    int len = (int)wstr.size();

    int nChars = WideCharToMultiByte(CP_UTF8, 0, ch, len, NULL, 0, NULL, NULL);
    if (nChars == 0)
    {
        return "";
    }

    std::string str;
    str.resize(nChars);
    WideCharToMultiByte(CP_UTF8, 0, ch, len, const_cast<char*>(str.c_str()), nChars, NULL, NULL);
    return str;
}

inline std::string ToString(BSTR bstr)
{
    return Utf8(std::wstring(bstr, SysStringLen(bstr)));
}
