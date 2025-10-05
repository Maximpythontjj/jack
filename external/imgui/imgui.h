// Dear ImGui: Bloat-free Graphical User interface for C++ with minimal dependencies
// Minimal header for SCARRY EXECUTOR

#pragma once

#include <Windows.h>

#define IMGUI_CHECKVERSION()

// Forward declarations
struct ImDrawData;
struct ImFontAtlas;
struct ImGuiContext;

typedef unsigned int ImU32;
typedef int ImGuiBackendFlags;
typedef int ImGuiKey_;
typedef int ImGuiMouseButton_;
typedef int ImGuiHoveredFlags;
typedef int ImGuiFocusedFlags;
typedef int ImGuiSelectableFlags;
typedef int ImGuiComboFlags;
typedef int ImGuiSliderFlags;
typedef int ImGuiColorEditFlags;
typedef int ImGuiTreeNodeFlags;
typedef int ImGuiPopupFlags;
typedef int ImGuiTableFlags;
typedef int ImGuiTableColumnFlags;
typedef int ImGuiTableRowFlags;
typedef int ImGuiTabBarFlags;
typedef int ImGuiTabItemFlags;
typedef int ImGuiDragDropFlags;
typedef int ImGuiButtonFlags;
typedef int ImGuiViewportFlags;
typedef void* ImGuiSizeCallback;
typedef void* ImGuiInputTextCallback;
typedef struct ImDrawList ImDrawList;
typedef struct ImDrawListSharedData ImDrawListSharedData;
typedef struct ImGuiStorage ImGuiStorage;
typedef struct ImGuiPayload ImGuiPayload;
typedef struct ImGuiTableSortSpecs ImGuiTableSortSpecs;
typedef struct ImFont ImFont;
typedef void*(*ImGuiMemAllocFunc)(size_t sz, void* user_data);
typedef void(*ImGuiMemFreeFunc)(void* ptr, void* user_data);

#define ImGuiKey_COUNT 512
#define FLT_MAX 3.402823466e+38F
#define FLT_MIN 1.175494351e-38F

enum ImGuiDir_ { ImGuiDir_None = -1, ImGuiDir_Left = 0, ImGuiDir_Right = 1, ImGuiDir_Up = 2, ImGuiDir_Down = 3, ImGuiDir_COUNT };

// Basic types
typedef unsigned int ImGuiID;
typedef int ImGuiCol;
typedef int ImGuiStyleVar;
typedef int ImGuiKey;
typedef int ImGuiMouseButton;
typedef int ImGuiMouseCursor;
typedef int ImGuiCond;
typedef int ImGuiDataType;
typedef int ImGuiDir;
typedef int ImGuiSortDirection;
typedef int ImGuiTableBgTarget;
typedef void* ImTextureID;
typedef unsigned short ImDrawIdx;
typedef unsigned int ImWchar32;
typedef unsigned short ImWchar16;
typedef ImWchar16 ImWchar;

// Flags for ImGui::Begin()
typedef int ImGuiWindowFlags;
enum ImGuiWindowFlags_
{
    ImGuiWindowFlags_None                   = 0,
    ImGuiWindowFlags_NoTitleBar             = 1 << 0,
    ImGuiWindowFlags_NoResize               = 1 << 1,
    ImGuiWindowFlags_NoMove                 = 1 << 2,
    ImGuiWindowFlags_NoScrollbar            = 1 << 3,
    ImGuiWindowFlags_NoScrollWithMouse      = 1 << 4,
    ImGuiWindowFlags_NoCollapse             = 1 << 5,
    ImGuiWindowFlags_AlwaysAutoResize       = 1 << 6,
    ImGuiWindowFlags_NoBackground           = 1 << 7,
    ImGuiWindowFlags_NoSavedSettings        = 1 << 8,
    ImGuiWindowFlags_NoMouseInputs          = 1 << 9,
    ImGuiWindowFlags_MenuBar                = 1 << 10,
    ImGuiWindowFlags_HorizontalScrollbar    = 1 << 11,
    ImGuiWindowFlags_NoFocusOnAppearing     = 1 << 12,
    ImGuiWindowFlags_NoBringToFrontOnFocus  = 1 << 13,
    ImGuiWindowFlags_AlwaysVerticalScrollbar= 1 << 14,
    ImGuiWindowFlags_AlwaysHorizontalScrollbar=1<< 15,
    ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 16,
    ImGuiWindowFlags_NoNavInputs            = 1 << 18,
    ImGuiWindowFlags_NoNavFocus             = 1 << 19,
    ImGuiWindowFlags_UnsavedDocument        = 1 << 20,
    ImGuiWindowFlags_NoDocking              = 1 << 21,
    ImGuiWindowFlags_NoNav                  = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus,
    ImGuiWindowFlags_NoDecoration           = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse,
    ImGuiWindowFlags_NoInputs               = ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus
};

