#include "ui/ui.h"
#include "ui/theme.h"

// DirectX 11 includes
#include <d3d11.h>
#include <tchar.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ai/simple_ai.h"

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#undef near
#undef far
#else
#include <unistd.h>
#include <limits.h>
#endif

// DirectX11 globals
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

// Forward declarations
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace NeuroShell {

UI::UI()
    : window_(nullptr)
    , terminal_(nullptr)
#ifdef ENABLE_CURL
    , aiClient_(nullptr)
#endif
    , simpleAI_(nullptr)
    , scrollToBottom_(false)
    , focusCommandInput_(true)
    , aiPanelWidth_(400.0f)
    , showHistorySidebar_(false)
    , historySidebarWidth_(300.0f)
    , showSettings_(false)
    , fontSize_(16.0f)
    , terminalOpacity_(0.95f)
    , aiEnabled_(true)
    , showWelcomeBanner_(true)
{
    commandInputBuffer_[0] = '\0';
    aiInputBuffer_[0] = '\0';
}

UI::~UI() {
    Shutdown();
}

bool UI::Initialize(int width, int height, const char* title) {
    std::cout << "Initializing DirectX11 window..." << std::endl;
    
    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"NeuroShell", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"NeuroShell Terminal", WS_OVERLAPPEDWINDOW, 100, 100, width, height, nullptr, nullptr, wc.hInstance, nullptr);
    
    // Store hwnd in window_ (cast to void*)
    window_ = (void*)hwnd;
    
    // Initialize Direct3D
    std::cout << "Creating DirectX11 device..." << std::endl;
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        std::cerr << "Failed to create DirectX11 device!" << std::endl;
        std::cerr << "Your graphics drivers may need updating." << std::endl;
        return false;
    }
    
    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    
    std::cout << "Creating ImGui context..." << std::endl;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    std::cout << "Initializing ImGui backends..." << std::endl;
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    
    std::cout << "Applying theme..." << std::endl;
    // Apply theme
    Theme::ApplyTokyoNight();
    Theme::EnableDocking();
    
    std::cout << "Setting up app state..." << std::endl;
    // Initialize appState theme colors
    appState_.theme = Theme::GetTokyoNightColors();
    
    // Initialize terminal
    terminal_ = std::make_unique<Terminal>();
    terminal_->Initialize();
    
    // Initialize simple AI (no network needed)
    simpleAI_ = std::make_unique<SimpleAI>();
    
#ifdef ENABLE_CURL
    // Initialize AI client (only if CURL is available)
    aiClient_ = std::make_unique<AIClient>();
    
    // Load configuration
    LoadConfiguration();
    
    // Initialize AI if configured
    if (!appState_.aiConfig.apiKey.empty()) {
        aiClient_->Initialize(appState_.aiConfig);
    }
#else
    appState_.showAIPanel = false; // Disable AI panel if no CURL
#endif
    
    std::cout << "✓ Initialization complete!" << std::endl;
    return true;
}

void UI::Run() {
    HWND hwnd = (HWND)window_;
    
    // Main loop
    bool done = false;
    while (!done) {
        // Poll and handle messages
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;
        
        // Handle window resize
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }
        
        // Start ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        // Render our UI
        RenderFrame();
        
        // Rendering
        ImGui::Render();
        const float clear_color[4] = { 
            appState_.theme.background.x, 
            appState_.theme.background.y, 
            appState_.theme.background.z, 
            appState_.theme.background.w 
        };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        
        // Update and render additional platform windows
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        
        g_pSwapChain->Present(1, 0); // Present with vsync
    }
}

void UI::Shutdown() {
    if (window_) {
        SaveConfiguration();
        
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        
        CleanupDeviceD3D();
        
        HWND hwnd = (HWND)window_;
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(L"NeuroShell", ::GetModuleHandle(nullptr));
        
        window_ = nullptr;
    }
}

