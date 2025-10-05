#pragma once

#include <Windows.h>
#include <string>
#include <unordered_map>

class Hooks
{
public:
    Hooks();
    ~Hooks();

    bool Initialize();
    void Cleanup();

private:
    struct HookInfo
    {
        LPVOID originalFunction;
        LPVOID hookedFunction;
        LPVOID targetAddress;
        bool isInstalled;
    };

    // Hook functions
    static LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo);
    
    // Roblox function hooks
    static int __stdcall lua_getfield_hook(void* L, int idx, const char* k);
    static int __stdcall lua_pcall_hook(void* L, int nargs, int nresults, int errfunc);
    static void __stdcall lua_pushstring_hook(void* L, const char* s);
    
    // Original function pointers
    typedef int(__stdcall* lua_getfield_t)(void*, int, const char*);
    typedef int(__stdcall* lua_pcall_t)(void*, int, int, int);
    typedef void(__stdcall* lua_pushstring_t)(void*, const char*);
    
    static lua_getfield_t original_lua_getfield;
    static lua_pcall_t original_lua_pcall;
    static lua_pushstring_t original_lua_pushstring;

    std::unordered_map<std::string, HookInfo> m_hooks;
    PVOID m_vehHandle;
    
    bool InstallHook(const std::string& name, LPVOID target, LPVOID detour, LPVOID* original);
    bool UninstallHook(const std::string& name);
    LPVOID FindPattern(const std::string& pattern, const std::string& mask, LPVOID start = nullptr, size_t size = 0);
    LPVOID GetModuleBaseAddress(const std::string& moduleName);
    size_t GetModuleSize(LPVOID moduleBase);
};