# Voice Key Implementation Analysis and Modernization

**Date:** 2025-11-18
**Purpose:** Analyze existing PEBL voice key implementation and identify modern alternatives

---

## Executive Summary

PEBL had a working voice key implementation based on SDL audio input that has fallen out of maintenance. The implementation used custom SDL audio functions (`SDL_InitAudioIn`, `SDL_OpenAudioIn`, etc.) that are not part of standard SDL2.

**Current Status:**
- ⚠️ Voice key code exists but is **disabled** (USE_AUDIOIN commented out in Makefile)
- ⚠️ Uses custom SDL_audioin functions not in standard SDL2
- ⚠️ Comments suggest it was based on early Audacity code (pre-2.0)
- ✅ Algorithm is solid - energy-based onset detection with sustain thresholds
- ✅ Test demo exists (`demo/testaudioin.pbl`)

**Recommendation:**
- **Short-term:** Port to SDL2 standard audio capture API (SDL_OpenAudioDevice)
- **Long-term:** Integrate lightweight VAD library (WebRTC VAD) for improved accuracy

---

## Existing Implementation Analysis

### File Locations

| File | Purpose | Status |
|------|---------|--------|
| `src/platforms/sdl/PlatformAudioIn.h` | Header for audio input | ✅ Exists |
| `src/platforms/sdl/PlatformAudioIn.cpp` | Voice key implementation | ✅ Exists (702 lines) |
| `demo/testaudioin.pbl` | Test demo script | ✅ Exists |
| Makefile line 38 | `#USE_AUDIOIN=1` | ❌ Commented out |

### Architecture

```
PlatformAudioIn (PEBLObjectBase)
├── CreateBuffer(int ms)        - Allocate audio buffer
├── Initialize(int type)        - Open audio device
├── RecordToBuffer()            - Start recording (unpause)
├── Stop()                      - Stop recording (pause)
├── VoiceKey(threshold, sustain) - Main voice key algorithm
├── SaveBufferToWave(filename)  - Export to WAV
├── ComputeStats(...)           - Compute energy/power stats
└── Power(...)                  - Compute signal power

Callbacks:
├── AudioInCallbackFill()       - Fill buffer from mic
└── AudioInCallbackLoop()       - Continuous loop mode
```

### Voice Key Algorithm (src/platforms/sdl/PlatformAudioIn.cpp:304-510)

**Algorithm:** Energy-based onset detection with sustain threshold

**Parameters:**
- `threshold` (double): Minimum energy level to trigger (typically 0.1-0.5)
- `sustain` (unsigned int): Minimum duration in ms to confirm onset (typically 50-300ms)

**Process:**
1. Create 1ms bins of audio data
2. Compute energy for each bin using `ComputeStats()`:
   - Energy: Mean absolute amplitude
   - Power: RMS amplitude
   - Sign changes: Zero-crossing rate
   - Direction changes: Slope changes
   - RMSSD: Root mean square of successive differences
3. Track sliding window of `sustain` ms
4. Trip when 55% of bins in window exceed threshold
5. Stop when 20% of bins in window exceed threshold (after trip)
6. Reprocess to find exact onset time

**Return Value:** `[triptime, offtime, tripped]`
- `triptime`: Voice onset time in ms
- `offtime`: Voice offset time in ms
- `tripped`: Boolean - did voice key trigger?

**Strengths:**
- ✅ Multiple acoustic features (energy, power, zero-crossings)
- ✅ Sliding window prevents false triggers
- ✅ Saves WAV files for verification
- ✅ Configurable threshold and sustain

**Weaknesses:**
- ⚠️ Fixed 55%/20% thresholds (should be configurable)
- ⚠️ No noise floor adaptation
- ⚠️ Simple energy-based detection (not robust to background noise)
- ⚠️ No frequency filtering (voice is typically 80-300 Hz fundamental)

### SDL Audio Functions Used

**Non-standard SDL functions** (not in SDL2):

```cpp
SDL_InitAudioIn()          // Line 121
SDL_OpenAudioIn(&want, &got) // Line 149
SDL_CloseAudioIn()         // Line 108
SDL_PauseAudioIn(0/1)      // Lines 285, 292
```

**Comments in code:**
```cpp
// Line 38: //#include "SDL/SDL_audioin.h"  //now handled directly.
// Line 54: //#include "SDL_audioin.h"
```

