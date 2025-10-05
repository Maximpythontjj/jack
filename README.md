# SCARRY EXECUTOR

A C++ Roblox script executor with GUI interface for testing anti-cheat systems.

## ⚠️ IMPORTANT DISCLAIMER

This software is developed strictly for educational and security research purposes. It is intended to help developers understand how script executors work so they can better protect their games against them. **DO NOT USE THIS FOR CHEATING OR ANY MALICIOUS ACTIVITIES.**

## Features

- Modern ImGui-based dark theme GUI
- Process injection and DLL loading
- Named pipe communication between GUI and injected DLL
- Lua script execution environment
- Real-time console output
- Script loading/saving functionality
- Anti-cheat evasion research capabilities

## Build Requirements

- Windows 10/11
- Visual Studio 2019 or later (with C++17/20 support)
- CMake 3.16 or later
- DirectX 11 SDK (usually included with Windows SDK)

## Project Structure

```
SCARRY_EXECUTOR/
├── src/
│   ├── main.cpp                 # Main entry point
│   ├── gui/                     # GUI management (ImGui)
│   ├── process/                 # Process management and finding
│   ├── injector/                # DLL injection implementation
│   ├── communication/           # Named pipe communication
│   ├── utils/                   # Logging and utilities
│   └── dll/                     # Internal DLL (injected into Roblox)
├── external/                    # External dependencies
│   ├── imgui/                   # ImGui library (minimal implementation)
│   ├── lua/                     # Lua headers and stub implementation
│   └── minhook/                 # MinHook headers (for function hooking)
├── CMakeLists.txt              # CMake build configuration
├── build.bat                   # Windows build script
└── README.md                   # This file
```

## Building

### Windows (Recommended)

1. Clone or download this repository
2. Open Command Prompt or PowerShell in the project directory
3. Run the build script:
   ```cmd
   build.bat
   ```

### Manual Build

1. Create a build directory:
   ```cmd
   mkdir build
   cd build
   ```

2. Configure with CMake:
   ```cmd
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

3. Build the project:
   ```cmd
   cmake --build . --config Release
   ```

4. The executable will be created at `build/bin/Release/SCARRY_EXECUTOR.exe`
5. The DLL will be created at `build/bin/Release/ScarryInternal.dll`

## Usage

### Basic Usage

1. Launch `SCARRY_EXECUTOR.exe`
2. Start Roblox and join any game
3. Click "ATTACH TO ROBLOX" button in the executor
4. Enter Lua scripts in the text area
5. Click "EXECUTE SCRIPT" to run the script
6. View output in the console window

### Example Scripts

Basic test script:
```lua
print("Hello from SCARRY EXECUTOR!")
print("Current time: " .. os.time())
```

Wait function test:
```lua
print("Testing wait function...")
wait(1)
print("One second has passed!")
```

Game API test (mock implementation):
```lua
local Players = game:GetService("Players")
print("Players service loaded")
```

## Current Limitations

This is a **proof-of-concept implementation** with several limitations:

### Implemented Features:
- ✅ GUI interface with dark theme
- ✅ Process discovery and DLL injection
- ✅ Named pipe communication
- ✅ Basic Lua script execution
- ✅ Console output and logging
- ✅ Script file loading/saving

### Not Fully Implemented:
- ❌ Real Roblox API hooking (uses mock functions)
- ❌ Advanced anti-cheat evasion techniques
- ❌ Real Lua state hijacking from Roblox
- ❌ Function signature scanning (placeholder patterns)
- ❌ Memory protection and stealth features

### For Production Use:

To make this a fully functional executor, you would need to:

1. **Replace Lua stub**: Integrate real Lua 5.4 libraries
2. **Implement MinHook**: Add proper function hooking library
3. **Reverse engineer Roblox**: Find current function signatures and offsets
4. **Add anti-cheat evasion**: Implement signature spoofing, memory protection
5. **Lua state hijacking**: Properly capture and use Roblox's Lua state
6. **Error handling**: Add robust error handling and recovery

## Security Research Notes

This project demonstrates several important security concepts:

### DLL Injection Techniques:
- CreateRemoteThread + LoadLibraryA method
- Manual DLL mapping (partially implemented)
- Process memory manipulation

### Inter-Process Communication:
- Named pipes for GUI ↔ DLL communication
- Message serialization and deserialization

### Anti-Detection Methods (Research):
- Vectored Exception Handling
- Memory protection changes
- Function signature obfuscation

## Legal and Ethical Use

This software is provided for:
- ✅ Security research and education
- ✅ Understanding injection techniques
- ✅ Developing better anti-cheat systems
- ✅ Academic purposes

This software should NOT be used for:
- ❌ Cheating in online games
- ❌ Circumventing game security
- ❌ Any malicious activities
- ❌ Commercial exploitation

## Dependencies

### External Libraries Used:
- **ImGui**: Immediate mode GUI library
- **DirectX 11**: Graphics rendering
- **Windows API**: Process manipulation, DLL injection
- **Lua** (stubbed): Script execution engine

### Note on Dependencies:
The current implementation includes minimal stub versions of ImGui and Lua for compilation purposes. For a production version, you would need to integrate the full libraries.

## Contributing

This is a research project. If you're contributing:

1. Ensure all contributions are for educational/research purposes
2. Document security implications of any changes
3. Follow responsible disclosure for any vulnerabilities found
4. Maintain the educational focus of the project

## Troubleshooting

### Common Issues:

**"Failed to attach to Roblox"**
- Ensure Roblox is running and you have administrator privileges
- Check Windows Defender or antivirus isn't blocking the injection

**"Build fails with missing headers"**
- Ensure you have the Windows SDK installed
- Check that Visual Studio has C++ development tools

**"DLL injection fails"**
- Run as administrator
- Ensure both executor and Roblox are same architecture (x64)
- Check that Windows Defender isn't quarantining the DLL

### Debug Mode:
The application includes detailed logging. Check `scarry_executor.log` for detailed error information.

## Acknowledgments

This project was created for educational purposes to understand:
- How game executors work internally
- DLL injection and process manipulation techniques
- GUI development with ImGui
- Inter-process communication methods

**Remember: Use this knowledge responsibly and ethically!**