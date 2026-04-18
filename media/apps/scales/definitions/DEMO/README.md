# OpenScales Feature Demonstration

**Abbreviation:** DEMO  
**Code:** `DEMO`  
**Version:** 1.1  
**License:** Public domain  

Comprehensive demonstration scale exercising all major OpenScales/ScaleRunner features: (1) all question types — likert, multi, multicheck, short, long, vas, grid, inst; (2) multiple sections; (3) section-order randomization with fixed sections (randomize_sections); (4) section-level item shuffle via randomize.method='shuffle' — items pinned via random_group=0 or the fixed ID list; (5) scale-level shuffle via random_group + shuffle_questions parameter — items with random_group=1 shuffle independently per section, random_group=0 items stay fixed;(6) dependent sections — visible_when on section markers keyed to a response; (7) dependent sections — visible_when keyed to a parameter; (8) dependent items — visible_when on item answers, including 'in' operator; (9) required vs. optional items; (10) input validation on short answers; (11) per-item Likert scale override; (12) forward and reverse scoring across two dimensions; (13) text substitution via {study_name} parameter; (14) parameter-driven branch routing selecting one of two context-specific question sets; (15) selectable dimensions — researcher can disable entire scoring subscales via boolean parameters. Use this scale to verify all engine features work correctly after changes.

## Scale Summary

- **Items:** 0
- **Dimensions:** 2
  - **Attitudes Toward Technology (ATT)**
  - **Subjective Well-being (WB)**

## Scoring

- **attitudes**: mean_coded (9 items)
  - Mean of attitude items after reverse-scoring att3 and att5. Branch items (context_general_q* or context_workplace_q*) contribute only one pair depending on parameter; NA responses are excluded from the mean.
- **wellbeing**: mean_coded (3 items)
  - Mean of 3 VAS items. Range 0-100. Higher = greater subjective well-being.

## Citation

> OpenScales internal demonstration scale — not for research use.

## Files

- `DEMO.osd` - Scale definition (OpenScales OSD format)

## Usage

This scale is designed to be run using the PEBL ScaleRunner system.
See the [PEBL documentation](https://pebl.sf.net) for details.
