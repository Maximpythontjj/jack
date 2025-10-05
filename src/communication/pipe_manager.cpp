#include "pipe_manager.h"
#include "../utils/logger.h"
#include <iostream>

PipeManager::PipeManager()
    : m_hPipe(INVALID_HANDLE_VALUE)
    , m_isConnected(false)
    , m_isServer(false)
    , m_shouldStop(false)
{
}

PipeManager::~PipeManager()
{
    Disconnect();
}

bool PipeManager::CreateServer(const std::string& pipeName)
{
    m_pipeName = pipeName;
    m_isServer = true;
    
    Logger::Log("Creating named pipe server: " + pipeName);
    
    m_hPipe = CreateNamedPipe(
        pipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFFER_SIZE,
        BUFFER_SIZE,
        0,
        NULL
    );
    
    if (m_hPipe == INVALID_HANDLE_VALUE)
    {
        Logger::Log("Failed to create named pipe. Error: " + std::to_string(GetLastError()), Logger::Level::Error);
        return false;
    }
    
    m_shouldStop = false;
    m_workerThread = std::thread(&PipeManager::ServerThreadProc, this);
    
    Logger::Log("Named pipe server created successfully");
    return true;
}

bool PipeManager::ConnectToServer(const std::string& pipeName)
{
    m_pipeName = pipeName;
    m_isServer = false;
    
    Logger::Log("Connecting to named pipe server: " + pipeName);
    
    // Wait for pipe to become available
    if (!WaitNamedPipe(pipeName.c_str(), 20000))
    {
        Logger::Log("Named pipe server not available", Logger::Level::Error);
        return false;
    }
    
    m_hPipe = CreateFile(
        pipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    
    if (m_hPipe == INVALID_HANDLE_VALUE)
    {
        Logger::Log("Failed to connect to named pipe. Error: " + std::to_string(GetLastError()), Logger::Level::Error);
        return false;
    }
    
    DWORD mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(m_hPipe, &mode, NULL, NULL))
    {
        Logger::Log("Failed to set pipe handle state", Logger::Level::Error);
        CloseHandle(m_hPipe);
        m_hPipe = INVALID_HANDLE_VALUE;
        return false;
    }
    
    m_isConnected = true;
    m_shouldStop = false;
    m_workerThread = std::thread(&PipeManager::ClientThreadProc, this);
    
    Logger::Log("Connected to named pipe server successfully");
    return true;
}

bool PipeManager::SendScript(const std::string& script)
{
    if (!m_isConnected || m_hPipe == INVALID_HANDLE_VALUE)
        return false;
    
    std::string message = "SCRIPT:" + script;
    return WriteMessage(m_hPipe, message);
}

bool PipeManager::SendOutput(const std::string& output)
{
    if (!m_isConnected || m_hPipe == INVALID_HANDLE_VALUE)
        return false;
    
    std::string message = "OUTPUT:" + output;
    return WriteMessage(m_hPipe, message);
}

bool PipeManager::ReadScript(std::string& script)
{
    std::string message;
    if (!ReadMessage(m_hPipe, message))
        return false;
    
    if (message.substr(0, 7) == "SCRIPT:")
    {
        script = message.substr(7);
        return true;
    }
    
    return false;
}

bool PipeManager::ReadOutput(std::string& output)
{
    std::lock_guard<std::mutex> lock(m_outputQueueMutex);
    
    if (m_outputQueue.empty())
        return false;
    
    output = m_outputQueue.front();
    m_outputQueue.pop();
    return true;
}

void PipeManager::Disconnect()
{
    m_shouldStop = true;
    m_isConnected = false;
    
    if (m_hPipe != INVALID_HANDLE_VALUE)
    {
        if (m_isServer)
        {
            DisconnectNamedPipe(m_hPipe);
        }
        CloseHandle(m_hPipe);
        m_hPipe = INVALID_HANDLE_VALUE;
    }
    
    if (m_workerThread.joinable())
    {
        m_workerThread.join();
    }
    
    Logger::Log("Pipe connection closed");
}

void PipeManager::ServerThreadProc()
{
    Logger::Log("Waiting for client connection...");
    
    if (ConnectNamedPipe(m_hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED)
    {
        m_isConnected = true;
        Logger::Log("Client connected to named pipe");
        
        while (!m_shouldStop && m_isConnected)
        {
            std::string message;
            if (ReadMessage(m_hPipe, message))
            {
                if (message.substr(0, 7) == "OUTPUT:")
                {
                    std::lock_guard<std::mutex> lock(m_outputQueueMutex);
                    m_outputQueue.push(message.substr(7));
                }
            }
            else
            {
                // Client disconnected or error occurred
                break;
            }
        }
    }
    else
    {
        Logger::Log("Failed to wait for client connection. Error: " + std::to_string(GetLastError()), Logger::Level::Error);
    }
    
    m_isConnected = false;
}

void PipeManager::ClientThreadProc()
{
    while (!m_shouldStop && m_isConnected)
    {
        std::string message;
        if (ReadMessage(m_hPipe, message))
        {
            if (message.substr(0, 7) == "SCRIPT:")
            {
                // Handle script execution in DLL
                Logger::Log("Received script from server");
            }
        }
        else
        {
            // Server disconnected or error occurred
            break;
        }
    }
    
    m_isConnected = false;
}

bool PipeManager::WriteMessage(HANDLE hPipe, const std::string& message)
{
    if (hPipe == INVALID_HANDLE_VALUE)
        return false;
    
    DWORD bytesWritten;
    DWORD messageLength = static_cast<DWORD>(message.length());
    
    // Write message length first
    if (!WriteFile(hPipe, &messageLength, sizeof(DWORD), &bytesWritten, NULL))
    {
        Logger::Log("Failed to write message length to pipe", Logger::Level::Error);
        return false;
    }
    
    // Write message data
    if (!WriteFile(hPipe, message.c_str(), messageLength, &bytesWritten, NULL))
    {
        Logger::Log("Failed to write message data to pipe", Logger::Level::Error);
        return false;
    }
    
    if (!FlushFileBuffers(hPipe))
    {
        Logger::Log("Failed to flush pipe buffer", Logger::Level::Error);
        return false;
    }
    
    return true;
}

bool PipeManager::ReadMessage(HANDLE hPipe, std::string& message)
{
    if (hPipe == INVALID_HANDLE_VALUE)
        return false;
    
    DWORD bytesRead;
    DWORD messageLength;
    
    // Read message length first
    if (!ReadFile(hPipe, &messageLength, sizeof(DWORD), &bytesRead, NULL))
    {
        DWORD error = GetLastError();
        if (error != ERROR_BROKEN_PIPE && error != ERROR_PIPE_NOT_CONNECTED)
        {
            Logger::Log("Failed to read message length from pipe. Error: " + std::to_string(error), Logger::Level::Error);
        }
        return false;
    }
    
    if (bytesRead != sizeof(DWORD))
        return false;
    
    if (messageLength > BUFFER_SIZE)
    {
        Logger::Log("Message too large: " + std::to_string(messageLength), Logger::Level::Error);
        return false;
    }
    
    // Read message data
    std::vector<char> buffer(messageLength + 1);
    if (!ReadFile(hPipe, buffer.data(), messageLength, &bytesRead, NULL))
    {
        Logger::Log("Failed to read message data from pipe", Logger::Level::Error);
        return false;
    }
    
    if (bytesRead != messageLength)
        return false;
    
    buffer[messageLength] = '\0';
    message = std::string(buffer.data());
    
    return true;
}