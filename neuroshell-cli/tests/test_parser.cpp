#include "../include/nlp/parser.h"
#include <iostream>
#include <cassert>

using namespace neuroshell::nlp;

void test_tokenization() {
    Parser parser;
    auto tokens = parser.tokenize("list all files in this directory");
    
    assert(tokens.size() > 0);
    assert(tokens[0].word == "list");
    
    std::cout << "✓ Tokenization test passed" << std::endl;
}

void test_action_extraction() {
    Parser parser;
    auto intent = parser.parse("show me all running processes");
    
    assert(intent.action == "list");
    std::cout << "✓ Action extraction test passed" << std::endl;
}

void test_target_extraction() {
    Parser parser;
    auto intent = parser.parse("list all files");
    
    assert(intent.target == "file");
    std::cout << "✓ Target extraction test passed" << std::endl;
}

void test_confidence_calculation() {
    Parser parser;
    auto intent = parser.parse("list all files");
    
    assert(intent.confidence > 0.0f);
    assert(intent.confidence <= 1.0f);
    
    std::cout << "✓ Confidence calculation test passed" << std::endl;
}

void test_unknown_command() {
    Parser parser;
    auto intent = parser.parse("blah blah random words");
    
    assert(intent.confidence < 0.5f);
    
    std::cout << "✓ Unknown command test passed" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "\n=== Running Parser Tests ===\n" << std::endl;
    
    try {
        test_tokenization();
        test_action_extraction();
        test_target_extraction();
        test_confidence_calculation();
        test_unknown_command();
        
        std::cout << "\n✅ All parser tests passed!\n" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
