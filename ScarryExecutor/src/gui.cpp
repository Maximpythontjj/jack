#include "gui.h"
#include "process_utils.h"
#include "injector.h"
#include "ipc.h"
#include <iostream>
#include <fstream>
#include <commctrl.h>

// Global GUI state
bool ScarryGUI::isAttached = false;
DWORD ScarryGUI::targetProcessId = 0;
std::vector<std::string> ScarryGUI::consoleMessages;
std::vector<ScriptInfo> ScarryGUI::savedScripts;
char ScarryGUI::scriptInput[32768] = {0};
char ScarryGUI::scriptName[256] = {0};

// Window handles
HWND g_hMainWindow = NULL;
HWND g_hAttachButton = NULL;
HWND g_hProcessCombo = NULL;
HWND g_hScriptEdit = NULL;
HWND g_hExecuteButton = NULL;
HWND g_hConsoleEdit = NULL;
HWND g_hScriptList = NULL;

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

bool ScarryGUI::Initialize() {
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    // Register window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "ScarryExecutorClass";

    if (!RegisterClassEx(&wc)) {
        std::cerr << "Failed to register window class" << std::endl;
        return false;
    }

    // Create main window
    if (!CreateMainWindow()) {
        return false;
    }

    // Initialize IPC
    if (!IPCManager::Initialize()) {
        std::cerr << "Failed to initialize IPC" << std::endl;
        return false;
    }

    // Initialize script input with default content
    strcpy_s(scriptInput, "print(\"Hello from SCARRY EXECUTOR!\")");

    ShowWindow(g_hMainWindow, SW_SHOW);
    UpdateWindow(g_hMainWindow);

    return true;
}

void ScarryGUI::Shutdown() {
    IPCManager::Shutdown();

    if (g_hMainWindow) {
        DestroyWindow(g_hMainWindow);
    }

    UnregisterClass("ScarryExecutorClass", GetModuleHandle(NULL));
}

void ScarryGUI::Render() {
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

bool ScarryGUI::IsRunning() {
    return IsWindow(g_hMainWindow) && g_hMainWindow != NULL;
}

bool ScarryGUI::CreateMainWindow() {
    g_hMainWindow = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "ScarryExecutorClass",
        "SCARRY EXECUTOR",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (g_hMainWindow == NULL) {
        std::cerr << "Failed to create main window" << std::endl;
        return false;
    }

    // Create controls
    CreateWindow("BUTTON", "Attach to Roblox",
                 WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 10, 10, 120, 30,
                 g_hMainWindow, (HMENU)1001, GetModuleHandle(NULL), NULL);

    CreateWindow("COMBOBOX", "",
                 WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                 140, 10, 200, 100,
                 g_hMainWindow, (HMENU)1002, GetModuleHandle(NULL), NULL);

    CreateWindow("BUTTON", "Execute",
                 WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 10, 50, 80, 25,
                 g_hMainWindow, (HMENU)1003, GetModuleHandle(NULL), NULL);

    CreateWindow("BUTTON", "Clear",
                 WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                 100, 50, 80, 25,
                 g_hMainWindow, (HMENU)1004, GetModuleHandle(NULL), NULL);

    CreateWindow("EDIT", "",
                 WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN,
                 10, 85, 580, 200,
                 g_hMainWindow, (HMENU)1005, GetModuleHandle(NULL), NULL);

    CreateWindow("EDIT", "",
                 WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
                 10, 295, 580, 150,
                 g_hMainWindow, (HMENU)1006, GetModuleHandle(NULL), NULL);

    CreateWindow("LISTBOX", "",
                 WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL,
                 600, 85, 180, 360,
                 g_hMainWindow, (HMENU)1007, GetModuleHandle(NULL), NULL);

    // Store handles for easy access
    g_hAttachButton = GetDlgItem(g_hMainWindow, 1001);
    g_hProcessCombo = GetDlgItem(g_hMainWindow, 1002);
    g_hExecuteButton = GetDlgItem(g_hMainWindow, 1003);
    g_hScriptEdit = GetDlgItem(g_hMainWindow, 1005);
    g_hConsoleEdit = GetDlgItem(g_hMainWindow, 1006);
    g_hScriptList = GetDlgItem(g_hMainWindow, 1007);

    // Set default text
    SetWindowText(g_hScriptEdit, scriptInput);

    return true;
}

LRESULT CALLBACK ScarryGUI::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Populate process combo box
            std::vector<ProcessInfo> processes = ProcessUtils::GetRobloxProcesses();
            for (const auto& process : processes) {
                std::string item = process.exeName + " (PID: " + std::to_string(process.processId) + ")";
                SendMessage(g_hProcessCombo, CB_ADDSTRING, 0, (LPARAM)item.c_str());
            }
            break;
        }

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case 1001: // Attach button
                    AttachToRoblox();
                    break;

                case 1003: // Execute button
                    if (isAttached) {
                        GetWindowText(g_hScriptEdit, scriptInput, sizeof(scriptInput));
                        std::string script(scriptInput);
                        if (!script.empty()) {
                            IPCManager::SendScript(script);
                            AddConsoleMessage("Script sent for execution...");
                        }
                    } else {
                        AddConsoleMessage("Please attach to Roblox first");
                    }
                    break;

                case 1004: // Clear button
                    SetWindowText(g_hConsoleEdit, "");
                    consoleMessages.clear();
                    break;
            }
            break;
        }

        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

