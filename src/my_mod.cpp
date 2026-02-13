#include "d3d_renderer.h"

extern "C" __declspec(dllexport) void ModInit()
{
    CreateThread(nullptr, 0, RendererThread, nullptr, 0, nullptr);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_DETACH)
        g_Running = false;

    return TRUE;
}