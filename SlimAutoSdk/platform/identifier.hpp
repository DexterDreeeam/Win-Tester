#pragma once

#include "common.hpp"

namespace slim
{

struct WndInfo
{
    string cls;
    string win;
    HWND   wnd;
};

enum action_type
{
    ACTION_NONE,

    ACTION_TEST,

    // parameter required
    LOOP_START,

    LOOP_END,

    // parameter required
    APP_LAUNCH,

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
    { action_type::ACTION_NONE,   "ACTION_NONE" },
    { action_type::ACTION_TEST,   "ACTION_TEST" },
    { action_type::APP_LAUNCH,    "APP_LAUNCH" },
    { action_type::HOVER,         "HOVER" },
    { action_type::LEFT_CLICK,    "LEFT_CLICK" },
    { action_type::RIGHT_CLICK,   "RIGHT_CLICK" },
    { action_type::KEY_INPUT,     "KEY_INPUT" },
};

}