// Flags for ImGui::InputText()
typedef int ImGuiInputTextFlags;
enum ImGuiInputTextFlags_
{
    ImGuiInputTextFlags_None                = 0,
    ImGuiInputTextFlags_CharsDecimal        = 1 << 0,
    ImGuiInputTextFlags_CharsHexadecimal    = 1 << 1,
    ImGuiInputTextFlags_CharsUppercase      = 1 << 2,
    ImGuiInputTextFlags_CharsNoBlank        = 1 << 3,
    ImGuiInputTextFlags_AutoSelectAll       = 1 << 4,
    ImGuiInputTextFlags_EnterReturnsTrue    = 1 << 5,
    ImGuiInputTextFlags_CallbackCompletion  = 1 << 6,
    ImGuiInputTextFlags_CallbackHistory     = 1 << 7,
    ImGuiInputTextFlags_CallbackAlways      = 1 << 8,
    ImGuiInputTextFlags_CallbackCharFilter  = 1 << 9,
    ImGuiInputTextFlags_AllowTabInput       = 1 << 10,
    ImGuiInputTextFlags_CtrlEnterForNewLine = 1 << 11,
    ImGuiInputTextFlags_NoHorizontalScroll  = 1 << 12,
    ImGuiInputTextFlags_AlwaysOverwrite     = 1 << 13,
    ImGuiInputTextFlags_ReadOnly            = 1 << 14,
    ImGuiInputTextFlags_Password            = 1 << 15,
    ImGuiInputTextFlags_NoUndoRedo          = 1 << 16,
    ImGuiInputTextFlags_CharsScientific     = 1 << 17,
    ImGuiInputTextFlags_CallbackResize      = 1 << 18,
    ImGuiInputTextFlags_CallbackEdit        = 1 << 19
};

typedef int ImGuiConfigFlags;
enum ImGuiConfigFlags_
{
    ImGuiConfigFlags_None                   = 0,
    ImGuiConfigFlags_NavEnableKeyboard     = 1 << 0,
    ImGuiConfigFlags_NavEnableGamepad      = 1 << 1,
    ImGuiConfigFlags_NavEnableSetMousePos  = 1 << 2,
    ImGuiConfigFlags_NavNoCaptureKeyboard  = 1 << 3,
    ImGuiConfigFlags_NoMouse               = 1 << 4,
    ImGuiConfigFlags_NoMouseCursorChange   = 1 << 5,
    ImGuiConfigFlags_DockingEnable         = 1 << 6,
    ImGuiConfigFlags_ViewportsEnable       = 1 << 10,
    ImGuiConfigFlags_DpiEnableScaleViewports= 1 << 14,
    ImGuiConfigFlags_DpiEnableScaleFonts    = 1 << 15
};

typedef int ImGuiDockNodeFlags;
enum ImGuiDockNodeFlags_
{
    ImGuiDockNodeFlags_None                         = 0,
    ImGuiDockNodeFlags_KeepAliveOnly                = 1 << 0,
    ImGuiDockNodeFlags_NoDockingInCentralNode       = 1 << 2,
    ImGuiDockNodeFlags_PassthruCentralNode          = 1 << 3,
    ImGuiDockNodeFlags_NoSplit                      = 1 << 4,
    ImGuiDockNodeFlags_NoResize                     = 1 << 5,
    ImGuiDockNodeFlags_AutoHideTabBar               = 1 << 6
};

// 2D vector (often used to store positions or sizes)
struct ImVec2
{
    float   x, y;
    ImVec2() { x = y = 0.0f; }
    ImVec2(float _x, float _y) { x = _x; y = _y; }
};

// 4D vector (often used to store colors)
struct ImVec4
{
    float   x, y, z, w;
    ImVec4() { x = y = z = w = 0.0f; }
    ImVec4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
};

enum ImGuiCol_
{
    ImGuiCol_Text,
    ImGuiCol_TextDisabled,
    ImGuiCol_WindowBg,
    ImGuiCol_ChildBg,
    ImGuiCol_PopupBg,
    ImGuiCol_Border,
    ImGuiCol_BorderShadow,
    ImGuiCol_FrameBg,
    ImGuiCol_FrameBgHovered,
    ImGuiCol_FrameBgActive,
    ImGuiCol_TitleBg,
    ImGuiCol_TitleBgActive,
    ImGuiCol_TitleBgCollapsed,
    ImGuiCol_MenuBarBg,
    ImGuiCol_ScrollbarBg,
    ImGuiCol_ScrollbarGrab,
    ImGuiCol_ScrollbarGrabHovered,
    ImGuiCol_ScrollbarGrabActive,
    ImGuiCol_CheckMark,
    ImGuiCol_SliderGrab,
    ImGuiCol_SliderGrabActive,
    ImGuiCol_Button,
    ImGuiCol_ButtonHovered,
    ImGuiCol_ButtonActive,
    ImGuiCol_Header,
    ImGuiCol_HeaderHovered,
    ImGuiCol_HeaderActive,
    ImGuiCol_COUNT
};

