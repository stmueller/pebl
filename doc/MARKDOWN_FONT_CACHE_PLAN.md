# Markdown Display + Font Cache/Factory Architecture

**Date:** 2025-12-04
**Author:** Claude Code
**Context:** Combines SDL_RTF_INVESTIGATION.md markdown plan with font caching optimization

---

## Executive Summary

This plan combines multiple complementary features:

1. **Font Cache/Factory** - Eliminate duplicate TTF_Font objects, especially for color variants
2. **Inline Formatted Text Display** - Simple rich text with `<b>`, `<i>`, `<c=color>` tags
3. **Markdown On-Screen Rendering** - Full CommonMark support for textboxes
4. **Markdown to HTML Export** - Generate HTML files from markdown using HTML.pbl library

### Key Insight

**SDL_TTF fonts are color-agnostic!** Colors are applied at render time via `TTF_RenderUTF8_Shaded()` and `TTF_RenderUTF8_Blended()`. Multiple PEBL PFont objects with identical (filename, style, size) but different colors can share the same underlying TTF_Font.

**Impact:**
- Markdown rendering with inline color changes won't create font bloat
- Bold/italic variants still need separate TTF_Font objects (different style)
- Dramatic memory savings for applications using many colored labels

---

## Part 1: Font Cache/Factory Architecture

### Current Problem

Every `MakeFont()` call in PEBL creates a new PlatformFont, which calls:

```cpp
mTTF_Font = TTF_OpenFont(mFontFileName.c_str(), mFontSize);
TTF_SetFontStyle(mTTF_Font, mFontStyle);
```

**Example wasteful pattern:**
```pebl
## Each creates separate TTF_Font despite only color difference
font_red   <- MakeFont(gPEBLBaseFont, 0, 24, MakeColor("red"), MakeColor("white"), 1)
font_blue  <- MakeFont(gPEBLBaseFont, 0, 24, MakeColor("blue"), MakeColor("white"), 1)
font_green <- MakeFont(gPEBLBaseFont, 0, 24, MakeColor("green"), MakeColor("white"), 1)
## Result: 3 identical TTF_Font objects loaded!
```

### Solution: Font Factory with Shared TTF_Font Cache

**Architecture:**

```cpp
// Font cache key (excludes color)
struct FontCacheKey {
    std::string filename;
    int style;           // PFS_Normal, PFS_Bold, PFS_Italic, etc.
    int size;

    bool operator<(const FontCacheKey& other) const;
};

// Cached TTF_Font with reference counting
struct CachedTTFFont {
    TTF_Font* ttf_font;
    int ref_count;

    CachedTTFFont(TTF_Font* f) : ttf_font(f), ref_count(1) {}
};

// Global font cache (singleton)
class FontCache {
public:
    static FontCache& GetInstance();

    // Get or create TTF_Font for given key
    TTF_Font* GetFont(const FontCacheKey& key, const std::string& full_path);

    // Release font (decrements ref count, deletes if 0)
    void ReleaseFont(const FontCacheKey& key);

private:
    std::map<FontCacheKey, CachedTTFFont> mCache;
    FontCache() {}
};
```

### Modified PlatformFont

**Constructor changes:**

```cpp
PlatformFont::PlatformFont(const std::string & filename, int style, int size,
                           PColor fgcolor, PColor bgcolor, bool aa):
    PFont(filename, style, size, fgcolor, bgcolor, aa),
    mChanged(false),
    mBuffer(NULL)
{
    string fontname = Evaluator::gPath.FindFile(mFontFileName);
    if(fontname == "")
        PError::SignalFatalError("Unable to find font file [" + mFontFileName + "].");

    // Create cache key (excludes color!)
    FontCacheKey key;
    key.filename = mFontFileName;
    key.style = mFontStyle;
    key.size = mFontSize;

    // Get shared TTF_Font from cache
    mTTF_Font = FontCache::GetInstance().GetFont(key, fontname);
    mFontCacheKey = key;  // Store for destructor

    // Colors are stored in PlatformFont, applied at render time
    mSDL_FGColor = SDLUtility::PColorToSDLColor(*mFontColor);
    mSDL_BGColor = SDLUtility::PColorToSDLColor(*mBackgroundColor);
}
```

