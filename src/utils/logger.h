#pragma once

#include <string>
#include <fstream>
#include <mutex>

class Logger
{
public:
    enum class Level
    {
        Info,
        Warning,
        Error,
        Debug
    };

    static void Initialize();
    static void Shutdown();
    static void Log(const std::string& message, Level level = Level::Info);
    static void SetLogFile(const std::string& filename);
    static void EnableConsoleOutput(bool enable);

private:
    static std::mutex s_logMutex;
    static std::ofstream s_logFile;
    static bool s_consoleOutput;
    static bool s_initialized;
    
    static std::string LevelToString(Level level);
    static std::string GetTimestamp();
};