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

    bool Interact() const
    {
        return _interact;
    }

    bool LoadProperty();

    string Property(const string& key)
    {
        if (_property.count(key))
        {
            return _property[key];
        }
        return "";
    }

    double InteractGrade(POINT pt)
    {
        return InteractGrade(point(pt));
    }

    double InteractGrade(point p);

    string Identifier();

    string FriendlyIdentifier();

    bool Envoke();

    bool test();

private:
    shared_ptr<platform>         _plt;
    int                          _sub_idx;
    int                          _depth;
    IUIAutomationElement*        _elm;
    vector<shared_ptr<element>>  _subs;
    area                         _area;
    map<string, string>          _property;
    bool                         _property_loaded;
    bool                         _interact;
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

__declspec(selectany) set<CONTROLTYPEID> interact_ct =
{
    UIA_ButtonControlTypeId,
    //UIA_CalendarControlTypeId,
    UIA_CheckBoxControlTypeId,
    UIA_ComboBoxControlTypeId,
    UIA_EditControlTypeId,
    UIA_HyperlinkControlTypeId,
    UIA_ImageControlTypeId,
    UIA_ListItemControlTypeId,
    //UIA_ListControlTypeId,
    //UIA_MenuControlTypeId,
    //UIA_MenuBarControlTypeId,
    UIA_MenuItemControlTypeId,
    //UIA_ProgressBarControlTypeId,
    UIA_RadioButtonControlTypeId,
    //UIA_ScrollBarControlTypeId,
    //UIA_SliderControlTypeId,
    //UIA_SpinnerControlTypeId,
    //UIA_StatusBarControlTypeId,
    //UIA_TabControlTypeId,
    UIA_TabItemControlTypeId,
    UIA_TextControlTypeId,
    //UIA_ToolBarControlTypeId,
    //UIA_ToolTipControlTypeId,
    //UIA_TreeControlTypeId,
    UIA_TreeItemControlTypeId,
    //UIA_CustomControlTypeId,
    //UIA_GroupControlTypeId,
    //UIA_ThumbControlTypeId,
    //UIA_DataGridControlTypeId,
    UIA_DataItemControlTypeId,
    //UIA_DocumentControlTypeId,
    UIA_SplitButtonControlTypeId,
    //UIA_WindowControlTypeId,
    //UIA_PaneControlTypeId,
    //UIA_HeaderControlTypeId,
    UIA_HeaderItemControlTypeId,
    //UIA_TableControlTypeId,
    //UIA_TitleBarControlTypeId,
    //UIA_SeparatorControlTypeId,
    //UIA_SemanticZoomControlTypeId,
    //UIA_AppBarControlTypeId,
};

}
