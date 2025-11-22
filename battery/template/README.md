# PEBL Battery Task Template

This template provides a complete, documented starting point for creating new PEBL experiments.

## Important Note

This template demonstrates a **recommended approach** for building comprehensive, shareable PEBL tests suitable for the test battery. However, **this is NOT the only way to write PEBL experiments!**

- **Translation system is optional** - You can hard-code text strings directly in your script
- **Parameter schema is optional** - You can hard-code parameter values
- **Data file structure is a suggestion** - Use whatever format suits your needs
- **You can write much simpler experiments** - This template shows best practices, not requirements

Use this template as a guide when creating tests you plan to share or distribute. For personal use or quick prototypes, feel free to simplify significantly.

## Purpose

This is NOT an actual psychological test - it's a template to help you create one. It includes:

- Complete experiment structure (instructions, practice, test, debriefing)
- Parameter system with schema file
- Translation/internationalization support
- Data file creation and recording
- Extensive inline documentation
- TODO markers for customization points

## How to Use This Template

### Manual Approach

1. **Copy the directory:**
   ```bash
   cp -r battery/template battery/yourtaskname
   ```

2. **Rename files:**
   ```bash
   cd battery/yourtaskname
   mv template.pbl yourtaskname.pbl
   mv template.pbl.about.txt yourtaskname.pbl.about.txt
   mv params/template.pbl.schema params/yourtaskname.pbl.schema
   mv translations/template.pbl-en.json translations/yourtaskname.pbl-en.json
   ```

3. **Edit files:**
   - Open `yourtaskname.pbl` and search for `TODO` comments
   - Customize each section based on your experimental needs
   - Update `params/yourtaskname.pbl.schema` with your parameters
   - Update `translations/yourtaskname.pbl-en.json` with your instructions
   - Update `yourtaskname.pbl.about.txt` with task description

4. **Test:**
   ```bash
   bin/pebl2 battery/yourtaskname/yourtaskname.pbl
   ```

### AI/LLM-Assisted Approach

Provide an LLM with:
- This template file (`template.pbl`)
- The PEBL programming guide (`Notes_for_Claude_on_Programming_PEBL.txt`)
- A clear description of your experiment

Example prompt:
```
Based on the PEBL template and programming notes, create a Stroop task where:
- Participants see color words (RED, BLUE, GREEN, YELLOW)
- Words are displayed in congruent or incongruent colors
- Participants press keys to indicate the ink color (not the word)
- Include 10 practice trials and 40 test trials
- Record accuracy and reaction time
```

The LLM should be able to:
- Fill in all TODO sections appropriately
- Create proper experimental design and counterbalancing
- Set up correct response collection
- Configure data output columns
- Write appropriate instructions

## Key Sections to Customize

### In `template.pbl`:

1. **Script name** (line ~31): Change to your task name
2. **Parameters** (lines ~40-65): Define your experimental parameters
3. **Data columns** (lines ~108-110): Match your data output needs
4. **Practice design** (lines ~127-135): Create practice trial structure
5. **Test design** (lines ~167-182): Create experimental conditions
6. **RunTrial function** (lines ~230-380): Implement your trial logic
   - Stimulus creation
   - Response collection
   - Feedback presentation

### In `params/template.pbl.schema`:

Add/modify/remove parameters as needed for your task.

### In `translations/template.pbl-en.json`:

Update all instruction text to explain your specific task.

## Common Experimental Patterns

The template includes commented examples for:

- **Simple conditions:** Shuffled list of conditions
- **Factorial designs:** Full counterbalancing
- **Balanced sampling:** Each condition appears before repeating
- **Keyboard responses:** Single key, multiple keys, timeout
- **Mouse responses:** Click on targets
- **Visual stimuli:** Text, images, shapes
- **Timing:** Fixation, stimulus duration, feedback, ITI

## File Structure

```
battery/template/
├── template.pbl                    # Main experiment file
├── template.pbl.about.txt          # Task description
├── README.md                       # This file
├── params/
│   └── template.pbl.schema         # Parameter definitions
├── translations/
│   └── template.pbl-en.json        # English instructions/text
└── data/                           # Created at runtime
```

## Testing Your New Task

1. Run with default parameters:
   ```bash
   bin/pebl2 battery/yourtaskname/yourtaskname.pbl
   ```

2. Run with specific subject number:
   ```bash
   bin/pebl2 battery/yourtaskname/yourtaskname.pbl -v subnum=123
   ```

3. Check data output in:
   ```
   battery/yourtaskname/data/[subnum]/yourtaskname-[subnum].csv
   ```

4. Verify:
   - All trials run without errors
   - Data file has correct columns
   - Instructions are clear
   - Feedback works as expected
   - Parameters can be modified in schema file

## Additional Resources

- **PEBL Manual:** `doc/PEBLManual2.2.pdf`
- **Programming Guide:** `Notes_for_Claude_on_Programming_PEBL.txt`
- **Function Reference:** Online at https://pebl.sourceforge.net
- **Example Tasks:** Browse `battery/` for similar tasks
- **Community:** pebl-list@lists.sourceforge.net

## Tips for Success

1. **Start simple:** Get basic structure working before adding complexity
2. **Use existing tasks:** Look at similar tasks in the battery for examples
3. **Test frequently:** Run your task after each major change
4. **Check data:** Verify data output matches your expectations
5. **Comment your code:** Explain non-obvious logic for future reference
6. **Use parameters:** Make values configurable rather than hard-coded
7. **Internationalize:** Use translation files even for English-only tasks

## Common Issues

- **Syntax errors:** Check for missing parentheses in `return()` statements
- **Missing return:** All functions must end with `return(value)`
- **Elseif spacing:** Must be `}elseif` with no intervening text
- **Line breaks:** Use `CR(1)` not `\n` for newlines in text
- **NOT operator:** Use `not` keyword, not `!`
- **Global variables:** Must start with lowercase 'g' prefix

Consult `Notes_for_Claude_on_Programming_PEBL.txt` for complete list of common mistakes.
