# Font Bundle Strategy for PEBL Emscripten Build

## Current Situation

### Font Inventory

The `media/fonts/` directory contains **~18MB** of fonts:

**Latin/Basic fonts (~5.5MB):**
- DejaVu fonts (Sans, SansMono, Serif variants) - 5 files, ~2.5MB
- Noto Sans/Serif (Regular, Bold, Mono) - 5 files, ~3MB
- Humanistic.ttf - 17KB
- Optician-Sans.otf - 11KB
- Stimulasia.ttf - 2.6KB

**Non-Latin script fonts (~650KB):**
- NotoSansArabic-Regular.ttf - 239KB (Arabic)
- NotoSansBengali-Regular.ttf - 198KB (Bengali/Bangla)
- NotoSansDevanagari-Regular.ttf - 224KB (Hindi, Marathi, Nepali)
- NotoSansGeorgian-Regular.ttf - 52KB (Georgian)
- NotoSansHebrew-Regular.ttf - 27KB (Hebrew)
- NotoSansThai-Regular.ttf - 37KB (Thai)

**CJK font (~19MB) - **THE BIG ONE:**
- NotoSansCJK-Regular.ttc - 19MB (Chinese, Japanese, Korean)
  - This single file is larger than all other fonts combined!

**Total: ~25MB** (but 19MB is the CJK font alone)

## Language Detection System

PEBL has a **three-tier language selection system** already implemented in `pebl-lib/Utility.pbl`:

### Tier 1: Explicit `gLanguage` Variable
```pebl
if(gLanguage != "")
{
  match <- Lookup(gLanguage, languages, translations)
}
```

### Tier 2: System Locale Detection
```pebl
if(IsList(match))  ## No match from gLanguage
{
  systemLocale <- GetSystemLocale()
  systemLang <- Uppercase(SubString(systemLocale, 1, 2))
  match <- Lookup(systemLang, languages, translations)
}
```

### Tier 3: English Fallback
Falls back to English if neither works.

### Font Selection Functions

PEBL also has automatic font selection based on detected script:
- `DetectTextScript(text)` - Detects script type
- `GetFontForText(text)` - Returns appropriate font name
- `IsRTL(text)` - Checks if text is right-to-left

These are C++ built-in functions in `src/libs/PEBLString.cpp`.

## Proposed Font Bundle Strategy

### Bundle Structure

Create **four separate font bundles** with lazy loading:

#### 1. **Core Latin Bundle** (mandatory, ~5.5MB)
Always loaded with main runtime.

**Contents:**
```json
{
  "name": "fonts-latin",
  "type": "font_bundle",
  "description": "Core Latin fonts (DejaVu, Noto Sans/Serif)",
  "output_dir": "bin/font-bundles",
  "includes": [
    {
      "path": "media/fonts/DejaVuSans.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/DejaVuSans.ttf"
    },
    {
      "path": "media/fonts/DejaVuSans-Bold.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/DejaVuSans-Bold.ttf"
    },
    {
      "path": "media/fonts/DejaVuSansMono.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/DejaVuSansMono.ttf"
    },
    {
      "path": "media/fonts/DejaVuSerif.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/DejaVuSerif.ttf"
    },
    {
      "path": "media/fonts/DejaVuSerif-Bold.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/DejaVuSerif-Bold.ttf"
    },
    {
      "path": "media/fonts/NotoSans-Regular.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSans-Regular.ttf"
    },
    {
      "path": "media/fonts/NotoSans-Bold.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSans-Bold.ttf"
    },
    {
      "path": "media/fonts/NotoSerif-Regular.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSerif-Regular.ttf"
    },
    {
      "path": "media/fonts/NotoSerif-Bold.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSerif-Bold.ttf"
    },
    {
      "path": "media/fonts/Humanistic.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/Humanistic.ttf"
    },
    {
      "path": "media/fonts/Optician-Sans.otf",
      "mount": "/usr/local/share/pebl2/media/fonts/Optician-Sans.otf"
    },
    {
      "path": "media/fonts/Stimulasia.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/Stimulasia.ttf"
    }
  ]
}
```

#### 2. **Non-Latin Scripts Bundle** (~650KB)
Load when `GetSystemLocale()` returns ar, bn, hi, ka, he, or th.

