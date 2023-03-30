#include "action.hpp"
#include "element_chain.hpp"
#include "element.hpp"
#include "identifier.hpp"

namespace slim
{

action::action(
    action_type ty, shared_ptr<element_chain> ec, int wait) :
    type(ty),
    parameter(),
    wnd_info(ec->_wnd_info),
    element_stacks(),
    wait_time_ms(wait)
{
    for (auto e : ec->_ve)
    {
        element_stacks.push_back(element_stack(e));
    }
}

}
