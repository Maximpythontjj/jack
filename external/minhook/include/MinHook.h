#pragma once

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// MinHook Error Codes
typedef enum MH_STATUS
{
    MH_UNKNOWN = -1,
    MH_OK = 0,
    MH_ERROR_ALREADY_INITIALIZED,
    MH_ERROR_NOT_INITIALIZED,
    MH_ERROR_ALREADY_CREATED,
    MH_ERROR_NOT_CREATED,
    MH_ERROR_ENABLED,
    MH_ERROR_DISABLED,
    MH_ERROR_NOT_EXECUTABLE,
    MH_ERROR_UNSUPPORTED_FUNCTION,
    MH_ERROR_MEMORY_ALLOC,
    MH_ERROR_MEMORY_PROTECT,
    MH_ERROR_MODULE_NOT_FOUND,
    MH_ERROR_FUNCTION_NOT_FOUND
} MH_STATUS;

// Initialize the MinHook library
MH_STATUS WINAPI MH_Initialize(VOID);

// Uninitialize the MinHook library
MH_STATUS WINAPI MH_Uninitialize(VOID);

// Create a hook for the specified target function, in disabled state
MH_STATUS WINAPI MH_CreateHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal);

// Enable an already created hook
MH_STATUS WINAPI MH_EnableHook(LPVOID pTarget);

// Disable an already created hook
MH_STATUS WINAPI MH_DisableHook(LPVOID pTarget);

// Remove an already created hook
MH_STATUS WINAPI MH_RemoveHook(LPVOID pTarget);

#ifdef __cplusplus
}
#endif