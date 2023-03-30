#include "platform.hpp"
#include "element.hpp"
#include "element_chain.hpp"

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

BOOL CALLBACK platform::_EnumWindowsCb(HWND wnd, LPARAM par)
{
    auto info = GetWndInfo(wnd);
    I()->_desktop_wnds[info.cls].push_back(info);
    return TRUE; // continue
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

void platform::_GetElementStacks(shared_ptr<element> root, point p, vector<shared_ptr<element>>& ve)
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
    _GetElementStacks(xref<element>::x(uia_e), p, ve);

    return xref<element_chain>::x(WndInfo(), ve);
}

shared_ptr<element> platform::FindElementInActiveWindow()
{
    return nullptr;
}

shared_ptr<element> platform::FindElementInDesktop()
{
    return nullptr;
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
