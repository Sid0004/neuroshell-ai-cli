# Quick Start Guide

## Installation

### Option 1: Build from Source (Recommended)

#### Windows:
1. Install Visual Studio 2017+ with C++ support
2. Install CMake (https://cmake.org/download/)
3. Open Command Prompt and navigate to the project folder
4. Run: `build.bat`
5. Executable will be in `build\bin\Release\neuroshell.exe`

#### Linux:
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake

# Build
chmod +x build.sh
./build.sh

# Run
./build/bin/neuroshell
```

#### macOS:
```bash
# Install dependencies
brew install cmake

# Build
chmod +x build.sh
./build.sh

# Run
./build/bin/neuroshell
```

### Option 2: Pre-built Binary
Download from releases page (when available)

## First Run

1. **Interactive Mode**:
   ```bash
   ./neuroshell
   ```
   You'll see a prompt: `🤖 neuroshell>`

2. **Try Your First Command**:
   ```
   🤖 neuroshell> list all files
   🔍 Parsing command...
   💡 Translates to: dir (or ls -la on Linux)
   ✅ Command executed successfully!
   ```

3. **Get Help**:
   ```
   🤖 neuroshell> help
   ```

## Common Usage Patterns

### File Management
```
list all files
create a folder called test
delete file example.txt
copy file1.txt to backup folder
```

### System Information
```
what's my ip address
show system information
how much disk space do I have
```

### Process Management
```
show me running processes
kill process notepad
```

## Safety Features

- **Automatic Confirmation**: Dangerous commands require your approval
- **Blacklist**: Extremely dangerous commands are blocked
- **Dry Run Mode**: Test without executing
  ```
  🤖 neuroshell> dryrun
  Dry run mode: ON
  ```

## Tips

1. **Be Specific**: "delete file test.txt" not just "delete test"
2. **Use Natural Language**: Speak normally, don't try to use CLI syntax
3. **Check Translations**: Review what command will execute before confirming
4. **Enable Verbose**: See detailed parsing information
   ```
   🤖 neuroshell> verbose
   ```

## Single Command Mode

Execute one command and exit:
```bash
neuroshell "list all files"
```

Great for scripts!

## Configuration

Edit `config/neuroshell.conf` to customize:
- Log level
- Safety settings
- UI preferences
- API integration (advanced)

## Troubleshooting

### "Unable to understand command"
- Try rephrasing with simpler words
- Check the examples in `EXAMPLES.md`
- Type `help` for supported patterns

### Low Confidence Warning
- Command might not be accurate
- Review the translation before proceeding
- Consider rephrasing for better accuracy

### Build Errors
- Windows: Ensure Visual Studio C++ tools are installed
- Linux: Install g++ and cmake
- Check `BUILD.md` for detailed instructions

## Next Steps

- Read `EXAMPLES.md` for more command examples
- Check `DEVELOPMENT.md` if you want to contribute
- Star the project on GitHub if you find it useful!

## Support

- Report bugs: Create an issue on GitHub
- Feature requests: Open a discussion
- Questions: Check existing issues first

## Quick Reference

| Natural Language | Windows Command | Linux Command |
|-----------------|----------------|---------------|
| list all files | dir | ls -la |
| create folder X | mkdir X | mkdir X |
| delete file X | del X | rm X |
| copy X to Y | copy X Y | cp X Y |
| show processes | tasklist | ps aux |
| my ip address | ipconfig | ifconfig |

Happy commanding! 🚀
