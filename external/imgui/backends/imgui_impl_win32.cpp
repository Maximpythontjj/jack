// Dear ImGui: Platform Backend for Windows (standard windows API for 32 and 64 bits applications)
// Minimal implementation for SCARRY EXECUTOR

#include "imgui.h"
#include "imgui_impl_win32.h"
#include <Windows.h>

IMGUI_IMPL_API bool ImGui_ImplWin32_Init(void* hwnd)
{
    return true;
}

IMGUI_IMPL_API void ImGui_ImplWin32_Shutdown() {}

IMGUI_IMPL_API void ImGui_ImplWin32_NewFrame() {}

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}