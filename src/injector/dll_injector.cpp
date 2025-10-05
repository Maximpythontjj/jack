#include "dll_injector.h"
#include "../utils/logger.h"
#include <filesystem>
#include <fstream>

DllInjector::DllInjector()
{
    // Enable debug privileges for process manipulation
    EnablePrivilege("SeDebugPrivilege");
}

DllInjector::~DllInjector()
{
}

bool DllInjector::InjectDLL(DWORD processId, const std::string& dllPath)
{
    Logger::Log("Attempting DLL injection for PID: " + std::to_string(processId));
    
    std::string fullPath = GetFullDllPath(dllPath);
    if (!std::filesystem::exists(fullPath))
    {
        Logger::Log("DLL file not found: " + fullPath, Logger::Level::Error);
        return false;
    }

    // Open target process
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
                                 PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 
                                 FALSE, processId);
    
    if (!hProcess)
    {
        Logger::Log("Failed to open target process. Error: " + std::to_string(GetLastError()), Logger::Level::Error);
        return false;
    }

    // Allocate memory in target process
    SIZE_T pathLength = fullPath.length() + 1;
    LPVOID pRemotePath = VirtualAllocEx(hProcess, NULL, pathLength, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (!pRemotePath)
    {
        Logger::Log("Failed to allocate memory in target process", Logger::Level::Error);
        CloseHandle(hProcess);
        return false;
    }

    // Write DLL path to target process
    SIZE_T bytesWritten;
    if (!WriteProcessMemory(hProcess, pRemotePath, fullPath.c_str(), pathLength, &bytesWritten))
    {
        Logger::Log("Failed to write DLL path to target process", Logger::Level::Error);
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Get LoadLibraryA address
    HMODULE hKernel32 = GetModuleHandle(TEXT("kernel32.dll"));
    if (!hKernel32)
    {
        Logger::Log("Failed to get kernel32.dll handle", Logger::Level::Error);
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    LPTHREAD_START_ROUTINE pLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");
    if (!pLoadLibrary)
    {
        Logger::Log("Failed to get LoadLibraryA address", Logger::Level::Error);
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Create remote thread to load the DLL
    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pLoadLibrary, pRemotePath, 0, NULL);
    if (!hRemoteThread)
    {
        Logger::Log("Failed to create remote thread. Error: " + std::to_string(GetLastError()), Logger::Level::Error);
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Wait for the remote thread to complete
    WaitForSingleObject(hRemoteThread, INFINITE);
    
    // Get the return value (HMODULE of loaded DLL)
    DWORD exitCode;
    GetExitCodeThread(hRemoteThread, &exitCode);
    
    // Cleanup
    CloseHandle(hRemoteThread);
    VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    if (exitCode == 0)
    {
        Logger::Log("DLL injection failed - LoadLibraryA returned NULL", Logger::Level::Error);
        return false;
    }

    Logger::Log("DLL injection successful! HMODULE: 0x" + std::to_string(exitCode));
    return true;
}

bool DllInjector::InjectDLLManualMap(DWORD processId, const std::string& dllPath)
{
    Logger::Log("Attempting manual mapping injection for PID: " + std::to_string(processId));
    
    std::string fullPath = GetFullDllPath(dllPath);
    if (!std::filesystem::exists(fullPath))
    {
        Logger::Log("DLL file not found: " + fullPath, Logger::Level::Error);
        return false;
    }

    // Read DLL file
    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        Logger::Log("Failed to open DLL file", Logger::Level::Error);
        return false;
    }

    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<BYTE> dllData(fileSize);
    file.read(reinterpret_cast<char*>(dllData.data()), fileSize);
    file.close();

    // Parse PE headers
    IMAGE_DOS_HEADER* pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(dllData.data());
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        Logger::Log("Invalid DOS signature", Logger::Level::Error);
        return false;
    }

    IMAGE_NT_HEADERS* pNtHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(dllData.data() + pDosHeader->e_lfanew);
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE)
    {
        Logger::Log("Invalid NT signature", Logger::Level::Error);
        return false;
    }

    // Open target process
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
                                 PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 
                                 FALSE, processId);
    
    if (!hProcess)
    {
        Logger::Log("Failed to open target process for manual mapping", Logger::Level::Error);
        return false;
    }

    // Allocate memory for DLL in target process
    LPVOID pRemoteImage = VirtualAllocEx(hProcess, NULL, pNtHeaders->OptionalHeader.SizeOfImage, 
                                        MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    
    if (!pRemoteImage)
    {
        Logger::Log("Failed to allocate memory for DLL image", Logger::Level::Error);
        CloseHandle(hProcess);
        return false;
    }

    // Copy headers
    if (!WriteProcessMemory(hProcess, pRemoteImage, dllData.data(), pNtHeaders->OptionalHeader.SizeOfHeaders, NULL))
    {
        Logger::Log("Failed to write DLL headers", Logger::Level::Error);
        VirtualFreeEx(hProcess, pRemoteImage, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Copy sections
    IMAGE_SECTION_HEADER* pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
    for (WORD i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSectionHeader++)
    {
        if (pSectionHeader->SizeOfRawData > 0)
        {
            LPVOID pSectionDest = (LPVOID)((ULONG_PTR)pRemoteImage + pSectionHeader->VirtualAddress);
            if (!WriteProcessMemory(hProcess, pSectionDest, 
                                   dllData.data() + pSectionHeader->PointerToRawData, 
                                   pSectionHeader->SizeOfRawData, NULL))
            {
                Logger::Log("Failed to write section: " + std::string((char*)pSectionHeader->Name), Logger::Level::Error);
                VirtualFreeEx(hProcess, pRemoteImage, 0, MEM_RELEASE);
                CloseHandle(hProcess);
                return false;
            }
        }
    }

    // Setup mapping data
    MANUAL_MAPPING_DATA mappingData;
    mappingData.fnLoadLibraryA = LoadLibraryA;
    mappingData.fnGetProcAddress = GetProcAddress;
    mappingData.hMod = (HMODULE)pRemoteImage;

    // Allocate memory for mapping data
    LPVOID pMappingData = VirtualAllocEx(hProcess, NULL, sizeof(MANUAL_MAPPING_DATA), 
                                        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if (!pMappingData)
    {
        Logger::Log("Failed to allocate memory for mapping data", Logger::Level::Error);
        VirtualFreeEx(hProcess, pRemoteImage, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Write mapping data
    if (!WriteProcessMemory(hProcess, pMappingData, &mappingData, sizeof(MANUAL_MAPPING_DATA), NULL))
    {
        Logger::Log("Failed to write mapping data", Logger::Level::Error);
        VirtualFreeEx(hProcess, pRemoteImage, 0, MEM_RELEASE);
        VirtualFreeEx(hProcess, pMappingData, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Create remote thread to execute mapper
    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, MapperFunction, pMappingData, 0, NULL);
    if (!hRemoteThread)
    {
        Logger::Log("Failed to create remote thread for manual mapping", Logger::Level::Error);
        VirtualFreeEx(hProcess, pRemoteImage, 0, MEM_RELEASE);
        VirtualFreeEx(hProcess, pMappingData, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hRemoteThread, INFINITE);
    
    // Cleanup
    CloseHandle(hRemoteThread);
    VirtualFreeEx(hProcess, pMappingData, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    Logger::Log("Manual mapping injection completed successfully!");
    return true;
}

bool DllInjector::EjectDLL(DWORD processId, const std::string& dllName)
{
    Logger::Log("Attempting to eject DLL: " + dllName + " from PID: " + std::to_string(processId));
    
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
                                 PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 
                                 FALSE, processId);
    
    if (!hProcess)
    {
        Logger::Log("Failed to open target process for DLL ejection", Logger::Level::Error);
        return false;
    }

    // Get module handle
    HMODULE hMod = GetModuleHandle(dllName.c_str());
    if (!hMod)
    {
        Logger::Log("Module not found: " + dllName, Logger::Level::Error);
        CloseHandle(hProcess);
        return false;
    }

    // Get FreeLibrary address
    HMODULE hKernel32 = GetModuleHandle(TEXT("kernel32.dll"));
    LPTHREAD_START_ROUTINE pFreeLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "FreeLibrary");
    
    if (!pFreeLibrary)
    {
        Logger::Log("Failed to get FreeLibrary address", Logger::Level::Error);
        CloseHandle(hProcess);
        return false;
    }

    // Create remote thread to unload DLL
    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pFreeLibrary, hMod, 0, NULL);
    if (!hRemoteThread)
    {
        Logger::Log("Failed to create remote thread for DLL ejection", Logger::Level::Error);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hRemoteThread, INFINITE);
    
    CloseHandle(hRemoteThread);
    CloseHandle(hProcess);

    Logger::Log("DLL ejection completed successfully!");
    return true;
}

bool DllInjector::EnablePrivilege(const std::string& privilegeName)
{
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;

    LUID luid;
    if (!LookupPrivilegeValue(NULL, privilegeName.c_str(), &luid))
    {
        CloseHandle(hToken);
        return false;
    }

    TOKEN_PRIVILEGES tokenPrivs = {};
    tokenPrivs.PrivilegeCount = 1;
    tokenPrivs.Privileges[0].Luid = luid;
    tokenPrivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    bool result = AdjustTokenPrivileges(hToken, FALSE, &tokenPrivs, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    CloseHandle(hToken);
    
    return result;
}

std::string DllInjector::GetFullDllPath(const std::string& dllName)
{
    char currentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDir);
    
    std::filesystem::path fullPath = std::filesystem::path(currentDir) / dllName;
    return fullPath.string();
}

bool DllInjector::IsTargetProcess64Bit(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (!hProcess)
        return false;

    BOOL isWow64 = FALSE;
    IsWow64Process(hProcess, &isWow64);
    CloseHandle(hProcess);

#ifdef _WIN64
    return !isWow64;  // If we're 64-bit and target is not WoW64, then target is 64-bit
#else
    return false;     // If we're 32-bit, we can only inject into 32-bit processes
#endif
}

DWORD WINAPI DllInjector::MapperFunction(MANUAL_MAPPING_DATA* pData)
{
    if (!pData)
        return FALSE;

    BYTE* pBase = reinterpret_cast<BYTE*>(pData->hMod);
    
    auto pOpt = &reinterpret_cast<IMAGE_NT_HEADERS*>(pBase + reinterpret_cast<IMAGE_DOS_HEADER*>(pBase)->e_lfanew)->OptionalHeader;
    
    auto pLoadLibrary = pData->fnLoadLibraryA;
    auto pGetProcAddress = pData->fnGetProcAddress;
    
    // Process relocations
    if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
    {
        auto pRelocData = reinterpret_cast<IMAGE_BASE_RELOCATION*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
        auto delta = reinterpret_cast<DWORD_PTR>(pBase) - pOpt->ImageBase;
        
        while (pRelocData->VirtualAddress)
        {
            UINT AmountOfEntries = (pRelocData->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            WORD* pRelativeInfo = reinterpret_cast<WORD*>(pRelocData + 1);
            
            for (UINT i = 0; i < AmountOfEntries; ++i, ++pRelativeInfo)
            {
                if ((*pRelativeInfo >> 12) == IMAGE_REL_BASED_HIGHLOW || (*pRelativeInfo >> 12) == IMAGE_REL_BASED_DIR64)
                {
                    DWORD_PTR* pPatch = reinterpret_cast<DWORD_PTR*>(pBase + pRelocData->VirtualAddress + ((*pRelativeInfo) & 0xFFF));
                    *pPatch += delta;
                }
            }
            pRelocData = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<BYTE*>(pRelocData) + pRelocData->SizeOfBlock);
        }
    }
    
    // Process imports
    if (pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
    {
        auto pImportDescr = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(pBase + pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        
        while (pImportDescr->Name)
        {
            char* szMod = reinterpret_cast<char*>(pBase + pImportDescr->Name);
            HINSTANCE hDll = pLoadLibrary(szMod);
            
            DWORD_PTR* pThunkRef = reinterpret_cast<DWORD_PTR*>(pBase + pImportDescr->OriginalFirstThunk);
            DWORD_PTR* pFuncRef = reinterpret_cast<DWORD_PTR*>(pBase + pImportDescr->FirstThunk);
            
            if (!pThunkRef)
                pThunkRef = pFuncRef;
                
            for (; *pThunkRef; ++pThunkRef, ++pFuncRef)
            {
                if (IMAGE_SNAP_BY_ORDINAL(*pThunkRef))
                {
                    *pFuncRef = reinterpret_cast<DWORD_PTR>(pGetProcAddress(hDll, reinterpret_cast<char*>(*pThunkRef & 0xFFFF)));
                }
                else
                {
                    auto pImport = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(pBase + *pThunkRef);
                    *pFuncRef = reinterpret_cast<DWORD_PTR>(pGetProcAddress(hDll, pImport->Name));
                }
            }
            ++pImportDescr;
        }
    }
    
    // Call DllMain
    auto DllMain = reinterpret_cast<BOOL(WINAPI*)(HINSTANCE, DWORD, LPVOID)>(pBase + pOpt->AddressOfEntryPoint);
    return DllMain(reinterpret_cast<HINSTANCE>(pBase), DLL_PROCESS_ATTACH, nullptr);
}