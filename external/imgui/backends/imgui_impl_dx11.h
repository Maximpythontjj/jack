// Dear ImGui: standalone example application for DirectX 11
#pragma once

#include <d3d11.h>

IMGUI_IMPL_API bool ImGui_ImplDX11_Init(ID3D11Device* device, ID3D11DeviceContext* device_context);
IMGUI_IMPL_API void ImGui_ImplDX11_Shutdown();
IMGUI_IMPL_API void ImGui_ImplDX11_NewFrame();
IMGUI_IMPL_API void ImGui_ImplDX11_RenderDrawData(struct ImDrawData* draw_data);