#pragma once

#include <windows.h>
#include <d3d11.h>

extern ID3D11Device* g_Device;
extern ID3D11DeviceContext* g_Context;
extern IDXGISwapChain* g_SwapChain;
extern ID3D11RenderTargetView* g_RTV;
extern HWND g_OverlayHWND;
extern HWND g_GameHWND;
extern bool g_Running;

DWORD WINAPI RendererThread(LPVOID param);