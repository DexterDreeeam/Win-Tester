#include "loop.hpp"
#include "recorder.hpp"
#include "platform/element_chain.hpp"
#include "utils/executor.hpp"

#ifdef _DEBUG
    #define TestMode 1
#else
    #define TestMode 0
#endif

namespace slim
{

recorder::~recorder()
{
}

int recorder::GetWaitMs()
{
    string s = GlobalInfo::I()->action_wait_for_ms;
    int num = atoi(s.c_str());
    if (num >= 0)
    {
        return num;
    }
    return GlobalInfo::I()->ResetWaitMs();
}

bool recorder::Export()
{
    WCHAR szFileName[MAX_PATH] = L"";
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"Json Files\0*.json\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"json";

    if (!GetSaveFileNameW(&ofn))
    {
        return false;
    }

    string s = Report();
    auto hF = CreateFileW(
        ofn.lpstrFile, GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hF == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwBytesWritten = 0;
    WriteFile(hF, s.c_str(), (DWORD)s.size(), &dwBytesWritten, NULL);
    CloseHandle(hF);

    return true;
}

bool recorder::Iterate()
{
    SlimLoop();

    auto me = _ins;
    auto chain = GlobalInfo::I()->chain;
    if (!me || !chain)
    {
        return false;
    }

    auto current = platform::GetCurrentWndInfo();
    auto isWinTester = HasWinTesterStr(current.cls) || HasWinTesterStr(current.win);

    guard __g(me->_execute_mtx);

    shared_ptr<action> ac = nullptr;
    if (GetAsyncKeyState(VK_RCONTROL) && !isWinTester)
    {
        ac = chain->Hover(GetWaitMs());
    }
    else if (GetAsyncKeyState(VK_RSHIFT) && !isWinTester)
    {
        ac = chain->Envoke(GetWaitMs());
    }
    else if (GetAsyncKeyState(VK_RMENU) && !isWinTester)
    {
        ac = chain->Menu(GetWaitMs());
    }
    else if ((bool)TestMode && GetAsyncKeyState(VK_F10) && !isWinTester)
    {
        ac = chain->Test(GetWaitMs());
    }
    else
    {
        for (char c : InputKeys)
        {
            if (GetAsyncKeyState(c) && !isWinTester)
            {
                ac = chain->Input(c, GetWaitMs());
                break;
            }
        }
    }

    if (ac && ac->type != action_type::ACTION_NONE && ac->type != action_type::ACTION_TEST)
    {
        me->_actions.Add(ac);
        Sleep(300);
    }

    return true;
}

bool recorder::LaunchAppPath()
{
    auto me = _ins;
    if (!me)
    {
        return false;
    }
    guard __g(me->_execute_mtx);

    string path = GlobalInfo::I()->app_to_launch;
    if (LaunchShellFilePath(path))
    {
        auto ac = xref<action>::x(GetWaitMs());
        ac->type = APP_LAUNCH_PATH;
        ac->AddParameter("path", path);
        me->_actions.Add(ac);
        return true;
    }
    return false;
}

bool recorder::LaunchAppId()
{
    auto me = _ins;
    if (!me)
    {
        return false;
    }
    guard __g(me->_execute_mtx);

    string appid = GlobalInfo::I()->app_to_launch;
    if (LaunchShellAppId(appid))
    {
        auto ac = xref<action>::x(GetWaitMs());
        ac->type = APP_LAUNCH_PATH;
        ac->AddParameter("appid", appid);
        me->_actions.Add(ac);
        return true;
    }
    return false;
}

bool recorder::LoopBegin()
{
    auto me = _ins;
    if (!me)
    {
        return false;
    }
    guard __g(me->_execute_mtx);

    string par = GlobalInfo::I()->record_loop_pars;
    vector<string> pars;
    int idx = 0;
    while (true)
    {
        auto found = par.find(",", idx);
        if (found == string::npos)
        {
            break;
        }
        pars.push_back(par.substr(idx, found - idx));
        idx = (int)found + 1;
    }
    if (idx < par.size())
    {
        pars.push_back(par.substr(idx));
    }

    bool valid = false;
    auto ac = xref<action>::x(GetWaitMs());
    ac->type = LOOP_BEGIN;
    for (const auto& p : pars)
    {
        if (p.ends_with("ms"))
        {
            ac->AddParameter("duration", p.substr(0, p.size() - 2));
            valid = true;
        }
        else if (p.ends_with("s"))
        {
            ac->AddParameter("duration", p.substr(0, p.size() - 1) + "000");
            valid = true;
        }
        else if (atoi(p.c_str()) > 0)
        {
            ac->AddParameter("times", p);
            valid = true;
        }
    }
    if (!valid)
    {
        return false;
    }

    ++GlobalInfo::I()->record_loop_stack;
    me->_actions.Add(ac);
    return true;
}

bool recorder::LoopEnd()
{
    auto me = _ins;
    if (!me)
    {
        return false;
    }
    guard __g(me->_execute_mtx);

    auto ac = xref<action>::x(GetWaitMs());
    ac->type = LOOP_END;

    --GlobalInfo::I()->record_loop_stack;
    me->_actions.Add(ac);
    return true;
}

string recorder::Report()
{
    string default_r = "[]";
    if (!_ins)
    {
        return default_r;
    }

    _ins->_actions.Sort();
    return _ins->_actions.ToString(true);
}

bool recorder::_Start()
{
    //_HookInput();
    _stop = false;
    _thrd = thread(
        [=]()
        {
            if (GlobalInfo::I()->Change(IDLE_TO_RECORDING, RECORDING) == false)
            {
                throw nullptr;
            }

            auto sp = _ins;
            while (!sp->_stop)
            {
                try
                {
                    sp->Iterate();
                }
                catch (...)
                {
                    // todo
                }
            }

            if (GlobalInfo::I()->Change(RECORDING_TO_IDLE, IDLE) == false)
            {
                throw nullptr;
            }
        }
    );
    _thrd.detach();
    return true;
}

bool recorder::_Stop()
{
    _stop = true;
    return GlobalInfo::I()->Change(RECORDING, RECORDING_TO_IDLE);

    //if (_mouse_hk)
    //{
    //    UnhookWindowsHookEx(_mouse_hk);
    //    _mouse_hk = nullptr;
    //}

    //if (_key_hk)
    //{
    //    UnhookWindowsHookEx(_key_hk);
    //    _key_hk = nullptr;
    //}
}

LRESULT __stdcall recorder::_HookMouseCb(int nCode, WPARAM wParam, LPARAM lParam)
{
    auto chain = GlobalInfo::I()->chain;
    // auto* mouseStruct = (MOUSEHOOKSTRUCT*)lParam;

    if (chain && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN))
    {
        //action ac = {};
        //if (wParam == WM_LBUTTONDOWN)
        //{
        //    ac.type = action_type::LEFT_CLICK;
        //}
        //else
        //{
        //    ac.type = action_type::RIGHT_CLICK;
        //}
        //ac.element = chain->Identifier();
        //_ins->_actions.Add(ac);
    }

    return CallNextHookEx(_ins->_mouse_hk, nCode, wParam, lParam);
}

LRESULT __stdcall recorder::_HookKeyCb(int nCode, WPARAM wParam, LPARAM lParam)
{
    auto me = _ins;
    auto chain = GlobalInfo::I()->chain;
    if (me && chain)
    {
        action ac(0);

        if (wParam >= 0x41 && wParam <= (0x41 + 'Z' - 'A'))
        {
            // char input
        }
        else if (wParam >= 0x30 && wParam <= (0x30 + '9' - '0'))
        {
            // char input
        }

        switch (wParam)
        {
        case VK_RSHIFT:
            //ac.type = action_type::LEFT_CLICK;
            //ac.element = chain->Identifier();
            //chain->Envoke();
            break;

        default:
            break;
        }

        if (ac.type != action_type::ACTION_NONE)
        {
            // me->_actions.Add(ac);
        }
    }

    return CallNextHookEx(_ins->_key_hk, nCode, wParam, lParam);
}

void recorder::_HookInput()
{
    // _mouse_hk = SetWindowsHookExA(WH_MOUSE, _HookMouseCb, NULL, 0);
    // _key_hk = SetWindowsHookExA(WH_KEYBOARD_LL, _HookKeyCb, NULL, 0);
}

}
