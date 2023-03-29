#include "loop.hpp"
#include "function/recorder.hpp"

void Dbg(shared_ptr<slim::element> root)
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

        auto elementChain = slim::platform::I()->ElementChainByPoint(slim::point(pt));
        GlobalInfo::I()->chain = elementChain;
        if (!elementChain)
        {
            return false;
        }

        slim::recorder::Loop();

        return true;
    }
    catch (...)
    {
        return false;
    }
}
