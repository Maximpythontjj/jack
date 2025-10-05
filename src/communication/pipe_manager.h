#pragma once

#include <Windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

class PipeManager
{
public:
    PipeManager();
    ~PipeManager();

    bool CreateServer(const std::string& pipeName = "\\\\.\\pipe\\ScarryExecutor");
    bool ConnectToServer(const std::string& pipeName = "\\\\.\\pipe\\ScarryExecutor");
    
    bool SendScript(const std::string& script);
    bool SendOutput(const std::string& output);
    
    bool ReadScript(std::string& script);
    bool ReadOutput(std::string& output);
    
    bool IsConnected() const { return m_isConnected; }
    void Disconnect();

private:
    void ServerThreadProc();
    void ClientThreadProc();
    
    bool WriteMessage(HANDLE hPipe, const std::string& message);
    bool ReadMessage(HANDLE hPipe, std::string& message);

    HANDLE m_hPipe;
    std::atomic<bool> m_isConnected;
    std::atomic<bool> m_isServer;
    std::atomic<bool> m_shouldStop;
    
    std::thread m_workerThread;
    std::mutex m_outputQueueMutex;
    std::queue<std::string> m_outputQueue;
    
    std::string m_pipeName;
    
    static const DWORD BUFFER_SIZE = 8192;
};