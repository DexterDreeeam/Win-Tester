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
    const char* default_wait_for_ms_str = "1000";
    const int   default_wait_for_ms = 1000;

    static GlobalInfo* I()
    {
        static GlobalInfo* _ins = nullptr;
        return _ins ? _ins : (_ins = new GlobalInfo());
    }

    GlobalInfo() :
        state(GlobalState::IDLE),
        exiting(false),
        fast_mode(false),
        highlight_frame(false),
        capture_desktop(false),
        record_finished(false),
        record_optional(false),
        record_loop_stack(0),
        console_window(false),
        action_wait_for_ms(""),
        app_to_launch(""),
        record_loop_pars(""),
        point(),
        chain(nullptr)
    {
        ResetWaitMs();
    }

    int ResetWaitMs()
    {
        memset(action_wait_for_ms, 0, 256);
        memcpy_s(action_wait_for_ms, 256, default_wait_for_ms_str, strlen(default_wait_for_ms_str));
        return default_wait_for_ms;
    }

    bool Change(GlobalState from, GlobalState to)
    {
        if (InterlockedCompareExchange((LONG*)&state, (LONG)to, (LONG)from) == (LONG)from)
        {
            return true;
        }
        return false;
    }

    volatile GlobalState state;
    bool exiting;
    bool fast_mode;
    bool highlight_frame;
    bool capture_desktop;
    int  record_loop_stack;
    bool record_optional;
    bool record_finished;
    bool console_window;

    char action_wait_for_ms[256];
    char app_to_launch[256];
    char record_loop_pars[256];

    slim::point point;
    shared_ptr<slim::element_chain> chain;
};

bool SlimCursorUpdate();
bool SlimLoop(bool draw = true);
bool SlimLoopTrigger(bool end_sync = false);
