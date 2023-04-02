#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#include "loop.hpp"
#include "function/recorder.hpp"
#include "function/runner.hpp"
#include "platform/element_chain.hpp"
#include "utils/draw.hpp"

static bool LightButton(const string& txt, bool _long = false);
static bool DarkButton(const string& txt, bool _long = false);
static void DisabledButton(const string& txt, bool _long = false);
static bool EditButton(const string& btn, char* ch, int sz);

bool GuiLoop(ImGuiWindowFlags window_flags, bool& done)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(640, 640));
    ImGui::Begin("Slim Demo", nullptr, window_flags);

    ImGui::Checkbox("All Desktop Capture", &GlobalInfo::I()->capture_desktop);
    ImGui::SameLine();
    ImGui::Checkbox("Highlight Element Frame", &GlobalInfo::I()->highlight_frame);

    SlimCursorUpdate();
    ImGui::Text("Cursor: [%d]-[%d]  ", GlobalInfo::I()->point.x, GlobalInfo::I()->point.y);
    ImGui::SameLine();
    ImGui::Text("%.1f FPS", io.Framerate);

    auto state = GlobalInfo::I()->state;
    bool moved = false;

    if (state == RECORDING)
    {
        if (DarkButton("Stop Recording", true) && !moved)
        {
            slim::recorder::Stop();
            moved = true;
        }

        if (EditButton("Launch App with Path/ID", GlobalInfo::I()->app_to_launch, 256))
        {
            slim::recorder::LaunchApp();
            moved = true;
        }
    }
    else if (state == RUNNING)
    {
        if (DarkButton("Stop Running", true) && !moved)
        {
            slim::runner::Stop();
            moved = true;
        }
    }
    else
    {
        SlimLoopTrigger();

        if (LightButton("Start Recording") && !moved)
        {
            slim::recorder::Start();
            moved = true;
        }

        ImGui::SameLine();

        if (slim::recorder::HasCases())
        {
            if (LightButton("Run Cases") && !moved)
            {
                slim::runner::Run(slim::recorder::Report());
                moved = true;
            }
        }
        else
        {
            DisabledButton("No Cases To Run");
        }

        if (LightButton(GlobalInfo::I()->console_window ? "Show DOM" : "Show Console") && !moved)
        {
            if (GlobalInfo::I()->console_window)
            {
                GlobalInfo::I()->ClearConsole();
            }
            GlobalInfo::I()->console_window = !GlobalInfo::I()->console_window;
            moved = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("Exit", ImVec2(300, 25)) && !moved)
        {
            done = true;
            moved = true;
        }

        if (GlobalInfo::I()->console_window)
        {
            ImGui::BeginChild("Console", ImVec2(620, 500));
            for (auto& c : GlobalInfo::I()->Consoles())
            {
                ImGui::Text(c.c_str());
            }
            ImGui::EndChild();
        }
        else
        {
            auto chain = GlobalInfo::I()->chain;
            if (chain)
            {
                ImGui::Text(chain->FriendlyIdentifier().c_str());
            }
        }
    }

    ImGui::End();
    return true;
}

static bool LightButton(const string& txt, bool _long)
{
    return ImGui::Button(txt.c_str(), ImVec2(_long ? 610.0f : 300.0f, 25.0f));
}

static bool DarkButton(const string& txt, bool _long)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.3f, 0.1f, 1.0f));
    bool click = ImGui::Button(txt.c_str(), ImVec2(_long ? 610.0f : 300.0f, 25.0f));
    ImGui::PopStyleColor(1);
    return click;
}

static void DisabledButton(const string& txt, bool _long)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::Button(txt.c_str(), ImVec2(_long ? 610.0f : 300.0f, 25.0f));
    ImGui::PopStyleColor(1);
}

static bool EditButton(const string& btn, char* ch, int sz)
{
    bool click = ImGui::Button(btn.c_str(), ImVec2(200.0f, 25.0f));
    ImGui::SameLine();
    ImGui::InputTextEx(" ", NULL, GlobalInfo::I()->app_to_launch, 256, ImVec2(400.0f, 25.0f), 0, NULL, NULL);
    return click;
}