**Origin:** These functions come from the **SDL_audio_in library**, a third-party extension library for SDL 1.2 that provided audio recording before SDL2 had native support. SDL_audio_in is no longer maintained since SDL2 includes this functionality natively.

---

## SDL2 Standard Audio Capture

SDL 2.0.5+ includes built-in audio recording via the standard SDL API.

### Standard SDL2 Approach

```cpp
// No custom SDL_InitAudioIn needed - use standard SDL_Init(SDL_INIT_AUDIO)

// Open recording device
SDL_AudioDeviceID dev;
SDL_AudioSpec want, have;

SDL_zero(want);
want.freq = 44100;
want.format = AUDIO_S16SYS;
want.channels = 1;
want.samples = 256;
want.callback = AudioCaptureCallback;

// Get recording device name (SDL2 standard)
const char* deviceName = SDL_GetAudioDeviceName(0, SDL_TRUE);  // SDL_TRUE = capture

// Open capture device (SDL2 standard)
dev = SDL_OpenAudioDevice(deviceName, SDL_TRUE, &want, &have, 0);

// Start recording (SDL2 standard)
SDL_PauseAudioDevice(dev, 0);  // 0 = unpause

// Stop recording (SDL2 standard)
SDL_PauseAudioDevice(dev, 1);  // 1 = pause

// Close (SDL2 standard)
SDL_CloseAudioDevice(dev);
```

### Key Differences from SDL_audio_in to SDL2

| SDL_audio_in (Third-party) | SDL2 Standard API |
|---------------------|---------------|
| `SDL_InitAudioIn()` | `SDL_Init(SDL_INIT_AUDIO)` (already done in PEBL) |
| `SDL_OpenAudioIn(&want, &got)` | `SDL_OpenAudioDevice(name, SDL_TRUE, &want, &got, 0)` |
| `SDL_CloseAudioIn()` | `SDL_CloseAudioDevice(dev)` |
| `SDL_PauseAudioIn(0/1)` | `SDL_PauseAudioDevice(dev, 0/1)` |
| Global audio device | Device ID returned from `SDL_OpenAudioDevice()` |

**Key Conceptual Difference:**
- **SDL_audio_in:** Single global recording device (like SDL 1.2 audio playback)
- **SDL2:** Multiple devices via device IDs (same pattern as playback)

### Conversion Effort

**Changes Required:**
1. Replace custom SDL_audioin calls with SDL2 standard API
2. Store `SDL_AudioDeviceID` instead of relying on global state
3. Update callback signatures (same as current)
4. Test on Linux, Windows, macOS

**Estimated Effort:** 1-2 days

**Benefits:**
- ✅ Works on all SDL2-supported platforms
- ✅ No custom SDL patches needed
- ✅ Maintained by SDL team
- ✅ Works in Emscripten (with browser permissions)

---

## Modern VAD/Voice Key Libraries

### Option 1: WebRTC VAD (Recommended)

**Source:** https://chromium.googlesource.com/external/webrtc/+/master/common_audio/vad/

**License:** BSD-style (compatible with PEBL GPL)

**Language:** Pure C

**Features:**
- Gaussian Mixture Model (GMM) based
- Frame-level VAD (10ms, 20ms, or 30ms frames)
- 3 aggressiveness modes (quality, low bitrate, aggressive)
- Supports 8kHz, 16kHz, 32kHz, 48kHz
- Very lightweight (~20KB compiled)
- No external dependencies
- Production-tested (used in Google Chrome, WebRTC)

**API Example:**
```c
#include "webrtc/common_audio/vad/include/webrtc_vad.h"

VadInst *vad;
WebRtcVad_Create(&vad);
WebRtcVad_Init(vad);
WebRtcVad_set_mode(vad, 2);  // 0=quality, 1=low bitrate, 2=aggressive, 3=very aggressive

// Process 10ms frames at 16kHz (160 samples)
int16_t frame[160];
int is_speech = WebRtcVad_Process(vad, 16000, frame, 160);

WebRtcVad_Free(vad);
```

