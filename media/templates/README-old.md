# PEBL Test Templates

This directory contains validated template files for creating new PEBL tests from the launcher.

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

## Using Templates

Templates are loaded automatically by the PEBL Launcher when creating a new test. Each template includes:
- ✓ Subject number collection
- ✓ Proper data file creation with CSV headers
- ✓ Timestamp and RT measurement
- ✓ Complete Start() function that runs
- ✓ Validated syntax (all templates pass pebl-validator)

## Customizing Templates

After creating a test from a template:
1. Edit the `.pbl` file to customize stimuli, trial counts, etc.
2. Create a `params/` directory with a `.pbl.schema.json` file for parameters
3. Create a `translations/` directory for multi-language support
4. Add a `.pbl.about.txt` file describing the test

## Template Validation

All templates have been validated with `pebl-validator` and confirmed to have valid PEBL syntax.
