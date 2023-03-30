#pragma once

#include <windows.h>
#include <UIAutomation.h>
#include <UIAutomationClient.h>

#include <iostream>
#include <format>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <mutex>
#include <thread>
#include <algorithm>
#include <fstream>
#include <locale>
#include <math.h>

using namespace std;

#include "utils/dbg.hpp"
#include "utils/log.hpp"
#include "utils/escape.hpp"
#include "utils/str.hpp"

#define Tell(e)         { if (FAILED(hr)) { throw e; } }
#define Fail(ret,log)   { if (FAILED(hr)) { log; return ret; } }
#define Rels(ptr)       { if (ptr) { ptr->Release(); ptr = nullptr; } }

template<typename Ty>
class xref
{
public:
    template<typename ...Tys>
    static shared_ptr<Ty> x(Tys... tys)
    {
        auto sp = make_shared<Ty>(tys...);
        weak_ptr<Ty> wp = sp;
        sp->_ref_init(wp);
        return sp;
    }

    void _ref_init(weak_ptr<Ty> _self)
    {
        this->_self = _self;
    }

protected:
    weak_ptr<Ty> _self;
};

using guard = lock_guard<mutex>;
