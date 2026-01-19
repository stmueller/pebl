# PEBL Study System - Documentation Index

## Overview

This directory contains complete documentation for the PEBL Native Launcher study management system. This system enables:

- **Study-based organization** of tests and chains
- **Bidirectional workflow** between native and online platforms
- **Snapshot format** for sharing and archiving studies
- **Self-contained studies** with all required files

## Status

**Current Status:** Design and specification complete, ready for C++ implementation

- ✅ Design documents complete
- ✅ JSON schemas defined
- ✅ Example study created
- ✅ ChainPage.pbl implemented and tested
- ⏳ C++ launcher implementation in progress

## Quick Start

### For C++ Developers

1. **Start here:** [CPP_IMPLEMENTATION_GUIDE.md](CPP_IMPLEMENTATION_GUIDE.md)
2. **System design:** [NATIVE_LAUNCHER_STUDY_SYSTEM.md](NATIVE_LAUNCHER_STUDY_SYSTEM.md)
3. **Implementation plan:** [STUDY_SYSTEM_IMPLEMENTATION_PLAN.md](STUDY_SYSTEM_IMPLEMENTATION_PLAN.md)

### For Format Documentation

1. **Study format:** [STUDY_FORMAT_SPECIFICATION.md](STUDY_FORMAT_SPECIFICATION.md)
2. **Chain format:** [CHAIN_FORMAT_SPECIFICATION.md](CHAIN_FORMAT_SPECIFICATION.md)
3. **ChainPage format:** [CHAINPAGE_SPECIFICATION.md](CHAINPAGE_SPECIFICATION.md)
4. **Naming rules:** [NAMING_CONVENTIONS.md](NAMING_CONVENTIONS.md)

### For Examples

1. **Example study:** `../examples/example-study/`
2. **JSON schemas:** `../schemas/`
3. **ChainPage.pbl:** `../pebl-lib/ChainPage.pbl`

## Document List

### Design Documents

| Document | Purpose | Audience |
|----------|---------|----------|
| [NATIVE_LAUNCHER_STUDY_SYSTEM.md](NATIVE_LAUNCHER_STUDY_SYSTEM.md) | Complete system design with UI mockups | All |
| [STUDY_SYSTEM_IMPLEMENTATION_PLAN.md](STUDY_SYSTEM_IMPLEMENTATION_PLAN.md) | Development phases and timeline | Developers |
| [CPP_IMPLEMENTATION_GUIDE.md](CPP_IMPLEMENTATION_GUIDE.md) | C++ code examples and quick start | C++ Developers |

### Format Specifications

| Document | Purpose | Audience |
|----------|---------|----------|
| [STUDY_FORMAT_SPECIFICATION.md](STUDY_FORMAT_SPECIFICATION.md) | study-info.json format | Developers, Users |
| [CHAIN_FORMAT_SPECIFICATION.md](CHAIN_FORMAT_SPECIFICATION.md) | Chain JSON format | Developers, Users |
| [CHAINPAGE_SPECIFICATION.md](CHAINPAGE_SPECIFICATION.md) | ChainPage.pbl configuration | Developers |
| [NAMING_CONVENTIONS.md](NAMING_CONVENTIONS.md) | File and directory naming rules | All |

### Legacy Documents

| Document | Purpose | Status |
|----------|---------|--------|
| [LAYOUT_MIGRATION_GUIDE.md](LAYOUT_MIGRATION_GUIDE.md) | Legacy layout system migration | Reference only |
| [MASTER_PLAN.md](MASTER_PLAN.md) | Historical planning document | Superseded |

## Directory Structure

```
pebl/
├── doc/                              # This directory
│   ├── STUDY_SYSTEM_README.md        # This file
│   ├── NATIVE_LAUNCHER_STUDY_SYSTEM.md
│   ├── STUDY_SYSTEM_IMPLEMENTATION_PLAN.md
│   ├── CPP_IMPLEMENTATION_GUIDE.md
│   ├── STUDY_FORMAT_SPECIFICATION.md
│   ├── CHAIN_FORMAT_SPECIFICATION.md
│   ├── CHAINPAGE_SPECIFICATION.md
│   └── NAMING_CONVENTIONS.md
├── schemas/                          # JSON schemas
│   ├── study-info.schema.json
│   ├── chain.schema.json
│   └── chainpage-config.schema.json
├── examples/                         # Example studies
│   └── example-study/
│       ├── README.md
│       ├── study-info.json
│       ├── chains/
│       │   ├── main-chain.json
│       │   └── practice-chain.json
│       └── tests/
│           ├── BART/
│           └── corsi/
└── pebl-lib/                         # PEBL library
    └── ChainPage.pbl                 # Page display script
```

