# Time-Tapping Task - PEBLOnlinePlatform Deployment Configuration

This document contains the complete configuration needed to deploy timetap to PEBLOnlinePlatform.

## Prerequisites

- ✅ Test fully migrated with Layout & Response System (layout-only mode)
- ✅ All 7 translations completed (en, pt, it, de, es, fr, nl)
- ✅ All parameter files created (6 response modes)
- ✅ Syntax validated with bin/pebl-validator
- ✅ Thoroughly tested in battery/
- ✅ Example data generated
- ✅ Bundle configuration added to bundle-config.json

## Bundle Configuration (COMPLETED)

Already added to `/home/smueller/Dropbox/Research/pebl/pebl/bundle-config.json`:

```json
{
  "name": "timetap",
  "type": "test_bundle",
  "description": "Time-Tapping Task - Motor timing task measuring self-paced tapping consistency (UTC-PAB test 19)",
  "output_dir": "bin/test-bundles",
  "includes": [
    {
      "path": "upload-battery/timetap",
      "mount": "/usr/local/share/pebl2/battery/timetap",
      "description": "Time-tapping test files with 7 translations and visual timer feedback"
    }
  ],
  "excludes": [
    "*/data/*",
    "*~",
    "*.pbl.png"
  ],
  "options": {
    "use_preload_cache": true,
    "separate_metadata": true,
    "lz4": false
  },
  "size_mb": 0.1,
  "catalog_entry": {
    "base_url": "/runtime/test-bundles",
    "tests": ["timetap"]
  }
}
```

---

## Required PEBLOnlinePlatform Configuration Files

### CONFIG FILE 1 of 4: library-tests.json

**Location**: `PEBLOnlinePlatform/config/library-tests.json`

**Add this entry**:

```json
{
  "id": "timetap",
  "name": "Time-Tapping Task",
  "main_file": "timetap.pbl",
  "path": "battery/timetap",
  "category": "Motor Function",
  "available": true,
  "estimated_minutes": 10
}
```

---

### CONFIG FILE 2 of 4: test-metadata/timetap.json

**Location**: `PEBLOnlinePlatform/config/test-metadata/timetap.json`

**Create this file**:

```json
{
  "version": "1.0",
  "test_id": "timetap",
  "test_name": "Time-Tapping Task",

  "overview": {
    "brief_description": "Motor timing task measuring self-paced tapping consistency across multiple trials",
    "task_description": "Participants tap at a self-paced even rate following a visual entrainment period. After seeing a flashing cross that demonstrates the target tapping rate, participants must maintain that rhythm for a sustained period (default 180 seconds per trial). The task assesses low-level motor timing ability and may be sensitive to fatigue, sleep deprivation, and motor control deficits.",
    "what_it_measures": "Motor timing consistency, rhythm maintenance, psychomotor speed, sustained motor control",
    "cognitive_domains": [
      "Motor Function",
      "Timing",
      "Psychomotor Speed"
    ],
    "duration_minutes": 10,
    "suitable_for": "Adults, clinical populations (fatigue, sleep deprivation, motor disorders), occupational testing"
  },

  "scientific_background": {
    "references": [
      {
        "citation": "Mueller, S. T., & Piper, B. J. (2014). The Psychology Experiment Building Language (PEBL) and PEBL Test Battery. Journal of Neuroscience Methods, 222, 250-259.",
        "doi": "10.1016/j.jneumeth.2013.10.024",
        "url": "https://doi.org/10.1016/j.jneumeth.2013.10.024",
        "type": "test_battery_description"
      }
    ],
    "validation_status": "Part of UTC-PAB (Unified Tri-Services Cognitive Performance Assessment Battery), Test 19",
    "original_source": "UTC-PAB",
    "pebl_version": "Based on UTC-PAB test 19, implemented in PEBL"
  },

  "data_output": {
    "summary": "Creates a single CSV file containing trial-by-trial tap timing data including training and test phases",
    "files": [
      {
        "filename": "timetap-example.csv",
        "type": "primary",
        "description": "Trial-by-trial tapping data with timestamps and inter-tap intervals"
      }
    ],
    "key_variables": [
      {
        "name": "subNum",
        "description": "Participant identifier code"
      },
      {
        "name": "trial",
        "description": "Trial number (X for training/entrainment, positive integers for test trials)"
      },
      {
        "name": "trialtype",
        "description": "Trial type indicator: -1 for training/entrainment flashes, 1 for actual test taps"
      },
      {
        "name": "targtime",
        "description": "Target inter-tap interval in milliseconds for this trial (e.g., 333, 666, 1000)"
      },
      {
        "name": "hits",
        "description": "Cumulative tap count within current trial"
      },
      {
        "name": "time",
        "description": "Absolute timestamp of tap in milliseconds since test start"
      },
      {
        "name": "rt",
        "description": "Inter-tap interval: time between current tap and previous tap in milliseconds"
      }
    ],
    "sample_data_url": "battery/timetap/data/timetap-example.csv"
  },

  "parameters": {
    "schema_file": "battery/timetap/params/timetap.pbl.schema.json",
    "key_parameters": [
      {
        "name": "taprates",
        "type": "array or string",
        "default": [333, 666, 1000],
        "description": "List of target inter-tap intervals in milliseconds, or 'random' to sample from range"
      },
      {
        "name": "numtrials",
        "type": "integer",
        "default": 3,
        "description": "Number of test trials (ignored if taprates is a list)"
      },
      {
        "name": "trialtime",
        "type": "integer",
        "default": 180,
        "description": "Duration of each trial in seconds"
      },
      {
        "name": "mintaptime",
        "type": "integer",
        "default": 333,
        "description": "Minimum tap interval when using random mode (ms)"
      },
      {
        "name": "maxtaptime",
        "type": "integer",
        "default": 1000,
        "description": "Maximum tap interval when using random mode (ms)"
      },
      {
        "name": "trainingflashes",
        "type": "integer",
        "default": 10,
        "description": "Number of visual flashes during entrainment/training period"
      },
      {
        "name": "responsemode",
        "type": "string",
        "default": "spacebar",
        "description": "Response input method: spacebar, leftclick, clicktarget, touchscreen, userselect"
      }
    ]
  },

  "assets": {
    "screenshots": [
      "battery/timetap/timetap.pbl.png"
    ],
    "videos": [],
    "demo_url": null
  },

  "platform_info": {
    "category": "Motor Function",
    "subcategory": "Timing",
    "browser_compatibility": {
      "chrome": true,
      "firefox": true,
      "safari": true,
      "edge": true,
      "mobile": true
    },
    "input_methods": [
      "keyboard",
      "mouse",
      "touch"
    ],
    "accessibility_features": [
      "Multi-modal response options",
      "User-selectable response keys",
      "Touch screen support",
      "Large visual timer feedback"
    ],
    "known_issues": [
      "Timing precision in browsers is ~16ms (acceptable for this task which measures consistency over hundreds of milliseconds)",
      "Visual timer updates every 500ms to minimize overhead"
    ],
    "requirements": {
      "minimum_screen_size": "1024x768",
      "audio_required": false,
      "fullscreen_recommended": false,
      "internet_required_during_test": false
    }
  },

  "related_tests": [
    {
      "test_id": "fingerTapping",
      "relationship": "Alternative motor speed task",
      "description": "Measures maximum tapping rate rather than rhythm consistency"
    },
    {
      "test_id": "srt",
      "relationship": "Related reaction time task",
      "description": "Measures simple reaction time without rhythm component"
    }
  ],

  "notes": {
    "administration_notes": "Ensure participants understand they should maintain a steady rhythm throughout each trial, not just tap as fast as possible. The entrainment period helps establish the target rhythm before the test phase begins.",
    "scoring_notes": "Consistency is measured by variability in inter-tap intervals. Lower variability indicates better motor timing control. Compare actual intervals to target intervals to assess accuracy.",
    "interpretation_notes": "Poor performance (high variability or deviation from target) may indicate fatigue, sleep deprivation, motor control deficits, or medication effects. Performance typically degrades with longer trial durations.",
    "clinical_relevance": "Sensitive to fatigue states, sleep deprivation, medication effects, and motor disorders. Part of UTC-PAB used for operational readiness assessment."
  },

  "tags": [
    "motor",
    "timing",
    "rhythm",
    "psychomotor",
    "UTC-PAB",
    "fatigue-sensitive",
    "clinical",
    "migrated-2.3",
    "layout-only"
  ],

  "sources": {
    "original_implementation": "UTC-PAB Test 19",
    "pebl_implementation_date": "2008",
    "pebl_2.3_migration_date": "2026-01-13",
    "author": "Shane T. Mueller",
    "maintainer": "PEBL Project",
    "license": "GPL-2.0",
    "repository": "https://github.com/stmueller/pebl-exp-battery"
  }
}
```

