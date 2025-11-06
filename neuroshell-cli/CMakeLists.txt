cmake_minimum_required(VERSION 3.15)
project(NeuroShell VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(BUILD_TESTS "Build test suite" OFF)
option(ENABLE_API "Enable OpenAI API integration" OFF)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external
)

# Source files
set(SOURCES
    src/main.cpp
    src/nlp/parser.cpp
    src/nlp/command_mapper.cpp
    src/executor/command_executor.cpp
    src/utils/safety.cpp
    src/utils/logger.cpp
    src/utils/config_loader.cpp
)

# Add API client if enabled
if(ENABLE_API)
    list(APPEND SOURCES src/ai/api_client.cpp)
    add_definitions(-DENABLE_API_INTEGRATION)
endif()

# Create executable
add_executable(neuroshell ${SOURCES})

# Platform-specific settings
if(WIN32)
    target_compile_definitions(neuroshell PRIVATE PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(neuroshell PRIVATE PLATFORM_LINUX)
elseif(APPLE)
    target_compile_definitions(neuroshell PRIVATE PLATFORM_MACOS)
endif()

# Link libraries
if(ENABLE_API)
    find_package(CURL REQUIRED)
    target_link_libraries(neuroshell CURL::libcurl)
endif()

# Copy config files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/config/commands.json
     DESTINATION ${CMAKE_BINARY_DIR}/bin/config/)

# Install targets
install(TARGETS neuroshell DESTINATION bin)
install(FILES config/commands.json DESTINATION share/neuroshell/)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Print configuration
message(STATUS "NeuroShell Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  API Integration: ${ENABLE_API}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
