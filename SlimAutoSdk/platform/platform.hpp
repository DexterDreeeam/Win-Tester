#pragma once

#include "common.hpp"
#include "element.hpp"
#include "element_chain.hpp"

namespace slim
{

class platform
{
    friend class element;

    platform();

public:
    static shared_ptr<platform> Instance()
    {
        return _ins ? _ins : (_ins = shared_ptr<platform>(new platform()));
    }

    static shared_ptr<platform> I()
    {
        return Instance();
    }

    ~platform();

    shared_ptr<element> Diagram(HWND win = nullptr);

    shared_ptr<element_chain> ElementChainByPoint(point p);

    void Elements(HWND wnd, point p, vector<pair<shared_ptr<slim::element>, int>>& vpei);

    pair<string, string> WndClassName(HWND wnd);

    const vector<string>& Consoles() const
    {
        return _consoles;
    }

    void Console(const string& c)
    {
        _consoles.push_back(c);
    }

    void ClearConsole()
    {
        _consoles.clear();
    }

private:
    static shared_ptr<platform> _ins;

private:
    IUIAutomation*           _ctx;
    IUIAutomationCondition*  _con;
    vector<string>           _consoles;
};

__declspec(selectany) shared_ptr<platform> platform::_ins = nullptr;

}
