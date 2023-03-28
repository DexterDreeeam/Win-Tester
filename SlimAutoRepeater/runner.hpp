#pragma once

#include "common.hpp"

namespace slim
{

class action;

class runner
{
public:
    runner()
    {
    }

    ~runner()
    {
    }

    bool Run(const string& str);

    bool Run(const action& ac);

private:
    vector<HWND> _FilterWindow(const action& ac);

    static BOOL CALLBACK _EnumWindowsCb(HWND hWnd, LPARAM lParam);

    static bool _IsTargetWindow(HWND wnd, const action& ac);
};

}