---

### CONFIG FILE 3 of 4: test_catalog.json (data_bundles section)

**Location**: `PEBLOnlinePlatform/config/test_catalog.json`

**Add to "data_bundles" section**:

```json
"timetap": {
  "file": "timetap.data",
  "url": "/runtime/test-bundles/timetap.data",
  "size_mb": 0.1,
  "description": "Time-Tapping Task - Motor timing task measuring self-paced tapping consistency (UTC-PAB test 19)",
  "tests": [
    "timetap"
  ],
  "base_url": "/runtime/test-bundles"
}
```

---

### CONFIG FILE 4 of 4: test_catalog.json (tests section)

**Location**: `PEBLOnlinePlatform/config/test_catalog.json`

**Add to "tests" section**:

```json
"timetap": {
  "id": "timetap",
  "name": "Time-Tapping Task",
  "directory": "timetap",
  "main_file": "timetap.pbl",
  "screenshot": "battery/timetap/timetap.pbl.png",
  "version": "1.0",
  "collection": "motor_cognition",
  "data_bundle": "timetap",
  "platform": [
    "web",
    "native"
  ],
  "visibility": "public",
  "min_tier": "free",
  "tags": [
    "motor",
    "timing",
    "rhythm",
    "psychomotor",
    "UTC-PAB",
    "fatigue-sensitive",
    "clinical",
    "migrated-2.3",
    "layout-only"
  ],
  "size_mb": 0.1,
  "duration_minutes": 10,
  "copyright_status": "open",
  "description": "Motor timing task where participants maintain a self-paced even tapping rhythm for sustained periods. After visual entrainment at the target rate, participants must continue tapping at that tempo while a visual timer provides progress feedback. Measures motor timing consistency and may be sensitive to fatigue, sleep deprivation, and motor control deficits. Based on UTC-PAB test 19.",
  "citation": "Mueller, S. T., & Piper, B. J. (2014). The Psychology Experiment Building Language (PEBL) and PEBL Test Battery. Journal of Neuroscience Methods, 222, 250-259."
}
```

---

## Deployment Steps

### Step 1: Copy Test Files to PEBLOnlinePlatform

```bash
# Copy entire test directory from battery/ (NOT upload-battery/)
cp -r battery/timetap/ PEBLOnlinePlatform/battery/timetap/

# Verify all required files present
ls PEBLOnlinePlatform/battery/timetap/timetap.pbl
ls PEBLOnlinePlatform/battery/timetap/timetap.pbl.png
ls PEBLOnlinePlatform/battery/timetap/params/*.par.json
ls PEBLOnlinePlatform/battery/timetap/translations/*.json
```

### Step 2: Copy Example Data

**CRITICAL**: Example data must be placed **directly** in the `data/` directory (NOT in `data/example/` subdirectory).

```bash
# Copy example data with standard naming
cp upload-battery/timetap/data/timetap-example.csv \
   PEBLOnlinePlatform/battery/timetap/data/timetap-example.csv

# Verify example data location
ls PEBLOnlinePlatform/battery/timetap/data/timetap-example.csv

# Should NOT exist (subdirectory structure)
ls PEBLOnlinePlatform/battery/timetap/data/example/ 2>/dev/null  # Should not exist
```

### Step 3: Update All Four Configuration Files

1. Edit `PEBLOnlinePlatform/config/library-tests.json` - add entry from CONFIG FILE 1
2. Create `PEBLOnlinePlatform/config/test-metadata/timetap.json` - full content from CONFIG FILE 2
3. Edit `PEBLOnlinePlatform/config/test_catalog.json` - add data_bundles entry from CONFIG FILE 3
4. Edit `PEBLOnlinePlatform/config/test_catalog.json` - add tests entry from CONFIG FILE 4

### Step 4: Build Bundle

