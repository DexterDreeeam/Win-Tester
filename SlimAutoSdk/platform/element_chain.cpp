#include "element_chain.hpp"
#include "element.hpp"
#include "action.hpp"

namespace slim
{

// WindowName@@ClassName  <13>      [2]             (3000)                 {ClickHere}      [ ...
//                        \_layers  \_childest idx  \_childest control id  \_childest name  \_upper element
string element_chain::Identifier()
{
    if (!Valid())
    {
        return "";
    }
    string id = "";
    id += _window + "@@" + _class;
    id += "[[" + to_string(_ve.size()) + "]]";
    for (auto e : _ve)
    {
        e->LoadProperty();
        id += e->Identifier();
    }
    return id;
}

string element_chain::FriendlyIdentifier()
{
    if (!Valid())
    {
        return "";
    }
    string id = "";
    id += _window + "\n@@\n" + _class + '\n';
    id += "[[" + to_string(_ve.size()) + "]]";
    id += '\n';
    for (auto e : _ve)
    {
        e->LoadProperty();
        id += e->FriendlyIdentifier();
        id += '\n';
    }
    return id;
}

shared_ptr<action> element_chain::Hover()
{
    if (!_ve.front()->Hover())
    {
        return nullptr;
    }
    auto ac = make_shared<action>();
    ac->type = action_type::HOVER;
    ac->class_name = _class;
    ac->window_name = _window;
    for (auto e : _ve)
    {
        ac->element_stacks.push_back(element_stack(e));
    }
    return ac;
}

shared_ptr<action> element_chain::Envoke()
{
    if (!_ve.front()->Envoke())
    {
        return nullptr;
    }
    auto ac = make_shared<action>();
    ac->type = action_type::LEFT_CLICK;
    ac->class_name = _class;
    ac->window_name = _window;
    for (auto e : _ve)
    {
        ac->element_stacks.push_back(element_stack(e));
    }
    return ac;
}

shared_ptr<action> element_chain::Menu()
{
    if (!_ve.front()->Menu())
    {
        return nullptr;
    }
    auto ac = make_shared<action>();
    ac->type = action_type::RIGHT_CLICK;
    ac->class_name = _class;
    ac->window_name = _window;
    for (auto e : _ve)
    {
        ac->element_stacks.push_back(element_stack(e));
    }
    return ac;
}

shared_ptr<action> element_chain::Input(char k)
{
    auto ac = make_shared<action>();
    ac->type = action_type::KEY_INPUT;
    ac->class_name = _class;
    ac->window_name = _window;
    ac->AddParameter("keys", string({ k }));
    return ac;
}

shared_ptr<action> element_chain::Test()
{
    if (!_ve.front()->Test())
    {
        return nullptr;
    }
    auto ac = make_shared<action>();
    ac->type = action_type::ACTION_TEST;
    ac->class_name = _class;
    ac->window_name = _window;
    for (auto e : _ve)
    {
        ac->element_stacks.push_back(element_stack(e));
    }
    return ac;
}

}