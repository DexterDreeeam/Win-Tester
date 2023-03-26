#pragma once

#include "../common.hpp"
#include "../automation.hpp"

class MsaaElement : public Element
{
public:
    MsaaElement(IAccessible* acc, LONG child = CHILDID_SELF);

    MsaaElement(std::shared_ptr<MsaaElement> rhs);

    ~MsaaElement() override;

    ImplementType WinImplementType() override;

    int CountProperty() override;

    ElementProperty GetProperty(const std::string& name) override;

    ElementProperty GetProperty(int index) override;

    void SetProperty(const ElementProperty& prop, bool discardEmpty = true) override;

    std::shared_ptr<Element> Parent() override;

    std::wstring Path() override;

    ElementRect Rect() override;

private:
    VARIANT ChildVar();

    void RepresentByParent();

    std::wstring PathSection();

    bool LoadRect();

private:
    IAccessible*                            acc;
    LONG                                    child_id;
    int                                     child_idx;
    ElementRect                             rect;
    std::map<std::string, ElementProperty>  props;
};

class MsaaAutomation : public Automation
{
public:
    ~MsaaAutomation() override;

    std::shared_ptr<Element> PointElement(int x, int y) override;

    std::shared_ptr<Element> CursorElement() override;

private:
    std::shared_ptr<Element> PointElement(POINT pt);
};
