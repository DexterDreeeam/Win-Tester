#include "element_chain.hpp"
#include "element_stack.hpp"
#include "element.hpp"
#include "action.hpp"

namespace slim
{

// WindowName@@ClassName  <13>      [2]             (3000)                 {ClickHere}      [ ...
//                        \_layers  \_childest idx  \_childest control id  \_childest name  \_upper element
string element_chain::Identifier() const
{
    if (!Valid())
    {
        return "";
    }
    string id = "";
    id += _wnd_info.win + "@@" + _wnd_info.cls;
    id += "[[" + to_string(_ve.size()) + "]]";
    for (auto e : _ve)
    {
        id += e->Identifier();
    }
    return id;
}

area element_chain::Area() const
{
    if (_ve.size())
    {
        return _ve.front()->_area;
    }
    return area();
}

string element_chain::FriendlyIdentifier() const
{
    if (!Valid())
    {
        return "";
    }
    string id = "";
    id += _wnd_info.win + "\n@@\n" + _wnd_info.cls + '\n';
    id += "[[" + to_string(_ve.size()) + "]]";
    id += '\n';
    for (auto e : _ve)
    {
        id += e->FriendlyIdentifier();
        id += '\n';
    }
    return id;
}

shared_ptr<action> element_chain::Hover(int wait)
{
    if (!_ve.front()->Hover())
    {
        return nullptr;
    }
    auto ac = xref<action>::x(wait);
    ac->type = action_type::HOVER;
    ac->wnd_info = _wnd_info;
    for (auto e : _ve)
    {
        ac->element_stacks.push_back(element_stack(e));
    }
    return ac;
}

shared_ptr<action> element_chain::Envoke(int wait)
{
    if (!_ve.front()->Envoke())
    {
        return nullptr;
    }
    auto ac = xref<action>::x(wait);
    ac->type = action_type::LEFT_CLICK;
    ac->wnd_info = _wnd_info;
    for (auto e : _ve)
    {
        ac->element_stacks.push_back(element_stack(e));
    }
    return ac;
}

shared_ptr<action> element_chain::Menu(int wait)
{
    if (!_ve.front()->Menu())
    {
        return nullptr;
    }
    auto ac = xref<action>::x(wait);
    ac->type = action_type::RIGHT_CLICK;
    ac->wnd_info = _wnd_info;
    for (auto e : _ve)
    {
        ac->element_stacks.push_back(element_stack(e));
    }
    return ac;
}

shared_ptr<action> element_chain::Input(char k, int wait)
{
    auto ac = xref<action>::x(wait);
    ac->type = action_type::KEY_INPUT;
    ac->wnd_info = _wnd_info;
    ac->AddParameter("keys", string({ k }));
    return ac;
}

shared_ptr<action> element_chain::Test(int wait)
{
    if (!_ve.front()->Test())
    {
        return nullptr;
    }
    auto ac = xref<action>::x(wait);
    ac->type = action_type::ACTION_TEST;
    ac->wnd_info = _wnd_info;
    for (auto e : _ve)
    {
        ac->element_stacks.push_back(element_stack(e));
    }
    return ac;
}

}