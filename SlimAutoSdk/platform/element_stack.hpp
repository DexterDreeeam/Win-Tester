#pragma once

#include "common.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace slim
{

class element;

class element_stack : public xref<element_stack>
{
public:
    int parent_index;
    int control_type;
    string element_name;
    string automation_id;

    element_stack() :
        parent_index(),
        control_type(),
        element_name(),
        automation_id()
    {
    }

    element_stack(shared_ptr<element> e);

    string ToString() const
    {
        string s = "";
        s += "[" + to_string(parent_index) + "]";
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
        es.parent_index = atoi(s.substr(f + 1, t - (f + 1)).c_str());

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
