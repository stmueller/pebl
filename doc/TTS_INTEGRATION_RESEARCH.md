# Text-to-Speech Integration Research for PEBL

**Date:** 2026-01-12
**Objective:** Evaluate TTS solutions for reading experiment instructions to participants
**Status:** ⏸️ **PROJECT ON HOLD** (Jan 12, 2026)

---

## ⚠️ PROJECT STATUS UPDATE

**Decision:** TTS integration postponed indefinitely.

**Reason:** eSpeak NG quality assessment revealed unacceptable robotic voice quality. While technically feasible, the auditory experience is not suitable for research participants.

**Quality findings:**
- **eSpeak NG (formant synthesis):** Robotic, monotone, sounds artificial
- **Piper TTS (neural):** High quality but 15-35MB per voice (too large for default distribution)
- **Web Speech API:** High quality but web-only, browser-inconsistent

**Recommendation:** Researchers needing spoken instructions should:
1. **Pre-record audio files** in natural human voice (researcher or voice actor)
2. Use standard PEBL audio playback: `LoadSound()`, `PlayForeground()`
3. Benefits: Better quality, full prosody control, no distribution size impact

**Future possibility:** Piper TTS as optional developer tool (not in core PEBL)
- Separate installation guide
- User downloads voice models independently
- Build flag: `USE_NEURAL_TTS=1` (disabled by default)
- Positioned as advanced/optional feature for specialized needs

---

## Executive Summary

**Recommended Approach:** Multi-tiered implementation with Web Speech API as primary solution for WebAssembly builds, and eSpeak NG for native builds.

**Quick Recommendation:**
- **Web builds:** Use Web Speech API (JavaScript integration, zero distribution cost)
- **Native builds:** Use eSpeak NG (3-4MB total, GPL-3.0, excellent WebAssembly support)
- **Future enhancement:** Consider Piper TTS for high-quality neural voices (requires ONNX runtime)

## Requirements Analysis

### PEBL-Specific Constraints

1. **Cross-platform deployment:** Linux (native), WebAssembly (Emscripten), potential Windows/macOS
2. **Dual build system:** Native C++ (SDL2) and WebAssembly (Emscripten)
3. **Current audio architecture:** SDL_Mixer for playback (lines 89-804 in PlatformAudioOut.cpp)
4. **Distribution size sensitivity:** WebAssembly builds must remain compact
5. **License compatibility:** GPL-2.0 (must be GPL-compatible)
6. **Use case:** Reading text instructions to participants (not real-time synthesis during trials)
7. **SDL3 migration:** Potential future migration from SDL2 to SDL3

### Functional Requirements

- **Synthesize text to audio buffer** for playback via existing SDL_Mixer system
- **Multi-language support** for international experiments
- **Offline operation** (no cloud dependencies)
- **Simple API** accessible from PEBL language (C++ function bindings)
- **Quality threshold:** Intelligible speech (neural quality preferred but not required)

---

## Available TTS Libraries

### 1. eSpeak NG (RECOMMENDED for Native)

**Repository:** https://github.com/espeak-ng/espeak-ng

**Language:** C
**License:** GPL-3.0-or-later ✅ (compatible with PEBL)
**Type:** Formant synthesis (rule-based)

#### Strengths
- **Compact size:** ~3-4MB for engine + all language data
- **WebAssembly support:** Official Emscripten port in `/emscripten` directory
- **Extensive language support:** 100+ languages and accents
- **Active development:** Regular updates and maintenance
- **SDL2 integration examples:** Community examples exist
- **Fast synthesis:** Generates audio in real-time or faster

#### Size Analysis
- Core engine: ~400KB
- Language data: ~3MB for all languages (~100KB per language if selective)
- Total distribution impact: 3-4MB (negligible for native, acceptable for WASM)