enum ImGuiStyleVar_
{
    ImGuiStyleVar_Alpha,
    ImGuiStyleVar_WindowPadding,
    ImGuiStyleVar_WindowRounding,
    ImGuiStyleVar_WindowBorderSize,
    ImGuiStyleVar_WindowMinSize,
    ImGuiStyleVar_WindowTitleAlign,
    ImGuiStyleVar_ChildRounding,
    ImGuiStyleVar_ChildBorderSize,
    ImGuiStyleVar_PopupRounding,
    ImGuiStyleVar_PopupBorderSize,
    ImGuiStyleVar_FramePadding,
    ImGuiStyleVar_FrameRounding,
    ImGuiStyleVar_FrameBorderSize,
    ImGuiStyleVar_ItemSpacing,
    ImGuiStyleVar_ItemInnerSpacing,
    ImGuiStyleVar_IndentSpacing,
    ImGuiStyleVar_CellPadding,
    ImGuiStyleVar_ScrollbarSize,
    ImGuiStyleVar_ScrollbarRounding,
    ImGuiStyleVar_GrabMinSize,
    ImGuiStyleVar_GrabRounding,
    ImGuiStyleVar_TabRounding,
    ImGuiStyleVar_ButtonTextAlign,
    ImGuiStyleVar_SelectableTextAlign,
    ImGuiStyleVar_COUNT
};

struct ImGuiStyle
{
    ImVec4      Colors[ImGuiCol_COUNT];
    float       Alpha;
    ImVec2      WindowPadding;
    float       WindowRounding;
    float       WindowBorderSize;
    ImVec2      WindowMinSize;
    ImVec2      WindowTitleAlign;
    float       ChildRounding;
    float       ChildBorderSize;
    float       PopupRounding;
    float       PopupBorderSize;
    ImVec2      FramePadding;
    float       FrameRounding;
    float       FrameBorderSize;
    ImVec2      ItemSpacing;
    ImVec2      ItemInnerSpacing;
    ImVec2      CellPadding;
    ImVec2      TouchExtraPadding;
    float       IndentSpacing;
    float       ColumnsMinSpacing;
    float       ScrollbarSize;
    float       ScrollbarRounding;
    float       GrabMinSize;
    float       GrabRounding;
    float       LogSliderDeadzone;
    float       TabRounding;
    float       TabBorderSize;
    float       TabMinWidthForCloseButton;
    ImGuiDir    ColorButtonPosition;
    ImVec2      ButtonTextAlign;
    ImVec2      SelectableTextAlign;
    ImVec2      DisplayWindowPadding;
    ImVec2      DisplaySafeAreaPadding;
    float       MouseCursorScale;
    bool        AntiAliasedLines;
    bool        AntiAliasedLinesUseTex;
    bool        AntiAliasedFill;
    float       CurveTessellationTol;
    float       CircleTessellationMaxError;
};

struct ImGuiIO
{
    ImGuiConfigFlags   ConfigFlags;
    ImGuiBackendFlags  BackendFlags;
    ImVec2      DisplaySize;
    float       DeltaTime;
    float       IniSavingRate;
    const char* IniFilename;
    const char* LogFilename;
    float       MouseDoubleClickTime;
    float       MouseDoubleClickMaxDist;
    float       MouseDragThreshold;
    int         KeyMap[ImGuiKey_COUNT];
    float       KeyRepeatDelay;
    float       KeyRepeatRate;
    void*       UserData;
};

struct ImGuiViewport
{
    ImGuiViewportFlags  Flags;
    ImVec2              Pos;
    ImVec2              Size;
    ImVec2              WorkPos;
    ImVec2              WorkSize;
    float               DpiScale;
    ImGuiID             ParentViewportId;
    ImDrawData*         DrawData;
    void*               RendererUserData;
    void*               PlatformUserData;
    void*               PlatformHandle;
    void*               PlatformHandleRaw;
    bool                PlatformRequestMove;
    bool                PlatformRequestResize;
    bool                PlatformRequestClose;
};

struct ImGuiWindowClass {};

// Main namespace
namespace ImGui
{
    // Context creation and access
    ImGuiContext* CreateContext(ImFontAtlas* shared_font_atlas = NULL);
    void          DestroyContext(ImGuiContext* ctx = NULL);
    ImGuiContext* GetCurrentContext();
    void          SetCurrentContext(ImGuiContext* ctx);

    // Main
    ImGuiIO&      GetIO();
    ImGuiStyle&   GetStyle();
    void          NewFrame();
    void          EndFrame();
    void          Render();
    ImDrawData*   GetDrawData();

    // Demo, Debug, Information
    void          ShowDemoWindow(bool* p_open = NULL);
    void          ShowMetricsWindow(bool* p_open = NULL);
    void          ShowAboutWindow(bool* p_open = NULL);
    void          ShowStyleEditor(ImGuiStyle* ref = NULL);
    bool          ShowStyleSelector(const char* label);
    void          ShowFontSelector(const char* label);
    void          ShowUserGuide();
    const char*   GetVersion();

