#pragma once

#include "common.hpp"

namespace slim
{

enum action_type
{
    // elemnt_id required
    LEFT_CLICK,

    // elemnt_id required
    RIGHT_CLICK,

    // parameter required
    KEY_INPUT,

    // parameter required
    WINDOW_SWITCH,
};

struct action
{
    action_type  type;

    string       element_id;  // optional
    string       parameter;   // optional
};

}
