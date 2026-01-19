# Study System Implementation Plan

## Overview

This plan implements the Native Launcher Study Management System as specified in `NATIVE_LAUNCHER_STUDY_SYSTEM.md`. The implementation is divided into phases that build upon each other, starting with foundational file formats and validation, then moving to C++ launcher development.

## Phase 1: File Formats and Specifications (Week 1)

**Status:** Ready to implement now
**Dependencies:** None
**Deliverables:**

1. **JSON Schema Definitions**
   - [ ] `schemas/study-info.schema.json` - JSON Schema for study-info.json validation
   - [ ] `schemas/chain.schema.json` - JSON Schema for chain files
   - [ ] `schemas/chainpage-config.schema.json` - JSON Schema for ChainPage.pbl configs

2. **Example Study Structure**
   - [ ] `examples/example-study/` - Complete example study with 2-3 tests
   - [ ] `examples/example-study/study-info.json`
   - [ ] `examples/example-study/chains/main-chain.json`
   - [ ] `examples/example-study/chains/practice-chain.json`
   - [ ] Example includes BART and corsi tests

3. **Documentation**
   - [ ] `doc/STUDY_FORMAT_SPECIFICATION.md` - Detailed format specification
   - [ ] `doc/CHAIN_FORMAT_SPECIFICATION.md` - Chain file format details
   - [ ] `examples/example-study/README.md` - How to use the example

**Implementation:** PEBL scripts, JSON files, documentation
**Owner:** Can implement now
**Validation:** JSON schema validators, manual inspection

## Phase 2: Validation and Testing Utilities (Week 2)

**Status:** Ready to implement after Phase 1
**Dependencies:** Phase 1 complete
**Deliverables:**

1. **Study Validator (PEBL)**
   - [ ] `utils/validate-study.pbl` - Validates study structure
   - [ ] Checks study-info.json against schema
   - [ ] Validates all chains
   - [ ] Verifies test directories exist
   - [ ] Reports errors and warnings

2. **Snapshot Validator (PEBL)**
   - [ ] `utils/validate-snapshot.pbl` - Validates snapshot structure
   - [ ] Ensures no data/ directories present
   - [ ] Checks completeness

3. **Study Creation Utility (PEBL Prototype)**
   - [ ] `utils/create-study.pbl` - Creates empty study structure
   - [ ] Prompts for study name, author, description
   - [ ] Creates directory structure
   - [ ] Generates initial study-info.json

**Implementation:** PEBL scripts
**Owner:** Can implement now
**Validation:** Run against example studies

## Phase 3: Workspace Initialization (Week 3)

**Status:** Needs C++ development
**Dependencies:** Phase 1-2 complete
**Deliverables:**

1. **First-Run Detection**
   - [ ] C++ function to check for `Documents/pebl-exp.2.3`
   - [ ] Version detection (migrate from 2.2 if exists)

2. **Directory Creation**
   - [ ] Create workspace structure
   - [ ] Copy bundled resources (demo, tutorial, docs)
   - [ ] Create README.txt

3. **Welcome Dialog**
   - [ ] First-run welcome screen
   - [ ] Quick start guide
   - [ ] Link to tutorials

**Implementation:** C++ code in launcher
**Owner:** C++ launcher developer
**Validation:** Test on clean system

## Phase 4: Study Management Core (Week 4-5)

**Status:** Needs C++ development
**Dependencies:** Phase 3 complete
**Deliverables:**

1. **Study Data Structures**
   - [ ] Study class/struct
   - [ ] Chain class/struct
   - [ ] ChainItem class/struct
   - [ ] Test class/struct
   - [ ] Parameter variant structures

2. **JSON Parsing**
   - [ ] study-info.json parser
   - [ ] chain JSON parser
   - [ ] chainpage-config.json generator
   - [ ] JSON writer for all formats

3. **Study File Operations**
   - [ ] Load study from directory
   - [ ] Save study-info.json
   - [ ] Create new study
   - [ ] Delete study
   - [ ] Update metadata

**Implementation:** C++ code
**Owner:** C++ launcher developer
**Validation:** Unit tests for each parser/writer

## Phase 5: Study Manager UI (Week 6)

**Status:** Needs C++ GUI development
**Dependencies:** Phase 4 complete
**Deliverables:**

1. **Study List View**
   - [ ] Display all studies in my_studies/
   - [ ] Show name, modified date, version
   - [ ] Refresh button

2. **Study Operations**
   - [ ] New Study dialog
   - [ ] Open Study (go to editor)
   - [ ] Delete Study (with confirmation)
   - [ ] Import Snapshot

3. **Menu Integration**
   - [ ] Studies → My Studies
   - [ ] Studies → New Study
   - [ ] Studies → Import Snapshot

**Implementation:** C++ GUI (Qt/wxWidgets/etc.)
**Owner:** C++ launcher developer
**Validation:** Manual UI testing

## Phase 6: Study Editor UI (Week 7-8)