**Destructor changes:**

```cpp
PlatformFont::~PlatformFont()
{
    // Don't call TTF_CloseFont directly! Use cache release
    FontCache::GetInstance().ReleaseFont(mFontCacheKey);
    mTTF_Font = NULL;

    free(mBuffer);
    mBuffer = NULL;
}
```

### Implementation Strategy

**Phase 1: Basic Font Cache**

1. Create `src/utility/FontCache.h` and `.cpp`
2. Implement `FontCache` singleton with `GetFont()` and `ReleaseFont()`
3. Modify `PlatformFont` constructor/destructor to use cache
4. Add `mFontCacheKey` member to `PlatformFont` class
5. **Backward compatible** - no PEBL script changes needed

**Phase 2: Verification**

1. Add debug output to track cache hits/misses
2. Test with battery tasks that create many fonts
3. Verify TTF_Font sharing works correctly with different colors
4. Check memory usage reduction

---

## Part 2: Markdown/Formatted Text Display

### Recommended Approach (from SDL_RTF_INVESTIGATION.md)

**Phase 1: Inline Formatting** (2-3 days)

Simple tag parser with basic formatting:
- `<b>bold</b>` - Bold text
- `<i>italic</i>` - Italic text
- `<c=red>colored</c>` - Colored text
- `<size=20>larger</size>` - Size changes

**Phase 2: Full Markdown Support** (1-2 weeks)

