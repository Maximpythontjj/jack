#pragma once

#include <Windows.h>
#include <string>
#include <vector>

struct ProcessInfo
{
    DWORD pid;
    std::string name;
    std::string path;
};

class ProcessManager
{
public:
    ProcessManager();
    ~ProcessManager();

    DWORD FindRobloxProcess();
    std::vector<ProcessInfo> GetRobloxProcesses();
    bool IsProcessRunning(DWORD pid);
    std::string GetProcessPath(DWORD pid);
    bool TerminateRobloxProcess(DWORD pid);

private:
    std::vector<ProcessInfo> EnumerateProcesses();
    bool IsRobloxProcess(const std::string& processName);
};