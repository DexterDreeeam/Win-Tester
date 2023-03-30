#include "platform.hpp"

namespace slim
{

platform::platform() :
    _ctx(nullptr),
    _con(nullptr)
{
    HRESULT hr;
    hr = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_ctx));
    Tell("create platform failed");

    hr = _ctx->CreateTrueCondition(&_con);
    Tell("create true condition failed");

    _mtxs["_desktop_wnds"] = new mutex();
    _mtxs["_consoles"]     = new mutex();
}

platform::~platform()
{
    Rels(_con);
    Rels(_ctx);
}

BOOL CALLBACK platform::_EnumWindowsCb(HWND wnd, LPARAM par)
{
    auto info = I()->GetWndInfo(wnd);
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
    lock_guard<mutex> _guard(*_mtxs["_desktop_wnds"]);
    I()->_desktop_wnds.clear();
    EnumWindows(_EnumWindowsCb, NULL);
}

vector<WndInfo> platform::GetWnds()
{
    lock_guard<mutex> _guard(*_mtxs["_desktop_wnds"]);
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
    lock_guard<mutex> _guard(*_mtxs["_desktop_wnds"]);
    return I()->_desktop_wnds[cls];
}

shared_ptr<element> platform::Diagram(HWND win)
{
    HRESULT hr;
    IUIAutomationElement* root = nullptr;

    if (!win)
    {
        win = GetForegroundWindow();
    }

    hr = _ctx->ElementFromHandle(win, &root);
    Tell("ctx->ElementFromHandle(win, &root) failed");

    auto ret = make_shared<element>(_ins, 0, 0, root);
    ret->LoadSub(1);

    return ret;
}

shared_ptr<element_chain> platform::ElementChainByPoint(point p)
{
    HWND window = GetForegroundWindow();
    // e : e_parent_idx_in_cand
    vector<pair<shared_ptr<slim::element>, int>> cand = {};
    vector<int> throughDialog = {};
    Elements(window, p, cand);
    if (cand.size() == 0)
    {
        return nullptr;
    }

    double maxGrade = -1;
    int maxElmIdx = -1;
    for (int i = 0; i < cand.size(); ++i)
    {
        int dialogStacks = cand[i].second >= 0 ? throughDialog[cand[i].second] : 1;
        bool isDialog = cand[i].first->IsDialog();
        dialogStacks += isDialog ? 1 : 0;
        double iG = cand[i].first->InteractGrade(p) * dialogStacks;
        if (iG > maxGrade)
        {
            maxGrade = iG;
            maxElmIdx = i;
        }
        throughDialog.push_back(dialogStacks);
    }
    if (maxElmIdx == -1)
    {
        return nullptr;
    }

    vector<shared_ptr<slim::element>> chain;
    int parent = cand[maxElmIdx].second;
    chain.push_back(cand[maxElmIdx].first);
    while (parent >= 0)
    {
        auto p = cand[parent];
        parent = p.second;
        chain.push_back(p.first);
    }

    auto pName = WndClassName(window);
    auto ec = make_shared<element_chain>();
    ec->Load(pName.first, pName.second, chain);
    return ec;
}

void platform::Elements(HWND wnd, point p, vector<pair<shared_ptr<slim::element>, int>>& vpei)
{
    if (!wnd)
    {
        wnd = GetForegroundWindow();
    }

    auto root = slim::platform::I()->Diagram(wnd);
    if (p.x != INT_MIN && !root->Area().Inside(p))
    {
        return;
    }

    // e : e_parent_idx_in_cand
    vector<pair<shared_ptr<slim::element>, int>> elms = { { root, -1 } };
    while (elms.size())
    {
        auto e = elms.back();
        elms.pop_back();
        vpei.push_back(e);
        int parent_idx = (int)vpei.size() - 1;
        for (int i = e.first->SubCount() - 1; i >= 0; --i)
        {
            auto s = e.first->Sub(i);
            elms.push_back({ s, parent_idx });
        }
    }
}

shared_ptr<element_chain> platform::ElementChainByPoint2(point p)
{
    HRESULT hr;
    IUIAutomationElement* elm;
    hr = _ctx->ElementFromPoint(POINT(p.x, p.y), &elm);
    if (FAILED(hr))
    {
        return ElementChainByPoint(p);
    }

    

    // e : e_parent_idx_in_cand
    vector<pair<shared_ptr<slim::element>, int>> cand = {};
    vector<int> throughDialog = {};
    Elements(window, p, cand);
    if (cand.size() == 0)
    {
        return nullptr;
    }

    double maxGrade = -1;
    int maxElmIdx = -1;
    for (int i = 0; i < cand.size(); ++i)
    {
        int dialogStacks = cand[i].second >= 0 ? throughDialog[cand[i].second] : 1;
        bool isDialog = cand[i].first->IsDialog();
        dialogStacks += isDialog ? 1 : 0;
        double iG = cand[i].first->InteractGrade(p) * dialogStacks;
        if (iG > maxGrade)
        {
            maxGrade = iG;
            maxElmIdx = i;
        }
        throughDialog.push_back(dialogStacks);
    }
    if (maxElmIdx == -1)
    {
        return nullptr;
    }

    vector<shared_ptr<slim::element>> chain;
    int parent = cand[maxElmIdx].second;
    chain.push_back(cand[maxElmIdx].first);
    while (parent >= 0)
    {
        auto p = cand[parent];
        parent = p.second;
        chain.push_back(p.first);
    }

    auto pName = WndClassName(window);
    auto ec = make_shared<element_chain>();
    ec->Load(pName.first, pName.second, chain);
    return ec;
}

}
