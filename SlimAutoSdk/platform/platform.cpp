#include "platform.hpp"
#include "element.hpp"
#include "element_chain.hpp"
#include "action.hpp"

namespace slim
{

platform::platform() :
    _uia(nullptr),
    _con(nullptr),
    _request(nullptr),
    _mtxs(),
    _desktop_wnds(),
    _consoles()
{
    HRESULT hr;
    hr = CoInitialize(NULL);
    Tell("CoInitialize(NULL) error")

    hr = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_uia));
    Tell("CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_ctx))error");

    hr = _uia->CreateTrueCondition(&_con);
    Tell("CreateTrueCondition(&_con) error");

    hr = _uia->CreateCacheRequest(&_request);
    Tell("_uia->CreateCacheRequest(&_request) error");

    for (auto p : element_props)
    {
        hr = _request->AddProperty(p);
        Tell("_request->AddProperty(p) error");
    }

    _mtxs["_desktop_wnds"] = new mutex();
    _mtxs["_consoles"]     = new mutex();
}

platform::~platform()
{
    Rels(_request);
    Rels(_con);
    Rels(_uia);
}

mutex& platform::Mutex(const string& name)
{
    return *I()->_mtxs[name];
}

WndInfo platform::GetWndInfo(HWND wnd)
{
    char ch_win[256];
    int winNameLen = GetWindowTextA(wnd, ch_win, 255);
    ch_win[winNameLen] = 0;

    char ch_cls[256] = {};
    int classNameLen = GetClassNameA(wnd, ch_cls, 255);
    ch_cls[classNameLen] = 0;

    WndInfo ret;
    ret.cls = ch_cls;
    ret.win = ch_win;
    ret.wnd = wnd;
    return ret;
}

WndInfo platform::GetCurrentWndInfo()
{
    auto hwnd = GetForegroundWindow();
    if (!hwnd)
    {
        return WndInfo();
    }

    return GetWndInfo(hwnd);
}

BOOL CALLBACK platform::_EnumWindowsCb(HWND wnd, LPARAM par)
{
    auto info = platform::I()->GetWndInfo(wnd);
    I()->_desktop_wnds[info.cls].push_back(info);
    return TRUE; // continue
}

void platform::UpdateDesktopWnds()
{
    guard __g(Mutex("_desktop_wnds"));
    I()->_desktop_wnds.clear();
    EnumWindows(_EnumWindowsCb, NULL);
}

vector<WndInfo> platform::GetWnds()
{
    guard __g(Mutex("_desktop_wnds"));
    vector<WndInfo> ret;
    for (const auto& p : I()->_desktop_wnds)
    {
        for (auto& info : p.second)
        {
            ret.push_back(info);
        }
    }
    return ret;
}

vector<WndInfo> platform::GetWnds(const WndInfo& info)
{
    guard __g(Mutex("_desktop_wnds"));
    return I()->_desktop_wnds[info.cls];
}

void platform::_PermuteElementTree(vector<TreeElementProxy>& candidates)
{
    int next = 0;
    while (next < candidates.size())
    {
        auto proxy = candidates[next];
        proxy.elm->LoadSub(false);
        for (int i = 0; i < proxy.elm->SubCount(); ++i)
        {
            auto s = proxy.elm->Sub(i);
            if (HasWinTesterStr(s->_name))
            {
                continue;
            }
            candidates.push_back({ s, next, proxy.dialogs + (s->_dialog ? 1 : 0) });
        }
        ++next;
    }
}

void platform::_GetElementStacks(shared_ptr<element> root, point p, vector<shared_ptr<element>>& ve)
{
    //HRESULT hr;
    vector<TreeElementProxy> candidates = {{ root, -1, 0 }};
    _PermuteElementTree(candidates);

    double maxGrade = -1.0;
    int maxElmIdx = -1;
    for (int i = 0; i < candidates.size(); ++i)
    {
        double iG = candidates[i].elm->InteractGrade(p) * (candidates[i].dialogs + 1);
        if (iG > maxGrade)
        {
            maxGrade = iG;
            maxElmIdx = i;
        }
    }
    if (maxElmIdx == -1)
    {
        return;
    }

    int parent = candidates[maxElmIdx].parent;
    ve.clear();
    ve.push_back(candidates[maxElmIdx].elm);
    while (parent >= 0)
    {
        auto st = candidates[parent];
        parent = st.parent;
        ve.push_back(st.elm);
    }
}

