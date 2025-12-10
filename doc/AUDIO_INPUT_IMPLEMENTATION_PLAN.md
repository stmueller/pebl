# PEBL Audio Input Implementation Plan

**Date:** 2025-11-18
**Purpose:** Restore and enhance PEBL's audio input capabilities for recording and voice key experiments
**Target Release:** PEBL 2.2 or 2.3

---

## Executive Summary

This plan outlines the restoration of PEBL's audio input system by migrating from the unmaintained SDL_audio_in library to SDL2's native audio capture API. The plan includes two main objectives:

1. **General Audio Recording** - Record, save, playback, and visualize audio
2. **Voice Key for Experiments** - Detect voice onset times for reaction time studies

**Estimated Total Effort:** 3-5 days for Phase 1 (SDL2 migration), optional 2-3 days for Phase 2 (enhancements)

---

## Current State Analysis

### Existing PEBL API (Currently Disabled)

```pebl
## Create audio input buffer (duration in ms)
buffer <- MakeAudioInputBuffer(5000)

## Get vocal response time (buffer, threshold, sustain_ms)
rt <- GetVocalResponseTime(buffer, 0.25, 200)
## Returns: [onset_ms, offset_ms, triggered_boolean]

## Save recorded audio to WAV file
SaveAudioToWaveFile("recording.wav", buffer)
```

### Architecture

```
PEBL Script API (PEBLObjects.cpp)
├── MakeAudioInputBuffer(duration)      → Creates recording buffer
├── GetVocalResponseTime(buf, th, sus)  → Voice key algorithm
└── SaveAudioToWaveFile(file, buffer)   → Export to WAV

C++ Implementation (PlatformAudioIn.cpp)
├── CreateBuffer(size)           → Allocate buffer
├── Initialize(type)             → Open audio device
├── RecordToBuffer()             → Start recording
├── Stop()                       → Stop recording
├── VoiceKey(threshold, sustain) → Onset detection algorithm
└── SaveBufferToWave(filename)   → WAV file export

Audio Buffer Architecture (AudioInfo struct)
├── SDL_AudioSpec spec           → Audio format (44.1kHz, 16-bit, mono)
├── Uint8* audio                 → Raw audio data
├── Uint32 audiolen              → Buffer size in bytes
├── Uint32 recordpos             → Current record position
├── Uint32 counter               → Sample counter
└── int volume                   → Volume level
```

### Why It's Disabled

- Uses **SDL_audio_in** third-party library (unmaintained since SDL2 release)
- SDL2 includes native audio recording since v2.0.5 (2016)
- API differs between SDL_audio_in and SDL2
- Never migrated when PEBL moved to SDL2

---

## Implementation Plan

## Phase 1: SDL2 Migration and Basic Restoration

**Goal:** Restore existing functionality using SDL2 standard API
**Effort:** 3-5 days
**Target:** PEBL 2.2 or 2.3

### Part A: SDL2 Audio Capture Migration (2 days)

#### Changes Required

**1. Update PlatformAudioIn.h**

```cpp
class PlatformAudioIn: public PEBLObjectBase
{
public:
    PlatformAudioIn();
    ~PlatformAudioIn();

    bool CreateBuffer(int size_ms);
    bool Initialize(int type);
    bool RecordToBuffer();
    bool Stop();
    Variant VoiceKey(double threshold, unsigned int sustain);
    void SaveBufferToWave(Variant filename);

    AudioInfo * GetAudioOutBuffer();
    AudioInfo * ReleaseAudioOutBuffer();
    bool UseBuffer(AudioInfo * buffer);

private:
    // NEW: Store SDL2 device ID
    SDL_AudioDeviceID mAudioDevice;

    unsigned int mSampleRate;
    Uint16 mAudioFormat;
    unsigned int mBytesPerSample;
    unsigned int mSamples;
    AudioInfo *mWave;

    // Existing helper functions
    double Power(Sint16 * data, int length);
    void ComputeStats(Sint16 * data, int length,
                      double & power, double & energy,
                      int & signchanges, int & dchanges,
                      double & rmssd);
};
```

**2. Update PlatformAudioIn.cpp::Initialize()**

