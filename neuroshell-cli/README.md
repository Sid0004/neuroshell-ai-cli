

# 🧠 NeuroShell

**A Modern, GPU-Rendered Terminal with AI-Powered Command Translation**

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++](https://img.shields.io/badge/C++-17-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)

---

## ✨ Features

- 🎨 **Modern GPU-Rendered UI** - Built with Dear ImGui + OpenGL for smooth, beautiful interface
- 🤖 **AI-Powered Commands** - Translate natural language to shell commands using OpenAI, Groq, Gemini, or Ollama
- 📦 **Command Blocks** - Warp-style command history with clear visual separation
- 🎯 **Native Desktop App** - Standalone executable, no browser required
- 🌈 **Beautiful Themes** - Tokyo Night theme with customizable colors
- ⚡ **Fast & Responsive** - Hardware-accelerated rendering

---

## 🚀 Quick Start

### Prerequisites

- **Windows 10/11**
- **Visual Studio 2022** with C++ Desktop Development
- **CMake 3.15+**
- **Git**

### Build Instructions

1. **Clone the repository**
```bash
git clone https://github.com/yourusername/neuroshell-cli.git
cd neuroshell-cli
```

2. **Setup dependencies**

The project requires the following libraries in the `thirdparty` directory:
- **ImGui** (docking branch)
- **GLFW**
- **GLAD**
- **nlohmann/json**

```bash
# Clone ImGui (docking branch)
cd thirdparty
git clone -b docking https://github.com/ocornut/imgui.git

# Clone GLFW
git clone https://github.com/glfw/glfw.git

# Clone nlohmann/json
git clone https://github.com/nlohmann/json.git

# GLAD - Already included in thirdparty/glad/
cd ..
```

3. **Build the project**
```bash
build.bat
```

4. **Run NeuroShell**
```bash
run.bat
```

Or manually:
```bash
cd build\bin\Release
neuroshell.exe
```

---

## ⚙️ Configuration

### AI Setup (Optional)

To enable AI-powered command translation, edit `config/settings.json`:

```json
{
  "ai": {
    "provider": "openai",
    "model": "gpt-4",
    "apiKey": "YOUR_API_KEY_HERE",
    "temperature": 0.7,
    "maxTokens": 500
  }
}
```

**Supported AI Providers:**
- **OpenAI** (`gpt-4`, `gpt-3.5-turbo`)
- **Groq** (`llama3-70b`, `mixtral-8x7b`)
- **Gemini** (`gemini-pro`)
- **Ollama** (`llama2`, `codellama`) - Local, no API key needed

### Enabling CURL for AI Features

AI features require CURL library. To enable:

1. Install CURL via vcpkg:
```bash
vcpkg install curl:x64-windows
```

2. Rebuild with CURL enabled:
```bash
cd build
cmake -DENABLE_CURL=ON -DCMAKE_TOOLCHAIN_FILE=path\to\vcpkg\scripts\buildsystems\vcpkg.cmake ..
cmake --build . --config Release
```

---

## ⌨️ Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+L` | Clear screen |
| `Ctrl+Shift+C` | Clear history |
| `Ctrl+J` | Toggle AI panel |
| `Ctrl+K` | Focus command input |
| `↑/↓` | Navigate command history |

---

## 🎨 Project Structure

```
NeuroShell/
├── src/
│   ├── main.cpp              # Application entry point
│   ├── ui/                   # UI implementation (ImGui)
│   ├── terminal/             # Terminal logic and command executor
│   ├── ai/                   # AI client (OpenAI, Groq, etc.)
│   └── utils/                # Utilities (config, logger, safety)
├── include/                  # Header files
├── thirdparty/               # Third-party libraries
│   ├── imgui/
│   ├── glfw/
│   ├── glad/
│   └── json/
├── config/                   # Configuration files
│   └── settings.json
├── build.bat                 # Build script
├── run.bat                   # Run script
└── CMakeLists.txt           # CMake configuration
```

---

## 🛠️ Development

### Building for Development

```bash
mkdir build && cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Debug
```

### Adding Features

1. UI components: Modify `src/ui/ui.cpp`
2. Terminal logic: Modify `src/terminal/terminal.cpp`
3. Command execution: Modify `src/terminal/command_executor.cpp`
4. AI integration: Modify `src/ai/ai_client.cpp`

---

## 🐛 Troubleshooting

### Build Errors

**Missing Dependencies**
```
ERROR: ImGui not found
Fix: cd thirdparty && git clone -b docking https://github.com/ocornut/imgui.git
```

**CMake Configuration Failed**
- Ensure Visual Studio 2022 is installed with C++ Desktop Development
- Ensure CMake is in your PATH
- Check that all dependencies are in `thirdparty/`

### Runtime Errors

**Window doesn't appear**
- Update graphics drivers
- Ensure OpenGL 3.3+ is supported
- Run as administrator

**AI not working**
- Verify API key in `config/settings.json`
- Check internet connection
- For Ollama: ensure `ollama serve` is running
- Rebuild with CURL enabled

---

## 📄 License

MIT License - see [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate mode GUI library
- [GLFW](https://www.glfw.org/) - Window and input handling
- [GLAD](https://glad.dav1d.de/) - OpenGL loader
- [nlohmann/json](https://github.com/nlohmann/json) - JSON parsing
- [Warp](https://www.warp.dev/) - UI inspiration
- [Tokyo Night](https://github.com/enkia/tokyo-night-vscode-theme) - Color scheme

---

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/neuroshell-cli/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/neuroshell-cli/discussions)

---

<div align="center">
  <strong>Made with ❤️ for developers who love beautiful terminals</strong>
</div>
