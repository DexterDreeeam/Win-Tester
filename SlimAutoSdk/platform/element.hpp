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
    element(shared_ptr<platform> p, int sub_idx, int depth = -1, IUIAutomationElement* e = nullptr);

    element(const element& rhs) = delete;

    ~element()
    {
        Rels(_elm);
    }

    bool Valid() const
    {
        return _elm != nullptr;
    }

    bool Invalid() const
    {
        return !Valid();
    }

    bool LoadSub(int depth, int depth_remain = -1);

    int SubCount() const
    {
        return (int)_subs.size();
    }

    shared_ptr<element> Sub(int idx)
    {
        if (idx < 0 || idx >= _subs.size())
        {
            return nullptr;
        }
        return _subs[idx];
    }

    int SubIdx() const
    {
        return _sub_idx;
    }

    int Depth() const
    {
        return _depth;
    }

    area Area() const
    {
        return _area;
    }

    bool LoadProperty();

    wstring Proerty(const string& key)
    {
        if (_property.count(key))
        {
            return _property[key];
        }
        return L"";
    }

private:

private:
    shared_ptr<platform>        _plt;
    int                         _sub_idx;
    int                         _depth;
    IUIAutomationElement*       _elm;
    vector<shared_ptr<element>> _subs;
    area                        _area;
    map<string, wstring>        _property;
};

template<typename Fn>
void IterateElement(shared_ptr<element> elm, Fn f)
{
    f(elm);
    for (int i = 0; i < elm->SubCount(); ++i)
    {
        IterateElement(elm->Sub(i), f);
    }
}

}
