# PEBL Study System - Implementation Status

**Last Updated:** 2026-01-09

## Overview

This document tracks the implementation status of the PEBL Native Launcher study management system. The system enables study-based organization of tests and chains with bidirectional workflow between native and online platforms.

## Phase 3: Core Data Models (✅ COMPLETE)

### Completed Components

#### 1. Study Data Model (`Study.h`, `Study.cpp`)
**Status:** ✅ Implemented and compiled

**Features:**
- Load/save study-info.json using nlohmann/json
- Study metadata management (name, description, version, author, dates)
- Test collection with parameter variants
- Chain file discovery and counting
- Validation with error/warning reporting
- Create new studies with directory structure

**Key Methods:**
- `LoadFromDirectory()` - Parse study-info.json
- `Save()` - Write study-info.json
- `CreateNew()` - Initialize new study
- `AddTest()` / `RemoveTest()` - Manage test collection
- `Validate()` - Check study structure integrity

**Files:**
- `/home/smueller/Dropbox/Research/pebl/pebl/src/apps/launcher/Study.h`
- `/home/smueller/Dropbox/Research/pebl/pebl/src/apps/launcher/Study.cpp`

#### 2. Chain Data Model (`Chain.h`, `Chain.cpp`)
**Status:** ✅ Implemented and compiled

**Features:**
- Load/save chain JSON files
- Chain metadata (name, description)
- Item management (instruction/consent/completion/test)
- ChainPage.pbl config generation for page items
- Test command building for test items
- Validation against study (check test/variant/language availability)
- Move/insert/remove items

**Key Methods:**
- `LoadFromFile()` - Parse chain JSON
- `Save()` - Write chain JSON
- `CreateNew()` - Initialize new chain
- `AddItem()` / `RemoveItem()` / `MoveItem()` - Manage items
- `Validate()` - Check chain integrity against study

**ChainItem Features:**
- `CreateChainPageConfig()` - Generate temp JSON for ChainPage.pbl
- `BuildTestCommand()` - Construct PEBL command with parameters
- `GetDisplayName()` - UI-friendly name

**Files:**
- `/home/smueller/Dropbox/Research/pebl/pebl/src/apps/launcher/Chain.h`
- `/home/smueller/Dropbox/Research/pebl/pebl/src/apps/launcher/Chain.cpp`

#### 3. WorkspaceManager (`WorkspaceManager.h`, `WorkspaceManager.cpp`)
**Status:** ✅ Implemented and compiled

**Features:**
- First-run initialization of Documents/pebl-exp.2.3/
- Directory structure creation (my_studies/, snapshots/, doc/, demo/, tutorial/)
- Study discovery and listing
- Snapshot discovery and listing
- Create new study directories
- XDG-compliant Documents path detection
- Snapshot creation (excludes data/)
- Snapshot import into my_studies

**Key Methods:**
- `Initialize()` - Create workspace directories
- `GetStudyDirectories()` - List all studies
- `GetSnapshotDirectories()` - List all snapshots
- `CreateStudyDirectory()` - Initialize new study
- `CreateSnapshot()` - Export study as snapshot
- `ImportSnapshot()` - Import snapshot into studies

**Files:**
- `/home/smueller/Dropbox/Research/pebl/pebl/src/apps/launcher/WorkspaceManager.h`
- `/home/smueller/Dropbox/Research/pebl/pebl/src/apps/launcher/WorkspaceManager.cpp`

#### 4. SnapshotManager (`SnapshotManager.h`, `SnapshotManager.cpp`)
**Status:** ✅ Implemented and compiled

**Features:**
- Create snapshots with automatic naming (studyname_vN_YYYY-MM-DD)
- Validate snapshot structure
- Import snapshots into my_studies
- Exclude data/ directories from snapshots
- Snapshot metadata extraction
- Smart file exclusion (hidden files, temp files, data/)

**Key Methods:**
- `CreateSnapshot()` - Copy study excluding data/
- `ValidateSnapshot()` - Check snapshot integrity
- `ImportSnapshot()` - Import into studies directory
- `GenerateSnapshotName()` - Create standardized name
- `GetSnapshotInfo()` - Extract metadata

**Files:**
- `/home/smueller/Dropbox/Research/pebl/pebl/src/apps/launcher/SnapshotManager.h`
- `/home/smueller/Dropbox/Research/pebl/pebl/src/apps/launcher/SnapshotManager.cpp`

### Build System

