#include "loop.hpp"
#include "runner.hpp"
#include "platform/platform.hpp"
#include "platform/action.hpp"
#include "platform/element.hpp"
#include "utils/draw.hpp"
#include "utils/executor.hpp"

namespace slim
{

bool runner::OpenToRun()
{
    OPENFILENAME ofn;       // common dialog box structure
    WCHAR szFileName[MAX_PATH] = L"";
    // HWND hwnd;              // owner window
    HANDLE hf;              // file handle

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFileName;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFileName);
    ofn.lpstrFilter = L"Json Files\0*.json\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.
    if (!GetOpenFileNameW(&ofn))
    {
        return false;
    }

    hf = CreateFileW(
        ofn.lpstrFile, GENERIC_READ, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hf == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    escape ef_handle = [=]() mutable { CloseHandle(hf); };

    int buf_sz = 1024 * 1024;
    char* buf = new char[buf_sz];
    memset(buf, 0, buf_sz);
    escape ef_buf = [=]() mutable { delete buf; };

    DWORD bytesRead = 0;
    if (!ReadFile(hf, buf, buf_sz, &bytesRead, NULL))
    {
        return false;
    }
    return Run(buf);
}

bool runner::Run(const string& str)
{
    if (!GlobalInfo::I()->Change(IDLE, IDLE_TO_RUNNING))
    {
        return false;
    }

    _ins = xref<runner>::x();
    _ins->_stop = false;
    _ins->_thrd = thread(
        [=]()
        {
            if (!GlobalInfo::I()->Change(IDLE_TO_RUNNING, RUNNING))
            {
                throw nullptr;
            }

            auto sp = _ins;
            if (!sp)
            {
                return;
            }
            try
            {
                Sleep(1000);
                auto as = action_set::FromString(str.c_str());
                sp->_RunScope(as._va, 0, (int)as._va.size());
            }
            catch (...)
            {
                // todo
            }

            GlobalInfo::I()->Change(RUNNING, RUNNING_TO_IDLE);
            GlobalInfo::I()->Change(RUNNING_TO_IDLE, IDLE);
        });
    _ins->_thrd.detach();
    return true;
}

bool runner::Stop()
{
    auto ins = _ins;
    if (!ins)
    {
        return false;
    }
    ins->_stop = true;
    if (!GlobalInfo::I()->Change(RUNNING, RUNNING_TO_IDLE))
    {
        return false;
    }
    return true;
}

bool runner::_RunScope(const vector<shared_ptr<action>>& va, int start, int end)
{
    int idx = start;
    while (idx < end)
    {
        if (_stop)
        {
            return false;
        }

        auto ac = va[idx];
        if (ac->type == action_type::LOOP_BEGIN)
        {
            // loop start
            int loop_len = _RunLoop(va, idx);
            if (loop_len == -1)
            {
                return false; // run fail
            }
            idx += loop_len;
        }
        else if (ac->type == action_type::LOOP_END)
        {
            // unexpected
            throw nullptr;
        }
        else
        {
            // normal action
            if (_Act(ac))
            {
                ++idx;
            }
            else if (ac->optional)
            {
                ++idx;
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

int runner::_RunLoop(const vector<shared_ptr<action>>& va, int loop_start)
{
    int idx = loop_start;
    if (va[idx++]->type != action_type::LOOP_BEGIN)
    {
        throw nullptr;
    }

    int loops = 1;
    while (idx < va.size())
    {
        if (va[idx]->type == action_type::LOOP_BEGIN)
        {
            ++loops;
        }
        else if (va[idx]->type == action_type::LOOP_END)
        {
            --loops;
        }

        ++idx;

        if (loops == 0)
        {
            break;
        }
    }

    // idx: loop_end
    string times_str = va[loop_start]->parameter["times"];
    int times = -1;
    if (times_str.size() > 0)
    {
        times = atoi(times_str.c_str());
    }

    string timeout_str = va[loop_start]->parameter["duration"];
    int seconds = -1;
    if (timeout_str.size() > 0)
    {
        seconds = atoi(timeout_str.c_str());
    }

    auto time_start = chrono::system_clock::now();
    int cnts = 0;

    while (true)
    {
        if (_stop)
        {
            return -1;
        }

        if (!_RunScope(va, loop_start + 1, idx - 1))
        {
            return -1; // fail
        }
        ++cnts;

        if (times != -1 && cnts >= times)
        {
            break;
        }

        chrono::duration<double, std::milli> ms_span;
        ms_span = chrono::system_clock::now() - time_start;
        if (seconds != -1 && ms_span.count() >= seconds * 1000)
        {
            break;
        }
    }

    return idx - loop_start;
}

bool runner::_Act(shared_ptr<action> ac)
{
    Sleep(ac->wait_time_ms);

    switch (ac->type)
    {

    case action_type::APP_LAUNCH_PATH:
        return LaunchShellFilePath(ac->parameter["path"]);

    case action_type::APP_LAUNCH_APPID:
        return LaunchShellAppId(ac->parameter["appid"]);

    case action_type::KEY_INPUT:
        if (ac->parameter["keys"].size() < 1)
        {
            return false;
        }
        for (auto c : ac->parameter["keys"])
        {
            if (!_KeyInput(c))
            {
                return false;
            }
        }
        return true;

    default:
        return _ActElement(ac);
    }
}

bool runner::_ActElement(shared_ptr<action> ac)
{
    shared_ptr<element> elm = nullptr;
    platform::UpdateDesktopWnds();

    if (ac->wnd_info.IsGlobal())
    {
        auto matched = platform::FindElementInDesktop(ac->element_stacks);
        elm = (matched && matched->score > 0) ? matched->elm : nullptr;
    }
    else
    {
        auto wnds = platform::GetWnds(ac->wnd_info);
        WndInfo best_wnd = {};
        shared_ptr<element_matched> best = nullptr;
        for (auto wnd : wnds)
        {
            auto matched = platform::FindElementInWindow(wnd, ac->element_stacks);
            if (!best || (matched && matched->score > best->score))
            {
                best = matched;
                best_wnd = wnd;
            }
        }
        if (best && best_wnd.wnd)
        {
            elm = best->elm;
            if (GetForegroundWindow() != best_wnd.wnd && !SetForegroundWindow(best_wnd.wnd))
            {
                return false;
            }
            Sleep(200);
        }
    }

    if (!elm)
    {
        return false;
    }
    if (GlobalInfo::I()->highlight_frame)
    {
        // framer::draw(elm->_area);
    }
    return elm->Act(ac->type);
}

bool runner::_KeyInput(char c)
{
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = MapVirtualKey(c, MAPVK_VK_TO_VSC);
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;
    input.ki.wVk = c;
    input.ki.dwFlags = 0;
    if (SendInput(1, &input, sizeof(INPUT)) == 0)
    {
        return false;
    }

    Sleep(20);

    input.ki.dwFlags = KEYEVENTF_KEYUP;
    if (SendInput(1, &input, sizeof(INPUT)) == 0)
    {
        return false;
    }

    Sleep(20);

    return true;
}

}
