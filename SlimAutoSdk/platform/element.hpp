#pragma once

#include "common.hpp"
#include "area.hpp"
#include "identifier.hpp"

namespace slim
{

class platform;
class element_stack;

struct search_criteria
{
    
};

class element : public xref<element>
{
public:
    friend struct iter;

public:
    element(IUIAutomationElement* e, int depth, int prnt_idx = -1);

    element(const element& rhs) = delete;

    ~element()
    {
        Rels(_uia_e);
    }

    bool Valid() const
    {
        return _uia_e != nullptr;
    }

    bool Invalid() const
    {
        return !Valid();
    }

    void LoadSub(bool recur);

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

    double InteractGrade(POINT pt)
    {
        return InteractGrade(point(pt));
    }

    double InteractGrade(point p);

    string Identifier() const;

    string FriendlyIdentifier() const;

    bool Act(action_type actionType);

    bool Hover();

    bool Envoke();

    bool Envoke2();

    bool Envoke_();

    bool Menu();

    bool Test();

private:
    void _LoadProperty();

public:
    int                          _parent_idx;
    int                          _depth;
    area                         _area;
    string                       _name;
    string                       _auto_id;
    string                       _control_str;
    int                          _control;
    bool                         _interact;
    bool                         _dialog;

private:
    IUIAutomationElement*        _uia_e;
    vector<shared_ptr<element>>  _subs;
    bool                         _subs_loaded;
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

__declspec(selectany) vector<long> element_props =
{
    UIA_BoundingRectanglePropertyId,
    UIA_NamePropertyId,
    UIA_AutomationIdPropertyId,
    UIA_ControlTypePropertyId,
    UIA_LocalizedControlTypePropertyId,
    UIA_IsDialogPropertyId,
};

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
    UIA_CustomControlTypeId,
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
