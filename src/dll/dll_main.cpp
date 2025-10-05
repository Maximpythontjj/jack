#include <Windows.h>
#include "lua_executor.h"
#include "../communication/pipe_manager.h"
#include "../utils/logger.h"
#include "hooks.h"

// Global instances
LuaExecutor* g_luaExecutor = nullptr;
PipeManager* g_pipeManager = nullptr;
Hooks* g_hooks = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            // Disable DLL thread notifications
            DisableThreadLibraryCalls(hModule);
            
            // Initialize logger
            Logger::Initialize();
            Logger::Log("SCARRY Internal DLL loaded into process PID: " + std::to_string(GetCurrentProcessId()));
            
            // Create a thread to initialize our components
            CreateThread(NULL, 0, [](LPVOID) -> DWORD {
                try
                {
                    Sleep(1000); // Give the process time to initialize
                    
                    // Initialize pipe communication
                    g_pipeManager = new PipeManager();
                    if (!g_pipeManager->ConnectToServer())
                    {
                        Logger::Log("Failed to connect to pipe server", Logger::Level::Error);
                        delete g_pipeManager;
                        g_pipeManager = nullptr;
                        return 1;
                    }
                    Logger::Log("Connected to pipe server successfully");
                    
                    // Initialize Lua executor
                    g_luaExecutor = new LuaExecutor();
                    if (!g_luaExecutor->Initialize())
                    {
                        Logger::Log("Failed to initialize Lua executor", Logger::Level::Error);
                        delete g_luaExecutor;
                        g_luaExecutor = nullptr;
                        return 1;
                    }
                    Logger::Log("Lua executor initialized successfully");
                    
                    // Set up print function redirect
                    g_luaExecutor->SetPrintCallback([](const std::string& output) {
                        if (g_pipeManager)
                        {
                            g_pipeManager->SendOutput(output);
                        }
                    });
                    
                    // Initialize hooks
                    g_hooks = new Hooks();
                    if (!g_hooks->Initialize())
                    {
                        Logger::Log("Failed to initialize hooks", Logger::Level::Warning);
                    }
                    else
                    {
                        Logger::Log("Hooks initialized successfully");
                    }
                    
                    // Main message loop
                    while (g_pipeManager && g_pipeManager->IsConnected())
                    {
                        std::string script;
                        if (g_pipeManager->ReadScript(script) && g_luaExecutor)
                        {
                            Logger::Log("Executing script: " + script.substr(0, 50) + "...");
                            
                            std::string result;
                            if (g_luaExecutor->ExecuteScript(script, result))
                            {
                                if (!result.empty())
                                {
                                    g_pipeManager->SendOutput(result);
                                }
                            }
                            else
                            {
                                g_pipeManager->SendOutput("[ERROR] Script execution failed: " + result);
                            }
                        }
                        
                        Sleep(50); // Small delay to prevent excessive CPU usage
                    }
                    
                    Logger::Log("DLL message loop exited");
                }
                catch (const std::exception& e)
                {
                    Logger::Log("Exception in DLL thread: " + std::string(e.what()), Logger::Level::Error);
                }
                
                return 0;
            }, NULL, 0, NULL);
        }
        break;
        
    case DLL_PROCESS_DETACH:
        {
            Logger::Log("SCARRY Internal DLL unloading...");
            
            // Cleanup hooks
            if (g_hooks)
            {
                g_hooks->Cleanup();
                delete g_hooks;
                g_hooks = nullptr;
            }
            
            // Cleanup Lua executor
            if (g_luaExecutor)
            {
                g_luaExecutor->Cleanup();
                delete g_luaExecutor;
                g_luaExecutor = nullptr;
            }
            
            // Cleanup pipe manager
            if (g_pipeManager)
            {
                g_pipeManager->Disconnect();
                delete g_pipeManager;
                g_pipeManager = nullptr;
            }
            
            Logger::Log("SCARRY Internal DLL unloaded");
            Logger::Shutdown();
        }
        break;
    }
    
    return TRUE;
}