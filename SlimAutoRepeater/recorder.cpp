#include "loop.hpp"
#include "recorder.hpp"

namespace slim
{

recorder::~recorder()
{
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
        action ac = {};
        if (wParam == WM_LBUTTONDOWN)
        {
            ac.type = action_type::LEFT_CLICK;
        }
        else
        {
            ac.type = action_type::RIGHT_CLICK;
        }
        ac.element_id = chain->Identifier();
        _ins->_actions.push_back(ac);
    }

    return CallNextHookEx(_ins->_mouse_hk, nCode, wParam, lParam);
}

LRESULT __stdcall recorder::_HookKeyCb(int nCode, WPARAM wParam, LPARAM lParam)
{
    auto chain = GlobalInfo::I()->chain;

    if (chain)
    {
        chain->test();
    }

    return CallNextHookEx(_ins->_key_hk, nCode, wParam, lParam);
}

void recorder::_HookInput()
{
    // _mouse_hk = SetWindowsHookExA(WH_MOUSE, _HookMouseCb, NULL, 0);
    _key_hk = SetWindowsHookExA(WH_KEYBOARD_LL, _HookKeyCb, NULL, 0);
}

}
