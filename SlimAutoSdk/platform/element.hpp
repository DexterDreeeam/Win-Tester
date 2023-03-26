#pragma once

#include "common.hpp"
#include "area.hpp"

namespace slim
{

class platform;

class element
{
public:
    friend struct iter;

public:
    element(shared_ptr<platform> p, IUIAutomationElement* e = nullptr) :
        plt(p),
        elm(e),
        sub()
    {
    }

    ~element()
    {
        Rels(elm);
    }

    bool Valid() const
    {
        return elm != nullptr;
    }

    bool Invalid() const
    {
        return !Valid();
    }

    bool LoadSub(int depth = -1);

    int SubCount() const
    {
        return sub.size();
    }

    shared_ptr<element> Sub(int idx)
    {
        if (idx < 0 || idx >= sub.size())
        {
            return nullptr;
        }
        return sub[idx];
    }

    area Area();

private:

private:
    shared_ptr<platform>        plt;
    IUIAutomationElement*       elm;
    vector<shared_ptr<element>> sub;
};

}
