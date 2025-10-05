#pragma once
#include "lua.h"

#ifdef __cplusplus
extern "C" {
#endif

// Library opening functions
int luaopen_base(lua_State* L);
int luaopen_table(lua_State* L);
int luaopen_io(lua_State* L);
int luaopen_os(lua_State* L);
int luaopen_string(lua_State* L);
int luaopen_math(lua_State* L);
int luaopen_debug(lua_State* L);
int luaopen_package(lua_State* L);

// Open all standard libraries
void luaL_openlibs(lua_State* L);

#ifdef __cplusplus
}
#endif