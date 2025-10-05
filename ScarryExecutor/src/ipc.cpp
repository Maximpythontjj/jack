#include "ipc.h"
#include <iostream>
#include <cstring>

const std::string IPCManager::SHARED_MEMORY_NAME = "ScarryExecutorSharedMemory";
const int IPCManager::SHARED_MEMORY_SIZE = 4096;

HANDLE IPCManager::hMapFile = NULL;
LPVOID IPCManager::pBuf = NULL;

bool IPCManager::Initialize() {
    hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        SHARED_MEMORY_SIZE,
        SHARED_MEMORY_NAME.c_str()
    );

    if (hMapFile == NULL) {
        std::cerr << "Failed to create shared memory: " << GetLastError() << std::endl;
        return false;
    }

    pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEMORY_SIZE);
    if (pBuf == NULL) {
        std::cerr << "Failed to map shared memory: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return false;
    }

    // Initialize shared memory with zeros
    memset(pBuf, 0, SHARED_MEMORY_SIZE);

    return true;
}

void IPCManager::Shutdown() {
    if (pBuf != NULL) {
        UnmapViewOfFile(pBuf);
        pBuf = NULL;
    }

    if (hMapFile != NULL) {
        CloseHandle(hMapFile);
        hMapFile = NULL;
    }
}

bool IPCManager::SendScript(const std::string& script) {
    if (pBuf == NULL) {
        return false;
    }

    // Create a structure to hold script data
    struct ScriptData {
        char command[16]; // "SCRIPT"
        char data[4080];  // Script content
    };

    ScriptData* scriptData = (ScriptData*)pBuf;

    // Clear the memory first
    memset(pBuf, 0, SHARED_MEMORY_SIZE);

    // Copy command
    strcpy_s(scriptData->command, sizeof(scriptData->command), "SCRIPT");

    // Copy script data (truncate if too long)
    size_t scriptLength = script.length();
    if (scriptLength > sizeof(scriptData->data) - 1) {
        scriptLength = sizeof(scriptData->data) - 1;
    }

    memcpy(scriptData->data, script.c_str(), scriptLength);
    scriptData->data[scriptLength] = '\0';

    return true;
}

bool IPCManager::SendConsoleMessage(const std::string& message) {
    if (pBuf == NULL) {
        return false;
    }

    // Create a structure to hold console data
    struct ConsoleData {
        char command[16]; // "CONSOLE"
        char data[4080];  // Console message
    };

    ConsoleData* consoleData = (ConsoleData*)pBuf;

    // Clear the memory first
    memset(pBuf, 0, SHARED_MEMORY_SIZE);

    // Copy command
    strcpy_s(consoleData->command, sizeof(consoleData->command), "CONSOLE");

    // Copy message data (truncate if too long)
    size_t messageLength = message.length();
    if (messageLength > sizeof(consoleData->data) - 1) {
        messageLength = sizeof(consoleData->data) - 1;
    }

    memcpy(consoleData->data, message.c_str(), messageLength);
    consoleData->data[messageLength] = '\0';

    return true;
}

bool IPCManager::IsConnected() {
    if (pBuf == NULL) {
        return false;
    }

    // Check if there's any data in shared memory
    struct IPCData {
        char command[16];
        char data[4080];
    };

    IPCData* data = (IPCData*)pBuf;
    return strlen(data->command) > 0;
}