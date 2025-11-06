#include <iostream>
#include <string>
#include <memory>
#include "nlp/parser.h"
#include "nlp/command_mapper.h"
#include "executor/command_executor.h"
#include "utils/logger.h"
#include "utils/config_loader.h"

using namespace neuroshell;

// Display banner
void displayBanner() {
    std::cout << R"(
    _   __                      _____ __         ____
   / | / /__  __  ___________  / ___// /_  ___  / / /
  /  |/ / _ \/ / / / ___/ __ \ \__ \/ __ \/ _ \/ / / 
 / /|  /  __/ /_/ / /  / /_/ /___/ / / / /  __/ / /  
/_/ |_/\___/\__,_/_/   \____//____/_/ /_/\___/_/_/   
                                                      
    Natural Language Command Line Interface v1.0
    Execute commands using plain English!
    Type 'help' for assistance, 'exit' to quit.
    )" << std::endl;
}

// Display help
void displayHelp() {
    std::cout << "\n📖 NeuroShell Help\n" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  Just type what you want to do in natural language!\n" << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  • list all files" << std::endl;
    std::cout << "  • show me running processes" << std::endl;
    std::cout << "  • create a folder called projects" << std::endl;
    std::cout << "  • what's my ip address" << std::endl;
    std::cout << "  • delete file test.txt" << std::endl;
    
    std::cout << "\nCommands:" << std::endl;
    std::cout << "  help      - Show this help message" << std::endl;
    std::cout << "  exit/quit - Exit the application" << std::endl;
    std::cout << "  clear     - Clear the screen" << std::endl;
    std::cout << "  verbose   - Toggle verbose mode" << std::endl;
    std::cout << "  dryrun    - Toggle dry run mode" << std::endl;
    std::cout << std::endl;
}

// Interactive mode
void interactiveMode() {
    nlp::Parser parser;
    nlp::CommandMapper mapper;
    executor::CommandExecutor executor;
    
    bool verbose = false;
    bool dryrun = false;
    
    std::string input;
    
    while (true) {
        std::cout << "\n🤖 neuroshell> ";
        std::getline(std::cin, input);
        
        // Trim input
        input.erase(0, input.find_first_not_of(" \t"));
        input.erase(input.find_last_not_of(" \t") + 1);
        
        if (input.empty()) continue;
        
        // Handle special commands
        if (input == "exit" || input == "quit") {
            std::cout << "Goodbye! 👋" << std::endl;
            break;
        } else if (input == "help") {
            displayHelp();
            continue;
        } else if (input == "clear" || input == "cls") {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            continue;
        } else if (input == "verbose") {
            verbose = !verbose;
            executor.setVerbose(verbose);
            std::cout << "Verbose mode: " << (verbose ? "ON" : "OFF") << std::endl;
            continue;
        } else if (input == "dryrun") {
            dryrun = !dryrun;
            executor.setDryRun(dryrun);
            std::cout << "Dry run mode: " << (dryrun ? "ON" : "OFF") << std::endl;
            continue;
        }
        
        // Parse natural language
        std::cout << "🔍 Parsing command..." << std::endl;
        auto intent = parser.parse(input);
        
        // Map to CLI command
        auto mapped = mapper.mapToCommand(intent);
        
        // Check confidence
        if (mapped.confidence < 0.3f) {
            std::cout << "⚠️  Low confidence (" << (mapped.confidence * 100) 
                      << "%). Command might not be accurate." << std::endl;
        }
        
        if (mapped.command.empty()) {
            std::cout << "❌ Unable to understand command. Try rephrasing or type 'help'." << std::endl;
            continue;
        }
        
        // Show what will be executed
        std::cout << "💡 Translates to: " << mapped.command << std::endl;
        
        // Execute
        auto result = executor.execute(mapped);
        
        if (result.success) {
            std::cout << "\n✅ Command executed successfully!\n" << std::endl;
            if (!result.output.empty()) {
                std::cout << "Output:" << std::endl;
                std::cout << result.output << std::endl;
            }
            std::cout << "Execution time: " << result.execution_time << "s" << std::endl;
        } else {
            std::cout << "\n❌ Command failed!" << std::endl;
            if (!result.error.empty()) {
                std::cout << "Error: " << result.error << std::endl;
            }
        }
    }
}

// Single command mode
void singleCommandMode(const std::string& command) {
    nlp::Parser parser;
    nlp::CommandMapper mapper;
    executor::CommandExecutor executor;
    
    executor.setVerbose(true);
    
    auto intent = parser.parse(command);
    auto mapped = mapper.mapToCommand(intent);
    
    if (mapped.command.empty()) {
        std::cerr << "Error: Unable to understand command." << std::endl;
        return;
    }
    
    auto result = executor.execute(mapped);
    
    if (result.success) {
        std::cout << result.output;
    } else {
        std::cerr << "Error: " << result.error << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Initialize logger
    utils::Logger::getInstance().initialize("neuroshell.log", false);
    
    LOG_INFO("NeuroShell started");
    
    // Check if single command mode
    if (argc > 1) {
        // Concatenate all arguments as single command
        std::string command;
        for (int i = 1; i < argc; ++i) {
            if (i > 1) command += " ";
            command += argv[i];
        }
        
        singleCommandMode(command);
    } else {
        // Interactive mode
        displayBanner();
        displayHelp();
        interactiveMode();
    }
    
    LOG_INFO("NeuroShell exited");
    
    return 0;
}
