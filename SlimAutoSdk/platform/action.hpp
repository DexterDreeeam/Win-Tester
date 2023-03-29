#pragma once

#include "common.hpp"
#include "json.hpp"
#include "identifier.hpp"

using json = nlohmann::json;

namespace slim
{

class element_chain;

class action
{
public:
    action_type type;
    map<string, string> parameter;
    string window_name;
    string class_name;
    vector<element_stack> element_stacks;
    int wait_time_ms;

    action() :
        type(action_type::ACTION_NONE),
        parameter(),
        window_name(),
        class_name(),
        element_stacks(),
        wait_time_ms(1000)
    {
    }

    action(action_type ty, shared_ptr<element_chain> ec, int wait = 1000);

    void AddParameter(const string& key, const string& val)
    {
        parameter[key] = val;
    }

    void SetTimeMs(int ms)
    {
        wait_time_ms = ms;
    }

    static string TypeToString(action_type t)
    {
        for (auto p : action_string_dict)
        {
            if (p.first == t)
            {
                return p.second;
            }
        }
        return "ACTION_NONE";
    }

    static action_type StringToType(const string& str)
    {
        for (auto p : action_string_dict)
        {
            if (p.second == str)
            {
                return p.first;
            }
        }
        return ACTION_NONE;
    }

    json ToJson() const
    {
        auto j = json::object();
        j["window_name"] = window_name;
        j["class_name"] = class_name;
        j["action_type"] = TypeToString(type);
        j["parameter"] = parameter;

        auto ja = json::array();
        for (const auto& s : element_stacks)
        {
            ja.push_back(s.ToString());
        }
        j["element_stacks"] = ja;

        return j;
    }

    static shared_ptr<action> FromJson(const json& j)
    {
        auto ac = make_shared<action>();
        ac->window_name = j["window_name"];
        ac->class_name = j["class_name"];
        ac->type = StringToType(j["action_type"]);
        ac->parameter = j["parameter"];

        auto es = j["element_stacks"];
        for (int i = 0; i < es.size(); ++i)
        {
            ac->element_stacks.push_back(element_stack::FromString(es[i]));
        }

        return ac;
    }

    static shared_ptr<action> FromString(const string& str)
    {
        return FromJson(json::parse(str));
    }
};

struct action_set
{
    vector<shared_ptr<action>> _va;

    void Add(shared_ptr<action> ac)
    {
        _va.push_back(ac);
    }

    void Sort()
    {
        vector<shared_ptr<action>> va;
        bool last_is_input = false;
        for (auto ac : _va)
        {
            if (last_is_input && ac->type == action_type::KEY_INPUT)
            {
                va.back()->parameter["keys"] += ac->parameter["keys"];
            }
            else
            {
                va.push_back(ac);
                last_is_input = ac->type == action_type::KEY_INPUT;
            }
        }
        _va = va;
    }

    string ToString(bool indent) const
    {
        return ToJson().dump(indent ? 2 : -1);
    }

    json ToJson() const
    {
        auto jArr = json::array();
        for (auto ac : _va)
        {
            jArr.push_back(ac->ToJson());
        }
        return jArr;
    }

    static action_set FromJson(const json& obj)
    {
        action_set as;
        for (int i = 0; i < obj.size(); ++i)
        {
            as._va.push_back(action::FromJson(obj[i]));
        }
        return as;
    }

    static action_set FromString(const string& str)
    {
        return FromJson(json::parse(str));
    }
};

}
