#include "ui/ui.h"
#include <iostream>
#include <exception>

#ifdef _WIN32
#include <windows.h>
#endif

void PrintBanner() {
    std::cout << R"(
    ███╗   ██╗███████╗██╗   ██╗██████╗  ██████╗ ███████╗██╗  ██╗███████╗██╗     ██╗     
    ████╗  ██║██╔════╝██║   ██║██╔══██╗██╔═══██╗██╔════╝██║  ██║██╔════╝██║     ██║     
    ██╔██╗ ██║█████╗  ██║   ██║██████╔╝██║   ██║███████╗███████║█████╗  ██║     ██║     
    ██║╚██╗██║██╔══╝  ██║   ██║██╔══██╗██║   ██║╚════██║██╔══██║██╔══╝  ██║     ██║     
    ██║ ╚████║███████╗╚██████╔╝██║  ██║╚██████╔╝███████║██║  ██║███████╗███████╗███████╗
    ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝
    )" << std::endl;
    std::cout << "    Natural Language Command Line Interface v1.0" << std::endl;
    std::cout << "    Execute commands using plain English!" << std::endl;
    std::cout << "    Type 'help' for assistance, 'exit' to quit." << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv) {
#ifdef _WIN32
    // Enable ANSI color support on Windows
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hConsole, &mode)) {
            SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }
#endif

    try {
        PrintBanner();
        std::cout << "Starting NeuroShell..." << std::endl;
        NeuroShell::UI app;
        
        std::cout << "Initializing UI..." << std::endl;
        // Initialize UI with window
        if (!app.Initialize(1600, 900, "NeuroShell Terminal")) {
            std::cerr << "Failed to initialize NeuroShell UI" << std::endl;
            std::cerr << "Please check:" << std::endl;
            std::cerr << "  - Graphics drivers are up to date" << std::endl;
            std::cerr << "  - OpenGL 3.3+ is supported" << std::endl;
            std::cin.get(); // Wait for user input
            return 1;
        }
        
        std::cout << "Running main loop..." << std::endl;
        // Run main loop
        app.Run();
        
        std::cout << "Shutting down..." << std::endl;
        // Cleanup
        app.Shutdown();
        
        std::cout << "Exit successful" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        std::cin.get(); // Wait for user input
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        std::cin.get(); // Wait for user input
        return 1;
    }
}
