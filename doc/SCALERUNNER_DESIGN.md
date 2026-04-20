# ScaleRunner System Design

## Overview

The ScaleRunner system transforms PEBL's fragmented survey/questionnaire infrastructure into a unified, extensible platform supporting standardized psychological scales and custom questionnaires.

**Goals**:
1. Eliminate code duplication across 7+ scale implementations
2. Support multiple question types from across the codebase
3. Enable reverse coding and composite scoring
4. Provide argument-based scale selection: `RunScale("bigfive", options)`
5. Generate formatted reports with scale scores
6. Support online parameter selection via schema

---

## Current Question Type Inventory

### 1. From survey.pbl (CSV-based)
- `inst` - Instruction screens
- `short` - Short text entry
- `long` - Long text entry with edit/confirm
- `multi` - Multiple choice (select one)
- `multicheck` - Multiple selection (check all that apply)
- `likert` - Horizontal Likert scale (click rectangles)
- `image` - Display image only
- `imageresponse` - Image with text response

### 2. From Custom Scale Implementations

**Horizontal Likert (SUS, BigFive, SSSQ, SimSickness)**:
- Horizontal row of numbered clickable rectangles
- Labels below each option
- Response labels from translations
- Usually 4-5 points, configurable
- Returns: `response` value or `[response, RT]`

**VAS - Visual Analog Scale (TLX, tiredness, KSS)**:
- Vertical scale with slider/click positioning
- Continuous or discrete (0-20, 1-21, etc.)
- Tick marks at key points
- Anchor labels at top/bottom
- Red marker shows current selection
- Returns: numeric rating + timing

**Grid/Table Rating (FASCW)**:
- Multiple items displayed simultaneously
- Same scale for all items
- Click on any item to rate
- Visual selection feedback
- Must rate all before continuing
- Returns: list of ratings

**Pairwise Comparison (TLX weighting phase)**:
- Present two options side-by-side
- Click to select preferred option
- Track counts across all comparisons
- Returns: weight vector

---

## JSON Scale Definition Schema

```json
{
  "scale_info": {
    "name": "Big Five Personality Inventory",
    "code": "bigfive",
    "version": "1.0",
    "citation": "Goldberg, L. R. (1992). Psychological Assessment, 4, 26-42.",
    "description": "50-item IPIP representation of Big-Five factor structure"
  },

  "parameters": {
    "shuffle_questions": true,
    "do_ext": true,
    "do_agr": true,
    "do_con": true,
    "do_emo": true,
    "do_int": true
  },

  "dimensions": [
    {
      "id": "ext",
      "name": "Extraversion",
      "enabled_param": "do_ext"
    },
    {
      "id": "agr",
      "name": "Agreeableness",
      "enabled_param": "do_agr"
    },
    {
      "id": "con",
      "name": "Conscientiousness",
      "enabled_param": "do_con"
    },
    {
      "id": "emo",
      "name": "Emotional Stability",
      "enabled_param": "do_emo"
    },
    {
      "id": "int",
      "name": "Intellect/Imagination",
      "enabled_param": "do_int"
    }
  ],

  "likert_options": {
    "points": 5,
    "labels": ["likert_1", "likert_2", "likert_3", "likert_4", "likert_5"],
    "question_head": "question_head"
  },

  "questions": [
    {
      "id": "q1",
      "text_key": "q1",
      "type": "likert",
      "dimension": "ext",
      "coding": 1
    },
    {
      "id": "q2",
      "text_key": "q2",
      "type": "likert",
      "dimension": "agr",
      "coding": -1
    },
    {
      "id": "q3",
      "text_key": "q3",
      "type": "likert",
      "dimension": "con",
      "coding": 1
    }
  ],

  "scoring": {
    "ext": {
      "method": "sum_coded",
      "items": ["q1", "q6", "q11", "q16", "q21", "q26", "q31", "q36", "q41", "q46"],
      "description": "Sum of coded responses (reverse-coded where coding=-1)"
    },
    "agr": {
      "method": "sum_coded",
      "items": ["q2", "q7", "q12", "q17", "q22", "q27", "q32", "q37", "q42", "q47"],
      "description": "Sum of coded responses"
    }
  },

  "report": {
    "template": "standard",
    "include": ["timestamp", "completion_time", "dimension_scores"],
    "header": "50-item IPIP Big-Five Factor Structure",
    "footer": "See http://ipip.ori.org/ for more information"
  },

  "data_output": {
    "individual_file": "{scale_code}-{subnum}.csv",
    "pooled_file": "{scale_code}-pooled.csv",
    "columns": "subnum,timestamp,time,q1,q2,q3,...,EXT,AGR,CONS,EMO,INT"
  }
}
```

