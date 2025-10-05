#pragma once

#include <string>
#include <functional>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class LuaExecutor
{
public:
    using PrintCallback = std::function<void(const std::string&)>;

    LuaExecutor();
    ~LuaExecutor();

    bool Initialize();
    void Cleanup();
    
    bool ExecuteScript(const std::string& script, std::string& result);
    void SetPrintCallback(PrintCallback callback) { m_printCallback = callback; }
    
    // Roblox-specific functions
    void RegisterRobloxAPI();
    void RegisterGlobalFunctions();

private:
    lua_State* m_luaState;
    PrintCallback m_printCallback;
    
    // Lua C functions
    static int lua_print(lua_State* L);
    static int lua_warn(lua_State* L);
    static int lua_wait(lua_State* L);
    static int lua_spawn(lua_State* L);
    static int lua_delay(lua_State* L);
    
    // Roblox game functions
    static int lua_getService(lua_State* L);
    static int lua_findFirstChild(lua_State* L);
    
    // Utility functions
    std::string GetLuaError(lua_State* L);
    void PushRobloxGame(lua_State* L);
};