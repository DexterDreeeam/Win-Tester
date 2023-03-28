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
    string window_name;
    string class_name;
    action_type type;
    string parameter;
    vector<element_stack> element_stacks;

    action() :
        window_name(),
        class_name(),
        type(action_type::ACTION_NONE),
        parameter(),
        element_stacks()
    {
    }

    action(action_type ty, shared_ptr<element_chain> ec, const string& par = "");

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

    static action FromJson(const json& j)
    {
        action ac;
        ac.window_name = j["window_name"];
        ac.class_name = j["class_name"];
        ac.type = StringToType(j["action_type"]);
        ac.parameter = j["parameter"];

        auto es = j["element_stacks"];
        for (int i = 0; i < es.size(); ++i)
        {
            ac.element_stacks.push_back(element_stack::FromString(es[i]));
        }

        return ac;
    }

    static action FromString(const string& str)
    {
        return FromJson(json::parse(str));
    }
};

struct action_set
{
    vector<action> _va;

    void Add(const action& ac)
    {
        _va.push_back(ac);
    }

    string ToString() const
    {
        return ToJson().dump();
    }

    json ToJson() const
    {
        auto jArr = json::array();
        for (const auto& ac : _va)
        {
            jArr.push_back(ac.ToJson());
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
