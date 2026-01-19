# PEBL Test Templates (Data-Driven System)

This directory contains validated template files for creating new PEBL tests from the launcher. The template system is fully data-driven - you can add, remove, or modify templates without recompiling the launcher.

## How It Works

The launcher automatically scans this directory (`media/templates/`) at startup and builds a dynamic list of available templates based on `.pbl` files found. Template names are automatically generated from filenames (e.g., `simple-rt.pbl` → "Simple Rt").

## Available Templates

### 1. Simple Reaction Time (`simple-rt.pbl`)
Measures how quickly participants respond to a stimulus appearing on screen.
- **10 trials**
- **Measures:** Response time
- **Use for:** Basic RT measurement, practice implementing trials

### 2. Choice Reaction Time (`choice-rt.pbl`)
Two-choice task: Press LEFT for 'X', RIGHT for 'O'.
- **20 trials** (10 of each stimulus)
- **Measures:** Response time, accuracy
- **Use for:** Choice RT, response mapping tasks

### 3. Survey/Questionnaire (`questionnaire.pbl`)
Likert-scale questionnaire with RT measurement.
- **3 sample questions** (easily customizable)
- **5-point scale** (Strongly Disagree to Strongly Agree)
- **Measures:** Response choice, RT per question
- **Use for:** Questionnaires, surveys, rating scales

### 4. Memory Test (`memory.pbl`)
Study-test recognition memory paradigm.
- **Study phase:** 5 words presented for 2s each
- **Distractor task:** Count backwards by 7s for 30s
- **Test phase:** Recognition test with 5 targets + 5 foils
- **Measures:** Recognition accuracy, RT
- **Use for:** Memory research, recognition paradigms

### 5. Visual Search (`visual-search.pbl`)
Find the target letter 'T' among distractor 'L's.
- **10 trials** (target present/absent)
- **10 distractors** per trial
- **Measures:** Search accuracy, RT
- **Use for:** Attention research, visual search tasks

### 6. Blank Template (`blank.pbl`)
Minimal starting point for custom tests.
- **Basic structure:** Window, subject number, data file
- **Example trial loop** (commented out)
- **Use for:** Creating completely custom tests

## Adding Custom Templates

To add your own template:

1. **Create a .pbl file** in this directory with your template code
   ```bash
   cd media/templates
   nano my-custom-template.pbl
   ```

2. **Include these minimum requirements:**
   - Complete `Start(p)` function
   - Subject number collection (`if(gSubNum+"" == "0") { gSubNum <- GetSubNum(gWin) }`)
   - Data file creation with CSV header
   - Proper `return(0)` at end

3. **Test with pebl-validator:**
   ```bash
   bin/pebl-validator media/templates/my-custom-template.pbl
   ```

4. **Restart launcher** - Your template will appear in the "New" tab automatically!

The template name in the launcher will be generated from your filename:
- `my-custom-template.pbl` → "My Custom Template"
- `2afc_task.pbl` → "2afc Task"
- `stroop-task.pbl` → "Stroop Task"

## Template Requirements

All templates should include:
- ✓ Complete `define Start(p) { }` function
- ✓ Window creation (`gWin <- MakeWindow(...)`)
- ✓ Subject number collection
- ✓ Data file creation with proper headers
- ✓ Cleanup/thank you message
- ✓ `return(0)` statement
- ✓ Valid PEBL syntax (test with `bin/pebl-validator`)

## Validation

All templates in this directory have been validated with `pebl-validator` and confirmed to have valid PEBL syntax. To validate a template:

```bash
cd /path/to/pebl
bin/pebl-validator media/templates/your-template.pbl
```

## Customizing After Creation

After creating a test from a template in the launcher:

1. Edit the `.pbl` file to customize stimuli, trial counts, etc.
2. Create a `params/` directory with a `.pbl.schema.json` file for parameters
3. Create a `translations/` directory for multi-language support
4. Add a `.pbl.about.txt` file describing the test
5. Add a `.pbl.png` screenshot for the battery browser

## System Notes

- **Location:** Templates were moved from `battery/template/templates/` to `media/templates/` for better organization (media is for shared resources)
- **Scanning:** The launcher scans this directory using `LauncherUI::ScanTemplates()` at startup
- **No hardcoding:** Template list is completely data-driven - add/remove files without recompiling
- **Alphabetical:** Templates appear in alphabetical order by display name
