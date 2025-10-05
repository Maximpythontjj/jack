#pragma once
#include <windows.h>
#include <string>

class IPCManager {
public:
    static bool Initialize();
    static void Shutdown();

    static bool SendScript(const std::string& script);
    static bool SendConsoleMessage(const std::string& message);

    static bool IsConnected();

private:
    static HANDLE hMapFile;
    static LPVOID pBuf;

    static const std::string SHARED_MEMORY_NAME;
    static const int SHARED_MEMORY_SIZE;
};