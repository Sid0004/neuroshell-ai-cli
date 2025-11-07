#ifndef COLORS_H
#define COLORS_H

#include <iostream>
#include <string>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace NeuroShell {

class Colors {
public:
    static void init() {
        #ifdef _WIN32
        // Enable ANSI escape codes on Windows 10+
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
        #endif
    }

    // Reset
    static const std::string RESET;
    
    // Regular colors
    static const std::string BLACK;
    static const std::string RED;
    static const std::string GREEN;
    static const std::string YELLOW;
    static const std::string BLUE;
    static const std::string MAGENTA;
    static const std::string CYAN;
    static const std::string WHITE;
    
    // Bold
    static const std::string BOLD;
    static const std::string BOLD_RED;
    static const std::string BOLD_GREEN;
    static const std::string BOLD_YELLOW;
    static const std::string BOLD_BLUE;
    static const std::string BOLD_MAGENTA;
    static const std::string BOLD_CYAN;
    static const std::string BOLD_WHITE;
    
    // Background colors
    static const std::string BG_BLACK;
    static const std::string BG_RED;
    static const std::string BG_GREEN;
    static const std::string BG_YELLOW;
    static const std::string BG_BLUE;
    static const std::string BG_MAGENTA;
    static const std::string BG_CYAN;
    static const std::string BG_WHITE;
    
    // Special
    static const std::string UNDERLINE;
    static const std::string BLINK;
    
    // Helper functions
    static std::string success(const std::string& text) {
        return BOLD_GREEN + "✓ " + text + RESET;
    }
    
    static std::string error(const std::string& text) {
        return BOLD_RED + "✗ " + text + RESET;
    }
    
    static std::string warning(const std::string& text) {
        return BOLD_YELLOW + "⚠ " + text + RESET;
    }
    
    static std::string info(const std::string& text) {
        return BOLD_CYAN + "ℹ " + text + RESET;
    }
    
    static std::string command(const std::string& text) {
        return BOLD_BLUE + "$ " + text + RESET;
    }
    
    static std::string highlight(const std::string& text) {
        return BOLD_MAGENTA + text + RESET;
    }
};

// ANSI color codes
const std::string Colors::RESET = "\033[0m";

const std::string Colors::BLACK = "\033[30m";
const std::string Colors::RED = "\033[31m";
const std::string Colors::GREEN = "\033[32m";
const std::string Colors::YELLOW = "\033[33m";
const std::string Colors::BLUE = "\033[34m";
const std::string Colors::MAGENTA = "\033[35m";
const std::string Colors::CYAN = "\033[36m";
const std::string Colors::WHITE = "\033[37m";

const std::string Colors::BOLD = "\033[1m";
const std::string Colors::BOLD_RED = "\033[1;31m";
const std::string Colors::BOLD_GREEN = "\033[1;32m";
const std::string Colors::BOLD_YELLOW = "\033[1;33m";
const std::string Colors::BOLD_BLUE = "\033[1;34m";
const std::string Colors::BOLD_MAGENTA = "\033[1;35m";
const std::string Colors::BOLD_CYAN = "\033[1;36m";
const std::string Colors::BOLD_WHITE = "\033[1;97m";

const std::string Colors::BG_BLACK = "\033[40m";
const std::string Colors::BG_RED = "\033[41m";
const std::string Colors::BG_GREEN = "\033[42m";
const std::string Colors::BG_YELLOW = "\033[43m";
const std::string Colors::BG_BLUE = "\033[44m";
const std::string Colors::BG_MAGENTA = "\033[45m";
const std::string Colors::BG_CYAN = "\033[46m";
const std::string Colors::BG_WHITE = "\033[47m";

const std::string Colors::UNDERLINE = "\033[4m";
const std::string Colors::BLINK = "\033[5m";

} // namespace NeuroShell

#endif // COLORS_H
