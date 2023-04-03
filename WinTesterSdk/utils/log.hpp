
class logger
{
public:
    logger(const string& file = "") :
        f()
    {
        if (file != "")
        {
            f.open(file, ios::app || ios::binary);
        }
    }

    ~logger()
    {
        if (f.is_open())
        {
            f.close();
        }
    }

    void w(const string& line)
    {
        string l = "";
        for (auto c : line)
        {
            l += c == '\n' ? ' ' : c;
        }
        if (f.is_open())
        {
            f << l;
        }
        else
        {
            cout << l;
        }
    }

    void w_endl()
    {
        if (f.is_open())
        {
            f << endl;
        }
        else
        {
            cout << endl;
        }
    }

private:
    string utf8(const wstring& wstr)
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

private:
    ofstream f;
};
