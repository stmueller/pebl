# Voice Key Calibration System

## Overview

This system provides adaptive threshold calibration for PEBL's voice key detection using ROC-based optimization. It supports three calibration modes depending on available data.

## Files

- **`voicekey-calibration.pbl`** - Core calibration functions
- **`test-voicekey-calibration.pbl`** - Demo/test script
- **`audio-calibration.pbl`** - Original data collection tool with CSV export

## Three Calibration Modes

### Mode 1: Default (No Calibration)
**When to use:** No calibration data available, need a reasonable starting point

**Usage:**
```pebl
result <- CalibrateVoiceKeyThreshold(0, 0, 0.15)
threshold <- First(result)
```

**Output:** Returns the default threshold (0.15)

---

### Mode 2: Noise-Only (Percentile-based)
**When to use:** Have noise/silence recording but no speech samples

**Usage:**
```pebl
## Collect noise data
silenceStats <- RecordSilence(gWin)
noisePower <- Nth(silenceStats, 2)  ## Extract power profile

## Calculate 95th percentile threshold
result <- CalibrateVoiceKeyThreshold(noisePower, 0, 0.15, 95)
threshold <- First(result)
```

**Algorithm:**
- Sorts all noise power values
- Returns the 95th percentile as threshold
- Ensures 95% of noise samples are below threshold

**Advantages:**
- Simple, robust
- Adapts to environment noise level
- Conservative (high noise rejection)

**Limitations:**
- May be too conservative (miss low-amplitude speech)
- No information about signal characteristics

---

### Mode 3: Adaptive ROC-based (Optimal)
**When to use:** Have both noise AND speech calibration recordings

**Usage:**
```pebl
## Collect noise and speech data
silenceStats <- RecordSilence(gWin)
speechResults <- []
loop(word, ["WISCONSIN", "SUSPECT", "CHRISTMAS", "HARMONY", "APARTMENT"])
{
    result <- RecordWord(word, gWin)
    speechResults <- Append(speechResults, result)
}

## Extract power values
powerData <- ExtractPowerFromRecordings(silenceStats, speechResults)
noisePower <- First(powerData)
speechPower <- Nth(powerData, 2)

## Calculate optimal threshold
result <- CalibrateVoiceKeyThreshold(noisePower, speechPower, 0.15, 95, 75, 100)
threshold <- First(result)
diagnostics <- Third(result)
```

**Algorithm:**
1. Combines all noise and speech power values to find overall range
2. Divides range into equal-width bins (default: 100 bins)
3. For each bin threshold, calculates:
   - **Noise rejection rate**: % of noise samples below threshold
   - **Signal capture rate**: % of speech samples above threshold
4. Finds threshold that:
   - Meets noise rejection criterion (≥95%)
   - Maximizes signal capture (target: 75%)

**ROC Analysis:**
- Creates cumulative distribution functions for both noise and speech
- Searches for optimal operating point on ROC curve
- Balances false alarm rate vs. detection rate

**Advantages:**
- Optimal threshold based on actual signal/noise characteristics
- Quantified performance (noise rejection %, signal capture %)
- Saves ROC curve for analysis

**Output:**
```
Optimal threshold: 0.0234
Noise rejection: 95.5%
Signal capture: 82.3%
```

**Saved Data:** `voicekey_roc_calibration.csv` contains full ROC curve

---

## API Reference

### `CalibrateVoiceKeyThreshold(noiseData, speechData, defaultThreshold, noiseReject, signalCapture, numBins)`

**Parameters:**
- `noiseData` - List of power values from silence (or 0 if not available)
- `speechData` - List of power values from speech (or 0 if not available)
- `defaultThreshold` - Fallback value (default: 0.15)
- `noiseReject` - Target noise rejection % 0-100 (default: 95)
- `signalCapture` - Target signal capture % 0-100 (default: 75)
- `numBins` - ROC resolution (default: 100)

**Returns:** `[threshold, method, diagnostics]`
- `threshold` - Optimal threshold value (float)
- `method` - "default", "noise-only", or "adaptive" (string)
- `diagnostics` - Property list with performance metrics

**Diagnostics Properties (Adaptive mode):**
- `threshold` - Final threshold
- `noiseReject` - Achieved noise rejection %
- `signalCapture` - Achieved signal capture %
- `noiseSamples` - Number of noise samples
- `speechSamples` - Number of speech samples
- `bins` - List of threshold bins
- `noiseCDF` - Cumulative distribution (noise)
- `speechCDF` - Cumulative distribution (speech)

---

### `ExtractPowerFromRecordings(silenceStats, speechResults)`

**Parameters:**
- `silenceStats` - Output from `RecordSilence()` (5-element list)
- `speechResults` - List of outputs from `RecordWord()` (each 6-element list)

**Returns:** `[noisePower, speechPower]`
- `noisePower` - List of power values from silence
- `speechPower` - Flattened list of power values from all speech recordings

---

### `SaveCalibrationResults(diagnostics, filename)`

**Parameters:**
- `diagnostics` - Diagnostics from `CalibrateVoiceKeyThreshold()`
- `filename` - Output CSV file (default: "voicekey_calibration.csv")

**Output CSV format:**
```csv
# Voice Key Calibration Results
method,adaptive
threshold,0.0234
noiseReject,95.5
signalCapture,82.3
noiseSamples,200
speechSamples,742

# ROC Curve Data
threshold,noiseRejectPct,signalCapturePct
0.0001,0.5,99.8
0.0002,1.0,99.5
...
```

---

## Example Workflow

### Complete Calibration and Usage

