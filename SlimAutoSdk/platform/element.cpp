#include "platform.hpp"
#include "element.hpp"

namespace slim
{

bool element::LoadSub(int depth)
{
    if (Invalid())
    {
        return false;
    }

    HRESULT hr;
    IUIAutomationElementArray* elmArr;

    hr = elm->FindAll(TreeScope_Children, plt->con, &elmArr);
    Tell("FindAll() failed when LoadSub()");

    int len = 0;
    hr = elmArr->get_Length(&len);
    Tell("get_Length failed");

    for (int i = 0; i < len; ++i)
    {
        IUIAutomationElement* c;
        hr = elmArr->GetElement(i, &c);
        Tell("elmArr->GetElement failed");

        sub.push_back(make_shared<element>(plt, c));

        if (depth == 0)
        {
            continue;
        }

        sub.back()->LoadSub(depth == -1 ? -1 : depth - 1);
    }

    return true;
}

area element::Area()
{
    HRESULT hr;
    RECT rc;

    hr = elm->get_CurrentBoundingRectangle(&rc);
    Tell("elm->get_CurrentBoundingRectangle failed");

    return area(rc);
}

}