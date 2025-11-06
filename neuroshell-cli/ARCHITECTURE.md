# NeuroShell Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         USER INPUT                               │
│              "list all files in this directory"                  │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────────┐
│                     NLP PARSER                                   │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ 1. Tokenize: ["list", "all", "files", "in", ...]         │  │
│  │ 2. Extract Action: "list"                                 │  │
│  │ 3. Extract Target: "file"                                 │  │
│  │ 4. Extract Parameters: {"recursive": "true"}              │  │
│  │ 5. Calculate Confidence: 0.85                             │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ▼
                  ParsedIntent
              ┌─────────────────┐
              │ action: "list"  │
              │ target: "file"  │
              │ params: {...}   │
              │ confidence: 0.85│
              └────────┬────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                   COMMAND MAPPER                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ 1. Create Key: "list:file"                                │  │
│  │ 2. Lookup Template: command_templates_["list:file"]       │  │
│  │ 3. Detect Platform: Windows/Linux/macOS                   │  │
│  │ 4. Get Platform Command: "dir" or "ls -la"                │  │
│  │ 5. Replace Parameters: Apply {name}, {path}, etc.         │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ▼
                  MappedCommand
           ┌──────────────────────────┐
           │ command: "dir"            │
           │ args: []                  │
           │ requires_confirmation: no │
           │ is_dangerous: no          │
           └────────┬─────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                   SAFETY CHECKER                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ 1. Check Blacklist: Is "dir" blacklisted? NO              │  │
│  │ 2. Check Injection: Any dangerous patterns? NO            │  │
│  │ 3. Check Whitelist: Is "dir" whitelisted? YES             │  │
│  │ 4. Validation Result: SAFE ✓                              │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────────┐
│                  COMMAND EXECUTOR                                │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ 1. Build Command String: "dir"                            │  │
│  │ 2. Sanitize Input: Remove dangerous chars                 │  │
│  │ 3. Check Confirmation: Required? NO                        │  │
│  │ 4. Execute: popen("dir")                                   │  │
│  │ 5. Capture Output: Read stdout/stderr                     │  │
│  │ 6. Measure Time: Track execution duration                 │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ▼
                  ExecutionResult
           ┌──────────────────────────┐
           │ success: true             │
           │ exit_code: 0              │
           │ output: "file1.txt..."    │
           │ error: ""                 │
           │ execution_time: 0.15s     │
           └────────┬─────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                       OUTPUT TO USER                             │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ ✅ Command executed successfully!                         │  │
│  │                                                            │  │
│  │ Output:                                                    │  │
│  │ file1.txt                                                  │  │
│  │ file2.txt                                                  │  │
│  │ folder1/                                                   │  │
│  │                                                            │  │
│  │ Execution time: 0.15s                                      │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

## Component Responsibilities

### 1. NLP Parser (`nlp/parser.cpp`)
**Converts natural language to structured data**
- Tokenization: Breaks input into words
- POS Tagging: Identifies parts of speech (basic)
- Action Extraction: Finds verbs (list, create, delete)
- Target Extraction: Finds nouns (file, folder, process)
- Parameter Extraction: Finds modifiers (all, recursive)
- Confidence Scoring: Estimates accuracy

### 2. Command Mapper (`nlp/command_mapper.cpp`)
**Maps intent to platform-specific commands**
- Template Matching: Finds matching command pattern
- Platform Detection: Windows, Linux, or macOS
- Parameter Substitution: Replaces {name}, {path}, etc.
- Cross-platform Support: Different commands per OS
- Danger Flagging: Marks risky operations

### 3. Safety Checker (`utils/safety.cpp`)
**Prevents dangerous operations**
- Whitelist: Pre-approved safe commands
- Blacklist: Forbidden commands (format, fdisk)
- Pattern Matching: Detects risky patterns (rm -rf)
- Injection Detection: Prevents command injection
- Validation: Final safety gate

### 4. Command Executor (`executor/command_executor.cpp`)
**Safely executes system commands**
- Command Building: Assembles full command string
- Input Sanitization: Removes dangerous characters
- Confirmation Prompts: Asks user for dangerous ops
- Process Execution: Runs command via popen
- Output Capture: Collects stdout/stderr
- Timing: Measures execution duration

### 5. Logger (`utils/logger.cpp`)
**Tracks all operations**
- File Logging: Writes to neuroshell.log
- Console Output: Optional terminal logging
- Log Levels: DEBUG, INFO, WARNING, ERROR, CRITICAL
- Thread Safety: Mutex-protected writes

## Data Flow Example: Dangerous Command

```
Input: "delete all files"
    ↓
Parser: action="delete", target="file", params={"recursive":"true"}
    ↓
Mapper: command="del /s /q *.*" (Windows) / "rm -rf *" (Linux)
        requires_confirmation=TRUE, is_dangerous=TRUE
    ↓
Safety: PASSES (whitelisted but flagged dangerous)
    ↓
Executor: Shows confirmation prompt:
         "⚠️  About to execute: del /s /q *.*"
         "Continue? (y/n):"
    ↓
User: Enters 'n'
    ↓
Result: "User cancelled operation" (not executed)
```

## Extension Points

### Adding New Commands
1. Add action/target mappings in `parser.cpp`
2. Add command template in `command_mapper.cpp`
3. Test with various phrasings

### Adding API Integration
1. Create `api_client.cpp` in `src/ai/`
2. Implement OpenAI/Anthropic API calls
3. Use for complex queries with low confidence
4. Fallback to pattern matching

### Adding Learning System
1. Store user corrections in database
2. Build custom mappings over time
3. Improve confidence scoring
4. Suggest commonly used patterns
```
