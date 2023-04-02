#pragma once

#include "common.hpp"
#include "identifier.hpp"
#include "area.hpp"

namespace slim
{

class element;
class action;

class element_chain : public xref<element_chain>
{
    friend class action;

public:
    element_chain(const WndInfo& wndInfo, const vector<shared_ptr<element>>& ve) :
        _wnd_info(wndInfo),
        _ve(ve)
    {
    }

    element_chain(const element_chain& rhs) :
        _wnd_info(rhs._wnd_info),
        _ve(rhs._ve)
    {
    }

    ~element_chain()
    {
    }

    bool Valid() const
    {
        return _ve.size() > 0;
    }

    area Area() const;

    string Identifier() const;

    string FriendlyIdentifier() const;

    shared_ptr<action> Hover(); // cursor hover

    shared_ptr<action> Envoke(); // cursor left click

    shared_ptr<action> Menu(); // cursor right click

    shared_ptr<action> Input(char k); // input a Key down

    shared_ptr<action> Test();

private:
    WndInfo                      _wnd_info;
    vector<shared_ptr<element>>  _ve; // from child to root
};

}