```cpp
bool PlatformAudioIn::Initialize(int type)
{
    // SDL2 audio already initialized in main PEBL startup

    SDL_AudioSpec want, have;
    SDL_zero(want);

    want.freq = mSampleRate;        // 44100
    want.format = mAudioFormat;     // AUDIO_S16
    want.channels = 1;              // Mono
    want.samples = mSamples;        // 256
    want.userdata = &have;

    // Select callback based on type
    if(type == 1) {
        want.callback = AudioInCallbackFill;
    } else {
        want.callback = AudioInCallbackLoop;
    }

    // Get first available recording device
    const char* deviceName = SDL_GetAudioDeviceName(0, SDL_TRUE);
    if(!deviceName) {
        PError::SignalWarning("No audio recording device found");
        return false;
    }

    // Open capture device (SDL_TRUE = recording)
    mAudioDevice = SDL_OpenAudioDevice(deviceName, SDL_TRUE, &want, &have, 0);
    if(mAudioDevice == 0) {
        PError::SignalWarning("Cannot open audio input device: " +
                              Variant(SDL_GetError()));
        return false;
    }

    // Update actual specs if they differ
    mSampleRate = have.freq;
    mAudioFormat = have.format;

    return true;
}
```

**3. Update PlatformAudioIn.cpp::RecordToBuffer()**

```cpp
bool PlatformAudioIn::RecordToBuffer()
{
    if(mAudioDevice == 0) {
        PError::SignalWarning("Audio device not initialized");
        return false;
    }

    SDL_PauseAudioDevice(mAudioDevice, 0);  // 0 = unpause/start
    return true;
}
```

**4. Update PlatformAudioIn.cpp::Stop()**

```cpp
bool PlatformAudioIn::Stop()
{
    if(mAudioDevice == 0) {
        return false;
    }

    SDL_PauseAudioDevice(mAudioDevice, 1);  // 1 = pause/stop
    return true;
}
```

**5. Update PlatformAudioIn.cpp::~PlatformAudioIn()**

```cpp
PlatformAudioIn::~PlatformAudioIn()
{
    // Close audio device
    if(mAudioDevice > 0) {
        SDL_CloseAudioDevice(mAudioDevice);
        mAudioDevice = 0;
    }

    // Clean up audio buffer
    if(mWave) {
        if(mWave == gAudioBuffer) {
            gAudioBuffer = NULL;
        }
        if(mWave->audio) {
            delete mWave->audio;
        }
        delete mWave;
    }
}
```

**6. Initialize mAudioDevice in Constructor**

```cpp
PlatformAudioIn::PlatformAudioIn()
{
    mAudioDevice = 0;  // NEW: Initialize to 0
    mSampleRate = 44100;
    mAudioFormat = AUDIO_S16;
    mBytesPerSample = 2;
    mSamples = 256;
    mWave = NULL;
}
```

**7. Update Makefile**

```makefile
# Enable audio input compilation
USE_AUDIOIN=1

# Ensure it's added to CXXFLAGS
ifeq ($(USE_AUDIOIN),1)
    CXXFLAGS_LINUX   += -DPEBL_AUDIOIN
    CXXFLAGS_EMSCRIPTEN += -DPEBL_AUDIOIN
    CXXFLAGS_WINDOWS += -DPEBL_AUDIOIN
endif
```

#### Testing Plan for Part A

**Test Script: `demo/testaudioin.pbl`**

```pebl
define Start(p) {
    gWin <- MakeWindow("black")
    font <- MakeFont("Vera.ttf", 0, 24, MakeColor("white"),
                     MakeColor("black"), 1)

    inst <- MakeLabel("Click to record 3 seconds", font)
    AddObject(inst, gWin)
    Move(inst, 400, 300)
    Draw()

    WaitForClickOnTarget([inst], [1])

    ## Create 3-second buffer
    buffer <- MakeAudioInputBuffer(3000)

    inst.text <- "Recording..."
    Draw()

    ## Record with voice key detection
    rt <- GetVocalResponseTime(buffer, 0.25, 200)

    ## Save to file
    SaveAudioToWaveFile("test_recording.wav", buffer)

    ## Display results
    inst.text <- "Onset: " + First(rt) + " ms" + CR(1) +
                 "Offset: " + Second(rt) + " ms" + CR(1) +
                 "Triggered: " + Third(rt) + CR(1) +
                 "Saved to test_recording.wav"
    Draw()

    WaitForAnyKeyPress()
}
```