void UI::RenderFrame() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                                   ImGuiWindowFlags_NoBackground;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("MainWindow", nullptr, windowFlags);
    ImGui::PopStyleVar();
    
    // Top toolbar with buttons
    RenderToolbar();
    
    // Main terminal area - full screen like real CMD
    ImGui::BeginChild("TerminalFullScreen", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
    RenderCMDStyle();
    ImGui::EndChild();
    
    ImGui::End();
    
    // Floating panels
    if (showHistorySidebar_) {
        RenderHistorySidebar();
    }
    
    if (appState_.showAIPanel) {
        RenderAIPanel();
    }
    
    if (showSettings_) {
        RenderSettingsWindow();
    }
    
    if (appState_.showDemoWindow) {
        ImGui::ShowDemoWindow(&appState_.showDemoWindow);
    }
    
    HandleKeyboardShortcuts();
}

void UI::RenderToolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
    
    // Logo and title
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
    ImGui::Text("  ⚡ NEUROSHELL");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    
    ImGui::Separator();
    ImGui::SameLine();
    
    // Three-dot menu button
    if (ImGui::Button("☰")) {
        // Toggle menu
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Menu");
    }
    ImGui::SameLine();
    
    // History button
    if (ImGui::Button("📜")) {
        showHistorySidebar_ = !showHistorySidebar_;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Toggle History");
    }
    ImGui::SameLine();
    
    // Settings button
    if (ImGui::Button("⚙")) {
        showSettings_ = !showSettings_;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Settings");
    }
    ImGui::SameLine();
    
    // AI button
    ImGui::PushStyleColor(ImGuiCol_Button, aiEnabled_ ? ImVec4(0.2f, 0.6f, 0.2f, 1.0f) : ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    if (ImGui::Button("🤖")) {
        aiEnabled_ = !aiEnabled_;
    }
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(aiEnabled_ ? "AI Enabled (Click to disable)" : "AI Disabled (Click to enable)");
    }
    ImGui::SameLine();
    
    ImGui::Separator();
    ImGui::SameLine();
    
    // Clear button
    if (ImGui::Button("Clear")) {
        terminal_->ClearScreen();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Clear Screen");
    }
    
    // Right side - current directory
    ImGui::SameLine(ImGui::GetWindowWidth() - 400);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
    std::string wd = terminal_->GetWorkingDirectory();
    if (wd.length() > 40) {
        wd = "..." + wd.substr(wd.length() - 37);
    }
    ImGui::Text("%s", wd.c_str());
    ImGui::PopStyleColor();
    
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
    
    ImGui::Separator();
}

