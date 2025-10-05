#include "lua_hooks.h"
#include "memory_utils.h"
#include "../include/ipc.h"
#include <iostream>
#include <string>

// Original function pointers
lua_getfield_t LuaHooks::OriginalLuaGetField = nullptr;
lua_pcall_t LuaHooks::OriginalLuaPCall = nullptr;
bool LuaHooks::hooksInstalled = false;

// Lua state
lua_State* g_LuaState = nullptr;

// Pattern for finding Lua functions in Roblox
const char* LUACALL_PATTERN = "\x55\x8B\xEC\x83\xEC\x00\x8B\x45\x00\x83\xF8\x00\x7C\x00\x8B\x4D\x00\x8D\x04\x00\x3B\xC8\x7C\x00\x8B\x55\x00\x8B\x04\x00\x8B\x00\x00\x00\x00\x00\x89\x45\x00";
const char* LUACALL_MASK = "xxxxxx?xx?xx?x?xx?xx?xx?xxx?xxxx?xx?xxx?xx?xx?x?xx?xxx?x";

// Function to find Lua state in Roblox process
lua_State* FindLuaState() {
    DWORD processId = GetCurrentProcessId();
    HANDLE hProcess = GetCurrentProcess();

    // Get RobloxPlayerBeta.exe base address
    uintptr_t robloxBase = MemoryUtils::GetModuleBaseAddress(processId, "RobloxPlayerBeta.exe");
    if (robloxBase == 0) {
        std::cerr << "[SCARRY] Failed to find Roblox base address" << std::endl;
        return nullptr;
    }

    // Scan for Lua state - this is a simplified approach
    // In reality, you'd need specific patterns for different Roblox versions
    uintptr_t scanStart = robloxBase;
    uintptr_t scanEnd = robloxBase + 0x1000000; // Scan first 16MB

    // Look for Lua state pointer pattern
    // This is a very basic pattern - real implementation needs version-specific patterns
    BYTE luaStatePattern[] = { 0xC7, 0x45, 0xFC, 0x00, 0x00, 0x00, 0x00 }; // mov [ebp-4], 0x0
    const char* luaStateMask = "xxx?????"; // First 3 bytes matter, next 4 are variable

    uintptr_t foundAddress = MemoryUtils::FindPattern(hProcess, scanStart, scanEnd, (const char*)luaStatePattern, luaStateMask);
    if (foundAddress != 0) {
        // Read the Lua state pointer from memory
        uintptr_t luaStatePtr;
        if (MemoryUtils::ReadMemory(hProcess, foundAddress + 3, &luaStatePtr, sizeof(uintptr_t))) {
            return (lua_State*)luaStatePtr;
        }
    }

    return nullptr;
}

bool LuaHooks::Initialize() {
    std::cout << "[SCARRY] Initializing Lua hooks..." << std::endl;

    // Find Lua state
    g_LuaState = FindLuaState();
    if (g_LuaState == nullptr) {
        std::cerr << "[SCARRY] Failed to find Lua state" << std::endl;
        return false;
    }

    std::cout << "[SCARRY] Found Lua state at: " << g_LuaState << std::endl;

    // Hook Lua functions
    HookLuaFunctions();

    hooksInstalled = true;
    std::cout << "[SCARRY] Lua hooks initialized successfully" << std::endl;

    return true;
}

void LuaHooks::Shutdown() {
    if (hooksInstalled) {
        // Restore original functions if needed
        hooksInstalled = false;
    }
}

void LuaHooks::ExecuteScript(const std::string& script) {
    if (g_LuaState == nullptr) {
        std::cerr << "[SCARRY] No Lua state available" << std::endl;
        return;
    }

    std::cout << "[SCARRY] Executing script: " << script.substr(0, 100) << "..." << std::endl;

    // Push the script to Lua stack
    lua_pushstring(g_LuaState, script.c_str());

    // Load the script
    if (luaL_loadstring(g_LuaState, script.c_str()) != 0) {
        const char* error = lua_tostring(g_LuaState, -1);
        std::cerr << "[SCARRY] Script load error: " << error << std::endl;

        // Send error to GUI
        IPCManager::SendConsoleMessage(std::string("Script load error: ") + error);
        lua_pop(g_LuaState, 1); // Remove error from stack
        return;
    }

    // Execute the script
    if (lua_pcall(g_LuaState, 0, 0, 0) != 0) {
        const char* error = lua_tostring(g_LuaState, -1);
        std::cerr << "[SCARRY] Script execution error: " << error << std::endl;

        // Send error to GUI
        IPCManager::SendConsoleMessage(std::string("Script execution error: ") + error);
        lua_pop(g_LuaState, 1); // Remove error from stack
        return;
    }

    // Send success message to GUI
    IPCManager::SendConsoleMessage("Script executed successfully");
    std::cout << "[SCARRY] Script executed successfully" << std::endl;
}

void LuaHooks::SendConsoleOutput(const std::string& output) {
    IPCManager::SendConsoleMessage(output);
}

lua_State* LuaHooks::GetLuaState() {
    return g_LuaState;
}

void LuaHooks::HookLuaFunctions() {
    // This is a simplified hooking approach
    // In a real implementation, you'd use a proper hooking library like MinHook

    // For now, we'll just use the original functions
    // Real hooking would require finding the function addresses and replacing them

    std::cout << "[SCARRY] Lua function hooking initialized (simplified)" << std::endl;
}

// Hooked Lua functions (these would be replaced with actual hooks in a real implementation)
int LuaHooks::HookedLuaGetField(lua_State* L, int idx, const char* k) {
    // Call original function
    int result = OriginalLuaGetField(L, idx, k);

    // Log field access if needed
    std::cout << "[SCARRY] LuaGetField called: " << (k ? k : "nil") << std::endl;

    return result;
}

int LuaHooks::HookedLuaPCall(lua_State* L, int nargs, int nresults, int errfunc) {
    // Call original function
    int result = OriginalLuaPCall(L, nargs, nresults, errfunc);

    // Log pcall if needed
    if (result != 0) {
        const char* error = lua_tostring(L, -1);
        std::cout << "[SCARRY] LuaPCall error: " << error << std::endl;
    }

    return result;
}