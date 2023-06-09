#pragma once

#include "common.hpp"

namespace slim
{

struct WndInfo
{
    string cls;
    string win;
    HWND   wnd;

    bool IsGlobal() const
    {
        return cls.size() == 0 && win.size() == 0;
    }
};

enum action_type
{
    ACTION_NONE,

    ACTION_TEST,

    // parameter required
    LOOP_BEGIN,

    LOOP_END,

    // parameter required
    APP_LAUNCH_PATH,

    // parameter required
    APP_LAUNCH_APPID,

    // elemnt_id required
    HOVER,

    // elemnt_id required
    LEFT_CLICK,

    // elemnt_id required
    RIGHT_CLICK,

    // parameter required
    KEY_INPUT,
};

static vector<pair<action_type, string>> action_string_dict =
{
    { action_type::ACTION_NONE,      "ACTION_NONE" },
    { action_type::ACTION_TEST,      "ACTION_TEST" },
    { action_type::LOOP_BEGIN,       "LOOP_BEGIN" },
    { action_type::LOOP_END,         "LOOP_END" },
    { action_type::APP_LAUNCH_PATH,  "APP_LAUNCH" },
    { action_type::APP_LAUNCH_APPID, "APP_LAUNCH_APPID" },
    { action_type::HOVER,            "HOVER" },
    { action_type::LEFT_CLICK,       "LEFT_CLICK" },
    { action_type::RIGHT_CLICK,      "RIGHT_CLICK" },
    { action_type::KEY_INPUT,        "KEY_INPUT" },
};

}