**Integration with PEBL:**
```cpp
// In PlatformAudioIn::VoiceKey()

VadInst *vad;
WebRtcVad_Create(&vad);
WebRtcVad_Init(vad);
WebRtcVad_set_mode(vad, 2);

int frame_length = 160;  // 10ms at 16kHz
int onset_frames = 0;
int sustain_frames = sustain / 10;  // Convert ms to 10ms frames

for(int i = 0; i < bufferSamples; i += frame_length)
{
    int is_speech = WebRtcVad_Process(vad, 16000,
                                      (int16_t*)(buffer + i*2),
                                      frame_length);

    if(is_speech) {
        onset_frames++;
        if(onset_frames >= sustain_frames && !tripped) {
            triptime = i / 16;  // Convert samples to ms
            tripped = true;
        }
    } else {
        onset_frames = 0;
    }
}

WebRtcVad_Free(vad);
```

**Pros:**
- ✅ Industry-standard VAD
- ✅ Much more robust than energy-based detection
- ✅ Lightweight and fast
- ✅ No dependencies
- ✅ Cross-platform

**Cons:**
- ⚠️ Requires downsampling to 16kHz (PEBL uses 44.1kHz)
- ⚠️ Adds ~20KB to binary
- ⚠️ More complex integration than simple energy threshold

**Effort:** 2-3 days

---

### Option 2: OnsetsDS

**Source:** https://onsetsds.sourceforge.net/

**License:** GPL v2+ (compatible with PEBL)

**Language:** C

**Features:**
- FFT-based onset detection
- Optimized for musical onsets
- Real-time processing
- Multiple onset detection algorithms

**Pros:**
- ✅ GPL compatible
- ✅ Designed for onset detection specifically

**Cons:**
- ⚠️ Requires FFT library (FFTW)
- ⚠️ More heavyweight than WebRTC VAD
- ⚠️ Designed for music, not voice

**Effort:** 3-5 days (due to FFT dependency)

---

### Option 3: TEN-VAD

**Source:** https://github.com/TEN-framework/ten-vad

**License:** Apache 2.0

**Language:** C

**Features:**
- Real-time streaming VAD
- Lower latency than Silero VAD
- Cross-platform (Linux, Windows, macOS, Android, iOS)
- Frame-level detection

**Pros:**
- ✅ Modern implementation
- ✅ Low latency
- ✅ Cross-platform

**Cons:**
- ⚠️ Apache 2.0 may have GPL compatibility issues
- ⚠️ Newer, less battle-tested
- ⚠️ May have more dependencies

**Effort:** Unknown (need to examine code)

---

### Option 4: Keep Current Algorithm + SDL2 Port

**Approach:** Just port to SDL2 standard API, keep energy-based algorithm

**Pros:**
- ✅ Minimal changes
- ✅ No new dependencies
- ✅ Already tested and working
- ✅ Simple and understandable

**Cons:**
- ⚠️ Less robust than ML-based VAD
- ⚠️ Susceptible to background noise
- ⚠️ No frequency filtering

**Effort:** 1-2 days

---

## Comparison Matrix

| Feature | Current (Broken) | SDL2 Port Only | + WebRTC VAD | + OnsetsDS |
|---------|-----------------|----------------|--------------|------------|
| **SDL2 Compatible** | ❌ No | ✅ Yes | ✅ Yes | ✅ Yes |
| **Effort** | N/A | 1-2 days | 2-3 days | 3-5 days |
| **Dependencies** | Custom SDL | SDL2 | SDL2 + WebRTC | SDL2 + FFTW |
| **Binary Size** | N/A | +0KB | +20KB | +100KB+ |
| **Robustness** | Medium | Medium | High | Medium |
| **Noise Resistance** | Low | Low | High | Medium |
| **Emscripten Support** | ❌ No | ✅ Yes | ⚠️ Maybe | ⚠️ Maybe |
| **Cross-Platform** | ❌ No | ✅ Yes | ✅ Yes | ✅ Yes |
| **Use Case** | Voice | Voice | Voice | Music/Voice |

---

## Recommendations

### Phase 1: SDL2 Port (Immediate - PEBL 2.2/2.3)

**Goal:** Restore voice key functionality using standard SDL2

**Approach:** Minimal changes - port to SDL2 standard audio capture API

**Steps:**
1. Replace custom SDL_audioin functions with `SDL_OpenAudioDevice(..., SDL_TRUE, ...)`
2. Store `SDL_AudioDeviceID` in `PlatformAudioIn`
3. Update callback to work with device-specific API
4. Test on Linux, Windows, macOS
5. Re-enable `USE_AUDIOIN=1` in Makefile

