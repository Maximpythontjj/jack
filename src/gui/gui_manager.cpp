#include "gui_manager.h"
#include "../communication/pipe_manager.h"
#include "../utils/logger.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <comdef.h>
#include <fstream>
#include <sstream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

GuiManager::GuiManager()
    : m_hwnd(nullptr)
    , m_pd3dDevice(nullptr)
    , m_pd3dDeviceContext(nullptr)
    , m_pSwapChain(nullptr)
    , m_pMainRenderTargetView(nullptr)
    , m_isAttached(false)
    , m_pipeManager(nullptr)
    , m_showConsole(true)
    , m_showScriptList(false)
    , m_consoleHeight(200.0f)
{
    memset(m_scriptBuffer, 0, sizeof(m_scriptBuffer));
    strcpy_s(m_scriptBuffer, sizeof(m_scriptBuffer), "print(\"Hello from SCARRY EXECUTOR!\")");
}

GuiManager::~GuiManager()
{
    Shutdown();
}

bool GuiManager::Initialize(HINSTANCE hInstance)
{
    // Create window class
    m_windowClass = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L, hInstance, nullptr, nullptr, nullptr, nullptr, L"ScarryExecutor", nullptr };
    RegisterClassEx(&m_windowClass);

    // Create window
    m_hwnd = CreateWindow(m_windowClass.lpszClassName, L"SCARRY EXECUTOR v1.0.0", 
        WS_OVERLAPPEDWINDOW, 100, 100, 1200, 800, nullptr, nullptr, hInstance, nullptr);

    if (!m_hwnd)
    {
        Logger::Log("Failed to create window", Logger::Level::Error);
        return false;
    }

    // Store this pointer in window user data for window proc access
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // Initialize Direct3D
    if (!CreateDeviceD3D(m_hwnd))
    {
        Logger::Log("Failed to create D3D device", Logger::Level::Error);
        CleanupDeviceD3D();
        UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
        return false;
    }

    // Show window
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ApplyDarkTheme();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(m_pd3dDevice, m_pd3dDeviceContext);

    Logger::Log("GUI initialized successfully");
    return true;
}

void GuiManager::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Render GUI
        RenderGui();

        // Check for messages from DLL
        if (m_pipeManager && m_pipeManager->IsConnected())
        {
            std::string output;
            if (m_pipeManager->ReadOutput(output))
            {
                if (!output.empty())
                {
                    m_consoleOutput.push_back(output);
                    if (m_consoleOutput.size() > 1000) // Limit console history
                    {
                        m_consoleOutput.erase(m_consoleOutput.begin());
                    }
                }
            }
        }

        // Rendering
        ImGui::Render();
        const float clear_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pMainRenderTargetView, nullptr);
        m_pd3dDeviceContext->ClearRenderTargetView(m_pMainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        m_pSwapChain->Present(1, 0);
    }
}

void GuiManager::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(m_hwnd);
    UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
}

LRESULT WINAPI GuiManager::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            GuiManager* pThis = (GuiManager*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
            if (pThis && pThis->m_pSwapChain)
            {
                pThis->CleanupRenderTarget();
                pThis->m_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                pThis->CreateRenderTarget();
            }
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool GuiManager::CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &featureLevel, &m_pd3dDeviceContext);
    
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
            featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &featureLevel, &m_pd3dDeviceContext);
    
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void GuiManager::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (m_pSwapChain) { m_pSwapChain->Release(); m_pSwapChain = nullptr; }
    if (m_pd3dDeviceContext) { m_pd3dDeviceContext->Release(); m_pd3dDeviceContext = nullptr; }
    if (m_pd3dDevice) { m_pd3dDevice->Release(); m_pd3dDevice = nullptr; }
}

void GuiManager::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pMainRenderTargetView);
    pBackBuffer->Release();
}

void GuiManager::CleanupRenderTarget()
{
    if (m_pMainRenderTargetView) { m_pMainRenderTargetView->Release(); m_pMainRenderTargetView = nullptr; }
}

void GuiManager::ApplyDarkTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // SCARRY EXECUTOR dark theme with red accents
    colors[ImGuiCol_Text]                   = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.20f, 0.20f, 0.20f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.40f, 0.20f, 0.20f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.60f, 0.20f, 0.20f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.60f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.80f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.70f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.90f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.70f, 0.20f, 0.20f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.80f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.90f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.70f, 0.20f, 0.20f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.80f, 0.30f, 0.30f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.90f, 0.20f, 0.20f, 1.00f);

    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.ScrollbarRounding = 2.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
}

