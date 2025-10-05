#include "memory_utils.h"
#include <psapi.h>
#include <algorithm>

uintptr_t MemoryUtils::FindPattern(HANDLE process, uintptr_t start, uintptr_t end, const char* pattern, const char* mask) {
    uintptr_t currentAddress = start;
    size_t patternLength = strlen(mask);

    while (currentAddress < end - patternLength) {
        bool found = true;

        for (size_t i = 0; i < patternLength; i++) {
            if (mask[i] == '?') continue;

            BYTE buffer;
            if (!ReadProcessMemory(process, (LPCVOID)(currentAddress + i), &buffer, sizeof(BYTE), NULL)) {
                found = false;
                break;
            }

            if ((char)buffer != pattern[i]) {
                found = false;
                break;
            }
        }

        if (found) {
            return currentAddress;
        }

        currentAddress++;
    }

    return 0;
}

uintptr_t MemoryUtils::GetModuleBaseAddress(DWORD processId, const std::string& moduleName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hSnapshot, &me32)) {
        CloseHandle(hSnapshot);
        return 0;
    }

    uintptr_t baseAddress = 0;

    do {
        std::string exeName = me32.szModule;
        std::string exePath = me32.szExePath;

        if (exeName.find(moduleName) != std::string::npos ||
            exePath.find(moduleName) != std::string::npos) {
            baseAddress = (uintptr_t)me32.modBaseAddr;
            break;
        }
    } while (Module32Next(hSnapshot, &me32));

    CloseHandle(hSnapshot);
    return baseAddress;
}

bool MemoryUtils::ReadMemory(HANDLE process, uintptr_t address, void* buffer, size_t size) {
    SIZE_T bytesRead;
    return ReadProcessMemory(process, (LPCVOID)address, buffer, size, &bytesRead) && bytesRead == size;
}

bool MemoryUtils::WriteMemory(HANDLE process, uintptr_t address, void* buffer, size_t size) {
    SIZE_T bytesWritten;
    return WriteProcessMemory(process, (LPVOID)address, buffer, size, &bytesWritten) && bytesWritten == size;
}

bool MemoryUtils::IsAddressValid(uintptr_t address) {
    // Basic check - in a real implementation you'd want more sophisticated validation
    return address != 0 && address > 0x10000; // Basic sanity check
}