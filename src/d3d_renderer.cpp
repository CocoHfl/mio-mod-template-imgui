#include "d3d_renderer.h"
#include "imgui_ui.h"
#include "dwmapi.h"
#include "imgui_impl_win32.h"
#include "stdio.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")

ID3D11Device* g_Device = nullptr;
ID3D11DeviceContext* g_Context = nullptr;
IDXGISwapChain* g_SwapChain = nullptr;
ID3D11RenderTargetView* g_RTV = nullptr;

HWND g_OverlayHWND = nullptr;
HWND g_GameHWND = nullptr;

bool g_Running = true;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool CreateDeviceD3D(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &g_SwapChain,
        &g_Device,
        nullptr,
        &g_Context
    );

    return SUCCEEDED(hr);
}

static void CleanupDeviceD3D()
{
    if (g_SwapChain)
    {
        g_SwapChain->Release();
        g_SwapChain = nullptr;
    }
    if (g_Context)
    {
        g_Context->Release();
        g_Context = nullptr;
    }
    if (g_Device)
    {
        g_Device->Release();
        g_Device = nullptr;
    }
}

static void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer)
    {
        g_Device->CreateRenderTargetView(pBackBuffer, nullptr, &g_RTV);
        pBackBuffer->Release();
    }
}

static void CleanupRenderTarget()
{
    if (g_RTV)
    {
        g_RTV->Release();
        g_RTV = nullptr;
    }
}

static void UpdateOverlayPosition()
{
    if (!g_GameHWND || !g_OverlayHWND)
        return;

    RECT rect;
    if (GetWindowRect(g_GameHWND, &rect))
    {
        SetWindowPos(
            g_OverlayHWND,
            HWND_TOPMOST,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            SWP_NOACTIVATE
        );
    }
}

static void ToggleOverlay(bool& overlayActive)
{
    overlayActive = !overlayActive;
    LONG exStyle = GetWindowLong(g_OverlayHWND, GWL_EXSTYLE);

    if (overlayActive)
    {
        exStyle &= ~WS_EX_TRANSPARENT;
		SetForegroundWindow(g_OverlayHWND);
        SetFocus(g_OverlayHWND);
        while (ShowCursor(TRUE) < 0);
    }
    else
    {
        exStyle |= WS_EX_TRANSPARENT;
        SetForegroundWindow(g_GameHWND);
        SetFocus(g_GameHWND);
        while (ShowCursor(FALSE) >= 0);
    }

    SetWindowLong(g_OverlayHWND, GWL_EXSTYLE, exStyle);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

DWORD WINAPI RendererThread(LPVOID)
{
    // Looking for game window
    while (g_Running && !g_GameHWND)
    {
        g_GameHWND = FindWindowW(nullptr, L"MIO");
        if (g_GameHWND)
            printf("Found game window! 0x%p\n", g_GameHWND);
        else
            Sleep(100);
    }

    HINSTANCE hInstance = GetModuleHandle(nullptr);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "OverlayClass";

    if (!RegisterClassEx(&wc))
        return 0;

    g_OverlayHWND = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        "Overlay",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );

    if (!g_OverlayHWND)
    {
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 0;
    }

    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(g_OverlayHWND, &margins);

    ShowWindow(g_OverlayHWND, SW_SHOWDEFAULT);
    UpdateWindow(g_OverlayHWND);

    if (!CreateDeviceD3D(g_OverlayHWND))
    {
        DestroyWindow(g_OverlayHWND);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 0;
    }

    CreateRenderTarget();

    InitializeUI();

    MSG msg = {};
    bool overlayActive = false;
    SetForegroundWindow(g_GameHWND);
    while (ShowCursor(FALSE) >= 0);

    while (g_Running && msg.message != WM_QUIT)
    {
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            ToggleOverlay(overlayActive);
        }

        UpdateOverlayPosition();

        constexpr float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_Context->OMSetRenderTargets(1, &g_RTV, nullptr);
        g_Context->ClearRenderTargetView(g_RTV, clearColor);

        RenderUI();

        g_SwapChain->Present(1, 0);
    }

    ShutdownUI();
    CleanupRenderTarget();
    CleanupDeviceD3D();
    DestroyWindow(g_OverlayHWND);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}