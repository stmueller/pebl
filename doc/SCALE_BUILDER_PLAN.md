# PEBL Scale Builder/Editor Implementation Plan

## Overview

Create an interactive scale generator and editor to make creating psychological scales accessible without manual JSON editing.

## Architecture Decision

**Recommendation: PEBL-based GUI (Option A)**

### Option A: PEBL Script GUI ✅ RECOMMENDED
**Pros:**
- Leverages existing UI.pbl widgets (buttons, text entry, scrollboxes)
- No C++ changes needed for basic functionality
- Can iterate quickly
- Users can modify/extend it
- Cross-platform automatically (native + Emscripten)
- Immediate JSON output

**Cons:**
- Limited to PEBL's UI capabilities
- May be slower for complex operations

### Option B: C++ Native GUI
**Pros:**
- Better UX potential
- More responsive
- Could integrate with launcher

**Cons:**
- Requires significant C++ development
- SDL2 GUI from scratch
- Harder to maintain
- Not easily customizable by users

### Option C: Hybrid
**Pros:**
- Best of both worlds

**Cons:**
- Complex architecture
- More moving parts

## Implementation: PEBL-Based Scale Builder

### File Structure
```
battery/scales/
├── ScaleBuilder.pbl           # Main scale editor/builder
├── ScaleRunner.pbl            # Existing runner
├── scale-validator.pbl        # Existing validator
├── templates/
│   └── empty-scale.json       # Template for new scales
├── definitions/
│   └── *.json                 # Scale definitions
└── translations/
    └── *.pbl-*.json           # Translation files
```

### Core Modules

#### 1. Main ScaleBuilder.pbl

**Entry Point:**
```pebl
define Start(p)
{
  gWin <- MakeWindow("white")

  ## Main menu
  ShowMainMenu()

  ## Options: New Scale, Edit Scale, Test Scale, Validate Scale, Exit
}
```

#### 2. Scale Metadata Editor

**UI Components:**
- Text entry for: name, code, abbreviation, description
- Text area for: citation, DOI, reference_url
- Dropdown for: license type
- Tags input (comma-separated)

**Data Structure:**
```json
{
  "scale_info": {
    "name": "",
    "code": "",
    "abbreviation": "",
    "description": "",
    "version": "1.0",
    "author": "",
    "year": "",
    "citation": "",
    "doi": "",
    "reference_url": "",
    "license": "",
    "license_url": ""
  }
}
```

#### 3. Question Editor

**UI Layout:**
- Left panel: Question list (scrollable)
  - Add/Remove/Reorder buttons
  - Question IDs and types shown
- Right panel: Question details
  - Type dropdown
  - ID text field
  - Text key field
  - Dimension dropdown
  - Coding (+1/-1 for reverse scoring)
  - Type-specific options

**Question Types:**
```
inst          - Instruction screen
likert        - Likert scale (1-N points)
vas           - Visual analog scale
grid          - Multiple items, one scale
short         - Short text entry
long          - Long text entry
multi         - Multiple choice (radio)
multicheck    - Multiple selection (checkbox)
image         - Image display
imageresponse - Image with text response
```

**Type-Specific Configuration:**

*For Likert:*
- Number of points (3, 5, 7, etc.)
- Label keys for endpoints
- Display mode (horizontal/vertical)

*For Grid:*
- Sub-items list
- Shared scale configuration

*For Multi/Multicheck:*
- Options list with values

#### 4. Translation Editor

**UI:**
- Language selector (en, de, es, fr, etc.)
- Key-value editor
- Auto-populate keys from questions
- Text area for each translation key

**Required Keys:**
- `LANGUAGE` - language code
- `inst1`, `inst2`, etc. - instruction texts
- `{question_id}` - question text for each question
- `debrief` - debrief message
- Likert labels: `likert_1`, `likert_2`, etc.
- Custom labels per question type

#### 5. Scoring & Dimensions Editor

**UI:**
- Dimension list
  - Add/Remove dimensions
  - Name and ID fields
  - Description
- Scoring methods per dimension
  - `sum_coded` - sum with reverse coding
  - `mean_coded` - mean with reverse coding
  - `weighted_sum` - weighted sum
  - `sum_correct` - count correct answers
- Item selection for each dimension
  - Checklist of questions
  - Shows which items included

**Data Structure:**
```json
{
  "dimensions": [
    {
      "id": "POE",
      "name": "Perseverance of Effort",
      "description": "..."
    }
  ],
  "scoring": {
    "POE": {
      "method": "mean_coded",
      "items": ["grit2", "grit6", "grit8"]
    }
  }
}
```

#### 6. Preview/Test Integration

**Functionality:**
- "Preview Scale" button
- Saves temporary JSON
- Launches ScaleRunner with temp scale
- User can test interactively
- Returns to editor after

#### 7. Validation Integration

**Real-time Validation:**
- Call scale-validator.pbl
- Show errors/warnings inline
- Highlight problematic fields
- Must validate before saving

### User Workflow

#### Creating a New Scale

1. **Launch Builder**
   ```bash
   bin/pebl2 battery/scales/ScaleBuilder.pbl
   ```

