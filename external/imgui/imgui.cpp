// Minimal ImGui implementation for SCARRY EXECUTOR
// This is a placeholder - replace with real ImGui source

#include "imgui.h"
#include <Windows.h>

struct ImGuiContext {};
static ImGuiContext* g_context = nullptr;

// Minimal implementations
ImGuiContext* ImGui::CreateContext(ImFontAtlas* shared_font_atlas)
{
    if (!g_context)
        g_context = new ImGuiContext();
    return g_context;
}

void ImGui::DestroyContext(ImGuiContext* ctx)
{
    if (ctx == g_context) {
        delete g_context;
        g_context = nullptr;
    }
}

ImGuiIO& ImGui::GetIO()
{
    static ImGuiIO io;
    return io;
}

ImGuiStyle& ImGui::GetStyle()
{
    static ImGuiStyle style;
    return style;
}

void ImGui::NewFrame() {}
void ImGui::Render() {}
ImDrawData* ImGui::GetDrawData() { return nullptr; }

bool ImGui::Begin(const char* name, bool* p_open, ImGuiWindowFlags flags)
{
    return true;
}

void ImGui::End() {}

void ImGui::Text(const char* fmt, ...) {}
void ImGui::TextColored(const ImVec4& col, const char* fmt, ...) {}
void ImGui::TextUnformatted(const char* text, const char* text_end) {}
void ImGui::Separator() {}
void ImGui::SameLine(float offset_from_start_x, float spacing) {}

bool ImGui::Button(const char* label, const ImVec2& size) { return false; }
bool ImGui::InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags) { return false; }

bool ImGui::BeginMenuBar() { return false; }
void ImGui::EndMenuBar() {}
bool ImGui::BeginMenu(const char* label, bool enabled) { return false; }
void ImGui::EndMenu() {}
bool ImGui::MenuItem(const char* label, const char* shortcut, bool selected, bool enabled) { return false; }
bool ImGui::MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled) { return false; }

ImGuiID ImGui::GetID(const char* str_id) { return 0; }
void ImGui::DockSpace(ImGuiID id, const ImVec2& size, ImGuiDockNodeFlags flags, const ImGuiWindowClass* window_class) {}

ImGuiViewport* ImGui::GetMainViewport()
{
    static ImGuiViewport viewport;
    return &viewport;
}

void ImGui::SetNextWindowPos(const ImVec2& pos, ImGuiCond cond, const ImVec2& pivot) {}
void ImGui::SetNextWindowSize(const ImVec2& size, ImGuiCond cond) {}
void ImGui::SetNextWindowViewport(ImGuiID viewport_id) {}

void ImGui::PushStyleVar(ImGuiStyleVar idx, float val) {}
void ImGui::PushStyleVar(ImGuiStyleVar idx, const ImVec2& val) {}
void ImGui::PopStyleVar(int count) {}

bool ImGui::BeginChild(const char* str_id, const ImVec2& size, bool border, ImGuiWindowFlags window_flags) { return true; }
void ImGui::EndChild() {}

ImVec2 ImGui::GetContentRegionAvail()
{
    return ImVec2(800, 600);
}

float ImGui::GetScrollY() { return 0.0f; }
float ImGui::GetScrollMaxY() { return 0.0f; }
void ImGui::SetScrollHereY(float center_y_ratio) {}

void ImGui::BeginDisabled(bool disabled) {}
void ImGui::EndDisabled() {}