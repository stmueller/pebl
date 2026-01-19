# PEBL Launcher Requirements & Design Notes

## Current Status
✅ **Phase 1 Complete** - Basic launcher prototype working on Linux

## Outstanding Requirements

### 1. Recent Experiments Panel (Left Side)
**Purpose**: Quick access to recently run experiments

**Design**:
- Add a "Recent" section at top of left file panel
- Show last 10-15 experiments run
- Display: experiment name, last run timestamp
- Click to select and populate details
- Store in launcher.cfg

**Implementation**:
```cpp
struct RecentExperiment {
    std::string path;
    std::string name;
    time_t lastRun;
};

// In LauncherConfig:
std::vector<RecentExperiment> mRecentExperiments;

// Add to recent list on experiment launch
void AddRecentExperiment(const std::string& path, const std::string& name);
```

---

### 2. Launch Logging System
**Purpose**: Record all experiment launches for debugging and audit trail

**Requirements**:
- Log file location: Same as launcher.cfg (PEBL installation directory)
- Filename: `launcher-log.txt` or `launcher-log.csv`
- Log on each experiment launch:
  - Timestamp
  - Experiment path
  - Subject code
  - Language
  - Parameters used
  - Exit status / errors

**Format** (CSV):
```csv
timestamp,experiment_path,subject_code,language,fullscreen,exit_status,duration_seconds
2026-01-09 03:00:15,/path/to/ANT.pbl,test001,en,true,0,45.2
```

**Implementation**:
```cpp
class LaunchLogger {
public:
    void LogLaunch(const std::string& expPath,
                   const std::string& subCode,
                   const std::string& language,
                   bool fullscreen);
    void LogCompletion(int exitCode, double duration);
private:
    std::string mLogFilePath;
    std::ofstream mLogFile;
};
```

---

### 3. Windows Installation Modes

#### 3A. Installed Mode (C:\Program Files)
**Detection**:
- Check if running from `C:\Program Files (x86)\PEBL\` or `C:\Program Files\PEBL\`
- Look for `pebl2.exe` in same directory as launcher
- Default experiment directory: `Documents\pebl-exp.X.X\battery\`

**PEBL Executable Path**:
```cpp
// Windows installed mode
std::string peblPath = "C:\\Program Files (x86)\\PEBL\\bin\\pebl2.exe";
// Or: get launcher path, replace launcher.exe with pebl2.exe
```

#### 3B. Portable/Standalone Mode (USB Drive)
**Detection**:
- Running from any non-Program Files location
- Check for `pebl2.exe` in same directory
- All files stored relative to launcher location

**Directory Structure** (Portable):
```
USBDrive:\PEBL\
├── pebl-launcher.exe
├── pebl2.exe
├── battery\              (experiments)
├── data\                 (collected data)
├── config\
│   └── launcher.cfg
└── logs\
    └── launcher-log.txt
```

**Auto-detection Logic**:
```cpp
std::string DetectInstallMode() {
    std::string exePath = GetExecutablePath();

    // Check if in Program Files
    if (exePath.find("Program Files") != std::string::npos) {
        return "installed";
    }

    // Check if pebl2.exe is in same directory
    if (FileExists(SameDirectory(exePath, "pebl2.exe"))) {
        return "portable";
    }

    return "unknown";
}
```

---

### 4. Upload Configuration System

#### Overview
Users need to easily configure experiments to upload data to peblhub.online after completion.

#### Requirements:
1. **Token Management**: Store PEBL Hub authentication tokens securely
2. **Per-Experiment Upload Settings**: Each experiment can have upload enabled/disabled
3. **Upload Target**: Specify which data files to upload
4. **Automatic Upload**: Optionally upload immediately after experiment completion

#### Design Approach 1: Manual Configuration
**UI**:
- "Enable Upload" checkbox in experiment details
- "Configure Upload" button opens dialog
- Dialog fields:
  - PEBL Hub token (masked password field)
  - Upload URL (default: https://peblhub.online/api/upload)
  - Data file patterns (e.g., "data/*.csv")
  - Auto-upload on completion checkbox

**Storage** (in launcher.cfg or separate upload.cfg):
```ini
[upload]
enabled=true
token=abc123...
url=https://peblhub.online/api/upload
auto_upload=true