### Extended Question Type Definitions

```json
{
  "questions": [
    {
      "id": "q1",
      "text_key": "q1",
      "type": "likert",
      "points": 5,
      "dimension": "ext",
      "coding": 1
    },
    {
      "id": "mental_demand",
      "text_key": "scale_1_question",
      "type": "vas",
      "points": 21,
      "anchors": ["scale_1_low", "scale_1_high"],
      "dimension": "mental",
      "coding": 1
    },
    {
      "id": "consent",
      "text_key": "consent_text",
      "type": "inst"
    },
    {
      "id": "feedback",
      "text_key": "feedback_prompt",
      "type": "long"
    },
    {
      "id": "grid_fatigue",
      "text_key": "fatigue_header",
      "type": "grid",
      "items": ["item1", "item2", "item3", "item4", "item5"],
      "points": 5,
      "anchors": ["anchor_1", "anchor_2", "anchor_3", "anchor_4", "anchor_5"],
      "dimension": "fatigue",
      "coding": 1
    }
  ]
}
```

---

## Coding System

### Reverse Coding

**Problem**: Some questionnaire items are worded negatively and need reverse scoring.

**Solution**: `coding` field in question definition
- `coding: 1` = Normal scoring (response value used as-is)
- `coding: -1` = Reverse coding (score = max_points + 1 - response)

**Example** (5-point scale):
```
Response: 2
Normal coding (1):   score = 2
Reverse coding (-1): score = 5 + 1 - 2 = 4
```

**Implementation**:
```pebl
define CodeResponse(response, coding, maxPoints)
{
  if(coding < 0)
  {
    return(maxPoints + 1 - response)
  } else {
    return(response)
  }
}
```

### Composite Scoring

**Methods**:
1. `sum_coded` - Sum of coded responses
2. `mean_coded` - Mean of coded responses
3. `weighted_sum` - Weighted sum (for TLX-style scales)
4. `custom` - Call custom scoring function

**Implementation**:
```pebl
define ComputeCompositeScore(dimension, scoringDef, responses, questions)
{
  method <- scoringDef.method
  items <- scoringDef.items

  values <- []
  loop(qid, items)
  {
    ## Find question definition
    qdef <- FindQuestion(questions, qid)
    resp <- GetResponse(responses, qid)

    ## Apply coding
    coded <- CodeResponse(resp, qdef.coding, qdef.points)
    PushOnEnd(values, coded)
  }

  if(method == "sum_coded")
  {
    return(Sum(values))
  } elseif(method == "mean_coded")
  {
    return(Mean(values))
  } elseif(method == "weighted_sum")
  {
    weights <- scoringDef.weights
    return(Sum(VecTimes(values, weights)))
  }
}
```

---

## ScaleRunner.pbl Implementation Plan

### Core Structure

```pebl
define Start(p)
{
  ## Option A: Argument-based selection
  parpairs <- [["scale", "bigfive"],
               ["shuffle_questions", 1],
               ["do_ext", 1],
               ["do_agr", 1],
               ## ... other scale-specific params
              ]

  gParams <- CreateParameters(parpairs, gParamFile)

  ## Load scale definition
  scaleDef <- LoadScaleDefinition(gParams.scale)

  ## Override parameters from scaleDef with gParams
  scaleDef <- MergeParameters(scaleDef, gParams)

  ## Initialize
  gWin <- MakeWindow("black")
  InitializeLSL(gWin)
  InitializeUpload()

  if(gSubNum+"" == "0")
  {
    gSubNum <- GetSubNum(gWin)
  }

  ## Load translations
  GetStrings(gLanguage, scaleDef.scale_info.code)

  ## Run scale
  results <- RunScale(scaleDef)

  ## Compute scores
  scores <- ComputeScores(results, scaleDef)

  ## Save data
  SaveData(results, scores, scaleDef)

  ## Generate report
  GenerateReport(scores, scaleDef)

  ## Upload
  UploadResults(scaleDef)

  FinalizeLSL()
}
```

### Question Type Handlers

