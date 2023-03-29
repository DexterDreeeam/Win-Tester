#include "platform.hpp"
#include "element.hpp"
#include "identifier.hpp"

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
    id += "{{" + Property("Name") + "}}";
    id += "<<" + Property("AutomationId") + ">>";
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
    id += "{{" + Property("Name") + "}}";
    id += "<<" + Property("AutomationId") + ">>";
    id += " " + Area().stringify();
    return id;
}

void element::Matching(
    const vector<element_stack>& es, int es_end, shared_ptr<element> self,
    double score, vector<pair<shared_ptr<element>, double>>& rst)
{
    if (!LoadProperty() || es_end < 0)
    {
        return;
    }

    double score_inc = 0;
    auto& stack = es[es_end];

    string auto_id = Property("AutomationId");
    int ctl_type = atoi(Property("ControlType").c_str());

    if (stack.automation_id != auto_id || stack.control_type != ctl_type)
    {
        return;
    }

    if (stack.automation_id.size() > 0)
    {
        score_inc += 50.0;
    }
    if (stack.element_name == Property("Name"))
    {
        score_inc += 2.0;
    }
    if (stack.sub_index == SubIdx())
    {
        score_inc += 0.5;
    }

    double score_new = score + score_inc;
    if (es_end == 0)
    {
        rst.push_back({ self, score_new });
        return;
    }

    for (int i = 0; i < SubCount(); ++i)
    {
        auto s = Sub(i);
        s->Matching(es, es_end - 1, s, score_new, rst);
    }
}

bool element::Envoke()
{
    HRESULT hr;
    IUnknown* pattern;
    hr = _elm->GetCurrentPattern(UIA_InvokePatternId, &pattern);
    Tell("_elm->GetCurrentPattern(UIA_InvokePatternId, &pattern) failed");
    if (FAILED(hr) || !pattern)
    {
        return Envoke_();
    }

    IUIAutomationInvokePattern* inv;
    hr = pattern->QueryInterface(&inv);
    if (FAILED(hr) || !inv)
    {
        return Envoke_();
    }

    hr = inv->Invoke();
    if (FAILED(hr))
    {
        return Envoke_();
    }
    return true;
}

bool element::Envoke2()
{
    HRESULT hr;
    IUnknown* pattern;
    hr = _elm->GetCurrentPattern(UIA_LegacyIAccessiblePatternId, &pattern);
    Tell("_elm->GetCurrentPattern(UIA_LegacyIAccessiblePatternId, &pattern) failed");
    if (FAILED(hr) || !pattern)
    {
        return false;
    }

    IUIAutomationLegacyIAccessiblePattern* inv;
    hr = pattern->QueryInterface(&inv);
    if (FAILED(hr) || !inv)
    {
        return false;
    }

    inv->DoDefaultAction();
    if (FAILED(hr))
    {
        return false;
    }
    return true;
}

bool element::Envoke_()
{
    int x = Area().center().x;
    int y = Area().center().y;
    if (!SetCursorPos(x, y))
    {
        return false;
    }
    Sleep(10);
    mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, NULL);
    Sleep(10);
    mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, NULL);
    return true;
}

bool element::Menu()
{
    int x = Area().center().x;
    int y = Area().center().y;
    if (!SetCursorPos(x, y))
    {
        return false;
    }
    Sleep(10);
    mouse_event(MOUSEEVENTF_RIGHTDOWN, x, y, 0, NULL);
    Sleep(10);
    mouse_event(MOUSEEVENTF_RIGHTUP, x, y, 0, NULL);
    return true;
}

bool element::Test()
{
    HRESULT hr;
    IUnknown* pattern;
    for (int i = UIA_InvokePatternId; i < UIA_InvokePatternId + 50; ++i)
    {
        hr = _elm->GetCurrentPattern(i, &pattern);
        if (SUCCEEDED(hr) && pattern)
        {
            platform::I()->Console(format("{} Control Pattern is supported", i));
        }
    }

    return true;
}

}
