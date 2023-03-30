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

    WndInfo GetWndInfo(HWND wnd);

    void UpdateDesktopWnds();

    vector<WndInfo> GetWnds();

    vector<WndInfo> GetWnds(const string& cls);

    shared_ptr<element> Diagram(HWND win = nullptr);

    shared_ptr<element_chain> ElementChainByPoint(point p);

    shared_ptr<element_chain> ElementChainByPoint2(point p);

    void Elements(HWND wnd, point p, vector<pair<shared_ptr<slim::element>, int>>& vpei);

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
    static BOOL CALLBACK platform::_EnumWindowsCb(HWND hWnd, LPARAM lParam);

private:
    static shared_ptr<platform>   _ins;

private:
    IUIAutomation*                _ctx;
    IUIAutomationCondition*       _con;

    map<string, mutex*>           _mtxs;
    map<string, vector<WndInfo>>  _desktop_wnds; // class name : vInfo
    vector<string>                _consoles;
};

__declspec(selectany) shared_ptr<platform> platform::_ins = nullptr;

}
