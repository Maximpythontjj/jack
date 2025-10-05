#include "hooks.h"
#include "../utils/logger.h"
#include <Psapi.h>
#include <vector>

// Static member definitions
Hooks::lua_getfield_t Hooks::original_lua_getfield = nullptr;
Hooks::lua_pcall_t Hooks::original_lua_pcall = nullptr;
Hooks::lua_pushstring_t Hooks::original_lua_pushstring = nullptr;

Hooks::Hooks()
    : m_vehHandle(nullptr)
{
}

Hooks::~Hooks()
{
    Cleanup();
}

bool Hooks::Initialize()
{
    Logger::Log("Initializing hooks...");
    
    try
    {
        // Install vectored exception handler
        m_vehHandle = AddVectoredExceptionHandler(1, VectoredExceptionHandler);
        if (!m_vehHandle)
        {
            Logger::Log("Failed to install vectored exception handler", Logger::Level::Warning);
        }
        
        // Find Roblox module base
        LPVOID robloxBase = GetModuleBaseAddress("RobloxPlayerBeta.exe");
        if (!robloxBase)
        {
            Logger::Log("Failed to find Roblox module base", Logger::Level::Warning);
            return true; // Continue without hooks
        }
        
        size_t moduleSize = GetModuleSize(robloxBase);
        Logger::Log("Roblox module base: 0x" + std::to_string(reinterpret_cast<uintptr_t>(robloxBase)) + 
                   ", size: " + std::to_string(moduleSize));
        
        // Pattern signatures for Lua functions (these would need to be updated for current Roblox)
        // These are placeholder patterns - real implementation would need proper reverse engineering
        
        // Try to find lua_getfield
        LPVOID lua_getfield_addr = FindPattern(
            "\x55\x8B\xEC\x83\xEC\x10\x56\x57\x8B\x7D\x08",
            "xxxxxxxxxxx",
            robloxBase,
            moduleSize
        );
        
        if (lua_getfield_addr)
        {
            if (InstallHook("lua_getfield", lua_getfield_addr, 
                          (LPVOID)lua_getfield_hook, (LPVOID*)&original_lua_getfield))
            {
                Logger::Log("Successfully hooked lua_getfield at 0x" + 
                           std::to_string(reinterpret_cast<uintptr_t>(lua_getfield_addr)));
            }
        }
        else
        {
            Logger::Log("Could not find lua_getfield function", Logger::Level::Warning);
        }
        
        // Try to find lua_pcall
        LPVOID lua_pcall_addr = FindPattern(
            "\x55\x8B\xEC\x83\xEC\x14\x53\x56\x57\x8B\x7D\x08",
            "xxxxxxxxxxxx",
            robloxBase,
            moduleSize
        );
        
        if (lua_pcall_addr)
        {
            if (InstallHook("lua_pcall", lua_pcall_addr, 
                          (LPVOID)lua_pcall_hook, (LPVOID*)&original_lua_pcall))
            {
                Logger::Log("Successfully hooked lua_pcall at 0x" + 
                           std::to_string(reinterpret_cast<uintptr_t>(lua_pcall_addr)));
            }
        }
        else
        {
            Logger::Log("Could not find lua_pcall function", Logger::Level::Warning);
        }
        
        Logger::Log("Hook initialization completed");
        return true;
    }
    catch (const std::exception& e)
    {
        Logger::Log("Exception during hook initialization: " + std::string(e.what()), Logger::Level::Error);
        return false;
    }
}

void Hooks::Cleanup()
{
    Logger::Log("Cleaning up hooks...");
    
    // Uninstall all hooks
    for (auto& hook : m_hooks)
    {
        UninstallHook(hook.first);
    }
    m_hooks.clear();
    
    // Remove vectored exception handler
    if (m_vehHandle)
    {
        RemoveVectoredExceptionHandler(m_vehHandle);
        m_vehHandle = nullptr;
    }
    
    Logger::Log("Hooks cleanup completed");
}

LONG WINAPI Hooks::VectoredExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
    // Handle access violations that might occur during hooking
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
    {
        Logger::Log("Access violation caught by VEH at 0x" + 
                   std::to_string(reinterpret_cast<uintptr_t>(ExceptionInfo->ExceptionRecord->ExceptionAddress)),
                   Logger::Level::Debug);
    }
    
    return EXCEPTION_CONTINUE_SEARCH;
}

int __stdcall Hooks::lua_getfield_hook(void* L, int idx, const char* k)
{
    Logger::Log("lua_getfield called with key: " + std::string(k ? k : "NULL"), Logger::Level::Debug);
    
    // Call original function
    if (original_lua_getfield)
    {
        return original_lua_getfield(L, idx, k);
    }
    
    return 0;
}

