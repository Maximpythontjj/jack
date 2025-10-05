#pragma once
#include <windows.h>
#include <string>

class DLLInjector {
public:
    static bool InjectDLL(DWORD processId, const std::string& dllPath);
    static bool EjectDLL(DWORD processId, const std::string& dllName);

private:
    static HMODULE GetModuleHandleInProcess(DWORD processId, const std::string& moduleName);
    static bool EnableDebugPrivileges();
};