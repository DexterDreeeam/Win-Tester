#include "action.hpp"
#include "element_chain.hpp"
#include "element.hpp"
#include "identifier.hpp"

namespace slim
{

action::action(action_type ty, shared_ptr<element_chain> ec, const string& par) :
    window_name(ec->_window),
    class_name(ec->_class),
    type(ty),
    parameter(par),
    element_stacks()
{
    for (auto e : ec->_ve)
    {
        element_stacks.push_back(element_stack::FromString(e->Identifier()));
    }
}

}