**Verification Checklist:**
- [ ] Compiles on Linux with `USE_AUDIOIN=1`
- [ ] Opens microphone without errors
- [ ] Records 3 seconds of audio
- [ ] Voice key triggers on speech
- [ ] Saves valid WAV file
- [ ] Playback of WAV file sounds correct
- [ ] No memory leaks (valgrind)
- [ ] Works on Windows (after Linux validation)
- [ ] Works on macOS (after Linux validation)

---

### Part B: Enhanced Audio Capabilities (1-2 days)

#### New Feature 1: Audio Playback from Buffer

**Goal:** Allow playback of recorded audio without saving to disk first

**New PEBL Function:**

```pebl
## Play recorded audio buffer
PlayAudioBuffer(buffer)
```

**Implementation:**

The current code already does this! In `MakeAudioInputBuffer()` (PEBLObjects.cpp:1228-1230):

```cpp
PlatformAudioOut * myOut = new PlatformAudioOut();
myOut->LoadSoundFromData((tmp->audio),tmp->audiolen,&(tmp->spec));
myOut->Initialize();
```

The buffer is converted to a `PlatformAudioOut` object, so it can already be played:

```pebl
buffer <- MakeAudioInputBuffer(3000)
rt <- GetVocalResponseTime(buffer, 0.25, 200)

## Buffer is already a PlatformAudioOut object!
PlayBackground(buffer)  ## This should already work!
```

**Action:** Test and document this capability. No new code needed!

---

#### New Feature 2: Audio Visualization

**Goal:** Real-time waveform display for debugging and feedback

**New PEBL Function:**

```pebl
## Draw waveform on canvas
DrawWaveform(buffer, canvas, color)
```

**Implementation:**

```cpp
// In PEBLObjects.cpp
Variant PEBLObjects::DrawWaveform(Variant v)
{
#ifdef PEBL_AUDIOIN
    PList * plist = v.GetComplexData()->GetList();

    // Get audio buffer
    Variant v1 = plist->First();
    PError::AssertType(v1, PEAT_AUDIOOUT, "Argument error in first parameter of function [DrawWaveform(<buffer>, <canvas>, <color>)]: ");
    PlatformAudioOut * audioOut = dynamic_cast<PlatformAudioOut*>(v1.GetComplexData()->GetObject().get());

    // Get canvas
    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_WIDGET, "Argument error in second parameter of function [DrawWaveform(<buffer>, <canvas>, <color>)]: ");
    PlatformCanvas * canvas = dynamic_cast<PlatformCanvas*>(v2.GetComplexData()->GetObject().get());

    // Get color
    Variant v3 = plist->Nth(3);
    PError::AssertType(v3, PEAT_COLOR, "Argument error in third parameter of function [DrawWaveform(<buffer>, <canvas>, <color>)]: ");
    PColor color = *((PColor*)(v3.GetComplexData()->GetObject().get()));

    // Get audio data
    AudioInfo * audioInfo = audioOut->GetAudioInfo();
    Sint16 * samples = (Sint16*)(audioInfo->audio);
    int numSamples = audioInfo->audiolen / audioInfo->bytesPerSample;

    // Get canvas dimensions
    int width = canvas->GetWidth();
    int height = canvas->GetHeight();
    int centerY = height / 2;

    // Downsample to fit canvas width
    int samplesPerPixel = numSamples / width;
    if(samplesPerPixel < 1) samplesPerPixel = 1;

    // Draw waveform
    for(int x = 0; x < width; x++)
    {
        int sampleIndex = x * samplesPerPixel;
        if(sampleIndex >= numSamples) break;

        // Get min/max in this pixel range
        Sint16 minVal = 32767;
        Sint16 maxVal = -32768;
        for(int i = 0; i < samplesPerPixel && (sampleIndex + i) < numSamples; i++)
        {
            Sint16 val = samples[sampleIndex + i];
            if(val < minVal) minVal = val;
            if(val > maxVal) maxVal = val;
        }

        // Scale to canvas height
        int y1 = centerY - (minVal * height / 2 / 32768);
        int y2 = centerY - (maxVal * height / 2 / 32768);

        // Draw vertical line from min to max
        for(int y = y1; y <= y2; y++)
        {
            canvas->SetPoint(x, y, color);
        }
    }

    return Variant(true);
#else
    PError::SignalFatalError("Audio Input not available on this version of PEBL.");
    return false;
#endif
}
```

