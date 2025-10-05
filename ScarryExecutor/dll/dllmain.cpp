#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "../include/ipc.h"
#include "../dll/lua_hooks.h"
#include "../dll/memory_utils.h"

// Global variables
HMODULE g_hModule = NULL;
bool g_Shutdown = false;

// Function to monitor shared memory for commands
void MonitorSharedMemory() {
    std::cout << "[SCARRY] DLL monitoring shared memory..." << std::endl;

    while (!g_Shutdown) {
        if (IPCManager::IsConnected()) {
            // Check for script execution command
            struct IPCData {
                char command[16];
                char data[4080];
            };

            IPCData* data = (IPCData*)IPCManager::GetSharedMemoryPointer();

            if (strcmp(data->command, "SCRIPT") == 0) {
                std::string script(data->data);
                std::cout << "[SCARRY] Executing script: " << script.substr(0, 50) << "..." << std::endl;

                // Execute the script
                LuaHooks::ExecuteScript(script);

                // Clear the command
                memset(data, 0, sizeof(IPCData));
            }
            else if (strcmp(data->command, "CONSOLE") == 0) {
                std::string message(data->data);
                std::cout << "[SCARRY] Console message: " << message << std::endl;

                // Process console message if needed
                memset(data, 0, sizeof(IPCData));
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Function to hide the DLL from process enumeration
void HideDLL() {
    // This is a basic implementation - in a real scenario you'd want more sophisticated hiding
    // For now, we'll just rename the module in memory if possible

    // Note: This is a simplified approach. Real DLL hiding requires more complex techniques
    // like unhooking from the module list or using manual mapping.
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            g_hModule = hModule;

            // Disable thread library calls to avoid issues
            DisableThreadLibraryCalls(hModule);

            // Initialize IPC
            if (!IPCManager::Initialize()) {
                std::cerr << "[SCARRY] Failed to initialize IPC" << std::endl;
                return FALSE;
            }

            // Initialize Lua hooks
            if (!LuaHooks::Initialize()) {
                std::cerr << "[SCARRY] Failed to initialize Lua hooks" << std::endl;
                return FALSE;
            }

            // Hide the DLL
            HideDLL();

            // Start monitoring thread
            std::thread(MonitorSharedMemory).detach();

            std::cout << "[SCARRY] DLL attached successfully!" << std::endl;
            break;
        }

        case DLL_PROCESS_DETACH: {
            g_Shutdown = true;

            // Cleanup
            LuaHooks::Shutdown();
            IPCManager::Shutdown();

            std::cout << "[SCARRY] DLL detached" << std::endl;
            break;
        }

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}