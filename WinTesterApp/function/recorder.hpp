#include "common.hpp"
#include "platform/action.hpp"

namespace slim
{

static char const InputKeys[] =
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z',
    ' ', '\b',
};

class recorder : public xref<recorder>
{
public:
    recorder() :
        _mouse_hk(nullptr),
        _key_hk(nullptr),
        _actions(),
        _thrd(),
        _stop(false),
        _execute_mtx()
    {
    }

    ~recorder();

    static int GetWaitMs();

    static bool HasCases()
    {
        auto ins = _ins;
        return ins && ins->_actions._va.size() > 0;
    }

    static bool Export();

    static bool Start()
    {
        if (GlobalInfo::I()->Change(IDLE, IDLE_TO_RECORDING))
        {
            _ins = xref<recorder>::x();
            return _ins->_Start();
        }
        else
        {
            return false;
        }
    }

    static bool Stop()
    {
        auto ins = _ins;
        if (ins)
        {
            if (ins->_Stop())
            {
                GlobalInfo::I()->record_finished = true;
                return true;
            }
        }
        return false;
    }

    static bool Iterate();

    static bool LaunchAppPath();

    static bool LaunchAppId();

    static bool LoopBegin();

    static bool LoopEnd();

    static string Report();

private:
    bool _Start();

    bool _Stop();

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
    thread            _thrd;
    bool              _stop;
    mutex             _execute_mtx;
};

__declspec(selectany) shared_ptr<recorder> recorder::_ins = nullptr;

}
