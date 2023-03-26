#include "common.hpp"
#include "utils.hpp"
#include "automation.hpp"

int main()
{
    // ShowWindow(GetConsoleWindow(), SW_HIDE);
    if (FAILED(CoInitialize(NULL)))
    {
        return 1;
    }

    std::wstring lastPath;
    ElementRect lastRect = ElementRect();

    ScreenPainter().InitWindow();

    while (1)
    {
        Sleep(10);

        try
        {
            auto ato = Automation::Instance(Msaa);
            auto ele = ato->CursorElement();
            if (!ele)
            {
                continue;
            }

            auto path = ele->Path();
            auto rect = ele->Rect();

            //if (path == lastPath && rect == lastRect)
            //{
            //    continue;
            //}

            std::vector<std::wstring> vstr;
            std::wstring str = L"";

            str += L"[";
            str += std::to_wstring(rect.left);
            str += L"] - [";
            str += std::to_wstring(rect.top);
            str += L"] - [";
            str += std::to_wstring(rect.right);
            str += L"] - [";
            str += std::to_wstring(rect.bottom);
            str += L"]";
            vstr.push_back(str);

            size_t pos = 3;
            std::wstring prefix = L"";
            while (1)
            {
                auto found = path.find(L"---", pos);
                if (found == std::wstring::npos)
                {
                    vstr.push_back(prefix + L"\\_" + path.substr(pos));
                    break;
                }
                vstr.push_back(prefix + L"\\_" + path.substr(pos, found - pos));
                prefix += L"  ";
                pos = found + 3;
            }

            ScreenPainter().Rect(rect.left, rect.top, rect.right, rect.bottom);
            ScreenPainter().LoopWindow(vstr);

            //for (int i = 0; i < 100; ++i)
            //{
            //    std::wcout << std::endl;
            //}
            //std::wcout << L"============" <<  std::endl;
            //std::wcout << "[" << rect.left << "] - [" << rect.top << "] - [" << rect.right << "] - [" << rect.bottom << "]" << std::endl;
            //std::wcout << L"============" << std::endl;
            //std::wcout << path << std::endl;
            //std::wcout << L"============" << std::endl;

            lastPath = path;
            lastRect = rect;
        }
        catch (...)
        {
            std::cout << "exception !" << std::endl;
        }
    }

    system("pause");
    return 0;
}
