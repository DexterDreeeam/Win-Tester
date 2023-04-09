#include "imgui.h"
#include "imgui_internal.h"

#include "loop.hpp"
#include "function/recorder.hpp"
#include "function/runner.hpp"
#include "platform/element_chain.hpp"
#include "utils/draw.hpp"

static bool LightButton(const string& txt, int width = 300);
static bool DarkButton(const string& txt, int width = 300);
static void DisabledButton(const string& txt, int width = 300);
static bool EditButton(const string& btn, char* ch, int sz, int width = 300, int editWidth = 300);

bool GuiLoop(ImGuiWindowFlags window_flags, bool& done)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(640, 640));
    ImGui::Begin("Win Tester", nullptr, window_flags);

    if (GlobalInfo::I()->exiting)
    {
        ImGui::Text("Exiting...");
        ImGui::End();
        SlimLoopTrigger(true);
        slim::platform::Release();
        done = true;
        return true;
    }

    ImGui::Checkbox("Fast-Mode  ", &GlobalInfo::I()->fast_mode);
    ImGui::SameLine();
    ImGui::Checkbox("Global-UI  ", &GlobalInfo::I()->capture_desktop);
    ImGui::SameLine();
    ImGui::Checkbox("Highlight  ", &GlobalInfo::I()->highlight_frame);

    SlimCursorUpdate();
    ImGui::Text("Cursor: [%d]-[%d]  ", GlobalInfo::I()->point.x, GlobalInfo::I()->point.y);
    ImGui::SameLine();
    ImGui::Text("%.1f FPS", io.Framerate);

    auto state = GlobalInfo::I()->state;
    bool moved = false;

    if (GlobalInfo::I()->record_finished || state == RECORDING)
    {
        if (GlobalInfo::I()->record_finished)
        {
            if (DarkButton("Export Cases File", 608) && !moved)
            {
                if (slim::recorder::Export())
                {
                    GlobalInfo::I()->record_finished = false;
                    GlobalInfo::I()->record_loop_stack = 0;
                }
                moved = true;
            }

            if (LightButton("Record New Cases", 300) && !moved)
            {
                GlobalInfo::I()->record_finished = false;
                GlobalInfo::I()->record_loop_stack = 0;
                slim::recorder::Start();
                moved = true;
            }
            ImGui::SameLine();
            if (LightButton("Back", 300) && !moved)
            {
                GlobalInfo::I()->record_finished = false;
                GlobalInfo::I()->record_loop_stack = 0;
                moved = true;
            }
        }
        else if (DarkButton("Stop Recording", 608) && !moved)
        {
            slim::recorder::Stop();
            moved = true;
        }

        ImGui::NewLine();
        ImGui::Text("Wait for time before next Action");
        ImGui::SameLine();
        ImGui::InputTextEx("ms", NULL, GlobalInfo::I()->action_wait_for_ms, 256, ImVec2((float)80, 20.0f), 0, NULL, NULL);
        ImGui::SameLine();
        if (ImGui::Button("Reset", ImVec2((float)60, 20.0f)))
        {
            GlobalInfo::I()->ResetWaitMs();
        }

        ImGui::Checkbox("Next Action as Optional", &GlobalInfo::I()->record_optional);

        ImGui::NewLine();
        ImGui::InputTextEx("    LaunchFilePathOrID", NULL, GlobalInfo::I()->app_to_launch, 256, ImVec2((float)608, 20.0f), 0, NULL, NULL);

        if (LightButton("Launch File Path"))
        {
            slim::recorder::LaunchAppPath();
            moved = true;
        }
        ImGui::SameLine();
        if (LightButton("Launch App ID"))
        {
            slim::recorder::LaunchAppId();
            moved = true;
        }

        ImGui::NewLine();
        ImGui::Text("Loop Parameter ('18' for 18 times, '60s' for 60s duration or '18,60s' for both)");

        if (LightButton("Loop Begin"))
        {
            slim::recorder::LoopBegin();
            moved = true;
        }
        ImGui::SameLine();
        ImGui::InputTextEx("  Loop Par", NULL, GlobalInfo::I()->record_loop_pars, 256, ImVec2((float)300, 25.0f), 0, NULL, NULL);

        if (LightButton("Loop End"))
        {
            slim::recorder::LoopEnd();
            moved = true;
        }
        ImGui::SameLine();
        ImGui::Text(" Loop Stacks Depth: [%d]", GlobalInfo::I()->record_loop_stack);

        auto chain = GlobalInfo::I()->chain;
        if (chain)
        {
            ImGui::NewLine();
            ImGui::Text(chain->FriendlyIdentifier().c_str());
        }
    }
    else if (state == RUNNING)
    {
        if (DarkButton("Stop Running", 608) && !moved)
        {
            slim::runner::Stop();
            moved = true;
        }
    }
    else
    {
        SlimLoopTrigger();

        if (LightButton("Start Recording", 608) && !moved)
        {
            slim::recorder::Start();
            moved = true;
        }

        if (slim::recorder::HasCases())
        {
            if (LightButton("Run Temp Cases", 608) && !moved)
            {
                slim::runner::Run(slim::recorder::Report());
                moved = true;
            }
        }
        else
        {
            DisabledButton("No Temp Cases To Run", 608);
        }

        if (LightButton("Import Cases To Run", 608))
        {
            slim::runner::OpenToRun();
            moved = true;
        }

        if (LightButton(GlobalInfo::I()->console_window ? "Show DOM" : "Show Console") && !moved)
        {
            if (GlobalInfo::I()->console_window)
            {
                // GlobalInfo::I()->ClearConsole();
            }
            GlobalInfo::I()->console_window = !GlobalInfo::I()->console_window;
            moved = true;
        }

        ImGui::SameLine();

        if (LightButton("Exit") && !moved)
        {
            GlobalInfo::I()->exiting = true;
            moved = true;
        }

        if (GlobalInfo::I()->console_window)
        {
            ImGui::BeginChild("Console", ImVec2(620, 500));
            //for (auto& c : GlobalInfo::I()->Consoles())
            //{
            //    ImGui::Text(c.c_str());
            //}
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

static bool LightButton(const string& txt, int width)
{
    return ImGui::Button(txt.c_str(), ImVec2((float)width, 25.0f));
}

static bool DarkButton(const string& txt, int width)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.3f, 0.1f, 1.0f));
    bool click = ImGui::Button(txt.c_str(), ImVec2((float)width, 25.0f));
    ImGui::PopStyleColor(1);
    return click;
}

static void DisabledButton(const string& txt, int width)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::Button(txt.c_str(), ImVec2((float)width, 25.0f));
    ImGui::PopStyleColor(1);
}

static bool EditButton(const string& btn, char* ch, int sz, int width, int editWidth)
{
    bool click = ImGui::Button(btn.c_str(), ImVec2((float)width, 25.0f));
    ImGui::SameLine();
    ImGui::InputTextEx(" ", NULL, ch, sz, ImVec2((float)editWidth, 25.0f), 0, NULL, NULL);
    return click;
}