void UI::RenderCMDStyle() {
    // Pure black background like CMD
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    
    // Everything in one scrollable area - like real CMD
    ImGui::BeginChild("CMDOutput", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    
    const auto& history = terminal_->GetHistory();
    
    // Show welcome banner only when no commands have been run
    if (history.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.2f, 1.0f)); // Orange color
        
        ImGui::Text("    ");
        ImGui::Text("     _   _ _____ _   _ ____   ___  ____  _   _ _____ _     _     ");
        ImGui::Text("    | \\ | | ____| | | |  _ \\ / _ \\/ ___|| | | | ____| |   | |    ");
        ImGui::Text("    |  \\| |  _| | | | | |_) | | | \\___ \\| |_| |  _| | |   | |    ");
        ImGui::Text("    | |\\  | |___| |_| |  _ <| |_| |___) |  _  | |___| |___| |___ ");
        ImGui::Text("    |_| \\_|_____|\\___/|_| \\_\\\\___/|____/|_| |_|_____|_____|_____|");
        ImGui::Text("    ");
        
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.9f, 1.0f, 1.0f)); // Light blue
        ImGui::Text("    Natural Language Command Line Interface v1.0");
        ImGui::Text("    Execute commands using plain English!");
        ImGui::PopStyleColor();
        ImGui::Spacing();
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.3f, 1.0f)); // Yellow
        ImGui::Text("    Type 'help' for assistance, 'exit' to quit.");
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Gray
        ImGui::Text("    Examples:");
        ImGui::Text("      - list all files");
        ImGui::Text("      - show me running processes");
        ImGui::Text("      - create a folder called projects");
        ImGui::Text("      - what's my ip address");
        ImGui::Text("      - delete file test.txt");
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.3f, 1.0f)); // Green
        ImGui::Text("    Click the 🤖 button in the toolbar to enable AI translation!");
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        ImGui::Spacing();
    }
    
    // Render command history in CMD style
    for (size_t i = 0; i < history.size(); ++i) {
        const auto& block = history[i];
        
        // Show prompt and command
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
        ImGui::Text("%s>%s", terminal_->GetWorkingDirectory().c_str(), block.input.c_str());
        ImGui::PopStyleColor();
        
        // Show output
        if (!block.output.empty()) {
            if (block.status == CommandStatus::Failed) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
            }
            ImGui::TextWrapped("%s", block.output.c_str());
            ImGui::PopStyleColor();
        }
        
        ImGui::Spacing();
    }
    
    // Current prompt line where user types - INLINE like real CMD
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    ImGui::Text("%s>", terminal_->GetWorkingDirectory().c_str());
    ImGui::PopStyleColor();
    ImGui::SameLine();
    
    // Invisible input box inline with prompt
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    
    if (focusCommandInput_) {
        ImGui::SetKeyboardFocusHere();
        focusCommandInput_ = false;
    }
    
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##CMDInput", commandInputBuffer_, sizeof(commandInputBuffer_), 
                        ImGuiInputTextFlags_EnterReturnsTrue)) {
        HandleCommandInput();
        scrollToBottom_ = true;
        focusCommandInput_ = true;
    }
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    
    // Auto-scroll to bottom
    if (scrollToBottom_) {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom_ = false;
    }
    
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

void UI::RenderHistorySidebar() {
    ImGui::SetNextWindowSize(ImVec2(historySidebarWidth_, -1), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.9f);
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    
    ImGui::Begin("📜 Command History", &showHistorySidebar_);
    
    const auto& history = terminal_->GetHistory();
    
    if (history.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::TextWrapped("No commands yet.\n\nType commands in the terminal below!");
        ImGui::PopStyleColor();
        ImGui::End();
        ImGui::PopStyleColor(2);
        return;
    }
    
    // Reverse order - newest first
    for (int i = static_cast<int>(history.size()) - 1; i >= 0; --i) {
        const auto& block = history[i];
        ImGui::PushID(i);
        
        // Status icon and color
        const char* icon = "⏳";
        ImVec4 color = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        if (block.status == CommandStatus::Success) {
            icon = "✓";
            color = ImVec4(0.3f, 1.0f, 0.3f, 1.0f);
        } else if (block.status == CommandStatus::Failed) {
            icon = "✗";
            color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
        }
        
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("%s", icon);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        
        // Command text
        std::string cmdText = block.input;
        if (cmdText.length() > 35) {
            cmdText = cmdText.substr(0, 32) + "...";
        }
        
        if (ImGui::Selectable(cmdText.c_str(), false)) {
            strncpy_s(commandInputBuffer_, block.input.c_str(), sizeof(commandInputBuffer_) - 1);
            focusCommandInput_ = true;
        }
        
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Command: %s", block.input.c_str());
            ImGui::Text("Exit Code: %d", block.exitCode);
            
            auto time = std::chrono::system_clock::to_time_t(block.timestamp);
            char timeStr[100];
            std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", std::localtime(&time));
            ImGui::Text("Time: %s", timeStr);
            
            if (block.isAIGenerated) {
                ImGui::Separator();
                ImGui::Text("🤖 AI: %s", block.aiPrompt.c_str());
            }
            ImGui::EndTooltip();
        }
        
        ImGui::PopID();
    }
    
    ImGui::End();
    ImGui::PopStyleColor(2);
}

