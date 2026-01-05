# PEBL Bundle Manifest System

## Overview

The bundle system now supports **manifest-based filtering** for core-battery bundles. This allows explicit control over which tests from `upload-battery/` are included in the core bundle, preventing accidental inclusion of large or specialized tests.

## Files

### 1. `core-battery-manifest.json`
Defines which tests are included in the core-battery bundle.

**Structure:**
```json
{
  "version": "1.0",
  "tests": ["ANT", "BART", "BST", ...],      // Tests to include
  "excluded_tests": ["bcst-64"],              // Tests explicitly excluded
  "exclude_patterns": ["*.pbl.png", ...]      // Additional file patterns to exclude
}
```

### 2. `bundle-config.json`
Updated to reference the manifest:

```json
{
  "core_battery": {
    "manifest": "core-battery-manifest.json",  // NEW: Reference to manifest
    ...
  }
}
```

### 3. `scripts/build-bundles.sh`
Enhanced to respect manifests:
- Checks for `manifest` field in bundle config
- If found, loads test list from manifest file
- Only includes specified tests from upload-battery
- Other paths (pebl-lib, media) are included normally

## How It Works

### Without Manifest (old behavior):
```bash
--preload upload-battery/@/usr/local/share/pebl2/battery
```
Includes **everything** in upload-battery.

### With Manifest (new behavior):
```bash
--preload upload-battery/ANT@/usr/local/share/pebl2/battery/ANT
--preload upload-battery/BART@/usr/local/share/pebl2/battery/BART
--preload upload-battery/BST@/usr/local/share/pebl2/battery/BST
...
```
Includes **only specified tests** from manifest.

## Adding New Tests to Core Battery

### Option 1: Add to Core (most tests)
Edit `core-battery-manifest.json`:
```json
{
  "tests": [
    ...existing tests...,
    "new-test"
  ]
}
```

### Option 2: Create Separate Bundle (large/specialized tests)
Edit `bundle-config.json`:
```json
{
  "test_bundles": [
    ...existing bundles...,
    {
      "name": "new-test",
      "path": "upload-battery/new-test",
      ...
    }
  ]
}
```

## Current Setup

### Core Battery (44 tests)
All tests from upload-battery EXCEPT:
- `bcst-64` - Separate bundle (large PNG assets)

### Separate Bundles
- **pcst** - PEBL Cognitive Screening Test (5.5 MB, audio files)
- **bcst-64** - Berg Card Sort 64-card version (600 KB, card images)

### Excluded Files (all bundles)
- `*/data/*` - Runtime data directories
- `*~` - Backup files
- `*.pbl.png` - Thumbnail images (never used in Emscripten)
- `*.config` - Local configuration files

## Benefits

1. **Explicit Control** - Know exactly what's in each bundle
2. **Prevent Bloat** - Large tests don't accidentally get bundled
3. **Flexibility** - Easy to create thematic bundles (spatial, attention, etc.)
4. **Documentation** - Manifest serves as documentation of bundle contents
5. **Maintainability** - Adding tests to upload-battery doesn't auto-bundle them

## Future Use Cases

### Thematic Bundles
```json
{
  "spatial-bundle": {
    "manifest": "spatial-battery-manifest.json",
    "tests": ["corsi", "maze", "tol", "toh", "manikin"]
  },
  "attention-bundle": {
    "manifest": "attention-battery-manifest.json",
    "tests": ["ANT", "flanker", "oddball", "gonogo", "pcpt"]
  }
}
```

### No Core Bundle Scenario
Remove `core_battery` entirely, only use thematic bundles.

## Building Bundles

```bash
# Build all bundles
./scripts/build-bundles.sh

# Build specific bundle
./scripts/build-bundles.sh core-battery
./scripts/build-bundles.sh bcst-64

# List available bundles
./scripts/build-bundles.sh --list
```

## Migration Path: Eliminating upload-battery/

With the manifest system and proper excludes, `upload-battery/` is no longer necessary. You can build directly from `battery/`:

### Current Setup (using upload-battery/)
```json
{
  "core_battery": {
    "manifest": "core-battery-manifest.json",
    "includes": [
      {
        "path": "upload-battery",
        "mount": "/usr/local/share/pebl2/battery"
      }
    ]
  }
}
```

### Future Setup (using battery/ directly)
```json
{
  "core_battery": {
    "manifest": "core-battery-manifest.json",
    "includes": [
      {
        "path": "battery",
        "mount": "/usr/local/share/pebl2/battery"
      }
    ]
  }
}
```

### Why This Works

The manifest's `exclude_patterns` removes all development artifacts:
- `*/data/*` - Test data directories
- `*.pbl.png` - Thumbnail images
- `*.pbl.schema` - Schema files (duplicates of params/*.schema.json)
- `*~` - Backup files
- `*-report.html`, `*-report.txt` - Test reports

This means:
1. **No need for upload-battery/** - battery/ can be used directly
2. **Single source of truth** - battery/ is the authoritative copy
3. **Simpler workflow** - No syncing between battery/ and upload-battery/
4. **Less maintenance** - One directory instead of two

### Migration Steps (Future)

1. Verify manifest excludes cover all unwanted files
2. Update bundle-config.json to point to `battery/` instead of `upload-battery/`
3. Test bundle build
4. Remove upload-battery/ directory
5. Update documentation

## Migration Notes

- Old bundle-config.json still works (no manifest = include everything)
- Manifest is optional - only used if specified
- Tests can be in multiple manifests (e.g., both core and thematic)
- Excludes still apply after manifest filtering
- Manifest system works with both battery/ and upload-battery/ paths
