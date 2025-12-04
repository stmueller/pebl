# PEBL Layout & Response System Template

This template demonstrates the PEBL Layout & Response System with colored visualization zones and configurable response modes.

## Usage

Simply run the template and choose your response mode when prompted:

```bash
cd battery/template
../../bin/pebl2 template.pbl -v subnum=999
```

A dialog will appear asking you to choose between:
1. **Keyboard (Shift Keys)** - Use left/right shift keys
2. **Mouse (Click Targets)** - Click on labeled targets

## Layout Zones

The layout is divided into five zones (visualized with colored rectangles):

- **Header** (yellow) - Test title and trial counter
- **Subheader** (cyan) - Status information  
- **Stimulus** (green) - Main content area for task stimuli
- **Response** (blue) - Response key/target labels
- **Footer** (red) - Instructions and prompts

## Response Modes

### Keyboard Mode (Shift Keys)
- Labels show: "LEFT-SHIFT" and "RIGHT-SHIFT"
- Instructions: "Press the keys shown below"
- Responses recorded as: "left" or "right"

### Mouse Target Mode (Click on Labels)
- Labels show: "Click LEFT" and "Click RIGHT"
- Instructions: "Click on one of the targets below"
- Participant clicks directly on the response labels
- Responses recorded as: "left" or "right"

## Using JSON Parameter Files (Optional)

You can also bypass the dialog and use JSON parameter files:

```bash
../../bin/pebl2 template.pbl -v subnum=999 -v jsonparam=params/keyboard.json
../../bin/pebl2 template.pbl -v subnum=999 -v jsonparam=params/mouse.json
```

Or override individual parameters via command line:
```bash
../../bin/pebl2 template.pbl -v subnum=999 -v responsemode=mousetarget
../../bin/pebl2 template.pbl -v subnum=999 -v responsemode=keyboardSafe
```

## Trial Structure

- **Practice trials**: 5 trials with feedback showing response and RT
- **Test trials**: 20 trials without feedback
- **Data recorded**: subnum, trial, phase, response (left/right), rt, timestamp

## Parameters

Available parameters (defined in `params/template.pbl.schema.json`):

- `numPracticeTrials` - Number of practice trials (default: 5)
- `numTestTrials` - Number of test trials (default: 20)
- `showFeedback` - Show RT feedback during practice (1=yes, 0=no)
- `responsemode` - Response mode options:
  - `keyboardShift` - Left/Right shift keys (native only, avoids Sticky Keys dialog)
  - `keyboardSafe` - Z and / keys (browser-safe)
  - `mousetarget` - Click on labeled targets
  - `mousebutton` - Left/right click anywhere

## Layout Configuration

The optimized default layout uses:
- **Margins**: 25px (reduced from 50px for maximum stimulus space)
- **Bottom reserve**: 25px (no-go zone in windowed mode)
- **Header**: 50px height (suitable for 44pt font)
- **Subheader**: 25px height (suitable for 22pt font)
- **Response zone**: 50px height
- **Footer**: 50px height (reduced from 80px)
- **Stimulus zone**: Flexible, takes ~70% of screen height

## Files

- `template.pbl` - Main template script with response mode chooser
- `params/template.pbl.schema.json` - Parameter schema definition
- `params/keyboard.json` - Keyboard mode preset
- `params/mouse.json` - Mouse target mode preset
- `README.md` - This file
