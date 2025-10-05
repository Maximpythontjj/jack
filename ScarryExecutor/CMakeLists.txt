cmake_minimum_required(VERSION 3.20)
project(ScarryExecutor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable static linking for standalone executable
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

# Find required packages
find_package(PkgConfig REQUIRED)

# Main executable
add_executable(ScarryExecutor src/main.cpp src/gui.cpp src/injector.cpp src/process_utils.cpp src/ipc.cpp src/anticheat_bypass.cpp)

# DLL for injection
add_library(ScarryHook SHARED dll/dllmain.cpp dll/lua_hooks.cpp dll/memory_utils.cpp)

# Include directories
target_include_directories(ScarryExecutor PRIVATE include)
target_include_directories(ScarryHook PRIVATE include dll)

# Link libraries for main executable
target_link_libraries(ScarryExecutor
    user32
    kernel32
    advapi32
    shell32
    comctl32
    gdi32
)

# Link libraries for DLL
target_link_libraries(ScarryHook
    user32
    kernel32
    advapi32
    psapi
)

# Copy DLL to output directory
add_custom_command(TARGET ScarryExecutor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ScarryHook> $<TARGET_DIR:ScarryExecutor>
    COMMENT "Copying DLL to executable directory"
)

# Set output directories
set_target_properties(ScarryExecutor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(ScarryHook PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)