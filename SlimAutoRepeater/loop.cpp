#include "loop.hpp"

namespace slim
{

void Dbg(shared_ptr<element> root)
{
    logger l;

    IterateElement(
        root,
        [&](auto e)
        {
            e->LoadProperty();
            string space = "";
            for (int i = 0; i < e->Depth(); ++i)
            {
                space += "  ";
            }
            l.w(space);
            l.w(e->Identifier());
            l.w("-[" + to_string(e->Area().left) + ", " + to_string(e->Area().right) + "]");
            l.w("-[" + to_string(e->Area().top) + ", " + to_string(e->Area().bottom) + "]");
            l.w_endl();
        });
}

bool SlimLoop()
{
    try
    {
        POINT pt;
        GetCursorPos(&pt);

        auto elementChain =  platform::I()->ElementChainByPoint(point(pt));
        GlobalInfo::I()->chain = elementChain;
        return elementChain ? true : false;
    }
    catch (...)
    {
        return false;
    }
}

}
