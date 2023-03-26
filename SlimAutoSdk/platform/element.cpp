#include "platform.hpp"
#include "element.hpp"

namespace slim
{

element::element(shared_ptr<platform> p, int sub_idx, int depth, IUIAutomationElement* e) :
    _plt(p),
    _sub_idx(sub_idx),
    _depth(depth),
    _elm(e),
    _subs(),
    _area()
{
    HRESULT hr;
    RECT rc;

    hr = _elm->get_CurrentBoundingRectangle(&rc);
    Tell("elm->get_CurrentBoundingRectangle failed");

    _area = area(rc);
}

bool element::LoadSub(int depth, int depth_remain)
{
    if (Invalid())
    {
        return false;
    }

    HRESULT hr;
    IUIAutomationElementArray* elmArr;

    hr = _elm->FindAll(TreeScope_Children, _plt->con, &elmArr);
    Tell("FindAll() failed when LoadSub()");

    int len = 0;
    hr = elmArr->get_Length(&len);
    Tell("get_Length failed");

    for (int i = 0; i < len; ++i)
    {
        IUIAutomationElement* c;
        hr = elmArr->GetElement(i, &c);
        Tell("elmArr->GetElement failed");
        _subs.push_back(make_shared<element>(_plt, i, depth, c));
        if (depth_remain == 0)
        {
            continue;
        }
        _subs.back()->LoadSub(depth + 1, depth_remain == -1 ? -1 : depth_remain - 1);
    }
    return true;
}

bool element::LoadProperty()
{
    if (Invalid())
    {
        return false;
    }

    HRESULT hr;
    BSTR bstr;
    VARIANT var;

    hr = _elm->get_CurrentName(&bstr);
    Tell("_elm->get_CurrentName failed");
    _property["Name"] = ToString(bstr);

    hr = _elm->get_CurrentAutomationId(&bstr);
    Tell("_elm->get_CurrentAutomationId failed");
    _property["AutomationId"] = ToString(bstr);

    hr = _elm->get_CurrentAccessKey(&bstr);
    Tell("_elm->get_CurrentAccessKey failed");
    _property["AccessKey"] = ToString(bstr);

    hr = _elm->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &var);
    Tell("_elm->GetCurrentPropertyValue failed");
    _property["Value"] = ToString(var.bstrVal);

    return true;
}

}
