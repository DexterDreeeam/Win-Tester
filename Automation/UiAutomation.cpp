//#include "common.hpp"
//
//void UiAutomationTest(EleProperties& props)
//{
//    HRESULT hr;
//    IUIAutomation* pAuto = nullptr;
//    hr = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pAuto));
//
//    POINT pt;
//    IUIAutomationElement* ele = nullptr;
//    if (!GetCursorPos(&pt))
//    {
//        return;
//    }
//
//    hr = pAuto->ElementFromPoint(pt, &ele);
//    // hr = pAuto->GetFocusedElement(&ele);
//    if (ele)
//    {
//        EleProperty prop;
//        std::wstring value;
//        for (auto desc : _ElePropertyDescs)
//        {
//            VARIANT v;
//            hr = ele->GetCurrentPropertyValue(desc.id, &v);
//            if (FAILED(hr))
//            {
//                continue;
//            }
//
//            value = std::wstring();
//            switch (desc.ty)
//            {
//            case VT_BSTR:
//                value = ToString(v.bstrVal);
//                break;
//            case VT_I4:
//                prop.value = std::to_wstring(v.intVal);
//                break;
//            case VT_R8:
//                break;
//            default:
//                break;
//            }
//
//            if (value.size() > 0)
//            {
//                prop.name = desc.name;
//                prop.value = value;
//                props.push_back(prop);
//            }
//        }
//    }
//
//    _Free(ele);
//    _Free(pAuto);
//}
