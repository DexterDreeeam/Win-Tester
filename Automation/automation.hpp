#pragma once

#include "common.hpp"

enum ImplementType
{
    UiA,
    Msaa
};

struct ElementProperty
{
    std::string  name;
    std::wstring value;
};

struct ElementRect
{
    int left;
    int top;
    int right;
    int bottom;

    bool operator ==(const ElementRect& rhs) const
    {
        return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom;
    }
};

class Element
{
public:
    virtual ~Element() = default;

    virtual ImplementType WinImplementType() = 0;

    virtual int CountProperty() = 0;

    virtual ElementProperty GetProperty(const std::string& name) = 0;

    virtual ElementProperty GetProperty(int index) = 0;

    virtual void SetProperty(const ElementProperty& prop, bool discardEmpty = true) = 0;

    virtual std::shared_ptr<Element> Parent() =  0;

    virtual std::wstring Path() = 0;

    virtual ElementRect Rect() = 0;
};

class Automation
{
public:
    virtual ~Automation() = default;

    virtual std::shared_ptr<Element> PointElement(int x, int y) = 0;

    virtual std::shared_ptr<Element> CursorElement() = 0;

public:
    static std::shared_ptr<Automation> Instance(ImplementType ty);
};
