#include <windows.h>
#include <uiautomation.h>

#include <string>
#include <vector>

template<typename T>
void _Free(T*& p)
{
    if (p)
    {
        p->Release();
        p = nullptr;
    }
}

class _Ele
{
    IUIAutomationElement*  p;
    std::vector<_Ele>      childs;
    bool                   childs_got;

public:
    _Ele(IUIAutomationElement* pNative = nullptr) :
        p(pNative),
        childs(),
        childs_got(false)
    {
        LoadChilds();
    }

    _Ele(const _Ele& e):
        p(e.p),
        childs(e.childs),
        childs_got(false)
    {
        p->AddRef();
        LoadChilds();
    }

    IUIAutomationElement* operator ->()
    {
        return p;
    }

    operator IUIAutomationElement** ()
    {
        return &p;
    }

    ~_Ele()
    {
        _Free(p);
    }

    void LoadChilds()
    {
        if (!childs_got && p)
        {
            IUIAutomationElementArray* arr = nullptr;
            auto hr = p->GetCachedChildren(&arr);
            if (arr)
            {
                int len = 0;
                arr->get_Length(&len);
                if (len > 0)
                {
                    for (int i = 0; i < len; ++i)
                    {
                        _Ele e;
                        arr->GetElement(0, e);
                        e.LoadChilds();
                        childs.push_back(e);
                    }
                }
            }
            childs_got = true;
        }
    }
};

class _EleProperties
{
public:
    std::vector<_EleProperty> ps;
};

struct _EleProperty
{
    long            id;
    std::string     name;
    int             value;
};