**Contents:**
```json
{
  "name": "fonts-nonlatin",
  "type": "font_bundle",
  "description": "Non-Latin script fonts (Arabic, Bengali, Devanagari, Georgian, Hebrew, Thai)",
  "output_dir": "bin/font-bundles",
  "includes": [
    {
      "path": "media/fonts/NotoSansArabic-Regular.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSansArabic-Regular.ttf"
    },
    {
      "path": "media/fonts/NotoSansBengali-Regular.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSansBengali-Regular.ttf"
    },
    {
      "path": "media/fonts/NotoSansDevanagari-Regular.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSansDevanagari-Regular.ttf"
    },
    {
      "path": "media/fonts/NotoSansGeorgian-Regular.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSansGeorgian-Regular.ttf"
    },
    {
      "path": "media/fonts/NotoSansHebrew-Regular.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSansHebrew-Regular.ttf"
    },
    {
      "path": "media/fonts/NotoSansThai-Regular.ttf",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSansThai-Regular.ttf"
    }
  ]
}
```

**Language codes:**
- ar = Arabic
- bn = Bengali/Bangla
- hi = Hindi (Devanagari)
- ka = Georgian
- he = Hebrew
- th = Thai

#### 3. **CJK Font Bundle** (~19MB) - CRITICAL FOR SIZE
Load when `GetSystemLocale()` returns zh, ja, or ko.

**Contents:**
```json
{
  "name": "fonts-cjk",
  "type": "font_bundle",
  "description": "CJK (Chinese, Japanese, Korean) font - 19MB",
  "output_dir": "bin/font-bundles",
  "includes": [
    {
      "path": "media/fonts/NotoSansCJK-Regular.ttc",
      "mount": "/usr/local/share/pebl2/media/fonts/NotoSansCJK-Regular.ttc"
    }
  ],
  "size_mb": 19,
  "catalog_entry": {
    "base_url": "/runtime/font-bundles",
    "languages": ["zh", "ja", "ko"]
  }
}
```

**Language codes:**
- zh = Chinese
- ja = Japanese
- ko = Korean

#### 4. **DejaVu Full Collection** (optional, ~12MB)
All DejaVu variants - load only if explicitly requested.

**Contents:**
All files in `media/fonts/dejavu-fonts-ttf-2.37/ttf/`

## Implementation Approach

### Option A: Platform-Level Detection with User Override (Recommended)

The **PEBLOnlinePlatform launchers** can handle font bundle loading intelligently with **three-tier language detection**:

1. **User-selected language** (URL parameter or UI selection)
2. **Browser/system locale** (automatic detection)
3. **English fallback**

**In PEBLOnlinePlatform launcher JavaScript:**

```javascript
// Three-tier language detection
function getLanguageCode() {
  // Tier 1: Check URL parameter (user/researcher override)
  const urlParams = new URLSearchParams(window.location.search);
  const urlLang = urlParams.get('lang') || urlParams.get('language');

  if (urlLang) {
    console.log('Using language from URL parameter:', urlLang);
    return urlLang.substring(0, 2).toLowerCase();
  }

  // Tier 2: Check browser/system locale (automatic)
  const systemLocale = navigator.language || navigator.userLanguage; // e.g., "zh-CN", "ja-JP"
  if (systemLocale) {
    const lang = systemLocale.substring(0, 2).toLowerCase();
    console.log('Using system locale:', lang);
    return lang;
  }

  // Tier 3: Fall back to English
  console.log('Falling back to English');
  return 'en';
}

const lang = getLanguageCode();

// Determine which font bundles to load
const fontBundles = [];

// Always load Latin bundle (included in core)
// fontBundles.push('fonts-latin');  // Already in core bundle

// Load CJK if needed (BIG SAVINGS if not needed!)
if(['zh', 'ja', 'ko'].includes(lang)) {
    fontBundles.push('fonts-cjk');
}

// Load non-Latin scripts if needed
if(['ar', 'bn', 'hi', 'ka', 'he', 'th'].includes(lang)) {
    fontBundles.push('fonts-nonlatin');
}

// Load bundles before starting PEBL
await Promise.all(fontBundles.map(loadBundle));

// Pass language to PEBL via command-line argument
const peblArgs = ['-v', `language=${lang}`];
instance.callMain(peblArgs);
```

