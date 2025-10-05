#pragma once
#include <windows.h>
#include <string>
#include <vector>

struct ScriptInfo {
    std::string name;
    std::string content;
    bool isFavorite;
};

class ScarryGUI {
public:
    static bool Initialize();
    static void Shutdown();
    static void Render();
    static bool IsRunning();

    // Script management
    static void AddScript(const std::string& name, const std::string& content);
    static void ExecuteScript(const std::string& script);
    static void AddConsoleMessage(const std::string& message);

private:
    static bool CreateMainWindow();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static void RenderGUI();

    // UI components
    static void RenderAttachSection();
    static void RenderScriptEditor();
    static void RenderConsole();
    static void RenderScriptList();

    static bool AttachToRoblox();
    static void LoadScriptFromFile();
    static void SaveScriptToFile();

    // State
    static bool isAttached;
    static DWORD targetProcessId;
    static std::vector<std::string> consoleMessages;
    static std::vector<ScriptInfo> savedScripts;
    static char scriptInput[32768];
    static char scriptName[256];
};