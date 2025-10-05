#pragma once

#include <Windows.h>
#include <string>

class DllInjector
{
public:
    DllInjector();
    ~DllInjector();

    bool InjectDLL(DWORD processId, const std::string& dllPath);
    bool InjectDLLManualMap(DWORD processId, const std::string& dllPath);
    bool EjectDLL(DWORD processId, const std::string& dllName);

private:
    bool EnablePrivilege(const std::string& privilegeName);
    std::string GetFullDllPath(const std::string& dllName);
    bool IsTargetProcess64Bit(DWORD processId);
    
    // Manual mapping functions
    struct MANUAL_MAPPING_DATA
    {
        HINSTANCE(WINAPI* fnLoadLibraryA)(LPCSTR);
        FARPROC(WINAPI* fnGetProcAddress)(HMODULE, LPCSTR);
        HMODULE hMod;
    };
    
    static DWORD WINAPI MapperFunction(MANUAL_MAPPING_DATA* pData);
};