**Code Changes:**

```cpp
// PlatformAudioIn.h - add member
private:
    SDL_AudioDeviceID mAudioDevice;

// PlatformAudioIn.cpp - Initialize()
bool PlatformAudioIn::Initialize(int type)
{
    SDL_AudioSpec want, have;

    SDL_zero(want);
    want.freq = mSampleRate;
    want.format = mAudioFormat;
    want.channels = 1;
    want.samples = mSamples;
    want.callback = (type == 1) ? AudioInCallbackFill : AudioInCallbackLoop;
    want.userdata = &have;

    // Get first recording device
    const char* deviceName = SDL_GetAudioDeviceName(0, SDL_TRUE);
    if(!deviceName) {
        PError::SignalWarning("No audio recording device found");
        return false;
    }

    // Open capture device (SDL_TRUE = recording)
    mAudioDevice = SDL_OpenAudioDevice(deviceName, SDL_TRUE, &want, &have, 0);
    if(mAudioDevice == 0) {
        PError::SignalWarning("Cannot open audio input device: " + Variant(SDL_GetError()));
        return false;
    }

    return true;
}

// Update other functions
bool PlatformAudioIn::RecordToBuffer()
{
    SDL_PauseAudioDevice(mAudioDevice, 0);  // Unpause
    return true;
}

bool PlatformAudioIn::Stop()
{
    SDL_PauseAudioDevice(mAudioDevice, 1);  // Pause
    return true;
}

PlatformAudioIn::~PlatformAudioIn()
{
    if(mAudioDevice > 0)
    {
        SDL_CloseAudioDevice(mAudioDevice);
    }
    // ... rest of cleanup
}
```

**Testing:**
```bash
# Enable in Makefile
sed -i 's/#USE_AUDIOIN=1/USE_AUDIOIN=1/' Makefile

# Rebuild
make clean && make main

# Test
bin/pebl2 demo/testaudioin.pbl
```

**Effort:** 1-2 days
**Risk:** Low
**Benefit:** Restore working voice key on all platforms

---

### Phase 2: WebRTC VAD Integration (Optional - PEBL 3.0)

**Goal:** Improve voice key accuracy and noise resistance

**Approach:** Integrate WebRTC VAD as alternative/additional algorithm

**Steps:**
1. Add WebRTC VAD source to `src/utility/webrtc_vad/`
2. Create `VoiceKeyVAD()` function using WebRTC
3. Add downsample/upsample for 44.1kHz → 16kHz conversion
4. Add `gVoiceKeyAlgorithm` parameter (energy vs vad)
5. Test and compare accuracy

**Integration:**

```cpp
// src/utility/webrtc_vad/ - add WebRTC VAD files
// webrtc_vad.h, webrtc_vad.c, vad_core.h, vad_core.c, etc.

// PlatformAudioIn.h
Variant VoiceKeyVAD(double threshold, unsigned int sustain);

// PlatformAudioIn.cpp
Variant PlatformAudioIn::VoiceKeyVAD(double threshold, unsigned int sustain)
{
    // Downsample 44.1kHz to 16kHz
    std::vector<int16_t> resampled = Resample44To16(mWave->audio,
                                                     mWave->audiolen);

    // Initialize WebRTC VAD
    VadInst *vad;
    WebRtcVad_Create(&vad);
    WebRtcVad_Init(vad);
    WebRtcVad_set_mode(vad, 2);  // Aggressive mode

    // Process in 10ms frames (160 samples at 16kHz)
    int frame_length = 160;
    int onset_frames = 0;
    int sustain_frames = sustain / 10;
    bool tripped = false;
    int triptime = 0;

    for(size_t i = 0; i < resampled.size(); i += frame_length)
    {
        if(i + frame_length > resampled.size()) break;

        int is_speech = WebRtcVad_Process(vad, 16000,
                                          &resampled[i],
                                          frame_length);

        if(is_speech) {
            onset_frames++;
            if(onset_frames >= sustain_frames && !tripped) {
                triptime = (i * 10) / 16;  // Convert to ms in original rate
                tripped = true;
            }
        } else {
            if(tripped && onset_frames < sustain_frames / 2) {
                // Voice ended
                break;
            }
            onset_frames = Max(0, onset_frames - 1);
        }
    }

    WebRtcVad_Free(vad);

    // Return [triptime, offtime, tripped]
    PList *result = new PList();
    result->PushBack(Variant(triptime));
    result->PushBack(Variant(0));  // Calculate offtime
    result->PushBack(Variant(tripped));

    return Variant(new PComplexData(counted_ptr<PEBLObjectBase>(result)));
}
```