**Add to Functions.h:**

```cpp
{(char*)"DRAWWAVEFORM", DrawWaveform, 3, 3},
```

**Usage Example:**

```pebl
buffer <- MakeAudioInputBuffer(5000)
rt <- GetVocalResponseTime(buffer, 0.25, 200)

canvas <- MakeCanvas(800, 200, MakeColor("black"))
DrawWaveform(buffer, canvas, MakeColor("green"))
AddObject(canvas, gWin)
Move(canvas, 400, 300)
Draw()
```

---

#### New Feature 3: Get Audio Metrics

**Goal:** Extract amplitude, RMS, zero-crossing rate for analysis

**New PEBL Function:**

```pebl
## Get audio statistics
stats <- GetAudioMetrics(buffer)
## Returns: [rms, peak_amplitude, zero_crossings, duration_ms]
```

**Implementation:**

```cpp
// In PEBLObjects.cpp
Variant PEBLObjects::GetAudioMetrics(Variant v)
{
#ifdef PEBL_AUDIOIN
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();
    PError::AssertType(v1, PEAT_AUDIOOUT, "Argument error in function [GetAudioMetrics(<buffer>)]: ");

    PlatformAudioOut * audioOut = dynamic_cast<PlatformAudioOut*>(v1.GetComplexData()->GetObject().get());
    AudioInfo * audioInfo = audioOut->GetAudioInfo();

    Sint16 * samples = (Sint16*)(audioInfo->audio);
    int numSamples = audioInfo->audiolen / audioInfo->bytesPerSample;

    // Calculate metrics
    double sumSq = 0.0;
    Sint16 peak = 0;
    int zeroCrossings = 0;
    Sint16 prevSample = 0;

    for(int i = 0; i < numSamples; i++)
    {
        Sint16 sample = samples[i];

        // RMS calculation
        double normalized = (double)sample / 32768.0;
        sumSq += normalized * normalized;

        // Peak amplitude
        Sint16 absSample = abs(sample);
        if(absSample > peak) peak = absSample;

        // Zero crossings
        if(i > 0 && (prevSample * sample) < 0)
        {
            zeroCrossings++;
        }

        prevSample = sample;
    }

    double rms = sqrt(sumSq / numSamples);
    double peakNorm = (double)peak / 32768.0;
    double durationMs = (double)numSamples / audioInfo->spec.freq * 1000.0;

    // Return list
    PList * result = new PList();
    result->PushBack(Variant(rms));
    result->PushBack(Variant(peakNorm));
    result->PushBack(Variant(zeroCrossings));
    result->PushBack(Variant(durationMs));

    counted_ptr<PEBLObjectBase> resultBase = counted_ptr<PEBLObjectBase>(result);
    PComplexData * pcd = new PComplexData(resultBase);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd = NULL;

    return tmp;
#else
    PError::SignalFatalError("Audio Input not available on this version of PEBL.");
    return false;
#endif
}
```

**Add to Functions.h:**

```cpp
{(char*)"GETAUDIOMETRICS", GetAudioMetrics, 1, 1},
```

**Usage Example:**

```pebl
buffer <- MakeAudioInputBuffer(3000)
rt <- GetVocalResponseTime(buffer, 0.25, 200)

stats <- GetAudioMetrics(buffer)
Print("RMS: " + First(stats))
Print("Peak: " + Second(stats))
Print("Zero crossings: " + Third(stats))
Print("Duration: " + Fourth(stats) + " ms")
```

---

#### New Feature 4: Continuous Recording Mode

**Goal:** Allow live monitoring without blocking

**New PEBL Functions:**

```pebl
## Start continuous recording (non-blocking)
StartRecording(buffer)

## Check if voice key has triggered
triggered <- CheckVoiceKey(buffer, threshold, sustain)

## Stop recording
StopRecording(buffer)
```

**Implementation:**

