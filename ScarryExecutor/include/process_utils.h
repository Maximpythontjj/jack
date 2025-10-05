#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>

struct ProcessInfo {
    DWORD processId;
    std::string exeName;
    std::string windowTitle;
};

class ProcessUtils {
public:
    static std::vector<ProcessInfo> GetRobloxProcesses();
    static bool IsProcessRunning(DWORD processId);
    static DWORD GetProcessIdByName(const std::string& processName);
    static std::string GetProcessWindowTitle(DWORD processId);
};