2. **Main Menu → "New Scale"**
   - Prompts for scale code (e.g., "mysscale")
   - Creates empty template
   - Opens editor

3. **Edit Scale Info**
   - Fill in name, description, citation
   - Set license information

4. **Add Questions**
   - Click "Add Question"
   - Select type (likert, short, etc.)
   - Enter question ID and text key
   - Configure type-specific options
   - Set dimension and coding
   - Repeat for all questions

5. **Add Translations**
   - Switch to "Translations" tab
   - Enter text for each text_key
   - Add instruction texts
   - Set likert labels

6. **Configure Scoring**
   - Define dimensions
   - Select scoring method
   - Choose items for each dimension

7. **Validate**
   - Click "Validate"
   - Fix any errors shown

8. **Save**
   - Saves to `definitions/{code}.json`
   - Saves to `translations/{code}.pbl-en.json`

9. **Test**
   - Click "Test Scale"
   - Launches ScaleRunner
   - Take the scale interactively

#### Editing an Existing Scale

1. **Main Menu → "Edit Scale"**
   - Shows list of existing scales
   - Select one to edit

2. **Edit as needed**
   - All tabs available
   - Modify any aspect

3. **Validate & Save**

### Implementation Phases

#### Phase 1: Basic Editor (Week 1)
- ✅ Create ScaleBuilder.pbl skeleton
- ✅ Implement main menu
- ✅ Scale metadata editor
- ✅ Save/load JSON for scale_info
- ✅ Basic validation

**Deliverable:** Can create scale metadata and save JSON

#### Phase 2: Question Editor (Week 2)
- ✅ Question list UI
- ✅ Add/remove questions
- ✅ Question type selection
- ✅ Type-specific configuration dialogs
- ✅ Save questions to JSON

**Deliverable:** Can define questions and save complete structure

#### Phase 3: Translation Editor (Week 3)
- ✅ Translation key-value editor
- ✅ Auto-generate keys from questions
- ✅ Multi-language support
- ✅ Save translation JSON

**Deliverable:** Can create complete translations

#### Phase 4: Scoring Editor (Week 4)
- ✅ Dimension definition UI
- ✅ Scoring method selection
- ✅ Item picker for dimensions
- ✅ Save scoring configuration

**Deliverable:** Can define full scoring system

#### Phase 5: Integration & Testing (Week 5)
- ✅ Integrate validator
- ✅ Test scale preview/launch
- ✅ Polish UI
- ✅ Error handling
- ✅ Documentation

**Deliverable:** Fully functional scale builder

### UI Component Inventory (from UI.pbl)

**Available Components:**
- `MakeButton()` - clickable buttons
- `MakeTextEntry()` - single-line text input
- `MakeTextBox()` - multi-line text display
- `MakeScrollBox()` - scrollable content area
- `MakePullDown()` - dropdown selection
- `MakeCheckBox()` - checkbox widget
- `PopUpEntryBox()` - dialog for text input
- `FilePicker()` - file selection dialog
- `MakeMenu()` - menu system

**Custom Components Needed:**
- Question list with reordering
- Key-value pair editor
- Item picker/checklist

### Data Flow

```
User Input (UI)
    ↓
PEBL Data Structures (lists, custom objects)
    ↓
JSON Generation (using existing pattern from ScaleRunner)
    ↓
File I/O (FileOpenWrite, FilePrint)
    ↓
Validation (scale-validator.pbl)
    ↓
Testing (ScaleRunner.pbl)
```

### Example: Question Editor Implementation

