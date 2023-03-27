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
    _area(),
    _property(),
    _property_loaded(false),
    _interact(false)
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

    hr = _elm->FindAll(TreeScope_Children, _plt->_con, &elmArr);
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

    if (_property_loaded)
    {
        return true;
    }

    HRESULT hr;
    BSTR bstr;
    //VARIANT var;
    CONTROLTYPEID ct;

    hr = _elm->get_CurrentName(&bstr);
    Tell("_elm->get_CurrentName failed");
    _property["Name"] = ToString(bstr);

    hr = _elm->get_CurrentAutomationId(&bstr);
    Tell("_elm->get_CurrentAutomationId failed");
    _property["AutomationId"] = ToString(bstr);

    //hr = _elm->get_CurrentAccessKey(&bstr);
    //Tell("_elm->get_CurrentAccessKey failed");
    //_property["AccessKey"] = ToString(bstr);

    //hr = _elm->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &var);
    //Tell("_elm->GetCurrentPropertyValue failed");
    //_property["Value"] = ToString(var.bstrVal);

    hr = _elm->get_CurrentControlType(&ct);
    Tell("_elm->get_CurrentControlType failed");
    _property["ControlType"] = to_string(ct);
    _interact = interact_ct.count(ct) > 0;

    hr = _elm->get_CurrentLocalizedControlType(&bstr);
    Tell("_elm->get_CurrentLocalizedControlType failed");
    _property["LocalizedControlType"] = ToString(bstr);

    _property_loaded = true;
    return true;
}

double element::InteractGrade(point p)
{
    if (!Area().Inside(p))
    {
        return 0;
    }

    if (!_property_loaded && !LoadProperty())
    {
        return 0;
    }

    if (!Interact())
    {
        return 0;
    }

    double areaSz = (double)Area().size();
    if (areaSz <= 0)
    {
        return 0;
    }

    double g = double(Depth() + 8);
    g = g * g;
    return g / areaSz;
}

string element::Identifier()
{
    if (!LoadProperty())
    {
        return "";
    }

    string id = "";
    id += "[" + to_string(SubIdx()) + "]";
    id += "(" + Property("ControlType") + ")";
    id += "{" + Property("Name") + "}";
    return id;
}

string element::FriendlyIdentifier()
{
    if (!LoadProperty())
    {
        return "";
    }

    string id = "";
    id += "[" + to_string(SubIdx()) + "]";
    id += "(" + Property("LocalizedControlType") + ")";
    id += "{" + Property("Name") + "}";
    id += " " + Area().stringify();
    return id;
}

bool element::test()
{
    HRESULT hr;
    IUnknown* pattern;
    hr = _elm->GetCurrentPattern(UIA_InvokePatternId, &pattern);
    Tell("_elm->GetCurrentPattern(UIA_InvokePatternId, &pattern) failed");
    if (!pattern)
    {
        return false;
    }

    IUIAutomationInvokePattern* inv;
    hr = pattern->QueryInterface(&inv);
    if (!inv)
    {
        return false;
    }

    hr = inv->Invoke();
    return true;
}

}
