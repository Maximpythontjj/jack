// Dear ImGui: Platform Backend for Windows
#pragma once

#include <Windows.h>

#define IMGUI_IMPL_API

IMGUI_IMPL_API bool ImGui_ImplWin32_Init(void* hwnd);
IMGUI_IMPL_API void ImGui_ImplWin32_Shutdown();
IMGUI_IMPL_API void ImGui_ImplWin32_NewFrame();
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);