```bash
# Build the timetap bundle
cd /home/smueller/Dropbox/Research/pebl/pebl
# Use appropriate bundle build command (depends on PEBLOnlinePlatform build system)
```

### Step 5: Verification

```bash
# 1. Verify test_catalog.json has both entries
jq '.data_bundles.timetap' PEBLOnlinePlatform/config/test_catalog.json
jq '.tests.timetap' PEBLOnlinePlatform/config/test_catalog.json

# 2. Verify library registration
jq '.tests[] | select(.id == "timetap")' PEBLOnlinePlatform/config/library-tests.json

# 3. Verify metadata completeness
jq '.data_output.key_variables | length' PEBLOnlinePlatform/config/test-metadata/timetap.json
# Should show 7 (number of CSV columns)

# 4. Verify example data exists
ls PEBLOnlinePlatform/battery/timetap/data/timetap-example.csv

# 5. Verify screenshot exists
ls PEBLOnlinePlatform/battery/timetap/timetap.pbl.png

# 6. Verify bundle was created
ls bin/test-bundles/timetap.data
ls bin/test-bundles/timetap.js.metadata
```

---

## Test Characteristics Summary

- **Type**: Motor timing task
- **Duration**: ~10 minutes (3 trials × 180 seconds + instructions + entrainment)
- **Response modes**: 6 modes (spacebar, leftclick, clicktarget, touchscreen, userselect, auto)
- **Translations**: 7 languages (en, pt, it, de, es, fr, nl)
- **Bundle size**: ~0.1 MB
- **PEBL version**: 2.3 (migrated January 2026)
- **Migration type**: Layout-only (uses CreateLayout but not WaitForLayoutResponse for trials)
- **Semantic responses**: "gonogo" (single response key)

---

## Data Output Format

**File**: `timetap-<SUBNUM>.csv`

**Columns**:
1. `subNum` - Participant ID
2. `trial` - Trial number (X for training, 1-3 for test)
3. `trialtype` - Type (-1 = training flash, 1 = test tap)
4. `targtime` - Target inter-tap interval (ms)
5. `hits` - Cumulative tap count in trial
6. `time` - Absolute timestamp (ms)
7. `rt` - Inter-tap interval (ms)

**Example rows**:
```
test-01,X,-1,333,1,5234,5234        # Training flash
test-01,X,-1,333,2,5567,333         # Training flash
test-01,1,1,333,0,10123,10123       # First test tap (trial 1)
test-01,1,1,333,1,10450,327         # Second test tap
test-01,1,1,333,2,10788,338         # Third test tap
```

---

## Known Issues and Limitations

1. **Timing precision**: Browser timing is ~16ms precision (acceptable for this task)
2. **Timer updates**: Visual timer updates every 500ms to minimize overhead
3. **Asyncify overhead**: May add small delays, monitored via event queue (stays under 4 events)

---

## Migration Notes

**Date**: 2026-01-13
**Migrated to**: PEBL 2.3 with Layout & Response System (layout-only mode)

**Changes Made**:
- Added `InitializeUpload()` and `UploadFile()` for online data transfer
- Integrated Layout & Response System (CreateLayout) for consistent UI
- Configured `responsesemantics = "gonogo"` for single-key response
- Added visual timer bar with self-chaining event system (RegisterEvent/UpdateTimer)
- Implemented trial progress counter in header
- Created 7 complete translations with TRIALCOUNTER string
- Created 6 response mode parameter files
- Fixed parameter files with taprates as proper JSON arrays
- Validated with bin/pebl-validator

**Testing**:
- ✅ Tested with multiple parameter files (clicktarget, spacebar, userselect)
- ✅ Tested in multiple languages (en, nl, de, fr, etc.)
- ✅ Visual timer updates correctly during trials
- ✅ Trial counter displays properly
- ✅ Event queue stays under 4 events (verified with debug output)
- ✅ Data output validated

**Browser Compatibility**:
- Chrome: Tested and working
- Firefox: Expected to work (standard PEBL)
- Safari: Expected to work (standard PEBL)
- Edge: Expected to work (standard PEBL)
- Mobile: Touch mode available

---

## Contact

For questions about this deployment configuration:
- PEBL Project: https://pebl.sourceforge.net
- Test Battery Repository: https://github.com/stmueller/pebl-exp-battery