This requires making recording asynchronous. The current `GetVocalResponseTime()` blocks until voice key triggers or buffer fills.

**Approach:** Add recording state to PlatformAudioIn

```cpp
// In PlatformAudioIn.h
private:
    bool mIsRecording;
    bool mVoiceKeyTripped;
    double mVoiceKeyThreshold;
    unsigned int mVoiceKeySustain;
    unsigned int mTripTime;
    unsigned int mOffTime;
```

**StartRecording() implementation:**

```cpp
bool PlatformAudioIn::StartRecording()
{
    if(!mWave) {
        PError::SignalWarning("No buffer attached to audio input");
        return false;
    }

    // Reset state
    mWave->recordpos = 0;
    mWave->counter = 0;
    memset(mWave->audio, 0, mWave->audiolen);

    mIsRecording = true;
    mVoiceKeyTripped = false;

    // Start audio device
    SDL_PauseAudioDevice(mAudioDevice, 0);

    return true;
}
```

**CheckVoiceKey() implementation:**

```cpp
Variant PlatformAudioIn::CheckVoiceKey(double threshold, unsigned int sustain)
{
    // Process recorded samples so far
    // Return [onset, offset, triggered] or [0, 0, false] if not yet triggered

    // This would be a non-blocking version of VoiceKey()
    // that processes available data without waiting

    // Implementation similar to VoiceKey() but only processes
    // samples up to mWave->recordpos
}
```

**Effort:** 1-2 days
**Benefit:** Allows real-time feedback and live experiments

---

### Part C: Voice Key Enhancements (1 day)

#### Enhancement 1: Configurable Detection Parameters

**Goal:** Expose the 55%/20% thresholds as parameters

**Current Code (PlatformAudioIn.cpp:448-460):**

```cpp
if(((double)abovecount)/sustainSamples > .55  &trip==false)
{
    trip = true;
    triptime = tickID -(sustainSamples*.55);
}

if(trip)
{
    if((double)abovecount/sustainSamples < .2)
    {
        Stop();
        stop = true;
        offtime = tickID- (sustainSamples*.8);
    }
}
```

**New Signature:**

```pebl
rt <- GetVocalResponseTime(buffer, threshold, sustain, trip_pct, release_pct)
## trip_pct: percentage of sustain window above threshold to trigger (default 0.55)
## release_pct: percentage of sustain window above threshold to maintain (default 0.20)
```

**Implementation:**

```cpp
Variant PlatformAudioIn::VoiceKey(double threshold, unsigned int sustain,
                                  double tripPercent, double releasePercent)
{
    // Default parameters if not provided
    if(tripPercent == 0.0) tripPercent = 0.55;
    if(releasePercent == 0.0) releasePercent = 0.20;

    // ... existing code ...

    if(((double)abovecount)/sustainSamples > tripPercent && trip==false)
    {
        trip = true;
        triptime = tickID - (sustainSamples * tripPercent);
    }

    if(trip)
    {
        if((double)abovecount/sustainSamples < releasePercent)
        {
            Stop();
            stop = true;
            offtime = tickID - (sustainSamples * (1.0 - releasePercent));
        }
    }
}
```

---

#### Enhancement 2: Return Additional Voice Key Metrics

**Current Return:** `[onset_ms, offset_ms, triggered]`

**Enhanced Return:** `[onset_ms, offset_ms, duration_ms, peak_amplitude, triggered]`

**Implementation:**

```cpp
Variant PlatformAudioIn::VoiceKey(double threshold, unsigned int sustain)
{
    // ... existing voice key algorithm ...

    // Calculate additional metrics
    double peakAmplitude = 0.0;
    for(size_t i = 0; i < powerbins.size(); i++)
    {
        if(powerbins[i] > peakAmplitude)
        {
            peakAmplitude = powerbins[i];
        }
    }

    double duration = (offtime - triptime) * msperchunk;

    // Return enhanced list
    PList * newlist = new PList();
    newlist->PushBack(Variant(triptime * msperchunk));
    newlist->PushBack(Variant(offtime * msperchunk));
    newlist->PushBack(Variant(duration));
    newlist->PushBack(Variant(peakAmplitude));
    newlist->PushBack(Variant(trip));

    // ... rest of code ...
}
```

**Usage:**