### URL Parameter Method (Researcher Control)

**Currently Implementable:** Researchers can specify language via URL parameter:

```
https://peblhub.online/launcher.html?token=ABC123&lang=zh
https://peblhub.online/launcher.html?token=ABC123&language=ja
https://peblhub.online/launcher.html?token=ABC123&lang=ar
```

Platform launcher extracts and uses the `lang` parameter before loading font bundles.

**Implementation in launcher:**
```javascript
// Extract language from URL (researcher/platform specified)
const urlParams = new URLSearchParams(window.location.search);
const urlLang = urlParams.get('lang') || urlParams.get('language');

if (urlLang) {
  // Researcher explicitly set language - use it
  lang = urlLang.substring(0, 2).toLowerCase();
} else {
  // No explicit language - auto-detect from browser
  const systemLocale = navigator.language || navigator.userLanguage;
  lang = systemLocale ? systemLocale.substring(0, 2).toLowerCase() : 'en';
}
```

### Platform Study Configuration (Recommended)

Add language field to study settings in PEBLOnlinePlatform:

```json
{
  "study_id": "123",
  "tests": ["stroop", "flanker"],
  "language": "zh",  // Force Chinese for all participants
  "font_bundles": ["fonts-cjk"]  // Explicitly specify required fonts
}
```

Platform generates study URLs with the configured language parameter automatically.

### Future Enhancement: Participant Language Selection UI

**Not Currently Implemented** - Requires significant additional work:

**Challenge 1: Language Availability Detection**
- Each test has different translations available
- Test chain may mix tests with different language support
- Example: Test A supports [en, es, zh], Test B supports [en, de, fr]
  - Common languages: [en] only
  - UI would need to know this before showing selector

**Challenge 2: Experimenter Control**
- Some studies MUST use a specific language (standardization)
- Some studies CAN allow participant choice (accessibility)
- Requires study-level permission setting

**Challenge 3: Cross-Test Consistency**
- If participant selects Chinese for Test 1, should Test 2 auto-select Chinese?
- What if Test 2 doesn't support Chinese?
- Requires complex state management across test chain

**Proposed Future Implementation:**

1. **Scan test chain for available languages:**
   ```javascript
   async function getAvailableLanguages(testChain) {
     const languages = [];
     for (const test of testChain) {
       const testLangs = await fetch(`/api/getTestLanguages.php?test=${test}`);
       languages.push(testLangs);
     }
     // Return intersection of all test languages
     return languages.reduce((common, langs) =>
       common.filter(lang => langs.includes(lang))
     );
   }
   ```

2. **Check experimenter permission:**
   ```json
   {
     "study_id": "123",
     "allow_participant_language_selection": true,  // Experimenter control
     "default_language": "en",
     "available_languages": ["en", "es", "zh"]  // Or "auto" to scan tests
   }
   ```

3. **Show UI only if allowed AND languages available:**
   ```javascript
   if (studyConfig.allow_participant_language_selection &&
       availableLanguages.length > 1) {
     showLanguageSelector(availableLanguages);
   }
   ```

**For now:** Font bundle system is designed to support future UI selection, but only implements:
- URL parameter (researcher control)
- Browser auto-detect (automatic)
- Study configuration (platform control)

### Option B: PEBL Runtime Detection

Alternatively, PEBL can detect and request fonts at runtime:

**In `pebl-lib/Utility.pbl` or launcher:**

```pebl
define LoadRequiredFonts()
{
  systemLocale <- GetSystemLocale()
  lang <- Lowercase(SubString(systemLocale, 1, 2))

  ## CJK languages need big font
  if(IsMember(lang, ["zh", "ja", "ko"]))
  {
    LoadFontBundle("fonts-cjk")
  }

  ## Non-Latin scripts
  if(IsMember(lang, ["ar", "bn", "hi", "ka", "he", "th"]))
  {
    LoadFontBundle("fonts-nonlatin")
  }
}
```

## Benefits

### 1. **Massive Size Reduction for Most Users**
- **Before:** Every user downloads 25MB of fonts
- **After:**
  - English/European users: 5.5MB (saves 19.5MB)
  - CJK users: 24.5MB (saves 0.5MB)
  - Non-Latin users: 6MB (saves 19MB)

