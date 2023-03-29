#pragma once

#include "common.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace slim
{

enum action_type
{
    ACTION_NONE,

    ACTION_TEST,

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

class element;

struct element_stack
{
    int sub_index;
    int control_type;
    string element_name;
    string automation_id;

    element_stack() :
        sub_index(),
        control_type(),
        element_name(),
        automation_id()
    {
    }

    element_stack(shared_ptr<element> e);

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
