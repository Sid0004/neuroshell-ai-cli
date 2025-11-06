#include "../include/nlp/command_mapper.h"
#include "../include/nlp/parser.h"
#include <iostream>
#include <cassert>

using namespace neuroshell::nlp;

void test_basic_mapping() {
    Parser parser;
    CommandMapper mapper;
    
    auto intent = parser.parse("list all files");
    auto mapped = mapper.mapToCommand(intent);
    
    assert(!mapped.command.empty());
    std::cout << "✓ Basic mapping test passed" << std::endl;
}

void test_create_folder_mapping() {
    Parser parser;
    CommandMapper mapper;
    
    auto intent = parser.parse("create a folder called test");
    auto mapped = mapper.mapToCommand(intent);
    
    assert(!mapped.command.empty());
    assert(mapped.command.find("mkdir") != std::string::npos);
    
    std::cout << "✓ Create folder mapping test passed" << std::endl;
}

void test_dangerous_command_flag() {
    Parser parser;
    CommandMapper mapper;
    
    auto intent = parser.parse("delete folder important");
    auto mapped = mapper.mapToCommand(intent);
    
    assert(mapped.is_dangerous);
    assert(mapped.requires_confirmation);
    
    std::cout << "✓ Dangerous command flag test passed" << std::endl;
}

void test_platform_detection() {
    CommandMapper mapper;
    
    // This will depend on the platform
    // Just ensure it doesn't crash
    Parser parser;
    auto intent = parser.parse("list files");
    auto mapped = mapper.mapToCommand(intent);
    
    assert(!mapped.command.empty());
    
    std::cout << "✓ Platform detection test passed" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "\n=== Running Command Mapper Tests ===\n" << std::endl;
    
    try {
        test_basic_mapping();
        test_create_folder_mapping();
        test_dangerous_command_flag();
        test_platform_detection();
        
        std::cout << "\n✅ All mapper tests passed!\n" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