```pebl
define DoQuestion(qdef, scaleDef)
{
  type <- qdef.type

  if(type == "likert")
  {
    return(DoLikert(qdef, scaleDef))
  } elseif(type == "vas")
  {
    return(DoVAS(qdef, scaleDef))
  } elseif(type == "grid")
  {
    return(DoGrid(qdef, scaleDef))
  } elseif(type == "short")
  {
    return(DoShortAnswer(qdef))
  } elseif(type == "long")
  {
    return(DoLongAnswer(qdef))
  } elseif(type == "multi")
  {
    return(DoMultiChoice(qdef))
  } elseif(type == "multicheck")
  {
    return(DoMultiCheck(qdef))
  } elseif(type == "inst")
  {
    return(DoInstructions(qdef))
  } else {
    SignalFatalError("Unknown question type: " + type)
  }
}
```

### Unified Likert Handler

```pebl
define DoLikert(qdef, scaleDef)
{
  ## Get configuration
  points <- qdef.points
  if(IsProperty(qdef, "points"))
  {
    points <- qdef.points
  } else {
    points <- scaleDef.likert_options.points
  }

  ## Get question text
  questionText <- GetProperty(gStrings, qdef.text_key)
  questionHead <- GetProperty(gStrings, scaleDef.likert_options.question_head)

  ## Create display
  head <- MakeLabel(questionHead, gQuestFont)
  AddObject(head, gWin)
  Move(head, gVideoWidth/2, 150)

  quest <- MakeLabel(questionText, gQuestFont)
  AddObject(quest, gWin)
  Move(quest, gVideoWidth/2, 300)

  ## Create response options
  skip <- 150
  left <- gVideoWidth/2 - skip * (points/2)
  yval <- gVideoHeight/2 + 100

  backs <- []
  labels <- []

  loop(i, Sequence(1, points, 1))
  {
    x <- left + skip * i
    back <- Rectangle(x, yval+50, skip-10, 180, MakeColor("white"), 0)
    AddObject(back, gWin)
    PushOnEnd(backs, back)

    num <- MakeLabel(i+"", gQuestFont)
    Move(num, x, yval)
    AddObject(num, gWin)

    labelKey <- Nth(scaleDef.likert_options.labels, i)
    labelText <- GetProperty(gStrings, labelKey)
    optLabel <- MakeTextBox(labelText, gSmallFont, skip-15, 80)
    AddObject(optLabel, gWin)
    Move(optLabel, x-optLabel.width/2, yval+50)
  }

  Draw()

  t1 <- GetTime()
  response <- WaitForClickOnTarget(backs, Sequence(1, points, 1))
  t2 <- GetTime()

  ## Clean up
  RemoveObjects([head, quest], gWin)
  RemoveObjects(backs, gWin)

  return([response, t2-t1])
}
```

### VAS Handler (from tiredness.pbl / TLX.pbl)

```pebl
define DoVAS(qdef, scaleDef)
{
  ## Create vertical scale with click positioning
  ## See tiredness.pbl PlotScale() and PlotScaleKSS() for implementation
  ## Returns rating value (1-21 or configurable range)
}
```

### Grid Handler (from FASCW.pbl)

```pebl
define DoGrid(qdef, scaleDef)
{
  ## Display multiple items on one screen
  ## Create rating lines for each item
  ## Allow clicking in any order
  ## Show "Done" button when all rated
  ## Returns list of ratings
}
```

---

## Scale Definition Files

**Location**: `battery/scales/definitions/`

**Naming**: `{scale_code}.json`

**Examples**:
- `bigfive.json` - Big Five Personality
- `sus.json` - System Usability Scale
- `tlx.json` - NASA Task Load Index
- `fascw.json` - Fatigue Assessment Scale
- `sssq.json` - Short Stress State Questionnaire
- `kss.json` - Karolinska Sleepiness Scale

---

## Parameter Schema for Online Platform

**File**: `battery/scales/params/ScaleRunner.pbl.schema.json`

```json
{
  "scale": {
    "type": "select",
    "default": "bigfive",
    "options": [
      {"value": "bigfive", "label": "Big Five Personality Inventory (50 items)"},
      {"value": "sus", "label": "System Usability Scale (10 items)"},
      {"value": "tlx", "label": "NASA Task Load Index (6 dimensions)"},
      {"value": "fascw", "label": "Fatigue Assessment Scale (10 items)"},
      {"value": "sssq", "label": "Short Stress State Questionnaire (24 items)"},
      {"value": "kss", "label": "Karolinska Sleepiness Scale (1 item)"}
    ],
    "description": "Select psychological scale to administer"
  },
  "shuffle_questions": {
    "type": "boolean",
    "default": 0,
    "description": "Randomize question order (if supported by scale)"
  }
}
```

