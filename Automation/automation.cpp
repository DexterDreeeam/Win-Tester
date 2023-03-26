#include "automation.hpp"
#include "Msaa/automation_msaa.hpp"

std::shared_ptr<Automation> Automation::Instance(ImplementType ty)
{
    if (ty == Msaa)
    {
        return std::make_shared<MsaaAutomation>();
    }

    return nullptr;
}
