#pragma once

#include "common.hpp"
#include "action.hpp"

namespace slim
{

class element;

class element_chain
{
public:
    element_chain() :
        _window(),
        _class(),
        _ve()
    {
    }

    element_chain(const string& window, const string& cls, const vector<shared_ptr<element>>& ve) :
        _window(window),
        _class(cls),
        _ve(ve)
    {
    }

    element_chain(const element_chain& rhs) :
        _window(rhs._window),
        _class(rhs._class),
        _ve(rhs._ve)
    {
    }

    ~element_chain()
    {
    }

    void Load(const string& window, const string& cls, const vector<shared_ptr<element>>& ve)
    {
        _window = window;
        _class = cls;
        _ve = ve;
    }

    bool Valid() const
    {
        return _ve.size() > 0;
    }

    string Identifier();

    string FriendlyIdentifier();

    bool test();

private:
    string                       _window;
    string                       _class;
    vector<shared_ptr<element>>  _ve; // from child to root
};

}
