#include "common.hpp"
#include "platform/action.hpp"

namespace slim
{

class recorder
{
public:
    recorder() :
        _mouse_hk(nullptr),
        _key_hk(nullptr),
        _actions()
    {
    }

    ~recorder();

    static void StartRecord()
    {
        if (_ins)
        {
            _ins = nullptr;
        }
        _ins = make_shared<recorder>();
        _ins->_StartRecord();
    }

    static void StopRecord()
    {
        _ins->_StopRecord();
    }

    static void Loop();

    static string Report();

private:
    void _StartRecord();

    void _StopRecord();

public:
    static LRESULT __stdcall _HookMouseCb(int nCode, WPARAM wParam, LPARAM lParam);

    static LRESULT __stdcall _HookKeyCb(int nCode, WPARAM wParam, LPARAM lParam);

private:
    void _HookInput();

private:
    static shared_ptr<recorder> _ins;

    HHOOK             _mouse_hk;
    HHOOK             _key_hk;
    action_set        _actions;
};

__declspec(selectany) shared_ptr<recorder> recorder::_ins = nullptr;

}
