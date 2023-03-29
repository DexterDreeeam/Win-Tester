#include "element_chain.hpp"
#include "element.hpp"

namespace slim
{

// WindowName@@ClassName  <13>      [2]             (3000)                 {ClickHere}      [ ...
//                        \_layers  \_childest idx  \_childest control id  \_childest name  \_upper element
string element_chain::Identifier()
{
    if (!Valid())
    {
        return "";
    }
    string id = "";
    id += _window + "@@" + _class;
    id += "[[" + to_string(_ve.size()) + "]]";
    for (auto e : _ve)
    {
        e->LoadProperty();
        id += e->Identifier();
    }
    return id;
}

string element_chain::FriendlyIdentifier()
{
    if (!Valid())
    {
        return "";
    }
    string id = "";
    id += _window + "\n@@\n" + _class + '\n';
    id += "[[" + to_string(_ve.size()) + "]]";
    id += '\n';
    for (auto e : _ve)
    {
        e->LoadProperty();
        id += e->FriendlyIdentifier();
        id += '\n';
    }
    return id;
}

double element_chain::Matching(const string& str)
{
    return 0;
}

bool element_chain::Envoke()
{
    return _ve.front()->Envoke();
}

bool element_chain::Menu()
{
    return _ve.front()->Menu();
}

bool element_chain::Test()
{
    return _ve.front()->Test();
}

}