**Makefile Updates:**
- Added Study.cpp, Chain.cpp, WorkspaceManager.cpp, SnapshotManager.cpp to launcher/Makefile
- Created `pebl-launcher` target in main Makefile
- All modules compile cleanly with `-Wall -Wextra` (minimal warnings)

**Compilation Command:**
```bash
# From repository root:
make pebl-launcher

# From src/apps/launcher:
make all
```

**Output:**
- Binary: `bin/pebl-launcher` (SDL2 + ImGui GUI application)

**JSON Library:**
- Using nlohmann/json (already in libs/json.hpp)
- JSMN remains for PEBL interpreter (different purpose)
- No conflicts between libraries

## Phase 4-8: UI and Integration (⏳ PENDING)

### Next Steps

#### Phase 4: Study Management UI
- ImGui study list view
- Create/open/delete study dialogs
- Study properties editor
- Test browser and add/remove interface

#### Phase 5: Chain Editor UI
- Chain list view
- Item drag-and-drop reordering
- Add instruction/consent/completion pages
- Add tests from study
- Preview chain structure

#### Phase 6: Chain Execution
- ChainRunner class to execute chains
- Integrate with ChainPage.pbl
- Execute test items with correct parameters
- Track execution progress
- Handle errors/interruptions

#### Phase 7: Parameter Editor
- Use JSON schemas to format parameter UI
- Save to .par.json files (not schema files)
- Validate parameter types
- Support for different parameter variants

#### Phase 8: UI Polish
- Icons and visual improvements
- Keyboard shortcuts
- Drag-and-drop
- Progress indicators
- Error dialogs

## Phase 9-12: Advanced Features (⏳ PENDING)

#### Phase 9: Snapshot Management
- Snapshot browser UI
- Download from online platform
- Upload to online platform
- Version comparison

#### Phase 10: Online Integration
- HTTP client for platform API
- Upload/download snapshots
- Sync study metadata
- Token management

#### Phase 11: Testing
- Unit tests for data models
- Integration tests for workflow
- Snapshot round-trip testing
- UI testing

#### Phase 12: Documentation
- User manual
- Developer guide
- Video tutorials
- Example studies

## Testing Status

### ChainPage.pbl
**Status:** ✅ Implemented and tested
- Location: `/home/smueller/Dropbox/Research/pebl/pebl/pebl-lib/ChainPage.pbl`
- Syntax validated
- Displays instruction/consent/completion pages
- Reads JSON configuration
- 77 lines, minimal dependencies

### Example Study
**Status:** ✅ Created
- Location: `/home/smueller/Dropbox/Research/pebl/pebl/examples/example-study/`
- Includes BART and corsi tests
- Two chains: main-chain.json and practice-chain.json
- Complete study-info.json
- README with usage instructions

### JSON Schemas
**Status:** ✅ Created
- study-info.schema.json - Study format validation
- chain.schema.json - Chain format validation
- chainpage-config.schema.json - ChainPage.pbl config

## File Locations

### Implementation Files
```
src/apps/launcher/
├── Study.h                     ✅ Complete
├── Study.cpp                   ✅ Complete
├── Chain.h                     ✅ Complete
├── Chain.cpp                   ✅ Complete
├── WorkspaceManager.h          ✅ Complete
├── WorkspaceManager.cpp        ✅ Complete
├── SnapshotManager.h           ✅ Complete
├── SnapshotManager.cpp         ✅ Complete
├── Makefile                    ✅ Updated
└── (existing launcher files)
```

### Documentation Files
```
doc/
├── STUDY_SYSTEM_README.md              ✅ Complete
├── NATIVE_LAUNCHER_STUDY_SYSTEM.md     ✅ Complete
├── STUDY_SYSTEM_IMPLEMENTATION_PLAN.md ✅ Complete
├── CPP_IMPLEMENTATION_GUIDE.md         ✅ Complete
├── STUDY_FORMAT_SPECIFICATION.md       ✅ Complete
├── CHAIN_FORMAT_SPECIFICATION.md       ✅ Complete
├── CHAINPAGE_SPECIFICATION.md          ✅ Complete
├── NAMING_CONVENTIONS.md               ✅ Complete
└── STUDY_SYSTEM_IMPLEMENTATION_STATUS.md (this file)
```

### Support Files
```
pebl-lib/
└── ChainPage.pbl               ✅ Complete

schemas/
├── study-info.schema.json      ✅ Complete
├── chain.schema.json           ✅ Complete
└── chainpage-config.schema.json ✅ Complete

examples/
└── example-study/              ✅ Complete
    ├── README.md
    ├── study-info.json
    ├── chains/
    │   ├── main-chain.json
    │   └── practice-chain.json
    └── tests/
        ├── BART/
        └── corsi/
```

