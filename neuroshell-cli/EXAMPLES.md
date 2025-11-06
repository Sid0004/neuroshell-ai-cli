# NeuroShell AI CLI - Examples

## Basic File Operations

### List Files
```
User: list all files
Translation: dir (Windows) / ls -la (Linux)
```

```
User: show me text files
Translation: dir *.txt (Windows) / ls *.txt (Linux)
```

```
User: find all PDF documents
Translation: dir /s *.pdf (Windows) / find . -name "*.pdf" (Linux)
```

### Create Files/Folders
```
User: create a new folder called projects
Translation: mkdir projects
```

```
User: make a directory named workspace
Translation: mkdir workspace
```

### Delete Operations
```
User: delete file test.txt
Translation: del test.txt (Windows) / rm test.txt (Linux)
⚠️ Requires confirmation
```

```
User: remove folder old_data
Translation: rmdir /s old_data (Windows) / rm -rf old_data (Linux)
⚠️ Requires confirmation - DANGEROUS
```

### Copy/Move
```
User: copy report.doc to backup folder
Translation: copy report.doc backup\ (Windows) / cp report.doc backup/ (Linux)
```

```
User: move image.jpg to pictures folder
Translation: move image.jpg pictures\ (Windows) / mv image.jpg pictures/ (Linux)
```

## Process Management

### List Processes
```
User: show me all running processes
Translation: tasklist (Windows) / ps aux (Linux)
```

```
User: list running applications
Translation: tasklist (Windows) / ps aux (Linux)
```

### Stop Processes
```
User: kill process notepad
Translation: taskkill /F /IM notepad.exe (Windows) / killall notepad (Linux)
⚠️ Requires confirmation
```

## Network Operations

### Network Information
```
User: what's my ip address
Translation: ipconfig (Windows) / ifconfig (Linux)
```

```
User: show network configuration
Translation: ipconfig /all (Windows) / ifconfig -a (Linux)
```

### Connectivity Testing
```
User: ping google.com
Translation: ping google.com
```

```
User: check connection to server
Translation: ping {server}
```

## System Information

### System Details
```
User: show system information
Translation: systeminfo (Windows) / uname -a (Linux)
```

```
User: what computer is this
Translation: systeminfo (Windows) / uname -a (Linux)
```

### Disk Space
```
User: how much disk space do I have
Translation: wmic logicaldisk get size,freespace (Windows) / df -h (Linux)
```

### Current Directory
```
User: where am i
Translation: cd (Windows) / pwd (Linux)
```

```
User: what's the current directory
Translation: cd (Windows) / pwd (Linux)
```

## Advanced Examples

### Multiple Conditions
```
User: find all log files modified today
Note: This requires more advanced parsing or API integration
```

### Piping (Future Enhancement)
```
User: list processes and filter by chrome
Note: Would translate to: tasklist | findstr chrome
```

### Complex Operations
```
User: create backup folder and copy all documents there
Note: Would execute multiple commands sequentially
```

## Tips for Best Results

1. **Be Specific**: Instead of "delete it", say "delete file test.txt"
2. **Use Common Words**: "show", "list", "create", "delete", "copy", "move"
3. **Include Names**: Always specify the file/folder name
4. **Check Confidence**: If confidence is low, try rephrasing
5. **Use Verbose Mode**: To see how commands are translated

## Safety Features

### Automatic Confirmation for:
- File/folder deletion
- Process termination
- System modifications
- Recursive operations

### Blocked Operations:
- Format disk
- Delete system folders
- Fork bombs
- Command injection attempts

## Error Messages

```
❌ Unable to understand command
→ Try rephrasing or type 'help'

⚠️ Low confidence (25%)
→ Command might not be accurate

🔒 Command blocked by safety checker
→ Operation too dangerous to auto-execute
```

## Interactive Mode Features

### Special Commands
- `help` - Show help
- `exit` / `quit` - Exit program
- `clear` / `cls` - Clear screen
- `verbose` - Toggle verbose mode
- `dryrun` - Toggle dry run (shows command without executing)

### Dry Run Mode
Test commands safely:
```
> dryrun
Dry run mode: ON

> delete all temporary files
[DRY RUN] Would execute: del /s /q *.tmp
```

## Integration Examples

### Use in Scripts (Windows)
```batch
@echo off
neuroshell.exe "list all files" > output.txt
```

### Use in Scripts (Linux)
```bash
#!/bin/bash
./neuroshell "show disk space" | grep "/"
```

### Chaining Commands
```bash
neuroshell "list processes" && neuroshell "show disk space"
```
