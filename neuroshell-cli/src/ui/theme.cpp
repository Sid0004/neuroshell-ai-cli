#include "ui/theme.h"
#include "imgui.h"

namespace NeuroShell {

void Theme::ApplyTokyoNight() {
    ThemeColors colors = GetTokyoNightColors();
    SetImGuiColors(colors);
    SetupTerminalStyle();
}

void Theme::ApplyCustomTheme(const ThemeColors& colors) {
    SetImGuiColors(colors);
    SetupTerminalStyle();
}

ThemeColors Theme::GetTokyoNightColors() {
    ThemeColors colors;
    
    // Tokyo Night Storm theme
    colors.background = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);      // #1a1b21
    colors.titleBar = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);        // #16161e
    colors.commandInput = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);    // #24283b
    colors.commandOutput = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);   // #1f2335
    colors.errorOutput = ImVec4(0.22f, 0.11f, 0.13f, 1.00f);     // Dark red
    colors.successOutput = ImVec4(0.11f, 0.19f, 0.13f, 1.00f);   // Dark green
    colors.aiPanel = ImVec4(0.09f, 0.10f, 0.12f, 1.00f);         // #1a1b26
    colors.text = ImVec4(0.78f, 0.81f, 0.88f, 1.00f);            // #c0caf5
    colors.textDim = ImVec4(0.47f, 0.51f, 0.61f, 1.00f);         // #565f89
    colors.accent = ImVec4(0.46f, 0.67f, 0.98f, 1.00f);          // #7aa2f7 (blue)
    colors.border = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);          // #414868
    colors.scrollbar = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);       // #292e42
    
    return colors;
}

ThemeColors Theme::GetWarpDarkColors() {
    ThemeColors colors;
    
    // Warp-inspired dark theme
    colors.background = ImVec4(0.07f, 0.08f, 0.10f, 1.00f);
    colors.titleBar = ImVec4(0.05f, 0.06f, 0.08f, 1.00f);
    colors.commandInput = ImVec4(0.11f, 0.12f, 0.14f, 1.00f);
    colors.commandOutput = ImVec4(0.09f, 0.10f, 0.12f, 1.00f);
    colors.errorOutput = ImVec4(0.25f, 0.08f, 0.10f, 1.00f);
    colors.successOutput = ImVec4(0.08f, 0.22f, 0.12f, 1.00f);
    colors.aiPanel = ImVec4(0.06f, 0.07f, 0.09f, 1.00f);
    colors.text = ImVec4(0.85f, 0.87f, 0.90f, 1.00f);
    colors.textDim = ImVec4(0.50f, 0.53f, 0.60f, 1.00f);
    colors.accent = ImVec4(0.38f, 0.78f, 0.95f, 1.00f);
    colors.border = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
    colors.scrollbar = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    
    return colors;
}

ThemeColors Theme::GetHyperColors() {
    ThemeColors colors;
    
    // Hyper terminal theme
    colors.background = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors.titleBar = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors.commandInput = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    colors.commandOutput = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
    colors.errorOutput = ImVec4(0.20f, 0.00f, 0.00f, 1.00f);
    colors.successOutput = ImVec4(0.00f, 0.20f, 0.00f, 1.00f);
    colors.aiPanel = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
    colors.text = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors.textDim = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors.accent = ImVec4(0.00f, 1.00f, 0.65f, 1.00f);
    colors.border = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors.scrollbar = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    
    return colors;
}

void Theme::SetImGuiColors(const ThemeColors& colors) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* styleColors = style.Colors;
    
    // Window
    styleColors[ImGuiCol_WindowBg] = colors.background;
    styleColors[ImGuiCol_ChildBg] = colors.background;
    styleColors[ImGuiCol_PopupBg] = colors.commandInput;
    
    // Title bar
    styleColors[ImGuiCol_TitleBg] = colors.titleBar;
    styleColors[ImGuiCol_TitleBgActive] = colors.titleBar;
    styleColors[ImGuiCol_TitleBgCollapsed] = colors.titleBar;
    
    // Text
    styleColors[ImGuiCol_Text] = colors.text;
    styleColors[ImGuiCol_TextDisabled] = colors.textDim;
    styleColors[ImGuiCol_TextSelectedBg] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.35f);
    
    // Borders
    styleColors[ImGuiCol_Border] = colors.border;
    styleColors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Frame (buttons, inputs, etc.)
    styleColors[ImGuiCol_FrameBg] = colors.commandInput;
    styleColors[ImGuiCol_FrameBgHovered] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.15f);
    styleColors[ImGuiCol_FrameBgActive] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.25f);
    
    // Buttons
    styleColors[ImGuiCol_Button] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.20f);
    styleColors[ImGuiCol_ButtonHovered] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.35f);
    styleColors[ImGuiCol_ButtonActive] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.50f);
    
    // Headers (collapsing headers, tree nodes)
    styleColors[ImGuiCol_Header] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.20f);
    styleColors[ImGuiCol_HeaderHovered] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.35f);
    styleColors[ImGuiCol_HeaderActive] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.50f);
    
    // Separator
    styleColors[ImGuiCol_Separator] = colors.border;
    styleColors[ImGuiCol_SeparatorHovered] = colors.accent;
    styleColors[ImGuiCol_SeparatorActive] = colors.accent;
    
    // Scrollbar
    styleColors[ImGuiCol_ScrollbarBg] = colors.scrollbar;
    styleColors[ImGuiCol_ScrollbarGrab] = ImVec4(colors.textDim.x, colors.textDim.y, colors.textDim.z, 0.50f);
    styleColors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(colors.textDim.x, colors.textDim.y, colors.textDim.z, 0.70f);
    styleColors[ImGuiCol_ScrollbarGrabActive] = colors.accent;
    
    // Tabs
    styleColors[ImGuiCol_Tab] = colors.commandInput;
    styleColors[ImGuiCol_TabHovered] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.35f);
    styleColors[ImGuiCol_TabActive] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.25f);
    styleColors[ImGuiCol_TabUnfocused] = colors.commandInput;
    styleColors[ImGuiCol_TabUnfocusedActive] = colors.commandInput;
    
    // Docking
    styleColors[ImGuiCol_DockingPreview] = ImVec4(colors.accent.x, colors.accent.y, colors.accent.z, 0.35f);
    styleColors[ImGuiCol_DockingEmptyBg] = colors.background;
}

void Theme::SetupTerminalStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Rounding
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;
    
    // Padding and spacing
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.FramePadding = ImVec2(8.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 12.0f;
    style.GrabMinSize = 8.0f;
    
    // Borders
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
}

void Theme::EnableDocking() {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    // When viewports are enabled, tweak WindowRounding/WindowBg
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
}

} // namespace NeuroShell
