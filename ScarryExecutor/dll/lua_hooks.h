#pragma once
#include <windows.h>
#include <lua.hpp>
#include <string>

// Lua API hook types
typedef int (__cdecl* lua_getfield_t)(lua_State* L, int idx, const char* k);
typedef int (__cdecl* lua_setfield_t)(lua_State* L, int idx, const char* k);
typedef void (__cdecl* lua_pushvalue_t)(lua_State* L, int idx);
typedef int (__cdecl* lua_pcall_t)(lua_State* L, int nargs, int nresults, int errfunc);
typedef void (__cdecl* lua_pushstring_t)(lua_State* L, const char* s);
typedef const char* (__cdecl* lua_tostring_t)(lua_State* L, int idx);

// Roblox Lua environment hooks
class LuaHooks {
public:
    static bool Initialize();
    static void Shutdown();

    static void ExecuteScript(const std::string& script);
    static void SendConsoleOutput(const std::string& output);

private:
    static lua_State* GetLuaState();
    static void HookLuaFunctions();

    // Hooked functions
    static int HookedLuaGetField(lua_State* L, int idx, const char* k);
    static int HookedLuaPCall(lua_State* L, int nargs, int nresults, int errfunc);

    // Original function pointers
    static lua_getfield_t OriginalLuaGetField;
    static lua_pcall_t OriginalLuaPCall;

    static bool hooksInstalled;
};