**Status:** Needs C++ GUI development
**Dependencies:** Phase 5 complete
**Deliverables:**

1. **Study Editor View**
   - [ ] Display study metadata (name, author, version)
   - [ ] Edit Metadata button/dialog
   - [ ] Tests list view
   - [ ] Chains list view

2. **Test Management**
   - [ ] Add Test from Battery (browser + copy operation)
   - [ ] Remove Test from Study
   - [ ] Manage Parameter Variants

3. **Chain Management**
   - [ ] New Chain button
   - [ ] Edit Chain button
   - [ ] Run Chain button
   - [ ] Delete Chain button

**Implementation:** C++ GUI
**Owner:** C++ launcher developer
**Validation:** Create/edit test study

## Phase 7: Chain Editor UI (Week 9)

**Status:** Needs C++ GUI development
**Dependencies:** Phase 6 complete
**Deliverables:**

1. **Chain Editor View**
   - [ ] Chain items list
   - [ ] Add buttons (Instruction/Consent/Test/Completion)
   - [ ] Edit/Delete item buttons
   - [ ] Move Up/Down buttons

2. **Page Dialogs**
   - [ ] Instruction page dialog (title + content)
   - [ ] Consent page dialog (title + content)
   - [ ] Completion page dialog (title + content)
   - [ ] Test item dialog (test + variant + language)

3. **Chain Persistence**
   - [ ] Save chain to JSON
   - [ ] Load chain from JSON
   - [ ] Validate chain before save

**Implementation:** C++ GUI
**Owner:** C++ launcher developer
**Validation:** Create complex chain, save/reload

## Phase 8: Chain Execution Engine (Week 10)

**Status:** Needs C++ development
**Dependencies:** Phase 7 complete
**Deliverables:**

1. **Chain Runner**
   - [ ] Execute chain items in sequence
   - [ ] Create temp ChainPage JSON files
   - [ ] Call ChainPage.pbl with temp config
   - [ ] Execute tests with parameters
   - [ ] Handle errors/interruptions

2. **Subject ID Management**
   - [ ] Prompt for subject ID before chain
   - [ ] Pass to all tests via -v subnum=
   - [ ] Validate/sanitize subject ID

3. **Process Management**
   - [ ] Launch pebl2 as subprocess
   - [ ] Wait for completion
   - [ ] Capture exit codes
   - [ ] Handle crashes

**Implementation:** C++ code
**Owner:** C++ launcher developer
**Validation:** Run complete chain with multiple tests

## Phase 9: Snapshot Management (Week 11)

**Status:** Needs C++ development
**Dependencies:** Phase 4 complete (can parallelize with UI)
**Deliverables:**

1. **Snapshot Creation**
   - [ ] Copy study excluding data/ directories
   - [ ] Create snapshot in snapshots/
   - [ ] Optional ZIP creation
   - [ ] Version increment option

2. **Snapshot Import**
   - [ ] Select snapshot directory or ZIP
   - [ ] Extract if ZIP
   - [ ] Validate snapshot structure
   - [ ] Copy to my_studies/
   - [ ] Create data/ directories

3. **Snapshot Browser**
   - [ ] List snapshots in snapshots/
   - [ ] Import button
   - [ ] Delete snapshot

**Implementation:** C++ code
**Owner:** C++ launcher developer
**Validation:** Create snapshot, import on different machine

## Phase 10: Parameter Editor (Week 12)

**Status:** Needs C++ GUI development
**Dependencies:** Phase 6 complete
**Deliverables:**

1. **Schema Parser**
   - [ ] Parse .schema.json files (pipe-delimited format)
   - [ ] Extract parameter name, default, description
   - [ ] Infer type from default value

2. **Dynamic Form Generator**
   - [ ] Create input fields based on schema
   - [ ] Show defaults and descriptions
   - [ ] Type-appropriate inputs (number/bool/string)

3. **Parameter File Writer**
   - [ ] Save edited values to .par.json
   - [ ] Only save non-default values (optional)
   - [ ] Validate values before save

**Implementation:** C++ GUI
**Owner:** C++ launcher developer
**Validation:** Edit BART parameters, save, load in test

## Phase 11: Online Platform Integration (Week 13-14)

**Status:** Needs both C++ and platform development
**Dependencies:** Phase 9 complete
**Deliverables:**

### Launcher Side:
1. **Upload Functionality**
   - [ ] Create snapshot
   - [ ] ZIP snapshot
   - [ ] HTTP POST to platform
   - [ ] Receive study token
   - [ ] Update local study-info.json

2. **Authentication**
   - [ ] Login dialog
   - [ ] Store credentials/token
   - [ ] Send with upload request

