# Font Bundling Strategy for PEBL Web Platform

## Current Situation

**Main bundle (pebl2.data)**: 13MB total
- Includes only 3 DejaVu fonts (1.3MB total):
  - DejaVuSans.ttf (740KB)
  - DejaVuSansMono.ttf (333KB)
  - DejaVuSerif.ttf (372KB)

**Available fonts in media/fonts/**: 18MB total
- DejaVu fonts: ~2.5MB (Sans, SansMono, Serif + Bold variants)
- Noto fonts: ~15.5MB (international language support)
  - NotoSansCJK-Regular.ttc: **18.5MB** (Chinese/Japanese/Korean)
  - Other Noto fonts: ~2.7MB (Thai, Devanagari, Bengali, Arabic, Hebrew, Georgian)
- Specialty fonts: ~30KB (Stimulasia, Humanistic, Optician-Sans)

## Font Requirements by Language

PEBL sets `gPEBLBaseFont`, `gPEBLBaseFontMono`, and `gPEBLBaseFontSerif` based on language:

| Language | Base Font | Mono Font | Serif Font | Size |
|----------|-----------|-----------|------------|------|
| **EN (default)** | DejaVuSans.ttf | DejaVuSansMono.ttf | DejaVuSerif.ttf | 1.4MB |
| **AR (Arabic)** | DejaVuSans.ttf | DejaVuSansMono.ttf | DejaVuSerif.ttf | 1.4MB |
| **HE (Hebrew)** | DejaVuSans.ttf | DejaVuSansMono.ttf | DejaVuSerif.ttf | 1.4MB |
| **TH (Thai)** | NotoSansThai-Regular.ttf | NotoSansMono-Regular.ttf | NotoSerif-Regular.ttf | 1.1MB |
| **HI/MR/NE (Devanagari)** | NotoSansDevanagari-Regular.ttf | NotoSansMono-Regular.ttf | NotoSerif-Regular.ttf | 1.3MB |
| **BN (Bengali)** | NotoSansBengali-Regular.ttf | NotoSansMono-Regular.ttf | NotoSerif-Regular.ttf | 1.3MB |
| **KA (Georgian)** | DejaVuSans.ttf | DejaVuSansMono.ttf | DejaVuSerif.ttf | 1.4MB |
| **ZH/JA/KO (CJK)** | NotoSansCJK-Regular.ttc | NotoSansMono-Regular.ttf | NotoSerif-Regular.ttf | **19.6MB** |

**Fallback fonts** (always needed): DejaVuSans.ttf, DejaVuSansMono.ttf, DejaVuSerif.ttf (1.4MB)

## Proposed Bundle Strategy

### Bundle 1: `pebl2` (default - now includes specialty fonts)
**Size**: 13MB (1.33MB fonts)
**Contents**: Core PEBL + DejaVu fonts + specialty fonts
- DejaVuSans.ttf
- DejaVuSansMono.ttf
- DejaVuSerif.ttf
- Stimulasia.ttf (2.6KB) - Eye chart/vision test font
- Humanistic.ttf (17KB) - Handwriting font
- Optician-Sans.otf (11KB) - Vision test font

**Languages supported**: EN, AR, HE, KA (default Western/Latin/Cyrillic/Greek)

**Rationale**: Keep the main bundle self-contained. Specialty fonts are not language-specific and should always be available for vision/cognitive tests. The 30KB addition is negligible.

### Bundle 2: `fonts_noto` (optional - international support)
**Size**: ~2.7MB
**Contents**: Noto fonts for non-CJK languages
- NotoSansThai-Regular.ttf (37KB)
- NotoSansDevanagari-Regular.ttf (224KB)
- NotoSansBengali-Regular.ttf (198KB)
- NotoSansArabic-Regular.ttf (239KB)
- NotoSansHebrew-Regular.ttf (27KB)
- NotoSansGeorgian-Regular.ttf (52KB)
- NotoSans-Regular.ttf (501KB)
- NotoSans-Bold.ttf (504KB)
- NotoSansMono-Regular.ttf (499KB)
- NotoSansMono-Bold.ttf (512KB)
- NotoSerif-Regular.ttf (576KB)
- NotoSerif-Bold.ttf (598KB)

**Languages supported**: TH, HI, MR, NE, BN, AR, HE, KA (with better font support)

**Rationale**: Only load when test requires international language support

### Bundle 3: `fonts_cjk` (optional - CJK support only)
**Size**: ~19.6MB
**Contents**: CJK font + Noto Mono/Serif
- NotoSansCJK-Regular.ttc (18.5MB)
- NotoSansMono-Regular.ttf (499KB)
- NotoSerif-Regular.ttf (576KB)

**Languages supported**: ZH, CN, TW, JA, JP, KO, KR, KP

**Rationale**: CJK font is huge - only load when explicitly needed

## Bundle Loading Rules

### For Single-Test Launchers (pebl-launcher.html, test-launcher.html)

`/api/getBundles.php` determines bundles based on:

1. **Language parameter**:
   - `?language=en` → `['pebl2']` (default)
   - `?language=th` → `['pebl2', 'fonts_noto']`
   - `?language=zh` → `['pebl2', 'fonts_cjk']`

2. **Test metadata** in `test_catalog.json` (if additional bundles needed):
   ```json
   {
     "test_id": "some_test",
     "required_bundles": ["fonts_noto"]
   }
   ```

   Note: Specialty fonts are now in the core `pebl2` bundle, so tests no longer need to request them separately.

### For Chain Launchers (chain-launcher.html, chain-test-launcher.html)

Chain configuration specifies all bundles upfront:
```json
{
  "chain_id": "international_study",
  "required_bundles": ["pebl2", "fonts_noto"],
  "items": [...]
}
```

If chain includes CJK tests:
```json
{
  "required_bundles": ["pebl2", "fonts_noto", "fonts_cjk"]
}
```

## Implementation Steps

1. ✅ Update launchers with unified bundle loading (DONE)
2. ✅ Create bundle generation scripts (`scripts/generate-font-bundles.sh`)
3. ✅ Add specialty fonts to emscripten bundle (`emscripten/media/fonts/`)
4. ⏳ Rebuild pebl2.data with specialty fonts (`make fp`)
5. ✅ Update `/api/getBundles.php` to return font bundles based on language
6. ⏳ Generate production font bundles (`./scripts/generate-font-bundles.sh prod`)
7. ⏳ Test with various language parameters

## Expected Performance Impact

### Typical English test (current):
- Download: 13MB (pebl2.data)
- Fonts included: 3 DejaVu fonts (1.3MB)

### Typical English test (after bundling):
- Download: 13MB (pebl2.data)
- Fonts included: 3 DejaVu fonts (1.3MB)
- **No change** ✅

### Thai language test (current):
- Download: 13MB (pebl2.data)
- Fonts available: ❌ MISSING - would fail or use fallback
- User experience: Broken Thai text rendering

### Thai language test (after bundling):
- Download: 13MB (pebl2) + 2.7MB (fonts_noto) = **15.7MB**
- Fonts included: DejaVu + all Noto fonts
- User experience: ✅ Proper Thai text rendering
- **Impact**: +2.7MB for international support

### Chinese/Japanese/Korean test (current):
- Download: 13MB (pebl2.data)
- Fonts available: ❌ MISSING - would fail catastrophically
- User experience: Can't display CJK characters

### Chinese/Japanese/Korean test (after bundling):
- Download: 13MB (pebl2) + 19.6MB (fonts_cjk) = **32.6MB**
- Fonts included: CJK font + necessary variants
- User experience: ✅ Proper CJK text rendering
- **Impact**: +19.6MB for CJK support (necessary)

## Benefits

✅ **Faster default loading**: Most tests stay at 13MB (no change)
✅ **International support**: Can properly support non-Latin scripts
✅ **Selective loading**: Only download fonts when needed
✅ **Future-proof**: Easy to add new font bundles (emoji, math symbols, etc.)
✅ **Consistent architecture**: Matches existing bundle pattern (private_uploads)

## Deferred: pebl2.data Font Reduction

**Current state**: pebl2.data includes 1.3MB of DejaVu fonts
**Opportunity**: Could move DejaVu fonts to fonts_default bundle, save 1.3MB from pebl2.data

**Decision**: DEFER this optimization
- Would require all launchers to always load fonts_default bundle
- Adds complexity to ensure fonts_default loads before pebl2
- Benefit is minimal (1.3MB savings on 13MB bundle = 10%)
- Better to keep pebl2.data self-contained and working standalone

**Recommendation**: Keep DejaVu fonts in pebl2.data for now, revisit if bundle size becomes critical

## File Locations

**Source fonts**: `media/fonts/`
**Bundle scripts**: `scripts/generate-font-bundles.sh`
**Generated bundles**:
- Production: `PEBLOnlinePlatform/runtime/fonts_*.js` + `fonts_*.data`
- Development: `bin/fonts_*.js` + `fonts_*.data`

**Mount point in PEBL filesystem**: `/usr/local/share/pebl2/media/fonts/` (same as current)
