#include "identifier.hpp"
#include "element.hpp"

namespace slim
{

element_stack::element_stack(shared_ptr<element> e) :
    sub_index(e->SubIdx()),
    control_type(atoi(e->Property("ControlType").c_str())),
    element_name(e->Property("Name")),
    automation_id(e->Property("AutomationId"))
{
}

}
