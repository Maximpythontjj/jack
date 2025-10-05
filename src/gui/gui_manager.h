#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <functional>
#include <string>
#include <vector>

class PipeManager;

class GuiManager
{
public:
    using AttachCallback = std::function<bool()>;
    using ExecuteCallback = std::function<bool(const std::string&)>;

    GuiManager();
    ~GuiManager();

    bool Initialize(HINSTANCE hInstance);
    void Run();
    void Shutdown();

    void SetAttachCallback(AttachCallback callback) { m_attachCallback = callback; }
    void SetExecuteCallback(ExecuteCallback callback) { m_executeCallback = callback; }
    void SetPipeManager(PipeManager* pipeManager) { m_pipeManager = pipeManager; }

private:
    static LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();

    void RenderGui();
    void ApplyDarkTheme();
    void RenderMainWindow();
    void RenderConsole();
    void HandleScriptExecution();
    void LoadScriptFromFile();
    void SaveScriptToFile();

    HWND m_hwnd;
    WNDCLASSEX m_windowClass;
    ID3D11Device* m_pd3dDevice;
    ID3D11DeviceContext* m_pd3dDeviceContext;
    IDXGISwapChain* m_pSwapChain;
    ID3D11RenderTargetView* m_pMainRenderTargetView;
    
    bool m_isAttached;
    std::string m_scriptText;
    std::vector<std::string> m_consoleOutput;
    std::vector<std::string> m_savedScripts;
    
    AttachCallback m_attachCallback;
    ExecuteCallback m_executeCallback;
    PipeManager* m_pipeManager;
    
    // GUI state
    char m_scriptBuffer[8192];
    bool m_showConsole;
    bool m_showScriptList;
    float m_consoleHeight;
};