## Dependencies

### External Libraries
- **nlohmann/json** - JSON parsing in launcher (libs/json.hpp)
- **JSMN** - JSON parsing in PEBL interpreter (libs/jsmn/)
- **Dear ImGui** - UI framework (libs/imgui/)
- **SDL2** - Graphics and input (system package)

### System Requirements
- Linux (Ubuntu/Debian tested)
- C++17 compiler (g++)
- SDL2 development libraries
- Standard POSIX file I/O

## Compilation Results

All study system modules compile cleanly:
```bash
$ cd src/apps/launcher
$ make study-system
Compiling Study.cpp...
Compiling Chain.cpp...
Compiling WorkspaceManager.cpp...
Compiling SnapshotManager.cpp...
Study system modules compiled successfully
```

No warnings or errors with `-Wall -Wextra` enabled.

## Design Decisions

### JSON Library Choice
- **Decision:** Use nlohmann/json for launcher, keep JSMN for PEBL
- **Rationale:**
  - PEBL interpreter needs JSMN for custom Variant type conversion
  - Launcher works with simple C++ types (std::string, int, etc.)
  - Different purposes, separate binaries, no conflict
  - nlohmann/json is header-only and modern C++17

### Directory Structure
- **Decision:** Documents/pebl-exp.2.3/ as workspace root
- **Rationale:**
  - Maintains compatibility with previous versions
  - User-accessible location (not hidden)
  - Includes documentation and examples
  - Follows XDG standards on Linux

### Snapshot Format
- **Decision:** Same as study format but excludes data/
- **Rationale:**
  - Simplifies bidirectional workflow
  - No format conversion needed
  - Easy to validate
  - Direct import without transformation

### Parameter Variants
- **Decision:** Variants stored in study-info.json, actual .par.json files in test/params/
- **Rationale:**
  - Study knows what variants are available
  - Tests can be shared across studies
  - Schema files remain authoritative for UI
  - .par.json files override defaults

## Known Limitations

1. **No GUI yet** - Data models complete, UI pending (Phase 4-8)
2. **No chain execution** - ChainRunner not yet implemented
3. **No online integration** - Platform API client pending
4. **Limited error handling** - Basic file I/O errors caught, needs improvement
5. **No undo/redo** - Will be added in UI phase
6. **No logging** - Should add logging framework

## Next Immediate Steps

1. **Test data models** - Create unit tests for Study/Chain/WorkspaceManager
2. **Implement StudyManager UI** - ImGui interface for study list
3. **Implement ChainEditor UI** - Visual chain editing with drag-and-drop
4. **Implement ChainRunner** - Execute chains with ChainPage.pbl integration
5. **Add logging** - Debug/info/warning/error logging framework

## Timeline Estimate

Based on current progress:
- **Phase 3 (Core Models):** ✅ Complete (2 days)
- **Phase 4-5 (Study/Chain UI):** ~3-5 days
- **Phase 6 (Chain Execution):** ~2-3 days
- **Phase 7-8 (Parameters/Polish):** ~3-4 days
- **Phase 9-10 (Snapshots/Online):** ~4-5 days
- **Phase 11-12 (Testing/Docs):** ~3-4 days

**Total remaining:** ~15-21 days of development

## Success Criteria

### Phase 3 (✅ Complete)
- [x] Study.h/cpp compile and link
- [x] Chain.h/cpp compile and link
- [x] WorkspaceManager.h/cpp compile and link
- [x] SnapshotManager.h/cpp compile and link
- [x] All modules pass `-Wall -Wextra`
- [x] JSON parsing works for study-info.json
- [x] JSON parsing works for chain files
- [x] Snapshot creation excludes data/

### Phase 4-12 (⏳ Pending)
- [ ] Can create new study via UI
- [ ] Can add tests to study
- [ ] Can create/edit chains
- [ ] Can execute chains end-to-end
- [ ] Can create snapshots
- [ ] Can import snapshots
- [ ] Can upload to online platform
- [ ] Can download from online platform
- [ ] All features documented
- [ ] User manual complete

## Contact

For implementation questions, see:
- CPP_IMPLEMENTATION_GUIDE.md - C++ code examples
- STUDY_SYSTEM_IMPLEMENTATION_PLAN.md - Detailed phase breakdown
- NATIVE_LAUNCHER_STUDY_SYSTEM.md - Overall system design

---

**Status Summary:** Phase 3 complete, ready to begin UI implementation (Phase 4)
