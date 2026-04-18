# PEBL Launcher (Dear ImGui Prototype)

A modern, native C++ launcher for PEBL experiments using SDL2 and Dear ImGui.

## Current Status

**Prototype** - Basic functionality implemented and working.

## Features Implemented

### Core Functionality
- ✅ Auto-detection of PEBL installation (searches Documents/pebl-exp.X.X/)
- ✅ File browser for .pbl experiments
- ✅ Recursive directory scanning
- ✅ Subject code input
- ✅ Language selection (auto-detects from translations/)
- ✅ Fullscreen toggle
- ✅ Experiment launching (fork/exec on Linux)
- ✅ Configuration persistence (launcher.cfg)

### UI Elements
- ✅ Menu bar (File, Tools, Help)
- ✅ Split-panel layout (file list + details)
- ✅ HiDPI support
- ✅ Keyboard navigation
- ✅ About dialog
- ✅ Parameter/translation detection

### Technical
- ✅ SDL2 + Dear ImGui integration
- ✅ Cross-platform path handling (Linux/Windows/macOS)
- ✅ Process management (spawn child processes)
- ✅ Configuration management

## Dependencies

### System Libraries
Install via your package manager:

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install SDL2-devel SDL2_image-devel
```

### External Libraries (Manual Installation)

#### Dear ImGui
The launcher uses Dear ImGui for the user interface. Clone it into `libs/imgui/`:

```bash
cd libs/
git clone https://github.com/ocornut/imgui.git
cd imgui
git checkout v1.90.1  # Or latest stable version
```

Required ImGui files (automatically compiled by Makefile):
- `imgui.cpp`, `imgui_draw.cpp`, `imgui_widgets.cpp`, `imgui_tables.cpp`
- `backends/imgui_impl_sdl2.cpp`, `backends/imgui_impl_sdlrenderer2.cpp`

#### nlohmann/json
The JSON library is already included as a single header file in `libs/json.hpp` (tracked in git).

## Building

### From Repository Root (Recommended)
```bash
make pebl-launcher
```

### From Launcher Directory
```bash
cd src/apps/launcher
make
```

**Requirements:**
- g++ with C++17 support
- SDL2 development libraries (libsdl2-dev, libsdl2-image-dev)
- Dear ImGui (see installation above)
- nlohmann/json (included in libs/json.hpp)

**Output:**
- Binary: `bin/pebl-launcher` (~7.4MB with study system)

## Usage

```bash
bin/pebl-launcher
```

On first run:
1. Launcher auto-detects PEBL installation in Documents/pebl-exp.X.X/
2. Defaults to battery/ subdirectory
3. Scans for .pbl experiments recursively
4. Creates launcher.cfg for persistent configuration

## Architecture

```
src/apps/launcher/
├── PEBLLauncher.cpp        - Main entry point, SDL/ImGui initialization
├── LauncherUI.cpp/h        - User interface rendering
├── LauncherConfig.cpp/h    - Configuration management & auto-detection
├── ExperimentRunner.cpp/h  - Process spawning & monitoring
├── Study.cpp/h             - Study data model (JSON parsing)
├── Chain.cpp/h             - Chain data model (test sequences)
├── WorkspaceManager.cpp/h  - Workspace initialization & management
├── SnapshotManager.cpp/h   - Snapshot creation & validation
└── Makefile                - Build configuration

libs/imgui/                 - Dear ImGui library
libs/json.hpp               - nlohmann JSON parser
```

## Study System (Phase 3 Implementation)

The launcher now includes a complete study management system:

### Implemented Components
- ✅ **Study data model** - Load/save study-info.json with tests and parameter variants
- ✅ **Chain data model** - Load/save chain JSON files with instruction/test sequences
- ✅ **WorkspaceManager** - Initialize Documents/pebl-exp.2.4/ workspace structure
- ✅ **SnapshotManager** - Create/import snapshots for sharing studies

### Build Study System Only
```bash
cd src/apps/launcher
make study-system
```

### Documentation
See comprehensive documentation in `doc/`:
- `STUDY_SYSTEM_IMPLEMENTATION_STATUS.md` - Current implementation status
- `CPP_IMPLEMENTATION_GUIDE.md` - C++ developer guide
- `NATIVE_LAUNCHER_STUDY_SYSTEM.md` - Complete system design
- `STUDY_FORMAT_SPECIFICATION.md` - JSON format specifications

## Configuration File

Stored in: `Documents/pebl-exp.X.X/launcher.cfg`
Fallback: `~/.pebl/launcher.cfg`

Format:
```ini
experiment_directory=/path/to/battery
subject_code=test
language=en
fullscreen=false
```

## Future Enhancements

### Phase 2 (Advanced Features)
- [ ] Parameter editor (JSON/CSV)
- [ ] Experiment chain builder
- [ ] Translation editor
- [ ] About file viewer (.pbl.about.txt)
- [ ] Log file viewer (stderr.txt)
- [ ] Version checker (HTTP)
- [ ] Upload integration (PEBL Hub tokens)
- [ ] Data combiner
- [ ] Recent experiments menu

### Phase 3 (Polish)
- [ ] Native file/folder dialogs
- [ ] Drag-drop experiment files
- [ ] Themes/styling
- [ ] Keyboard shortcuts (Ctrl+O, etc.)
- [ ] Process monitoring (show output, detect completion)
- [ ] Error dialogs
- [ ] Windows/macOS builds and testing

## Advantages Over PEBL-based Launcher

| Feature | Old (PEBL) | New (ImGui) |
|---------|------------|-------------|
| Binary size | ~8MB | 1.1MB |
| Startup time | 2-3 seconds | <0.5 seconds |
| Memory usage | ~80-120 MB | ~15-30 MB |
| HiDPI support | Poor | Native |
| Development | 4,034 lines PEBL | ~800 lines C++ |
| Debuggable | Limited | Full gdb/lldb |

## License

Copyright (c) 2026 Shane T. Mueller
Licensed under GPL (same as PEBL)
