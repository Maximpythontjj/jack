#include "process_utils.h"
#include <algorithm>
#include <cctype>

std::vector<ProcessInfo> ProcessUtils::GetRobloxProcesses() {
    std::vector<ProcessInfo> processes;

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return processes;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return processes;
    }

    do {
        std::string exeName = pe32.szExeFile;
        std::transform(exeName.begin(), exeName.end(), exeName.begin(), ::tolower);

        if (exeName.find("roblox") != std::string::npos) {
            ProcessInfo info;
            info.processId = pe32.th32ProcessID;
            info.exeName = pe32.szExeFile;
            info.windowTitle = GetProcessWindowTitle(info.processId);

            processes.push_back(info);
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return processes;
}

bool ProcessUtils::IsProcessRunning(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (hProcess == NULL) {
        return false;
    }

    DWORD exitCode;
    GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);

    return exitCode == STILL_ACTIVE;
}

DWORD ProcessUtils::GetProcessIdByName(const std::string& processName) {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return 0;
    }

    DWORD processId = 0;
    std::string targetName = processName;
    std::transform(targetName.begin(), targetName.end(), targetName.begin(), ::tolower);

    do {
        std::string exeName = pe32.szExeFile;
        std::transform(exeName.begin(), exeName.end(), exeName.begin(), ::tolower);

        if (exeName.find(targetName) != std::string::npos) {
            processId = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return processId;
}

std::string ProcessUtils::GetProcessWindowTitle(DWORD processId) {
    std::string windowTitle;

    // Get all windows
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);

        if (pid == (DWORD)lParam) {
            char title[256];
            GetWindowTextA(hwnd, title, sizeof(title));

            std::string* titlePtr = (std::string*)lParam;
            *titlePtr = std::string(title);

            return FALSE; // Stop enumeration
        }

        return TRUE;
    }, (LPARAM)&windowTitle);

    return windowTitle;
}