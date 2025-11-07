#pragma once

#include "common/types.h"
#include "terminal/terminal.h"
#ifdef ENABLE_CURL
#include "ai/ai_client.h"
#endif
#include "ai/simple_ai.h"
#include "imgui.h"
#include <memory>
#include <string>

namespace NeuroShell {

class UI {
public:
    UI();
    ~UI();
    
    // Initialize UI (window, DirectX11, ImGui)
    bool Initialize(int width, int height, const char* title);
    
    // Main render loop
    void Run();
    
    // Cleanup
    void Shutdown();
    
private:
    void* window_;  // HWND window handle
    std::unique_ptr<Terminal> terminal_;
#ifdef ENABLE_CURL
    std::unique_ptr<AIClient> aiClient_;
#endif
    std::unique_ptr<SimpleAI> simpleAI_;
    AppState appState_;
    
    // UI state
    char commandInputBuffer_[1024];
    char aiInputBuffer_[1024];
    bool scrollToBottom_;
    bool focusCommandInput_;
    float aiPanelWidth_;
    bool showHistorySidebar_;
    float historySidebarWidth_;
    bool showSettings_;
    float fontSize_;
    float terminalOpacity_;
    bool aiEnabled_;
    std::string statusMessage_;
    bool showWelcomeBanner_;
    
    // Rendering methods
    void RenderFrame();
    void RenderToolbar();
    void RenderCMDStyle();
    void RenderHistorySidebar();
    void RenderTerminalArea();
    void RenderCommandHistory();
    void RenderCommandInput();
    void RenderAIPanel();
    void RenderStatusBar();
    void RenderSettingsWindow();
    
    // Command block rendering
    void RenderCommandBlock(const CommandBlock& block, int index);
    
    // Input handling
    void HandleCommandInput();
    void HandleAIInput();
    void HandleKeyboardShortcuts();
    
    // AI interaction
    void ProcessAIQuery(const std::string& query);
    void ExecuteAICommand(const std::string& command, const std::string& originalQuery);
    
    // Helper methods
    void SetStatusMessage(const std::string& message);
    void LoadConfiguration();
    void SaveConfiguration();
    void ShowSettingsWindow();
    
    // ImGui style helpers
    void ApplyCustomStyle();
    void SetupDockSpace();
    
    // ASCII logo
    const char* GetASCIILogo() const;
    
    // Keyboard shortcuts
    void RegisterShortcuts();
    bool IsKeyComboPressed(ImGuiKey key, bool ctrl, bool shift = false);
};

} // namespace NeuroShell
