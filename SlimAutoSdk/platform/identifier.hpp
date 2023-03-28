#pragma once

#include "common.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace slim
{

enum action_type
{
    ACTION_NONE,

    // elemnt_id required
    LEFT_CLICK,

    // elemnt_id required
    RIGHT_CLICK,

    // parameter required
    KEY_INPUT,

    // parameter required
    WINDOW_SWITCH,
};

static vector<pair<action_type, string>> action_string_dict =
{
    { ACTION_NONE,   "ACTION_NONE" },
    { LEFT_CLICK,    "LEFT_CLICK" },
    { RIGHT_CLICK,   "RIGHT_CLICK" },
    { KEY_INPUT,     "KEY_INPUT" },
    { WINDOW_SWITCH, "WINDOW_SWITCH" },
};

struct element_stack
{
    int sub_index;
    int control_type;
    string element_name;
    string automation_id;

    string ToString() const
    {
        string s = "";
        s += "[" + to_string(sub_index) + "]";
        s += "(" + to_string(control_type) + ")";
        s += "{{" + element_name + "}}";
        s += "<<" + automation_id + ">>";
        return s;
    }

    static element_stack FromString(const string& s)
    {
        element_stack es;
        size_t f, t;

        f = 0;
        t = s.find(']');
        es.sub_index = atoi(s.substr(f + 1, t - (f + 1)).c_str());

        f = t + 1;
        t = s.find(')', f);
        es.control_type = atoi(s.substr(f + 1, t - (f + 1)).c_str());

        f = t + 1;
        t = s.find("}}", f);
        es.element_name = s.substr(f + 2, t - (f + 2));

        f = t + 2;
        t = s.find(">>", f);
        es.automation_id = s.substr(f + 2, t - (f + 2));

        return es;
    }
};

}
