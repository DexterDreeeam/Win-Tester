#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#include "loop.hpp"
#include "recorder.hpp"
#include "runner.hpp"

bool GuiLoop(ImGuiWindowFlags window_flags)
{
    ImGuiIO& io = ImGui::GetIO();

    auto cursor_str = "[" + to_string(GlobalInfo::I()->point.x) + "] - [" + to_string(GlobalInfo::I()->point.y) + "]";
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(800, 600));
    ImGui::Begin("Slim Demo", nullptr, window_flags);

    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    if (ImGui::Button(
        GlobalInfo::I()->recording ? "Finish Recording" : "Start Recording",
        ImVec2(200, 25)))
    {
        if (GlobalInfo::I()->recording)
        {
            slim::recorder::StopRecord();
            GlobalInfo::I()->recording = false;

            string report = slim::recorder::Report();
            cout << report;
        }
        else if (!GlobalInfo::I()->running)
        {
            slim::recorder::StartRecord();
            GlobalInfo::I()->recording = true;
        }
    }

    if (ImGui::Button(
        GlobalInfo::I()->running ? "Is Running" : "Run Case",
        ImVec2(200, 25)))
    {
        if (!GlobalInfo::I()->running)
        {
            string report = slim::recorder::Report();
            slim::runner().Run(report);
        }
    }

    bool oldConsole = GlobalInfo::I()->console_window;
    ImGui::Checkbox("Console", &GlobalInfo::I()->console_window);

    ImGui::NewLine();
    ImGui::Text(cursor_str.c_str());
    ImGui::NewLine();

    auto chain = GlobalInfo::I()->chain;
    if (chain)
    {
        ImGui::Text(GlobalInfo::I()->chain->FriendlyIdentifier().c_str());
    }

    ImGui::End();

    if (oldConsole && !GlobalInfo::I()->console_window)
    {
        // close console
        GlobalInfo::I()->consoles.clear();
    }

    if (GlobalInfo::I()->console_window)
    {
        ;
    }

    return true;
}
