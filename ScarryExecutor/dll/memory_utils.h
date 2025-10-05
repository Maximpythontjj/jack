#pragma once
#include <windows.h>
#include <string>

class MemoryUtils {
public:
    static uintptr_t FindPattern(HANDLE process, uintptr_t start, uintptr_t end, const char* pattern, const char* mask);
    static uintptr_t GetModuleBaseAddress(DWORD processId, const std::string& moduleName);
    static bool ReadMemory(HANDLE process, uintptr_t address, void* buffer, size_t size);
    static bool WriteMemory(HANDLE process, uintptr_t address, void* buffer, size_t size);
    static bool IsAddressValid(uintptr_t address);

private:
    static uintptr_t FindPatternInSection(HANDLE process, uintptr_t start, uintptr_t size, const char* pattern, const char* mask);
};