# NeuroShell Setup Guide# NeuroShell Setup Guide



This guide will walk you through setting up NeuroShell on Windows.## Prerequisites Installation



---### 1. Visual Studio 2022



## PrerequisitesDownload and install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/)



### Required SoftwareDuring installation, select:

- **Desktop development with C++**

1. **Visual Studio 2022** (Community Edition or higher)- **CMake tools for Windows**

   - Download: https://visualstudio.microsoft.com/downloads/- **MSVC v143 - VS 2022 C++ x64/x86 build tools**

   - During installation, select:- **Windows 10/11 SDK**

     - ✅ Desktop development with C++

     - ✅ MSVC v143 - VS 2022 C++ x64/x86 build tools### 2. CMake

     - ✅ Windows 10/11 SDK

     - ✅ CMake tools for WindowsDownload from [cmake.org](https://cmake.org/download/) or install via chocolatey:



2. **CMake 3.15+**```powershell

   - Download: https://cmake.org/download/choco install cmake

   - Or install via Chocolatey: `choco install cmake````

   - Verify: `cmake --version`

### 3. vcpkg (Dependency Manager)

3. **Git**

   - Download: https://git-scm.com/download/win```powershell

   - Or install via Chocolatey: `choco install git`# Clone vcpkg

   - Verify: `git --version`git clone https://github.com/Microsoft/vcpkg.git

cd vcpkg

---

# Bootstrap

## Step 1: Clone the Repository.\bootstrap-vcpkg.bat



```bash# Add to PATH (optional)

git clone https://github.com/yourusername/neuroshell-cli.gitsetx PATH "%PATH%;C:\path\to\vcpkg"

cd neuroshell-cli```

```

## Dependency Installation

---

### Option A: Using vcpkg (Recommended)

## Step 2: Install Dependencies

```powershell

NeuroShell requires the following third-party libraries:# Install required libraries

vcpkg install glfw3:x64-windows

### 2.1 ImGui (Docking Branch)vcpkg install curl:x64-windows

vcpkg install nlohmann-json:x64-windows

```bash

cd thirdparty# Integrate with Visual Studio

git clone -b docking https://github.com/ocornut/imgui.gitvcpkg integrate install

``````



### 2.2 GLFW### Option B: Manual Installation



```bash#### GLFW

git clone https://github.com/glfw/glfw.git```bash

```cd thirdparty

git clone https://github.com/glfw/glfw.git

### 2.3 nlohmann/jsoncd glfw

mkdir build && cd build

```bashcmake .. -DCMAKE_INSTALL_PREFIX=../install

git clone https://github.com/nlohmann/json.gitcmake --build . --config Release --target install

``````



### 2.4 GLAD#### GLAD

1. Go to [https://glad.dav1d.de/](https://glad.dav1d.de/)

GLAD files are already included in `thirdparty/glad/`. If missing, generate from:2. Select:

- Go to https://glad.dav1d.de/   - Language: C/C++

- Select:   - Specification: OpenGL

  - Language: C/C++   - API gl: Version 3.3+

  - Specification: OpenGL   - Profile: Core

  - API gl: Version 3.3+3. Generate and download

  - Profile: Core4. Extract to `thirdparty/glad/`

- Generate and extract to `thirdparty/glad/`

#### Dear ImGui

### 2.5 Return to Project Root```bash

cd thirdparty

```bashgit clone -b docking https://github.com/ocornut/imgui.git

cd ..```

```

#### nlohmann/json

---```bash

cd thirdparty

## Step 3: Build NeuroShellgit clone https://github.com/nlohmann/json.git

```

### Option A: Using build.bat (Recommended)

#### CURL

```bash- Download from [https://curl.se/download.html](https://curl.se/download.html)

build.bat- Or use vcpkg: `vcpkg install curl:x64-windows`

```

## Building NeuroShell

This will:

1. Check dependencies### Using build.bat (Easy)

2. Create build directory

3. Configure with CMake```cmd

4. Build Release versionbuild.bat

5. Show executable location```



### Option B: Manual BuildThis will:

1. Create build directory

```bash2. Configure with CMake

# Create build directory3. Build Release version

mkdir build4. Show location of executable

cd build

### Manual Build

# Configure with CMake

cmake -G "Visual Studio 17 2022" -A x64 ..```powershell

# Create build directory

# Build Release versionmkdir build

cmake --build . --config Releasecd build



# Executable will be in: bin\Release\neuroshell.exe# Configure (with vcpkg)

```cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake



---# Or without vcpkg (manual dependencies)

cmake ..

## Step 4: Run NeuroShell

# Build

### Option A: Using run.batcmake --build . --config Release



```bash# Run

run.bat.\bin\Release\neuroshell.exe

``````



### Option B: Manual Run## Configuration



```bash### AI Setup

cd build\bin\Release

neuroshell.exe1. Edit `config/settings.json`

```2. Choose your AI provider:



---#### OpenAI

```json

## Optional: Enable AI Features{

  "ai": {

AI features require the CURL library. Here's how to enable them:    "provider": "openai",

    "model": "gpt-4",

### Option 1: Using vcpkg (Recommended)    "apiKey": "sk-your-openai-key-here",

    "endpoint": "",

1. **Install vcpkg** (if not already installed)    "temperature": 0.7,

```bash    "maxTokens": 500

git clone https://github.com/Microsoft/vcpkg.git  }

cd vcpkg}

.\bootstrap-vcpkg.bat```

.\vcpkg integrate install

```Get API key: [platform.openai.com](https://platform.openai.com/api-keys)



2. **Install CURL**#### Groq (Fast, Free Tier Available)

```bash```json

vcpkg install curl:x64-windows{

```  "ai": {

    "provider": "groq",

3. **Rebuild with CURL enabled**    "model": "llama3-70b-8192",

```bash    "apiKey": "gsk_your-groq-key-here",

cd neuroshell-cli\build    "endpoint": "",

cmake -DENABLE_CURL=ON -DCMAKE_TOOLCHAIN_FILE=path\to\vcpkg\scripts\buildsystems\vcpkg.cmake ..    "temperature": 0.7,

cmake --build . --config Release    "maxTokens": 500

```  }

}

### Option 2: Manual CURL Installation```



1. Download CURL from: https://curl.se/download.htmlGet API key: [console.groq.com](https://console.groq.com/keys)

2. Extract to `C:\curl`

3. Add to CMake:#### Google Gemini

```bash```json

cmake -DENABLE_CURL=ON -DCURL_ROOT=C:\curl ..{

```  "ai": {

    "provider": "gemini",

---    "model": "gemini-pro",

    "apiKey": "your-gemini-key-here",

## Configuration    "endpoint": "",

    "temperature": 0.7,

### AI Configuration    "maxTokens": 500

  }

Edit `config/settings.json`:}

```

```json

{Get API key: [makersuite.google.com](https://makersuite.google.com/app/apikey)

  "ai": {

    "provider": "openai",#### Ollama (Local, No API Key)

    "model": "gpt-4",```json

    "apiKey": "YOUR_API_KEY_HERE",{

    "temperature": 0.7,  "ai": {

    "maxTokens": 500    "provider": "ollama",

  }    "model": "llama2",

}    "apiKey": "",

```    "endpoint": "http://localhost:11434/api/generate",

    "temperature": 0.7,

### Supported Providers    "maxTokens": 500

  }

| Provider | Models | API Key Source |}

|----------|--------|----------------|```

| **OpenAI** | gpt-4, gpt-3.5-turbo | https://platform.openai.com/api-keys |

| **Groq** | llama3-70b, mixtral-8x7b | https://console.groq.com/keys |Install Ollama:

| **Gemini** | gemini-pro | https://makersuite.google.com/app/apikey |1. Download from [ollama.ai](https://ollama.ai)

| **Ollama** | llama2, codellama | No API key (local) |2. Run: `ollama pull llama2`

3. Start: `ollama serve`

### Ollama Setup (Local AI)

## Running

1. Download: https://ollama.ai

2. Install Ollama### First Launch

3. Pull a model: `ollama pull llama2`

4. Start server: `ollama serve````cmd

5. Configure settings.json:cd build\bin\Release

```jsonneuroshell.exe

{```

  "ai": {

    "provider": "ollama",Or use the shortcut:

    "model": "llama2",```cmd

    "apiKey": "",run.bat

    "endpoint": "http://localhost:11434/api/generate"```

  }

}### What to Expect

```

1. Window opens with NeuroShell UI

---2. GPU-rendered interface (ImGui)

3. Command input at bottom

## Troubleshooting4. AI panel on right side

5. Terminal history in center

### Build Issues

### Test Commands

#### "ImGui not found"

```bashTry these in the command input:

cd thirdparty

git clone -b docking https://github.com/ocornut/imgui.git```

```# Regular commands

dir

#### "GLFW not found"whoami

```bashipconfig

cd thirdparty

git clone https://github.com/glfw/glfw.git# AI-powered (if configured)

```Type in AI panel: "create a folder called test"

→ Generates: mkdir test

#### "CMake configuration failed"

- Ensure Visual Studio 2022 is installedType: "show me all text files"

- Ensure CMake is in PATH: `cmake --version`→ Generates: dir *.txt /s

- Check that all dependencies are in `thirdparty/````



### Runtime Issues## Troubleshooting



#### "Failed to initialize NeuroShell UI"### Build Errors

- Update graphics drivers

- Verify OpenGL 3.3+ support**Error: Cannot find GLFW**

- Try running as administrator```

Solution: Install via vcpkg or add to CMAKE_PREFIX_PATH

#### "AI Not Configured"```

- Add API key to `config/settings.json`

- Rebuild with CURL enabled: `cmake -DENABLE_CURL=ON ..`**Error: CURL not found**

```powershell

#### Window appears but is blankvcpkg install curl:x64-windows

- Check if antivirus is blocking```

- Verify graphics drivers are up to date

- Try running in compatibility mode**Error: ImGui headers missing**

```bash

---cd thirdparty

git clone -b docking https://github.com/ocornut/imgui.git

## Next Steps```



Once NeuroShell is running:### Runtime Errors



1. Test basic commands:**Window doesn't appear**

   ```- Check if graphics drivers are updated

   dir- Try running as administrator

   whoami- Check Event Viewer for OpenGL errors

   ipconfig

   ```**AI not working**

- Verify API key in `config/settings.json`

2. Try AI-powered commands (if configured):- Check internet connection

   - Type in AI panel: "create a folder called test"- For Ollama: ensure `ollama serve` is running

   - Type: "show me all files"

   - Type: "what's my IP address"**DLL not found**

- Copy missing DLLs from vcpkg: `vcpkg/installed/x64-windows/bin/`

3. Customize theme in `config/settings.json`- Or rebuild with static linking: `cmake -DCURL_STATICLIB=ON ..`



4. Explore keyboard shortcuts (Ctrl+J, Ctrl+L, Ctrl+K)## Packaging for Distribution



---```powershell

# After successful build

## Getting Helpcd build\bin\Release



- **Build Issues**: Check the troubleshooting section above# Create package directory

- **GitHub Issues**: https://github.com/yourusername/neuroshell-cli/issuesmkdir NeuroShell-v2.0-Windows

- **Documentation**: See README.md for feature detailscd NeuroShell-v2.0-Windows



---# Copy files

copy ..\neuroshell.exe .

**Happy coding! 🚀**copy ..\*.dll .

xcopy /E /I ..\..\..\config config
xcopy /E /I ..\..\..\assets assets

# Create README
echo NeuroShell v2.0 - AI-Powered Terminal > README.txt
echo. >> README.txt
echo Run neuroshell.exe to start. >> README.txt
echo Configure AI in config\settings.json >> README.txt

# Zip for distribution
powershell Compress-Archive -Path * -DestinationPath ..\NeuroShell-v2.0-Windows.zip
```

## Performance Optimization

### Release Build
```cmake
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

### Static Linking (Single EXE)
```cmake
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
```

### Reduce Binary Size
```cmake
add_link_options(/OPT:REF /OPT:ICF)
```

## Next Steps

1. ✅ Build successful
2. ⚙️ Configure AI provider
3. 🎨 Customize theme
4. 🚀 Start using natural language commands!

## Getting Help

- **Documentation**: See `README_NEW.md`
- **Issues**: https://github.com/Sid0004/neuroshell-ai-cli/issues
- **Examples**: See `config/settings.json` for configuration examples

---

**Happy building! 🚀**