bool ScarryGUI::AttachToRoblox() {
    int selectedIndex = SendMessage(g_hProcessCombo, CB_GETCURSEL, 0, 0);
    if (selectedIndex == CB_ERR) {
        AddConsoleMessage("Please select a Roblox process first");
        return false;
    }

    char selectedText[256];
    SendMessage(g_hProcessCombo, CB_GETLBTEXT, selectedIndex, (LPARAM)selectedText);

    // Extract PID from text (format: "RobloxPlayerBeta.exe (PID: XXXXX)")
    std::string text(selectedText);
    size_t pidStart = text.find("PID: ") + 5;
    size_t pidEnd = text.find(")", pidStart);
    std::string pidStr = text.substr(pidStart, pidEnd - pidStart);

    DWORD processId = std::stoi(pidStr);

    if (!ProcessUtils::IsProcessRunning(processId)) {
        AddConsoleMessage("Selected process is not running");
        return false;
    }

    // Get current directory for DLL path
    char currentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDir);
    std::string dllPath = std::string(currentDir) + "\\ScarryHook.dll";

    AddConsoleMessage("Injecting DLL into process " + std::to_string(processId) + "...");

    if (DLLInjector::InjectDLL(processId, dllPath)) {
        isAttached = true;
        targetProcessId = processId;
        AddConsoleMessage("Successfully attached to Roblox!");

        // Enable execute button
        EnableWindow(g_hExecuteButton, TRUE);

        // Update attach button text
        SetWindowText(g_hAttachButton, "Detach");
    } else {
        AddConsoleMessage("Failed to inject DLL");
    }

    return isAttached;
}

void ScarryGUI::AddConsoleMessage(const std::string& message) {
    consoleMessages.push_back(message);

    // Update console display (show last 100 messages)
    std::string consoleText;
    size_t start = (consoleMessages.size() > 100) ? consoleMessages.size() - 100 : 0;

    for (size_t i = start; i < consoleMessages.size(); i++) {
        consoleText += consoleMessages[i] + "\r\n";
    }

    SetWindowText(g_hConsoleEdit, consoleText.c_str());

    // Scroll to bottom
    SendMessage(g_hConsoleEdit, EM_SETSEL, consoleText.length(), consoleText.length());
    SendMessage(g_hConsoleEdit, EM_SCROLLCARET, 0, 0);
}

void ScarryGUI::AddScript(const std::string& name, const std::string& content) {
    ScriptInfo script = {name, content, false};
    savedScripts.push_back(script);

    // Update script list
    SendMessage(g_hScriptList, LB_RESETCONTENT, 0, 0);
    for (const auto& s : savedScripts) {
        SendMessage(g_hScriptList, LB_ADDSTRING, 0, (LPARAM)s.name.c_str());
    }
}

void ScarryGUI::ExecuteScript(const std::string& script) {
    if (isAttached && !script.empty()) {
        IPCManager::SendScript(script);
        AddConsoleMessage("Script sent for execution...");
    }
}