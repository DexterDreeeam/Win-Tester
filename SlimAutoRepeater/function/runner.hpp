#pragma once

#include "common.hpp"

namespace slim
{

class action;

class runner
{
public:
    runner() :
        mtx()
    {
    }

    ~runner()
    {
    }

    bool Run(const string& str);

    bool Act(shared_ptr<action> ac);

    bool ActElement(shared_ptr<action> ac);

private:
    vector<HWND> _FilterWindow(shared_ptr<action> ac);

    static BOOL CALLBACK _EnumWindowsCb(HWND hWnd, LPARAM lParam);

    static bool _IsTargetWindow(HWND wnd, shared_ptr<action> ac);

    static bool _KeyInput(char c);

private:
    mutex mtx;
};

}
