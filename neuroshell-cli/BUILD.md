# Building NeuroShell

## Windows (Visual Studio)

### Using CMake GUI:
1. Open CMake GUI
2. Set source directory to the project root
3. Set build directory to `build/`
4. Click "Configure" and select your Visual Studio version
5. Click "Generate"
6. Open the generated `.sln` file in Visual Studio
7. Build the solution (F7)

### Using Command Line:
```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Using Visual Studio directly:
1. Install "C++ CMake tools for Windows" in Visual Studio Installer
2. Open Visual Studio
3. File -> Open -> CMake...
4. Select CMakeLists.txt
5. Press F7 to build

## Linux/macOS

### Prerequisites:
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake

# macOS
brew install cmake
```

### Build:
```bash
mkdir build && cd build
cmake ..
make -j4
```

## Running

After building, the executable will be in `build/bin/`:

```bash
# Windows
build\bin\neuroshell.exe

# Linux/macOS
./build/bin/neuroshell
```

## Build Options

### Enable API Integration (Optional):
```bash
cmake .. -DENABLE_API=ON
```

### Disable Tests:
```bash
cmake .. -DBUILD_TESTS=OFF
```

### Release Build:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

## Troubleshooting

### Windows:
- Ensure you have Visual Studio 2017 or newer
- Make sure C++ development tools are installed

### Linux:
- Install g++ 7.0 or newer: `sudo apt-get install g++-7`

### macOS:
- Install Xcode Command Line Tools: `xcode-select --install`

## Docker Build (Optional)

```bash
docker build -t neuroshell .
docker run -it neuroshell
```
