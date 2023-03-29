#include "loop.hpp"
#include "recorder.hpp"

namespace slim
{

recorder::~recorder()
{
}

void recorder::Loop()
{
    auto me = _ins;
    if (!me)
    {
        return;
    }

    auto chain = GlobalInfo::I()->chain;
    if (!chain)
    {
        return;
    }

    if (!GlobalInfo::I()->recording)
    {
        return;
    }

    shared_ptr<action> ac = nullptr;
    if (GetAsyncKeyState(VK_RCONTROL))
    {
        ac = chain->Hover();
    }
    else if (GetAsyncKeyState(VK_RSHIFT))
    {
        ac = chain->Envoke();
    }
    else if (GetAsyncKeyState(VK_RMENU))
    {
        ac = chain->Menu();
    }
    else if (GetAsyncKeyState(VK_F10))
    {
        ac = chain->Test();
    }
    else
    {
        for (char c : InputKeys)
        {
            if (GetAsyncKeyState(c))
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

void recorder::_StartRecord()
{
    _HookInput();
}

void recorder::_StopRecord()
{
    if (_mouse_hk)
    {
        UnhookWindowsHookEx(_mouse_hk);
        _mouse_hk = nullptr;
    }

    if (_key_hk)
    {
        UnhookWindowsHookEx(_key_hk);
        _key_hk = nullptr;
    }
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