**PEBL API:**
```pebl
## Current (energy-based)
rt <- GetVocalResponseTime(buffer, threshold, sustain)

## New (VAD-based) - add parameter
rt <- GetVocalResponseTimeVAD(buffer, threshold, sustain)

## Or make it configurable
SetVoiceKeyAlgorithm("energy")  ## Default
SetVoiceKeyAlgorithm("vad")     ## WebRTC VAD
```

**Effort:** 2-3 days
**Risk:** Medium (new dependency, resampling complexity)
**Benefit:** Much more robust voice key, better for noisy environments

---

## Testing Plan

### Test Cases

1. **Basic Functionality**
   - Record 5 seconds of speech
   - Verify buffer fills correctly
   - Verify WAV export works

2. **Voice Key Accuracy**
   - Single word utterances ("cat", "dog", "house")
   - Measure onset detection accuracy (compare to manual annotation)
   - Test with varying threshold (0.1, 0.25, 0.5)
   - Test with varying sustain (50ms, 100ms, 200ms)

3. **Noise Resistance**
   - Quiet environment
   - Background music
   - Background conversation
   - White noise

4. **Cross-Platform**
   - Linux (Ubuntu, Fedora)
   - Windows 10/11
   - macOS
   - Emscripten (if possible)

5. **Performance**
   - CPU usage during recording
   - Memory usage for buffers
   - Latency from onset to detection

### Success Criteria

**Phase 1 (SDL2 Port):**
- ✅ Compiles on Linux, Windows, macOS
- ✅ Records audio from default microphone
- ✅ Voice key triggers on speech onset
- ✅ Saves WAV files correctly
- ✅ Accuracy ≥80% in quiet environment

**Phase 2 (WebRTC VAD):**
- ✅ All Phase 1 criteria
- ✅ Accuracy ≥90% in quiet environment
- ✅ Accuracy ≥70% with moderate background noise
- ✅ Latency <100ms from onset to detection

---

## Implementation Checklist

### Phase 1: SDL2 Port

- [ ] Update `PlatformAudioIn.h` to add `SDL_AudioDeviceID mAudioDevice`
- [ ] Replace `SDL_InitAudioIn()` → standard SDL_Init (already done)
- [ ] Replace `SDL_OpenAudioIn()` → `SDL_OpenAudioDevice(..., SDL_TRUE, ...)`
- [ ] Replace `SDL_PauseAudioIn()` → `SDL_PauseAudioDevice(mAudioDevice, ...)`
- [ ] Replace `SDL_CloseAudioIn()` → `SDL_CloseAudioDevice(mAudioDevice)`
- [ ] Update Makefile: `USE_AUDIOIN=1`
- [ ] Test on Linux
- [ ] Test on Windows
- [ ] Test on macOS
- [ ] Update documentation

### Phase 2: WebRTC VAD (Optional)

- [ ] Add WebRTC VAD source files to `src/utility/webrtc_vad/`
- [ ] Add resampling function (44.1kHz → 16kHz)
- [ ] Implement `VoiceKeyVAD()` function
- [ ] Add PEBL API: `GetVocalResponseTimeVAD()`
- [ ] Compare accuracy: energy vs VAD
- [ ] Update documentation
- [ ] Add examples to battery tests

---

## References

### PEBL Code
- `src/platforms/sdl/PlatformAudioIn.h` - Voice key header
- `src/platforms/sdl/PlatformAudioIn.cpp` - Voice key implementation
- `demo/testaudioin.pbl` - Test demo

### SDL2 Audio Capture
- **Tutorial:** https://lazyfoo.net/tutorials/SDL/34_audio_recording/
- **SDL Wiki:** https://wiki.libsdl.org/SDL2/SDL_OpenAudioDevice
- **Example:** https://gist.github.com/andraantariksa/f5e6d848364b11a425625ec7fbbfc187