### 2. **Faster Initial Load**
- 78% reduction for non-CJK users
- Parallel loading of required font bundles
- Browser caching works per-bundle

### 3. **Automatic Language Support**
- No manual configuration needed
- Uses existing `GetSystemLocale()` detection
- Falls back gracefully to Latin fonts

### 4. **Future Extensibility**
- Easy to add new language bundles
- Can add region-specific variants (zh-CN vs zh-TW)
- Optional bundles for specialized fonts

## Implementation Steps

1. **Update `bundle-config.json`** with font bundle definitions
2. **Modify core bundle** to exclude fonts (already separated)
3. **Update platform launchers** to detect locale and load appropriate font bundles
4. **Create font catalog** mapping language codes to bundles
5. **Test** with different browser locales

## Platform Integration

### test_catalog.json Addition

```json
{
  "font_bundles": {
    "fonts-cjk": {
      "file": "fonts-cjk.data",
      "url": "/runtime/font-bundles/fonts-cjk.data",
      "base_url": "/runtime/font-bundles",
      "size_mb": 19,
      "description": "CJK (Chinese/Japanese/Korean) fonts",
      "languages": ["zh", "ja", "ko"]
    },
    "fonts-nonlatin": {
      "file": "fonts-nonlatin.data",
      "url": "/runtime/font-bundles/fonts-nonlatin.data",
      "base_url": "/runtime/font-bundles",
      "size_mb": 0.65,
      "description": "Non-Latin script fonts",
      "languages": ["ar", "bn", "hi", "ka", "he", "th"]
    }
  }
}
```

### API Endpoint

```php
// /api/getFontBundles.php
// Returns font bundles needed for given language code
$lang = $_GET['lang'] ?? '';
$bundles = determineFontBundles($lang);
echo json_encode($bundles);
```

## Fallback Strategy

If font bundle loading fails:
1. PEBL continues with Latin fonts only
2. Non-Latin text renders as boxes/fallback glyphs
3. User experience degrades gracefully
4. Console warning logged for debugging

## Language Selection Priority (Current Implementation)

The font bundle system supports **three methods** of language selection (in priority order):

### 1. URL Parameter (Highest Priority - Researcher Control)
   - `?lang=zh` or `?language=ja`
   - Researcher/platform specifies in study URL
   - **Overrides browser locale**
   - **Use case:** Standardized studies requiring specific language

### 2. Browser/System Locale (Automatic)
   - `navigator.language` detection
   - Automatic, no configuration needed
   - **Default behavior** if no URL parameter
   - **Use case:** Adaptive studies, accessibility

### 3. English Fallback (Lowest Priority)
   - If browser locale detection fails
   - Always works with Latin fonts (in core bundle)
   - **Guaranteed baseline** functionality

**Current Implementation Flow:**
```
┌─────────────────────┐
│ URL has ?lang=      │ YES ──> Use URL language parameter
│ parameter?          │         (researcher control)
└──────────┬──────────┘
           │ NO
           ▼
┌─────────────────────┐
│ Browser locale      │ YES ──> Use navigator.language
│ detected?           │         (automatic detection)
└──────────┬──────────┘
           │ NO
           ▼
┌─────────────────────┐
│ Fall back to        │
│ English             │
└─────────────────────┘
```

### Future: Participant UI Selection (Not Yet Implemented)

A **fourth tier** could be added in the future for participant language selection:

**Challenges requiring resolution:**
1. **Language availability varies per test** - Need to detect common languages across test chain
2. **Experimenter control needed** - Some studies must force a language for standardization
3. **Cross-test consistency** - State management when switching between tests
4. **UI complexity** - Only show available languages, handle errors gracefully

**When implemented, priority would be:**
1. Participant UI selection (new highest priority)
2. URL parameter (researcher override)
3. Browser locale (automatic)
4. English fallback

**Font bundle system is architected to support this future enhancement** - the bundle loading mechanism doesn't care how the language was determined, only what language code it receives.

## Testing Checklist

