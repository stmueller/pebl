# PEBL Online Platform Deployment

This document describes how to deploy compiled PEBL files to the PEBLOnlinePlatform.

## Directory Structure

As of the reorganization, the PEBL project is split into two directories:

```
Dropbox/Research/pebl/
├── pebl_CL/                    # Main PEBL repository (git-tracked)
│   ├── src/                    # C++ source code
│   ├── bin/                    # Compiled binaries (pebl2, pebl2.html, etc.)
│   ├── emscripten/             # Emscripten build files
│   ├── upload-battery/         # Battery tests for upload
│   ├── Makefile                # Build configuration
│   └── deploy-to-online-platform.sh   # Deployment script
│
└── PEBLOnlinePlatform/         # Online platform (separate repository)
    ├── runtime/                # PEBL runtime files (deployed from pebl_CL)
    ├── battery/                # Battery tests (deployed from pebl_CL)
    ├── api/                    # PHP API endpoints
    ├── researcher/             # Researcher interface
    ├── public/                 # Public pages
    └── ...                     # Other platform files
```

## Build Process

### 1. Build PEBL for Emscripten

**Production Build:**
```bash
cd /path/to/pebl_CL
make em-opt
```

**Test Build (for debugging):**
```bash
cd /path/to/pebl_CL
make em-test
```

This compiles PEBL to WebAssembly and creates the following files in `bin/`:
- `pebl2.html` - Test runner HTML
- `pebl2.js` - JavaScript runtime
- `pebl2.wasm` - WebAssembly binary
- `pebl2.data` - Packaged files (battery, media, libraries)

### 2. Deploy to Online Platform

After building, deploy the files using the deployment script:

**Deploy production build:**
```bash
./deploy-to-online-platform.sh production
```

**Deploy test build:**
```bash
./deploy-to-online-platform.sh test
```

The script will:
- Copy compiled files from `bin/` to `../PEBLOnlinePlatform/runtime/`
- Copy launcher files from `emscripten/` to runtime
- Copy battery tests from `upload-battery/` to `../PEBLOnlinePlatform/battery/`
- Verify the target directory exists
- Display confirmation when complete

### 3. Make the script executable (first time only)

```bash
chmod +x deploy-to-online-platform.sh
```

## What Gets Deployed

### Production Build
```
../PEBLOnlinePlatform/
├── runtime/
│   ├── pebl2.html
│   ├── pebl2.js
│   ├── pebl2.wasm
│   ├── pebl2.data
│   └── pebl-launcher.html
└── battery/
    └── [all battery test files from upload-battery/]
```

### Test Build
```
../PEBLOnlinePlatform/
└── runtime/
    ├── pebl2-test.html
    ├── pebl2-test.js
    ├── pebl2-test.wasm
    └── pebl2-test.data
```

## Workflow

Typical development workflow:

1. Make changes to PEBL source code in `pebl_CL/src/`
2. Build: `make em-opt`
3. Deploy: `./deploy-to-online-platform.sh production`
4. Test in browser at the online platform URL
5. Commit PEBL changes to git (pebl_CL repository)
6. Commit platform changes to git (PEBLOnlinePlatform repository - if private)

## Troubleshooting

**Error: "Online platform directory not found"**
- Ensure `PEBLOnlinePlatform` exists as a sibling to `pebl_CL`
- Path should be: `../PEBLOnlinePlatform` relative to pebl_CL

**Error: "Production build not found"**
- Run `make em-opt` first to compile the production build
- Check that files exist in `bin/` directory

**Error: "Test build not found"**
- Run `make em-test` first to compile the test build
- Check that `bin/pebl2-test.html` exists

## Notes

- The Makefile no longer automatically copies files to the online platform
- This separation allows the PEBL compiler to remain in a public git repository
  while the online platform can be kept private
- The deployment script can be run separately after building
- Battery files are only copied during production deployment (not test)
