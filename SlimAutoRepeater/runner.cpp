#include "runner.hpp"
#include "platform/platform.hpp"
#include "platform/action.hpp"

namespace slim
{

bool runner::Run(const string& str)
{
    Sleep(2000);
    auto as = action_set::FromString(str);
    for (auto& ac : as._va)
    {
        if (!Run(ac))
        {
            return false;
        }
        Sleep(1000);
    }
    return true;
}

bool runner::Run(const action& ac)
{
    auto wnds = _FilterWindow(ac);
    for (auto wnd : wnds)
    {
        auto root = platform::I()->Diagram(wnd);
        vector<pair<shared_ptr<element>, double>> ranks;
        for (int i = 0; i < root->SubCount(); ++i)
        {
            auto s = root->Sub(i);
            s->Matching(ac.element_stacks, ac.element_stacks.size() - 2, s, 0, ranks);
        }

        double score_max = -1.0;
        int idx_max = -1;
        for (int i = 0; i < ranks.size(); ++i)
        {
            if (ranks[i].second > score_max)
            {
                score_max = ranks[i].second;
                idx_max = i;
            }
        }
        if (idx_max == -1)
        {
            continue;
        }

        if (GetForegroundWindow() != wnd && !SetForegroundWindow(wnd))
        {
            continue;
        }
        Sleep(200);

        auto elm = ranks[idx_max].first; // found correct element
        switch (ac.type)
        {
        case action_type::LEFT_CLICK:
            return elm->Envoke();

        case action_type::RIGHT_CLICK:
            return elm->Menu();

        default:
            break;
        }
    }

    return false;
}

struct _EnumWindowsStruct
{
    vector<HWND> targets;
    const action* act = nullptr;
};

vector<HWND> runner::_FilterWindow(const action& ac)
{
    _EnumWindowsStruct st;
    st.targets = vector<HWND>();
    st.act = &ac;

    EnumWindows(runner::_EnumWindowsCb, (LPARAM)&st);
    return st.targets;
}

BOOL CALLBACK runner::_EnumWindowsCb(HWND hWnd, LPARAM lParam)
{
    _EnumWindowsStruct* st = (_EnumWindowsStruct*)lParam;
    if (_IsTargetWindow(hWnd, *st->act))
    {
        st->targets.push_back(hWnd);
    }
    return TRUE; // continue
}

bool runner::_IsTargetWindow(HWND wnd, const action& ac)
{
    auto pName = platform::I()->WndClassName(wnd);
    if (pName.second == ac.class_name) // class name
    {
        return true;
    }
    return false;
}

}
