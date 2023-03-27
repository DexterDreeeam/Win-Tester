#include "common.hpp"
#include "platform/action.hpp"

namespace slim
{

class recorder
{
private:
    recorder() :
        _mouse_hk(nullptr),
        _key_hk(nullptr),
        _actions()
    {
    }

public:
    ~recorder();

    static void StartRecord()
    {
        if (_ins)
        {
            delete _ins;
            _ins = nullptr;
        }
        _ins = new recorder();
        _ins->_StartRecord();
    }

    static void StopRecord()
    {
        _ins->StopRecord();
    }

private:
    void _StartRecord();

    void _StopRecord();

public:
    static LRESULT __stdcall _HookMouseCb(int nCode, WPARAM wParam, LPARAM lParam);

    static LRESULT __stdcall _HookKeyCb(int nCode, WPARAM wParam, LPARAM lParam);

    static DWORD WINAPI _HookThread(LPVOID lpParam);

private:
    void _HookInput();

private:
    static recorder*  _ins;

    HHOOK             _mouse_hk;
    HHOOK             _key_hk;
    vector<action>    _actions;
};

__declspec(selectany) recorder* recorder::_ins = nullptr;

}