**Automatic Detection:**
- [ ] English locale (en-US) - Latin fonts only
- [ ] Chinese locale (zh-CN) - Latin + CJK
- [ ] Japanese locale (ja-JP) - Latin + CJK
- [ ] Korean locale (ko-KR) - Latin + CJK
- [ ] Arabic locale (ar-SA) - Latin + Non-Latin
- [ ] Hindi locale (hi-IN) - Latin + Non-Latin
- [ ] Hebrew locale (he-IL) - Latin + Non-Latin

**URL Parameter Control:**
- [ ] URL parameter `?lang=zh` (Chinese)
- [ ] URL parameter `?language=ja` (Japanese)
- [ ] URL parameter `?lang=ar` (Arabic)
- [ ] URL parameter overrides browser locale correctly
- [ ] Platform study configuration generates correct URLs

**Edge Cases:**
- [ ] Mixed language tests
- [ ] Language switch mid-session
- [ ] Bundle loading failure scenarios
- [ ] Browser cache behavior
- [ ] Slow network (font bundle timeout)

## Performance Impact

**Network transfer savings:**
- English users: Save 19.5MB download
- Download time at 10 Mbps: ~15 seconds saved
- Mobile data cost: ~$0.20-0.40 saved per user (at typical mobile data rates)

**With 1000 test takers:**
- 80% English speakers: 800 users × 19.5MB = 15.6 GB saved
- 20% CJK speakers: 200 users × 0 saved
- **Total bandwidth saved: 15.6 GB**

## Recommendations

### Immediate Action:
1. **Separate CJK font** into its own bundle - this alone saves 78% for non-CJK users
2. **Include Latin fonts** in core bundle (already done in current setup)
3. **Implement platform-level locale detection** in launchers

### Future Enhancements:
1. Separate non-Latin scripts into individual bundles for even finer control
2. Add font subset generation for CJK (could reduce from 19MB to 5-10MB with common characters only)
3. Implement font preloading hints for known language preferences
4. Add user preference override for font bundles

## Alternative: Font Subsetting

For even better optimization, consider **CJK font subsetting**:

Tools like `fonttools` can extract only commonly-used characters:
- Full CJK: 19MB (50,000+ glyphs)
- Common subset: 5-8MB (3,000-5,000 most common characters)
- Basic subset: 2-3MB (1,000 most common characters)

This requires additional build tooling but could provide 70-85% reduction in CJK bundle size.

## Test-Specific Font Requirements

### The Problem

Some tests **hard-code specific fonts** in their `.pbl` files, independent of user locale:

**Examples:**
- Visual pattern learning with Chinese characters
- Stroop test with Arabic text
- Memory test using specific non-Latin script
- Cross-cultural studies requiring multiple scripts

**The challenge:**
A test using `MakeFont("NotoSansCJK-Regular", ...)` needs that font loaded **regardless** of the participant's browser locale. Automatic locale-based loading won't work.

### Solution: Test-Level Font Requirements in test_catalog.json

Add **per-test font bundle declarations** to the test catalog:

```json
{
  "tests": {
    "stroop": {
      "id": "stroop",
      "name": "Stroop Test",
      "directory": "stroop-vic",
      "main_file": "stroop-vic.pbl",
      "collection": "basic_cognitive",
      "data_bundle": "core"
      // No required_fonts - uses locale-based defaults
    },

    "chinese_character_learning": {
      "id": "chinese_learning",
      "name": "Chinese Character Learning",
      "directory": "chinese-learning",
      "main_file": "chinese-learning.pbl",
      "collection": "custom",
      "data_bundle": "core",
      "required_fonts": ["fonts-cjk"]  // OVERRIDE: Always load CJK fonts
    },

    "multilingual_stroop": {
      "id": "multilingual_stroop",
      "name": "Multilingual Stroop",
      "directory": "stroop-multilingual",
      "main_file": "stroop-multi.pbl",
      "collection": "advanced",
      "data_bundle": "core",
      "required_fonts": ["fonts-cjk", "fonts-nonlatin"]  // Multiple bundles
    },

    "arabic_memory": {
      "id": "arabic_memory",
      "name": "Arabic Memory Test",
      "directory": "arabic-memory",
      "main_file": "arabic-memory.pbl",
      "collection": "custom",
      "data_bundle": "core",
      "required_fonts": ["fonts-nonlatin"]  // Override for Arabic
    }
  }
}
```

### Loading Logic (Three-Tier Priority)

The platform launcher implements a **priority-based loading system**:

#### Tier 1: Test-Specific Requirements (Highest Priority)
```javascript
// Load test metadata
const testConfig = await fetch(`/api/getTestConfig.php?test=${testId}`);
const requiredFonts = testConfig.required_fonts || [];

// If test specifies fonts, use ONLY those
if (requiredFonts.length > 0) {
    fontBundlesToLoad = requiredFonts;
    console.log(`Test ${testId} requires specific fonts:`, requiredFonts);
}
```

#### Tier 2: User Locale Detection (Medium Priority)
```javascript
// If no test-specific fonts, use locale
else {
    const systemLocale = navigator.language || navigator.userLanguage;
    const lang = systemLocale.substring(0, 2).toLowerCase();

    fontBundlesToLoad = [];

    if(['zh', 'ja', 'ko'].includes(lang)) {
        fontBundlesToLoad.push('fonts-cjk');
    }

    if(['ar', 'bn', 'hi', 'ka', 'he', 'th'].includes(lang)) {
        fontBundlesToLoad.push('fonts-nonlatin');
    }

    console.log(`Using locale ${lang} fonts:`, fontBundlesToLoad);
}
```

#### Tier 3: Latin-Only Fallback (Lowest Priority)
```javascript
// If locale detection fails or returns unrecognized language
if (fontBundlesToLoad.length === 0) {
    // Latin fonts already in core bundle - nothing to load
    console.log('Using Latin fonts only (from core bundle)');
}
```

### Complete Loading Flow

```javascript
async function loadFontsForTest(testId) {
    // 1. Fetch test configuration
    const testConfig = await fetch(`/api/getTestConfig.php?test=${testId}`)
        .then(r => r.json());

    // 2. Determine which font bundles to load (priority-based)
    let fontBundles = [];

    if (testConfig.required_fonts && testConfig.required_fonts.length > 0) {
        // TIER 1: Test explicitly requires fonts
        fontBundles = testConfig.required_fonts;
        console.log(`[OVERRIDE] Test requires: ${fontBundles.join(', ')}`);
    } else {
        // TIER 2: Use browser locale
        const locale = navigator.language || navigator.userLanguage;
        const lang = locale.substring(0, 2).toLowerCase();

        if (['zh', 'ja', 'ko'].includes(lang)) {
            fontBundles.push('fonts-cjk');
        }
        if (['ar', 'bn', 'hi', 'ka', 'he', 'th'].includes(lang)) {
            fontBundles.push('fonts-nonlatin');
        }

        if (fontBundles.length > 0) {
            console.log(`[LOCALE ${lang}] Loading: ${fontBundles.join(', ')}`);
        } else {
            // TIER 3: Latin only (already in core)
            console.log('[DEFAULT] Using Latin fonts from core bundle');
        }
    }

    // 3. Load required font bundles
    if (fontBundles.length > 0) {
        await Promise.all(fontBundles.map(bundle => loadBundle(bundle)));
        console.log('Font bundles loaded successfully');
    }

    // 4. Proceed with test launch
    return true;
}
```

### API Endpoint: getTestConfig.php

```php
<?php
// /api/getTestConfig.php

require_once(__DIR__ . "/../config/config.php");

$testId = $_GET['test'] ?? '';

// Load test catalog
$catalog = json_decode(file_get_contents(CONFIG_PATH . '/test_catalog.json'), true);

// Find test config
$testConfig = null;
foreach ($catalog['tests'] as $id => $config) {
    if ($id === $testId) {
        $testConfig = $config;
        break;
    }
}

if (!$testConfig) {
    http_response_code(404);
    echo json_encode(['error' => 'Test not found']);
    exit;
}

// Return test configuration including required_fonts
header('Content-Type: application/json');
echo json_encode([
    'id' => $testId,
    'name' => $testConfig['name'],
    'required_fonts' => $testConfig['required_fonts'] ?? [],
    'data_bundle' => $testConfig['data_bundle'] ?? 'core'
]);
```

### Declaring Font Requirements in Tests

Test developers can document required fonts in the test's `.pbl.about.txt` file:

```
Test Name: Chinese Character Learning
Description: Visual pattern learning with Chinese characters
Required Fonts: NotoSansCJK-Regular
Font Bundles: fonts-cjk

This test displays Chinese characters and MUST have CJK fonts loaded.
Add to test_catalog.json:
  "required_fonts": ["fonts-cjk"]
```

