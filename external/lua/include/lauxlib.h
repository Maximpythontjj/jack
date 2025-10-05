#pragma once
#include "lua.h"

#ifdef __cplusplus
extern "C" {
#endif

// Auxiliary functions
const char* luaL_tolstring(lua_State* L, int idx, size_t* len);

#ifdef __cplusplus
}
#endif