#pragma once

#include "common.hpp"
#include "element.hpp"

namespace slim
{

class platform
{
    friend class element;

    platform() :
        ctx(nullptr),
        con(nullptr)
    {
        HRESULT hr;
        hr = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ctx));
        Tell("create platform failed");

        hr = ctx->CreateTrueCondition(&con);
        Tell("create true condition failed");
    }

public:
    static shared_ptr<platform> Instance()
    {
        return _ins ? _ins : _ins = shared_ptr<platform>(new platform());
    }

    static shared_ptr<platform> I()
    {
        return Instance();
    }

    ~platform()
    {
        Rels(con);
        Rels(ctx);
    }

    shared_ptr<element> Diagram();

private:
    static shared_ptr<platform> _ins;

private:
    IUIAutomation*           ctx;
    IUIAutomationCondition*  con;
};

__declspec(selectany) shared_ptr<platform> platform::_ins = nullptr;

}
