#include "platform.hpp"
#include "element.hpp"
#include "element_stack.hpp"

namespace slim
{

element::element(IUIAutomationElement* e, int prnt_idx, int depth) :
    _uia_e(e),
    _subs(),
    _parent_idx(prnt_idx),
    _depth(depth),
    _area(),
    _name(),
    _auto_id(),
    _control_str(),
    _control(),
    _interact(false),
    _dialog(false)
{
    _LoadProperty();
}

void element::_LoadProperty()
{
    if (Invalid())
    {
        return;
    }

    HRESULT hr;
    BSTR bstr;
    //VARIANT var;
    CONTROLTYPEID ct;
    BOOL bl;

    hr = _uia_e->get_CachedName(&bstr);
    if (SUCCEEDED(hr))
    {
        _name = ToString(bstr);
        SysFreeString(bstr);
    }

    hr = _uia_e->get_CachedAutomationId(&bstr);
    if (SUCCEEDED(hr))
    {
        _auto_id = ToString(bstr);
        SysFreeString(bstr);
    }

    hr = _uia_e->get_CachedLocalizedControlType(&bstr);
    if (SUCCEEDED(hr))
    {
        _control_str = ToString(bstr);
        SysFreeString(bstr);
    }

    hr = _uia_e->get_CachedControlType(&ct);
    if (SUCCEEDED(hr))
    {
        _control = ct;
        _interact = interact_ct.count(ct) > 0;
    }

    IUIAutomationElement9* e9 = nullptr;
    hr = _uia_e->QueryInterface(&e9);
    if (SUCCEEDED(hr))
    {
        bl = FALSE;
        hr = e9->get_CachedIsDialog(&bl);
        if (SUCCEEDED(hr) && bl)
        {
            _dialog = true;
        }
    }
}

void element::LoadSub(bool recur)
{
    if (Invalid() || _subs.size() > 0)
    {
        return;
    }

    HRESULT hr;
    IUIAutomationElementArray* arr;

    hr = _uia_e->FindAllBuildCache(TreeScope_Children, platform::I()->_con, platform::I()->_request, &arr);
    Fail(, "_uia_e->FindAllBuildCache(TreeScope_Children, platform::I()->_con, platform::I()->_request, &arr)");
    escape ef = [=]() mutable { arr->Release(); };

    int len = 0;
    hr = arr->get_Length(&len);
    Fail(, "arr->get_Length(&len)");

    for (int i = 0; i < len; ++i)
    {
        IUIAutomationElement* c;
        hr = arr->GetElement(i, &c);
        Fail(,"arr->GetElement(i, &c)");
        auto r_c = xref<element>::x(c, i, _depth == -1 ? -1 : _depth + 1);
        if (recur)
        {
            r_c->LoadSub(true);
        }
        _subs.push_back(r_c);
    }
}

double element::InteractGrade(point p)
{
    if (!_area.Inside(p) || !_interact)
    {
        return 0;
    }

    double areaSz = (double)_area.size();
    if (areaSz <= 0)
    {
        return 0;
    }
    double areaSqr4 = sqrt(sqrt(areaSz));

    // g: 1 ~ 20
    // areaSqr: 5 ~ 20
    double g = double(_depth) + 1.0;
    return g / areaSz;
}

string element::Identifier() const
{
    string id = "";
    id += "[" + to_string(_parent_idx) + "]";
    id += "(" + to_string(_control) + ")";
    id += "{{" + _name + "}}";
    id += "<<" + _auto_id + ">>";
    return id;
}

string element::FriendlyIdentifier() const
{
    string id = "";
    id += "[" + to_string(_parent_idx) + "]";
    id += "(" + _control_str + ")";
    id += "{{" + _name + "}}";
    id += "<<" + _auto_id + ">>";
    id += " " + _area.stringify();
    return id;
}

void element::Matching(
    const vector<element_stack>& es, int es_end, shared_ptr<element> self,
    double score, vector<pair<shared_ptr<element>, double>>& rst)
{
    if (es_end < 0)
    {
        return;
    }

    double score_inc = 0;
    auto& stack = es[es_end];

    if (stack.automation_id != _auto_id || stack.control_type != _control)
    {
        return;
    }

    // max 2.0
    double lcss = (double)LCSS(stack.element_name, _name);
    lcss = max(lcss, 5);
    if (lcss > 0.1)
    {
        int len1 = (int)stack.element_name.size();
        int len2 = (int)_name.size();
        lcss *= (min(len1, len2)) / (max(len1, len2) + 0.1);
    }
    score_inc += lcss * 0.4;

    // max 1.5
    int idx_diff = abs(stack.parent_index - _parent_idx);
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
    int x = _area.center().x;
    int y = _area.center().y;
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
    hr = _uia_e->GetCurrentPattern(UIA_LegacyIAccessiblePatternId, &pattern);
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
    int x = _area.center().x;
    int y = _area.center().y;
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
    int x = _area.center().x;
    int y = _area.center().y;
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
        hr = _uia_e->GetCurrentPattern(i, &pattern);
        if (SUCCEEDED(hr) && pattern)
        {
            //platform::I()->Console(format("{} Control Pattern is supported", i));
        }
    }

    return true;
}

}
