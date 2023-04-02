#pragma once

#include "common.hpp"

namespace slim
{

class action;

class runner : public xref<runner>
{
public:
    runner() :
        _thrd(),
        _stop(false)
    {
    }

    ~runner()
    {
    }

    static bool Run(const string& str);

    static bool Stop();

private:
    bool _RunScope(const vector<shared_ptr<action>>& va, int start, int end);

    int _RunLoop(const vector<shared_ptr<action>>& va, int start);

    bool _Act(shared_ptr<action> ac);

    bool _ActElement(shared_ptr<action> ac);

private:
    static bool _KeyInput(char c);

private:
    static shared_ptr<runner> _ins;

    thread  _thrd;
    bool    _stop;
};

__declspec(selectany) shared_ptr<runner> runner::_ins = nullptr;

}
