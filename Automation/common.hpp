#pragma once

#include <windows.h>
#include <uiautomation.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

template<typename T>
void _Free(T*& p)
{
    if (p)
    {
        p->Release();
        p = nullptr;
    }
}

inline std::wstring ToString(BSTR bstr)
{
    return std::wstring(bstr, SysStringLen(bstr));
}

struct ElePropertyDesc
{
    long            id;
    VARENUM         ty;
    std::string     name;
};

struct EleProperty
{
    std::string   name;
    std::wstring  value;
};

class EleProperties : public std::vector<EleProperty>
{
};

const ElePropertyDesc _ElePropertyDescs[] =
{
    // UIA_RuntimeIdPropertyId,
    // { UIA_BoundingRectanglePropertyId, VT_R8 , "BoundingRectangle" },
    // UIA_ProcessIdPropertyId,
    { UIA_ControlTypePropertyId, VT_I4, "ControlType" },
    // UIA_LocalizedControlTypePropertyId,
    { UIA_NamePropertyId, VT_BSTR, "Name" },
    // UIA_AcceleratorKeyPropertyId,
    { UIA_AccessKeyPropertyId, VT_BSTR, "AccessKey" },
    // UIA_HasKeyboardFocusPropertyId,
    // UIA_IsKeyboardFocusablePropertyId,
    // UIA_IsEnabledPropertyId,
    { UIA_AutomationIdPropertyId, VT_BSTR, "AutomationId" },
    { UIA_ClassNamePropertyId, VT_BSTR, "ClassName" },
    // UIA_HelpTextPropertyId,
    // UIA_ClickablePointPropertyId,
    // UIA_CulturePropertyId,
    // UIA_IsControlElementPropertyId,
    // UIA_IsContentElementPropertyId,
    // UIA_LabeledByPropertyId,
    // UIA_IsPasswordPropertyId,
    // UIA_NativeWindowHandlePropertyId,
    // UIA_ItemTypePropertyId,
    // UIA_IsOffscreenPropertyId,
    // UIA_OrientationPropertyId,
    // UIA_FrameworkIdPropertyId,
    // UIA_IsRequiredForFormPropertyId,
    // UIA_ItemStatusPropertyId,
    // UIA_IsDockPatternAvailablePropertyId,
    // UIA_IsExpandCollapsePatternAvailablePropertyId,
    // UIA_IsGridItemPatternAvailablePropertyId,
    // UIA_IsGridPatternAvailablePropertyId,
    // UIA_IsInvokePatternAvailablePropertyId,
    // UIA_IsMultipleViewPatternAvailablePropertyId,
    // UIA_IsRangeValuePatternAvailablePropertyId,
    // UIA_IsScrollPatternAvailablePropertyId,
    // UIA_IsScrollItemPatternAvailablePropertyId,
    // UIA_IsSelectionItemPatternAvailablePropertyId,
    // UIA_IsSelectionPatternAvailablePropertyId,
    // UIA_IsTablePatternAvailablePropertyId,
    // UIA_IsTableItemPatternAvailablePropertyId,
    // UIA_IsTextPatternAvailablePropertyId,
    // UIA_IsTogglePatternAvailablePropertyId,
    // UIA_IsTransformPatternAvailablePropertyId,
    // UIA_IsValuePatternAvailablePropertyId,
    // UIA_IsWindowPatternAvailablePropertyId,
    { UIA_ValueValuePropertyId, VT_BSTR, "ValueValue" },
    // UIA_ValueIsReadOnlyPropertyId,
    // UIA_RangeValueValuePropertyId,
    // UIA_RangeValueIsReadOnlyPropertyId,
    // UIA_RangeValueMinimumPropertyId,
    // UIA_RangeValueMaximumPropertyId,
    // UIA_RangeValueLargeChangePropertyId,
    // UIA_RangeValueSmallChangePropertyId,
    // UIA_ScrollHorizontalScrollPercentPropertyId,
    // UIA_ScrollHorizontalViewSizePropertyId,
    // UIA_ScrollVerticalScrollPercentPropertyId,
    // UIA_ScrollVerticalViewSizePropertyId,
    // UIA_ScrollHorizontallyScrollablePropertyId,
    // UIA_ScrollVerticallyScrollablePropertyId,
    // UIA_SelectionSelectionPropertyId,
    // UIA_SelectionCanSelectMultiplePropertyId,
    // UIA_SelectionIsSelectionRequiredPropertyId,
    { UIA_GridRowCountPropertyId, VT_I4, "GridRowCount" },
    { UIA_GridColumnCountPropertyId, VT_I4, "GridColumnCount" },
    { UIA_GridItemRowPropertyId, VT_I4, "GridItemRow" },
    { UIA_GridItemColumnPropertyId, VT_I4, "GridItemColumn" },
    // UIA_GridItemRowSpanPropertyId,
    // UIA_GridItemColumnSpanPropertyId,
    // UIA_GridItemContainingGridPropertyId,
    // UIA_DockDockPositionPropertyId,
    // UIA_ExpandCollapseExpandCollapseStatePropertyId,
    // UIA_MultipleViewCurrentViewPropertyId,
    // UIA_MultipleViewSupportedViewsPropertyId,
    // UIA_WindowCanMaximizePropertyId,
    // UIA_WindowCanMinimizePropertyId,
    // UIA_WindowWindowVisualStatePropertyId,
    // UIA_WindowWindowInteractionStatePropertyId,
    // UIA_WindowIsModalPropertyId,
    // UIA_WindowIsTopmostPropertyId,
    // UIA_SelectionItemIsSelectedPropertyId,
    // UIA_SelectionItemSelectionContainerPropertyId,
    // UIA_TableRowHeadersPropertyId,
    // UIA_TableColumnHeadersPropertyId,
    // UIA_TableRowOrColumnMajorPropertyId,
    // UIA_TableItemRowHeaderItemsPropertyId,
    // UIA_TableItemColumnHeaderItemsPropertyId,
    // UIA_ToggleToggleStatePropertyId,
    // UIA_TransformCanMovePropertyId,
    // UIA_TransformCanResizePropertyId,
    // UIA_TransformCanRotatePropertyId,
    // UIA_IsLegacyIAccessiblePatternAvailablePropertyId,
    // UIA_LegacyIAccessibleChildIdPropertyId,
    // UIA_LegacyIAccessibleNamePropertyId,
    // UIA_LegacyIAccessibleValuePropertyId,
    // UIA_LegacyIAccessibleDescriptionPropertyId,
    // UIA_LegacyIAccessibleRolePropertyId,
    // UIA_LegacyIAccessibleStatePropertyId,
    // UIA_LegacyIAccessibleHelpPropertyId,
    // UIA_LegacyIAccessibleKeyboardShortcutPropertyId,
    // UIA_LegacyIAccessibleSelectionPropertyId,
    // UIA_LegacyIAccessibleDefaultActionPropertyId,
    // UIA_AriaRolePropertyId,
    // UIA_AriaPropertiesPropertyId,
    // UIA_IsDataValidForFormPropertyId,
    // UIA_ControllerForPropertyId,
    // UIA_DescribedByPropertyId,
    // UIA_FlowsToPropertyId,
    // UIA_ProviderDescriptionPropertyId,
    // UIA_IsItemContainerPatternAvailablePropertyId,
    // UIA_IsVirtualizedItemPatternAvailablePropertyId,
    // UIA_IsSynchronizedInputPatternAvailablePropertyId,
    // UIA_OptimizeForVisualContentPropertyId,
    // UIA_IsObjectModelPatternAvailablePropertyId,
    // UIA_AnnotationAnnotationTypeIdPropertyId,
    // UIA_AnnotationAnnotationTypeNamePropertyId,
    // UIA_AnnotationAuthorPropertyId,
    // UIA_AnnotationDateTimePropertyId,
    // UIA_AnnotationTargetPropertyId,
    // UIA_IsAnnotationPatternAvailablePropertyId,
    // UIA_IsTextPattern2AvailablePropertyId,
    // UIA_StylesStyleIdPropertyId,
    // UIA_StylesStyleNamePropertyId,
    // UIA_StylesFillColorPropertyId,
    // UIA_StylesFillPatternStylePropertyId,
    // UIA_StylesShapePropertyId,
    // UIA_StylesFillPatternColorPropertyId,
    // UIA_StylesExtendedPropertiesPropertyId,
    // UIA_IsStylesPatternAvailablePropertyId,
    // UIA_IsSpreadsheetPatternAvailablePropertyId,
    // UIA_SpreadsheetItemFormulaPropertyId,
    // UIA_SpreadsheetItemAnnotationObjectsPropertyId,
    // UIA_SpreadsheetItemAnnotationTypesPropertyId,
    // UIA_IsSpreadsheetItemPatternAvailablePropertyId,
    // UIA_Transform2CanZoomPropertyId,
    // UIA_IsTransformPattern2AvailablePropertyId,
    // UIA_LiveSettingPropertyId,
    // UIA_IsTextChildPatternAvailablePropertyId,
    // UIA_IsDragPatternAvailablePropertyId,
    // UIA_DragIsGrabbedPropertyId,
    // UIA_DragDropEffectPropertyId,
    // UIA_DragDropEffectsPropertyId,
    // UIA_IsDropTargetPatternAvailablePropertyId,
    // UIA_DropTargetDropTargetEffectPropertyId,
    // UIA_DropTargetDropTargetEffectsPropertyId,
    // UIA_DragGrabbedItemsPropertyId,
    // UIA_Transform2ZoomLevelPropertyId,
    // UIA_Transform2ZoomMinimumPropertyId,
    // UIA_Transform2ZoomMaximumPropertyId,
    // UIA_FlowsFromPropertyId,
    // UIA_IsTextEditPatternAvailablePropertyId,
    // UIA_IsPeripheralPropertyId,
    // UIA_IsCustomNavigationPatternAvailablePropertyId,
    // UIA_PositionInSetPropertyId,
    // UIA_SizeOfSetPropertyId,
    // UIA_LevelPropertyId,
    // UIA_AnnotationTypesPropertyId,
    // UIA_AnnotationObjectsPropertyId,
    // UIA_LandmarkTypePropertyId,
    // UIA_LocalizedLandmarkTypePropertyId,
    // UIA_FullDescriptionPropertyId,
    // UIA_FillColorPropertyId,
    // UIA_OutlineColorPropertyId,
    // UIA_FillTypePropertyId,
    // UIA_VisualEffectsPropertyId,
    // UIA_OutlineThicknessPropertyId,
    // UIA_CenterPointPropertyId,
    // UIA_RotationPropertyId,
    // UIA_SizePropertyId,
    // UIA_IsSelectionPattern2AvailablePropertyId,
    // UIA_Selection2FirstSelectedItemPropertyId,
    // UIA_Selection2LastSelectedItemPropertyId,
    // UIA_Selection2CurrentSelectedItemPropertyId,
    // UIA_Selection2ItemCountPropertyId,
    // UIA_HeadingLevelPropertyId,
    // UIA_IsDialogPropertyId
};
