#include "common.hpp"

void MsaaTest(EleProperties& props)
{
    HRESULT hr;

    POINT pt;
    if (!GetCursorPos(&pt))
    {
        return;
    }

    IAccessible* acc;
    VARIANT var, tmp;
    BSTR bstr;

    hr = AccessibleObjectFromPoint(pt, &acc, &var);
    tmp.vt = VT_I4;
    tmp.lVal = CHILDID_SELF;

    hr = acc->get_accName(tmp, &bstr);
    props.push_back({ "Name", ToString(bstr) });

    hr = acc->get_accValue(tmp, &bstr);
    props.push_back({ "Value", ToString(bstr) });

    _Free(acc);
}
