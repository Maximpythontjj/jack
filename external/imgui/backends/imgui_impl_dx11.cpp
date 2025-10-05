// Dear ImGui: Renderer Backend for DirectX11
// Minimal implementation for SCARRY EXECUTOR

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;

bool ImGui_ImplDX11_Init(ID3D11Device* device, ID3D11DeviceContext* device_context)
{
    g_pd3dDevice = device;
    g_pd3dDeviceContext = device_context;
    return true;
}

void ImGui_ImplDX11_Shutdown()
{
    g_pd3dDevice = nullptr;
    g_pd3dDeviceContext = nullptr;
}

void ImGui_ImplDX11_NewFrame() {}

void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data) {}