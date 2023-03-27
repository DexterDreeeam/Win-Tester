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
}

platform::~platform()
{
    Rels(_con);
    Rels(_ctx);
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
    auto root = slim::platform::I()->Diagram(window);
    if (!root->Area().Inside(p))
    {
        return nullptr;
    }

    // e : e_parent_idx_in_cand
    vector<pair<shared_ptr<slim::element>, int>> elms = { { root, -1 } };
    vector<pair<shared_ptr<slim::element>, int>> cand = {};

    while (elms.size())
    {
        auto e = elms.back();
        elms.pop_back();
        cand.push_back(e);
        int parent_idx = (int)cand.size() - 1;
        for (int i = e.first->SubCount() - 1; i >= 0; --i)
        {
            auto s = e.first->Sub(i);
            elms.push_back({ s, parent_idx });
        }
    }

    double maxGrade = -1;
    int maxElmIdx = -1;
    for (int i = 0; i < cand.size(); ++i)
    {
        double iG = cand[i].first->InteractGrade(p);
        if (iG > maxGrade)
        {
            maxGrade = iG;
            maxElmIdx = i;
        }
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

    char ch_win[256];
    int winNameLen = GetWindowTextA(window, ch_win, 255);
    ch_win[winNameLen] = 0;

    char ch_cls[256] = {};
    int classNameLen = GetClassNameA(window, ch_cls, 255);
    ch_cls[classNameLen] = 0;

    auto ec = make_shared<element_chain>();
    ec->Load(ch_win, ch_cls, chain);
    return ec;
}

}
