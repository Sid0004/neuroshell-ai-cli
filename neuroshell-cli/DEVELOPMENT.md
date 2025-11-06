# NeuroShell Development Guide

## Project Structure

```
neuroshell-cli/
├── src/                        # Source files
│   ├── main.cpp               # Application entry point
│   ├── nlp/                   # Natural Language Processing
│   │   ├── parser.cpp         # Parse NL input
│   │   └── command_mapper.cpp # Map to CLI commands
│   ├── executor/              # Command execution
│   │   └── command_executor.cpp
│   └── utils/                 # Utilities
│       ├── safety.cpp         # Safety checks
│       ├── logger.cpp         # Logging
│       └── config_loader.cpp  # Configuration
├── include/                    # Header files
├── config/                     # Configuration files
├── tests/                      # Unit tests
└── docs/                       # Documentation
```

## Architecture

### 1. NLP Parser (`nlp/parser.h`)
Converts natural language to structured intent:
- **Tokenization**: Breaks input into words
- **Action Extraction**: Identifies verbs (list, create, delete)
- **Target Extraction**: Identifies objects (file, folder, process)
- **Parameter Extraction**: Captures additional info (names, paths)

### 2. Command Mapper (`nlp/command_mapper.h`)
Maps structured intent to platform-specific CLI commands:
- Cross-platform support (Windows, Linux, macOS)
- Template-based command generation
- Parameter substitution

### 3. Command Executor (`executor/command_executor.h`)
Safely executes commands:
- Safety validation
- User confirmation for dangerous operations
- Output capture
- Error handling
- Dry-run mode

### 4. Safety Checker (`utils/safety.h`)
Prevents dangerous operations:
- Whitelisting safe commands
- Blacklisting dangerous commands
- Pattern matching for risky operations
- Command injection prevention

## Adding New Commands

### Step 1: Add Action/Target Mappings
In `src/nlp/parser.cpp`, add to `initializeActionMappings()` or `initializeTargetMappings()`:

```cpp
action_mappings_["compress"] = "compress";
target_mappings_["archive"] = "archive";
```

### Step 2: Add Command Template
In `src/nlp/command_mapper.cpp`, add to `initializeDefaultMappings()`:

```cpp
command_templates_["compress:archive"] = {
    "tar -czf {output} {input}",  // Windows (if tar installed)
    "tar -czf {output} {input}",  // Linux
    "tar -czf {output} {input}",  // macOS
    false, false, {"input", "output"}
};
```

### Step 3: Test
```bash
./neuroshell "compress folder into archive.tar.gz"
```

## Code Style

- **Naming**: Use `snake_case` for variables, `PascalCase` for classes
- **Comments**: Document public APIs with Doxygen-style comments
- **Includes**: Group by system, external, internal
- **Namespaces**: Always use `neuroshell` namespace

## Testing

Run tests with:
```bash
cd build
ctest --output-on-failure
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

## Future Enhancements

### Short Term:
- [ ] Add more command patterns
- [ ] Improve confidence scoring
- [ ] Add command history
- [ ] Support for pipes and redirection

### Medium Term:
- [ ] OpenAI API integration for complex queries
- [ ] Learning from user corrections
- [ ] Command suggestions
- [ ] Alias system

### Long Term:
- [ ] Voice input
- [ ] GUI version
- [ ] Plugin system
- [ ] Cloud sync
- [ ] Multi-language support

## API Integration (Advanced)

To enable OpenAI API for better NLP:

1. Build with API support:
```bash
cmake .. -DENABLE_API=ON
```

2. Set API key:
```bash
export OPENAI_API_KEY="your-key-here"
./neuroshell "complex natural language command"
```

## Performance Tips

- Parser is optimized for common patterns
- Caching frequently used command mappings
- Minimal regex usage for speed
- Lazy loading of configuration

## Security Notes

- Never disable safety checks in production
- Always validate user input
- Use whitelisting over blacklisting
- Regularly update dangerous patterns list
- Log all command executions

## Debugging

Enable verbose logging:
```cpp
utils::Logger::getInstance().setLevel(LogLevel::DEBUG);
```

Or use verbose mode:
```bash
./neuroshell
> verbose  # Toggle verbose mode
```

## License

MIT License - See LICENSE file
