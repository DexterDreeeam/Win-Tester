#include "element_stack.hpp"
#include "element.hpp"

namespace slim
{

element_stack::element_stack(shared_ptr<element> e) :
    parent_index(e->_parent_idx),
    control_type(e->_control),
    element_name(e->_name),
    automation_id(e->_auto_id)
{
}

}