### WebRTC VAD
- **Source:** https://chromium.googlesource.com/external/webrtc/+/master/common_audio/vad/
- **Documentation:** https://webrtc.googlesource.com/src/+/refs/heads/main/common_audio/vad/
- **Standalone port:** https://github.com/dpirch/libfvad (easier to integrate)

### VAD Resources
- **Comparison:** https://dsp.stackexchange.com/questions/2386/libraries-for-voice-activity-detection
- **WebRTC VAD tutorial:** https://nickmccullum.com/voice-activity-detection/

---

## Appendix A: Current PEBL Voice Key API

```pebl
## Create audio buffer (5000ms)
buffer <- MakeAudioInputBuffer(5000)

## Get voice response time
## threshold: 0.0-1.0 (typically 0.1-0.5)
## sustain: ms to sustain above threshold (typically 50-300)
rt <- GetVocalResponseTime(buffer, threshold, sustain)

## rt is a list: [onset_ms, offset_ms, triggered]
onset <- First(rt)
offset <- Second(rt)
triggered <- Third(rt)

## Save recorded audio to WAV file
SaveAudioToWaveFile("output.wav", buffer)
```

### Example from `demo/testaudioin.pbl`

```pebl
buffer <- MakeAudioInputBuffer(5000)

loop(word, ["cat", "dog", "house"])
{
    ## Show word
    stimlab.text <- word
    Draw()

    ## Get voice onset time
    rt <- GetVocalResponseTime(buffer, 0.25, 200)

    ## Save recording
    SaveAudioToWaveFile("output/" + word + ".wav", buffer)

    ## Log results
    Print(word + "," + First(rt) + "," + Second(rt) + "," + Third(rt))
}
```

---

## Appendix B: Energy vs VAD Comparison

### Energy-Based Detection (Current)

**Pros:**
- Simple algorithm
- No dependencies
- Fast computation
- Low memory

**Cons:**
- Sensitive to background noise
- No frequency filtering
- False triggers on loud non-speech sounds
- Requires careful threshold tuning

**Best for:**
- Quiet controlled environments
- Laboratory settings
- When simplicity is important

### WebRTC VAD

**Pros:**
- ML-based (Gaussian Mixture Model)
- Robust to background noise
- Frequency-aware (focuses on speech frequencies)
- Aggressiveness modes for different scenarios
- Battle-tested in production (Chrome, WebRTC)

**Cons:**
- Requires resampling to 16kHz
- Slightly more CPU intensive
- External dependency (~20KB)
- More complex integration

**Best for:**
- Real-world environments
- Background noise present
- Remote/online testing
- When accuracy is critical

---

## Appendix C: SDL2 Audio Capture Sample Code

```cpp
// Complete working example for SDL2 audio recording

#include <SDL2/SDL.h>
#include <vector>

std::vector<Uint8> recordedSamples;

void AudioCaptureCallback(void* userdata, Uint8* stream, int len)
{
    // Append captured audio to buffer
    recordedSamples.insert(recordedSamples.end(), stream, stream + len);
}

int main()
{
    // Initialize SDL audio subsystem
    SDL_Init(SDL_INIT_AUDIO);

    // Configure desired audio format
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_S16SYS;  // 16-bit signed
    want.channels = 1;           // Mono
    want.samples = 4096;
    want.callback = AudioCaptureCallback;

    // Get first recording device name
    const char* deviceName = SDL_GetAudioDeviceName(0, SDL_TRUE);
    if(!deviceName) {
        printf("No recording device found\n");
        return 1;
    }
    printf("Using device: %s\n", deviceName);

    // Open capture device (SDL_TRUE = recording)
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(deviceName, SDL_TRUE,
                                                 &want, &have, 0);
    if(dev == 0) {
        printf("Failed to open audio device: %s\n", SDL_GetError());
        return 1;
    }

    // Start recording
    SDL_PauseAudioDevice(dev, 0);
    printf("Recording for 5 seconds...\n");
    SDL_Delay(5000);

    // Stop recording
    SDL_PauseAudioDevice(dev, 1);
    printf("Recorded %zu bytes\n", recordedSamples.size());

    // Cleanup
    SDL_CloseAudioDevice(dev);
    SDL_Quit();

    return 0;
}
```

Compile: `g++ -o record record.cpp $(sdl2-config --cflags --libs)`