**Note**: Scale-specific parameters (like `do_ext`, `do_agr`) are defined in individual scale JSON files and loaded dynamically.

---

## Report Generation

### Standard Report Format

```
--------------------------------------------------------------------
[Scale Name]
[Citation]
--------------------------------------------------------------------

Timestamp: 2026-02-09 15:30:45
Participant Code: P001
Completion Time: 4.2 minutes

Dimension Scores:
  Extraversion:              42
  Agreeableness:             38
  Conscientiousness:         45
  Emotional Stability:       35
  Intellect/Imagination:     41

--------------------------------------------------------------------
Generated with PEBL ScaleRunner
http://pebl.sf.net
--------------------------------------------------------------------
```

### Implementation

```pebl
define GenerateReport(scores, scaleDef)
{
  reportFile <- GetNewDataFile(gSubNum, gWin, scaleDef.scale_info.code + "-report", "txt", "")

  FilePrint(reportFile, CR(2) + Repeat("-", 70))
  FilePrint(reportFile, scaleDef.scale_info.name)
  FilePrint(reportFile, scaleDef.scale_info.citation)
  FilePrint(reportFile, Repeat("-", 70))
  FilePrint(reportFile, "")
  FilePrint(reportFile, "Timestamp: " + TimeStamp())
  FilePrint(reportFile, "Participant Code: " + gSubNum)
  FilePrint(reportFile, "Completion Time: " + scores.completion_time + " minutes")
  FilePrint(reportFile, "")
  FilePrint(reportFile, "Dimension Scores:")

  loop(dim, scaleDef.dimensions)
  {
    dimScore <- GetProperty(scores, dim.id)
    FilePrint(reportFile, Format("  " + dim.name + ":", 30) + dimScore)
  }

  FilePrint(reportFile, "")
  FilePrint(reportFile, Repeat("-", 70))
  FilePrint(reportFile, "Generated with PEBL ScaleRunner")
  FilePrint(reportFile, "http://pebl.sf.net")
  FilePrint(reportFile, Repeat("-", 70))

  FileClose(reportFile)

  return(reportFile)
}
```

---

## Migration Strategy

### Phase 1: Core Implementation (Week 1-2)
1. Create `ScaleRunner.pbl` based on `survey.pbl`
2. Implement unified Likert handler
3. Implement coding/scoring system
4. Implement report generation
5. Test with simple scale

### Phase 2: Question Types (Week 2-3)
1. Integrate VAS from tiredness.pbl
2. Integrate Grid from FASCW.pbl
3. Port all survey.pbl question types
4. Test each type independently

### Phase 3: Scale Definitions (Week 3-4)
1. Create BigFive.json
2. Create SUS.json
3. Create TLX.json (with pairwise weighting)
4. Create FASCW.json
5. Create SSSQ.json
6. Create KSS.json
7. Test all scales

### Phase 4: Integration (Week 4-5)
1. Update parameter schema
2. Add to online platform battery list
3. Create documentation
4. Migrate existing tests (optional)

---

## Backward Compatibility

**Existing scales remain functional**:
- `battery/bigfive/bigfive.pbl` - Keep as-is
- `battery/SUS/SystemUsabilityScale.pbl` - Keep as-is
- `battery/TLX/TLX.pbl` - Keep as-is

**New ScaleRunner location**:
- `battery/scales/ScaleRunner.pbl` - New unified runner
- `battery/scales/definitions/*.json` - Scale definitions

**Users can choose**:
- Run old implementations directly (backward compatible)
- Run via ScaleRunner for new features (composite scoring, reports)

---

## Future Enhancements

1. **Online Scale Builder**: Web interface to create custom scale JSON files
2. **Scale Validation**: Automatic validation of scale JSON against schema
3. **Multi-language scales**: Automatic translation loading
4. **Conditional Questions**: Branching logic based on previous responses
5. **Adaptive Scales**: CAT (Computerized Adaptive Testing) support
6. **Scale Library**: Public repository of validated scales