const long _ElePropertyIds[] =
{
    UIA_RuntimeIdPropertyId,
    UIA_BoundingRectanglePropertyId,
    UIA_ProcessIdPropertyId,
    UIA_ControlTypePropertyId,
    UIA_LocalizedControlTypePropertyId,
    UIA_NamePropertyId,
    UIA_AcceleratorKeyPropertyId,
    UIA_AccessKeyPropertyId,
    UIA_HasKeyboardFocusPropertyId,
    UIA_IsKeyboardFocusablePropertyId,
    UIA_IsEnabledPropertyId,
    UIA_AutomationIdPropertyId,
    UIA_ClassNamePropertyId,
    UIA_HelpTextPropertyId,
    UIA_ClickablePointPropertyId,
    UIA_CulturePropertyId,
    UIA_IsControlElementPropertyId,
    UIA_IsContentElementPropertyId,
    UIA_LabeledByPropertyId,
    UIA_IsPasswordPropertyId,
    UIA_NativeWindowHandlePropertyId,
    UIA_ItemTypePropertyId,
    UIA_IsOffscreenPropertyId,
    UIA_OrientationPropertyId,
    UIA_FrameworkIdPropertyId,
    UIA_IsRequiredForFormPropertyId,
    UIA_ItemStatusPropertyId,
    UIA_IsDockPatternAvailablePropertyId,
    UIA_IsExpandCollapsePatternAvailablePropertyId,
    UIA_IsGridItemPatternAvailablePropertyId,
    UIA_IsGridPatternAvailablePropertyId,
    UIA_IsInvokePatternAvailablePropertyId,
    UIA_IsMultipleViewPatternAvailablePropertyId,
    UIA_IsRangeValuePatternAvailablePropertyId,
    UIA_IsScrollPatternAvailablePropertyId,
    UIA_IsScrollItemPatternAvailablePropertyId,
    UIA_IsSelectionItemPatternAvailablePropertyId,
    UIA_IsSelectionPatternAvailablePropertyId,
    UIA_IsTablePatternAvailablePropertyId,
    UIA_IsTableItemPatternAvailablePropertyId,
    UIA_IsTextPatternAvailablePropertyId,
    UIA_IsTogglePatternAvailablePropertyId,
    UIA_IsTransformPatternAvailablePropertyId,
    UIA_IsValuePatternAvailablePropertyId,
    UIA_IsWindowPatternAvailablePropertyId,
    UIA_ValueValuePropertyId,
    UIA_ValueIsReadOnlyPropertyId,
    UIA_RangeValueValuePropertyId,
    UIA_RangeValueIsReadOnlyPropertyId,
    UIA_RangeValueMinimumPropertyId,
    UIA_RangeValueMaximumPropertyId,
    UIA_RangeValueLargeChangePropertyId,
    UIA_RangeValueSmallChangePropertyId,
    UIA_ScrollHorizontalScrollPercentPropertyId,
    UIA_ScrollHorizontalViewSizePropertyId,
    UIA_ScrollVerticalScrollPercentPropertyId,
    UIA_ScrollVerticalViewSizePropertyId,
    UIA_ScrollHorizontallyScrollablePropertyId,
    UIA_ScrollVerticallyScrollablePropertyId,
    UIA_SelectionSelectionPropertyId,
    UIA_SelectionCanSelectMultiplePropertyId,
    UIA_SelectionIsSelectionRequiredPropertyId,
    UIA_GridRowCountPropertyId,
    UIA_GridColumnCountPropertyId,
    UIA_GridItemRowPropertyId,
    UIA_GridItemColumnPropertyId,
    UIA_GridItemRowSpanPropertyId,
    UIA_GridItemColumnSpanPropertyId,
    UIA_GridItemContainingGridPropertyId,
    UIA_DockDockPositionPropertyId,
    UIA_ExpandCollapseExpandCollapseStatePropertyId,
    UIA_MultipleViewCurrentViewPropertyId,
    UIA_MultipleViewSupportedViewsPropertyId,
    UIA_WindowCanMaximizePropertyId,
    UIA_WindowCanMinimizePropertyId,
    UIA_WindowWindowVisualStatePropertyId,
    UIA_WindowWindowInteractionStatePropertyId,
    UIA_WindowIsModalPropertyId,
    UIA_WindowIsTopmostPropertyId,
    UIA_SelectionItemIsSelectedPropertyId,
    UIA_SelectionItemSelectionContainerPropertyId,
    UIA_TableRowHeadersPropertyId,
    UIA_TableColumnHeadersPropertyId,
    UIA_TableRowOrColumnMajorPropertyId,
    UIA_TableItemRowHeaderItemsPropertyId,
    UIA_TableItemColumnHeaderItemsPropertyId,
    UIA_ToggleToggleStatePropertyId,
    UIA_TransformCanMovePropertyId,
    UIA_TransformCanResizePropertyId,
    UIA_TransformCanRotatePropertyId,
    UIA_IsLegacyIAccessiblePatternAvailablePropertyId,
    UIA_LegacyIAccessibleChildIdPropertyId,
    UIA_LegacyIAccessibleNamePropertyId,
    UIA_LegacyIAccessibleValuePropertyId,
    UIA_LegacyIAccessibleDescriptionPropertyId,
    UIA_LegacyIAccessibleRolePropertyId,
    UIA_LegacyIAccessibleStatePropertyId,
    UIA_LegacyIAccessibleHelpPropertyId,
    UIA_LegacyIAccessibleKeyboardShortcutPropertyId,
    UIA_LegacyIAccessibleSelectionPropertyId,
    UIA_LegacyIAccessibleDefaultActionPropertyId,
    UIA_AriaRolePropertyId,
    UIA_AriaPropertiesPropertyId,
    UIA_IsDataValidForFormPropertyId,
    UIA_ControllerForPropertyId,
    UIA_DescribedByPropertyId,
    UIA_FlowsToPropertyId,
    UIA_ProviderDescriptionPropertyId,
    UIA_IsItemContainerPatternAvailablePropertyId,
    UIA_IsVirtualizedItemPatternAvailablePropertyId,
    UIA_IsSynchronizedInputPatternAvailablePropertyId,
    UIA_OptimizeForVisualContentPropertyId,
    UIA_IsObjectModelPatternAvailablePropertyId,
    UIA_AnnotationAnnotationTypeIdPropertyId,
    UIA_AnnotationAnnotationTypeNamePropertyId,
    UIA_AnnotationAuthorPropertyId,
    UIA_AnnotationDateTimePropertyId,
    UIA_AnnotationTargetPropertyId,
    UIA_IsAnnotationPatternAvailablePropertyId,
    UIA_IsTextPattern2AvailablePropertyId,
    UIA_StylesStyleIdPropertyId,
    UIA_StylesStyleNamePropertyId,
    UIA_StylesFillColorPropertyId,
    UIA_StylesFillPatternStylePropertyId,
    UIA_StylesShapePropertyId,
    UIA_StylesFillPatternColorPropertyId,
    UIA_StylesExtendedPropertiesPropertyId,
    UIA_IsStylesPatternAvailablePropertyId,
    UIA_IsSpreadsheetPatternAvailablePropertyId,
    UIA_SpreadsheetItemFormulaPropertyId,
    UIA_SpreadsheetItemAnnotationObjectsPropertyId,
    UIA_SpreadsheetItemAnnotationTypesPropertyId,
    UIA_IsSpreadsheetItemPatternAvailablePropertyId,
    UIA_Transform2CanZoomPropertyId,
    UIA_IsTransformPattern2AvailablePropertyId,
    UIA_LiveSettingPropertyId,
    UIA_IsTextChildPatternAvailablePropertyId,
    UIA_IsDragPatternAvailablePropertyId,
    UIA_DragIsGrabbedPropertyId,
    UIA_DragDropEffectPropertyId,
    UIA_DragDropEffectsPropertyId,
    UIA_IsDropTargetPatternAvailablePropertyId,
    UIA_DropTargetDropTargetEffectPropertyId,
    UIA_DropTargetDropTargetEffectsPropertyId,
    UIA_DragGrabbedItemsPropertyId,
    UIA_Transform2ZoomLevelPropertyId,
    UIA_Transform2ZoomMinimumPropertyId,
    UIA_Transform2ZoomMaximumPropertyId,
    UIA_FlowsFromPropertyId,
    UIA_IsTextEditPatternAvailablePropertyId,
    UIA_IsPeripheralPropertyId,
    UIA_IsCustomNavigationPatternAvailablePropertyId,
    UIA_PositionInSetPropertyId,
    UIA_SizeOfSetPropertyId,
    UIA_LevelPropertyId,
    UIA_AnnotationTypesPropertyId,
    UIA_AnnotationObjectsPropertyId,
    UIA_LandmarkTypePropertyId,
    UIA_LocalizedLandmarkTypePropertyId,
    UIA_FullDescriptionPropertyId,
    UIA_FillColorPropertyId,
    UIA_OutlineColorPropertyId,
    UIA_FillTypePropertyId,
    UIA_VisualEffectsPropertyId,
    UIA_OutlineThicknessPropertyId,
    UIA_CenterPointPropertyId,
    UIA_RotationPropertyId,
    UIA_SizePropertyId,
    UIA_IsSelectionPattern2AvailablePropertyId,
    UIA_Selection2FirstSelectedItemPropertyId,
    UIA_Selection2LastSelectedItemPropertyId,
    UIA_Selection2CurrentSelectedItemPropertyId,
    UIA_Selection2ItemCountPropertyId,
    UIA_HeadingLevelPropertyId,
    UIA_IsDialogPropertyId
};

int main()
{
    HRESULT hr;
    IUIAutomation* pAuto = nullptr;
    _Ele e;

    hr = CoInitialize(NULL);
    hr = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pAuto));
    // hr = pAuto->GetRootElement(e);

    while (1)
    {
        POINT pt;
        IUIAutomationElement* ele;

        Sleep(2000);
        hr = GetCursorPos(&pt);
        hr = pAuto->ElementFromPoint(pt, &ele);
        ele->GetCurrentPropertyValue
        ele = nullptr;
    }


    return 0;
}
