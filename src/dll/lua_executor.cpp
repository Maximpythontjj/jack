#include "lua_executor.h"
#include "../utils/logger.h"
#include <sstream>
#include <thread>

LuaExecutor::LuaExecutor()
    : m_luaState(nullptr)
{
}

LuaExecutor::~LuaExecutor()
{
    Cleanup();
}

bool LuaExecutor::Initialize()
{
    Logger::Log("Initializing Lua executor...");
    
    // Create new Lua state
    m_luaState = luaL_newstate();
    if (!m_luaState)
    {
        Logger::Log("Failed to create Lua state", Logger::Level::Error);
        return false;
    }
    
    // Load standard libraries
    luaL_openlibs(m_luaState);
    
    // Register custom functions
    RegisterGlobalFunctions();
    RegisterRobloxAPI();
    
    Logger::Log("Lua executor initialized successfully");
    return true;
}

void LuaExecutor::Cleanup()
{
    if (m_luaState)
    {
        lua_close(m_luaState);
        m_luaState = nullptr;
    }
}

bool LuaExecutor::ExecuteScript(const std::string& script, std::string& result)
{
    if (!m_luaState)
    {
        result = "Lua state not initialized";
        return false;
    }
    
    // Store reference to this instance for callbacks
    lua_pushlightuserdata(m_luaState, this);
    lua_setglobal(m_luaState, "__executor_instance");
    
    // Load and execute the script
    int loadResult = luaL_loadstring(m_luaState, script.c_str());
    if (loadResult != LUA_OK)
    {
        result = GetLuaError(m_luaState);
        return false;
    }
    
    int execResult = lua_pcall(m_luaState, 0, LUA_MULTRET, 0);
    if (execResult != LUA_OK)
    {
        result = GetLuaError(m_luaState);
        return false;
    }
    
    result = "Script executed successfully";
    return true;
}

void LuaExecutor::RegisterGlobalFunctions()
{
    // Override print function
    lua_pushcfunction(m_luaState, lua_print);
    lua_setglobal(m_luaState, "print");
    
    // Add warn function
    lua_pushcfunction(m_luaState, lua_warn);
    lua_setglobal(m_luaState, "warn");
    
    // Add wait function
    lua_pushcfunction(m_luaState, lua_wait);
    lua_setglobal(m_luaState, "wait");
    
    // Add spawn function
    lua_pushcfunction(m_luaState, lua_spawn);
    lua_setglobal(m_luaState, "spawn");
    
    // Add delay function
    lua_pushcfunction(m_luaState, lua_delay);
    lua_setglobal(m_luaState, "delay");
}

void LuaExecutor::RegisterRobloxAPI()
{
    // Create game object
    PushRobloxGame(m_luaState);
    lua_setglobal(m_luaState, "game");
    
    // Create workspace reference
    PushRobloxGame(m_luaState);
    lua_setglobal(m_luaState, "workspace");
}

int LuaExecutor::lua_print(lua_State* L)
{
    // Get executor instance
    lua_getglobal(L, "__executor_instance");
    LuaExecutor* executor = static_cast<LuaExecutor*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    
    if (!executor || !executor->m_printCallback)
        return 0;
    
    std::stringstream ss;
    int argc = lua_gettop(L);
    
    for (int i = 1; i <= argc; ++i)
    {
        if (i > 1) ss << "\t";
        
        const char* str = luaL_tolstring(L, i, nullptr);
        ss << (str ? str : "nil");
        lua_pop(L, 1); // Remove the string created by luaL_tolstring
    }
    
    executor->m_printCallback(ss.str());
    return 0;
}

int LuaExecutor::lua_warn(lua_State* L)
{
    // Get executor instance
    lua_getglobal(L, "__executor_instance");
    LuaExecutor* executor = static_cast<LuaExecutor*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    
    if (!executor || !executor->m_printCallback)
        return 0;
    
    std::stringstream ss;
    ss << "[WARN] ";
    
    int argc = lua_gettop(L);
    for (int i = 1; i <= argc; ++i)
    {
        if (i > 1) ss << "\t";
        
        const char* str = luaL_tolstring(L, i, nullptr);
        ss << (str ? str : "nil");
        lua_pop(L, 1);
    }
    
    executor->m_printCallback(ss.str());
    return 0;
}

