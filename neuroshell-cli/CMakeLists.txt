cmake_minimum_required(VERSION 3.15)

# vcpkg manifest mode support - use local vcpkg_installed
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows")
    message(STATUS "Using vcpkg manifest mode from vcpkg_installed/")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows")
endif()

project(NeuroShell VERSION 2.0.0 LANGUAGES CXX)

# Build Configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Options
option(ENABLE_CURL "Enable CURL for AI features (requires libcurl)" ON)

# Dependencies
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/thirdparty/imgui)
if(NOT EXISTS ${IMGUI_DIR})
    message(FATAL_ERROR "ImGui not found. Run setup-dependencies.bat first.")
endif()

set(GLFW_DIR ${CMAKE_SOURCE_DIR}/thirdparty/glfw)
if(NOT EXISTS ${GLFW_DIR})
    message(FATAL_ERROR "GLFW not found. Run setup-dependencies.bat first.")
endif()

set(JSON_DIR ${CMAKE_SOURCE_DIR}/thirdparty/json)
if(NOT EXISTS ${JSON_DIR})
    message(FATAL_ERROR "nlohmann_json not found. Run setup-dependencies.bat first.")
endif()

set(GLAD_DIR ${CMAKE_SOURCE_DIR}/thirdparty/glad)
if(NOT EXISTS ${GLAD_DIR})
    message(FATAL_ERROR "GLAD not found. Should be in thirdparty/glad directory.")
endif()

# Build GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_DIR} ${CMAKE_BINARY_DIR}/glfw EXCLUDE_FROM_ALL)

# CURL (optional)
if(ENABLE_CURL)
    find_package(CURL REQUIRED)
    if(CURL_FOUND)
        message(STATUS "CURL found - AI features enabled")
        add_compile_definitions(ENABLE_CURL)
    else()
        message(WARNING "CURL not found - AI features disabled")
    endif()
else()
    message(STATUS "CURL disabled - AI features will not be available")
endif()

# ImGui Library
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(imgui PUBLIC d3d11 dxgi d3dcompiler)

# GLAD Library
add_library(glad STATIC
    ${GLAD_DIR}/src/glad.c
)

target_include_directories(glad PUBLIC
    ${GLAD_DIR}/include
)

# Main Executable
file(GLOB_RECURSE NEUROSHELL_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Keep SimpleAI always available (offline AI), only exclude HTTP-based AI if CURL disabled
if(NOT ENABLE_CURL)
    list(FILTER NEUROSHELL_SOURCES EXCLUDE REGEX ".*ai/ai_client\\.cpp$")
    list(FILTER NEUROSHELL_SOURCES EXCLUDE REGEX ".*ai/http_client\\.cpp$")
endif()

add_executable(neuroshell ${NEUROSHELL_SOURCES})

target_include_directories(neuroshell PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${JSON_DIR}/include
)

target_link_libraries(neuroshell PRIVATE
    imgui
    d3d11
    dxgi
    d3dcompiler
)

if(ENABLE_CURL AND CURL_FOUND)
    target_link_libraries(neuroshell PRIVATE CURL::libcurl)
endif()

# Windows-specific settings
if(WIN32)
    target_compile_definitions(neuroshell PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )
    set_target_properties(neuroshell PROPERTIES
        WIN32_EXECUTABLE OFF
    )
endif()

# Installation
install(TARGETS neuroshell DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/config DESTINATION .)
install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION .)

message(STATUS "NeuroShell Configuration Complete")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  AI Features: ${ENABLE_CURL}")