#!/bin/bash

echo "========================================"
echo "NeuroShell AI CLI - Build Script"
echo "========================================"
echo

# Clean previous build
if [ -d "build" ]; then
    echo "Cleaning previous build..."
    rm -rf build
fi

# Create build directory
echo "Creating build directory..."
mkdir build
cd build

# Run CMake
echo
echo "Running CMake..."
cmake ..

if [ $? -ne 0 ]; then
    echo
    echo "ERROR: CMake configuration failed!"
    echo "Make sure CMake is installed."
    exit 1
fi

# Build the project
echo
echo "Building NeuroShell..."
make -j4

if [ $? -ne 0 ]; then
    echo
    echo "ERROR: Build failed!"
    exit 1
fi

echo
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
echo
echo "Executable location: build/bin/neuroshell"
echo
echo "To run:"
echo "  ./build/bin/neuroshell"
echo
