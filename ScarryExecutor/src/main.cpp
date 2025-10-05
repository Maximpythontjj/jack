#include <windows.h>
#include <iostream>
#include <string>
#include "gui.h"
#include "process_utils.h"
#include "injector.h"
#include "ipc.h"

// Global variables
bool g_Running = true;

// Console control handler
BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType) {
    switch (dwCtrlType) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            g_Running = false;
            ScarryGUI::Shutdown();
            return TRUE;
        default:
            return FALSE;
    }
}

int main() {
    // Set console title
    SetConsoleTitle("SCARRY EXECUTOR - Debug Console");

    std::cout << "=============================================" << std::endl;
    std::cout << "        SCARRY EXECUTOR v1.0" << std::endl;
    std::cout << "  Educational Roblox Executor for Testing" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;

    // Set console control handler
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

    // Check if DLL exists
    if (GetFileAttributes("ScarryHook.dll") == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Warning: ScarryHook.dll not found in current directory" << std::endl;
        std::cerr << "Please ensure the DLL is built and placed in the same directory as the executable" << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Initializing GUI..." << std::endl;

    // Initialize GUI
    if (!ScarryGUI::Initialize()) {
        std::cerr << "Failed to initialize GUI" << std::endl;
        return 1;
    }

    std::cout << "GUI initialized successfully" << std::endl;
    std::cout << "Waiting for user interaction..." << std::endl;
    std::cout << std::endl;

    // Main message loop
    while (g_Running && ScarryGUI::IsRunning()) {
        ScarryGUI::Render();

        // Check for Roblox processes periodically
        static DWORD lastCheck = 0;
        DWORD currentTime = GetTickCount();

        if (currentTime - lastCheck > 5000) { // Check every 5 seconds
            std::vector<ProcessInfo> processes = ProcessUtils::GetRobloxProcesses();
            if (!processes.empty()) {
                std::cout << "Found " << processes.size() << " Roblox process(es):" << std::endl;
                for (const auto& process : processes) {
                    std::cout << "  - " << process.exeName << " (PID: " << process.processId << ")" << std::endl;
                }
                std::cout << std::endl;
            }
            lastCheck = currentTime;
        }

        Sleep(100); // Small delay to prevent busy waiting
    }

    std::cout << "Shutting down..." << std::endl;
    ScarryGUI::Shutdown();

    std::cout << "SCARRY EXECUTOR terminated" << std::endl;
    return 0;
}