void UI::RenderTerminalArea() {
    // Terminal title and info
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 12));
    
    ImGui::PushStyleColor(ImGuiCol_Text, appState_.theme.accent);
    ImGui::Text(">");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("Terminal Output");
    
    ImGui::SameLine(ImGui::GetWindowWidth() - 120);
    if (ImGui::SmallButton("Clear")) {
        terminal_->ClearScreen();
    }
    
    ImGui::PopStyleVar();
    ImGui::Separator();
    
    // Output area
    ImGui::BeginChild("TerminalOutput", ImVec2(0, -80), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    RenderCommandHistory();
    
    if (scrollToBottom_) {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom_ = false;
    }
    ImGui::EndChild();
    
    // Command input area
    RenderCommandInput();
}

void UI::RenderCommandHistory() {
    const auto& history = terminal_->GetHistory();
    
    if (history.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, appState_.theme.textDim);
        ImGui::Text("\nWelcome to NeuroShell!\n");
        ImGui::Text("Type a command and press Enter to execute.");
        ImGui::Text("Examples: dir, cd .., echo Hello World\n");
        ImGui::PopStyleColor();
        return;
    }
    
    for (size_t i = 0; i < history.size(); ++i) {
        RenderCommandBlock(history[i], static_cast<int>(i));
    }
}

void UI::RenderCommandBlock(const CommandBlock& block, int index) {
    ImGui::PushID(index);
    
    // Command prompt line
    ImGui::PushStyleColor(ImGuiCol_Text, appState_.theme.accent);
    ImGui::Text(">");
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
    ImGui::TextWrapped("%s", block.input.c_str());
    ImGui::PopStyleColor();
    
    // AI badge if AI-generated
    if (block.isAIGenerated) {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 1.0f, 0.5f, 1.0f));
        ImGui::Text("[AI]");
        ImGui::PopStyleColor();
    }
    
    // Output
    if (!block.output.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, appState_.theme.text);
        ImGui::TextWrapped("%s", block.output.c_str());
        ImGui::PopStyleColor();
    }
    
    // Status line
    ImGui::PushStyleColor(ImGuiCol_Text, appState_.theme.textDim);
    const char* statusStr = "⏳ Running";
    ImVec4 statusColor = appState_.theme.textDim;
    
    if (block.status == CommandStatus::Success) {
        statusStr = "✓ Success";
        statusColor = appState_.theme.successOutput;
    } else if (block.status == CommandStatus::Failed) {
        statusStr = "✗ Failed";
        statusColor = appState_.theme.errorOutput;
    }
    
    ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
    ImGui::Text("%s", statusStr);
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::Text("| Exit: %d", block.exitCode);
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::PopID();
}

void UI::RenderCommandInput() {
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
    
    // Prompt symbol
    ImGui::PushStyleColor(ImGuiCol_Text, appState_.theme.accent);
    ImGui::Text(">");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    
    // Auto-focus
    if (focusCommandInput_) {
        ImGui::SetKeyboardFocusHere();
        focusCommandInput_ = false;
    }
    
    ImGui::SetNextItemWidth(-120);
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;
    
    if (ImGui::InputText("##CommandInput", commandInputBuffer_, sizeof(commandInputBuffer_), flags)) {
        HandleCommandInput();
        scrollToBottom_ = true;
        focusCommandInput_ = true;
    }
    
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, appState_.theme.accent);
    if (ImGui::Button("Execute", ImVec2(100, 0)) && commandInputBuffer_[0] != '\0') {
        HandleCommandInput();
        scrollToBottom_ = true;
        focusCommandInput_ = true;
    }
    ImGui::PopStyleColor();
    
    ImGui::PopStyleVar();
    
    // Quick commands
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, appState_.theme.textDim);
    ImGui::Text("Quick: ");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    
    if (ImGui::SmallButton("dir")) {
        strcpy_s(commandInputBuffer_, "dir");
        focusCommandInput_ = true;
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("cls")) {
        terminal_->ClearScreen();
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("help")) {
        strcpy_s(commandInputBuffer_, "help");
        focusCommandInput_ = true;
    }
}

