#include "loop.hpp"
#include "recorder.hpp"
#include "platform/element_chain.hpp"

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

bool recorder::Loop()
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
        ac = chain->Hover();
    }
    else if (GetAsyncKeyState(VK_RSHIFT) && !isWinTester)
    {
        ac = chain->Envoke();
    }
    else if (GetAsyncKeyState(VK_RMENU) && !isWinTester)
    {
        ac = chain->Menu();
    }
    else if ((bool)TestMode && GetAsyncKeyState(VK_F10) && !isWinTester)
    {
        ac = chain->Test();
    }
    else
    {
        for (char c : InputKeys)
        {
            if (GetAsyncKeyState(c) && !isWinTester)
            {
                ac = chain->Input(c);
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

bool recorder::LaunchApp()
{
    auto me = _ins;
    if (!me)
    {
        return false;
    }
    guard __g(me->_execute_mtx);

    string str = GlobalInfo::I()->app_to_launch;
    auto hIns = ShellExecuteA(NULL, "open", str.c_str(), NULL, NULL, SW_SHOWNORMAL);
    if ((LONGLONG)hIns > 32)
    {
        auto ac = xref<action>::x();
        ac->type = APP_LAUNCH;
        ac->AddParameter("path", str);
        me->_actions.Add(ac);
        return true;
    }


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
                    sp->Loop();
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
        action ac;

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