```pebl
rt <- GetVocalResponseTime(buffer, 0.25, 200)

onset <- rt[1]
offset <- rt[2]
duration <- rt[3]
amplitude <- rt[4]
triggered <- rt[5]

Print("Response time: " + onset + " ms")
Print("Duration: " + duration + " ms")
Print("Amplitude: " + amplitude)
```

---

## Phase 2: Advanced Features (Optional - PEBL 3.0)

**Goal:** Integrate WebRTC VAD for robust voice detection
**Effort:** 2-3 days
**Target:** PEBL 3.0

See `doc/VOICE_KEY_ANALYSIS.md` for WebRTC VAD integration details.

**Key Benefits:**
- Robust to background noise
- Frequency-aware (focuses on speech frequencies)
- Multiple aggressiveness modes
- Production-tested (used in Chrome/WebRTC)

---

## Testing Strategy

### Unit Tests

**Test 1: Basic Recording**
```pebl
buffer <- MakeAudioInputBuffer(1000)
## Verify: Buffer created, no crash
```

**Test 2: Silent Recording**
```pebl
buffer <- MakeAudioInputBuffer(1000)
rt <- GetVocalResponseTime(buffer, 0.25, 200)
## Expected: [0, 0, false] (no trigger)
SaveAudioToWaveFile("silent.wav", buffer)
## Verify: WAV file is valid but silent
```

**Test 3: Voice Key Trigger**
```pebl
buffer <- MakeAudioInputBuffer(3000)
## User speaks after 1 second
rt <- GetVocalResponseTime(buffer, 0.25, 200)
## Expected: onset around 1000ms, triggered = true
```

**Test 4: Playback**
```pebl
buffer <- MakeAudioInputBuffer(2000)
rt <- GetVocalResponseTime(buffer, 0.25, 200)
PlayBackground(buffer)
## Expected: Hear recorded audio
```

**Test 5: Waveform Display**
```pebl
buffer <- MakeAudioInputBuffer(2000)
rt <- GetVocalResponseTime(buffer, 0.25, 200)
canvas <- MakeCanvas(800, 200, MakeColor("black"))
DrawWaveform(buffer, canvas, MakeColor("green"))
## Expected: Visual waveform on canvas
```

### Integration Tests

**Stroop Task with Voice Key:**

```pebl
define Start(p) {
    gWin <- MakeWindow("white")
    font <- MakeFont("Vera.ttf", 0, 48, MakeColor("red"),
                     MakeColor("white"), 1)

    stim <- MakeLabel("GREEN", font)
    AddObject(stim, gWin)
    Move(stim, 400, 300)

    buffer <- MakeAudioInputBuffer(3000)

    trials <- ["RED", "BLUE", "GREEN", "YELLOW"]
    colors <- [MakeColor("red"), MakeColor("blue"),
               MakeColor("green"), MakeColor("yellow")]

    fileOut <- FileOpenWrite("stroop_voice.csv")
    FilePrint(fileOut, "trial,word,color,onset_ms,offset_ms,duration_ms,triggered")

    loop(i, Sequence(1, 10, 1))
    {
        word <- SampleN(trials, 1)[1]
        color <- SampleN(colors, 1)[1]

        stim.text <- word
        SetFont(stim, MakeFont("Vera.ttf", 0, 48, color, MakeColor("white"), 1))
        Draw()

        Wait(500)

        rt <- GetVocalResponseTime(buffer, 0.25, 200)

        FilePrint(fileOut, i + "," + word + "," + GetProperty(color, "NAME") + "," +
                  rt[1] + "," + rt[2] + "," + (rt[2] - rt[1]) + "," + rt[5])

        Wait(1000)
    }

    FileClose(fileOut)
}
```

### Platform Tests

- [ ] Linux (Ubuntu 22.04+, Fedora 38+)
- [ ] Windows 10/11
- [ ] macOS (Intel and Apple Silicon)
- [ ] Emscripten (browser audio capture with getUserMedia)

### Performance Tests

**Latency Test:**
- Measure time from voice onset to detection
- Target: <100ms latency
- Method: Use known audio file with precise onset

**CPU Usage:**
- Monitor CPU during 5-minute continuous recording
- Target: <5% CPU on modern hardware

