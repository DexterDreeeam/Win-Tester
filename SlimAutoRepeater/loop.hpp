#pragma once

#include "common.hpp"
#include "platform/platform.hpp"

enum GlobalState
{
    IDLE,

    IDLE_TO_RUNNING,
    RUNNING_TO_IDLE,
    RUNNING,

    IDLE_TO_RECORDING,
    RECORDING_TO_IDLE,
    RECORDING,
};

struct GlobalInfo
{
    static GlobalInfo* I()
    {
        static GlobalInfo* _ins = nullptr;
        return _ins ? _ins : (_ins = new GlobalInfo());
    }

    GlobalInfo() :
        state(GlobalState::IDLE),
        highlight_frame(false),
        capture_desktop(false),
        console_window(false),
        app_to_launch(""),
        point(),
        chain(nullptr)
    {
    }

    bool Change(GlobalState from, GlobalState to)
    {
        if (InterlockedCompareExchange((LONG*)&state, (LONG)to, (LONG)from) == (LONG)from)
        {
            return true;
        }
        return false;
    }

    static const vector<string> Consoles()
    {
        return slim::platform::I()->Consoles();
    }

    static void ClearConsole()
    {
        slim::platform::I()->ClearConsole();
    }

    volatile GlobalState state;
    bool highlight_frame;
    bool capture_desktop;
    bool console_window;

    char app_to_launch[256];

    slim::point point;
    shared_ptr<slim::element_chain> chain;
};

bool SlimCursorUpdate();
bool SlimLoop(bool draw = true);
bool SlimLoopTrigger();
