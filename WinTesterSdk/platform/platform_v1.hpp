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

struct TreeElementProxy
{
    shared_ptr<slim::element>  elm;
    int                        parent;
    int                        dialogs;
};

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

    static void Release() { _ins = nullptr; }

    ~platform();

    static mutex& Mutex(const string& name);

    static WndInfo GetWndInfo(HWND wnd);

    static WndInfo GetCurrentWndInfo();

    static void UpdateDesktopWnds();

    static bool UpdateDesktopWnds(const WndInfo& wndInfo);

    static vector<WndInfo> GetWnds();

    static vector<WndInfo> GetWnds(const WndInfo& info);

    static shared_ptr<element_chain> GetElementChainInActiveWindow(point p, bool fast);

    static shared_ptr<element_chain> GetElementChainInDesktop(point p, bool fast);

    static shared_ptr<element_matched> FindElementInWindow(WndInfo& wnd, const vector<element_stack>& ess);

    static shared_ptr<element_matched> FindElementInDesktop(const vector<element_stack>& ess);

    static void Test(point p);

    static vector<string> Consoles();

    static void Console(const string& c);

    static void ClearConsole();

private:
    static BOOL CALLBACK _EnumWindowsCb(HWND wnd, LPARAM par);

    static void _PermuteElementTree(vector<TreeElementProxy>& candidates, point p, bool fast);

    static void _GetElementStacks(shared_ptr<element> root, point p, bool fast, vector<shared_ptr<element>>& ve);

    static shared_ptr<element_matched> _FindElement(shared_ptr<element> self, const element_searching& searching);

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
