#include "imgui_ui.h"
#include "d3d_renderer.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "modding_api.h"
#include "imgui_impl_win32.h"

void InitializeUI()
{
    ImGui_ImplWin32_EnableDpiAwareness();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 24.0f);

    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(g_OverlayHWND);
    ImGui_ImplDX11_Init(g_Device, g_Context);
}

void ShutdownUI()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void RenderUI()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::Begin("Mod", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Add your mod UI content here
    // Example using modding API:
    f32x3 loc = GetPlayerLocation();
    ImGui::Text("Player location: %.2f, %.2f, %.2f", loc.x, loc.y, loc.z);

    if (ImGui::Button("Teleport to 0, 0, 0"))
        SetPlayerLocation(make_f32x3(0, 0, 0));

    ImGui::Separator();
    ImGui::TextDisabled("Press INSERT to toggle interaction with UI");

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}