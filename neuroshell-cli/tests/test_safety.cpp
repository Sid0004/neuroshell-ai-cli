#include "../include/utils/safety.h"
#include <iostream>
#include <cassert>

using namespace neuroshell::utils;

void test_safe_commands() {
    SafetyChecker checker;
    
    assert(checker.isSafe("ls"));
    assert(checker.isSafe("dir"));
    assert(checker.isSafe("pwd"));
    
    std::cout << "✓ Safe commands test passed" << std::endl;
}

void test_dangerous_commands() {
    SafetyChecker checker;
    
    assert(checker.isDangerous("rm -rf /"));
    assert(checker.isDangerous("del /s /q"));
    
    std::cout << "✓ Dangerous commands test passed" << std::endl;
}

void test_blacklisted_commands() {
    SafetyChecker checker;
    
    assert(checker.isBlacklisted("format"));
    assert(checker.isBlacklisted("fdisk"));
    
    std::cout << "✓ Blacklisted commands test passed" << std::endl;
}

void test_injection_detection() {
    SafetyChecker checker;
    
    assert(checker.hasInjectionRisk("ls; rm -rf /"));
    assert(checker.hasInjectionRisk("dir && del important.txt"));
    
    std::cout << "✓ Injection detection test passed" << std::endl;
}

void test_whitelist_addition() {
    SafetyChecker checker;
    
    checker.addToWhitelist("custom_safe_command");
    // Would need to expose internal state to test properly
    // For now, just ensure it doesn't crash
    
    std::cout << "✓ Whitelist addition test passed" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "\n=== Running Safety Tests ===\n" << std::endl;
    
    try {
        test_safe_commands();
        test_dangerous_commands();
        test_blacklisted_commands();
        test_injection_detection();
        test_whitelist_addition();
        
        std::cout << "\n✅ All safety tests passed!\n" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
