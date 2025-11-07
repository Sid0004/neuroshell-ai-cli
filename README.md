<img width="824" height="159" alt="image" src="https://github.com/user-attachments/assets/69609f23-c476-4c1c-a4b7-5f4d7b29e8f8" />
<img width="1411" height="248" alt="image" src="https://github.com/user-attachments/assets/988f8c7a-f5be-47ef-a4c6-e7117b9318d6" />

# NeuroShell AI CLI

🤖 neuroshell>

**Natural Language Command Line Interface** - Execute CLI commands using natural language!

## 🚀 Features

- **Natural Language Processing**: Write commands in plain English
- **Smart Command Translation**: Converts NLP to actual CLI commands
- **Cross-Platform Support**: Works on Windows, Linux, and macOS
- **Safety Mechanisms**: Prevents dangerous command execution
- **Learning System**: Improves with usage
- **Command History**: Tracks your natural language commands
- **Interactive Mode**: Chat-like interface for executing commands

## 📋 Examples

Instead of typing:
```bash
dir /s /b *.txt
```

Just say:
```
find all text files in this directory
```

More examples:
- "show me all running processes" → `tasklist` (Windows) / `ps aux` (Linux)
- "create a new folder called projects" → `mkdir projects`
- "what's my ip address" → `ipconfig` (Windows) / `ifconfig` (Linux)
- "delete file test.txt" → `del test.txt` (with confirmation)
- "compress this folder into archive.zip" → `tar -czf archive.zip folder/`

## 🛠️ Installation

### Prerequisites
- C++17 or higher
- CMake 3.15+
- cURL library (for API integration)
- JSON library (nlohmann/json)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/neuroshell-cli.git
cd neuroshell-cli

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build .

# Run
./neuroshell
```

## 🎯 Usage

### Interactive Mode
```bash
./neuroshell
```

### Single Command Mode
```bash
./neuroshell "list all files"
```

### With API Integration (Advanced NLP)
```bash
./neuroshell --api-key YOUR_OPENAI_KEY "complex command description"
```

## 🏗️ Architecture

```
neuroshell-cli/
├── src/
│   ├── main.cpp                 # Entry point
│   ├── nlp/
│   │   ├── parser.cpp          # NLP parsing logic
│   │   └── command_mapper.cpp  # Maps NLP to CLI commands
│   ├── executor/
│   │   └── command_executor.cpp # Executes commands safely
│   ├── ai/
│   │   └── api_client.cpp      # OpenAI API integration
│   └── utils/
│       ├── safety.cpp          # Safety checks
│       └── logger.cpp          # Logging system
├── include/                     # Header files
├── tests/                       # Unit tests
└── config/
    └── commands.json           # Command mappings
```

## 🔒 Safety Features

- Whitelist of safe commands
- Confirmation prompts for destructive operations
- Sudo/Admin command blocking by default
- Command sandboxing options

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 📄 License

MIT License - See LICENSE file for details

## 🌟 Roadmap

- [ ] Multi-language support
- [ ] Voice command integration
- [ ] Custom command aliases
- [ ] Plugin system
- [ ] GUI version
- [ ] Cloud sync for learned commands

## 📧 Contact

For questions or suggestions, please open an issue on GitHub.