shared_ptr<element_chain> platform::GetElementChainInActiveWindow(point p)
{
    HRESULT hr;
    HWND wnd;
    IUIAutomationElement* uia_e;

    wnd = GetForegroundWindow();
    if (!wnd)
    {
        return nullptr;
    }

    auto info = GetWndInfo(wnd);
    if (HasWinTesterStr(info.cls) || HasWinTesterStr(info.win))
    {
        return nullptr;
    }

    hr = CacheElement ?
        I()->_uia->ElementFromHandleBuildCache(wnd, I()->_request, &uia_e) :
        I()->_uia->ElementFromHandle(wnd, &uia_e);
    Fail(nullptr, "I()->_uia->ElementFromHandleBuildCache(wnd, I()->_request, &uia_e) fail");

    vector<shared_ptr<element>> ve;
    _GetElementStacks(xref<element>::x(uia_e, 0, -1), p, ve);
    return xref<element_chain>::x(GetWndInfo(wnd), ve);
}

shared_ptr<element_chain> platform::GetElementChainInDesktop(point p)
{
    HRESULT hr;
    IUIAutomationElement* uia_e;

    hr = CacheElement ?
        I()->_uia->GetRootElementBuildCache(I()->_request, &uia_e) :
        I()->_uia->GetRootElement(&uia_e);
    Fail(nullptr, "I()->_uia->GetRootElementBuildCache(I()->_request, &uia_e) fail");

    vector<shared_ptr<element>> ve;
    _GetElementStacks(xref<element>::x(uia_e, 0, -1), p, ve);

    return xref<element_chain>::x(WndInfo(), ve);
}

shared_ptr<element_matched> platform::_FindElement(shared_ptr<element> self, const element_searching& searching)
{
    if (searching.ess_len < 1)
    {
        return nullptr;
    }

    double score_inc = 0;
    const auto& stack = searching.ess[searching.ess_len - 1];
    if (stack.automation_id != self->_auto_id || stack.control_type != self->_control)
    {
        return nullptr;
    }

    // max 2.0
    double lcss = (double)LCSS(stack.element_name, self->_name);
    lcss = max(lcss, 5);
    if (lcss > 0.1)
    {
        int len1 = (int)stack.element_name.size();
        int len2 = (int)self->_name.size();
        lcss *= (min(len1, len2)) / (max(len1, len2) + 0.1);
    }
    score_inc += lcss * 0.4;

    // max 1.5
    int idx_diff = abs(stack.parent_index - self->_parent_idx);
    score_inc += (5 - max(idx_diff, 5)) * 0.3;

    if (searching.ess_len == 1)
    {
        return xref<element_matched>::x(self, searching.score + score_inc);
    }

    element_searching next_searching = element_searching(searching);
    next_searching.ess_len--;
    next_searching.dialogs += self->_dialog ? 1 : 0;
    next_searching.score += score_inc;

    shared_ptr<element_matched> rst = nullptr;
    self->LoadSub(false);
    for (int i = 0; i < self->SubCount(); ++i)
    {
        auto s = self->Sub(i);
        if (HasWinTesterStr(s->_name))
        {
            continue;
        }
        auto m = _FindElement(s, next_searching);
        if (m)
        {
            rst = !rst ? m : m->score > rst->score ? m : rst;
        }
    }
    return rst;
}

shared_ptr<element_matched> platform::FindElementInWindow(WndInfo& wnd, const vector<element_stack>& ess)
{
    HRESULT hr;
    IUIAutomationElement* uia_e;

    if (HasWinTesterStr(wnd.cls) || HasWinTesterStr(wnd.win))
    {
        return nullptr;
    }

    hr = CacheElement ?
        I()->_uia->ElementFromHandleBuildCache(wnd.wnd, I()->_request, &uia_e) :
        I()->_uia->ElementFromHandle(wnd.wnd, &uia_e);
    Fail(nullptr, "I()->_uia->ElementFromHandleBuildCache(wnd_info.wnd, I()->_request, &uia_e) fail");

    auto root = xref<element>::x(uia_e, -1, 0);
    element_searching searching(ess);
    return _FindElement(root, searching);
}

shared_ptr<element_matched> platform::FindElementInDesktop(const vector<element_stack>& ess)
{
    HRESULT hr;
    IUIAutomationElement* uia_e;

    hr = CacheElement ?
        I()->_uia->GetRootElementBuildCache(I()->_request, &uia_e) :
        I()->_uia->GetRootElement(&uia_e);
    Fail(nullptr, "I()->_uia->GetRootElementBuildCache(I()->_request, &uia_e) fail");

    auto root = xref<element>::x(uia_e, -1, 0);
    element_searching searching(ess);
    return _FindElement(root, searching);
}

void platform::Test(point p)
{

}

vector<string> platform::Consoles()
{
    guard __g(Mutex("_consoles"));
    return I()->_consoles;
}

void platform::Console(const string& c)
{
    guard __g(Mutex("_consoles"));
    return I()->_consoles.push_back(c);
}

void platform::ClearConsole()
{
    guard __g(Mutex("_consoles"));
    return I()->_consoles.clear();
}

}
