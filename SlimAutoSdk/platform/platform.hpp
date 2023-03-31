#pragma once

#include "common.hpp"
#include "area.hpp"
#include "identifier.hpp"
#include "element_stack.hpp"

namespace slim
{

class element;
class element_chain;
class action;

class platform : public xref<platform>
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

    static mutex& Mutex(const string& name);

    static WndInfo GetWndInfo(HWND wnd);

    static void UpdateDesktopWnds();

    static vector<WndInfo> GetWnds();

    static vector<WndInfo> GetWnds(const string& cls);

    static shared_ptr<element_chain> GetElementChainInActiveWindow(point p);

    static shared_ptr<element_chain> GetElementChainInDesktop(point p);

    static element_match FindElementInWindow(WndInfo& wnd_info, vector<element_stack>& ess);

    static element_match FindElementInDesktop(vector<element_stack>& ess);

    static void Test(point p);

    static vector<string> Consoles();

    static void Console(const string& c);

    static void ClearConsole();

private:
    static shared_ptr<platform>   _ins;

private:
    IUIAutomation*                _uia;
    IUIAutomationCondition*       _con;
    IUIAutomationCacheRequest*    _request;

    map<string, mutex*>           _mtxs;
    map<string, vector<WndInfo>>  _desktop_wnds; // class name : vInfo
    vector<string>                _consoles;
};

__declspec(selectany) shared_ptr<platform> platform::_ins = nullptr;

}
