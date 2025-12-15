# Response Mode Translations

This directory contains language-specific translation overrides for response mode labels and footer instructions.

## Architecture: Base + Override Pattern

The response mode translation system uses a **base + override** pattern:

1. **Base file** (`../response-modes.json`): Contains complete mode definitions with English labels and instructions
2. **Override files** (`response-modes-{lang}.json`): Contain only translated strings for each language

### How It Works

When PEBL loads response modes:

1. Always loads base `response-modes.json` with complete structure (type, keys, buttons, semantic, platforms, etc.)
2. Uses 3-tier language fallback to find override file:
   - Tier 1: Explicit language if provided (e.g., `gLanguage = "es"`)
   - Tier 2: System locale if no explicit language (e.g., `es_ES` → `es`)
   - Tier 3: Falls back to English (base file only, no override)
3. Merges translated `labels` and `footerInstruction` into base modes

### Override File Format

Override files contain only translated strings:

```json
{
  "modes": {
    "spacebar": {
      "labels": ["BARRA ESPACIADORA"],
      "footerInstruction": "Presione la barra espaciadora para responder"
    },

    "leftclick": {
      "labels": ["CLIC-IZQ"],
      "footerInstruction": "Haga clic en cualquier lugar para responder"
    }
  }
}
```

**Important**: Override files do NOT contain `type`, `keys`, `buttons`, `semantic`, or `platforms` fields. These are preserved from the base file.

## Supported Languages (23 total)

- ar: Arabic
- cs: Czech
- da: Danish
- de: German
- es: Spanish
- fi: Finnish
- fr: French
- he: Hebrew
- hr: Croatian
- hu: Hungarian
- it: Italian
- ja: Japanese
- ko: Korean
- nl: Dutch
- no: Norwegian
- pl: Polish
- pt: Portuguese
- ro: Romanian
- ru: Russian
- sv: Swedish
- tr: Turkish
- uk: Ukrainian
- zh: Chinese

## Adding New Languages

To add a new language:

1. Create `response-modes-{lang}.json` (use ISO 639-1 two-letter code)
2. Translate only the `labels` and `footerInstruction` fields
3. Include translations for modes used in your battery tasks
4. Do NOT duplicate structural fields from base file

Example minimal override for French:

```json
{
  "modes": {
    "spacebar": {
      "labels": ["BARRE D'ESPACE"],
      "footerInstruction": "Appuyez sur la barre d'espace pour répondre"
    }
  }
}
```

## Implementation

See `pebl-lib/Layout.pbl`:
- `LoadResponseModes(lang)` - Loads base + override with 3-tier fallback
- `MergeResponseModeTranslations(baseModes, overrideModes)` - Merges translations into base
