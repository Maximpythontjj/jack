#include "logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

std::mutex Logger::s_logMutex;
std::ofstream Logger::s_logFile;
bool Logger::s_consoleOutput = true;
bool Logger::s_initialized = false;

void Logger::Initialize()
{
    std::lock_guard<std::mutex> lock(s_logMutex);
    
    if (s_initialized)
        return;
    
    SetLogFile("scarry_executor.log");
    s_initialized = true;
    
    Log("Logger initialized", Level::Info);
}

void Logger::Shutdown()
{
    std::lock_guard<std::mutex> lock(s_logMutex);
    
    if (!s_initialized)
        return;
    
    Log("Logger shutting down", Level::Info);
    
    if (s_logFile.is_open())
    {
        s_logFile.close();
    }
    
    s_initialized = false;
}

void Logger::Log(const std::string& message, Level level)
{
    std::lock_guard<std::mutex> lock(s_logMutex);
    
    std::string timestamp = GetTimestamp();
    std::string levelStr = LevelToString(level);
    std::string fullMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    // Console output
    if (s_consoleOutput)
    {
        if (level == Level::Error)
        {
            std::cerr << fullMessage << std::endl;
        }
        else
        {
            std::cout << fullMessage << std::endl;
        }
    }
    
    // File output
    if (s_logFile.is_open())
    {
        s_logFile << fullMessage << std::endl;
        s_logFile.flush();
    }
}

void Logger::SetLogFile(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(s_logMutex);
    
    if (s_logFile.is_open())
    {
        s_logFile.close();
    }
    
    s_logFile.open(filename, std::ios::out | std::ios::app);
    if (!s_logFile.is_open())
    {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::EnableConsoleOutput(bool enable)
{
    std::lock_guard<std::mutex> lock(s_logMutex);
    s_consoleOutput = enable;
}

std::string Logger::LevelToString(Level level)
{
    switch (level)
    {
        case Level::Info:    return "INFO";
        case Level::Warning: return "WARN";
        case Level::Error:   return "ERROR";
        case Level::Debug:   return "DEBUG";
        default:             return "UNKNOWN";
    }
}

std::string Logger::GetTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}