**Memory:**
- Check for memory leaks during 100 record cycles
- Tool: valgrind on Linux

---

## Emscripten Considerations

SDL2 audio capture **does work** in Emscripten using the Web Audio API.

**Browser API Used:** `navigator.mediaDevices.getUserMedia()`

**Permissions:** Browser will prompt user for microphone access

**Example Test:**

```html
<script>
navigator.mediaDevices.getUserMedia({ audio: true })
    .then(stream => {
        console.log("Microphone access granted");
    })
    .catch(err => {
        console.error("Microphone access denied:", err);
    });
</script>
```

**Testing Plan:**
1. Test on localhost first (Chrome, Firefox, Safari)
2. Test on HTTPS server (required for production)
3. Handle permission denial gracefully
4. Add UI prompt explaining microphone requirement

---

## Documentation Updates

### PEBL Manual Additions

**Chapter: Audio Input and Voice Key**

```
# Audio Recording

PEBL can record audio from the system microphone for voice key experiments
and audio analysis.

## Basic Recording

buffer <- MakeAudioInputBuffer(duration_ms)

Creates an audio buffer that records for `duration_ms` milliseconds at 44.1kHz,
16-bit, mono. The buffer begins recording when used with GetVocalResponseTime().

## Voice Key Detection

rt <- GetVocalResponseTime(buffer, threshold, sustain_ms)

Records audio and detects voice onset time. Returns a list:
- rt[1]: Voice onset time in milliseconds
- rt[2]: Voice offset time in milliseconds
- rt[3]: Duration in milliseconds
- rt[4]: Peak amplitude (0.0-1.0)
- rt[5]: Boolean - did voice key trigger?

Parameters:
- threshold: Energy threshold (0.0-1.0), typically 0.1-0.5
- sustain_ms: Minimum duration above threshold, typically 50-300ms

## Saving Recordings

SaveAudioToWaveFile(filename, buffer)

Exports recorded audio to a WAV file.

## Playback

PlayBackground(buffer)

Plays recorded audio buffer.

## Visualization

DrawWaveform(buffer, canvas, color)

Draws waveform visualization on a canvas.

## Audio Metrics

stats <- GetAudioMetrics(buffer)

Returns:
- stats[1]: RMS amplitude
- stats[2]: Peak amplitude
- stats[3]: Zero crossing count
- stats[4]: Duration in milliseconds

# Example: Simple Voice RT Task

define Start(p) {
    gWin <- MakeWindow("white")
    font <- MakeFont("Vera.ttf", 0, 36, MakeColor("black"),
                     MakeColor("white"), 1)

    stim <- MakeLabel("", font)
    AddObject(stim, gWin)
    Move(stim, 400, 300)

    buffer <- MakeAudioInputBuffer(3000)

    words <- ["CAT", "DOG", "HOUSE", "TREE"]

    fileOut <- FileOpenWrite("voicert.csv")
    FilePrint(fileOut, "trial,word,rt_ms,duration_ms")

    loop(i, Sequence(1, Length(words), 1))
    {
        stim.text <- words[i]
        Draw()

        rt <- GetVocalResponseTime(buffer, 0.25, 200)

        if(rt[5])  ## If voice key triggered
        {
            FilePrint(fileOut, i + "," + words[i] + "," +
                     rt[1] + "," + rt[3])
            SaveAudioToWaveFile("recordings/trial_" + i + ".wav", buffer)
        }

        Wait(1500)
    }

    FileClose(fileOut)
}
```

### Example Battery Task Update

Update `battery/stroop/stroop.pbl` to support voice key mode:

```pebl
## In params/stroop.pbl.schema
mode|keyboard|Response mode: keyboard or voice
voice_threshold|0.25|Voice key energy threshold (0.0-1.0)
voice_sustain|200|Voice key sustain duration (ms)

## In stroop.pbl
if(gParams.mode == "voice")
{
    buffer <- MakeAudioInputBuffer(gParams.responsedur)
    rt <- GetVocalResponseTime(buffer, gParams.voice_threshold,
                               gParams.voice_sustain)
    response <- "VOICE"
    corr <- rt[5]  ## Triggered = correct
    SaveAudioToWaveFile(gFileBase + "_trial_" + trial + ".wav", buffer)
}
else
{
    response <- WaitForListKeyPress(gParams.respkeys)
    rt <- GetTime() - time0
    corr <- Lookup(response, gParams.respkeys, [1,1,0,0])
}
```

