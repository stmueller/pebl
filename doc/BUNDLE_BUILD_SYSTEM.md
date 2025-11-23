# PEBL Bundle Build System

## Overview

The PEBL Bundle Build System provides a scalable, configuration-driven approach to building and deploying data bundles for the PEBL Online Platform. Instead of creating individual build scripts for each test, all bundles are defined in a single configuration file (`bundle-config.json`) and built using universal scripts.

This system can scale to 100+ test bundles without requiring new build scripts or Makefile targets.

## Architecture

### Components

1. **`bundle-config.json`** - Centralized configuration defining all bundles
2. **`scripts/build-bundles.sh`** - Universal bundle builder script
3. **`scripts/deploy-bundles.sh`** - Universal deployment script
4. **Makefile targets** - Convenience targets for common build operations

### Bundle Types

- **Core bundle** (`pebl2`) - Main PEBL runtime with standard test battery
- **Test bundles** - Individual tests with large assets (e.g., `pcst` with audio files)
- **Private bundles** - Tests requiring individual access grants (future)

## Quick Start

### Building Bundles

```bash
# List all configured bundles
make list-bundles
# or
./scripts/build-bundles.sh --list

# Build all bundles
make bundles
# or
./scripts/build-bundles.sh

# Build specific bundle
make bundle-pcst
# or
./scripts/build-bundles.sh pcst

# Build PEBL runtime + all bundles
make em-opt-full
```

### Deploying Bundles

```bash
# Deploy all bundles to platform
./scripts/deploy-bundles.sh /path/to/PEBLOnlinePlatform-dev

# Deploy specific bundle
./scripts/deploy-bundles.sh /path/to/PEBLOnlinePlatform-dev pcst

# Dry run (preview without copying)
./scripts/deploy-bundles.sh --dry-run /path/to/PEBLOnlinePlatform-dev
```

## Configuration: bundle-config.json

### Structure

```json
{
  "version": "1.0",
  "description": "PEBL Bundle Build Configuration",

  "bundles": {
    "core": {
      "name": "pebl2",
      "type": "core",
      "description": "Main PEBL core bundle",
      "output_dir": "bin",
      "includes": [
        {
          "path": "upload-battery",
          "mount": "/usr/local/share/pebl2/battery",
          "description": "Standard test battery"
        }
      ],
      "excludes": ["*/data/*", "*~"],
      "options": {
        "use_preload_cache": true,
        "separate_metadata": true,
        "lz4": false
      }
    },

    "test_bundles": [
      {
        "name": "pcst",
        "type": "test_bundle",
        "description": "PEBL Cognitive Screening Test",
        "output_dir": "bin/test-bundles",
        "includes": [
          {
            "path": "battery/PCST",
            "mount": "/usr/local/share/pebl2/battery/PCST"
          }
        ],
        "options": {
          "use_preload_cache": true,
          "separate_metadata": true
        }
      }
    ]
  }
}
```

### Configuration Fields

#### Bundle Object

- **`name`** (string, required) - Bundle identifier (e.g., "pcst")
- **`type`** (string, required) - Bundle type: "core", "test_bundle", "private"
- **`description`** (string) - Human-readable description
- **`output_dir`** (string, required) - Output directory relative to pebl-dev root
- **`includes`** (array, required) - Files/directories to include
- **`excludes`** (array, optional) - Patterns to exclude
- **`options`** (object, required) - Emscripten file_packager options

#### Include Object

- **`path`** (string, required) - Source path relative to pebl-dev root
- **`mount`** (string, required) - Virtual filesystem mount point
- **`description`** (string, optional) - Description for build output

#### Options Object

- **`use_preload_cache`** (boolean) - Enable browser caching (recommended: true)
- **`separate_metadata`** (boolean) - Create separate .metadata file (recommended: true)
- **`lz4`** (boolean) - Enable LZ4 compression (requires PEBL built with -sLZ4)

## Adding New Test Bundles

To add a new test bundle:

1. **Edit `bundle-config.json`** and add to `test_bundles` array:

```json
{
  "name": "newtest",
  "type": "test_bundle",
  "description": "My New Test",
  "output_dir": "bin/test-bundles",
  "includes": [
    {
      "path": "battery/newtest",
      "mount": "/usr/local/share/pebl2/battery/newtest"
    }
  ],
  "options": {
    "use_preload_cache": true,
    "separate_metadata": true,
    "lz4": false
  }
}
```

2. **Build the bundle**:

```bash
./scripts/build-bundles.sh newtest
```

3. **Deploy to platform**:

```bash
./scripts/deploy-bundles.sh /path/to/PEBLOnlinePlatform-dev newtest
```

4. **Update platform configuration** (`PEBLOnlinePlatform-dev/config/test_catalog.json`):