```pebl
define EditQuestion(question, allDimensions)
{
  ## question is custom object with properties:
  ## - id, type, text_key, dimension, coding

  win <- MakeWindow("grey")

  ## ID entry
  idLabel <- EasyLabel("Question ID:", 50, 50, win, 18)
  idEntry <- MakeTextEntry("", 18, win)
  SetText(idEntry, question.id)
  AddObject(idEntry, win)
  Move(idEntry, 200, 50)

  ## Type dropdown
  typeLabel <- EasyLabel("Type:", 50, 100, win, 18)
  types <- ["inst", "likert", "vas", "grid", "short", "long",
            "multi", "multicheck", "image", "imageresponse"]
  typePullDown <- MakePullDown(types, 18, win, 200)
  AddObject(typePullDown, win)
  Move(typePullDown, 200, 100)
  SelectPullDownByText(typePullDown, question.type)

  ## Text key entry
  textKeyLabel <- EasyLabel("Text Key:", 50, 150, win, 18)
  textKeyEntry <- MakeTextEntry("", 18, win)
  SetText(textKeyEntry, question.text_key)
  AddObject(textKeyEntry, win)
  Move(textKeyEntry, 200, 150)

  ## Dimension dropdown
  dimLabel <- EasyLabel("Dimension:", 50, 200, win, 18)
  dimPullDown <- MakePullDown(allDimensions, 18, win, 200)
  AddObject(dimPullDown, win)
  Move(dimPullDown, 200, 200)
  SelectPullDownByText(dimPullDown, question.dimension)

  ## Coding radio buttons
  codingLabel <- EasyLabel("Coding:", 50, 250, win, 18)
  normalCoding <- MakeCheckBox("Normal (+1)", 18, win)
  reverseCoding <- MakeCheckBox("Reverse (-1)", 18, win)
  AddObject(normalCoding, win)
  AddObject(reverseCoding, win)
  Move(normalCoding, 200, 250)
  Move(reverseCoding, 200, 280)

  if(question.coding == 1)
  {
    SetCheckBox(normalCoding, 1)
  } else {
    SetCheckBox(reverseCoding, 1)
  }

  ## Save/Cancel buttons
  saveBtn <- MakeButton("Save", 18, win)
  cancelBtn <- MakeButton("Cancel", 18, win)
  AddObject(saveBtn, win)
  AddObject(cancelBtn, win)
  Move(saveBtn, 200, 350)
  Move(cancelBtn, 300, 350)

  Draw()

  ## Event loop for button clicks
  done <- 0
  savedQuestion <- question

  while(not done)
  {
    click <- WaitForMouseButton()

    if(ClickOn(saveBtn, click))
    {
      ## Update question object
      savedQuestion.id <- GetText(idEntry)
      savedQuestion.text_key <- GetText(textKeyEntry)
      savedQuestion.type <- GetPullDownValue(typePullDown)
      savedQuestion.dimension <- GetPullDownValue(dimPullDown)

      if(GetCheckBox(normalCoding))
      {
        savedQuestion.coding <- 1
      } else {
        savedQuestion.coding <- -1
      }

      done <- 1
    }

    if(ClickOn(cancelBtn, click))
    {
      done <- 1
    }
  }

  return(savedQuestion)
}
```

### JSON Export Pattern

```pebl
define ExportScaleJSON(scale, filename)
{
  ## scale is custom object with all scale data

  file <- FileOpenWrite(filename)

  ## Write scale_info
  FilePrint(file, "{")
  FilePrint(file, "  " + Quote("scale_info") + ": {")
  FilePrint(file, "    " + Quote("name") + ": " + Quote(scale.name) + ",")
  FilePrint(file, "    " + Quote("code") + ": " + Quote(scale.code) + ",")
  ## ... etc
  FilePrint(file, "  },")

  ## Write questions array
  FilePrint(file, "  " + Quote("questions") + ": [")

  qnum <- 1
  loop(question, scale.questions)
  {
    FilePrint(file, "    {")
    FilePrint(file, "      " + Quote("id") + ": " + Quote(question.id) + ",")
    FilePrint(file, "      " + Quote("type") + ": " + Quote(question.type) + ",")
    FilePrint(file, "      " + Quote("text_key") + ": " + Quote(question.text_key))

    if(qnum < Length(scale.questions))
    {
      FilePrint(file, "    },")
    } else {
      FilePrint(file, "    }")
    }

    qnum <- qnum + 1
  }

  FilePrint(file, "  ]")
  FilePrint(file, "}")

  FileClose(file)
}

define Quote(str)
{
  return("\"" + str + "\"")
}
```

### Testing Strategy

1. **Unit Testing**
   - Test each editor component individually
   - Verify JSON output format
   - Validate data structures

2. **Integration Testing**
   - Create scale from scratch
   - Edit existing scale
   - Validate output
   - Test with ScaleRunner

3. **User Testing**
   - Have researchers create scales
   - Gather feedback on UX
   - Iterate on design

### Documentation Needed

1. **User Guide**
   - How to launch scale builder
   - Creating a new scale step-by-step
   - Editing existing scales
   - Question type reference
   - Scoring method explanations

2. **Developer Guide**
   - Code structure
   - Adding new question types
   - Extending functionality
   - JSON format specification

### Future Enhancements

**Phase 6+:**
- Import from CSV/spreadsheet
- Question library/templates
- Multi-language translation wizard
- Online version (pebl-online integration)
- Collaborative editing
- Version control/history
- Scale validation against published versions
- Automated scoring verification
- Response simulation/testing

### Advantages of This Approach

1. **Accessibility**: Non-programmers can create scales
2. **Validation**: Built-in validation prevents errors
3. **Testing**: Immediate preview with ScaleRunner
4. **Portability**: Pure PEBL, works everywhere
5. **Extensibility**: Users can modify the builder itself
6. **Integration**: Seamless with existing ScaleRunner ecosystem

### Technical Challenges & Solutions

**Challenge 1: Complex UI in PEBL**
- Solution: Use existing UI.pbl widgets, keep interface simple

**Challenge 2: JSON Generation**
- Solution: Manual string building with proper escaping

**Challenge 3: State Management**
- Solution: Use custom objects with properties

**Challenge 4: Real-time Validation**
- Solution: Call validator script, parse output

**Challenge 5: Large Scales (50+ questions)**
- Solution: Scrollable question list, pagination if needed

### Next Steps

1. Create ScaleBuilder.pbl skeleton with main menu
2. Implement scale metadata editor
3. Test save/load cycle
4. Build question editor
5. Iterate based on testing

Would you like me to start implementing Phase 1?
