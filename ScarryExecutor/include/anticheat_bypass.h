#pragma once
#include <windows.h>
#include <string>

class AnticheatBypass {
public:
    static bool Initialize();
    static void Shutdown();

    static bool HideModule(HMODULE module);
    static bool SpoofModuleName(HMODULE module, const std::string& newName);
    static bool BypassDebuggerCheck();

    static void EncryptStrings();
    static void ObfuscateAPICalls();

private:
    static bool RemoveFromModuleList(HMODULE module);
    static bool PatchIsDebuggerPresent();
    static bool PatchCheckRemoteDebuggerPresent();

    static DWORD originalIsDebuggerPresent;
    static DWORD originalCheckRemoteDebuggerPresent;

    static bool hooksInstalled;
};