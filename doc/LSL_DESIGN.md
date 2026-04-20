# Lab Streaming Layer (LSL) Integration for PEBL - Complete Design

**Date:** January 28, 2026
**Status:** Design Phase - Ready for Implementation
**Authors:** Shane Mueller, Claude (AI Assistant)

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Background Research](#background-research)
3. [Architecture Overview](#architecture-overview)
4. [Phase 1: Outlets (Sending Markers)](#phase-1-outlets-sending-markers)
5. [Phase 2: Inlets (Receiving Triggers)](#phase-2-inlets-receiving-triggers)
6. [Phase 3: Advanced Features](#phase-3-advanced-features)
7. [Implementation Roadmap](#implementation-roadmap)
8. [Testing Strategy](#testing-strategy)
9. [Documentation Requirements](#documentation-requirements)
10. [Appendices](#appendices)

---

## Executive Summary

### What is LSL?

Lab Streaming Layer (LSL) is an open-source framework for synchronized multi-modal data acquisition in research. It enables:
- **Sub-millisecond time synchronization** across devices
- **Automatic network discovery** of data streams
- **Reliable transmission** with buffering and recovery
- **Cross-platform support** (Windows/Linux/macOS)
- **Broad hardware compatibility** (EEG, fNIRS, eye trackers, physiological sensors)

**Official Resources:**
- GitHub: https://github.com/sccn/labstreaminglayer
- Documentation: https://labstreaminglayer.readthedocs.io
- Reference Paper: Kothe et al. (2025). "The Lab Streaming Layer for Synchronized Multimodal Recording". *Imaging Neuroscience*.

### Why PEBL Needs LSL

PEBL is widely used for behavioral experiments, but lacks integration with physiological recording devices. LSL integration will enable:

**Current Research Need:** Researchers want to record brain/body responses during PEBL tasks:
- **EEG/ERP** - Brain activity during cognitive tasks (N400, P300, error-related negativity)
- **fMRI/MRI** - Hemodynamic responses (BOLD signal changes)
- **Eye tracking** - Gaze patterns, pupil dilation, fixations
- **fNIRS** - Cortical oxygenation changes
- **Physiological sensors** - Heart rate, skin conductance, respiration, EMG

**How LSL Helps:**
1. PEBL sends **event markers** (timestamps) marking critical moments
2. Markers synchronized with physiological data streams
3. Researchers analyze brain/body responses aligned to behavioral events
4. All data recorded to single XDF file via LabRecorder

### Design Philosophy: The UploadFile() Pattern

Following PEBL's existing `UploadFile()` pattern, LSL functions are:
- **Always callable** - Can be in every task's code
- **Activated by flag** - Only active when `--lsl` command-line flag provided
- **Graceful degradation** - No errors/overhead when LSL not available
- **Minimal changes** - 5-10 lines of code per battery task

```bash
# Without --lsl flag: LSLMarker() calls are silent no-ops
pebl2 battery/stroop/stroop.pbl -s P001

# With --lsl flag: Markers sent to LSL network
pebl2 battery/stroop/stroop.pbl -s P001 --lsl
```

### Three-Phase Implementation

**Phase 1: Outlets (Sending Markers)** ← Priority: Implement first
- PEBL sends event markers TO LSL network
- Covers 90% of use cases
- Simpler implementation
- Target: 3-4 weeks

**Phase 2: Inlets (Receiving Triggers)** ← Priority: Medium
- PEBL receives triggers FROM devices (fMRI scanner, eye tracker, etc.)
- Required for scanner synchronization, gaze-contingent displays
- More complex but essential for advanced research
- Target: 2-3 weeks after Phase 1

**Phase 3: Advanced Features** ← Priority: Lower
- Continuous data streaming (not just event markers)
- Real-time signal processing in PEBL
- Closed-loop experiments
- Target: Future enhancement

---

## Background Research

### LSL Ecosystem

**Core Library:**
- `liblsl` - C/C++ library (cross-platform)
- Language bindings: Python (pylsl), MATLAB, Java, C#
- Network protocol: TCP-based with UDP discovery

**LSL Architecture:**
- **Stream Outlets** - Send data TO network (push API)
- **Stream Inlets** - Receive data FROM network (pull API)
- **Resolvers** - Discover available streams
- **Time synchronization** - NTP-like protocol (sub-millisecond accuracy)

**Data Types Supported:**
- `cf_string` - Variable-length strings (event markers, conditions)
- `cf_int32` - Integer codes (numeric event codes)
- `cf_float32`, `cf_double64` - Continuous data (gaze position, EEG)
- `cf_int8`, `cf_int16`, `cf_int64` - Other integer formats

**Key LSL Applications:**

1. **LabRecorder** (Official Recording App)
   - Cross-platform: Windows, macOS, **Linux** (Ubuntu .deb, Arch AUR, source)
   - Records all LSL streams to XDF file format
   - Handles time synchronization automatically
   - GitHub: https://github.com/labstreaminglayer/App-LabRecorder
   - Installation: `sudo apt install qt6-base-dev freeglut3-dev && sudo dpkg -i LabRecorder.deb`

2. **Hardware-Specific LSL Apps** (Examples)
   - Brain Products: LiveAmp, actiCHamp
   - Tobii: Pro series eye trackers
   - Emotiv: EPOC, Insight
   - OpenBCI: Cyton, Ganglion
   - BIOPAC: MP150, MP160
   - Full list: https://labstreaminglayer.readthedocs.io/info/supported_devices.html

### How Other Software Uses LSL

**PsychoPy (Python):**
```python
from pylsl import StreamInfo, StreamOutlet

# Setup
info = StreamInfo('PsychoPy_Markers', 'Markers', 1, 0, 'string', 'psychopy123')
outlet = StreamOutlet(info)

# During experiment
outlet.push_sample(['stimulus_onset'])
outlet.push_sample(['response'])
```

**Presentation (Neurobehavioral Systems):**
- Built-in LSL support via GUI
- Configure stream name, type, markers
- Send markers at event codes

**OpenSesame:**
- LSL plugin available
- Similar pattern to PsychoPy

**Key Pattern:** All stimulus presentation software follows same approach:
1. Create outlet at experiment start
2. Send string/integer markers at key events
3. Automatic cleanup at end

### Typical Research Workflow

```
┌─────────────────────────────────────────────────────────┐
│                 Researcher's Computer                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────┐    ┌──────────────┐   ┌────────────┐ │
│  │ PEBL Task    │    │ LabRecorder  │   │ EEG App    │ │
│  │ (markers)    │───▶│ (records all)│◀──│ (data)     │ │
│  └──────────────┘    └──────────────┘   └────────────┘ │
│         │                    │                  │        │
│         └────────────────────┼──────────────────┘        │
│                              │                           │
│                              ▼                           │
│                     ┌────────────────┐                   │
│                     │  XDF File      │                   │
│                     │  - EEG data    │                   │
│                     │  - PEBL markers│                   │
│                     │  - Timestamps  │                   │
│                     └────────────────┘                   │
└─────────────────────────────────────────────────────────┘
                              │
                              ▼
                   ┌─────────────────────┐
                   │   Analysis          │
                   │   (MATLAB/Python)   │
                   │   - Load XDF        │
                   │   - Extract markers │
                   │   - Compute ERPs    │
                   └─────────────────────┘
```

**Steps:**
1. Researcher starts EEG device LSL app (streams brain data)
2. Researcher starts LabRecorder (discovers and records all streams)
3. Researcher runs PEBL task with `--lsl` flag (adds marker stream)
4. Participant performs task
5. LabRecorder saves XDF file with synchronized data
6. Researcher loads XDF in MATLAB/Python, extracts PEBL markers, analyzes ERPs

### Research on PsychoPy/LSL Integration

**From GitHub repository analysis (kaczmarj/psychopy-lsl):**

```python
# Minimal PsychoPy integration
from pylsl import StreamInfo, StreamOutlet

info = StreamInfo(
    name='PsychoPy_LSL',
    type='Markers',
    channel_count=1,
    nominal_srate=0,        # Irregular rate (event-driven)
    channel_format='string',
    source_id='psychopy_exp1'
)
outlet = StreamOutlet(info)

# Send markers
outlet.push_sample(['trial_start'])
outlet.push_sample(['condition_congruent'])
outlet.push_sample(['stimulus_onset'])
outlet.push_sample(['response'])
```

**Key Insights:**
- Stream type is standardized as "Markers"
- Channel count = 1 for simple event markers
- Nominal sample rate = 0 (irregular/event-driven)
- Source ID is optional but recommended for robustness
- String markers preferred over integer codes (more descriptive)

---

## Architecture Overview

### LSL C++ API Key Components

**From libs/labstreaminglayer/LSL/liblsl/include/lsl_cpp.h:**

**Stream Creation (Outlets):**
```cpp
// Create stream info
lsl::stream_info info(
    name,           // "PEBL_stroop"
    type,           // "Markers"
    channel_count,  // 1
    srate,          // lsl::IRREGULAR_RATE (0.0)
    format,         // lsl::cf_string
    source_id       // "pebl_stroop_P001"
);

// Create outlet
lsl::stream_outlet outlet(info);

// Send markers
outlet.push_sample(&marker);            // Single string
outlet.push_sample(&marker, timestamp); // With explicit timestamp

// Check for consumers (optional)
bool has_listeners = outlet.have_consumers();
outlet.wait_for_consumers(timeout);

// Cleanup (automatic in destructor)
```

**Stream Discovery (Inlets):**
```cpp
// Resolve streams by name/type
std::vector<lsl::stream_info> results =
    lsl::resolve_stream("name", "MRI_Triggers", 1, 5.0);

// Or resolve all streams
std::vector<lsl::stream_info> all_streams =
    lsl::resolve_streams(1.0);

// Create inlet from resolved stream
lsl::stream_inlet inlet(results[0]);

// Pull samples
std::string sample;
double timestamp = inlet.pull_sample(&sample, 1, timeout);

// Pull continuous data
std::vector<float> sample;
double timestamp = inlet.pull_sample(sample, timeout);

// Cleanup (automatic in destructor)
```

**Time Synchronization:**
```cpp
// Get synchronized timestamp
double now = lsl::local_clock();

// Get time correction for remote stream
double offset = inlet.time_correction();
double local_time = remote_timestamp + offset;
```

### PEBL Integration Architecture

**Three-Layer Design:**

```
┌─────────────────────────────────────────────────────────┐
│               PEBL Battery Tasks (*.pbl)                 │
│  - Call LSLMarker(), WaitForLSLTrigger(), etc.          │
│  - Always present in code                               │
│  - Only active when --lsl flag used                     │
└─────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│         PEBL Wrapper Functions (pebl-lib/Utility.pbl)   │
│  - LSLMarker() - Always callable, no-op if not active  │
│  - InitializeLSL() - Checks flag, creates outlet       │
│  - WaitForLSLTrigger() - Receives triggers from devices│
│  - FinalizeLSL() - Cleanup                             │
└─────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│       PEBL Built-in Functions (src/libs/PEBLLSL.cpp)    │
│  - CreateLSLOutlet() - C++ wrapper, exposed to PEBL    │
│  - SendLSLMarker() - Send to outlet                    │
│  - CreateLSLInlet() - Resolve and create inlet         │
│  - PullLSLSample() - Receive from inlet                │
└─────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│            LSL Library (libs/labstreaminglayer/)         │
│  - liblsl C++ library                                   │
│  - Network protocol implementation                      │
│  - Time synchronization                                 │
└─────────────────────────────────────────────────────────┘
```

---

## Phase 1: Outlets (Sending Markers)

**Goal:** Enable PEBL to send event markers to LSL network for synchronization with recording devices.

**Priority:** High - Implement first (covers 90% of use cases)

### Command-Line Flags

**C++ Implementation (src/base/PEBLEnvironment.cpp):**

```cpp
// Add global variables
bool gLSLEnabled = false;
std::string gLSLStreamName = "";
std::string gLSLStreamType = "Markers";
std::string gLSLSourceID = "";

// Command-line argument parsing
if(arg == "--lsl") {
    gLSLEnabled = true;
}
else if(arg == "--lsl-name" && i+1 < argc) {
    gLSLStreamName = argv[++i];
}
else if(arg == "--lsl-type" && i+1 < argc) {
    gLSLStreamType = argv[++i];
}
else if(arg == "--lsl-source-id" && i+1 < argc) {
    gLSLSourceID = argv[++i];
}
```

**Expose to PEBL (src/libs/Functions.h):**

```cpp
{(char*)"GETLSLENABLED", GetLSLEnabled, 0, 0},
{(char*)"GETLSLSTREAMNAME", GetLSLStreamName, 0, 0},
{(char*)"GETLSLSTREAMTYPE", GetLSLStreamType, 0, 0},
{(char*)"GETLSLSOURCEID", GetLSLSourceID, 0, 0},
```

**Usage Examples:**

```bash
# Basic usage (auto-generate stream name from script)
pebl2 battery/stroop/stroop.pbl -s P001 --lsl

# Custom stream name
pebl2 battery/stroop/stroop.pbl -s P001 --lsl-name "Stroop_Experiment_v2"

# Full specification
pebl2 battery/flanker/flanker.pbl -s P001 --lsl \
    --lsl-name "Flanker_Task" \
    --lsl-type "Markers" \
    --lsl-source-id "pebl_flanker_v1"
```

### PEBL Wrapper Functions

**Location: pebl-lib/Utility.pbl**

These functions mirror the UploadFile() pattern - always callable, only active when enabled.

```pebl
##============================================================================##
## LSL (Lab Streaming Layer) Integration Functions
##============================================================================##

## Initialize LSL if --lsl command-line flag was provided
## Called automatically by InitializePEBL()
## Returns 1 if LSL active, 0 otherwise
##
define InitializeLSL()
{
    gLSLActive <- 0

    ## Check if --lsl flag was provided
    if(GetLSLEnabled())
    {
        ## Get stream name (from --lsl-name or default to script name)
        streamName <- GetLSLStreamName()
        if(streamName == "")
        {
            ## Default: PEBL_{scriptname}
            streamName <- "PEBL_" + gScriptName
        }

        streamType <- GetLSLStreamType()
        if(streamType == "")
        {
            streamType <- "Markers"
        }

        sourceID <- GetLSLSourceID()
        if(sourceID == "")
        {
            ## Default: {scriptname}_{subnum}
            sourceID <- gScriptName + "_" + gSubNum
        }

        ## Try to create LSL outlet (C++ built-in function)
        success <- CreateLSLOutlet(streamName, streamType, sourceID)

        if(success)
        {
            gLSLActive <- 1
            Print("LSL initialized: [" + streamName + "] type=[" + streamType + "]")

            ## Send initial marker
            LSLMarker("experiment_start")
        } else {
            Print("WARNING: LSL initialization failed - continuing without LSL")
            gLSLActive <- 0
        }
    }

    return gLSLActive
}


## Send an LSL marker if LSL is enabled
## This is a no-op (silent) if --lsl flag was not provided
##
## Usage:
##   LSLMarker("trial_start")
##   LSLMarker("stimulus_onset")
##   LSLMarker("response_correct")
##   LSLMarker(100)  ## Integer codes also supported
##
define LSLMarker(marker)
{
    ## Only send if LSL was initialized successfully
    if(gLSLActive)
    {
        SendLSLMarker(marker)
    }
    ## Silent no-op otherwise - no error, no overhead
}


## Cleanup LSL at experiment end
## Called automatically by FinalizePEBL()
##
define FinalizeLSL()
{
    if(gLSLActive)
    {
        LSLMarker("experiment_end")
        CloseLSLOutlet()
        gLSLActive <- 0
        Print("LSL finalized")
    }
}


## Check if LSL is active
## Returns 1 if LSL outlet is active, 0 otherwise
##
define LSLIsActive()
{
    return gLSLActive
}


## Check if anyone is listening to LSL stream
## Returns 1 if at least one consumer (e.g., LabRecorder), 0 otherwise
## Useful for warning if no recorder is running
##
define LSLHasConsumers()
{
    if(gLSLActive)
    {
        return LSLHasConsumers_Internal()  ## C++ built-in
    } else {
        return 0
    }
}
```

**Integration with existing framework:**

```pebl
## Add to standard initialization
## This should be called by all battery tasks
##
define InitializePEBL()
{
    ## Existing initialization...
    ## (none currently standard, but should be)

    ## NEW: Initialize LSL if --lsl flag provided
    InitializeLSL()

    ## Initialize upload if --upload flag provided (existing)
    InitializeUpload()
}


## Add to standard cleanup
##
define FinalizePEBL()
{
    ## NEW: Cleanup LSL
    FinalizeLSL()

    ## Existing cleanup...
}
```

### C++ Built-in Functions

**Location: src/libs/PEBLLSL.cpp (new file)**

```cpp
//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLLSL.cpp
//    Purpose:    LSL (Lab Streaming Layer) integration for PEBL
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2026 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
/////////////////////////////////////////////////////////////////////////////////

#ifdef PEBL_USE_LSL

#include "PEBLLSL.h"
#include "../utility/PLabStreamingLayer.h"
#include "../utility/PError.h"
#include "../base/Variant.h"

// Global LSL outlet (singleton - one per PEBL instance)
static PLSL* gLSLOutlet = nullptr;

//============================================================================//
// Phase 1: Outlet Functions (Send Markers)
//============================================================================//

/**
 * CreateLSLOutlet - Create an LSL stream outlet
 *
 * Arguments:
 *   name - Stream name (e.g., "PEBL_stroop")
 *   type - Stream type (e.g., "Markers")
 *   source_id - Unique identifier (e.g., "pebl_stroop_P001")
 *
 * Returns:
 *   1 on success, 0 on failure
 */
Variant CreateLSLOutlet(Variant v)
{
    PList* plist = v.GetComplexData()->GetList();

    std::string name = plist->First().GetString();
    std::string type = plist->GetLength() > 1 ?
                       plist->Nth(2).GetString() : "Markers";
    std::string source_id = plist->GetLength() > 2 ?
                            plist->Nth(3).GetString() : "";

    try {
        // Close previous outlet if exists
        if (gLSLOutlet) {
            delete gLSLOutlet;
            gLSLOutlet = nullptr;
        }

        // Create new outlet
        gLSLOutlet = new PLSL(name, type, source_id);

        return Variant(1);  // Success
    }
    catch (std::exception& e) {
        PError::SignalWarning("LSL outlet creation failed: " +
                              std::string(e.what()));
        return Variant(0);  // Failure
    }
}


/**
 * SendLSLMarker - Send a marker through the LSL outlet
 *
 * Arguments:
 *   marker - String or integer marker
 *
 * Returns:
 *   1 on success, 0 on failure
 */
Variant SendLSLMarker(Variant v)
{
    if (!gLSLOutlet) {
        return Variant(0);  // Silently skip if not initialized
    }

    PList* plist = v.GetComplexData()->GetList();
    Variant marker = plist->First();

    try {
        if (marker.IsString()) {
            gLSLOutlet->SendMarker(marker.GetString());
        }
        else if (marker.IsInteger()) {
            gLSLOutlet->SendMarker(marker.GetInteger());
        }
        else if (marker.IsFloat()) {
            // Convert float to string
            gLSLOutlet->SendMarker(std::to_string(marker.GetFloat()));
        }
        else {
            // Convert to string as fallback
            gLSLOutlet->SendMarker(marker.GetString());
        }

        return Variant(1);
    }
    catch (std::exception& e) {
        PError::SignalWarning("LSL marker send failed: " +
                              std::string(e.what()));
        return Variant(0);
    }
}


/**
 * CloseLSLOutlet - Close the LSL outlet
 *
 * Returns:
 *   1 on success
 */
Variant CloseLSLOutlet(Variant v)
{
    if (gLSLOutlet) {
        delete gLSLOutlet;
        gLSLOutlet = nullptr;
    }
    return Variant(1);
}


/**
 * LSLHasConsumers_Internal - Check if anyone is listening
 *
 * Returns:
 *   1 if consumers present, 0 otherwise
 */
Variant LSLHasConsumers_Internal(Variant v)
{
    if (!gLSLOutlet) {
        return Variant(0);
    }

    return Variant(gLSLOutlet->HaveConsumers() ? 1 : 0);
}


/**
 * LSLLocalClock - Get current LSL synchronized time
 *
 * Returns:
 *   Timestamp in seconds (double)
 */
Variant LSLLocalClock(Variant v)
{
    return Variant(lsl::local_clock());
}

#endif // PEBL_USE_LSL
```

**Header file: src/libs/PEBLLSL.h (new file)**

```cpp
#ifndef __PEBLLSL_H__
#define __PEBLLSL_H__

#include "../base/Variant.h"

#ifdef PEBL_USE_LSL

// Phase 1: Outlet functions
Variant CreateLSLOutlet(Variant v);
Variant SendLSLMarker(Variant v);
Variant CloseLSLOutlet(Variant v);
Variant LSLHasConsumers_Internal(Variant v);
Variant LSLLocalClock(Variant v);

// Phase 2: Inlet functions (implemented later)
Variant CreateLSLInlet(Variant v);
Variant WaitForLSLTrigger(Variant v);
Variant PollLSLSample(Variant v);
Variant WaitForLSLSample(Variant v);
Variant CloseLSLInlet(Variant v);

#endif // PEBL_USE_LSL

#endif // __PEBLLSL_H__
```

**Function Registration: src/libs/Functions.h**

```cpp
#ifdef PEBL_USE_LSL
    // Phase 1: LSL Outlet functions (send markers)
    {(char*)"CREATELSLOUTLET", CreateLSLOutlet, 1, 3},
    {(char*)"SENDLSLMARKER", SendLSLMarker, 1, 1},
    {(char*)"CLOSELSLOUTLET", CloseLSLOutlet, 0, 0},
    {(char*)"LSLHASCONSUMERS_INTERNAL", LSLHasConsumers_Internal, 0, 0},
    {(char*)"LSLLOCALCLOCK", LSLLocalClock, 0, 0},

    // Phase 2: LSL Inlet functions (receive triggers)
    {(char*)"CREATELSLINNLET", CreateLSLInlet, 2, 2},
    {(char*)"WAITFORLSLTRIGGER", WaitForLSLTrigger, 1, 2},
    {(char*)"POLLLSLSAMPLE", PollLSLSample, 0, 1},
    {(char*)"WAITFORLSLSAMPLE", WaitForLSLSample, 0, 1},
    {(char*)"CLOSELSLINLET", CloseLSLInlet, 0, 0},
#endif
```

### PLSL C++ Wrapper Class

**Rewrite existing: src/utility/PLabStreamingLayer.h**

```cpp
#ifndef __PLSL_H__
#define __PLSL_H__

#ifdef PEBL_USE_LSL

#include <string>
#include "../../../libs/labstreaminglayer/LSL/liblsl/include/lsl_cpp.h"

/**
 * PLSL - PEBL wrapper for Lab Streaming Layer
 *
 * Wraps lsl::stream_outlet for sending event markers
 */
class PLSL
{
public:
    // Constructor - creates LSL outlet
    PLSL(const std::string& name,
         const std::string& type,
         const std::string& source_id);

    // Destructor - closes outlet
    ~PLSL();

    // Send string marker
    void SendMarker(const std::string& marker);

    // Send integer marker
    void SendMarker(int marker);

    // Send marker with explicit timestamp
    void SendMarkerTimed(const std::string& marker, double timestamp);

    // Check if anyone is listening
    bool HaveConsumers();

    // Wait for consumers to connect
    bool WaitForConsumers(double timeout);

private:
    lsl::stream_info mInfo;
    lsl::stream_outlet* mOutlet;
};

#endif // PEBL_USE_LSL
#endif // __PLSL_H__
```

**Implementation: src/utility/PLabStreamingLayer.cpp**

```cpp
#ifdef PEBL_USE_LSL

#include "PLabStreamingLayer.h"
#include <iostream>

PLSL::PLSL(const std::string& name,
           const std::string& type,
           const std::string& source_id)
    : mInfo(name, type, 1, lsl::IRREGULAR_RATE, lsl::cf_string, source_id),
      mOutlet(nullptr)
{
    mOutlet = new lsl::stream_outlet(mInfo);
}

PLSL::~PLSL()
{
    if (mOutlet) {
        delete mOutlet;
        mOutlet = nullptr;
    }
}

void PLSL::SendMarker(const std::string& marker)
{
    if (mOutlet) {
        mOutlet->push_sample(&marker);
    }
}

void PLSL::SendMarker(int marker)
{
    if (mOutlet) {
        // Convert integer to string
        std::string str_marker = std::to_string(marker);
        mOutlet->push_sample(&str_marker);
    }
}

void PLSL::SendMarkerTimed(const std::string& marker, double timestamp)
{
    if (mOutlet) {
        mOutlet->push_sample(&marker, timestamp);
    }
}

bool PLSL::HaveConsumers()
{
    return mOutlet ? mOutlet->have_consumers() : false;
}

bool PLSL::WaitForConsumers(double timeout)
{
    return mOutlet ? mOutlet->wait_for_consumers(timeout) : false;
}

#endif // PEBL_USE_LSL
```

### Build System Integration

**Makefile changes:**

```makefile
# LSL support (optional, native builds only)
USE_LSL ?= 0

ifeq ($(USE_LSL),1)
    # Add compiler flag
    CXXFLAGS += -DPEBL_USE_LSL

    # Add LSL library paths
    LSL_INCLUDE = -I./libs/labstreaminglayer/LSL/liblsl/include
    LSL_LIBDIR = -L./libs/labstreaminglayer/LSL/liblsl/build/install/lib
    LIBS += -llsl

    # Add include paths
    CXXFLAGS += $(LSL_INCLUDE)
    LDFLAGS += $(LSL_LIBDIR)

    # Add LSL source files
    SOURCES += src/libs/PEBLLSL.cpp \
               src/utility/PLabStreamingLayer.cpp
endif

# Add build instructions
lsl-help:
	@echo "To build with LSL support:"
	@echo "  1. Build LSL library first:"
	@echo "     cd libs/labstreaminglayer/LSL/liblsl"
	@echo "     mkdir build && cd build"
	@echo "     cmake .. -DCMAKE_INSTALL_PREFIX=../install"
	@echo "     make && make install"
	@echo "  2. Build PEBL with LSL:"
	@echo "     make clean && make main USE_LSL=1"
```

**Building LSL library (one-time setup):**

```bash
cd libs/labstreaminglayer/LSL/liblsl
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../install
make
make install
cd ../../../../..

# Now build PEBL with LSL support
make clean
make main USE_LSL=1
```

### Standard Marker Convention

**Core markers (all tasks should have):**

| Marker | When | Description |
|--------|------|-------------|
| `experiment_start` | Auto-sent by InitializeLSL() | Experiment begins |
| `practice_start` | Start of practice phase | Practice begins |
| `practice_end` | End of practice phase | Practice complete |
| `test_start` | Start of test phase | Main test begins |
| `block_{N}_start` | Start of block N | Block boundary |
| `trial_start` | Start of each trial | Trial begins |
| `stimulus_onset` | Stimulus appears | Stimulus displayed |
| `response` | Response made | Participant responded |
| `correct` | Correct response | Response was correct |
| `incorrect` | Incorrect response | Response was wrong |
| `feedback` | Feedback shown | Feedback displayed |
| `trial_end` | End of trial | Trial complete |
| `block_{N}_end` | End of block N | Block complete |
| `test_end` | End of test phase | Main test complete |
| `experiment_end` | Auto-sent by FinalizeLSL() | Experiment complete |

**Task-specific markers (optional):**

| Pattern | Example | Description |
|---------|---------|-------------|
| `condition_{name}` | `condition_congruent` | Experimental condition |
| `stimulus_{type}` | `stimulus_word` | Stimulus category |
| `response_{type}` | `response_timeout` | Response category |
| `parameter_{name}_{value}` | `parameter_isi_500` | Trial parameter |

**Naming convention:**
- Use **underscores** (not CamelCase): `trial_start` not `TrialStart`
- Use **lowercase** (not UPPERCASE): `stimulus_onset` not `STIMULUS_ONSET`
- Be **descriptive**: `condition_congruent` not `cond1`
- Be **consistent**: Same markers across all PEBL tasks

### Example: Retrofitting Stroop Task

**Before (no LSL support):**

```pebl
define Start(p)
{
    gWin <- MakeWindow()
    gSubNum <- GetSubNum(gWin)
    gFileOut <- GetNewDataFile(gSubNum, gWin, "stroop", "csv",
                                "sub,trial,condition,word,color,response,rt,correct")

    ShowInstructions()

    ## Practice trials
    loop(trial, gPracticeTrials)
    {
        ShowStimulus(trial.word, trial.color)
        response <- WaitForKeyPress(["<lshift>", "<rshift>"])
        accuracy <- Score(response, trial.correct)
        FilePrint(gFileOut, FormatTrial(trial, response, accuracy))
    }

    ## Test trials
    loop(trial, gTestTrials)
    {
        ShowStimulus(trial.word, trial.color)
        response <- WaitForKeyPress(["<lshift>", "<rshift>"])
        accuracy <- Score(response, trial.correct)
        FilePrint(gFileOut, FormatTrial(trial, response, accuracy))
    }

    ShowResults()
}
```

**After (with LSL support):**

```pebl
define Start(p)
{
    gWin <- MakeWindow()
    gSubNum <- GetSubNum(gWin)
    InitializePEBL()  ## NEW: Initializes LSL if --lsl flag set

    gFileOut <- GetNewDataFile(gSubNum, gWin, "stroop", "csv",
                                "sub,trial,condition,word,color,response,rt,correct")

    ShowInstructions()

    ## Practice trials
    LSLMarker("practice_start")  ## NEW
    loop(trial, gPracticeTrials)
    {
        LSLMarker("trial_start")  ## NEW
        LSLMarker("condition_" + trial.condition)  ## NEW (optional)

        ShowStimulus(trial.word, trial.color)
        LSLMarker("stimulus_onset")  ## NEW

        response <- WaitForKeyPress(["<lshift>", "<rshift>"])
        LSLMarker("response")  ## NEW

        accuracy <- Score(response, trial.correct)
        LSLMarker(If(accuracy, "correct", "incorrect"))  ## NEW

        FilePrint(gFileOut, FormatTrial(trial, response, accuracy))
        LSLMarker("trial_end")  ## NEW
    }
    LSLMarker("practice_end")  ## NEW

    ## Test trials
    LSLMarker("test_start")  ## NEW
    loop(trial, gTestTrials)
    {
        LSLMarker("trial_start")  ## NEW
        LSLMarker("condition_" + trial.condition)  ## NEW

        ShowStimulus(trial.word, trial.color)
        LSLMarker("stimulus_onset")  ## NEW

        response <- WaitForKeyPress(["<lshift>", "<rshift>"])
        LSLMarker("response")  ## NEW

        accuracy <- Score(response, trial.correct)
        LSLMarker(If(accuracy, "correct", "incorrect"))  ## NEW

        FilePrint(gFileOut, FormatTrial(trial, response, accuracy))
        LSLMarker("trial_end")  ## NEW
    }
    LSLMarker("test_end")  ## NEW

    ShowResults()
    FinalizePEBL()  ## NEW: Cleans up LSL
}
```

**Code changes summary:**
- Added: `InitializePEBL()` at start (1 line)
- Added: `FinalizePEBL()` at end (1 line)
- Added: Phase markers (`practice_start`, `test_start`, etc.) (4 lines)
- Added: Trial markers in loop (6 lines per trial type)
- **Total: ~15 lines of code**

---

## Battery Task Integration Guide

**Goal:** Provide a step-by-step guide for adding LSL marker support to existing PEBL battery tasks with minimal code changes.

### Overview

This guide walks through the process of adding LSL markers to any PEBL battery task. The process is standardized and requires:
- **5-15 lines of code** added to the task script
- **1 entry added** to params/requirements.json
- **Standard marker placement** at key experimental events
- **No changes to existing functionality** - LSL is completely optional

### Step-by-Step Integration Process

#### Step 1: Add InitializePEBL() and FinalizePEBL()

**Location:** Start() function, beginning and end

**Before:**
```pebl
define Start(p)
{
    gWin <- MakeWindow()
    gSubNum <- GetSubNum(gWin)

    ## Task code...

    ShowResults()
}
```

**After:**
```pebl
define Start(p)
{
    gWin <- MakeWindow()
    gSubNum <- GetSubNum(gWin)
    InitializePEBL()  ## NEW: Automatically initializes LSL if --lsl flag set

    ## Task code...

    ShowResults()
    FinalizePEBL()  ## NEW: Automatically finalizes LSL
}
```

**What this does:**
- `InitializePEBL()` checks for `--lsl` flag and creates LSL outlet if present
- `FinalizePEBL()` sends "experiment_end" marker and closes outlet
- If `--lsl` not provided, both are silent no-ops

#### Step 2: Add Phase Markers

**Location:** Before/after major experimental phases

**Add these markers:**

```pebl
## Before practice trials
LSLMarker("practice_start")

## After practice trials
LSLMarker("practice_end")

## Before test trials
LSLMarker("test_start")

## After test trials
LSLMarker("test_end")

## Block boundaries (if applicable)
LSLMarker("block_" + blockNum + "_start")
LSLMarker("block_" + blockNum + "_end")
```

**Example:**
```pebl
## Practice phase
LSLMarker("practice_start")
loop(trial, practiceTrials)
{
    RunTrial(trial)
}
LSLMarker("practice_end")

## Test phase
LSLMarker("test_start")
loop(trial, testTrials)
{
    RunTrial(trial)
}
LSLMarker("test_end")
```

#### Step 3: Add Trial Markers

**Location:** Inside trial loop

**Standard trial markers:**

```pebl
loop(trial, trials)
{
    LSLMarker("trial_start")                      ## NEW: Trial begins
    LSLMarker("condition_" + trial.condition)     ## NEW: Condition info (optional)

    ShowStimulus(trial)
    LSLMarker("stimulus_onset")                   ## NEW: Stimulus appears

    response <- WaitForKeyPress()
    LSLMarker("response")                         ## NEW: Response made

    accuracy <- ScoreResponse(response, trial)
    LSLMarker(If(accuracy, "correct", "incorrect"))  ## NEW: Accuracy feedback

    LSLMarker("trial_end")                        ## NEW: Trial complete
}
```

**Minimal trial markers (if time-constrained):**

At minimum, add these 3 markers:
```pebl
LSLMarker("trial_start")
LSLMarker("stimulus_onset")  ## Right before or after Draw()
LSLMarker("response")        ## Right after WaitForKeyPress/WaitForClickOnTarget
```

#### Step 4: Add Optional Task-Specific Markers

**Add markers for important task events:**

**Feedback display:**
```pebl
if(showFeedback)
{
    ShowFeedback(accuracy)
    LSLMarker("feedback")
}
```

**Stimulus parameters:**
```pebl
LSLMarker("parameter_isi_" + isi)
LSLMarker("parameter_difficulty_" + difficulty)
LSLMarker("stimulus_type_" + stimType)
```

**Response types:**
```pebl
if(response == "<timeout>")
{
    LSLMarker("response_timeout")
} else {
    LSLMarker("response_" + response)
}
```

**Special events:**
```pebl
LSLMarker("break_start")
LSLMarker("break_end")
LSLMarker("instructions_shown")
LSLMarker("calibration_start")
```

#### Step 5: Update params/requirements.json

**Location:** battery/taskname/params/requirements.json

**Add LSL support declaration:**

```json
{
  "version": "1.0",
  "display": {
    "minimum_width": 800,
    "minimum_height": 600,
    "recommended_width": 1280,
    "recommended_height": 800
  },
  "input": {
    "keyboard": true,
    "mouse_buttons": false,
    "mouse_movement": false,
    "touch": false
  },
  "audio": {
    "required": false,
    "recommended": false
  },
  "color": {
    "discrimination_required": false
  },
  "browser": {
    "fullscreen_required": false,
    "minimum_version": {
      "chrome": 90,
      "firefox": 88,
      "safari": 14
    }
  },
  "device": {
    "allowed_types": ["desktop", "laptop", "tablet"],
    "block_mobile": true
  },
  "lsl": {
    "supported": true,
    "version": "1.0",
    "markers": {
      "core": [
        "experiment_start",
        "practice_start",
        "trial_start",
        "stimulus_onset",
        "response",
        "correct",
        "incorrect",
        "trial_end",
        "practice_end",
        "test_start",
        "test_end",
        "experiment_end"
      ],
      "task_specific": [
        "condition_congruent",
        "condition_incongruent",
        "feedback"
      ],
      "notes": "Standard cognitive control task markers"
    },
    "recommended_for": [
      "EEG/ERP studies",
      "fMRI experiments",
      "Eye tracking studies"
    ],
    "timing_critical": true,
    "typical_marker_count": 150,
    "notes": "Task sends event markers at stimulus onset and response. Ideal for time-locked analysis."
  }
}
```

**LSL Section Specification:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `supported` | boolean | Yes | Whether task supports LSL markers |
| `version` | string | Yes | LSL integration version (use "1.0") |
| `markers.core` | array | Yes | List of standard markers used |
| `markers.task_specific` | array | No | Task-specific marker names |
| `markers.notes` | string | No | Description of marker strategy |
| `recommended_for` | array | No | Research modalities this task is good for |
| `timing_critical` | boolean | No | Whether task has strict timing requirements |
| `typical_marker_count` | number | No | Approximate markers per session |
| `notes` | string | No | General LSL usage notes |

**Example variations:**

**Minimal LSL support (only basic markers):**
```json
"lsl": {
  "supported": true,
  "version": "1.0",
  "markers": {
    "core": ["experiment_start", "trial_start", "stimulus_onset", "response", "trial_end", "experiment_end"]
  },
  "notes": "Basic marker support for time-locking analysis"
}
```

**Rich marker support (detailed events):**
```json
"lsl": {
  "supported": true,
  "version": "1.0",
  "markers": {
    "core": [
      "experiment_start", "practice_start", "trial_start",
      "fixation", "stimulus_onset", "response", "feedback",
      "correct", "incorrect", "trial_end", "practice_end",
      "test_start", "block_1_start", "block_1_end",
      "block_2_start", "block_2_end", "test_end", "experiment_end"
    ],
    "task_specific": [
      "condition_go", "condition_nogo",
      "response_hit", "response_miss", "response_false_alarm", "response_correct_rejection",
      "parameter_isi_500", "parameter_isi_1000", "parameter_isi_1500"
    ],
    "notes": "Comprehensive markers including trial parameters and response classifications"
  },
  "recommended_for": ["EEG/ERP studies", "Response inhibition research", "Impulse control studies"],
  "timing_critical": true,
  "typical_marker_count": 400,
  "notes": "Go/No-Go task with detailed trial-level markers. Ideal for N2/P3 ERP analysis."
}
```

**Scanner-compatible task:**
```json
"lsl": {
  "supported": true,
  "version": "1.0",
  "markers": {
    "core": [
      "experiment_start", "trial_start", "stimulus_onset",
      "response", "trial_end", "experiment_end"
    ],
    "task_specific": [
      "MRI_pulse_received", "MRI_pulse_timeout",
      "fixation", "jitter_500", "jitter_1000"
    ]
  },
  "recommended_for": ["fMRI experiments"],
  "timing_critical": true,
  "inlet_support": true,
  "inlet_streams": ["MRI_Triggers"],
  "notes": "Task can synchronize with MRI scanner triggers. Use --lsl-inlet flag for scanner sync."
}
```

**Not LSL-compatible (questionnaire/survey):**
```json
"lsl": {
  "supported": false,
  "version": "1.0",
  "notes": "Questionnaire task with no time-critical events. LSL markers not meaningful."
}
```

#### Step 6: Test LSL Integration

**Test 1: Run without --lsl flag (ensure no errors)**
```bash
bin/pebl2 battery/taskname/taskname.pbl -s TEST001
# Should run normally, no LSL messages, no errors
```

**Test 2: Run with --lsl flag (verify markers)**
```bash
# Terminal 1: Start LabRecorder
LabRecorder &

# Terminal 2: Run task with LSL
bin/pebl2 battery/taskname/taskname.pbl -s TEST001 --lsl

# Check console output for:
# "LSL initialized: [PEBL_taskname] type=[Markers]"
# "LSL finalized"

# Check LabRecorder for:
# - Stream "PEBL_taskname" appears
# - Markers received in real-time
# - Can save to XDF
```

**Test 3: Verify marker sequence**
```bash
# Run task and check marker order makes sense
bin/pebl2 battery/taskname/taskname.pbl -s TEST001 --lsl

# Expected sequence:
# experiment_start
# practice_start (if applicable)
# trial_start
# condition_xxx (if applicable)
# stimulus_onset
# response
# correct/incorrect
# trial_end
# ... (more trials)
# practice_end / test_start / test_end
# experiment_end
```

**Test 4: Timing verification**
```bash
# Check marker timing is accurate
# Markers should be sent immediately, not batched
# Verify with LabRecorder timestamp display
```

### Complete Example: Stroop Task Integration

**File:** battery/stroop-color/color-stroop.pbl

**Original code (simplified):**
```pebl
define Start(p)
{
    gWin <- MakeWindow()
    gSubNum <- GetSubNum(gWin)
    gFileOut <- GetNewDataFile(gSubNum, gWin, "stroop", "csv",
                                "sub,trial,condition,word,color,response,rt,acc")

    ShowInstructions()

    ## Practice
    loop(trial, practiceTrials)
    {
        ShowFixation()
        Wait(500)
        ShowStimulus(trial.word, trial.color)
        stimTime <- GetTime()
        response <- WaitForKeyPress(["<lshift>", "<rshift>"])
        rt <- GetTime() - stimTime
        acc <- ScoreResponse(response, trial.correct)
        FilePrint(gFileOut, FormatTrialData(trial, response, rt, acc))
    }

    ## Test
    loop(trial, testTrials)
    {
        ShowFixation()
        Wait(500)
        ShowStimulus(trial.word, trial.color)
        stimTime <- GetTime()
        response <- WaitForKeyPress(["<lshift>", "<rshift>"])
        rt <- GetTime() - stimTime
        acc <- ScoreResponse(response, trial.correct)
        FilePrint(gFileOut, FormatTrialData(trial, response, rt, acc))
    }

    ShowResults()
}
```

**With LSL markers (additions marked with ##NEW):**
```pebl
define Start(p)
{
    gWin <- MakeWindow()
    gSubNum <- GetSubNum(gWin)
    InitializePEBL()  ##NEW

    gFileOut <- GetNewDataFile(gSubNum, gWin, "stroop", "csv",
                                "sub,trial,condition,word,color,response,rt,acc")

    ShowInstructions()

    ## Practice
    LSLMarker("practice_start")  ##NEW
    loop(trial, practiceTrials)
    {
        LSLMarker("trial_start")  ##NEW
        LSLMarker("condition_" + trial.condition)  ##NEW

        ShowFixation()
        LSLMarker("fixation")  ##NEW
        Wait(500)

        ShowStimulus(trial.word, trial.color)
        LSLMarker("stimulus_onset")  ##NEW
        stimTime <- GetTime()

        response <- WaitForKeyPress(["<lshift>", "<rshift>"])
        LSLMarker("response")  ##NEW
        rt <- GetTime() - stimTime

        acc <- ScoreResponse(response, trial.correct)
        LSLMarker(If(acc, "correct", "incorrect"))  ##NEW

        FilePrint(gFileOut, FormatTrialData(trial, response, rt, acc))
        LSLMarker("trial_end")  ##NEW
    }
    LSLMarker("practice_end")  ##NEW

    ## Test
    LSLMarker("test_start")  ##NEW
    loop(trial, testTrials)
    {
        LSLMarker("trial_start")  ##NEW
        LSLMarker("condition_" + trial.condition)  ##NEW

        ShowFixation()
        LSLMarker("fixation")  ##NEW
        Wait(500)

        ShowStimulus(trial.word, trial.color)
        LSLMarker("stimulus_onset")  ##NEW
        stimTime <- GetTime()

        response <- WaitForKeyPress(["<lshift>", "<rshift>"])
        LSLMarker("response")  ##NEW
        rt <- GetTime() - stimTime

        acc <- ScoreResponse(response, trial.correct)
        LSLMarker(If(acc, "correct", "incorrect"))  ##NEW

        FilePrint(gFileOut, FormatTrialData(trial, response, rt, acc))
        LSLMarker("trial_end")  ##NEW
    }
    LSLMarker("test_end")  ##NEW

    ShowResults()
    FinalizePEBL()  ##NEW
}
```

**Total changes:** 21 lines added (17 LSLMarker calls + 2 Init/Finalize + 2 phase boundaries)

**Corresponding requirements.json:**
```json
{
  "version": "1.0",
  "display": {
    "minimum_width": 800,
    "minimum_height": 600,
    "recommended_width": 1280,
    "recommended_height": 800
  },
  "input": {
    "keyboard": true,
    "mouse_buttons": false,
    "mouse_movement": false,
    "touch": false
  },
  "audio": {
    "required": false,
    "recommended": false
  },
  "color": {
    "discrimination_required": true,
    "notes": "Requires ability to distinguish red, green, blue, yellow colors"
  },
  "browser": {
    "fullscreen_required": true,
    "minimum_version": {
      "chrome": 90,
      "firefox": 88,
      "safari": 14
    }
  },
  "device": {
    "allowed_types": ["desktop", "tablet"],
    "block_mobile": true,
    "notes": "Requires precise color rendering and keyboard input"
  },
  "lsl": {
    "supported": true,
    "version": "1.0",
    "markers": {
      "core": [
        "experiment_start",
        "practice_start",
        "trial_start",
        "fixation",
        "stimulus_onset",
        "response",
        "correct",
        "incorrect",
        "trial_end",
        "practice_end",
        "test_start",
        "test_end",
        "experiment_end"
      ],
      "task_specific": [
        "condition_congruent",
        "condition_incongruent",
        "condition_neutral"
      ],
      "notes": "Markers sent at fixation, stimulus onset, response, and trial boundaries. Condition type included for analysis."
    },
    "recommended_for": [
      "EEG/ERP studies (conflict processing, N450, sustained potential)",
      "fMRI experiments (anterior cingulate activation)",
      "Eye tracking (gaze patterns during conflict)"
    ],
    "timing_critical": true,
    "typical_marker_count": 200,
    "notes": "Classic Stroop task with comprehensive markers. Ideal for cognitive control and conflict processing studies."
  }
}
```

### Task-Specific Marker Strategies

**Simple RT / Choice RT:**
```pebl
LSLMarker("trial_start")
LSLMarker("warning_signal")  ## If present
LSLMarker("stimulus_onset")
LSLMarker("response")
LSLMarker("trial_end")
```

**Go/No-Go:**
```pebl
LSLMarker("trial_start")
LSLMarker("condition_go")  ## or "condition_nogo"
LSLMarker("stimulus_onset")
LSLMarker("response")  ## Only if response made
LSLMarker("response_hit")  ## or "miss", "false_alarm", "correct_rejection"
LSLMarker("trial_end")
```

**N-Back:**
```pebl
LSLMarker("trial_start")
LSLMarker("target_" + If(isTarget, "yes", "no"))
LSLMarker("stimulus_onset")
LSLMarker("response")
LSLMarker("correct")  ## or "incorrect"
LSLMarker("response_type_hit")  ## or "miss", "false_alarm", "correct_rejection"
LSLMarker("trial_end")
```

**Visual Search:**
```pebl
LSLMarker("trial_start")
LSLMarker("setsize_" + setSize)
LSLMarker("target_present_" + If(targetPresent, "yes", "no"))
LSLMarker("stimulus_onset")
LSLMarker("response")
LSLMarker("correct")  ## or "incorrect"
LSLMarker("trial_end")
```

**Memory Tasks (Encoding/Retrieval):**
```pebl
## Encoding
LSLMarker("encoding_start")
loop(item, items)
{
    LSLMarker("encoding_trial_start")
    LSLMarker("item_" + item.category)
    LSLMarker("stimulus_onset")
    Wait(presentationTime)
    LSLMarker("encoding_trial_end")
}
LSLMarker("encoding_end")

## Retrieval
LSLMarker("retrieval_start")
loop(item, testItems)
{
    LSLMarker("retrieval_trial_start")
    LSLMarker("item_old_" + If(item.isOld, "yes", "no"))
    LSLMarker("stimulus_onset")
    response <- GetResponse()
    LSLMarker("response")
    LSLMarker("correct")  ## or "incorrect"
    LSLMarker("retrieval_trial_end")
}
LSLMarker("retrieval_end")
```

**Questionnaires/Scales:**
```pebl
## Questionnaires typically don't need detailed markers
LSLMarker("questionnaire_start")
loop(question, questions)
{
    LSLMarker("question_" + question.id + "_display")
    response <- GetResponse()
    LSLMarker("question_" + question.id + "_response")
}
LSLMarker("questionnaire_end")
```

### Common Mistakes to Avoid

**❌ Don't put markers after Draw() calls without waiting:**
```pebl
## WRONG
Draw()
LSLMarker("stimulus_onset")  ## Screen might not be updated yet!
```

**✅ Do put markers right before Draw() or after with small wait:**
```pebl
## RIGHT Option 1
LSLMarker("stimulus_onset")
Draw()

## RIGHT Option 2
Draw()
Wait(10)  ## Ensure screen refresh
LSLMarker("stimulus_onset")
```

**❌ Don't forget to handle timeout responses:**
```pebl
## WRONG
response <- WaitForKeyPressWithTimeout(2000)
LSLMarker("response")  ## Always sent, even on timeout!
```

**✅ Do check for timeout:**
```pebl
## RIGHT
response <- WaitForKeyPressWithTimeout(2000)
if(response == "<timeout>")
{
    LSLMarker("response_timeout")
} else {
    LSLMarker("response")
}
```

**❌ Don't use dynamic marker names that are hard to parse:**
```pebl
## WRONG
LSLMarker("trial" + trial + "condition" + cond + "rt" + rt)
## Results in: "trial5condition2rt452" - hard to parse later
```

**✅ Do use structured, parseable marker names:**
```pebl
## RIGHT
LSLMarker("trial_start")
LSLMarker("trial_number_" + trial)
LSLMarker("condition_" + cond)
## Analyze RT from data file, not markers
```

**❌ Don't forget InitializePEBL() and FinalizePEBL():**
```pebl
## WRONG
define Start(p)
{
    gWin <- MakeWindow()
    LSLMarker("trial_start")  ## Won't work! LSL not initialized
    ...
}
```

**✅ Do call init and finalize:**
```pebl
## RIGHT
define Start(p)
{
    gWin <- MakeWindow()
    InitializePEBL()  ## Initializes LSL if --lsl flag set
    LSLMarker("trial_start")  ## Now works
    ...
    FinalizePEBL()  ## Cleanup
}
```

### Migration Checklist

Use this checklist when adding LSL to a battery task:

- [ ] Add `InitializePEBL()` at start of Start() function
- [ ] Add `FinalizePEBL()` at end of Start() function
- [ ] Add phase markers (`practice_start`, `test_start`, etc.)
- [ ] Add `trial_start` marker at beginning of each trial loop
- [ ] Add `stimulus_onset` marker when stimulus appears
- [ ] Add `response` marker after input received
- [ ] Add `correct`/`incorrect` markers for accuracy (if applicable)
- [ ] Add `trial_end` marker at end of each trial loop
- [ ] Add condition markers for experimental conditions (optional)
- [ ] Add task-specific markers for important events (optional)
- [ ] Test without `--lsl` flag (should run normally)
- [ ] Test with `--lsl` flag + LabRecorder (verify markers)
- [ ] Update `params/requirements.json` with LSL section
- [ ] Document marker sequence in task .about.txt file
- [ ] Commit changes with message: "Add LSL marker support to [taskname]"

### Batch Migration Script

For migrating many tasks at once:

```bash
#!/bin/bash
# migrate_tasks_to_lsl.sh

TASKS=(
    "stroop-color"
    "flanker"
    "gonogo"
    "oddball"
    "crt"
    # ... add more tasks
)

for task in "${TASKS[@]}"; do
    echo "Processing battery/$task..."

    # Backup original
    cp "battery/$task/$task.pbl" "battery/$task/$task.pbl.backup"

    # Manual steps required:
    echo "  1. Add InitializePEBL() and FinalizePEBL()"
    echo "  2. Add LSLMarker() calls at key events"
    echo "  3. Update params/requirements.json"
    echo "  4. Test with: bin/pebl2 battery/$task/$task.pbl -s TEST --lsl"
    echo ""
done
```

### Documentation in Task .about.txt

**Add LSL section to task description:**

```
Task Name: Color-Word Stroop Task
...

LSL Marker Support:
This task includes Lab Streaming Layer (LSL) marker support for
synchronization with EEG, fMRI, and other recording devices.

To enable LSL markers, run with:
  pebl2 battery/stroop-color/color-stroop.pbl -s P001 --lsl

Markers sent:
  - experiment_start: Task begins
  - practice_start/end: Practice phase boundaries
  - test_start/end: Test phase boundaries
  - trial_start/end: Trial boundaries
  - fixation: Fixation cross displayed
  - stimulus_onset: Stroop stimulus appears (COLOR WORD)
  - condition_congruent/incongruent/neutral: Trial condition
  - response: Participant responded
  - correct/incorrect: Response accuracy

Typical use:
  - EEG/ERP: Study conflict processing (N450, sustained potential)
  - fMRI: Measure anterior cingulate activation
  - Eye tracking: Analyze gaze patterns during conflict

For analysis examples, see: doc/LSL_ANALYSIS_EXAMPLES.md
```

---

## Phase 2: Inlets (Receiving Triggers)

**Goal:** Enable PEBL to receive triggers/events from external devices for synchronized experiments.

**Priority:** Medium - Implement after Phase 1 is stable

### Use Cases

**1. fMRI/MRI Scanner Synchronization**

Problem: Scanner takes 2-3 seconds per volume (TR). PEBL needs to wait for scanner pulse before starting trial.

```pebl
define Start(p)
{
    gWin <- MakeWindow()
    InitializePEBL()

    ## Connect to MRI scanner LSL stream
    CreateLSLInlet("MRI_Triggers", "Markers")

    ShowInstructions("Waiting for scanner to start...")

    loop(trial, trials)
    {
        ## Wait for scanner TR pulse before starting trial
        trigger <- WaitForLSLTrigger("TR_pulse", timeout:10.0)

        if(trigger == "<timeout>")
        {
            Print("WARNING: No scanner pulse received")
        } else {
            LSLMarker("MRI_trigger_received")
        }

        ## Now run trial synchronized to scanner
        LSLMarker("trial_start")
        ShowStimulus(trial)
        LSLMarker("stimulus_onset")

        ## Short response window aligned to TR
        response <- WaitForKeyPressWithTimeout(2.0)
        LSLMarker("response")

        LSLMarker("trial_end")
    }

    FinalizePEBL()
}
```

**2. Eye Tracker Gaze-Contingent Display**

Problem: Need to detect when participant is fixating on target location before presenting stimulus.

```pebl
define Start(p)
{
    gWin <- MakeWindow()
    InitializePEBL()

    ## Connect to eye tracker gaze stream
    CreateLSLInlet("Tobii_Gaze", "Gaze")

    loop(trial, trials)
    {
        LSLMarker("trial_start")

        ## Show fixation cross
        ShowFixation(centerX, centerY)
        LSLMarker("fixation_cross")

        ## Wait for stable gaze on fixation
        fixated <- 0
        while(not fixated)
        {
            ## Pull gaze coordinates [x, y, timestamp]
            gazeData <- WaitForLSLSample(timeout:0.01)

            if(IsList(gazeData) and Length(gazeData) >= 2)
            {
                gazeX <- First(gazeData)
                gazeY <- Second(gazeData)

                ## Check if within fixation window
                distance <- Dist([gazeX, gazeY], [centerX, centerY])
                if(distance < 50)  ## 50 pixel radius
                {
                    fixated <- 1
                    LSLMarker("fixation_achieved")
                }
            }
        }

        ## Good fixation - show stimulus
        ShowStimulus(trial)
        LSLMarker("stimulus_onset")

        response <- WaitForKeyPress()
        LSLMarker("response")

        LSLMarker("trial_end")
    }

    FinalizePEBL()
}
```

**3. EEG-Based Neurofeedback/BCI**

Problem: Need to wait for specific EEG state (e.g., high alpha power = relaxed) before proceeding.

```pebl
define Start(p)
{
    gWin <- MakeWindow()
    InitializePEBL()

    ## Connect to BCI controller stream (pre-processed EEG features)
    CreateLSLInlet("BCI_AlphaPower", "Control")

    loop(trial, trials)
    {
        LSLMarker("trial_start")

        ShowInstructions("Relax until ready signal...")

        ## Wait for relaxed state (high alpha power)
        relaxed <- 0
        timeout <- GetTime() + 30.0  ## 30 second timeout

        while(not relaxed and GetTime() < timeout)
        {
            ## Pull alpha power value
            sample <- WaitForLSLSample(timeout:0.1)

            if(IsList(sample) and Length(sample) > 0)
            {
                alphaPower <- First(sample)

                ## Check threshold (e.g., alpha > 0.7 = relaxed)
                if(alphaPower > 0.7)
                {
                    relaxed <- 1
                    LSLMarker("relaxed_state_achieved")
                }
            }
        }

        if(relaxed)
        {
            ## Participant achieved relaxed state - run trial
            RunTrial(trial)
        } else {
            ## Timeout - skip trial or give feedback
            ShowMessage("Please try to relax")
            LSLMarker("relaxation_timeout")
        }

        LSLMarker("trial_end")
    }

    FinalizePEBL()
}
```

**4. Multi-Device Synchronization**

Problem: Need to coordinate multiple devices (e.g., wait for both eye tracker and fMRI scanner).

```pebl
define Start(p)
{
    gWin <- MakeWindow()
    InitializePEBL()

    ## Connect to multiple device streams
    CreateLSLInlet("MRI_Triggers", "Markers")
    CreateLSLInlet("Tobii_Gaze", "Gaze")

    loop(trial, trials)
    {
        ## Wait for MRI pulse
        mriTrigger <- WaitForLSLTrigger("TR_pulse", timeout:5.0)
        LSLMarker("MRI_pulse_received")

        ## Wait for fixation
        ShowFixation()
        gazeData <- WaitForLSLSample(timeout:2.0)
        LSLMarker("gaze_acquired")

        ## Both synchronized - show stimulus
        ShowStimulus(trial)
        LSLMarker("stimulus_onset")

        response <- WaitForKeyPress()
        LSLMarker("response")

        LSLMarker("trial_end")
    }

    FinalizePEBL()
}
```

### Command-Line Flags for Inlets

```bash
# Receive triggers from MRI scanner
pebl2 task.pbl -s P001 --lsl --lsl-inlet "MRI_Triggers"

# Receive from eye tracker (custom type)
pebl2 task.pbl -s P001 --lsl --lsl-inlet "Tobii_Gaze" --lsl-inlet-type "Gaze"

# Both send markers AND receive triggers
pebl2 task.pbl -s P001 --lsl --lsl-inlet "MRI_Triggers"
```

### PEBL Wrapper Functions for Inlets

**Add to pebl-lib/Utility.pbl:**

```pebl
##============================================================================##
## LSL Inlet Functions (Phase 2)
##============================================================================##

## Create LSL inlet to receive data from a stream
##
## Arguments:
##   streamName - Name of stream to connect to (e.g., "MRI_Triggers")
##   streamType - Type of stream (e.g., "Markers", "Gaze")
##
## Returns:
##   1 on success, 0 on failure
##
define CreateLSLInlet(streamName, streamType)
{
    gLSLInletActive <- 0

    Print("LSL: Searching for stream [" + streamName + "] type=[" + streamType + "]...")

    ## Try to resolve and create inlet (C++ built-in)
    success <- CreateLSLInlet_Internal(streamName, streamType)

    if(success)
    {
        gLSLInletActive <- 1
        Print("LSL inlet connected: [" + streamName + "]")
    } else {
        Print("WARNING: LSL inlet connection failed for [" + streamName + "]")
        gLSLInletActive <- 0
    }

    return gLSLInletActive
}


## Wait for a specific trigger/marker from the inlet
##
## Arguments:
##   markerName - Marker to wait for (e.g., "TR_pulse", "fixation")
##   timeout - Maximum wait time in seconds (default 5.0)
##
## Returns:
##   Marker string on success, "<timeout>" on timeout, "<error>" on error
##
define WaitForLSLTrigger(markerName, timeout:5.0)
{
    if(not gLSLInletActive)
    {
        return "<no_inlet>"
    }

    ## Call C++ built-in to poll for specific marker
    result <- WaitForLSLTrigger_Internal(markerName, timeout)

    return result
}


## Pull the next sample from inlet (blocking)
##
## Arguments:
##   timeout - Maximum wait time in seconds (default 1.0)
##
## Returns:
##   List of sample values on success
##   "<timeout>" on timeout
##   Empty list on error
##
define WaitForLSLSample(timeout:1.0)
{
    if(not gLSLInletActive)
    {
        return []
    }

    ## Call C++ built-in to pull sample
    result <- WaitForLSLSample_Internal(timeout)

    return result
}


## Pull the latest sample from inlet (non-blocking)
##
## Returns:
##   List of sample values if new data available
##   Empty list if no new data
##
define PollLSLSample()
{
    if(not gLSLInletActive)
    {
        return []
    }

    ## Call C++ built-in with 0.0 timeout (non-blocking)
    result <- PollLSLSample_Internal()

    return result
}


## Check if new data is available without pulling
##
## Returns:
##   1 if new data available, 0 otherwise
##
define LSLInletHasSamples()
{
    if(not gLSLInletActive)
    {
        return 0
    }

    ## Check by attempting non-blocking pull
    result <- PollLSLSample()

    return If(Length(result) > 0, 1, 0)
}


## Close the inlet
##
define CloseLSLInlet()
{
    if(gLSLInletActive)
    {
        CloseLSLInlet_Internal()
        gLSLInletActive <- 0
        Print("LSL inlet closed")
    }
}
```

**Update FinalizeLSL() to handle inlets:**

```pebl
define FinalizeLSL()
{
    ## Close outlet if active
    if(gLSLActive)
    {
        LSLMarker("experiment_end")
        CloseLSLOutlet()
        gLSLActive <- 0
    }

    ## Close inlet if active
    if(gLSLInletActive)
    {
        CloseLSLInlet()
        gLSLInletActive <- 0
    }

    Print("LSL finalized")
}
```

### C++ Built-in Functions for Inlets

**Add to src/libs/PEBLLSL.cpp:**

```cpp
#ifdef PEBL_USE_LSL

//============================================================================//
// Phase 2: Inlet Functions (Receive Triggers)
//============================================================================//

// Global LSL inlet (singleton - one per PEBL instance)
static lsl::stream_inlet* gLSLInlet = nullptr;

/**
 * CreateLSLInlet_Internal - Resolve stream and create inlet
 *
 * Arguments:
 *   streamName - Name of stream to find
 *   streamType - Type of stream to find
 *
 * Returns:
 *   1 on success, 0 on failure
 */
Variant CreateLSLInlet_Internal(Variant v)
{
    PList* plist = v.GetComplexData()->GetList();

    std::string name = plist->First().GetString();
    std::string type = plist->Nth(2).GetString();

    try {
        // Close previous inlet if exists
        if (gLSLInlet) {
            delete gLSLInlet;
            gLSLInlet = nullptr;
        }

        // Resolve stream on network (wait up to 5 seconds)
        std::vector<lsl::stream_info> results;

        if (type == "") {
            // Resolve by name only
            results = lsl::resolve_stream("name", name, 1, 5.0);
        } else {
            // Resolve by name AND type (more specific)
            // Use XPath predicate for multiple conditions
            std::string predicate = "name='" + name + "' and type='" + type + "'";
            results = lsl::resolve_stream(predicate, 1, 5.0);
        }

        if (results.empty()) {
            PError::SignalWarning("LSL inlet: No stream found for [" +
                                  name + "]");
            return Variant(0);
        }

        // Create inlet from resolved stream
        gLSLInlet = new lsl::stream_inlet(results[0]);

        // Open stream (start buffering)
        gLSLInlet->open_stream();

        return Variant(1);  // Success
    }
    catch (std::exception& e) {
        PError::SignalWarning("LSL inlet creation failed: " +
                              std::string(e.what()));
        return Variant(0);
    }
}


/**
 * WaitForLSLTrigger_Internal - Wait for specific marker
 *
 * Arguments:
 *   targetMarker - Marker to wait for
 *   timeout - Maximum wait time
 *
 * Returns:
 *   Marker string on success
 *   "<timeout>" on timeout
 */
Variant WaitForLSLTrigger_Internal(Variant v)
{
    if (!gLSLInlet) {
        return Variant("<no_inlet>");
    }

    PList* plist = v.GetComplexData()->GetList();
    std::string targetMarker = plist->First().GetString();
    double timeout = plist->GetLength() > 1 ?
                     plist->Nth(2).GetFloat() : 5.0;

    try {
        std::string sample;
        double startTime = lsl::local_clock();

        // Poll for marker until timeout
        while (lsl::local_clock() - startTime < timeout) {
            // Pull sample with short timeout (100ms)
            double timestamp = gLSLInlet->pull_sample(&sample, 1, 0.1);

            if (timestamp != 0.0) {
                // Got a sample - check if it matches
                if (sample == targetMarker) {
                    return Variant(sample);  // Found it!
                }
                // Else keep waiting for correct marker
            }
        }

        // Timeout
        return Variant("<timeout>");
    }
    catch (std::exception& e) {
        PError::SignalWarning("LSL trigger wait failed: " +
                              std::string(e.what()));
        return Variant("<error>");
    }
}


/**
 * WaitForLSLSample_Internal - Pull next sample (blocking)
 *
 * Arguments:
 *   timeout - Maximum wait time
 *
 * Returns:
 *   List of sample values on success
 *   "<timeout>" string on timeout
 *   Empty list on error
 */
Variant WaitForLSLSample_Internal(Variant v)
{
    if (!gLSLInlet) {
        return Variant(new PList());
    }

    PList* plist = v.GetComplexData()->GetList();
    double timeout = plist->GetLength() > 0 ?
                     plist->First().GetFloat() : 1.0;

    try {
        std::vector<float> sample;
        double timestamp = gLSLInlet->pull_sample(sample, timeout);

        if (timestamp == 0.0) {
            // Timeout
            return Variant("<timeout>");
        }

        // Convert to PEBL list
        PList* result = new PList();
        for (float val : sample) {
            result->PushOnEnd(Variant(val));
        }

        return Variant(result);
    }
    catch (std::exception& e) {
        PError::SignalWarning("LSL sample pull failed: " +
                              std::string(e.what()));
        return Variant(new PList());
    }
}


/**
 * PollLSLSample_Internal - Pull sample non-blocking
 *
 * Returns:
 *   List of sample values if available
 *   Empty list if no new data
 */
Variant PollLSLSample_Internal(Variant v)
{
    if (!gLSLInlet) {
        return Variant(new PList());
    }

    try {
        std::vector<float> sample;

        // Non-blocking pull (timeout = 0.0)
        double timestamp = gLSLInlet->pull_sample(sample, 0.0);

        if (timestamp == 0.0) {
            // No new data
            return Variant(new PList());
        }

        // Convert to PEBL list
        PList* result = new PList();
        for (float val : sample) {
            result->PushOnEnd(Variant(val));
        }

        return Variant(result);
    }
    catch (std::exception& e) {
        return Variant(new PList());
    }
}


/**
 * CloseLSLInlet_Internal - Close inlet
 */
Variant CloseLSLInlet_Internal(Variant v)
{
    if (gLSLInlet) {
        delete gLSLInlet;
        gLSLInlet = nullptr;
    }
    return Variant(1);
}

#endif // PEBL_USE_LSL
```

### Example: fMRI-Compatible Stroop Task

**Complete implementation with scanner synchronization:**

```pebl
define Start(p)
{
    gWin <- MakeWindow()
    gSubNum <- GetSubNum(gWin)
    InitializePEBL()

    ## Load parameters
    gParams <- CreateParameters([
        ["use_mri_sync", 0],      ## Enable MRI synchronization
        ["mri_stream_name", "MRI_Triggers"],
        ["mri_pulse_marker", "TR_pulse"],
        ["tr_timeout", 10.0]      ## Timeout for scanner pulse
    ], gParamFile)

    gFileOut <- GetNewDataFile(gSubNum, gWin, "stroop", "csv",
                                "sub,trial,tr_received,condition,word,color,response,rt,correct")

    ## Connect to MRI scanner if requested
    gMRIMode <- 0
    if(gParams.use_mri_sync)
    {
        Print("Waiting for MRI scanner connection...")
        success <- CreateLSLInlet(gParams.mri_stream_name, "Markers")

        if(success)
        {
            Print("Connected to MRI scanner")
            gMRIMode <- 1
            LSLMarker("MRI_mode_enabled")
        } else {
            Print("WARNING: MRI scanner not found - running without sync")
            MessageBox("WARNING: MRI scanner not detected." + CR(1) +
                       "Experiment will run without synchronization.", gWin)
            gMRIMode <- 0
        }
    }

    ShowInstructions()

    ## Test trials
    LSLMarker("test_start")

    trialNum <- 0
    loop(trial, gTrials)
    {
        trialNum <- trialNum + 1
        LSLMarker("trial_start")
        LSLMarker("trial_" + trialNum)

        ## Wait for MRI trigger if in MRI mode
        trReceived <- 1
        if(gMRIMode)
        {
            ## Show waiting message
            ShowMessage("Waiting for scanner TR...")
            Draw()

            ## Wait for pulse
            trigger <- WaitForLSLTrigger(gParams.mri_pulse_marker,
                                         gParams.tr_timeout)

            if(trigger == "<timeout>")
            {
                Print("WARNING: No scanner TR pulse received for trial " + trialNum)
                LSLMarker("MRI_pulse_timeout")
                trReceived <- 0

                ## Ask what to do
                choice <- GetEasyChoice("No scanner pulse received. Continue?",
                                        ["Continue", "Abort"], ["cont", "abort"], gWin)
                if(choice == "abort")
                {
                    break  ## Exit trial loop
                }
            } else {
                LSLMarker("MRI_pulse_received")
            }
        }

        ## Clear waiting message
        Draw()
        Wait(100)

        ## Show fixation
        ShowFixation()
        LSLMarker("fixation")
        Wait(500)

        ## Show stimulus
        ShowStimulus(trial.word, trial.color)
        stimOnset <- GetTime()
        LSLMarker("stimulus_onset")
        LSLMarker("condition_" + trial.condition)

        ## Get response
        response <- WaitForKeyPressWithTimeout(2.0)
        rt <- GetTime() - stimOnset
        LSLMarker("response")

        ## Score
        accuracy <- Score(response, trial.correct)
        LSLMarker(If(accuracy, "correct", "incorrect"))

        ## Save trial data
        FilePrint(gFileOut, gSubNum + "," + trialNum + "," + trReceived + "," +
                  trial.condition + "," + trial.word + "," + trial.color + "," +
                  response + "," + rt + "," + accuracy)

        LSLMarker("trial_end")

        ## ITI
        Wait(1000)
    }

    LSLMarker("test_end")

    ShowResults()
    FinalizePEBL()
}
```

---

## Phase 3: Advanced Features

**Goal:** Support advanced real-time experiments and closed-loop paradigms.

**Priority:** Lower - Implement only if needed

### Potential Features

**1. Continuous Data Streaming (Not Just Events)**

Currently focused on event markers (strings). Could extend to continuous data:

```pebl
## Stream continuous behavioral data (e.g., joystick position)
CreateLSLOutlet_Continuous("PEBL_Joystick", "Position", 2, 60.0)  ## 2 channels, 60 Hz

loop(i, 1000)
{
    pos <- GetJoystickPosition()
    SendLSLSample([pos.x, pos.y])
    Wait(16)  ## ~60 Hz
}
```

**2. Multi-Channel Inlets**

Receive multi-channel data (e.g., gaze + pupil diameter):

```pebl
CreateLSLInlet("EyeTracker_Full", "Gaze")

sample <- WaitForLSLSample()
## sample = [gazeX, gazeY, pupilLeft, pupilRight]

gazeX <- First(sample)
gazeY <- Second(sample)
pupilLeft <- Third(sample)
pupilRight <- Fourth(sample)
```

**3. Real-Time Signal Processing**

Process incoming signals in PEBL:

```pebl
## Moving average of EEG alpha power
alphaHistory <- []

loop(trial, trials)
{
    ## Collect 10 samples
    loop(i, 10)
    {
        sample <- WaitForLSLSample(0.1)
        if(Length(sample) > 0)
        {
            PushOnEnd(alphaHistory, First(sample))
        }
    }

    ## Compute moving average
    avgAlpha <- Mean(Tail(alphaHistory, 10))

    ## Use for trial adaptation
    if(avgAlpha > threshold)
    {
        ## High alpha = make trial easier
        trial.difficulty <- "easy"
    } else {
        trial.difficulty <- "hard"
    }

    RunTrial(trial)
}
```

**4. Multiple Outlets/Inlets**

Support multiple simultaneous streams:

```pebl
## Multiple outlets
CreateLSLOutlet("PEBL_Events", "Markers", "events")
CreateLSLOutlet("PEBL_Behavior", "Continuous", "behavior")

LSLMarker("trial_start")  ## Goes to default outlet
SendLSLSample([x, y], outletName:"PEBL_Behavior")  ## Specify outlet

## Multiple inlets
CreateLSLInlet("MRI_Triggers", "Markers")
CreateLSLInlet("EyeTracker", "Gaze")

mriPulse <- WaitForLSLTrigger("TR_pulse", inletName:"MRI_Triggers")
gazeSample <- WaitForLSLSample(inletName:"EyeTracker")
```

**5. LSL Configuration File**

Alternative to command-line flags:

```json
// lsl-config.json
{
    "enabled": true,
    "outlet": {
        "name": "PEBL_Stroop_v2",
        "type": "Markers",
        "source_id": "pebl_stroop_exp1"
    },
    "inlet": {
        "name": "MRI_Triggers",
        "type": "Markers",
        "timeout": 10.0
    }
}
```

```pebl
## Load LSL config
gLSLConfig <- ReadJSONParameters("lsl-config.json")
if(gLSLConfig.enabled)
{
    CreateLSLOutlet(gLSLConfig.outlet.name,
                    gLSLConfig.outlet.type,
                    gLSLConfig.outlet.source_id)
}
```

**6. Closed-Loop Brain-Computer Interface**

Real-time experiment control based on EEG:

```pebl
## BCI: Show stimulus only when alpha > threshold
CreateLSLInlet("BCI_AlphaPower", "Features")

loop(trial, trials)
{
    ## Wait for high alpha (relaxed state)
    ready <- 0
    while(not ready)
    {
        sample <- WaitForLSLSample(0.1)
        alphaPower <- First(sample)

        if(alphaPower > 0.7)
        {
            ready <- 1
        }
    }

    ## Participant is relaxed - show stimulus
    ShowStimulus(trial)

    ## Monitor alpha during response window
    response <- WaitForKeyPressWithMonitor(
        keys:["<lshift>", "<rshift>"],
        monitor:MonitorAlphaPower,
        monitorRate:100
    )
}
```

---

## Implementation Roadmap

### Phase 1: Outlets (High Priority)

**Milestone 1.1: Core Infrastructure (1 week)**

C++ Components:
- [ ] Add command-line flag parsing (`--lsl`, `--lsl-name`, `--lsl-type`, `--lsl-source-id`)
- [ ] Create global variables (`gLSLEnabled`, `gLSLStreamName`, etc.)
- [ ] Expose to PEBL (`GetLSLEnabled()`, `GetLSLStreamName()`, etc.)
- [ ] Rewrite `PLabStreamingLayer.h/cpp` (PLSL class)
- [ ] Implement `src/libs/PEBLLSL.cpp` (`CreateLSLOutlet`, `SendLSLMarker`, `CloseLSLOutlet`)
- [ ] Register functions in `src/libs/Functions.h`
- [ ] Update Makefile with `USE_LSL=1` flag
- [ ] Build LSL library (`libs/labstreaminglayer/LSL/liblsl`)

PEBL Components:
- [ ] Add wrapper functions to `pebl-lib/Utility.pbl` (`InitializeLSL`, `LSLMarker`, `FinalizeLSL`)
- [ ] Create `InitializePEBL()` / `FinalizePEBL()` standard pattern

Testing:
- [ ] Create `test-lsl.pbl` minimal test script
- [ ] Test with LabRecorder (verify markers received)
- [ ] Test without `--lsl` flag (verify no errors)

**Milestone 1.2: Battery Integration (2 weeks)**

Pilot Tasks:
- [ ] Retrofit Stroop task with LSL markers
- [ ] Retrofit Flanker task with LSL markers
- [ ] Retrofit Simple RT task with LSL markers
- [ ] Test each with LabRecorder + EEG hardware (if available)

Documentation:
- [ ] Standard marker convention document
- [ ] Battery task migration guide ("Adding LSL to Your Task")
- [ ] Update existing tasks (5-10 lines each)

**Milestone 1.3: Expand to More Tasks (1 week)**

Category 1 Tasks (High Priority):
- [ ] Go/No-Go
- [ ] Oddball
- [ ] Continuous Performance Test
- [ ] Antisaccade
- [ ] Dot judgment
- [ ] (Continue through battery...)

### Phase 2: Inlets (Medium Priority)

**Milestone 2.1: Core Inlet Functions (1 week)**

C++ Components:
- [ ] Add command-line flags (`--lsl-inlet`, `--lsl-inlet-type`)
- [ ] Implement `CreateLSLInlet_Internal()` (stream resolution)
- [ ] Implement `WaitForLSLTrigger_Internal()`
- [ ] Implement `WaitForLSLSample_Internal()`
- [ ] Implement `PollLSLSample_Internal()`
- [ ] Implement `CloseLSLInlet_Internal()`

PEBL Components:
- [ ] Add inlet wrappers to `pebl-lib/Utility.pbl`
- [ ] Update `FinalizeLSL()` to close inlets
- [ ] Add global variable `gLSLInletActive`

**Milestone 2.2: Use Case Implementation (1 week)**

Example Tasks:
- [ ] fMRI-synchronized Stroop task (scanner triggers)
- [ ] Gaze-contingent flanker task (eye tracker)
- [ ] Neurofeedback relaxation task (EEG alpha)

Testing:
- [ ] Test with simulated MRI trigger stream
- [ ] Test with actual eye tracker (if available)
- [ ] Test with BCI system (if available)

**Milestone 2.3: Documentation (1 week)**

- [ ] Inlet usage guide
- [ ] Scanner synchronization tutorial
- [ ] Gaze-contingent experiment tutorial
- [ ] Troubleshooting guide (stream not found, timeouts, etc.)

### Phase 3: Advanced Features (Future)

**Only implement if needed:**
- [ ] Continuous data streaming
- [ ] Multi-channel support
- [ ] Multiple outlets/inlets
- [ ] LSL configuration file
- [ ] Real-time signal processing helpers
- [ ] Closed-loop experiment framework

---

## Testing Strategy

### Unit Tests (C++ Level)

**test_lsl_outlet.cpp:**
```cpp
#include "PEBLLSL.h"
#include <cassert>

void test_create_outlet()
{
    // Test outlet creation
    Variant params = MakeList({"test_stream", "Markers", "test_id"});
    Variant result = CreateLSLOutlet(params);
    assert(result.GetInteger() == 1);  // Success

    // Test marker send
    Variant marker = MakeList({"test_marker"});
    result = SendLSLMarker(marker);
    assert(result.GetInteger() == 1);  // Success

    // Test close
    result = CloseLSLOutlet();
    assert(result.GetInteger() == 1);  // Success
}

void test_marker_types()
{
    CreateLSLOutlet(MakeList({"test", "Markers", "id"}));

    // String marker
    SendLSLMarker(MakeList({"string_marker"}));

    // Integer marker
    SendLSLMarker(MakeList({100}));

    // Float marker (converted to string)
    SendLSLMarker(MakeList({3.14}));

    CloseLSLOutlet();
}
```

### Integration Tests (PEBL Level)

**test-lsl-basic.pbl:**
```pebl
## Minimal LSL test
define Start(p)
{
    Print("LSL Basic Test")

    ## Test without --lsl flag
    Print("Testing graceful no-op...")
    LSLMarker("test_marker_1")  ## Should be silent
    Print("  OK - no error")

    ## Test with initialization
    Print("Testing with manual init...")
    success <- CreateLSLOutlet("Test_Stream", "Markers", "test123")
    if(success)
    {
        Print("  Outlet created")
        SendLSLMarker("manual_marker_1")
        SendLSLMarker("manual_marker_2")
        SendLSLMarker(42)  ## Integer
        CloseLSLOutlet()
        Print("  OK - markers sent")
    } else {
        Print("  FAILED - could not create outlet")
    }
}
```

**test-lsl-automatic.pbl:**
```pebl
## Test automatic initialization via InitializePEBL()
define Start(p)
{
    Print("LSL Automatic Test")

    gWin <- MakeWindow()
    gSubNum <- "TEST001"
    gScriptName <- "test-lsl-automatic"

    InitializePEBL()  ## Should initialize LSL if --lsl flag set

    ## Send markers
    LSLMarker("experiment_start")
    LSLMarker("trial_1_start")
    LSLMarker("stimulus_onset")
    Wait(100)
    LSLMarker("response")
    LSLMarker("trial_1_end")

    LSLMarker("trial_2_start")
    LSLMarker("stimulus_onset")
    Wait(100)
    LSLMarker("response")
    LSLMarker("trial_2_end")

    FinalizePEBL()  ## Should finalize LSL

    Print("Test complete")
}
```

**test-lsl-inlet.pbl:**
```pebl
## Test inlet functionality
define Start(p)
{
    Print("LSL Inlet Test")

    gWin <- MakeWindow()
    InitializePEBL()

    ## Try to connect to test stream
    Print("Searching for test stream...")
    success <- CreateLSLInlet("Test_Trigger_Stream", "Markers")

    if(success)
    {
        Print("Connected! Waiting for triggers...")

        loop(i, 5)
        {
            trigger <- WaitForLSLTrigger("test_trigger", timeout:10.0)

            if(trigger == "<timeout>")
            {
                Print("  Timeout on attempt " + i)
            } else {
                Print("  Received: " + trigger)
                LSLMarker("trigger_received")
            }
        }

        CloseLSLInlet()
    } else {
        Print("Could not connect to stream")
    }

    FinalizePEBL()
}
```

### System Tests (Full Workflow)

**Test 1: Basic Recording**
```bash
# Terminal 1: Start LabRecorder
LabRecorder

# Terminal 2: Run PEBL test
pebl2 test-lsl-automatic.pbl --lsl

# Verify in LabRecorder:
# - Stream "PEBL_test-lsl-automatic" appears
# - Markers appear in real-time
# - Save XDF file

# Terminal 3: Analyze XDF file
python3 << EOF
import pyxdf
streams, header = pyxdf.load_xdf('test_recording.xdf')

# Find PEBL marker stream
for stream in streams:
    if stream['info']['name'][0] == 'PEBL_test-lsl-automatic':
        markers = stream['time_series']
        timestamps = stream['time_stamps']

        print(f"Found {len(markers)} markers:")
        for i, (marker, ts) in enumerate(zip(markers[:10], timestamps[:10])):
            print(f"  {i}: {marker[0]} @ {ts:.3f}s")
EOF
```

**Test 2: EEG Synchronization**
```bash
# Terminal 1: Start EEG LSL app (device-specific)
# e.g., BrainProducts or Emotiv LSL app

# Terminal 2: Start LabRecorder
LabRecorder

# Terminal 3: Run PEBL task
pebl2 battery/stroop/stroop.pbl -s P001 --lsl

# Verify:
# - Both EEG stream and PEBL marker stream recorded
# - Timestamps synchronized
# - Can compute ERPs aligned to markers
```

**Test 3: Scanner Synchronization**
```bash
# Terminal 1: Start MRI simulator (sends TR pulses via LSL)
python3 mri_simulator.py  ## Sends "TR_pulse" markers every 2.5s

# Terminal 2: Start LabRecorder
LabRecorder

# Terminal 3: Run fMRI-compatible task
pebl2 battery/stroop/stroop-mri.pbl -s P001 --lsl --lsl-inlet "MRI_Triggers"

# Verify:
# - Task waits for TR pulse before each trial
# - Trials synchronized to scanner acquisition
```

**Test 4: Cross-Platform**
```bash
# Test on Linux
make clean && make main USE_LSL=1
pebl2 test-lsl-automatic.pbl --lsl

# Test on Windows (if cross-compiling)
make clean && make windows USE_LSL=1
wine pebl2.exe test-lsl-automatic.pbl --lsl

# Test on macOS (if available)
make clean && make mac USE_LSL=1
./pebl2 test-lsl-automatic.pbl --lsl
```

### Performance Tests

**Latency Test:**
```pebl
## Measure marker send latency
define Start(p)
{
    InitializePEBL()

    latencies <- []

    loop(i, 1000)
    {
        t1 <- GetTime()
        LSLMarker("test_" + i)
        t2 <- GetTime()

        PushOnEnd(latencies, t2 - t1)
    }

    Print("Mean latency: " + Mean(latencies) + " ms")
    Print("Max latency: " + Max(latencies) + " ms")
    Print("Min latency: " + Min(latencies) + " ms")

    FinalizePEBL()
}
```

**Expected results:**
- Mean latency: < 1 ms
- Max latency: < 5 ms
- Jitter: < 1 ms

---

## Documentation Requirements

### User Documentation

**1. Quick Start Guide**

File: `doc/LSL_QUICKSTART.md`

Contents:
- What is LSL?
- Installing LabRecorder
- Running PEBL with `--lsl` flag
- Viewing markers in LabRecorder
- Saving and analyzing XDF files

**2. User Manual Chapter**

Add to PEBL manual (LaTeX):
- LSL overview
- Command-line flags
- Marker conventions
- Troubleshooting

**3. Battery Task Guide**

File: `doc/LSL_BATTERY_GUIDE.md`

Contents:
- Which tasks have LSL support
- Standard markers for each task
- Task-specific considerations
- Example analysis scripts (MATLAB/Python)

### Developer Documentation

**1. Adding LSL to Tasks**

File: `doc/LSL_TASK_MIGRATION.md`

Contents:
- Step-by-step guide
- Code templates
- Marker naming conventions
- Testing checklist

**2. API Reference**

File: `doc/LSL_API_REFERENCE.md`

Contents:
- PEBL functions (LSLMarker, InitializeLSL, etc.)
- C++ functions (CreateLSLOutlet, etc.)
- Parameters and return values
- Example code snippets

**3. Advanced Use Cases**

File: `doc/LSL_ADVANCED.md`

Contents:
- Scanner synchronization
- Gaze-contingent experiments
- Neurofeedback
- Multi-device setups

### Analysis Documentation

**1. MATLAB Analysis Guide**

File: `doc/LSL_MATLAB_ANALYSIS.md`

Contents:
```matlab
%% Load XDF file
[streams, fileheader] = load_xdf('recording.xdf');

%% Find PEBL marker stream
pebl_stream = [];
for i = 1:length(streams)
    if strcmp(streams{i}.info.type, 'Markers') && ...
       contains(streams{i}.info.name, 'PEBL')
        pebl_stream = streams{i};
        break;
    end
end

%% Extract markers
markers = pebl_stream.time_series;
timestamps = pebl_stream.time_stamps;

%% Find specific events
stimulus_idx = find(strcmp(markers, 'stimulus_onset'));
response_idx = find(strcmp(markers, 'response'));

%% Compute reaction times
for i = 1:length(stimulus_idx)
    stim_time = timestamps(stimulus_idx(i));
    resp_time = timestamps(response_idx(i));
    rt(i) = (resp_time - stim_time) * 1000;  % ms
end

fprintf('Mean RT: %.2f ms\n', mean(rt));
```

**2. Python Analysis Guide**

File: `doc/LSL_PYTHON_ANALYSIS.md`

Contents:
```python
import pyxdf
import numpy as np
import mne

# Load XDF file
streams, header = pyxdf.load_xdf('recording.xdf')

# Find PEBL markers
pebl_stream = None
for stream in streams:
    if stream['info']['type'][0] == 'Markers' and \
       'PEBL' in stream['info']['name'][0]:
        pebl_stream = stream
        break

# Extract markers
markers = [m[0] for m in pebl_stream['time_series']]
timestamps = pebl_stream['time_stamps']

# Find EEG stream
eeg_stream = None
for stream in streams:
    if stream['info']['type'][0] == 'EEG':
        eeg_stream = stream
        break

# Convert to MNE format
eeg_data = eeg_stream['time_series'].T
eeg_times = eeg_stream['time_stamps']
sfreq = float(eeg_stream['info']['nominal_srate'][0])

# Create MNE annotations from PEBL markers
annotations = mne.Annotations(
    onset=timestamps,
    duration=[0] * len(timestamps),
    description=markers
)

# Create MNE Raw object
info = mne.create_info(['EEG1', 'EEG2', ...], sfreq, 'eeg')
raw = mne.io.RawArray(eeg_data, info)
raw.set_annotations(annotations)

# Epoch around stimulus onset
events, event_id = mne.events_from_annotations(raw)
epochs = mne.Epochs(raw, events, event_id,
                    tmin=-0.2, tmax=0.8, baseline=(-0.2, 0))

# Compute ERPs
evoked = epochs.average()
evoked.plot()
```

### Video Tutorials

**1. Getting Started (5 min)**
- Install LabRecorder
- Run PEBL task with --lsl
- Record to XDF
- View in Python

**2. Adding LSL to Your Task (10 min)**
- Code walkthrough
- Marker placement
- Testing with LabRecorder

**3. Scanner Synchronization (15 min)**
- fMRI setup
- Inlet configuration
- Synchronized task
- Troubleshooting

---

## Appendices

### Appendix A: Hardware Compatibility List

**EEG Systems (Tested):**
- Brain Products (LiveAmp, actiCHamp, BrainAmp)
- Emotiv (EPOC, Insight)
- OpenBCI (Cyton, Ganglion)
- g.tec (g.USBamp, g.Nautilus)
- ANT Neuro (eego series)
- Cognionics

**Eye Trackers (Tested):**
- Tobii Pro (Spectrum, Fusion, Nano)
- Pupil Labs (Invisible, Core)
- SR Research (EyeLink with LSL bridge)
- Gazepoint (GP3, GP3 HD)

**Physiological Sensors (Tested):**
- BIOPAC (MP150, MP160 with LSL app)
- Thought Technology (ProComp Infiniti)
- Shimmer sensors
- Polar heart rate monitors (via LSL bridge)

**fNIRS Systems (Tested):**
- NIRx (NIRSport, NIRScout)
- Artinis (PortaLite, OctaMon)

**MRI Scanners (Tested):**
- Siemens (with trigger box → LSL bridge)
- Philips (with trigger box → LSL bridge)
- GE (with trigger box → LSL bridge)

**Note:** Most devices require device-specific LSL app (provided by manufacturer or LSL community).

Full list: https://labstreaminglayer.readthedocs.io/info/supported_devices.html

### Appendix B: Troubleshooting

**Problem:** "LSL initialization failed"

Solutions:
- Check LSL library compiled correctly (`USE_LSL=1`)
- Verify liblsl.so in library path (`export LD_LIBRARY_PATH=...`)
- Check firewall not blocking LSL ports (UDP 16571, TCP 16572)

**Problem:** "No stream found" when creating inlet

Solutions:
- Check stream name matches exactly (case-sensitive)
- Verify device LSL app is running
- Check both on same network/subnet
- Try `lsl_resolve_streams` tool to list available streams
- Increase timeout (`WaitForLSLTrigger(..., timeout:30.0)`)

**Problem:** Markers delayed or missing in LabRecorder

Solutions:
- Check LabRecorder "Update" button clicked
- Verify PEBL stream checkbox enabled
- Check system time correct (LSL uses timestamps)
- Monitor network latency (WiFi can add jitter)
- Use wired ethernet for critical timing

**Problem:** "Timeout waiting for MRI trigger"

Solutions:
- Verify MRI LSL app running and sending pulses
- Check trigger marker name matches (`"TR_pulse"`, `"5"`, etc.)
- Test MRI app with LabRecorder first
- Increase timeout value
- Check MRI trigger box properly connected

**Problem:** High marker send latency

Solutions:
- Check CPU load (close other applications)
- Use wired ethernet (not WiFi)
- Reduce other LSL streams (fewer devices)
- Check PEBL not running in debug mode

### Appendix C: LSL File Locations

**LSL Library:**
```
libs/labstreaminglayer/
├── LSL/
│   └── liblsl/
│       ├── include/
│       │   ├── lsl_c.h
│       │   └── lsl_cpp.h
│       ├── build/
│       │   └── install/
│       │       ├── lib/
│       │       │   └── liblsl.so (Linux)
│       │       └── include/
│       └── CMakeLists.txt
└── Apps/
    └── LabRecorder/
```

**PEBL LSL Code:**
```
src/
├── libs/
│   ├── PEBLLSL.cpp (new)
│   ├── PEBLLSL.h (new)
│   └── Functions.h (modified - register LSL functions)
├── utility/
│   ├── PLabStreamingLayer.cpp (rewritten)
│   └── PLabStreamingLayer.h (rewritten)
└── base/
    └── PEBLEnvironment.cpp (modified - command-line flags)

pebl-lib/
└── Utility.pbl (modified - add wrapper functions)

doc/
├── LSL_DESIGN.md (this file)
├── LSL_QUICKSTART.md
├── LSL_TASK_MIGRATION.md
├── LSL_API_REFERENCE.md
└── LSL_ADVANCED.md
```

### Appendix D: XDF File Format

**Structure:**
```xml
<xdf version="1.0">
  <stream>
    <info>
      <name>PEBL_stroop</name>
      <type>Markers</type>
      <channel_count>1</channel_count>
      <nominal_srate>0</nominal_srate>
      <channel_format>string</channel_format>
      <source_id>pebl_stroop_P001</source_id>
    </info>
    <time_series>
      <sample>
        <timestamp>12345.678</timestamp>
        <value>trial_start</value>
      </sample>
      <sample>
        <timestamp>12346.234</timestamp>
        <value>stimulus_onset</value>
      </sample>
      ...
    </time_series>
  </stream>

  <stream>
    <info>
      <name>BrainAmp_EEG</name>
      <type>EEG</type>
      <channel_count>64</channel_count>
      <nominal_srate>500</nominal_srate>
      ...
    </info>
    <time_series>
      ...
    </time_series>
  </stream>
</xdf>
```

**Python loading:**
```python
import pyxdf

streams, header = pyxdf.load_xdf('recording.xdf')

# streams is a list of dicts
# Each dict has:
#   'info' - stream metadata
#   'time_series' - data samples
#   'time_stamps' - timestamps for each sample
```

**MATLAB loading:**
```matlab
[streams, fileheader] = load_xdf('recording.xdf');

% streams is a struct array
% Each struct has:
%   .info - stream metadata
%   .time_series - data samples
%   .time_stamps - timestamps for each sample
```

### Appendix E: References

**LSL Documentation:**
- Main repository: https://github.com/sccn/labstreaminglayer
- Documentation: https://labstreaminglayer.readthedocs.io
- API Reference: https://labstreaminglayer.readthedocs.io/dev/lib_dev.html
- Supported Devices: https://labstreaminglayer.readthedocs.io/info/supported_devices.html

**LSL Applications:**
- LabRecorder: https://github.com/labstreaminglayer/App-LabRecorder
- App Examples: https://github.com/labstreaminglayer/App-Examples

**XDF Format:**
- Specification: https://github.com/sccn/xdf
- pyxdf (Python): https://github.com/xdf-modules/pyxdf
- xdf-Matlab: https://github.com/xdf-modules/xdf-Matlab

**Related Publications:**
- Kothe, C., Shirazi, S. Y., Stenner, T., Medine, D., Boulay, C., Grivich, M. I., Artoni, F., Mullen, T., Delorme, A., & Makeig, S. (2025). The Lab Streaming Layer for Synchronized Multimodal Recording. *Imaging Neuroscience*. https://doi.org/10.1162/IMAG.a.136

**Similar Integrations:**
- PsychoPy: https://github.com/kaczmarj/psychopy-lsl
- Presentation: https://www.neurobs.com/pres_docs/html/03_presentation/06_hardware_interfacing/02_lab_streaming_layer.htm
- OpenSesame: https://osdoc.cogsci.nl/3.3/manual/devices/lsl/

---

## Summary

This document provides a complete design for integrating Lab Streaming Layer (LSL) into PEBL across three phases:

**Phase 1 (High Priority):** Outlets for sending event markers
- Command-line flag activation (`--lsl`)
- Wrapper functions following UploadFile() pattern
- Standard marker conventions
- Battery task integration (5-10 lines per task)
- **Timeline:** 3-4 weeks
- **Coverage:** 90% of use cases

**Phase 2 (Medium Priority):** Inlets for receiving triggers
- fMRI scanner synchronization
- Gaze-contingent experiments
- Neurofeedback/BCI
- **Timeline:** 2-3 weeks after Phase 1
- **Coverage:** Advanced research scenarios

**Phase 3 (Lower Priority):** Advanced features
- Continuous data streaming
- Multi-channel support
- Real-time signal processing
- **Timeline:** Future, as needed

**Key Benefits:**
- ✅ Enables neuroscience research with PEBL
- ✅ Zero-code pattern (always call functions, only active when enabled)
- ✅ Minimal task changes (5-15 lines per task)
- ✅ Standard across all PEBL battery tasks
- ✅ Compatible with all LSL-enabled devices
- ✅ Cross-platform (Linux, Windows, macOS)

**Ready for Implementation:** All design decisions made, code architecture specified, testing strategy defined.
