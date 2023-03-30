#include "platform.hpp"
#include "element.hpp"
#include "identifier.hpp"

namespace slim
{

element::element(IUIAutomationCondition* con, int sub_idx, int depth, IUIAutomationElement* e) :
    _con(con),
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

element::element(IUIAutomationCondition* con, IUIAutomationElement* e) :
    _con(con),
    _sub_idx(-1),
    _depth(-1),
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

list<shared_ptr<element>> element::GenerateStacks(IUIAutomationCondition* con, point p)
{
    auto elm = make_shared<element>(con);
}

bool element::LoadSub(int depth, int depth_remain)
{
    if (Invalid())
    {
        return false;
    }

    HRESULT hr;
    IUIAutomationElementArray* elmArr;

    hr = _elm->FindAll(TreeScope_Children, _con, &elmArr);
    Tell("FindAll() failed when LoadSub()");

    int len = 0;
    hr = elmArr->get_Length(&len);
    Tell("get_Length failed");

    for (int i = 0; i < len; ++i)
    {
        IUIAutomationElement* c;
        hr = elmArr->GetElement(i, &c);
        Tell("elmArr->GetElement failed");
        _subs.push_back(make_shared<element>(_con, i, depth, c));
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
    BOOL bl;

    hr = _elm->get_CurrentName(&bstr);
    if (SUCCEEDED(hr))
    {
        _property["Name"] = ToString(bstr);
        SysFreeString(bstr);
    }

    hr = _elm->get_CurrentAutomationId(&bstr);
    if (SUCCEEDED(hr))
    {
        _property["AutomationId"] = ToString(bstr);
        SysFreeString(bstr);
    }

    //hr = _elm->get_CurrentAccessKey(&bstr);
    //Tell("_elm->get_CurrentAccessKey failed");
    //_property["AccessKey"] = ToString(bstr);

    //hr = _elm->GetCurrentPropertyValue(UIA_ValueValuePropertyId, &var);
    //Tell("_elm->GetCurrentPropertyValue failed");
    //_property["Value"] = ToString(var.bstrVal);

    hr = _elm->get_CurrentControlType(&ct);
    if (SUCCEEDED(hr))
    {
        _property["ControlType"] = to_string(ct);
        _interact = interact_ct.count(ct) > 0;
    }

    hr = _elm->get_CurrentLocalizedControlType(&bstr);
    if (SUCCEEDED(hr))
    {
        _property["LocalizedControlType"] = ToString(bstr);
        SysFreeString(bstr);
    }

    bl = FALSE;
    IUIAutomationElement9* e9 = nullptr;
    hr = _elm->QueryInterface(&e9);
    if (SUCCEEDED(hr))
    {
        hr = e9->get_CurrentIsDialog(&bl);
        if (SUCCEEDED(hr) && bl)
        {
            _property["IsDialog"] = "true";
        }
    }

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
    double areaSqr4 = sqrt(sqrt(areaSz));

    // g: 1 ~ 20
    // areaSqr: 5 ~ 20
    double g = double(Depth()) + 1.0;
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

    // max 2.0
    double lcss = (double)LCSS(stack.element_name, Property("Name"));
    lcss = max(lcss, 5);
    if (lcss > 0.1)
    {
        int len1 = (int)stack.element_name.size();
        int len2 = (int)Property("Name").size();
        lcss *= (min(len1, len2)) / (max(len1, len2) + 0.1);
    }
    score_inc += lcss * 0.4;

    // max 1.5
    int idx_diff = abs(stack.sub_index - SubIdx());
    score_inc += (5 - max(idx_diff, 5)) * 0.3;

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

bool element::Act(action_type actionType)
{
    switch (actionType)
    {
    case action_type::ACTION_TEST:
        return Test();

    case action_type::HOVER:
        return Hover();

    case action_type::LEFT_CLICK:
        return Envoke();

    case action_type::RIGHT_CLICK:
        return Menu();

    default:
        return false;
    }
}

bool element::Hover()
{
    int x = Area().center().x;
    int y = Area().center().y;
    if (!SetCursorPos(x, y))
    {
        return false;
    }
    Sleep(500);
    return true;
}

bool element::Envoke()
{
    // always use physical button
    return Envoke_();
    //HRESULT hr;
    //IUnknown* pattern;
    //hr = _elm->GetCurrentPattern(UIA_InvokePatternId, &pattern);
    //Tell("_elm->GetCurrentPattern(UIA_InvokePatternId, &pattern) failed");
    //if (FAILED(hr) || !pattern)
    //{
    //    return Envoke_();
    //}

    //IUIAutomationInvokePattern* inv;
    //hr = pattern->QueryInterface(&inv);
    //if (FAILED(hr) || !inv)
    //{
    //    return Envoke_();
    //}

    //hr = inv->Invoke();
    //if (FAILED(hr))
    //{
    //    return Envoke_();
    //}
    //return true;
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

    INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dx = x;
    input.mi.dy = y;

    Sleep(10);
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE;
    if (SendInput(1, &input, sizeof(INPUT)) == 0)
    {
        return false;
    }

    Sleep(10);
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE;
    if (SendInput(1, &input, sizeof(INPUT)) == 0)
    {
        return false;
    }

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

    INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dx = x;
    input.mi.dy = y;

    Sleep(10);
    input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_ABSOLUTE;
    if (SendInput(1, &input, sizeof(INPUT)) == 0)
    {
        return false;
    }

    Sleep(10);
    input.mi.dwFlags = MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_ABSOLUTE;
    if (SendInput(1, &input, sizeof(INPUT)) == 0)
    {
        return false;
    }

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
