#include "process_manager.h"
#include "../utils/logger.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <algorithm>

ProcessManager::ProcessManager()
{
}

ProcessManager::~ProcessManager()
{
}

DWORD ProcessManager::FindRobloxProcess()
{
    auto processes = GetRobloxProcesses();
    
    if (processes.empty())
    {
        Logger::Log("No Roblox processes found", Logger::Level::Warning);
        return 0;
    }

    // Prefer RobloxPlayerBeta.exe
    for (const auto& proc : processes)
    {
        if (proc.name.find("RobloxPlayerBeta.exe") != std::string::npos)
        {
            Logger::Log("Found RobloxPlayerBeta.exe with PID: " + std::to_string(proc.pid));
            return proc.pid;
        }
    }

    // Fallback to any Roblox process
    Logger::Log("Found Roblox process: " + processes[0].name + " with PID: " + std::to_string(processes[0].pid));
    return processes[0].pid;
}

std::vector<ProcessInfo> ProcessManager::GetRobloxProcesses()
{
    std::vector<ProcessInfo> robloxProcesses;
    auto allProcesses = EnumerateProcesses();

    for (const auto& proc : allProcesses)
    {
        if (IsRobloxProcess(proc.name))
        {
            robloxProcesses.push_back(proc);
        }
    }

    return robloxProcesses;
}

bool ProcessManager::IsProcessRunning(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL)
        return false;

    DWORD exitCode;
    bool isRunning = GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE;
    
    CloseHandle(hProcess);
    return isRunning;
}

std::string ProcessManager::GetProcessPath(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL)
        return "";

    char path[MAX_PATH];
    DWORD size = sizeof(path);
    
    if (QueryFullProcessImageName(hProcess, 0, path, &size))
    {
        CloseHandle(hProcess);
        return std::string(path);
    }

    CloseHandle(hProcess);
    return "";
}

bool ProcessManager::TerminateRobloxProcess(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL)
        return false;

    bool success = TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
    
    if (success)
    {
        Logger::Log("Successfully terminated Roblox process with PID: " + std::to_string(pid));
    }
    else
    {
        Logger::Log("Failed to terminate Roblox process with PID: " + std::to_string(pid), Logger::Level::Error);
    }
    
    return success;
}

std::vector<ProcessInfo> ProcessManager::EnumerateProcesses()
{
    std::vector<ProcessInfo> processes;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        Logger::Log("Failed to create process snapshot", Logger::Level::Error);
        return processes;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
            ProcessInfo info;
            info.pid = pe32.th32ProcessID;
            info.name = pe32.szExeFile;
            info.path = GetProcessPath(info.pid);
            
            processes.push_back(info);
            
        } while (Process32Next(hSnapshot, &pe32));
    }
    else
    {
        Logger::Log("Failed to enumerate processes", Logger::Level::Error);
    }

    CloseHandle(hSnapshot);
    return processes;
}

bool ProcessManager::IsRobloxProcess(const std::string& processName)
{
    std::string lowerName = processName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    return lowerName.find("roblox") != std::string::npos ||
           lowerName.find("robloxplayerbeta") != std::string::npos ||
           lowerName.find("robloxstudio") != std::string::npos;
}