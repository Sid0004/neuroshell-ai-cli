cmake_minimum_required(VERSION 3.15)

# Test executable
add_executable(neuroshell_tests
    test_parser.cpp
    test_command_mapper.cpp
    test_safety.cpp
)

# Link against main library sources (we'd need to refactor to create a library)
target_include_directories(neuroshell_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# Link sources (simplified - in real project, create a library)
target_sources(neuroshell_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/src/nlp/parser.cpp
    ${PROJECT_SOURCE_DIR}/src/nlp/command_mapper.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/safety.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/logger.cpp
)

# Add tests
add_test(NAME ParserTests COMMAND neuroshell_tests parser)
add_test(NAME MapperTests COMMAND neuroshell_tests mapper)
add_test(NAME SafetyTests COMMAND neuroshell_tests safety)

# Test discovery
enable_testing()
