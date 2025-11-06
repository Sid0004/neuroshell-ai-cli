# Contributing to NeuroShell AI CLI

Thank you for your interest in contributing! 🎉

## Good First Issues

Looking for something to work on? Check out these beginner-friendly tasks:

### 🟢 Easy (Good First Issue)

#### 1. Add More Command Patterns
**Issue:** Expand NLP vocabulary and command mappings
- **Files to modify:** `src/nlp/parser.cpp`, `src/nlp/command_mapper.cpp`
- **What to do:** 
  - Add new action verbs (e.g., "compress", "extract", "search")
  - Add new target objects (e.g., "archive", "port", "service")
  - Add command templates for new actions
- **Example:**
  ```cpp
  // In parser.cpp
  action_mappings_["compress"] = "compress";
  
  // In command_mapper.cpp
  command_templates_["compress:folder"] = {
      "tar -czf {name}.tar.gz {name}",  // Linux
      "tar -czf {name}.tar.gz {name}",  // macOS
      false, false, {"name"}
  };
  ```

#### 2. Improve Documentation
**Issue:** Add more examples and improve existing docs
- **Files to modify:** `EXAMPLES.md`, `QUICKSTART.md`, `README.md`
- **What to do:**
  - Add 20+ more real-world usage examples
  - Create a troubleshooting FAQ section
  - Add screenshots/GIFs of the tool in action
  - Improve installation instructions

#### 3. Add More Unit Tests
**Issue:** Increase test coverage
- **Files to modify:** `tests/` directory
- **What to do:**
  - Fix the test suite (currently disabled due to multiple main() functions)
  - Add tests for edge cases
  - Add tests for parameter extraction
  - Test dangerous command detection

#### 4. Improve Error Messages
**Issue:** Make error messages more helpful
- **Files to modify:** `src/nlp/parser.cpp`, `src/executor/command_executor.cpp`
- **What to do:**
  - When confidence is low, suggest alternative phrasings
  - Provide "Did you mean?" suggestions
  - Add helpful hints for common mistakes

### 🟡 Medium

#### 5. Add Command History
**Issue:** Store and recall previously executed commands
- **Files to modify:** Create new `src/utils/history.cpp`
- **What to do:**
  - Save command history to a file
  - Allow browsing history with arrow keys
  - Add `history` command to view past commands
  - Support searching history

#### 6. Implement Confidence Scoring Improvements
**Issue:** Better accuracy detection
- **Files to modify:** `src/nlp/parser.cpp`
- **What to do:**
  - Improve the confidence calculation algorithm
  - Consider multiple factors (action found, target found, parameters)
  - Add machine learning-based scoring (optional)

#### 7. Add Command Suggestions
**Issue:** Auto-suggest commands as user types
- **Files to modify:** `src/main.cpp`, create `src/utils/autocomplete.cpp`
- **What to do:**
  - Implement tab-completion
  - Show possible completions
  - Learn from user's command history

#### 8. Cross-Platform Testing
**Issue:** Test on different operating systems
- **What to do:**
  - Test on Linux distributions (Ubuntu, Fedora, Arch)
  - Test on different Windows versions
  - Test on macOS
  - Document any platform-specific issues
  - Fix platform-specific bugs

### 🔴 Advanced

#### 9. OpenAI API Integration
**Issue:** Add AI-powered command translation
- **Files to modify:** Create `src/ai/api_client.cpp`, update `CMakeLists.txt`
- **What to do:**
  - Integrate OpenAI API for complex queries
  - Use GPT to translate when confidence is low
  - Add API key management
  - Handle API errors gracefully
  - Add tests for API integration

#### 10. Plugin System
**Issue:** Allow users to add custom commands via plugins
- **Files to modify:** Create `src/plugins/` directory, update architecture
- **What to do:**
  - Design plugin API
  - Support loading .so/.dll plugin files
  - Create example plugin
  - Add plugin documentation

#### 11. GUI Application
**Issue:** Create a graphical interface
- **What to do:**
  - Create Electron-based GUI wrapper
  - Or create native GUI with Qt/wxWidgets
  - Visual command history
  - Settings panel
  - Real-time command preview

#### 12. Voice Input Support
**Issue:** Execute commands via voice
- **What to do:**
  - Integrate speech-to-text library
  - Add voice activation
  - Support multiple languages
  - Add voice feedback

## How to Contribute

### 1. Fork the Repository
Click the "Fork" button at the top right of the repository page.

### 2. Clone Your Fork
```bash
git clone https://github.com/YOUR_USERNAME/neuroshell-ai-cli.git
cd neuroshell-ai-cli
```

### 3. Create a Branch
```bash
git checkout -b feature/your-feature-name
```

### 4. Make Your Changes
- Follow the existing code style
- Add comments for complex logic
- Update documentation if needed
- Add tests for new features

### 5. Test Your Changes
```bash
mkdir build && cd build
cmake ..
cmake --build .
./neuroshell  # Test your changes
```

### 6. Commit and Push
```bash
git add .
git commit -m "Add: description of your changes"
git push origin feature/your-feature-name
```

### 7. Create Pull Request
Go to your fork on GitHub and click "New Pull Request"

## Code Style Guidelines

- **C++ Standard:** C++17
- **Naming:**
  - Classes: `PascalCase`
  - Functions/Variables: `snake_case`
  - Constants: `UPPER_CASE`
- **Comments:** Use Doxygen-style comments for public APIs
- **Formatting:** Use consistent indentation (4 spaces)

## Testing

- Test on your platform before submitting
- Add unit tests for new features
- Ensure existing tests pass
- Test edge cases

## Questions?

- Open an issue for questions
- Check existing issues first
- Join discussions

## Code of Conduct

- Be respectful and inclusive
- Provide constructive feedback
- Help others learn

---

**Thank you for contributing to NeuroShell! 🚀**