[experiment_uploads]
ANT=enabled
corsi=enabled
stroop=disabled
```

#### Design Approach 2: Package Download from Hub (Preferred)
**Workflow**:
1. User goes to peblhub.online and creates a study
2. Hub generates a "launcher configuration package" (JSON file)
3. User downloads package: `study-12345-config.json`
4. Launcher has "Import Hub Configuration" menu option
5. Imports configuration with all upload settings pre-configured

**Package Format** (JSON):
```json
{
  "study_id": "12345",
  "study_name": "Memory Battery Study",
  "hub_url": "https://peblhub.online",
  "upload_token": "abc123...",
  "experiments": [
    {
      "name": "ANT",
      "upload_enabled": true,
      "data_files": ["data/*-summary.csv"]
    },
    {
      "name": "corsi",
      "upload_enabled": true,
      "data_files": ["data/*-pooled.csv"]
    }
  ],
  "auto_upload": true,
  "require_subject_code": true
}
```

**Benefits**:
- User doesn't need to manually configure each experiment
- Hub can push updates to configuration
- Centralized management
- More secure (token generated on server)

**Implementation**:
```cpp
class UploadConfig {
public:
    bool ImportFromHubPackage(const std::string& jsonPath);
    bool IsUploadEnabled(const std::string& experimentName);
    bool UploadData(const std::string& experimentPath,
                    const std::string& subjectCode);

private:
    std::string mHubUrl;
    std::string mUploadToken;
    std::string mStudyId;
    std::map<std::string, ExperimentUploadConfig> mExperiments;
};
```

---

### 5. Cross-Platform Executable Paths

**Linux**:
```
/usr/local/bin/pebl2                    (installed)
./pebl2                                  (portable - same directory)
~/Documents/pebl-exp.2.2/battery/       (experiments)
```

**Windows Installed**:
```
C:\Program Files (x86)\PEBL\bin\pebl-launcher.exe
C:\Program Files (x86)\PEBL\bin\pebl2.exe
%USERPROFILE%\Documents\pebl-exp.2.2\battery\
```

**Windows Portable**:
```
E:\PEBL\pebl-launcher.exe               (USB drive)
E:\PEBL\pebl2.exe
E:\PEBL\battery\                        (experiments)
E:\PEBL\data\                           (data output)
```

**macOS**:
```
/Applications/PEBL.app/Contents/MacOS/pebl2
~/Documents/pebl-exp.2.2/battery/
```

---

## Implementation Priority

### Phase 2A (Essential Features)
1. ✅ Native file browser dialog
2. ✅ Process monitoring with output display
3. ✅ Recent experiments panel
4. ✅ Launch logging

### Phase 2B (Cross-Platform Support)
5. ✅ Windows installed mode detection
6. ✅ Windows portable mode support
7. ✅ Configuration file compatibility

### Phase 2C (Advanced Features)
8. ✅ Parameter editor
9. ✅ Upload configuration system
10. ✅ Hub package import
11. ✅ Experiment chains

### Phase 3 (Polish)
12. Error handling and user feedback
13. Keyboard shortcuts
14. Help documentation
15. About dialog with version info
16. Themes/styling options

---

## Testing Checklist

### Linux
- [ ] Installed mode (/usr/local/bin)
- [ ] Portable mode (./pebl2 in same directory)
- [ ] Auto-detect Documents/pebl-exp.2.2
- [ ] Launch experiments with various parameters
- [ ] Screenshot display for all battery tests

### Windows
- [ ] Installed mode (C:\Program Files)
- [ ] Portable mode (USB drive)
- [ ] Auto-detect Documents\pebl-exp.2.2
- [ ] Process spawning works correctly
- [ ] File paths with spaces handled

### macOS
- [ ] .app bundle structure
- [ ] Documents directory detection
- [ ] Experiment launching

---

## Notes

- Keep backwards compatibility with old launcher.cfg format
- Upload functionality should gracefully handle network failures
- All file operations need proper error handling
- Consider adding a "safe mode" that runs without uploading
- Log file should rotate or have size limit to prevent growth
- Recent experiments list should persist across launcher restarts
