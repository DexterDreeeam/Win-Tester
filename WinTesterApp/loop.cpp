#include "loop.hpp"
#include "function/recorder.hpp"
#include "function/runner.hpp"
#include "platform/element.hpp"
#include "platform/element_chain.hpp"
#include "utils/draw.hpp"

void Dbg(shared_ptr<slim::element> root)
{
    logger l;

    IterateElement(
        root,
        [&](auto e)
        {
            string space = "";
            for (int i = 0; i < e->_depth; ++i)
            {
                space += "  ";
            }
            l.w(space);
            l.w(e->Identifier());
            l.w("-[" + to_string(e->_area.left) + ", " + to_string(e->_area.right) + "]");
            l.w("-[" + to_string(e->_area.top) + ", " + to_string(e->_area.bottom) + "]");
            l.w_endl();
        });
}

bool SlimCursorUpdate()
{
    POINT pt;
    GetCursorPos(&pt);
    auto cursor = slim::point(pt);
    GlobalInfo::I()->point = cursor;
    return true;
}

DWORD WINAPI _LoopInternal(LPVOID lpParam)
{
    try
    {
        GlobalInfo::I()->chain = GlobalInfo::I()->capture_desktop ?
            slim::platform::GetElementChainInDesktop(GlobalInfo::I()->point, GlobalInfo::I()->fast_mode) :
            slim::platform::GetElementChainInActiveWindow(GlobalInfo::I()->point, GlobalInfo::I()->fast_mode);
    }
    catch (...)
    {
        // todo
    }

    return 0;
}

bool SlimLoop(bool draw)
{
    try
    {
        DWORD thrd_id;
        HANDLE thrd = CreateThread(NULL, 0, _LoopInternal, nullptr, 0, &thrd_id);
        if (!thrd)
        {
            return false;
        }
        auto state = WaitForSingleObject(thrd, 8000);
        if (state == WAIT_TIMEOUT)
        {
            TerminateThread(thrd, 0);
        }
        CloseHandle(thrd);

        auto chain = GlobalInfo::I()->chain;
        if (chain && draw && GlobalInfo::I()->highlight_frame)
        {
            framer::draw(chain->Area());
        }
        else
        {
            framer::draw({ 1, -1, 1, -1 });
        }

        return true;
    }
    catch (std::exception& e)
    {
        e;
        return false;
    }
    catch (const string& e)
    {
        e;
        return false;
    }
    catch (...)
    {
        return false;
    }
}

bool SlimLoopTrigger(bool end_sync)
{
    static future<void> _f;

    if (end_sync && _f.valid())
    {
        return _f.wait_for(60000ms) == future_status::ready;
    }

    if (!_f.valid() || _f.wait_for(0ms) == future_status::ready)
    {
        _f = async(
            launch::async,
            []()
            {
                SlimLoop();
            });

        return true;
    }

    return false;
}