```json
{
  "data_bundles": {
    "newtest": {
      "file": "newtest.data",
      "url": "/runtime/test-bundles/newtest.data",
      "base_url": "/runtime/test-bundles",
      "size_mb": 2.5,
      "description": "My New Test bundle"
    }
  },
  "tests": {
    "newtest": {
      "id": "newtest",
      "name": "My New Test",
      "directory": "newtest",
      "main_file": "newtest.pbl",
      "collection": "some_collection",
      "data_bundle": "separate"
    }
  }
}
```

That's it! No new build scripts needed.

## Output Files

For each bundle, three files are created:

- **`bundlename.data`** - Packed file data (5-20MB typically)
- **`bundlename.js`** - Loader script (~14KB)
- **`bundlename.js.metadata`** - File metadata (~4KB)

## Platform Integration

### Runtime Loading

The platform's launchers load bundles dynamically based on API responses:

1. Launcher calls `/api/getBundles.php?token=X&test=Y`
2. API returns bundle metadata including `base_url`
3. Launcher loads bundle scripts from specified locations
4. Emscripten `Module.locateFile` redirects data files to correct paths

### Directory Structure

```
PEBLOnlinePlatform-dev/
├── runtime/
│   ├── pebl2.js              # PEBL runtime
│   ├── pebl2.wasm
│   ├── pebl2.data            # Core bundle (future)
│   └── test-bundles/         # Separate test bundles
│       ├── pcst.data
│       ├── pcst.js
│       └── pcst.js.metadata
```

## Makefile Integration

The Makefile now separates runtime and bundle builds:

```makefile
# Build PEBL runtime only (no embedded data)
make em-opt

# Build all data bundles
make bundles

# Build runtime + all bundles
make em-opt-full

# Build specific bundle
make bundle-pcst

# List available bundles
make list-bundles
```

## Migration from Old System

### What Changed

**Before:**
- `em-opt-real` embedded all data with `--preload-file` flags
- Each test bundle needed its own build script
- 30+ second compile times even for data-only changes

**After:**
- `em-opt-real` builds runtime only (~10 seconds)
- Data bundles built separately (~2 seconds each)
- Single configuration file for all bundles
- Incremental builds: only rebuild changed bundles

### Old vs New Workflow

**Old workflow (for new test):**
1. Create `scripts/build-testname-bundle.sh`
2. Add Makefile target
3. Manually copy files to platform
4. Update platform config

**New workflow (for new test):**
1. Add entry to `bundle-config.json`
2. Run `./scripts/build-bundles.sh testname`
3. Run `./scripts/deploy-bundles.sh /platform/path testname`
4. Update platform config

### Migrating Existing Tests

To convert existing tests to separate bundles:

1. Remove test directory from `upload-battery` excludes (if applicable)
2. Add test to `bundle-config.json` test_bundles array
3. Update `test_catalog.json` to set `"data_bundle": "separate"`
4. Build and deploy new bundle
5. Test in platform launchers

## Troubleshooting

### Bundle Build Fails

```bash
# Check if source path exists
ls -la battery/TESTNAME

# Verify Emscripten is installed
ls libs/emsdk/upstream/emscripten/tools/file_packager.py

# Check bundle-config.json syntax
jq . bundle-config.json
```

### Deployment Fails

```bash
# Verify bundle was built
ls -lh bin/test-bundles/BUNDLENAME.*

# Check platform directory structure
ls /path/to/PEBLOnlinePlatform-dev/runtime/

# Use dry-run to preview
./scripts/deploy-bundles.sh --dry-run /platform/path BUNDLENAME
```

### Bundle Not Loading in Platform

1. Check browser console for 404 errors
2. Verify `base_url` in test_catalog.json matches actual file location
3. Confirm API returns correct bundle_metadata
4. Check launcher's `loadBundleScript` and `Module.locateFile` functions

## Best Practices

1. **Keep bundles focused** - One test per bundle for large tests (>5MB)
2. **Use excludes** - Exclude data directories, backup files, .git, etc.
3. **Test locally** - Always test bundle in platform before deployment
4. **Document changes** - Update test_catalog.json descriptions
5. **Version control** - Commit bundle-config.json changes with test changes

## Future Enhancements

- **User-uploaded test bundles** - Support custom upload directories
- **Automatic deployment** - CI/CD integration for bundle updates
- **Bundle versioning** - Track bundle versions for cache management
- **Dependency management** - Shared asset bundles across tests
- **Compression options** - Automatic LZ4 compression for large bundles

## Support

For issues or questions:
1. Check this documentation
2. Review `bundle-config.json` for examples
3. Run scripts with `--help` flag for usage information
4. Examine script output for error messages