void UI::RenderAIPanel() {
#ifdef ENABLE_CURL
    ImGui::SetNextWindowSize(ImVec2(aiPanelWidth_, -1), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - aiPanelWidth_ - 10, 30), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("AI Assistant", &appState_.showAIPanel);
    
    // AI Status
    if (aiClient_ && aiClient_->IsAvailable()) {
        ImGui::PushStyleColor(ImGuiCol_Text, appState_.theme.successOutput);
        ImGui::Text("🟢 AI Connected");
        ImGui::PopStyleColor();
        
        const auto& config = aiClient_->GetConfig();
        ImGui::Text("Provider: %s", 
                   config.provider == AIProvider::OpenAI ? "OpenAI" :
                   config.provider == AIProvider::Groq ? "Groq" :
                   config.provider == AIProvider::Gemini ? "Gemini" :
                   config.provider == AIProvider::Ollama ? "Ollama" : "None");
        ImGui::Text("Model: %s", config.model.c_str());
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, appState_.theme.errorOutput);
        ImGui::Text("🔴 AI Not Configured");
        ImGui::PopStyleColor();
        ImGui::TextWrapped("Configure AI in settings to use natural language commands.");
    }
    
    ImGui::Separator();
    
    // AI Input
    ImGui::Text("Ask AI to generate commands:");
    ImGui::InputTextMultiline("##AIInput", aiInputBuffer_, sizeof(aiInputBuffer_), 
                              ImVec2(-1, 100));
    
    if (ImGui::Button("Generate Command", ImVec2(-1, 0)) && aiInputBuffer_[0] != '\0') {
        HandleAIInput();
    }
    
    ImGui::Separator();
    
    // Quick examples
    ImGui::Text("Examples:");
    if (ImGui::SmallButton("Create folder 'test'")) {
        strcpy(aiInputBuffer_, "create a folder called test");
    }
    if (ImGui::SmallButton("List files")) {
        strcpy(aiInputBuffer_, "show me all files in current directory");
    }
    if (ImGui::SmallButton("System info")) {
        strcpy(aiInputBuffer_, "show system information");
    }
    
    ImGui::End();
#else
    // No CURL - show message
    ImGui::SetNextWindowSize(ImVec2(aiPanelWidth_, 200), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - aiPanelWidth_ - 10, 30), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("AI Assistant", &appState_.showAIPanel);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
    ImGui::TextWrapped("AI features require CURL library.");
    ImGui::PopStyleColor();
    ImGui::TextWrapped("\nRebuild with: cmake -DENABLE_CURL=ON");
    ImGui::End();
#endif
}

void UI::RenderStatusBar() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - 25));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 25));
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                   ImGuiWindowFlags_NoSavedSettings;
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, appState_.theme.titleBar);
    ImGui::Begin("StatusBar", nullptr, windowFlags);
    
    ImGui::Text("%s", statusMessage_.c_str());
    
    ImGui::End();
    ImGui::PopStyleColor();
}

void UI::HandleCommandInput() {
    std::string input = commandInputBuffer_;
    if (input.empty()) return;
    
    // Check if AI is enabled and this looks like natural language
    if (aiEnabled_ && simpleAI_ && simpleAI_->IsNaturalLanguage(input)) {
        std::string translated = simpleAI_->TranslateToCommand(input);
        
        if (!translated.empty()) {
            // Show what AI translated it to
            SetStatusMessage("🤖 AI: \"" + input + "\" → " + translated);
            terminal_->ExecuteCommand(translated);
            commandInputBuffer_[0] = '\0';
            return;
        }
    }
    
    // Execute as normal command
    terminal_->ExecuteCommand(input);
    SetStatusMessage("Executed: " + input);
    commandInputBuffer_[0] = '\0';
}

void UI::HandleAIInput() {
    std::string query = aiInputBuffer_;
#ifdef ENABLE_CURL
    if (!query.empty() && aiClient_ && aiClient_->IsAvailable()) {
        ProcessAIQuery(query);
        aiInputBuffer_[0] = '\0';
    }
#else
    SetStatusMessage("AI not available - rebuild with CURL support");
#endif
}

