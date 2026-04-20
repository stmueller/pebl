# ScaleRunner JSON Format Documentation

## Overview

ScaleRunner uses a **two-file system**:
1. **Definition file** (`definitions/{code}.json`) - Structure, scoring, metadata
2. **Translation files** (`translations/{code}.pbl-{lang}.json`) - Actual text content

This separation makes translations easier and keeps the structure clean.

## Definition File Format

### scale_info
```json
{
  "scale_info": {
    "name": "Grit Scale",
    "code": "grit",
    "abbreviation": "Grit-O",
    "description": "Brief description",
    "citation": "Full citation with DOI",
    "license": "License information",
    "version": "1.0",
    "url": "https://scale-website.com"
  }
}
```

### parameters
Runtime parameters for the scale:
```json
{
  "parameters": {
    "shuffle_questions": {
      "type": "boolean",
      "default": 0,
      "description": "Randomize question order"
    }
  }
}
```

### likert_options
Global likert configuration (if scale uses likert questions):
```json
{
  "likert_options": {
    "points": 5,
    "min": -1,
    "max": -1,
    "question_head": "question_head",
    "labels": ["likert_1", "likert_2", "likert_3", "likert_4", "likert_5"]
  }
}
```
Note: `question_head` and `labels` are **keys** that reference translations, not literal text.

**Min/max values:**
- `min` and `max` define the numeric range for response values (optional)
- Default behavior when omitted or set to -1:
  - Binary scales (points=2): range 0-1
  - Regular Likert scales (points≥3): range 1-N where N=points
- Custom ranges enable specialized scales like:
  - Bipolar scales: -10 to +10
  - Percentage scales: 0 to 100
  - Custom ranges: 50 to 300
- These values affect **data storage** and **reverse coding** calculations

### dimensions
Groupings of questions for scoring:
```json
{
  "dimensions": [
    {
      "id": "COI",
      "name": "Consistency of Interest",
      "abbreviation": "COI",
      "description": "The tendency to maintain focus...",
      "enabled_param": null
    }
  ]
}
```

### questions
Question definitions (structure only, no actual text):
```json
{
  "questions": [
    {
      "id": "grit2",
      "text_key": "grit2",
      "type": "likert",
      "dimension": "COI",
      "coding": 1,
      "likert_points": 5,
      "likert_min": -1,
      "likert_max": -1
    }
  ]
}
```

**Question types**: inst, likert, vas, grid, short, long, multi, multicheck, image, imageresponse

**Coding**: 1 = normal, -1 = reverse coded, 0 = not scored (e.g., instructions)

**Likert-specific fields**:
- `likert_points`: Number of response options (overrides scale default if specified)
- `likert_min`: Minimum value for this question (optional, -1 = use scale default)
- `likert_max`: Maximum value for this question (optional, -1 = use scale default)
- `likert_labels`: Array of translation keys for response options (optional, empty = use scale default)

**Value hierarchy** for Likert scales:
1. Question-level `likert_min`/`likert_max`/`likert_labels` (highest priority)
2. Scale-level `likert_options.min`/`max`/`labels` (if question fields are -1/empty/missing)
3. System defaults based on `points` (if scale fields are -1 or missing):
   - Binary scales (points=2): 0-1
   - Regular Likert (points≥3): 1-N

**Per-question labels** enable mixed-format scales:
```json
{
  "questions": [
    {
      "id": "Q1",
      "likert_labels": ["disagree1", "disagree2", "disagree3", "disagree4", "disagree5"]
    },
    {
      "id": "Q2",
      "likert_labels": ["never", "rarely", "sometimes", "often", "always"]
    }
  ]
}
```
This allows scales with different anchors per question, or general surveys mixing question types.

**Backward compatibility:** Existing scales without `likert_labels` continue to work using scale-level labels.

### scoring
Scoring definitions for each dimension/score:
```json
{
  "scoring": {
    "COI": {
      "method": "mean_coded",
      "items": ["grit3", "grit5", "grit7"],
      "description": "Mean consistency of interest score (1-5 scale)"
    },
    "GRIT_TOTAL": {
      "method": "mean_coded",
      "items": ["grit2", "grit3", "grit5", ...],
      "description": "Overall grit score"
    }
  }
}
```

