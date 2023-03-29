#include "runner.hpp"
#include "platform/platform.hpp"
#include "platform/action.hpp"

namespace slim
{

bool runner::Run(const string& str)
{
    Sleep(1000);
    auto as = action_set::FromString(str);
    for (auto ac : as._va)
    {
        Sleep(ac->wait_time_ms);
        if (!Act(ac) && !ActElement(ac))
        {
            return false;
        }
    }
    return true;
}

bool runner::Act(shared_ptr<action> ac)
{
    switch (ac->type)
    {
    case action_type::APP_LAUNCH:
        return false;

    case action_type::KEY_INPUT:
        if (ac->parameter["keys"].size() < 1)
        {
            return false;
        }
        for (auto c : ac->parameter["keys"])
        {
            if (!_KeyInput(c))
            {
                return false;
            }
        }
        return true;

    default:
        return false;
    }
}

bool runner::ActElement(shared_ptr<action> ac)
{
    HWND best_wnd = nullptr;
    shared_ptr<element> best_elm;
    double best_score = 0.1;

    auto wnds = _FilterWindow(ac);
    for (auto wnd : wnds)
    {
        auto root = platform::I()->Diagram(wnd);
        vector<pair<shared_ptr<element>, double>> ranks;
        root->Matching(ac->element_stacks, (int)ac->element_stacks.size() - 1, root, 0, ranks);

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
        if (score_max > best_score)
        {
            best_wnd = wnd;
            best_elm = ranks[idx_max].first;
            best_score = score_max;
        }
    }

    if (!best_wnd)
    {
        return false;
    }

    if (GetForegroundWindow() != best_wnd && !SetForegroundWindow(best_wnd))
    {
        return false;
    }
    Sleep(200);
    return best_elm->Act(ac->type);

    return false;
}

struct _EnumWindowsStruct
{
    vector<HWND> targets;
    shared_ptr<action> act = nullptr;
};

vector<HWND> runner::_FilterWindow(shared_ptr<action> ac)
{
    _EnumWindowsStruct st;
    st.targets = vector<HWND>();
    st.act = ac;

    EnumWindows(runner::_EnumWindowsCb, (LPARAM)&st);
    return st.targets;
}

BOOL CALLBACK runner::_EnumWindowsCb(HWND hWnd, LPARAM lParam)
{
    _EnumWindowsStruct* st = (_EnumWindowsStruct*)lParam;
    if (_IsTargetWindow(hWnd, st->act))
    {
        st->targets.push_back(hWnd);
    }
    return TRUE; // continue
}

bool runner::_IsTargetWindow(HWND wnd, shared_ptr<action> ac)
{
    auto pName = platform::I()->WndClassName(wnd);
    if (pName.second == ac->class_name) // class name
    {
        return true;
    }
    return false;
}

bool runner::_KeyInput(char c)
{
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = MapVirtualKey(c, MAPVK_VK_TO_VSC);
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;
    input.ki.wVk = c;
    input.ki.dwFlags = 0;
    if (SendInput(1, &input, sizeof(INPUT)) == 0)
    {
        return false;
    }

    Sleep(20);

    input.ki.dwFlags = KEYEVENTF_KEYUP;
    if (SendInput(1, &input, sizeof(INPUT)) == 0)
    {
        return false;
    }

    Sleep(20);

    return true;
}

}
