#include "platform.hpp"

namespace slim
{

shared_ptr<element> platform::Diagram()
{
    HRESULT hr;
    IUIAutomationElement* root = nullptr;

    hr = ctx->ElementFromHandle(GetForegroundWindow(), &root);
    Tell("Element From Handle of GetForegroundWindow() failed");

    auto ret = make_shared<element>(_ins, root);
    ret->LoadSub();

    return ret;
}

}