void GuiManager::RenderGui()
{
    // Main dockspace
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | 
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("SCARRY EXECUTOR", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // Menu bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Script"))
                LoadScriptFromFile();
            if (ImGui::MenuItem("Save Script"))
                SaveScriptToFile();
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
                PostQuitMessage(0);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Console", nullptr, &m_showConsole);
            ImGui::MenuItem("Script List", nullptr, &m_showScriptList);
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }

    // Create dockspace
    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    RenderMainWindow();
    
    if (m_showConsole)
        RenderConsole();

    ImGui::End();
}

void GuiManager::RenderMainWindow()
{
    ImGui::Begin("Script Editor");

    // Status indicator
    ImGui::Text("Status: ");
    ImGui::SameLine();
    if (m_isAttached)
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "ATTACHED");
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "NOT ATTACHED");
    }

    ImGui::Separator();

    // Attach/Detach button
    if (!m_isAttached)
    {
        if (ImGui::Button("ATTACH TO ROBLOX", ImVec2(200, 40)))
        {
            if (m_attachCallback && m_attachCallback())
            {
                m_isAttached = true;
                m_consoleOutput.push_back("[SCARRY] Successfully attached to Roblox process!");
            }
        }
    }
    else
    {
        if (ImGui::Button("DETACH", ImVec2(200, 40)))
        {
            m_isAttached = false;
            m_consoleOutput.push_back("[SCARRY] Detached from Roblox process.");
        }
    }

    ImGui::SameLine();
    
    // Execute button
    bool canExecute = m_isAttached && strlen(m_scriptBuffer) > 0;
    if (!canExecute)
        ImGui::BeginDisabled();
    
    if (ImGui::Button("EXECUTE SCRIPT", ImVec2(200, 40)))
    {
        HandleScriptExecution();
    }
    
    if (!canExecute)
        ImGui::EndDisabled();

    ImGui::SameLine();
    
    // Clear button
    if (ImGui::Button("CLEAR", ImVec2(100, 40)))
    {
        memset(m_scriptBuffer, 0, sizeof(m_scriptBuffer));
    }

    ImGui::Separator();

    // Script text area
    ImGui::Text("Lua Script:");
    ImGui::InputTextMultiline("##script", m_scriptBuffer, sizeof(m_scriptBuffer), 
        ImVec2(-1, ImGui::GetContentRegionAvail().y - 60), ImGuiInputTextFlags_AllowTabInput);

    ImGui::End();
}

void GuiManager::RenderConsole()
{
    ImGui::Begin("Console Output", &m_showConsole);

    if (ImGui::Button("Clear Console"))
    {
        m_consoleOutput.clear();
    }

    ImGui::Separator();

    ImGui::BeginChild("ConsoleScrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    
    for (const auto& line : m_consoleOutput)
    {
        ImGui::TextUnformatted(line.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();

    ImGui::End();
}

void GuiManager::HandleScriptExecution()
{
    if (!m_executeCallback)
        return;

    std::string script = m_scriptBuffer;
    m_consoleOutput.push_back("[SCARRY] Executing script...");
    
    if (m_executeCallback(script))
    {
        m_consoleOutput.push_back("[SCARRY] Script sent successfully.");
    }
    else
    {
        m_consoleOutput.push_back("[SCARRY] Failed to send script!");
    }
}

void GuiManager::LoadScriptFromFile()
{
    OPENFILENAME ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Lua Files\0*.lua\0Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
    {
        std::ifstream file(szFile);
        if (file.is_open())
        {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            
            if (content.length() < sizeof(m_scriptBuffer))
            {
                strcpy_s(m_scriptBuffer, sizeof(m_scriptBuffer), content.c_str());
                m_consoleOutput.push_back("[SCARRY] Script loaded from: " + std::string(szFile));
            }
            else
            {
                m_consoleOutput.push_back("[SCARRY] Script file too large!");
            }
            
            file.close();
        }
        else
        {
            m_consoleOutput.push_back("[SCARRY] Failed to open file: " + std::string(szFile));
        }
    }
}

void GuiManager::SaveScriptToFile()
{
    OPENFILENAME ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Lua Files\0*.lua\0Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn))
    {
        std::ofstream file(szFile);
        if (file.is_open())
        {
            file << m_scriptBuffer;
            file.close();
            m_consoleOutput.push_back("[SCARRY] Script saved to: " + std::string(szFile));
        }
        else
        {
            m_consoleOutput.push_back("[SCARRY] Failed to save file: " + std::string(szFile));
        }
    }
}