int __stdcall Hooks::lua_pcall_hook(void* L, int nargs, int nresults, int errfunc)
{
    Logger::Log("lua_pcall called with " + std::to_string(nargs) + " arguments", Logger::Level::Debug);
    
    // Call original function
    if (original_lua_pcall)
    {
        return original_lua_pcall(L, nargs, nresults, errfunc);
    }
    
    return 0;
}

void __stdcall Hooks::lua_pushstring_hook(void* L, const char* s)
{
    Logger::Log("lua_pushstring called with: " + std::string(s ? s : "NULL"), Logger::Level::Debug);
    
    // Call original function
    if (original_lua_pushstring)
    {
        original_lua_pushstring(L, s);
    }
}

bool Hooks::InstallHook(const std::string& name, LPVOID target, LPVOID detour, LPVOID* original)
{
    if (m_hooks.find(name) != m_hooks.end())
    {
        Logger::Log("Hook " + name + " already installed", Logger::Level::Warning);
        return false;
    }
    
    try
    {
        // Simple trampoline hook implementation
        // In a real implementation, you'd use a proper hooking library like MinHook
        
        DWORD oldProtect;
        if (!VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            Logger::Log("Failed to change memory protection for " + name, Logger::Level::Error);
            return false;
        }
        
        // Save original bytes
        BYTE originalBytes[5];
        memcpy(originalBytes, target, 5);
        
        // Create JMP instruction to detour
        BYTE jmpInstruction[5];
        jmpInstruction[0] = 0xE9; // JMP opcode
        *reinterpret_cast<DWORD*>(&jmpInstruction[1]) = 
            reinterpret_cast<DWORD>(detour) - reinterpret_cast<DWORD>(target) - 5;
        
        // Write JMP instruction
        memcpy(target, jmpInstruction, 5);
        
        // Restore original protection
        VirtualProtect(target, 5, oldProtect, &oldProtect);
        
        // Store hook info
        HookInfo info;
        info.targetAddress = target;
        info.hookedFunction = detour;
        info.isInstalled = true;
        memcpy(&info.originalFunction, originalBytes, 5);
        
        m_hooks[name] = info;
        
        Logger::Log("Successfully installed hook: " + name);
        return true;
    }
    catch (const std::exception& e)
    {
        Logger::Log("Exception installing hook " + name + ": " + e.what(), Logger::Level::Error);
        return false;
    }
}

bool Hooks::UninstallHook(const std::string& name)
{
    auto it = m_hooks.find(name);
    if (it == m_hooks.end())
    {
        return false;
    }
    
    HookInfo& info = it->second;
    if (!info.isInstalled)
    {
        return false;
    }
    
    try
    {
        DWORD oldProtect;
        if (!VirtualProtect(info.targetAddress, 5, PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            Logger::Log("Failed to change memory protection for unhooking " + name, Logger::Level::Error);
            return false;
        }
        
        // Restore original bytes
        memcpy(info.targetAddress, &info.originalFunction, 5);
        
        // Restore original protection
        VirtualProtect(info.targetAddress, 5, oldProtect, &oldProtect);
        
        info.isInstalled = false;
        Logger::Log("Successfully uninstalled hook: " + name);
        return true;
    }
    catch (const std::exception& e)
    {
        Logger::Log("Exception uninstalling hook " + name + ": " + e.what(), Logger::Level::Error);
        return false;
    }
}

LPVOID Hooks::FindPattern(const std::string& pattern, const std::string& mask, LPVOID start, size_t size)
{
    if (!start || size == 0)
        return nullptr;
    
    const BYTE* patternBytes = reinterpret_cast<const BYTE*>(pattern.c_str());
    const char* maskStr = mask.c_str();
    size_t patternLength = pattern.length();
    
    BYTE* searchStart = reinterpret_cast<BYTE*>(start);
    BYTE* searchEnd = searchStart + size - patternLength;
    
    for (BYTE* current = searchStart; current <= searchEnd; ++current)
    {
        bool found = true;
        for (size_t i = 0; i < patternLength; ++i)
        {
            if (maskStr[i] == 'x' && current[i] != patternBytes[i])
            {
                found = false;
                break;
            }
        }
        
        if (found)
        {
            return current;
        }
    }
    
    return nullptr;
}

LPVOID Hooks::GetModuleBaseAddress(const std::string& moduleName)
{
    HMODULE hModule = GetModuleHandleA(moduleName.c_str());
    return reinterpret_cast<LPVOID>(hModule);
}

size_t Hooks::GetModuleSize(LPVOID moduleBase)
{
    if (!moduleBase)
        return 0;
    
    MODULEINFO moduleInfo;
    if (GetModuleInformation(GetCurrentProcess(), 
                           reinterpret_cast<HMODULE>(moduleBase), 
                           &moduleInfo, 
                           sizeof(moduleInfo)))
    {
        return moduleInfo.SizeOfImage;
    }
    
    return 0;
}