    // Windows
    bool          Begin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);
    void          End();

    // Child Windows
    bool          BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags window_flags = 0);
    bool          BeginChild(ImGuiID id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags window_flags = 0);
    void          EndChild();

    // Windows Utilities
    bool          IsWindowAppearing();
    bool          IsWindowCollapsed();
    bool          IsWindowFocused(ImGuiFocusedFlags flags=0);
    bool          IsWindowHovered(ImGuiHoveredFlags flags=0);
    ImDrawList*   GetWindowDrawList();
    ImVec2        GetWindowPos();
    ImVec2        GetWindowSize();
    float         GetWindowWidth();
    float         GetWindowHeight();
    ImGuiViewport*GetWindowViewport();

    // Window manipulation
    void          SetNextWindowPos(const ImVec2& pos, ImGuiCond cond = 0, const ImVec2& pivot = ImVec2(0, 0));
    void          SetNextWindowSize(const ImVec2& size, ImGuiCond cond = 0);
    void          SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, ImGuiSizeCallback custom_callback = NULL, void* custom_callback_data = NULL);
    void          SetNextWindowContentSize(const ImVec2& size);
    void          SetNextWindowCollapsed(bool collapsed, ImGuiCond cond = 0);
    void          SetNextWindowFocus();
    void          SetNextWindowBgAlpha(float alpha);
    void          SetNextWindowViewport(ImGuiID viewport_id);
    void          SetWindowPos(const ImVec2& pos, ImGuiCond cond = 0);
    void          SetWindowSize(const ImVec2& size, ImGuiCond cond = 0);
    void          SetWindowCollapsed(bool collapsed, ImGuiCond cond = 0);
    void          SetWindowFocus();
    void          SetWindowFontScale(float scale);
    void          SetWindowPos(const char* name, const ImVec2& pos, ImGuiCond cond = 0);
    void          SetWindowSize(const char* name, const ImVec2& size, ImGuiCond cond = 0);
    void          SetWindowCollapsed(const char* name, bool collapsed, ImGuiCond cond = 0);
    void          SetWindowFocus(const char* name);

    // Content region
    ImVec2        GetContentRegionAvail();
    ImVec2        GetContentRegionMax();
    ImVec2        GetWindowContentRegionMin();
    ImVec2        GetWindowContentRegionMax();

    // Windows Scrolling
    float         GetScrollX();
    float         GetScrollY();
    void          SetScrollX(float scroll_x);
    void          SetScrollY(float scroll_y);
    float         GetScrollMaxX();
    float         GetScrollMaxY();
    void          SetScrollHereX(float center_x_ratio = 0.5f);
    void          SetScrollHereY(float center_y_ratio = 0.5f);
    void          SetScrollFromPosX(float local_x, float center_x_ratio = 0.5f);
    void          SetScrollFromPosY(float local_y, float center_y_ratio = 0.5f);

    // Parameters stacks (shared)
    void          PushFont(ImFont* font);
    void          PopFont();
    void          PushStyleColor(ImGuiCol idx, ImU32 col);
    void          PushStyleColor(ImGuiCol idx, const ImVec4& col);
    void          PopStyleColor(int count = 1);
    void          PushStyleVar(ImGuiStyleVar idx, float val);
    void          PushStyleVar(ImGuiStyleVar idx, const ImVec2& val);
    void          PopStyleVar(int count = 1);
    void          PushAllowKeyboardFocus(bool allow_keyboard_focus);
    void          PopAllowKeyboardFocus();
    void          PushButtonRepeat(bool repeat);
    void          PopButtonRepeat();

    // Parameters stacks (current window)
    void          PushItemWidth(float item_width);
    void          PopItemWidth();
    void          SetNextItemWidth(float item_width);
    float         CalcItemWidth();
    void          PushTextWrapPos(float wrap_local_pos_x = 0.0f);
    void          PopTextWrapPos();

    // Style read access
    ImFont*       GetFont();
    float         GetFontSize();
    ImVec2        GetFontTexUvWhitePixel();
    ImU32         GetColorU32(ImGuiCol idx, float alpha_mul = 1.0f);
    ImU32         GetColorU32(const ImVec4& col);
    ImU32         GetColorU32(ImU32 col);
    const ImVec4& GetStyleColorVec4(ImGuiCol idx);

    // Cursor / Layout
    void          Separator();
    void          SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f);
    void          NewLine();
    void          Spacing();
    void          Dummy(const ImVec2& size);
    void          Indent(float indent_w = 0.0f);
    void          Unindent(float indent_w = 0.0f);
    void          BeginGroup();
    void          EndGroup();
    ImVec2        GetCursorPos();
    float         GetCursorPosX();
    float         GetCursorPosY();
    void          SetCursorPos(const ImVec2& local_pos);
    void          SetCursorPosX(float local_x);
    void          SetCursorPosY(float local_y);
    ImVec2        GetCursorStartPos();
    ImVec2        GetCursorScreenPos();
    void          SetCursorScreenPos(const ImVec2& pos);
    void          AlignTextToFramePadding();
    float         GetTextLineHeight();
    float         GetTextLineHeightWithSpacing();
    float         GetFrameHeight();
    float         GetFrameHeightWithSpacing();

    // ID stack/scopes
    void          PushID(const char* str_id);
    void          PushID(const char* str_id_begin, const char* str_id_end);
    void          PushID(const void* ptr_id);
    void          PushID(int int_id);
    void          PopID();
    ImGuiID       GetID(const char* str_id);
    ImGuiID       GetID(const char* str_id_begin, const char* str_id_end);
    ImGuiID       GetID(const void* ptr_id);

    // Widgets: Text
    void          TextUnformatted(const char* text, const char* text_end = NULL);
    void          Text(const char* fmt, ...);
    void          TextV(const char* fmt, va_list args);
    void          TextColored(const ImVec4& col, const char* fmt, ...);
    void          TextColoredV(const ImVec4& col, const char* fmt, va_list args);
    void          TextDisabled(const char* fmt, ...);
    void          TextDisabledV(const char* fmt, va_list args);
    void          TextWrapped(const char* fmt, ...);
    void          TextWrappedV(const char* fmt, va_list args);
    void          LabelText(const char* label, const char* fmt, ...);
    void          LabelTextV(const char* label, const char* fmt, va_list args);
    void          BulletText(const char* fmt, ...);
    void          BulletTextV(const char* fmt, va_list args);

    // Widgets: Main
    bool          Button(const char* label, const ImVec2& size = ImVec2(0, 0));
    bool          SmallButton(const char* label);
    bool          InvisibleButton(const char* str_id, const ImVec2& size, ImGuiButtonFlags flags = 0);
    bool          ArrowButton(const char* str_id, ImGuiDir dir);
    void          Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0));
    bool          ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1));
    bool          Checkbox(const char* label, bool* v);
    bool          CheckboxFlags(const char* label, int* flags, int flags_value);
    bool          CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value);
    bool          RadioButton(const char* label, bool active);
    bool          RadioButton(const char* label, int* v, int v_button);
    void          ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-FLT_MIN, 0), const char* overlay = NULL);
    void          Bullet();

    // Widgets: Combo Box
    bool          BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);
    void          EndCombo(); // only call EndCombo() if BeginCombo() returns true!
    bool          Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
    bool          Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);      // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
    bool          Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

    // Widgets: Drag Sliders
    bool          DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);     // If v_min >= v_max we have no bound
    bool          DragFloat2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool          DragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool          DragFloat4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool          DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", const char* format_max = NULL, ImGuiSliderFlags flags = 0);
    bool          DragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);  // If v_min >= v_max we have no bound
    bool          DragInt2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool          DragInt3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool          DragInt4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool          DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", const char* format_max = NULL, ImGuiSliderFlags flags = 0);
    bool          DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0);
    bool          DragScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0);

    // Widgets: Regular Sliders
    bool          SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display.
    bool          SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool          SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool          SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool          SliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg", ImGuiSliderFlags flags = 0);
    bool          SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool          SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool          SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool          SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool          SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
    bool          SliderScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
    bool          VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool          VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool          VSliderScalar(const char* label, const ImVec2& size, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);

    // Widgets: Input with Keyboard
    bool          InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    bool          InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    bool          InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    bool          InputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
    bool          InputFloat2(const char* label, float v[2], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
    bool          InputFloat3(const char* label, float v[3], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
    bool          InputFloat4(const char* label, float v[4], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
    bool          InputInt(const char* label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
    bool          InputInt2(const char* label, int v[2], ImGuiInputTextFlags flags = 0);
    bool          InputInt3(const char* label, int v[3], ImGuiInputTextFlags flags = 0);
    bool          InputInt4(const char* label, int v[4], ImGuiInputTextFlags flags = 0);
    bool          InputDouble(const char* label, double* v, double step = 0.0, double step_fast = 0.0, const char* format = "%.6f", ImGuiInputTextFlags flags = 0);
    bool          InputScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step = NULL, const void* p_step_fast = NULL, const char* format = NULL, ImGuiInputTextFlags flags = 0);
    bool          InputScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_step = NULL, const void* p_step_fast = NULL, const char* format = NULL, ImGuiInputTextFlags flags = 0);

    // Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little color square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
    bool          ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
    bool          ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);
    bool          ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
    bool          ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = NULL);
    bool          ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0)); // display a color square/button, hover for details, return true when pressed.
    void          SetColorEditOptions(ImGuiColorEditFlags flags);                     // initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls.

    // Widgets: Trees
    bool          TreeNode(const char* label);
    bool          TreeNode(const char* str_id, const char* fmt, ...);   // helper variation to easily decoratethe string with a prefix or suffix. Generally the most useful variant.
    bool          TreeNode(const void* ptr_id, const char* fmt, ...);   // "
    bool          TreeNodeV(const char* str_id, const char* fmt, va_list args);
    bool          TreeNodeV(const void* ptr_id, const char* fmt, va_list args);
    bool          TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags = 0);
    bool          TreeNodeEx(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, ...);
    bool          TreeNodeEx(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, ...);
    bool          TreeNodeExV(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args);
    bool          TreeNodeExV(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args);
    void          TreePush(const char* str_id);                                       // ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call TreePush/TreePop yourself if desired.
    void          TreePush(const void* ptr_id = NULL);                                // "
    void          TreePop();                                                          // ~ Unindent()+PopId()
    float         GetTreeNodeToLabelSpacing();                                        // horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode
    bool          CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0);  // if returning 'true' the header is open. doesn't indent nor push on ID stack. user doesn't have to call TreePop().
    bool          CollapsingHeader(const char* label, bool* p_visible, ImGuiTreeNodeFlags flags = 0); // when 'p_visible != NULL': if '*p_visible==true' display an additional small close button on upper right of the header which will set the bool to false when clicked, if '*p_visible==false' don't display the header.
    void          SetNextItemOpen(bool is_open, ImGuiCond cond = 0);                  // set next TreeNode/CollapsingHeader open state.

    // Widgets: Selectables
    bool          Selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0)); // "bool selected" carry the selection state (read-only). Selectable() is clicked is returns true so you can modify your selection state. size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height
    bool          Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));       // "bool* p_selected" point to the selection state (read-write), as a convenient helper.

    // Widgets: List Boxes
    bool          BeginListBox(const char* label, const ImVec2& size = ImVec2(0, 0)); // open a framed scrolling region
    void          EndListBox();                                                       // only call EndListBox() if BeginListBox() returned true!
    bool          ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);
    bool          ListBox(const char* label, int* current_item, bool (*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1);

    // Widgets: Data Plotting
    void          PlotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    void          PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));
    void          PlotHistogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    void          PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));

    // Widgets: Value() Helpers.
    void          Value(const char* prefix, bool b);
    void          Value(const char* prefix, int v);
    void          Value(const char* prefix, unsigned int v);
    void          Value(const char* prefix, float v, const char* float_format = NULL);

    // Widgets: Menus
    bool          BeginMainMenuBar();
    void          EndMainMenuBar();
    bool          BeginMenuBar();
    void          EndMenuBar();
    bool          BeginMenu(const char* label, bool enabled = true);
    void          EndMenu();
    bool          MenuItem(const char* label, const char* shortcut = NULL, bool selected = false, bool enabled = true);  // return true when activated. shortcuts are displayed for convenience but not processed by ImGui at the moment
    bool          MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true);              // return true when activated + toggle (*p_selected) if p_selected != NULL

    // Tooltips
    void          BeginTooltip();
    void          EndTooltip();
    void          SetTooltip(const char* fmt, ...);
    void          SetTooltipV(const char* fmt, va_list args);

    // Popups, Modals
    bool          BeginPopup(const char* str_id, ImGuiWindowFlags flags = 0);
    bool          BeginPopupModal(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);
    void          EndPopup();
    void          OpenPopup(const char* str_id, ImGuiPopupFlags popup_flags = 0);
    void          OpenPopup(ImGuiID id, ImGuiPopupFlags popup_flags = 0);
    void          OpenPopupOnItemClick(const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1);
    void          CloseCurrentPopup();

    // Popups: open+begin combined functions helpers
    bool          BeginPopupContextItem(const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1);
    bool          BeginPopupContextWindow(const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1);
    bool          BeginPopupContextVoid(const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1);

    // Popups: query functions
    bool          IsPopupOpen(const char* str_id, ImGuiPopupFlags flags = 0);

    // Tables (BETA API, see Demo->Tables)
    bool          BeginTable(const char* str_id, int column, ImGuiTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0.0f, 0.0f), float inner_width = 0.0f);
    void          EndTable();                                         // only call EndTable() if BeginTable() returns true!
    void          TableNextRow(ImGuiTableRowFlags row_flags = 0, float min_row_height = 0.0f); // append into the first cell of a new row.
    bool          TableNextColumn();                                  // append into the next column (or first column of next row if currently in last column). Return true when column is visible.
    bool          TableSetColumnIndex(int column_n);                  // append into the specified column. Return true when column is visible.

    // Tables: Headers & Columns declaration
    void          TableSetupColumn(const char* label, ImGuiTableColumnFlags flags = 0, float init_width_or_weight = 0.0f, ImGuiID user_id = 0);
    void          TableSetupScrollFreeze(int cols, int rows);         // lock columns/rows so they stay visible when scrolled.
    void          TableHeadersRow();                                  // submit all headers cells based on data provided to TableSetupColumn() + submit context menu
    void          TableHeader(const char* label);                     // submit one header cell manually (rarely used)

    // Tables: Sorting & Miscellaneous functions
    ImGuiTableSortSpecs* TableGetSortSpecs();                        // get latest sort specs for the table (NULL if not sorting).  Lifetime: don't hold on this pointer over multiple frames or past any subsequent call to BeginTable().
    int                  TableGetColumnCount();                       // return number of columns (value passed to BeginTable)
    int                  TableGetColumnIndex();                       // return current column index.
    int                  TableGetRowIndex();                          // return current row index.
    const char*          TableGetColumnName(int column_n = -1);       // return "" if column didn't have a name declared by TableSetupColumn(). Pass -1 to use current column.
    ImGuiTableColumnFlags TableGetColumnFlags(int column_n = -1);     // return column flags so you can query their Enabled/Visible/Sorted/Hovered status flags. Pass -1 to use current column.
    void                 TableSetColumnEnabled(int column_n, bool v); // change user accessible enabled/disabled state of a column. Set to false to hide the column. User can use the context menu to change this themselves (right-click in headers, or right-click in columns body with ImGuiTableFlags_ContextMenuInBody)
    void                 TableSetBgColor(ImGuiTableBgTarget target, ImU32 color, int column_n = -1);  // change the color of a cell, row, or column. See ImGuiTableBgTarget_ flags for details.

    // Legacy Columns API (prefer using Tables!)
    void          Columns(int count = 1, const char* id = NULL, bool border = true);
    void          NextColumn();
    int           GetColumnIndex();
    float         GetColumnWidth(int column_index = -1);
    void          SetColumnWidth(int column_index, float width);
    float         GetColumnOffset(int column_index = -1);
    void          SetColumnOffset(int column_index, float offset_x);
    int           GetColumnsCount();

    // Tab Bars, Tabs
    bool          BeginTabBar(const char* str_id, ImGuiTabBarFlags flags = 0);
    void          EndTabBar();
    bool          BeginTabItem(const char* label, bool* p_open = NULL, ImGuiTabItemFlags flags = 0);
    void          EndTabItem();
    bool          TabItemButton(const char* label, ImGuiTabItemFlags flags = 0);
    void          SetTabItemClosed(const char* tab_or_docked_window_label);

    // Docking
    ImGuiID       DockSpace(ImGuiID id, const ImVec2& size = ImVec2(0, 0), ImGuiDockNodeFlags flags = 0, const ImGuiWindowClass* window_class = NULL);
    ImGuiID       DockSpaceOverViewport(const ImGuiViewport* viewport = NULL, ImGuiDockNodeFlags flags = 0, const ImGuiWindowClass* window_class = NULL);
    void          SetNextWindowDockID(ImGuiID dock_id, ImGuiCond cond = 0);
    void          SetNextWindowClass(const ImGuiWindowClass* window_class);
    ImGuiID       GetWindowDockID();
    bool          IsWindowDocked();

    // Logging/Capture
    void          LogToTTY(int auto_open_depth = -1);
    void          LogToFile(int auto_open_depth = -1, const char* filename = NULL);
    void          LogToClipboard(int auto_open_depth = -1);
    void          LogFinish();
    void          LogButtons();
    void          LogText(const char* fmt, ...);
    void          LogTextV(const char* fmt, va_list args);

    // Drag and Drop
    bool          BeginDragDropSource(ImGuiDragDropFlags flags = 0);
    bool          SetDragDropPayload(const char* type, const void* data, size_t sz, ImGuiCond cond = 0);
    void          EndDragDropSource();
    bool          BeginDragDropTarget();
    const ImGuiPayload* AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags = 0);
    void          EndDragDropTarget();
    const ImGuiPayload* GetDragDropPayload();

    // Disabling [BETA API]
    void          BeginDisabled(bool disabled = true);
    void          EndDisabled();

    // Clipping
    void          PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect);
    void          PopClipRect();

    // Focus, Activation
    void          SetItemDefaultFocus();
    void          SetKeyboardFocusHere(int offset = 0);

    // Item/Widgets Utilities and Query Functions
    bool          IsItemHovered(ImGuiHoveredFlags flags = 0);
    bool          IsItemActive();
    bool          IsItemFocused();
    bool          IsItemClicked(ImGuiMouseButton mouse_button = 0);
    bool          IsItemVisible();
    bool          IsItemEdited();
    bool          IsItemActivated();
    bool          IsItemDeactivated();
    bool          IsItemDeactivatedAfterEdit();
    bool          IsItemToggledOpen();
    bool          IsAnyItemHovered();
    bool          IsAnyItemActive();
    bool          IsAnyItemFocused();
    ImVec2        GetItemRectMin();
    ImVec2        GetItemRectMax();
    ImVec2        GetItemRectSize();
    void          SetItemAllowOverlap();

    // Viewports
    ImGuiViewport* GetMainViewport();

    // Miscellaneous Utilities
    bool          IsRectVisible(const ImVec2& size);
    bool          IsRectVisible(const ImVec2& rect_min, const ImVec2& rect_max);
    double        GetTime();
    int           GetFrameCount();
    ImDrawList*   GetBackgroundDrawList();
    ImDrawList*   GetForegroundDrawList();
    ImDrawList*   GetBackgroundDrawList(ImGuiViewport* viewport);
    ImDrawList*   GetForegroundDrawList(ImGuiViewport* viewport);
    ImDrawListSharedData* GetDrawListSharedData();
    const char*   GetStyleColorName(ImGuiCol idx);
    void          SetStateStorage(ImGuiStorage* storage);
    ImGuiStorage* GetStateStorage();
    bool          BeginChildFrame(ImGuiID id, const ImVec2& size, ImGuiWindowFlags flags = 0);
    void          EndChildFrame();

    // Text Utilities
    ImVec2        CalcTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);

    // Color Utilities
    ImVec4        ColorConvertU32ToFloat4(ImU32 in);
    ImU32         ColorConvertFloat4ToU32(const ImVec4& in);
    void          ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v);
    void          ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);

    // Inputs Utilities: Keyboard/Mouse/Gamepad
    bool          IsKeyDown(ImGuiKey key);                                      // is key being held
    bool          IsKeyPressed(ImGuiKey key, bool repeat = true);               // was key pressed (went from !Down to Down)? if repeat=true, uses io.KeyRepeatDelay / KeyRepeatRate
    bool          IsKeyReleased(ImGuiKey key);                                  // was key released (went from Down to !Down)?
    int           GetKeyPressedAmount(ImGuiKey key, float repeat_delay, float rate); // uses provided repeat rate/delay. return a count, most often 0 or 1 but might be >1 if RepeatRate is small enough that DeltaTime > RepeatRate
    const char*   GetKeyName(ImGuiKey key);                                     // [DEBUG] returns English name of the key. Those names a provided for debugging purpose and are not meant to be saved persistently not compared.
    void          SetNextFrameWantCaptureKeyboard(bool want_capture_keyboard);  // Override io.WantCaptureKeyboard flag next frame (said flag is left for your application to handle, typically when true it instructs your app to ignore inputs). e.g. force capture keyboard when your widget is being hovered. This is equivalent to setting "io.WantCaptureKeyboard = want_capture_keyboard"; after the next NewFrame() call.

    // Inputs Utilities: Mouse specific
    bool          IsMouseDown(ImGuiMouseButton button);                         // is mouse button held?
    bool          IsMouseClicked(ImGuiMouseButton button, bool repeat = false); // did mouse button clicked? (went from !Down to Down). Same as GetMouseClickedCount() == 1.
    bool          IsMouseReleased(ImGuiMouseButton button);                     // did mouse button released? (went from Down to !Down)
    bool          IsMouseDoubleClicked(ImGuiMouseButton button);                // did mouse button double-clicked? Same as GetMouseClickedCount() == 2. (note that a double-click will also report IsMouseClicked() == true)
    int           GetMouseClickedCount(ImGuiMouseButton button);                // return the number of successive mouse-clicks at the time where a click happen (otherwise 0).
    bool          IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip = true);// is mouse hovering given bounding rect (in screen space). clipped by current clipping settings, but disregarding of other consideration of focus/window ordering/popup-block.
    bool          IsMousePosValid(const ImVec2* mouse_pos = NULL);              // by convention we use (-FLT_MAX,-FLT_MAX) to denote that there is no mouse available
    bool          IsAnyMouseDown();                                             // [WILL OBSOLETE] is any mouse button held? This was designed for backends, but prefer having backend maintain a mask of held mouse buttons, because upcoming input queue system will make this invalid.
    ImVec2        GetMousePos();                                                // shortcut to ImGui::GetIO().MousePos provided by user, to be consistent with other calls
    ImVec2        GetMousePosOnOpeningCurrentPopup();                           // retrieve mouse position at the time of opening popup we have BeginPopup() into (helper to avoid user backing that value themselves)
    bool          IsMouseDragging(ImGuiMouseButton button, float lock_threshold = -1.0f);         // is mouse dragging? (if lock_threshold < -1.0f, uses io.MouseDraggingThreshold)
    ImVec2        GetMouseDragDelta(ImGuiMouseButton button = 0, float lock_threshold = -1.0f);   // return the delta from the initial clicking position while the mouse button is pressed or was just released. This is locked and return 0.0f until the mouse moves past a distance threshold at least once (if lock_threshold < -1.0f, uses io.MouseDraggingThreshold)
    void          ResetMouseDragDelta(ImGuiMouseButton button = 0);             //
    ImGuiMouseCursor GetMouseCursor();                                          // get desired cursor type, reset in ImGui::NewFrame(), this is updated during the frame. valid before Render(). If you use software rendering by setting io.MouseDrawCursor ImGui will render those for you
    void          SetMouseCursor(ImGuiMouseCursor cursor_type);                 // set desired cursor type
    void          SetNextFrameWantCaptureMouse(bool want_capture_mouse);        // Override io.WantCaptureMouse flag next frame (said flag is left for your application to handle, typical when true it instucts your app to ignore inputs). This is equivalent to setting "io.WantCaptureMouse = want_capture_mouse;" after the next NewFrame() call.

    // Clipboard Utilities
    const char*   GetClipboardText();
    void          SetClipboardText(const char* text);

    // Settings/.Ini Utilities
    void          LoadIniSettingsFromDisk(const char* ini_filename);                  // call after CreateContext() and before the first call to NewFrame(). NewFrame() automatically calls LoadIniSettingsFromDisk(io.IniFilename).
    void          LoadIniSettingsFromMemory(const char* ini_data, size_t ini_size=0); // call after CreateContext() and before the first call to NewFrame() to provide .ini data from your own data source.
    void          SaveIniSettingsToDisk(const char* ini_filename);                    // this is automatically called (if io.IniFilename is not empty) a few seconds after any modification that should be reflected in the .ini file (and also by DestroyContext).
    const char*   SaveIniSettingsToMemory(size_t* out_ini_size = NULL);              // return a zero-terminated string with the .ini data which you can save by your own mean. call when io.WantSaveIniSettings is set, then save data by your own mean and clear io.WantSaveIniSettings.

    // Debug Utilities
    void          DebugTextEncoding(const char* text);
    bool          DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert, size_t sz_drawidx); // This is called by IMGUI_CHECKVERSION() macro.

    // Memory Allocators
    void          SetAllocatorFunctions(ImGuiMemAllocFunc alloc_func, ImGuiMemFreeFunc free_func, void* user_data = NULL);
    void          GetAllocatorFunctions(ImGuiMemAllocFunc* p_alloc_func, ImGuiMemFreeFunc* p_free_func, void** p_user_data);
    void*         MemAlloc(size_t size);
    void          MemFree(void* ptr);
}