### Font Bundle Manifest

Create a font bundle manifest to document what's included:

**`config/font_bundles.json`:**
```json
{
  "fonts-cjk": {
    "name": "CJK Fonts",
    "size_mb": 19,
    "languages": ["zh", "ja", "ko"],
    "fonts": [
      {
        "file": "NotoSansCJK-Regular.ttc",
        "scripts": ["Han", "Hiragana", "Katakana", "Hangul"],
        "font_names": ["Noto Sans CJK SC", "Noto Sans CJK TC", "Noto Sans CJK JP", "Noto Sans CJK KR"]
      }
    ]
  },
  "fonts-nonlatin": {
    "name": "Non-Latin Scripts",
    "size_mb": 0.65,
    "languages": ["ar", "bn", "hi", "ka", "he", "th"],
    "fonts": [
      {"file": "NotoSansArabic-Regular.ttf", "scripts": ["Arabic"], "font_names": ["Noto Sans Arabic"]},
      {"file": "NotoSansBengali-Regular.ttf", "scripts": ["Bengali"], "font_names": ["Noto Sans Bengali"]},
      {"file": "NotoSansDevanagari-Regular.ttf", "scripts": ["Devanagari"], "font_names": ["Noto Sans Devanagari"]},
      {"file": "NotoSansGeorgian-Regular.ttf", "scripts": ["Georgian"], "font_names": ["Noto Sans Georgian"]},
      {"file": "NotoSansHebrew-Regular.ttf", "scripts": ["Hebrew"], "font_names": ["Noto Sans Hebrew"]},
      {"file": "NotoSansThai-Regular.ttf", "scripts": ["Thai"], "font_names": ["Noto Sans Thai"]}
    ]
  }
}
```

### Migration Path for Existing Tests

1. **Audit existing tests** for hard-coded font usage:
   ```bash
   grep -r "MakeFont.*Noto" battery/ --include="*.pbl"
   grep -r "Chinese\|Japanese\|Korean\|Arabic\|Hebrew" battery/ --include="*.pbl"
   ```

2. **Document findings** in test `.pbl.about.txt` files

3. **Update test_catalog.json** with `required_fonts` for affected tests

4. **Test each configuration** with different browser locales

### Benefits of This Approach

✅ **Explicit Control:** Test developers declare exactly what fonts they need

✅ **Backward Compatible:** Tests without `required_fonts` fall back to locale-based loading

✅ **Bandwidth Efficient:** Only loads what's actually needed

✅ **No Silent Failures:** Missing fonts are explicit, not hidden by locale mismatch

✅ **Easy to Maintain:** Central configuration in `test_catalog.json`

✅ **Future-Proof:** Easy to add new font bundles or update requirements

### Example Scenarios

| Scenario | Locale | Test Config | Fonts Loaded | Why |
|----------|--------|-------------|--------------|-----|
| English user, standard Stroop | en-US | (none) | Latin only | Locale default |
| Chinese user, standard Stroop | zh-CN | (none) | Latin + CJK | Locale detection |
| English user, Chinese learning | en-US | `["fonts-cjk"]` | Latin + CJK | Test override |
| Arabic user, English test | ar-SA | (none) | Latin + Non-Latin | Locale detection |
| Any user, multilingual test | * | `["fonts-cjk", "fonts-nonlatin"]` | Latin + Both | Test override |

## Conclusion

The font bundling strategy with test-specific overrides provides:
- **Significant** bandwidth savings (78% for majority of users)
- **Automatic** language detection using existing PEBL infrastructure
- **Explicit** test-level font requirements for special cases
- **Graceful** degradation if bundles fail to load
- **Easy** future extensibility for new languages

**Recommended implementation order:**
1. CJK bundle separation (immediate, high impact - 78% savings)
2. Platform launcher integration with locale detection
3. Add `required_fonts` field to test_catalog.json schema
4. Update getTestConfig.php API endpoint
5. Implement three-tier loading logic in launcher
6. Audit existing tests for hard-coded font usage
7. Document font requirements in affected tests
8. Non-Latin bundle separation (optional, incremental improvement)
9. Font subsetting (future optimization)