---

## Implementation Checklist

### Phase 1A: SDL2 Migration

- [ ] Update `PlatformAudioIn.h` with `SDL_AudioDeviceID mAudioDevice`
- [ ] Update `PlatformAudioIn::Initialize()` to use SDL2 API
- [ ] Update `PlatformAudioIn::RecordToBuffer()` to use device ID
- [ ] Update `PlatformAudioIn::Stop()` to use device ID
- [ ] Update `PlatformAudioIn::~PlatformAudioIn()` to close device
- [ ] Initialize `mAudioDevice = 0` in constructor
- [ ] Enable `USE_AUDIOIN=1` in Makefile
- [ ] Test compilation on Linux
- [ ] Run `demo/testaudioin.pbl` test
- [ ] Verify WAV file output
- [ ] Check for memory leaks with valgrind
- [ ] Test on Windows
- [ ] Test on macOS

### Phase 1B: Enhanced Features

- [ ] Test audio playback (already implemented)
- [ ] Implement `DrawWaveform()` function
- [ ] Add `DRAWWAVEFORM` to Functions.h
- [ ] Implement `GetAudioMetrics()` function
- [ ] Add `GETAUDIOMETRICS` to Functions.h
- [ ] Create waveform visualization test
- [ ] Create metrics analysis test

### Phase 1C: Voice Key Enhancements

- [ ] Add configurable trip/release percentages
- [ ] Update `GetVocalResponseTime()` signature
- [ ] Add duration and amplitude to return value
- [ ] Update PEBL manual with new parameters
- [ ] Create voice key calibration tool
- [ ] Test with various threshold/sustain values

### Documentation

- [ ] Update PEBL Manual Chapter on Audio Input
- [ ] Create voice key tutorial
- [ ] Add example battery task (Stroop with voice)
- [ ] Document Emscripten microphone permissions
- [ ] Add troubleshooting guide
- [ ] Create video tutorial for voice key setup

---

## Risk Assessment

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| SDL2 audio capture API differs across platforms | Medium | High | Test on all platforms early |
| Microphone permissions on Emscripten | High | Medium | Clear documentation, permission handling |
| Voice key accuracy in noisy environments | High | Medium | Document threshold tuning, consider WebRTC VAD later |
| Latency too high for RT experiments | Low | High | Profile and optimize, target <100ms |
| Memory leaks in audio buffer management | Medium | High | Valgrind testing, careful buffer lifecycle |

---

## Success Criteria

**Phase 1A Complete When:**
- ✅ Compiles on Linux, Windows, macOS
- ✅ Records audio from default microphone
- ✅ Voice key triggers reliably (>90% accuracy in quiet environment)
- ✅ Saves valid WAV files
- ✅ No memory leaks
- ✅ Latency <100ms from onset to detection

**Phase 1B Complete When:**
- ✅ All Phase 1A criteria met
- ✅ Waveform visualization works
- ✅ Audio metrics extraction works
- ✅ Playback works correctly

**Phase 1C Complete When:**
- ✅ All Phase 1A/1B criteria met
- ✅ Configurable detection parameters work
- ✅ Enhanced return values provide useful metrics
- ✅ Documentation updated

---

## Timeline Estimate

| Task | Duration | Dependencies |
|------|----------|--------------|
| SDL2 migration (1A) | 2 days | None |
| Testing & debugging | 1 day | 1A |
| Enhanced features (1B) | 1-2 days | 1A |
| Voice key enhancements (1C) | 1 day | 1A |
| Documentation | 1 day | All above |
| **Total Phase 1** | **5-6 days** | |
| WebRTC VAD (Phase 2) | 2-3 days | Phase 1 |

---

## References

- SDL2 Audio Capture: https://wiki.libsdl.org/SDL2/SDL_OpenAudioDevice
- VOICE_KEY_ANALYSIS.md: Detailed voice key algorithm analysis
- demo/testaudioin.pbl: Existing test script
- PlatformAudioIn.cpp: Current implementation
- PlatformAudioOut.h: Audio buffer architecture