## Key Concepts

### Study

A self-contained directory containing:
- Metadata (study-info.json)
- Tests (copied from battery)
- Chains (test sequences)
- Participant data (runtime)

**Location:** `Documents/pebl-exp.2.3/my_studies/study-name/`

### Chain

An ordered sequence of:
- Instruction pages
- Consent forms
- Tests
- Completion pages

**Format:** JSON file in `study/chains/`

### Snapshot

A clean copy of a study ready for sharing:
- No participant data
- Complete test files
- All chains included

**Location:** `Documents/pebl-exp.2.3/snapshots/study_vN_date/`

### ChainPage.pbl

PEBL script that displays instruction/consent/completion pages.
- Reads JSON configuration
- Displays title and content
- Waits for OK click
- No data collection

**Location:** `pebl-lib/ChainPage.pbl`

## Workflow

### Native → Online

1. Create study in native launcher
2. Add tests from battery
3. Create chains
4. Run chains with participants
5. Export snapshot (excludes data)
6. Upload snapshot to platform

### Online → Native

1. Download snapshot from platform
2. Import into native launcher
3. Runs directly (same format)
4. Collect data locally
5. Can modify and re-upload

## Implementation Status

### Completed (Phase 1-2)

- ✅ JSON schema definitions
- ✅ Example study with BART and Corsi
- ✅ ChainPage.pbl implementation
- ✅ Format specifications
- ✅ System design document
- ✅ Implementation plan
- ✅ C++ implementation guide
- ✅ Naming conventions

### In Progress (Phase 3-8)

- ⏳ C++ launcher development
- ⏳ Study management UI
- ⏳ Chain editor UI
- ⏳ Chain execution engine
- ⏳ Parameter editor

### Future (Phase 9-12)

- ⏳ Snapshot management
- ⏳ Online platform integration
- ⏳ Polish and testing
- ⏳ User documentation

## Testing the Current System

### Test ChainPage.pbl

```bash
# Create test config
cat > test-page.json << EOF
{
  "title": "Test Page",
  "content": "This is a test.\n\nPress OK to continue.",
  "page_type": "instruction"
}
EOF

# Run ChainPage
bin/pebl2 pebl-lib/ChainPage.pbl -v test-page.json
```

### Explore Example Study

```bash
# View structure
ls -R examples/example-study/

# Read study info
cat examples/example-study/study-info.json

# Read main chain
cat examples/example-study/chains/main-chain.json

# Run individual test
bin/pebl2 examples/example-study/tests/BART/BART.pbl -v subnum=999
```

## File Naming Rules

**CRITICAL:** Never use spaces in filenames or directory names

- ✓ `my-study` (hyphens)
- ✓ `main-chain.json` (hyphens)
- ✓ `subject-101` (hyphens or underscores)
- ✗ `My Study` (spaces!)
- ✗ `main chain.json` (spaces!)

See [NAMING_CONVENTIONS.md](NAMING_CONVENTIONS.md) for complete rules.

## JSON Validation

Schemas available in `../schemas/` for validation:

```bash
# Using jsonschema (Python)
jsonschema -i study-info.json schemas/study-info.schema.json

# Using ajv (Node.js)
ajv validate -s schemas/study-info.schema.json -d study-info.json
```

## Platform Integration

### Online Platform Needs

To support bidirectional workflow, platform needs:

1. **Upload endpoint** - Accept snapshot ZIP
2. **Parse JSON** - study-info.json and chains/*.json
3. **Import to database** - Store study structure
4. **Generate tokens** - For access control
5. **Download format** - Create snapshots for download

See [NATIVE_LAUNCHER_STUDY_SYSTEM.md](NATIVE_LAUNCHER_STUDY_SYSTEM.md) for details.

## Contributing

### Adding Documentation

1. Follow existing format and style
2. Use markdown for all docs
3. Include examples
4. Update this README

### Updating Specifications

1. Discuss changes first
2. Update JSON schemas
3. Update example study if needed
4. Update all affected docs
5. Increment version numbers

### Testing Changes

1. Validate JSON schemas
2. Test with example study
3. Verify ChainPage.pbl still works
4. Check all cross-references

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-09 | Initial complete specification |

## Contact

For questions or issues:
- Check documentation first
- Review example study
- Test with ChainPage.pbl
- Consult PEBL manual

## License

Part of the PEBL project. See main PEBL license.

---

**Last Updated:** 2026-01-09
**Status:** Design complete, ready for implementation
**Next Steps:** Begin C++ launcher development (Phase 3)