void UI::ProcessAIQuery(const std::string& query) {
#ifdef ENABLE_CURL
    SetStatusMessage("Processing AI query...");
    
    // Async AI call
    aiClient_->TranslateToCommandAsync(query, [this, query](const AIResponse& response) {
        if (response.success && !response.commands.empty()) {
            for (const auto& cmd : response.commands) {
                ExecuteAICommand(cmd, query);
            }
            SetStatusMessage("AI generated " + std::to_string(response.commands.size()) + " command(s)");
        } else {
            SetStatusMessage("AI Error: " + response.error);
        }
    });
#else
    SetStatusMessage("AI features not available - rebuild with CURL");
#endif
}

void UI::ExecuteAICommand(const std::string& command, const std::string& originalQuery) {
    terminal_->ExecuteAICommand(command, originalQuery);
    scrollToBottom_ = true;
}

void UI::HandleKeyboardShortcuts() {
    // Ctrl+H: Toggle history sidebar
    if (IsKeyComboPressed(ImGuiKey_H, true)) {
        showHistorySidebar_ = !showHistorySidebar_;
    }
    
    // Ctrl+L: Clear screen
    if (IsKeyComboPressed(ImGuiKey_L, true)) {
        terminal_->ClearScreen();
        SetStatusMessage("Screen cleared");
    }
    
    // Ctrl+Shift+C: Clear history
    if (IsKeyComboPressed(ImGuiKey_C, true, true)) {
        terminal_->ClearHistory();
        SetStatusMessage("History cleared");
    }
    
    // Ctrl+K: Focus command input
    if (IsKeyComboPressed(ImGuiKey_K, true)) {
        focusCommandInput_ = true;
    }
    
    // Ctrl+J: Toggle AI panel
    if (IsKeyComboPressed(ImGuiKey_J, true)) {
        appState_.showAIPanel = !appState_.showAIPanel;
    }
    
    // Ctrl+Comma: Open settings
    if (IsKeyComboPressed(ImGuiKey_Comma, true)) {
        showSettings_ = true;
    }
    
    // Ctrl+R: Focus command input
    if (IsKeyComboPressed(ImGuiKey_R, true)) {
        focusCommandInput_ = true;
    }
}

bool UI::IsKeyComboPressed(ImGuiKey key, bool ctrl, bool shift) {
    ImGuiIO& io = ImGui::GetIO();
    bool ctrlDown = ctrl ? (io.KeyCtrl) : true;
    bool shiftDown = shift ? (io.KeyShift) : true;
    return ImGui::IsKeyPressed(key) && ctrlDown && shiftDown;
}

void UI::SetStatusMessage(const std::string& message) {
    statusMessage_ = message;
}

void UI::LoadConfiguration() {
    // Load from config/settings.json
    // TODO: Implement JSON config loading
    appState_.theme = Theme::GetTokyoNightColors();
}

void UI::SaveConfiguration() {
    // Save to config/settings.json
    // TODO: Implement JSON config saving
}

void UI::ShowSettingsWindow() {
    showSettings_ = true;
}

