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

inline int LCSS(const string& s1, const string& s2)
{
    if (s1.size() == 0 || s2.size() == 0)
    {
        return 0;
    }

    int rst = 0;
    int len1 = s1.size() < 32 ? (int)s1.size() : 32;
    int len2 = s2.size() < 32 ? (int)s2.size() : 32;
    short dp[32][32] = {};
    for (int i = 0; i < len1; ++i)
    {
        if (s1[i] == s2[0])
        {
            dp[i][0] = 1;
            rst = 1;
        }
    }
    for (int j = 0; j < len2; ++j)
    {
        if (s1[0] == s2[j])
        {
            dp[0][j] = 1;
            rst = 1;
        }
    }
    for (int i = 1; i < len1; ++i)
    {
        for (int j = 1; j < len2; ++j)
        {
            if (s1[i] == s2[j])
            {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                rst = dp[i][j] > rst ? dp[i][j] : rst;
            }
        }
    }
    return rst;
}
