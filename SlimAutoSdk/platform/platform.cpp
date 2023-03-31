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

    hr = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_uia));
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

BOOL CALLBACK _EnumWindowsCb(HWND wnd, LPARAM par)
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

vector<WndInfo> platform::GetWnds(const string& cls)
{
    guard __g(Mutex("_desktop_wnds"));
    return I()->_desktop_wnds[cls];
}

struct elementSt
{
    shared_ptr<slim::element>  elm;
    int                        parent;
    int                        dialogs;
};

static void _PermuteElementTree(vector<elementSt>& candidates)
{
    int next = 0;
    while (next < candidates.size())
    {
        auto nest = candidates[next];
        for (int i = 0; i < nest.elm->SubCount(); ++i)
        {
            nest.elm->LoadSub(false);
            auto s = nest.elm->Sub(i);
            candidates.push_back({ s, next, nest.dialogs + (s->_dialog ? 1 : 0) });
        }
        ++next;
    }
}

void _GetElementStacks(shared_ptr<element> root, point p, vector<shared_ptr<element>>& ve)
{
    //HRESULT hr;
    vector<elementSt> candidates = {{ root, -1, 0 }};
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

    hr = I()->_uia->ElementFromHandleBuildCache(wnd, I()->_request, &uia_e);
    Fail(nullptr, "I()->_uia->ElementFromHandleBuildCache(wnd, I()->_request, &uia_e) fail");

    vector<shared_ptr<element>> ve;
    _GetElementStacks(xref<element>::x(uia_e), p, ve);

    return xref<element_chain>::x(GetWndInfo(wnd), ve);
}

shared_ptr<element_chain> platform::GetElementChainInDesktop(point p)
{
    HRESULT hr;
    IUIAutomationElement* uia_e;

    hr = I()->_uia->GetRootElementBuildCache(I()->_request, &uia_e);
    Fail(nullptr, "I()->_uia->GetRootElementBuildCache(I()->_request, &uia_e) fail");

    vector<shared_ptr<element>> ve;
    _GetElementStacks(xref<element>::x(uia_e, -1, 0), p, ve);

    return xref<element_chain>::x(WndInfo(), ve);
}

element_match _FindElement(shared_ptr<element> self, double score, const vector<element_stack>& es, int es_len = -1)
{
    if (es_len == -1)
    {
        es_len = es.size();
    }
    if (es_len < 1)
    {
        return {};
    }

    double score_inc = 0;
    const auto& stack = es[es_len - 1];
    if (stack.automation_id != self->_auto_id || stack.control_type != self->_control)
    {
        return {};
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

    double score_new = score + score_inc;
    if (es_len == 1)
    {
        rst.push_back({ self, score_new });
        return;
    }

    for (int i = 0; i < SubCount(); ++i)
    {
        auto s = Sub(i);
        s->Matching(es, es_end - 1, s, score_new, rst);
    }

    return {};
}

element_match platform::FindElementInWindow(WndInfo& wnd_info, vector<element_stack>& ess)
{
    HRESULT hr;
    HWND wnd;
    IUIAutomationElement* uia_e;

    hr = I()->_uia->ElementFromHandleBuildCache(wnd_info.wnd, I()->_request, &uia_e);
    Fail({}, "I()->_uia->ElementFromHandleBuildCache(wnd_info.wnd, I()->_request, &uia_e) fail");

    auto root = xref<element>::x(uia_e, -1, 0);
    return _FindElement(root, 0, ess);
}

element_match platform::FindElementInDesktop(vector<element_stack>& ess)
{
    HRESULT hr;
    IUIAutomationElement* uia_e;

    hr = I()->_uia->GetRootElementBuildCache(I()->_request, &uia_e);
    Fail({}, "I()->_uia->GetRootElementBuildCache(I()->_request, &uia_e) fail");

    auto root = xref<element>::x(uia_e, -1, 0);
    return _FindElement(root, ess, ess.size());
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