void UI::RenderSettingsWindow() {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    
    if (ImGui::Begin("⚙ Settings", &showSettings_)) {
        if (ImGui::BeginTabBar("SettingsTabs")) {
            // Appearance tab
            if (ImGui::BeginTabItem("Appearance")) {
                ImGui::Spacing();
                ImGui::Text("Font Settings");
                ImGui::Separator();
                ImGui::SliderFloat("Font Size", &fontSize_, 12.0f, 24.0f, "%.0f pt");
                
                ImGui::Spacing();
                ImGui::Text("Transparency");
                ImGui::Separator();
                ImGui::SliderFloat("Window Opacity", &terminalOpacity_, 0.5f, 1.0f, "%.2f");
                
                ImGui::Spacing();
                ImGui::Text("Layout");
                ImGui::Separator();
                ImGui::SliderFloat("History Sidebar Width", &historySidebarWidth_, 200.0f, 500.0f, "%.0f px");
                ImGui::Checkbox("Show History Sidebar by Default", &showHistorySidebar_);
                
                ImGui::Spacing();
                if (ImGui::Button("Reset to Defaults", ImVec2(150, 0))) {
                    fontSize_ = 16.0f;
                    terminalOpacity_ = 0.95f;
                    historySidebarWidth_ = 300.0f;
                }
                
                ImGui::EndTabItem();
            }
            
            // Terminal tab
            if (ImGui::BeginTabItem("Terminal")) {
                ImGui::Spacing();
                ImGui::Text("Shell Settings");
                ImGui::Separator();
                
                static char shellPath[512] = "cmd.exe";
                ImGui::InputText("Shell Path", shellPath, sizeof(shellPath));
                ImGui::Text("Default: cmd.exe");
                
                ImGui::Spacing();
                ImGui::Text("Behavior");
                ImGui::Separator();
                
                static bool autoScroll = true;
                static bool confirmExit = true;
                ImGui::Checkbox("Auto-scroll to bottom", &autoScroll);
                ImGui::Checkbox("Confirm before exit", &confirmExit);
                ImGui::Checkbox("Save command history", &confirmExit);
                
                ImGui::EndTabItem();
            }
            
            // AI tab
            if (ImGui::BeginTabItem("AI Assistant")) {
                ImGui::Spacing();
                
#ifdef ENABLE_CURL
                ImGui::Text("AI Provider Configuration");
                ImGui::Separator();
                
                const char* providers[] = { "OpenAI", "Groq", "Gemini", "Ollama", "None" };
                static int currentProvider = 0;
                ImGui::Combo("Provider", &currentProvider, providers, IM_ARRAYSIZE(providers));
                
                ImGui::Spacing();
                static char apiKey[256] = "";
                ImGui::InputText("API Key", apiKey, sizeof(apiKey), ImGuiInputTextFlags_Password);
                
                static char model[128] = "gpt-4";
                ImGui::InputText("Model", model, sizeof(model));
                
                ImGui::Spacing();
                if (ImGui::Button("Test Connection", ImVec2(150, 0))) {
                    SetStatusMessage("Testing AI connection...");
                }
                ImGui::SameLine();
                if (ImGui::Button("Save", ImVec2(150, 0))) {
                    SetStatusMessage("AI settings saved!");
                }
#else
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
                ImGui::TextWrapped("AI features are not available in this build.");
                ImGui::PopStyleColor();
                ImGui::Spacing();
                ImGui::TextWrapped("To enable AI features, rebuild with: cmake -DENABLE_CURL=ON");
                ImGui::Spacing();
                ImGui::TextWrapped("This requires libcurl to be installed on your system.");
#endif
                
                ImGui::EndTabItem();
            }
            
            // About tab
            if (ImGui::BeginTabItem("About")) {
                ImGui::Spacing();
                
                ImGui::PushStyleColor(ImGuiCol_Text, appState_.theme.accent);
                ImGui::Text("NeuroShell Terminal");
                ImGui::PopStyleColor();
                ImGui::Text("Version 2.0.0");
                ImGui::Spacing();
                
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Text("A modern terminal with AI integration");
                ImGui::Text("Built with DirectX11, ImGui, and C++17");
                ImGui::Spacing();
                
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Text("Keyboard Shortcuts:");
                ImGui::BulletText("Ctrl+H - Toggle history sidebar");
                ImGui::BulletText("Ctrl+J - Toggle AI panel");
                ImGui::BulletText("Ctrl+L - Clear screen");
                ImGui::BulletText("Ctrl+Shift+C - Clear history");
                ImGui::BulletText("Ctrl+K - Focus command input");
                ImGui::BulletText("Ctrl+, - Open settings");
                
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

void UI::ApplyCustomStyle() {
    Theme::SetupTerminalStyle();
}

void UI::SetupDockSpace() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
                                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("DockSpace", nullptr, windowFlags);
    ImGui::PopStyleVar(3);
    
    ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    
    ImGui::End();
}

const char* UI::GetASCIILogo() const {
    return "NEUROSHELL";  // Simplified for single line
}

} // namespace NeuroShell

// DirectX11 Helper Functions
bool CreateDeviceD3D(HWND hWnd) {
    // Setup swap chain
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
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
