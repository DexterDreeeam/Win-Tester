#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#include "loop.hpp"
#include "function/recorder.hpp"
#include "function/runner.hpp"

bool GuiLoop(ImGuiWindowFlags window_flags, bool& done)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(640, 640));
    ImGui::Begin("Slim Demo", nullptr, window_flags);
    // element stacks
    POINT cur;
    if (GetCursorPos(&cur))
    {
        ImGui::Text("Cursor: [%d]-[%d]  ", cur.x, cur.y);
        ImGui::SameLine();
    }
    ImGui::Text("%.1f FPS", io.Framerate);
    if (ImGui::Button(
        GlobalInfo::I()->recording ? "Finish Recording" : "Start Recording",
        ImVec2(300, 25)))
    {
        if (GlobalInfo::I()->recording)
        {
            slim::recorder::StopRecord();
            GlobalInfo::I()->recording = false;

            string report = slim::recorder::Report();
            slim::platform::I()->Console(report);
        }
        else if (!GlobalInfo::I()->running)
        {
            slim::recorder::StartRecord();
            GlobalInfo::I()->recording = true;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button(
        GlobalInfo::I()->running ? "Is Running" : "Run Case",
        ImVec2(300, 25)))
    {
        if (!GlobalInfo::I()->running)
        {
            string report = slim::recorder::Report();
            slim::runner().Run(report);
        }
    }

    if (ImGui::Button(
        GlobalInfo::I()->console_window ? "Show DOM" : "Show Console",
        ImVec2(300, 25)))
    {
        GlobalInfo::I()->console_window = !GlobalInfo::I()->console_window;
        if (!GlobalInfo::I()->console_window)
        {
            GlobalInfo::I()->ClearConsole();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Exit", ImVec2(300, 25)))
    {
        done = true;
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
            ImGui::Text(GlobalInfo::I()->chain->FriendlyIdentifier().c_str());
        }
    }

    ImGui::End();

    return true;
}