**Scoring methods**: mean_coded, sum_coded, weighted_sum, sum_correct

### report
Report configuration:
```json
{
  "report": {
    "template": "standard",
    "include": ["timestamp", "completion_time", "dimension_scores"],
    "header": "Brief description of the scale",
    "footer_refs": [
      "Citation 1",
      "See <a href='url'>link</a>"
    ]
  }
}
```

### data_output
Data file configuration:
```json
{
  "data_output": {
    "individual_file": "grit-{subnum}.csv",
    "pooled_file": "grit-pooled.csv",
    "report_file": "grit-report-{subnum}.html",
    "columns": "subnum,order,time,qnum,ques,dim,valence,resp,rt",
    "pooled_columns": "subnum,timestamp,time,grit2,grit3,...,COI,POE,GRIT_TOTAL"
  }
}
```

## Translation File Format

Format: `{code}.pbl-{lang}.json` (e.g., `grit.pbl-en.json`)

```json
{
  "LANGUAGE": "en",

  "question_head": "Instructions for the entire scale...",

  "likert_1": "Not like me at all",
  "likert_2": "Not much like me",
  "likert_3": "Somewhat like me",
  "likert_4": "Mostly like me",
  "likert_5": "Very much like me",

  "inst1": "<b>Scale Title</b><br><br>Instructions...",

  "grit2": "I have overcome setbacks to conquer an important challenge.",
  "grit3": "New ideas and projects sometimes distract me from previous ones.",

  "debrief": "<b>Thank you!</b><br><br>Your responses have been recorded."
}
```

## Likert Min/Max Use Cases

The min/max feature enables flexible response ranges for different scale types.

### Example 1: TRUE/FALSE Scale (Binary)
For scales like MOCI where TRUE=1 (endorsement), FALSE=0 (no endorsement):
```json
{
  "likert_options": {
    "points": 2,
    "min": 0,
    "max": 1,
    "labels": ["true", "false"]
  }
}
```
**Reverse coding**: `reversed = (0+1) - response = 1 - response`
- TRUE (1) becomes FALSE (0)
- FALSE (0) becomes TRUE (1)

### Example 2: Bipolar Scale (-10 to +10)
For attitude scales with negative to positive range:
```json
{
  "likert_options": {
    "points": 21,
    "min": -10,
    "max": 10,
    "question_head": "rate_feeling"
  }
}
```
**Reverse coding**: `reversed = (-10+10) - response = 0 - response`
- +10 becomes -10
- 0 stays 0
- -10 becomes +10

### Example 3: Temperature Scale (50-300)
For specialized scales with arbitrary ranges:
```json
{
  "likert_options": {
    "points": 6,
    "min": 50,
    "max": 300
  }
}
```
Values displayed: 50, 100, 150, 200, 250, 300

**Reverse coding**: `reversed = (50+300) - response = 350 - response`
- 50 becomes 300
- 300 becomes 50

### Example 4: Mixed Ranges (Question-level Override)
For scales where most questions use standard 1-5, but some use custom ranges:
```json
{
  "likert_options": {
    "points": 5,
    "min": 1,
    "max": 5
  },
  "questions": [
    {
      "id": "q1",
      "type": "likert",
      "coding": 1
    },
    {
      "id": "q2",
      "type": "likert",
      "coding": -1,
      "likert_min": 0,
      "likert_max": 100
    }
  ]
}
```
- Question q1 uses scale default: 1-5
- Question q2 overrides with custom range: 0-100

### Reverse Coding Formula
The universal formula `reversed = (min + max) - response` works for any range:
- Binary (0,1): `(0+1) - response = 1 - response`
- Standard Likert (1,5): `(1+5) - response = 6 - response`
- Bipolar (-10,10): `(-10+10) - response = 0 - response`
- Custom (50,300): `(50+300) - response = 350 - response`

## Scale Builder Strategy

When creating a scale in the GUI:
1. User enters actual question text in the UI
2. Builder auto-generates `text_key` from question ID
3. Save definition with `text_key` references
4. Save translation with actual text mapped to keys
5. Both files work seamlessly with ScaleRunner