### Platform Side:
3. **Upload Endpoint**
   - [ ] Accept ZIP file upload
   - [ ] Extract and validate
   - [ ] Parse study-info.json
   - [ ] Parse chains/*.json
   - [ ] Import into database
   - [ ] Generate study token
   - [ ] Return token and URL

4. **Study Token Management**
   - [ ] Generate unique tokens
   - [ ] Associate with user account
   - [ ] Access control via token

**Implementation:** C++ launcher + PHP/JS platform code
**Owner:** C++ launcher dev + Platform dev
**Validation:** Upload study, verify in platform, download, verify identical

## Phase 12: Polish and Testing (Week 15-16)

**Status:** Final phase
**Dependencies:** All previous phases
**Deliverables:**

1. **Error Handling**
   - [ ] Validate all user inputs
   - [ ] Graceful error messages
   - [ ] Confirmation dialogs
   - [ ] Undo/cancel options

2. **User Experience**
   - [ ] Progress indicators for long operations
   - [ ] Keyboard shortcuts
   - [ ] Tooltips and help text
   - [ ] Consistent styling

3. **Testing**
   - [ ] Unit tests for all core functions
   - [ ] Integration tests for workflows
   - [ ] User acceptance testing
   - [ ] Cross-platform testing (Windows/Linux/Mac)

4. **Documentation**
   - [ ] User manual updates
   - [ ] Tutorial videos/guides
   - [ ] Developer documentation
   - [ ] API documentation

**Implementation:** All components
**Owner:** Full team
**Validation:** Comprehensive test plan

## Immediate Action Plan (This Week)

### What Can Be Done Now (No C++ Required)

**Day 1: File Formats**
- [x] Create NATIVE_LAUNCHER_STUDY_SYSTEM.md design doc
- [ ] Create JSON schemas for validation
- [ ] Create example study structure
- [ ] Write format specification docs

**Day 2: Example Study**
- [ ] Create example-study/ with BART and corsi
- [ ] Write study-info.json
- [ ] Write main-chain.json and practice-chain.json
- [ ] Test with ChainPage.pbl

**Day 3: Validation Tools**
- [ ] Write validate-study.pbl
- [ ] Write validate-snapshot.pbl
- [ ] Test validators on example study

**Day 4: Utility Scripts**
- [ ] Write create-study.pbl (prototype)
- [ ] Write create-snapshot.pbl (prototype)
- [ ] Test end-to-end workflow

**Day 5: Documentation**
- [ ] Complete format specifications
- [ ] Write migration guide from old launcher
- [ ] Create developer guide for C++ team

### What Needs C++ Development

Everything from Phase 3 onward requires C++ launcher development:
- GUI framework selection (Qt recommended)
- Study manager window
- Study editor window
- Chain editor window
- Chain execution engine
- Parameter editor
- File operations

## Resource Requirements

### Development Team
- **PEBL Developer** (Phase 1-2): File formats, examples, validation
- **C++ Developer** (Phase 3-12): Launcher application
- **UI/UX Designer** (Phase 5-7, 10): Interface design
- **Platform Developer** (Phase 11): Upload/download endpoints
- **QA Tester** (Phase 12): Testing and validation

### Tools and Libraries
- **JSON Library:** nlohmann/json or RapidJSON for C++
- **GUI Framework:** Qt (cross-platform, mature)
- **HTTP Library:** libcurl or Qt Network
- **ZIP Library:** libzip or Qt Archive
- **Testing:** Google Test for C++ unit tests

### Timeline
- **Phase 1-2:** 2 weeks (Can start immediately)
- **Phase 3-8:** 8 weeks (C++ development)
- **Phase 9-11:** 4 weeks (C++ + platform integration)
- **Phase 12:** 2 weeks (Polish and testing)
- **Total:** 16 weeks (~4 months)

## Risk Mitigation

### Technical Risks
1. **JSON parsing complexity** → Use well-tested library
2. **Cross-platform GUI issues** → Use Qt for consistency
3. **Process management complexity** → Thorough testing
4. **Data loss during operations** → Implement backup/undo

### Schedule Risks
1. **C++ developer availability** → Start Phase 1-2 immediately to unblock
2. **Platform integration delays** → Implement offline functionality first
3. **Testing time underestimated** → Allocate 2 full weeks

## Success Criteria

### Phase 1-2 Success
- ✓ Example study validates successfully
- ✓ Can create snapshot from example
- ✓ ChainPage.pbl works with example chains
- ✓ Documentation is clear and complete

### Overall Success
- ✓ User can create study from scratch in < 5 minutes
- ✓ User can add battery tests to study
- ✓ User can create and run chains
- ✓ Snapshots work bidirectionally (create/import)
- ✓ Upload/download from platform works
- ✓ No data loss during any operation
- ✓ Works on Windows, Linux, and Mac

## Next Steps

**Immediate (Today):**
1. Create JSON schemas
2. Create example study structure
3. Test with existing ChainPage.pbl

**This Week:**
4. Build validation utilities
5. Complete documentation
6. Hand off to C++ team with complete spec

**Next Month:**
7. C++ team implements Phase 3-5
8. Weekly check-ins on progress
9. Iterate on design based on implementation feedback

---

**Document Status:** Draft v1.0
**Last Updated:** 2026-01-09
**Next Review:** After Phase 1-2 completion
