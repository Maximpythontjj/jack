#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

#define LUA_OK 0
#define LUA_MULTRET (-1)

// Basic Lua API functions
lua_State* luaL_newstate(void);
void lua_close(lua_State* L);
void luaL_openlibs(lua_State* L);
int luaL_loadstring(lua_State* L, const char* s);
int lua_pcall(lua_State* L, int nargs, int nresults, int msgh);

// Stack manipulation
int lua_gettop(lua_State* L);
void lua_pop(lua_State* L, int n);
void lua_pushvalue(lua_State* L, int idx);

// Type checking
int lua_isnumber(lua_State* L, int idx);
int lua_isstring(lua_State* L, int idx);
int lua_isfunction(lua_State* L, int idx);

// Value access
double lua_tonumber(lua_State* L, int idx);
const char* lua_tolstring(lua_State* L, int idx, size_t* len);
#define lua_tostring(L,i) lua_tolstring(L, (i), NULL)
void* lua_touserdata(lua_State* L, int idx);

// Value pushing
void lua_pushnumber(lua_State* L, double n);
void lua_pushstring(lua_State* L, const char* s);
void lua_pushcfunction(lua_State* L, int (*f)(lua_State*));
void lua_pushlightuserdata(lua_State* L, void* p);
void lua_pushnil(lua_State* L);

// Table operations
void lua_newtable(lua_State* L);
void lua_setfield(lua_State* L, int idx, const char* k);

// Function calls
void lua_call(lua_State* L, int nargs, int nresults);

// Global variables
void lua_setglobal(lua_State* L, const char* name);
void lua_getglobal(lua_State* L, const char* name);

// Error handling
int luaL_error(lua_State* L, const char* fmt, ...);

#ifdef __cplusplus
}
#endif