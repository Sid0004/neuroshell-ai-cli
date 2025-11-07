#pragma once

#include "common/types.h"
#include "imgui.h"

namespace NeuroShell {

class Theme {
public:
    // Apply Tokyo Night dark theme (Warp-style)
    static void ApplyTokyoNight();
    
    // Apply custom theme colors
    static void ApplyCustomTheme(const ThemeColors& colors);
    
    // Get predefined color schemes
    static ThemeColors GetTokyoNightColors();
    static ThemeColors GetWarpDarkColors();
    static ThemeColors GetHyperColors();
    
    // Setup ImGui style for terminal application
    static void SetupTerminalStyle();
    
    // Enable docking
    static void EnableDocking();
    
private:
    static void SetImGuiColors(const ThemeColors& colors);
};

} // namespace NeuroShell