```pebl
define Start(p)
{
    ## Load calibration functions
    Load("voicekey-calibration.pbl")

    gWin <- MakeWindow("grey40")
    gMonitor <- StartAudioMonitor(2000)

    ## === CALIBRATION PHASE ===

    ## 1. Collect noise baseline
    MessageBox("We'll now calibrate the voice key. First, stay silent for 2 seconds.", gWin)
    silenceStats <- RecordSilence(gWin)

    ## 2. Collect speech samples
    MessageBox("Now say each word when prompted.", gWin)
    speechResults <- []
    loop(word, ["WISCONSIN", "SUSPECT", "CHRISTMAS", "HARMONY", "APARTMENT"])
    {
        result <- RecordWord(word, gWin)
        speechResults <- Append(speechResults, result)
    }

    ## 3. Extract power and calculate optimal threshold
    powerData <- ExtractPowerFromRecordings(silenceStats, speechResults)
    noisePower <- First(powerData)
    speechPower <- Nth(powerData, 2)

    result <- CalibrateVoiceKeyThreshold(noisePower, speechPower, 0.15, 95, 75)
    gThreshold <- First(result)  ## Store as global
    diagnostics <- Third(result)

    ## Save calibration
    SaveCalibrationResults(diagnostics, "calibration_" + gSubNum + ".csv")

    Print("Calibration complete! Using threshold: " + gThreshold)
    Print("Noise rejection: " + Round(GetProperty(diagnostics, "noiseReject"), 1) + "%")
    Print("Signal capture: " + Round(GetProperty(diagnostics, "signalCapture"), 1) + "%")

    ## === EXPERIMENTAL PHASE ===

    MessageBox("Calibration complete. Starting experiment...", gWin)

    ## Use calibrated threshold in experiment
    loop(trial, trials)
    {
        ## Show stimulus
        ShowStimulus(trial)

        ## Wait for vocal response with calibrated threshold
        result <- GetVocalResponseTime(gMonitor, gThreshold, 100)
        rt <- First(result)
        offset <- Nth(result, 2)
        detected <- Third(result)

        ## Record data
        FilePrint(gFileOut, gSubNum + "," + trial + "," + rt + "," + detected)
    }

    StopAudioMonitor(gMonitor)
}
```

---

## Recommended Settings

### Conservative (minimize false alarms)
```pebl
result <- CalibrateVoiceKeyThreshold(noisePower, speechPower, 0.15, 98, 70)
```
- 98% noise rejection
- 70% signal capture
- Best for noisy environments

### Balanced (default)
```pebl
result <- CalibrateVoiceKeyThreshold(noisePower, speechPower, 0.15, 95, 75)
```
- 95% noise rejection
- 75% signal capture
- Recommended for most applications

### Sensitive (maximize detection)
```pebl
result <- CalibrateVoiceKeyThreshold(noisePower, speechPower, 0.15, 90, 85)
```
- 90% noise rejection
- 85% signal capture
- Best for quiet environments with soft-spoken participants

---

## Implementation Notes

### Power vs. Energy
The system uses **power** (RMS) values, not energy. If you want the VoiceKey C++ implementation to also use power instead of energy, change line 432 in `src/platforms/sdl/PlatformAudioIn.cpp`:

**Current (uses energy):**
```cpp
powerbins[tickID] = energy;
```

**Change to (use power):**
```cpp
powerbins[tickID] = power;
```

Then recompile with `make clean && make main`.

### Calibration Word Selection
The default words ("WISCONSIN", "SUSPECT", "CHRISTMAS", "HARMONY", "APARTMENT") were chosen for diverse phonetic onsets:
- **W** - labial glide (low energy onset)
- **S** - voiceless fricative (moderate energy)
- **C/K** - voiceless stop (burst onset)
- **H** - voiceless glottal fricative (breathy onset)
- **A** - vowel (immediate high energy)

You can customize for your language/application.

### Sustain Parameter
The `sustain` parameter in `GetVocalResponseTime()` controls temporal smoothing:
- **50ms** - Very responsive, may false-trigger on clicks
- **100ms** - Balanced (recommended)
- **150ms** - Conservative, may miss fast speech onsets

---

## Troubleshooting

### Problem: Threshold too high (missing detections)
**Solution:** Lower `noiseReject` or raise `signalCapture`
```pebl
result <- CalibrateVoiceKeyThreshold(noisePower, speechPower, 0.15, 90, 80)
```

### Problem: Threshold too low (false alarms)
**Solution:** Raise `noiseReject` or lower `signalCapture`
```pebl
result <- CalibrateVoiceKeyThreshold(noisePower, speechPower, 0.15, 98, 70)
```

### Problem: ROC finds no valid threshold
**Check:**
1. Noise and speech data actually overlap
2. Power values are reasonable (0.0-1.0 range)
3. Try increasing `numBins` for finer resolution

**Debug:**
```pebl
Print("Noise range: " + Min(noisePower) + " to " + Max(noisePower))
Print("Speech range: " + Min(speechPower) + " to " + Max(speechPower))
```

### Problem: Inconsistent performance across participants
**Solution:** Re-calibrate for each participant:
```pebl
## Run calibration at start of each session
result <- CalibrateVoiceKeyThreshold(noisePower, speechPower)
gThreshold <- First(result)

## Save participant-specific threshold
FilePrint(gCalibFile, gSubNum + "," + gThreshold)
```

---

## References

- PEBL Voice Key implementation: `src/platforms/sdl/PlatformAudioIn.cpp:339`
- Audio statistics computation: `src/platforms/sdl/PlatformAudioIn.cpp:486`
- Quantile function: `pebl-lib/Math.pbl:201`
