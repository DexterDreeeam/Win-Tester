#pragma once

inline std::wstring ToString(BSTR bstr)
{
    return std::wstring(bstr, SysStringLen(bstr));
}

inline std::string WtoA(const std::wstring& ws, bool removeFeedline = true)
{
    std::string str = "";
    for (auto c : ws)
    {
        if (removeFeedline && char(c) == '\n')
        {
            str += ' ';
        }
        else
        {
            str += char(c);
        }
    }
    return str;
}
