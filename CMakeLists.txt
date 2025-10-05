cmake_minimum_required(VERSION 3.16)
project(ScarryExecutor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set static linking for standalone executable
if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Find required packages
find_package(OpenGL REQUIRED)

# ImGui source files
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_win32.cpp
    external/imgui/backends/imgui_impl_dx11.cpp
)

# Main executor executable
add_executable(SCARRY_EXECUTOR
    src/main.cpp
    src/gui/gui_manager.cpp
    src/gui/gui_manager.h
    src/process/process_manager.cpp
    src/process/process_manager.h
    src/injector/dll_injector.cpp
    src/injector/dll_injector.h
    src/communication/pipe_manager.cpp
    src/communication/pipe_manager.h
    src/utils/logger.cpp
    src/utils/logger.h
    ${IMGUI_SOURCES}
)

# Internal DLL
add_library(ScarryInternal SHARED
    src/dll/dll_main.cpp
    src/dll/lua_executor.cpp
    src/dll/lua_executor.h
    src/dll/hooks.cpp
    src/dll/hooks.h
    src/communication/pipe_manager.cpp
    src/communication/pipe_manager.h
    src/utils/logger.cpp
    src/utils/logger.h
    external/lua/lua_stub.cpp
)

# Include directories
target_include_directories(SCARRY_EXECUTOR PRIVATE
    src/
    external/imgui/
    external/imgui/backends/
    external/minhook/include/
)

target_include_directories(ScarryInternal PRIVATE
    src/
    external/lua/include/
    external/minhook/include/
)

# Link libraries for main executable
target_link_libraries(SCARRY_EXECUTOR
    d3d11
    dxgi
    dxguid
    ${OPENGL_LIBRARIES}
    user32
    gdi32
    shell32
    ole32
    oleaut32
    uuid
    comdlg32
    advapi32
)

# Link libraries for DLL  
target_link_libraries(ScarryInternal
    user32
    kernel32
    advapi32
    psapi
)

# Set output directory
set_target_properties(SCARRY_EXECUTOR PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(ScarryInternal PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy DLL to output directory
add_custom_command(TARGET SCARRY_EXECUTOR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:ScarryInternal>
    $<TARGET_FILE_DIR:SCARRY_EXECUTOR>
)