#### Integration Approach
```cpp
// Pseudocode integration
#include <espeak-ng/espeak_ng.h>

PlatformAudioOut* CreateTTSAudio(const char* text, const char* voice) {
    espeak_ng_InitializePath(NULL);
    espeak_ng_Initialize(NULL);

    // Set voice (e.g., "en", "es", "fr")
    espeak_ng_SetVoiceByName(voice);

    // Synthesize to buffer
    std::vector<short> audio_buffer;
    espeak_ng_Synthesize(text, /*callback captures to buffer*/);

    // Convert to SDL_Mixer format via PlatformAudioOut::LoadSoundFromData()
    SDL_AudioSpec spec;
    spec.freq = 22050;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;

    PlatformAudioOut* audio = new PlatformAudioOut();
    audio->LoadSoundFromData((Uint8*)audio_buffer.data(),
                             audio_buffer.size() * sizeof(short),
                             &spec);
    return audio;
}
```

#### SDL_Mixer Compatibility
- eSpeak NG outputs 16-bit PCM audio (typically 22050 Hz mono)
- PEBL's `LoadSoundFromData()` already handles raw PCM buffers (line 451 in PlatformAudioOut.cpp)
- SDL_Mixer will convert format if needed (PEBL already does this for recorded audio)

#### WebAssembly Compatibility
- **Official Emscripten port:** `/emscripten` directory in repo with build scripts
- **Optimization:** Uses `-O3` flag, achieves significant size reduction
- **File packaging:** Language data can be embedded via `--preload-file` (similar to PEBL's media files)
- **Tested:** Browser-based TTS synthesis working in production applications

#### SDL3 Migration Impact
- **Low risk:** eSpeak NG only generates audio buffers, doesn't depend on SDL version
- PEBL's audio loading layer would handle SDL2→SDL3 changes
- No direct SDL dependency in eSpeak NG itself

---

### 2. Web Speech API (RECOMMENDED for WebAssembly)

**Specification:** https://wicg.github.io/speech-api/

**Language:** JavaScript (browser native)
**License:** Browser implementation (no distribution)
**Type:** Platform-dependent (often neural-based)

#### Strengths
- **Zero distribution size:** Built into modern browsers
- **High quality:** Often uses neural voices (Google, Apple, Microsoft engines)
- **Multi-language:** Extensive language support via browser
- **No compilation:** JavaScript API, no C++ integration needed
- **Perfect for PEBL Web:** Already running in browser context

#### Weaknesses
- **Web-only:** Not available in native builds
- **Voice consistency:** Different voices across browsers/platforms
- **Limited control:** Cannot customize voice characteristics extensively
- **Privacy considerations:** May send data to cloud (browser-dependent)

#### Integration Approach

Since PEBL's WebAssembly build already integrates with JavaScript (via Emscripten), Web Speech API can be called from PEBL scripts or C++ via EM_ASM:

```cpp
// C++ integration via Emscripten
#ifdef PEBL_EMSCRIPTEN
#include <emscripten.h>

void SpeakText(const char* text, const char* lang) {
    EM_ASM({
        const utterance = new SpeechSynthesisUtterance(UTF8ToString($0));
        utterance.lang = UTF8ToString($1);

        // Store audio for PEBL playback (optional: can play directly)
        // For PEBL integration, could capture to WAV via Web Audio API
        window.speechSynthesis.speak(utterance);
    }, text, lang);
}
#endif
```

**Alternative:** Expose to PEBL language as JavaScript function callable from .pbl scripts:

```javascript
// In PEBL's emscripten shell
function pebl_speak(text, lang) {
    const utterance = new SpeechSynthesisUtterance(text);
    utterance.lang = lang || 'en-US';
    speechSynthesis.speak(utterance);
}
```

#### Limitations for PEBL
- **Audio capture complexity:** Web Speech API plays directly, capturing to buffer for SDL_Mixer requires Web Audio API recording
- **Recommendation:** For web builds, use direct speech playback (simpler) rather than routing through SDL_Mixer
- **Fallback:** If audio buffer needed, use eSpeak NG WebAssembly build instead

---

### 3. Flite (Festival Lite)

**Repository:** https://github.com/festvox/flite
**Language:** C
**License:** BSD-like (permissive) ✅
**Type:** Formant synthesis

#### Size Comparison with Festival
| Component | Flite | Festival |
|-----------|-------|----------|
| Core code | 60KB | 2.6MB |
| Lexicon | 600KB | 5MB |
| Voice data | 1.8MB | 2.1MB |
| Runtime memory | <1MB | 16-20MB |
| **Total** | **~2.5MB** | **~10MB** |

#### Strengths
- **Very compact:** Smallest full-featured TTS engine
- **Fast:** 70× real-time synthesis speed
- **Permissive license:** More flexible than GPL
- **Embedded-friendly:** Designed for resource-constrained devices
- **C library:** Easy C++ integration

#### Weaknesses
- **Limited language support:** Primarily English, few other languages
- **Voice quality:** Lower than neural approaches, similar to eSpeak
- **Less active development:** Maintained but not rapidly evolving
- **Emscripten support:** No official WebAssembly port (would need custom build)

#### Evaluation
**Verdict:** Good for native-only builds if only English needed, but eSpeak NG offers better language support for similar size. Would require manual Emscripten porting.

---

### 4. Piper TTS (Neural, Future Consideration)

**Repository:** https://github.com/rhasspy/piper
**Language:** C++ (uses ONNX Runtime)
**License:** MIT ✅
**Type:** Neural TTS (VITS model)

#### Strengths
- **High quality:** Neural synthesis, natural-sounding voices
- **Fast:** Optimized for Raspberry Pi, runs well on modest hardware
- **WebAssembly port exists:** `piper-tts-plus` with WASM support
- **Offline:** No cloud dependencies
- **Multi-language:** Extensive voice model library

#### Size Analysis
- **WASM binary:** <400KB (compressed)
- **JavaScript:** <40KB
- **Voice models:** 10-30MB per voice (separate downloads)
- **ONNX Runtime (WASM):** ~2-3MB

**Total for one voice:** ~15-35MB (significantly larger than eSpeak NG)

#### Weaknesses
- **Model size:** Each voice is 10-30MB (much larger than eSpeak's ~100KB per language)
- **Dependency:** Requires ONNX Runtime (adds complexity)
- **WebAssembly maturity:** Newer, less battle-tested than eSpeak NG
- **Integration complexity:** More complex build setup

#### Evaluation
**Verdict:** Excellent quality but size cost is high. Recommended as **future enhancement** for experiments requiring highest-quality voices. Could be optional download (user chooses quality vs. size).

---

### 5. MeloTTS.cpp (Neural, Research Interest)

**Repository:** https://github.com/apinge/MeloTTS.cpp
**Language:** C++ (uses OpenVINO)
**License:** Not clearly specified in search results
**Type:** Neural TTS

#### Notes
- Lightweight C++ neural TTS implementation
- Supports English, Chinese, and mixed-language synthesis
- Uses OpenVINO for inference (Intel optimization)
- **Insufficient information** about licensing, size, and WebAssembly support
- Appears to be newer/experimental project

**Verdict:** Requires further investigation before recommendation.

---

## Rejected Approaches

### Python-Based Solutions (Coqui TTS, Bark, Tortoise TTS, etc.)

**Reason for rejection:**
- Require Python runtime (not viable for PEBL's C++/WebAssembly architecture)
- Massive dependencies (PyTorch, TensorFlow)
- Distribution size would be 500MB-2GB
- No practical C++ integration path without embedding Python interpreter

### Cloud-Based APIs (Google TTS, Amazon Polly, etc.)

**Reason for rejection:**
- Require internet connection (experiments often offline)
- Privacy concerns with participant data
- API costs and rate limiting
- Latency issues
- Against PEBL's self-contained design philosophy

---

## Integration with PEBL Architecture

### Current Audio System

PEBL uses SDL_Mixer (when `PEBL_MIXER` defined) for audio playback:

**Initialization:** Lines 89-97 (PlatformAudioOut.cpp)
```cpp
Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
Mix_Init(MIX_INIT_OGG|MIX_INIT_MP3|MIX_INIT_FLAC|MIX_INIT_MOD);
Mix_AllocateChannels(16);
```

**Buffer Loading:** Lines 451-495 (LoadSoundFromData)
- Accepts raw PCM buffer + SDL_AudioSpec
- Creates Mix_Chunk for playback
- Already used for audio recording playback

### Proposed TTS Integration Points

#### Option A: Pre-synthesis (Recommended)

Generate TTS audio during experiment setup, store as PlatformAudioOut objects:

```cpp
// In PEBLEnvironment.cpp - new function
PlatformAudioOut* SynthesizeSpeech(const char* text, const char* voice) {
#ifdef PEBL_EMSCRIPTEN
    // Use Web Speech API or eSpeak NG WASM
    return SynthesizeSpeech_Web(text, voice);
#else
    // Use eSpeak NG native
    return SynthesizeSpeech_Native(text, voice);
#endif
}
```

**PEBL language binding** (in Functions.h):
```cpp
{"SYNTHESIZESPEECH", &PEBLEnvironment::SynthesizeSpeech, 1, 2}
```

**Usage in .pbl scripts:**
```pebl
## Generate instruction audio
gInstructionAudio <- SynthesizeSpeech(gStrings.INSTRUCTIONS, "en")

## Play when needed
PlayForeground(gInstructionAudio)
```

#### Option B: Real-time synthesis

Less recommended due to potential latency, but simpler for web:

```javascript
// For web builds only
function speakInstructions(text) {
    const utterance = new SpeechSynthesisUtterance(text);
    speechSynthesis.speak(utterance);
}
```

---

## SDL3 Migration Considerations

### Current Status
- PEBL uses SDL2 (`USE_SDL=2` in Makefile)
- SDL_Mixer 2.x for audio

### SDL3 Changes
- SDL3 has redesigned audio API (no more callbacks, uses audio streams)
- SDL_Mixer 3.x is in development (not stable as of 2024)

### TTS Impact Analysis

**eSpeak NG:** ✅ **No impact**
- Only generates PCM buffers
- PEBL's audio loading layer abstracts SDL version
- Buffer format conversion handled by PEBL code

**Web Speech API:** ✅ **No impact**
- Browser API, independent of SDL

**Piper TTS:** ✅ **No impact**
- Same as eSpeak NG, only produces buffers

**Integration layer:** ⚠️ **Minor changes needed**
- `LoadSoundFromData()` would need SDL3 adaptation (affects all audio, not just TTS)
- TTS synthesis code itself unchanged

---

## License Compatibility Matrix

| Library | License | Compatible with PEBL GPL-2.0? | Distribution |
|---------|---------|-------------------------------|--------------|
| **eSpeak NG** | GPL-3.0 | ✅ Yes (GPL-compatible) | Source/binary |
| **Web Speech API** | Browser built-in | ✅ Yes (no distribution) | N/A |
| **Flite** | BSD-like | ✅ Yes (permissive) | Source/binary |
| **Piper TTS** | MIT | ✅ Yes (permissive) | Source/binary |
| ONNX Runtime | MIT | ✅ Yes (permissive) | Required for Piper |
| Festival | X11-like | ✅ Yes (permissive) | Source/binary |

**All considered options are license-compatible.**

---

## Distribution Size Impact

### WebAssembly Build

**Current PEBL WebAssembly size:** ~10-15MB (pebl2.wasm + pebl2.data)

| Solution | Additional Size | % Increase | Acceptable? |
|----------|----------------|------------|-------------|
| **Web Speech API** | 0 bytes | 0% | ✅ Excellent |
| **eSpeak NG (WASM)** | ~1-2MB (with selective languages) | ~10% | ✅ Good |
| **eSpeak NG (all languages)** | ~4MB | ~30% | ⚠️ Acceptable |
| **Piper TTS** | ~15-35MB (per voice) | 100-200%+ | ❌ Too large for default |

### Native Build

**Current PEBL native size:** ~5-10MB

| Solution | Additional Size | % Increase | Acceptable? |
|----------|----------------|------------|-------------|
| **eSpeak NG** | ~4MB | 40-80% | ✅ Good |
| **Flite** | ~2.5MB | 25-50% | ✅ Excellent |
| **Piper TTS** | ~20-40MB | 200-400% | ⚠️ Optional feature |

---

## Recommendations

### Phase 1: Initial Implementation (Immediate)

**Web Builds:**
1. **Implement Web Speech API integration** via EM_ASM
   - Zero distribution cost
   - High quality voices
   - Simple implementation
   - Direct speech playback (no buffer capture)

**Native Builds:**
2. **Integrate eSpeak NG** as C library
   - Add to Makefile as optional dependency (`USE_TTS=1`)
   - Link against libespeakng
   - Implement `SynthesizeSpeech()` function
   - Package language data in `media/espeak-ng-data/`

**PEBL Language Function:**
```pebl
## New function in PEBL
define SynthesizeSpeech(text, voice)
  ## Returns AudioOut object
  ## voice: "en", "es", "fr", "de", etc.

## Example usage
instructions <- SynthesizeSpeech(gStrings.INSTRUCTIONS, gLanguage)
PlayForeground(instructions)
```

### Phase 2: Enhanced Quality (Future)

**Optional Neural TTS:**
3. **Add Piper TTS as optional dependency**
   - Build flag: `USE_NEURAL_TTS=1`
   - Voice models as separate downloads (not in main distribution)
   - Let users choose quality tier:
     - Default: eSpeak NG (small, fast)
     - Premium: Piper (large, high-quality)

**PEBL Language Enhancement:**
```pebl
## Specify TTS engine
instructions <- SynthesizeSpeech(text, voice, "neural")  ## vs "standard"
```

### Phase 3: Advanced Features (Optional)

4. **Voice caching system**
   - Cache synthesized speech to `data/tts-cache/`
   - Avoid re-synthesis on repeated runs
   - MD5 hash of (text + voice + engine) as filename

5. **SSML support** (for advanced prosody control)
   - Speed, pitch, emphasis adjustments
   - Helpful for instruction clarity

---

## Implementation Roadmap

### Milestone 1: Web Speech API (1-2 days)

**Files to modify:**
- `src/libs/PEBLEnvironment.cpp` - Add `SynthesizeSpeech_Web()` function
- `src/libs/Functions.h` - Register `SYNTHESIZESPEECH` function
- `emscripten/shell_PEBL_debug.html` - Add JavaScript helper functions

**Testing:**
- Create `demo/tts-demo.pbl` demonstrating usage
- Test in Firefox, Chrome, Safari

### Milestone 2: eSpeak NG Native (3-5 days)

**Files to modify:**
- `Makefile` - Add `USE_TTS` flag, link against `-lespeakng`
- `src/libs/PEBLEnvironment.cpp` - Add `SynthesizeSpeech_Native()` using eSpeak NG API
- Package `espeak-ng-data/` in `media/` directory

**Dependencies:**
```bash
# Ubuntu/Debian
sudo apt-get install libespeak-ng-dev

# Build integration
make USE_TTS=1 main
```

**Testing:**
- Multi-language synthesis tests
- Audio quality verification
- Memory leak testing (valgrind)

### Milestone 3: eSpeak NG WebAssembly (5-7 days)

**Tasks:**
1. Clone eSpeak NG repository
2. Build with Emscripten using `/emscripten` directory instructions
3. Link eSpeak NG WASM library with PEBL
4. Package espeak-ng-data with `--preload-file`

**Challenges:**
- Emscripten build configuration
- Data file embedding
- Function export/import between modules

### Milestone 4: Documentation & Examples (2-3 days)

**Create:**
- `doc/TTS_USAGE.md` - User guide for TTS functions
- Update `doc/CLAUDE.md` - Add TTS integration notes
- `battery/tts-instructions-demo/` - Example experiment using TTS

**Documentation includes:**
- Function reference
- Language codes
- Performance considerations
- Caching strategies

---

## Alternatives Considered and Rejected

### 1. SAPI (Windows Speech API)
- **Pros:** Native Windows TTS, free
- **Cons:** Windows-only, no Linux/WebAssembly support
- **Verdict:** Not cross-platform

### 2. Festival (full version)
- **Pros:** Mature, well-documented
- **Cons:** 10MB distribution, slower than Flite/eSpeak
- **Verdict:** Flite provides same functionality in smaller package

### 3. Mimic (Mycroft's TTS)
- **Pros:** Open source, decent quality
- **Cons:** Development stalled, limited language support
- **Verdict:** eSpeak NG more actively maintained

### 4. Pico TTS
- **Pros:** Very small (~500KB engine)
- **Cons:** Limited language support, unclear licensing
- **Verdict:** eSpeak NG offers better language coverage

---

## Performance Considerations

### Synthesis Speed

| Engine | Speed | Latency | Suitable for real-time? |
|--------|-------|---------|-------------------------|
| **eSpeak NG** | ~100× real-time | <50ms | ✅ Yes |
| **Flite** | ~70× real-time | <100ms | ✅ Yes |
| **Piper TTS** | ~10-20× real-time | ~200ms | ✅ Yes (acceptable) |
| **Web Speech API** | Varies | 100-500ms | ⚠️ Browser-dependent |

**For PEBL:** All options fast enough for pre-synthesis during setup. No real-time constraints.

### Memory Usage

- **eSpeak NG:** <10MB RAM during synthesis
- **Flite:** <5MB RAM
- **Piper TTS:** ~50-100MB (neural model loaded)
- **Web Speech API:** Handled by browser (no PEBL impact)

**For PEBL:** All acceptable. Piper's higher usage is only during synthesis, released after.

---

## Testing Strategy

### Unit Tests
1. **Synthesis correctness:** Verify audio buffer generated for input text
2. **Multi-language:** Test all supported language codes
3. **SDL_Mixer integration:** Confirm playback via existing audio system
4. **Memory leaks:** Valgrind verification
5. **WebAssembly build:** Browser playback tests

### Integration Tests
1. Create battery task using TTS instructions
2. Test with gStrings translation system
3. Verify caching behavior
4. Test error handling (missing voice, invalid text)

### Performance Tests
1. Synthesis time for various text lengths
2. Memory usage monitoring
3. WebAssembly bundle size verification

---

## Conclusion

**Recommended Implementation:**

**Immediate (Phase 1):**
- ✅ **Web builds:** Web Speech API (zero cost, high quality)
- ✅ **Native builds:** eSpeak NG (small, multi-language, GPL-compatible)

**Future (Phase 2):**
- 🔄 **Optional:** Piper TTS for premium neural voices (user choice)

**Benefits:**
- Cross-platform coverage (Linux, Web, future Windows/macOS)
- Small distribution size impact (0-4MB)
- Multi-language support (100+ languages)
- GPL-compatible licensing
- SDL2/SDL3 compatible (no API dependencies)
- Easy PEBL language integration

**Next Steps:**
1. Prototype Web Speech API integration (demo/tts-demo.pbl)
2. Evaluate eSpeak NG build integration on Linux
3. Design PEBL language API (SynthesizeSpeech function signature)
4. Create implementation plan document

---

## References

- **eSpeak NG:** https://github.com/espeak-ng/espeak-ng
- **Web Speech API:** https://developer.mozilla.org/en-US/docs/Web/API/Web_Speech_API
- **Flite:** http://www.festvox.org/flite/
- **Piper TTS:** https://github.com/rhasspy/piper
- **awesome-free-tts:** https://github.com/arzen07/awesome-free-tts
- **SDL_Mixer:** https://www.libsdl.org/projects/SDL_mixer/
- **PEBL Audio System:** `src/platforms/sdl/PlatformAudioOut.cpp`