Integrate md4c parser for CommonMark support:
- Headers (# H1, ## H2)
- **Bold** and *italic*
- Bulleted/numbered lists
- Links and code blocks

### Integration with Font Cache

**Critical synergy:** Markdown rendering with inline color/style changes will create many font objects. The font cache prevents this from becoming a memory problem!

**Example markdown rendering:**

```pebl
text <- "This is <b>bold</b> and <c=red>red</c> and <b><c=blue>bold blue</c></b>."
```

**Without font cache:**
- Regular font @ 24pt
- Bold font @ 24pt (separate TTF_Font needed)
- Regular font @ 24pt + red color (duplicate TTF_Font created!)
- Bold font @ 24pt + blue color (duplicate TTF_Font created!)
- **Result:** 4 TTF_Font objects

**With font cache:**
- Regular font @ 24pt (cached, shared by all regular colored variants)
- Bold font @ 24pt (cached, shared by all bold colored variants)
- **Result:** 2 TTF_Font objects

### Formatted Text Rendering Architecture

```cpp
// Text segment for inline formatting
struct TextSegment {
    std::string text;
    FontCacheKey font_key;    // Cache key for TTF_Font
    SDL_Color fg_color;       // Applied at render time
    SDL_Color bg_color;       // Applied at render time
};

// Parser for inline formatting codes
class FormatParser {
public:
    std::vector<TextSegment> Parse(const std::string& text,
                                    const FontCacheKey& base_font);
private:
    void ParseTag(const std::string& tag, FontCacheKey& current_font,
                  SDL_Color& current_color);
};

// Renderer using font cache
SDL_Surface* RenderFormattedText(const std::vector<TextSegment>& segments) {
    // Create composite surface
    SDL_Surface* result = CreateCompositeSurface(segments);

    int x_offset = 0;
    for(const auto& seg : segments) {
        // Get cached TTF_Font
        TTF_Font* font = FontCache::GetInstance().GetFont(seg.font_key);

        // Render segment with segment-specific colors
        SDL_Surface* seg_surface = TTF_RenderUTF8_Blended(font,
                                                          seg.text.c_str(),
                                                          seg.fg_color);

        // Blit to result at x_offset
        SDL_Rect dest = {x_offset, 0, 0, 0};
        SDL_BlitSurface(seg_surface, NULL, result, &dest);
        x_offset += seg_surface->w;

        SDL_FreeSurface(seg_surface);
        // Note: Don't close font - it's cached!
    }

    return result;
}
```

---

## Part 3: Implementation Plan

### Phase 1: Font Cache (Week 1)

**Goal:** Eliminate duplicate TTF_Font objects for color variants

- [ ] Create `src/utility/FontCache.h` and `FontCache.cpp`
- [ ] Implement `FontCacheKey` struct with comparison operators
- [ ] Implement `CachedTTFFont` wrapper with ref counting
- [ ] Implement `FontCache` singleton class:
  - [ ] `GetFont(key, path)` - returns cached or creates new
  - [ ] `ReleaseFont(key)` - decrements ref, closes if 0
  - [ ] `GetStats()` - debug info on cache hits/misses
- [ ] Modify `PlatformFont.h`:
  - [ ] Add `#include "../../utility/FontCache.h"`
  - [ ] Add `FontCacheKey mFontCacheKey;` member
- [ ] Modify `PlatformFont.cpp`:
  - [ ] Constructor: Use `FontCache::GetFont()` instead of `TTF_OpenFont`
  - [ ] Copy constructor: Use `FontCache::GetFont()` with ref increment
  - [ ] Destructor: Use `FontCache::ReleaseFont()` instead of `TTF_CloseFont`
- [ ] Add debug output (optional, commented out):
  - [ ] Cache hits/misses
  - [ ] Ref count changes
  - [ ] Font creation/deletion
- [ ] Test:
  - [ ] Create multiple fonts with same specs but different colors
  - [ ] Verify single TTF_Font is shared
  - [ ] Verify proper cleanup when fonts destroyed

**Verification script:**
```pebl
define Start(p) {
    gWin <- MakeWindow("grey")

    ## These should share one TTF_Font (only color differs)
    font_red   <- MakeFont(gPEBLBaseFont, 0, 24, MakeColor("red"),   MakeColor("white"), 1)
    font_blue  <- MakeFont(gPEBLBaseFont, 0, 24, MakeColor("blue"),  MakeColor("white"), 1)
    font_green <- MakeFont(gPEBLBaseFont, 0, 24, MakeColor("green"), MakeColor("white"), 1)

    ## These need separate TTF_Font (different size/style)
    font_bold  <- MakeFont(gPEBLBaseFont, 1, 24, MakeColor("black"), MakeColor("white"), 1)
    font_big   <- MakeFont(gPEBLBaseFont, 0, 48, MakeColor("black"), MakeColor("white"), 1)

    ## Check FontCache stats - should show ~2 cache hits, 3 unique fonts
}
```

### Phase 2: Inline Formatting (Week 2)

**Goal:** Basic rich text with `<b>`, `<i>`, `<c=color>`, `<size=N>` tags

- [ ] Create `src/utility/FormatParser.h` and `FormatParser.cpp`
- [ ] Implement `TextSegment` struct
- [ ] Implement `FormatParser::Parse()`:
  - [ ] Tokenize text by tags
  - [ ] Build `FontCacheKey` for each segment (inherits + modifies)
  - [ ] Parse color tags to `SDL_Color`
  - [ ] Handle nested tags (e.g., `<b><c=red>text</c></b>`)
- [ ] Create `PlatformFont::RenderFormattedText()`:
  - [ ] Call `FormatParser::Parse()`
  - [ ] Get TTF_Font from cache for each segment
  - [ ] Render each segment with appropriate colors
  - [ ] Composite segments into single surface
- [ ] Modify `PlatformTextBox::RenderText()`:
  - [ ] Check if "FORMATTED" property is set
  - [ ] If true, call `RenderFormattedText()` instead of `RenderText()`
- [ ] Add "FORMATTED" property to PTextBox
- [ ] Update `FindNextLineBreak()` to strip tags when calculating width
- [ ] Test:
  - [ ] Plain text (no regression)
  - [ ] Simple formatting (`<b>`, `<i>`, `<c=red>`)
  - [ ] Nested formatting
  - [ ] Line wrapping with formatted text
  - [ ] Emscripten compatibility

**Test script:**
```pebl
define Start(p) {
    gWin <- MakeWindow("grey")
    font <- MakeFont(gPEBLBaseFont, 0, 24, MakeColor("black"), MakeColor("white"), 1)

    text <- "This is <b>bold</b>, <i>italic</i>, and <c=red>red text</c>."
    tb <- MakeTextBox(text, font, 400, 200)
    AddObject(tb, gWin)
    SetProperty(tb, "FORMATTED", 1)  ## Enable formatting

    Draw()
    WaitForKeyPress("X")
}
```

### Phase 3: Markdown Support (Week 3-4)

**Goal:** Full Markdown parsing with md4c for on-screen rendering

- [ ] Download and integrate md4c:
  - [ ] Add `src/utility/md4c.h` and `md4c.c` to project
  - [ ] Update Makefile to compile md4c
- [ ] Create `src/utility/MarkdownRenderer.h` and `.cpp`
- [ ] Implement `MarkdownRenderer` class:
  - [ ] Integrate md4c parser callbacks
  - [ ] Convert markdown AST to `TextSegment` list
  - [ ] Handle block elements (paragraphs, headers, lists)
  - [ ] Handle inline elements (bold, italic, code, links)
  - [ ] Use FontCache for all font variants
- [ ] Add "MARKDOWN" property to PTextBox
- [ ] Extend `PlatformTextBox::RenderText()`:
  - [ ] Check "MARKDOWN" property
  - [ ] Call `MarkdownRenderer::Render()` if true
- [ ] Test:
  - [ ] Basic markdown (bold, italic)
  - [ ] Headers (different sizes)
  - [ ] Lists (bulleted, numbered)
  - [ ] Links (underlined, colored)
  - [ ] Mixed markdown and plain text
  - [ ] Emscripten compatibility

**Test script:**
```pebl
define Start(p) {
    gWin <- MakeWindow("grey")
    font <- MakeFont(gPEBLBaseFont, 0, 18, MakeColor("black"), MakeColor("white"), 1)

    markdown <- "# Heading" + CR(2) +
                "This is **bold** and *italic* text." + CR(2) +
                "- Item 1" + CR(1) +
                "- Item 2" + CR(1) +
                "- Item 3"

    tb <- MakeTextBox(markdown, font, 500, 400)
    AddObject(tb, gWin)
    SetProperty(tb, "MARKDOWN", 1)  ## Enable markdown

    Draw()
    WaitForKeyPress("X")
}
```

### Phase 4: Markdown to HTML Export (Week 5)

**Goal:** Convert markdown to HTML files using pebl-lib/HTML.pbl

This phase adds the ability to export markdown content as HTML files, which is useful for:
- Generating static documentation
- Creating printable experiment instructions
- Viewing formatted content in web browsers
- Archiving experiment materials in portable format

#### Architecture

**New PEBL function:** `MarkdownToHTML(markdown_text, output_file)`

```cpp
// In src/libs/PEBLEnvironment.cpp or new file src/libs/PEBLMarkdown.cpp
Variant PEBLMarkdown::MarkdownToHTML(Variant v)
{
    // v is list: [markdown_text, output_file]
    PList* plist = v.GetComplexData()->GetList();
    std::string markdown_text = plist->Get(1).GetString();
    std::string output_file = plist->Get(2).GetString();

    // Parse markdown using md4c
    MarkdownParser parser;
    HTMLNode* ast = parser.Parse(markdown_text);

    // Convert AST to HTML string
    std::string html = ConvertASTToHTML(ast);

    // Write to file
    std::ofstream out(output_file);
    out << html;
    out.close();

    return Variant(true);
}

// Helper to convert md4c AST to HTML
std::string ConvertASTToHTML(HTMLNode* node)
{
    // Traverse AST and generate HTML tags
    // This is where we map markdown elements to HTML
}
```

#### Implementation Steps

- [ ] Create `src/utility/MarkdownParser.h` and `.cpp`:
  - [ ] Wrapper around md4c for parsing
  - [ ] Build internal AST from md4c callbacks
  - [ ] AST nodes: Heading, Paragraph, List, ListItem, Bold, Italic, Code, Link, etc.

- [ ] Create `src/utility/HTMLGenerator.h` and `.cpp`:
  - [ ] Convert markdown AST to HTML string
  - [ ] Map markdown elements to HTML tags:
    - `# Heading` → `<h1>Heading</h1>`
    - `**bold**` → `<b>bold</b>`
    - `*italic*` → `<i>italic</i>`
    - `- item` → `<ul><li>item</li></ul>`
    - `` `code` `` → `<code>code</code>`
    - `[link](url)` → `<a href="url">link</a>`

- [ ] Add PEBL function `MarkdownToHTML()`:
  - [ ] Register in `src/libs/Functions.h`
  - [ ] Implement in `src/libs/PEBLEnvironment.cpp` or create `src/libs/PEBLMarkdown.cpp`
  - [ ] Signature: `MarkdownToHTML(markdown_text, output_file)`
  - [ ] Optional: `MarkdownToHTML(markdown_text, output_file, stylesheet_path)`

- [ ] Extend pebl-lib/HTML.pbl with markdown helpers (optional):
  - [ ] `MarkdownPage(markdown_text)` - Convert markdown to full HTML page
  - [ ] Integrate with existing `Page()` function
  - [ ] Add CSS styling for markdown elements

- [ ] Add optional styling support:
  - [ ] Default CSS for markdown rendering
  - [ ] User can provide custom CSS file path
  - [ ] Include common styles: code blocks, lists, headers, links

- [ ] Test:
  - [ ] Convert simple markdown to HTML
  - [ ] Verify all markdown elements render correctly
  - [ ] Test with complex nested structures
  - [ ] Verify file writing works
  - [ ] Test with custom CSS stylesheets

#### PEBL Script Usage

**Basic usage:**
```pebl
define Start(p) {
    markdown <- "# Experiment Instructions" + CR(2) +
                "Welcome to the **reaction time** experiment." + CR(2) +
                "## Procedure" + CR(2) +
                "1. Press spacebar when ready" + CR(1) +
                "2. Respond as *quickly* as possible" + CR(1) +
                "3. Complete all 100 trials"

    ## Export to HTML file
    MarkdownToHTML(markdown, "instructions.html")

    ## Can also display on screen
    gWin <- MakeWindow("grey")
    font <- MakeFont(gPEBLBaseFont, 0, 18, MakeColor("black"), MakeColor("white"), 1)
    tb <- MakeTextBox(markdown, font, 600, 400)
    AddObject(tb, gWin)
    SetProperty(tb, "MARKDOWN", 1)
    Draw()
}
```

**With custom styling:**
```pebl
define Start(p) {
    markdown <- FileReadText("experiment_protocol.md")

    ## Generate HTML with custom CSS
    MarkdownToHTML(markdown, "protocol.html", "styles/custom.css")

    Print("HTML documentation generated: protocol.html")
}
```

**Batch conversion:**
```pebl
define Start(p) {
    ## Convert all markdown files in a directory
    files <- FileReadList("docs/markdown_files.txt")

    loop(file, files) {
        markdown <- FileReadText("docs/" + file + ".md")
        MarkdownToHTML(markdown, "output/" + file + ".html")
        Print("Converted: " + file)
    }
}
```

#### Integration with Existing HTML.pbl

The HTML.pbl library can be extended to support markdown conversion at the PEBL script level:

**Add to pebl-lib/HTML.pbl:**
```pebl
## Convert markdown to HTML using built-in MarkdownToHTML function
define MarkdownToHTMLFile(markdown, filename, css:"default.css")
{
    ## Use the C++ built-in function
    success <- MarkdownToHTML(markdown, filename)
    return success
}

## Wrap markdown in full HTML page structure
define MarkdownPage(markdown_text, title:"Document")
{
    ## This would call C++ MarkdownToHTML to get body HTML
    ## Then wrap it in Page() structure with CSS

    body <- MarkdownToHTMLBody(markdown_text)  ## C++ function

    page <- "<html>" + CR(1) +
            "<head>" + CR(1) +
            "<title>" + title + "</title>" + CR(1) +
            "<link rel='stylesheet' href='markdown.css'/>" + CR(1) +
            "</head>" + CR(1) +
            "<body>" + CR(1) +
            body + CR(1) +
            "</body></html>"

    return page
}
```

#### Default CSS for Markdown

Include a default `markdown.css` stylesheet in `media/`:

```css
body {
    font-family: 'DejaVu Sans', Arial, sans-serif;
    line-height: 1.6;
    max-width: 800px;
    margin: 0 auto;
    padding: 20px;
}

h1 { font-size: 32px; color: #333; border-bottom: 2px solid #333; }
h2 { font-size: 24px; color: #444; }
h3 { font-size: 20px; color: #555; }

code {
    background-color: #f4f4f4;
    padding: 2px 4px;
    border-radius: 3px;
    font-family: 'Courier New', monospace;
}

pre {
    background-color: #f4f4f4;
    padding: 10px;
    border-radius: 5px;
    overflow-x: auto;
}

ul, ol {
    padding-left: 30px;
}

a {
    color: #0066cc;
    text-decoration: none;
}

a:hover {
    text-decoration: underline;
}
```

#### Use Cases

1. **Experiment Documentation:**
   ```pebl
   ## Generate HTML documentation from markdown protocol
   protocol <- FileReadText("protocols/stroop.md")
   MarkdownToHTML(protocol, "docs/stroop_protocol.html")
   ```

2. **Results Reporting:**
   ```pebl
   ## Create formatted results report
   report <- "# Experiment Results" + CR(2) +
             "**Participant:** " + gSubNum + CR(2) +
             "## Summary Statistics" + CR(1) +
             "- Mean RT: " + Mean(rts) + " ms" + CR(1) +
             "- Accuracy: " + Mean(accuracy) + "%" + CR(1)

   MarkdownToHTML(report, "results/subject_" + gSubNum + "_report.html")
   ```

3. **Instruction Sheets:**
   ```pebl
   ## Generate printable instruction sheet
   instructions <- FileReadText("instructions.md")
   MarkdownToHTML(instructions, "print/instructions.html", "print_styles.css")
   Print("Open print/instructions.html in browser and print")
   ```

4. **Multi-language Documentation:**
   ```pebl
   ## Generate HTML instructions in multiple languages
   languages <- ["en", "es", "fr", "de"]

   loop(lang, languages) {
       markdown <- FileReadText("instructions_" + lang + ".md")
       MarkdownToHTML(markdown, "docs/instructions_" + lang + ".html")
   }
   ```

#### Success Criteria

- ✅ Markdown text converts to valid HTML
- ✅ All markdown elements supported: headers, bold, italic, lists, links, code
- ✅ Generated HTML opens correctly in web browsers
- ✅ Default CSS provides readable styling
- ✅ Custom CSS can be specified
- ✅ File writing works reliably
- ✅ Function accessible from PEBL scripts
- ✅ Integration with existing HTML.pbl library

---

## Part 4: Performance Characteristics

### Memory Savings

**Without font cache:**
```
10 labels with same font but different colors:
10 × 33,270 bytes = 332,700 bytes (~325 KB)
```

**With font cache:**
```
10 labels with same font but different colors:
1 × 33,270 bytes = 33,270 bytes (~32 KB)
Savings: ~293 KB (90% reduction)
```

**Markdown with inline colors:**
```
Text: "The <c=red>red</c>, <c=blue>blue</c>, and <c=green>green</c> words."

Without cache: 4 TTF_Font objects (1 base + 3 colored "copies")
With cache: 1 TTF_Font object (shared by all color variants)
```

### CPU Performance

**Font loading:**
- Without cache: `TTF_OpenFont()` called on every `MakeFont()`
- With cache: `TTF_OpenFont()` called once, subsequent lookups are map lookups (O(log n))

**Text rendering:**
- No change - `TTF_RenderUTF8_*` calls are identical
- Colors applied at render time (always has been the case)

---

## Part 5: Backward Compatibility

### PEBL Scripts

✅ **100% backward compatible** - no script changes required

**Existing behavior preserved:**
```pebl
## This continues to work exactly as before
font <- MakeFont(gPEBLBaseFont, 0, 24, MakeColor("red"), MakeColor("white"), 1)
tb <- MakeTextBox("Plain text", font, 400, 200)
AddObject(tb, gWin)
```

**New features opt-in:**
```pebl
## Inline formatting (opt-in via property)
SetProperty(tb, "FORMATTED", 1)

## Markdown (opt-in via property)
SetProperty(tb, "MARKDOWN", 1)
```

### C++ API

Font cache is **internal implementation detail**:
- PFont/PlatformFont interface unchanged
- MakeFont() PEBL function unchanged
- Existing code continues to work

---

## Part 6: Testing Strategy

### Unit Tests

1. **Font Cache Tests:**
   - Create identical fonts → verify single TTF_Font
   - Create different colors → verify TTF_Font shared
   - Create different sizes → verify separate TTF_Font
   - Destroy fonts → verify ref counting works
   - Edge case: All fonts destroyed → cache empty

2. **Format Parser Tests:**
   - Parse simple tags: `<b>`, `<i>`, `<c=red>`
   - Parse nested tags: `<b><c=red>text</c></b>`
   - Parse malformed tags (error handling)
   - Strip tags for width calculation

3. **Rendering Tests:**
   - Render plain text (no regression)
   - Render formatted text
   - Render markdown
   - Line wrapping with formatting
   - Mixed LTR/RTL with formatting

### Integration Tests

1. **Battery tasks:**
   - Run existing battery tasks → no regressions
   - Check memory usage (should be lower with cache)
   - Verify all labels/textboxes render correctly

2. **Platform tests:**
   - Native Linux build
   - Emscripten/WebAssembly build
   - Windows build (if applicable)

### Performance Tests

1. **Memory usage:**
   - Create 100 fonts with different colors
   - Measure memory before/after font cache
   - Expected: 90%+ reduction

2. **Rendering speed:**
   - Benchmark text rendering with/without cache
   - Expected: Slight improvement (fewer font loads)

---

## Part 7: Risks and Mitigations

### Risk 1: Thread Safety

**Risk:** Font cache is global singleton, could have threading issues

**Mitigation:**
- PEBL is single-threaded (evaluator runs on main thread)
- If threading added later, use mutex for cache access

### Risk 2: TTF_Font Sharing Issues

**Risk:** SDL_TTF might not be designed for sharing TTF_Font across multiple owners

**Mitigation:**
- SDL_TTF documentation doesn't forbid this
- Colors are applied at render time, not stored in TTF_Font
- Reference counting ensures font not closed while in use
- Test thoroughly before deploying

### Risk 3: Cache Memory Growth

**Risk:** Cache grows unbounded if fonts never destroyed

**Mitigation:**
- Reference counting automatically removes unused fonts
- Cache only holds fonts with ref_count > 0
- Can add cache size limit if needed (unlikely)

### Risk 4: Formatted Text Compatibility

**Risk:** Existing scripts with `<` or `>` in text might be interpreted as tags

**Mitigation:**
- Formatting disabled by default
- Only enabled via `SetProperty(obj, "FORMATTED", 1)`
- Escape sequences: `&lt;` and `&gt;` for literal brackets

---

## Part 8: Documentation Updates

### User Documentation

1. **PEBL Manual (doc/pman/):**
   - Add section on formatted text
   - Document inline formatting syntax
   - Document markdown support
   - Show examples

2. **Function Reference:**
   - Update MakeTextBox entry
   - Document FORMATTED property
   - Document MARKDOWN property

### Developer Documentation

1. **Architecture docs:**
   - Document FontCache design
   - Document FormatParser design
   - Document MarkdownRenderer design

2. **CLAUDE.md updates:**
   - Add notes on font caching
   - Add notes on formatted text rendering
   - Reference MARKDOWN_FONT_CACHE_PLAN.md

---

## Part 9: Future Enhancements

### Potential Extensions

1. **CSS-like styling:**
   ```pebl
   SetProperty(tb, "STYLE", "h1 {size:32, color:blue} p {size:16}")
   ```

2. **HTML subset rendering:**
   - Full HTML would be overkill
   - But common tags (`<p>`, `<div>`, `<span>`) could be useful

3. **Rich text editor:**
   - Editable formatted text
   - WYSIWYG-style input
   - Copy/paste with formatting

4. **Font preloading:**
   - Warm cache with common fonts at startup
   - Reduce first-render latency

5. **Font fallback chains:**
   - "DejaVu Sans, Arial, sans-serif"
   - Try fonts in order until one works

6. **Extended HTML export features (Phase 4 extensions):**
   - PDF generation from markdown via HTML intermediate
   - Template system for consistent document styling
   - Table of contents generation
   - Automatic syntax highlighting for code blocks
   - Image embedding and optimization
   - Multi-page HTML document generation with navigation

---

## Part 10: Success Criteria

### Phase 1 (Font Cache)

- ✅ Multiple PFont objects with different colors share single TTF_Font
- ✅ Memory usage reduced by 80%+ for color-variant fonts
- ✅ All existing battery tasks work unchanged
- ✅ No memory leaks (valgrind clean)

### Phase 2 (Inline Formatting)

- ✅ Basic formatting tags work: `<b>`, `<i>`, `<c=color>`, `<size=N>`
- ✅ Nested tags work correctly
- ✅ Line wrapping handles formatted text
- ✅ Backward compatible (disabled by default)
- ✅ Works on native and Emscripten builds

### Phase 3 (Markdown On-Screen Rendering)

- ✅ CommonMark markdown renders correctly
- ✅ Headers, lists, bold, italic, code blocks supported
- ✅ md4c integrates cleanly
- ✅ Works on all platforms
- ✅ Font cache prevents memory bloat from markdown styling

### Phase 4 (Markdown to HTML Export)

- ✅ `MarkdownToHTML()` function converts markdown to HTML files
- ✅ All markdown elements map correctly to HTML tags
- ✅ Generated HTML is valid and browser-compatible
- ✅ Default CSS stylesheet provides readable formatting
- ✅ Custom CSS can be specified by user
- ✅ Integration with existing HTML.pbl library
- ✅ Use cases: documentation, results reporting, instruction sheets

---

## Conclusion

This integrated plan delivers:

1. **Immediate value:** Font cache eliminates wasteful duplicate TTF_Font objects
2. **User-facing feature:** Rich text rendering for better experiment instructions
3. **Future-proof:** Markdown support for standard, portable formatting
4. **Export capability:** Markdown to HTML conversion for documentation and archiving
5. **Performance:** Memory savings + faster font lookups
6. **Compatibility:** Zero breaking changes, all features opt-in

The font cache is **essential foundation** for markdown rendering. Without it, inline formatting would create memory problems. With it, we can render complex formatted text efficiently.

**Estimated timeline:**
- Week 1: Font cache implementation and testing
- Week 2: Inline formatting implementation and testing
- Week 3-4: Markdown on-screen rendering (optional, can defer)
- Week 5: Markdown to HTML export (optional, leverages Phase 3)

**Recommended action:** Start with Phase 1 (font cache), verify memory savings, then proceed to Phase 2 (inline formatting). Phases 3 and 4 can be implemented independently after Phase 1 is complete.