int LuaExecutor::lua_wait(lua_State* L)
{
    double seconds = 0.03; // Default Roblox wait time
    
    if (lua_gettop(L) > 0 && lua_isnumber(L, 1))
    {
        seconds = lua_tonumber(L, 1);
    }
    
    // Convert to milliseconds and sleep
    int ms = static_cast<int>(seconds * 1000);
    if (ms > 0)
    {
        Sleep(ms);
    }
    
    lua_pushnumber(L, seconds);
    return 1;
}

int LuaExecutor::lua_spawn(lua_State* L)
{
    if (lua_gettop(L) < 1 || !lua_isfunction(L, 1))
    {
        luaL_error(L, "spawn expects a function");
        return 0;
    }
    
    // For now, just execute immediately
    // In a real implementation, this would create a new thread
    lua_call(L, 0, 0);
    return 0;
}

int LuaExecutor::lua_delay(lua_State* L)
{
    double delayTime = 0.0;
    
    if (lua_gettop(L) < 2)
    {
        luaL_error(L, "delay expects delay time and function");
        return 0;
    }
    
    if (lua_isnumber(L, 1))
    {
        delayTime = lua_tonumber(L, 1);
    }
    
    if (!lua_isfunction(L, 2))
    {
        luaL_error(L, "delay expects a function as second argument");
        return 0;
    }
    
    // Simple implementation - sleep then execute
    int ms = static_cast<int>(delayTime * 1000);
    if (ms > 0)
    {
        Sleep(ms);
    }
    
    lua_pushvalue(L, 2); // Copy function to top of stack
    lua_call(L, 0, 0);
    return 0;
}

int LuaExecutor::lua_getService(lua_State* L)
{
    if (lua_gettop(L) < 2 || !lua_isstring(L, 2))
    {
        luaL_error(L, "GetService expects service name");
        return 0;
    }
    
    const char* serviceName = lua_tostring(L, 2);
    
    // Create a mock service table
    lua_newtable(L);
    
    // Set service name
    lua_pushstring(L, serviceName);
    lua_setfield(L, -2, "Name");
    
    // Add common methods for different services
    if (strcmp(serviceName, "Players") == 0)
    {
        lua_newtable(L);
        lua_setfield(L, -2, "LocalPlayer");
    }
    else if (strcmp(serviceName, "Workspace") == 0)
    {
        lua_newtable(L);
        lua_setfield(L, -2, "CurrentCamera");
    }
    else if (strcmp(serviceName, "RunService") == 0)
    {
        // Add RunService events (mock)
        lua_newtable(L);
        lua_setfield(L, -2, "Heartbeat");
    }
    
    return 1;
}

int LuaExecutor::lua_findFirstChild(lua_State* L)
{
    if (lua_gettop(L) < 2 || !lua_isstring(L, 2))
    {
        lua_pushnil(L);
        return 1;
    }
    
    // Mock implementation - return nil for now
    lua_pushnil(L);
    return 1;
}

std::string LuaExecutor::GetLuaError(lua_State* L)
{
    if (lua_isstring(L, -1))
    {
        std::string error = lua_tostring(L, -1);
        lua_pop(L, 1);
        return error;
    }
    
    lua_pop(L, 1);
    return "Unknown Lua error";
}

void LuaExecutor::PushRobloxGame(lua_State* L)
{
    lua_newtable(L);
    
    // Add GetService method
    lua_pushcfunction(L, lua_getService);
    lua_setfield(L, -2, "GetService");
    
    // Add FindFirstChild method
    lua_pushcfunction(L, lua_findFirstChild);
    lua_setfield(L, -2, "FindFirstChild");
    
    // Add Name property
    lua_pushstring(L, "Game");
    lua_setfield(L, -2, "Name");
}