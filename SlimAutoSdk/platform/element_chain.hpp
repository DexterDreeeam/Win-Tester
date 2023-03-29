#pragma once

#include "common.hpp"

namespace slim
{

class element;
class action;

class element_chain
{
    friend class action;

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

    shared_ptr<action> Hover(); // cursor hover

    shared_ptr<action> Envoke(); // cursor left click

    shared_ptr<action> Menu(); // cursor right click

    shared_ptr<action> Input(char k); // input a Key down

    shared_ptr<action> Test();

private:
    string                       _window;
    string                       _class;
    vector<shared_ptr<element>>  _ve; // from child to root
};

}
