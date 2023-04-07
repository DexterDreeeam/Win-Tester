#include "platform.hpp"
#include "element.hpp"
#include "element_stack.hpp"

namespace slim
{

element::element(IUIAutomationElement* e, int depth, int prnt_idx) :
    _uia_e(e),
    _subs(),
    _subs_loaded(false),
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
    RECT rc;
    BSTR bstr;
    //VARIANT var;
    CONTROLTYPEID ct;
    BOOL bl;

    hr = CacheElement ? _uia_e->get_CachedBoundingRectangle(&rc) : _uia_e->get_CurrentBoundingRectangle(&rc);
    if (SUCCEEDED(hr))
    {
        _area = area(rc);
    }

    hr = CacheElement ? _uia_e->get_CachedName(&bstr) : _uia_e->get_CurrentName(&bstr);
    if (SUCCEEDED(hr))
    {
        _name = ToString(bstr);
        SysFreeString(bstr);
    }

    // hr = _uia_e->get_CurrentAutomationId(&bstr);
    hr = CacheElement ? _uia_e->get_CachedAutomationId(&bstr) : _uia_e->get_CurrentAutomationId(&bstr);
    if (SUCCEEDED(hr))
    {
        _auto_id = ToString(bstr);
        SysFreeString(bstr);
    }

    hr = CacheElement ? _uia_e->get_CachedLocalizedControlType(&bstr) : _uia_e->get_CurrentLocalizedControlType(&bstr);
    if (SUCCEEDED(hr))
    {
        _control_str = ToString(bstr);
        SysFreeString(bstr);
    }

    hr = CacheElement ? _uia_e->get_CachedControlType(&ct) : _uia_e->get_CurrentControlType(&ct);
    if (SUCCEEDED(hr))
    {
        _control = ct;
        _interact = interact_ct.count(ct) > 0;
    }

    IUIAutomationElement9* e9 = nullptr;
    hr = _uia_e->QueryInterface(&e9);
    if (SUCCEEDED(hr) && e9)
    {
        bl = FALSE;
        hr = CacheElement ? e9->get_CachedIsDialog(&bl) : e9->get_CurrentIsDialog(&bl);
        if (SUCCEEDED(hr) && bl)
        {
            _dialog = true;
        }
    }
}

void element::LoadSub(bool recur)
{
    if (Invalid() || _subs.size() > 0 || _subs_loaded)
    {
        return;
    }

    HRESULT hr;
    IUIAutomationElementArray* arr;
    hr = CacheElement ?
        _uia_e->FindAllBuildCache(TreeScope_Children, platform::I()->_con, platform::I()->_request, &arr) :
        _uia_e->FindAll(TreeScope_Children, platform::I()->_con, &arr);
    Fail(, "_uia_e->FindAllBuildCache(TreeScope_Children, platform::I()->_con, platform::I()->_request, &arr)");
    escape ef = [=]() mutable { if (arr) arr->Release(); };

    if (!arr)
    {
        return;
    }
    int len = 0;
    hr = arr->get_Length(&len);
    Fail(, "arr->get_Length(&len)");

    for (int i = 0; i < len; ++i)
    {
        IUIAutomationElement* c = nullptr;;
        hr = arr->GetElement(i, &c);
        Fail(,"arr->GetElement(i, &c)");
        if (!c)
        {
            continue;
        }
        auto r_c = xref<element>::x(c, i, _depth == -1 ? -1 : _depth + 1);
        if (recur)
        {
            r_c->LoadSub(true);
        }
        _subs.push_back(r_c);
    }

    _subs_loaded = true;
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
    // areaSqr4: 5 ~ 20
    double g = double(_depth) + 1.0;
    return g / areaSqr4;
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
