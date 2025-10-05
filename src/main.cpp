#include <Windows.h>
#include <iostream>
#include <memory>
#include "gui/gui_manager.h"
#include "process/process_manager.h"
#include "injector/dll_injector.h"
#include "communication/pipe_manager.h"
#include "utils/logger.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize logger
    Logger::Initialize();
    Logger::Log("SCARRY EXECUTOR v1.0.0 Starting...");

    try
    {
        // Initialize GUI
        auto guiManager = std::make_unique<GuiManager>();
        if (!guiManager->Initialize(hInstance))
        {
            Logger::Log("Failed to initialize GUI", Logger::Level::Error);
            return -1;
        }

        // Initialize process manager
        auto processManager = std::make_unique<ProcessManager>();
        
        // Initialize DLL injector
        auto dllInjector = std::make_unique<DllInjector>();
        
        // Initialize communication pipe
        auto pipeManager = std::make_unique<PipeManager>();

        // Set up GUI callbacks
        guiManager->SetAttachCallback([&]() -> bool {
            DWORD pid = processManager->FindRobloxProcess();
            if (pid == 0)
            {
                Logger::Log("Roblox process not found!", Logger::Level::Error);
                return false;
            }

            Logger::Log("Found Roblox process with PID: " + std::to_string(pid));
            
            if (!dllInjector->InjectDLL(pid, "ScarryInternal.dll"))
            {
                Logger::Log("Failed to inject DLL", Logger::Level::Error);
                return false;
            }

            if (!pipeManager->CreateServer())
            {
                Logger::Log("Failed to create communication pipe", Logger::Level::Error);
                return false;
            }

            Logger::Log("Successfully attached to Roblox!");
            return true;
        });

        guiManager->SetExecuteCallback([&](const std::string& script) -> bool {
            if (!pipeManager->IsConnected())
            {
                Logger::Log("Not connected to injected DLL", Logger::Level::Error);
                return false;
            }

            return pipeManager->SendScript(script);
        });

        guiManager->SetPipeManager(pipeManager.get());

        // Main message loop
        Logger::Log("SCARRY EXECUTOR GUI initialized successfully");
        guiManager->Run();

        Logger::Log("SCARRY EXECUTOR shutting down...");
    }
    catch (const std::exception& e)
    {
        Logger::Log("Fatal error: " + std::string(e.what()), Logger::Level::Error);
        MessageBoxA(NULL, e.what(), "SCARRY EXECUTOR - Fatal Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    return 0;
}

// Console entry point for debugging
int main()
{
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOW);
}