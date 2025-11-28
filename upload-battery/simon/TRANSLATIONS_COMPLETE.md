# Simon Task - Translation Files Complete

## Summary
Created translation files for the Simon Task in 7 languages (including the existing English and Chinese).

## Available Languages

| Language   | Code | File                    | Status |
|------------|------|-------------------------|--------|
| English    | en   | simon.pbl-en.json       | ✓ Existing |
| Chinese    | cn   | simon.pbl-cn.json       | ✓ Existing |
| German     | de   | simon.pbl-de.json       | ✓ Created |
| Spanish    | es   | simon.pbl-es.json       | ✓ Created |
| French     | fr   | simon.pbl-fr.json       | ✓ Created |
| Italian    | it   | simon.pbl-it.json       | ✓ Created |
| Dutch      | nl   | simon.pbl-nl.json       | ✓ Created |
| Portuguese | pt   | simon.pbl-pt.json       | ✓ Created |

## Translation Keys

All translation files contain these 5 keys:

1. **INST1**: Main task instructions explaining the red/blue circle task and key mappings
2. **FOOTER1**: Footer text showing "left shift" and "right shift" labels
3. **BREAK**: Break message between blocks
4. **DEBRIEF**: Thank you message at the end

## File Locations

**Development:**
- `/home/smueller/Dropbox/Research/pebl/pebl/upload-battery/simon/translations/`

**Deployed:**
- `/home/smueller/Dropbox/Research/pebl/PEBLOnlinePlatform/battery/simon/translations/` (for browse.php)
- `/home/smueller/Dropbox/Research/pebl/PEBLOnlinePlatform/runtime/test-bundles/simon.data` (bundle includes translations)

## Bundle Update

The simon test bundle has been rebuilt and deployed with the new translations:
- **Old size**: 8.7K
- **New size**: 14K (increased due to 6 additional translation files)
- **Deployed**: November 27, 2024

## Usage

To run the Simon task in a specific language, use the `language` URL parameter:

```
# German
?test=simon&mode=demo&language=de

# Spanish
?test=simon&mode=demo&language=es

# French
?test=simon&mode=demo&language=fr

# Italian
?test=simon&mode=demo&language=it

# Dutch
?test=simon&mode=demo&language=nl

# Portuguese
?test=simon&mode=demo&language=pt

# Chinese
?test=simon&mode=demo&language=cn
```

## Testing

To test each translation:
1. Navigate to: `http://localhost/runtime/public-launcher.html?test=simon&mode=demo&scriptFile=simon.pbl&language=XX`
   (Replace XX with language code: de, es, fr, it, nl, pt, cn)
2. Verify instructions appear in the correct language
3. Check that key labels in footer are translated
4. Complete a few trials and verify break/debrief messages

## Translation Quality

All translations follow these principles:
- Natural, fluent language appropriate for the target audience
- Preserve technical accuracy (shift key names, color names)
- Maintain consistent formatting (\n for line breaks)
- Equivalent instruction clarity to English original
- Culturally appropriate phrasing for experimental contexts

## Notes

- Footer spacing may need adjustment for languages with longer key names
- All files use UTF-8 encoding to support special characters (ñ, é, ü, etc.)
- Translation files are loaded automatically by PEBL's GetStrings() function based on gLanguage parameter
