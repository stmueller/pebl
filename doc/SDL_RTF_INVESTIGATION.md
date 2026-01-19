# SDL_rtf Investigation: Rich Text and Markdown Support for PEBL

**Date:** 2025-11-18
**Purpose:** Investigate options for adding SDL_rtf library or markdown rendering to PEBL textboxes

---

## Executive Summary

**SDL_rtf Status:**
- ❌ **Not recommended** for PEBL
- Repository is **archived** with no further development planned
- Only supports **RTF format**, not Markdown
- **Not available** as an Emscripten port
- Requires SDL3 (PEBL uses SDL2)

**Alternative Recommendations:**
1. **Option A (Recommended):** Implement **inline formatting codes** in existing PlatformTextBox
2. **Option B:** Integrate **md4c** (C markdown parser) + custom renderer
3. **Option C:** Create separate **PFormattedTextBox** class

---

## SDL_rtf Analysis

### What is SDL_rtf?

SDL_rtf is a library for rendering Rich Text Format (.rtf) files in SDL applications.

### Key Findings

| Aspect | Details |
|--------|---------|
| **Current Version** | 3.0.0 (SDL3 only) |
| **SDL Compatibility** | SDL3 (PEBL uses SDL2) |
| **Format Support** | RTF only (NO Markdown support) |
| **Repository Status** | Archived - no further work planned |
| **Emscripten Port** | ❌ NOT available in emscripten-ports |
| **Windows Support** | Yes (when it was actively maintained) |
| **License** | zlib |
| **Dependencies** | SDL3 + SDL3_ttf |

### Why SDL_rtf is Not Suitable

1. **Wrong SDL Version** - Requires SDL3, PEBL uses SDL2
2. **Wrong Format** - RTF is not Markdown
3. **Abandoned** - Repository explicitly states "no further work is planned"
4. **No Web Support** - Not available as Emscripten port
5. **API Mismatch** - Would require significant integration work

---

## Emscripten SDL Port Availability

### Available SDL Companion Libraries in Emscripten

| Library | Status | Purpose |
|---------|--------|---------|
| SDL2 | ✅ Available | Core library |
| SDL2_image | ✅ Available | Image loading |
| SDL2_ttf | ✅ Available | TrueType font rendering |
| SDL2_net | ✅ Available | Networking (PEBL now uses this) |
| SDL2_mixer | ✅ Available | Audio mixing (PEBL uses this) |
| **SDL_rtf** | ❌ **NOT available** | Rich text format |

**To check:** Run `emcc --show-ports` to see full list

---

## Current PEBL Text Rendering Architecture

### Class Hierarchy

```
PEBLObject
  └─ PWidget
      └─ PTextObject (generic text)
          └─ PTextBox (editable/non-editable textbox)
              └─ PlatformTextBox (SDL-specific implementation)
      └─ PLabel (single-line text, different from TextBox)
```

### Key Components

#### 1. **PlatformFont** (`src/platforms/sdl/PlatformFont.cpp`)
**Responsibilities:**
- Font loading via SDL_ttf (`TTF_OpenFontRW`)
- Text rendering to SDL_Surface (`TTF_RenderUTF8_Shaded`, `TTF_RenderUTF8_Blended`)
- UTF-8 support
- RTL (right-to-left) text support via HarfBuzz
- Text measurement (`GetTextWidth`, `GetTextHeight`, `GetPosition`)
- Script detection and direction setting

**Key Functions:**
```cpp
SDL_Surface * RenderText(const std::string & text);  // Main rendering
unsigned int GetTextWidth(const std::string & text);
unsigned int GetTextHeight(const std::string & text);
unsigned int GetPosition(const std::string & text, unsigned int x);
```

**Current Capabilities:**
- ✅ UTF-8 full support
- ✅ RTL languages (Arabic, Hebrew) via HarfBuzz
- ✅ Font styles (bold, italic, underline via TTF_SetFontStyle)
- ✅ Anti-aliasing
- ✅ Foreground/background colors
- ❌ No inline formatting (bold/italic within same string)
- ❌ No hyperlinks
- ❌ No embedded images

#### 2. **PlatformTextBox** (`src/platforms/sdl/PlatformTextBox.cpp`) - 1332 lines

**Responsibilities:**
- Multi-line text layout
- Line wrapping (`FindNextLineBreak`)
- Text justification (LEFT, CENTER, RIGHT)
- Automatic RTL justification
- Cursor positioning and rendering
- Keyboard input handling
- Editable text support

**Key Functions:**
```cpp
bool RenderText();                                  // Renders all lines
void FindBreaks();                                  // Calculate line breaks
int FindNextLineBreak(unsigned int curposition);    // Word wrapping logic
int FindCursorPosition(long int x, long int y);     // Mouse click to cursor
void DrawCursor();                                  // Visual cursor
```

**Current Capabilities:**
- ✅ Multi-line text
- ✅ Word wrapping at word boundaries
- ✅ Three justification modes (LEFT, CENTER, RIGHT)
- ✅ Auto-RTL justification for Arabic/Hebrew
- ✅ Editable textboxes with cursor
- ✅ UTF-8 multi-byte character handling
- ❌ No inline styling
- ❌ No bulleted lists
- ❌ No embedded formatting

#### 3. **PTextObject** (`src/objects/PTextObject.h`)
**Responsibilities:**
- Abstract base for all text-containing objects
- Stores `mText` (std::string)
- Text change tracking (`mTextChanged`)
- RTL direction support (`mDirection`)

---

## Architecture Options for Rich Text Support

### Option A: Inline Formatting Codes (RECOMMENDED)

**Concept:** Extend existing PlatformTextBox to parse simple inline formatting codes

**Example Syntax:**
```
This is <b>bold text</b> and <i>italic text</i>.
Use <c=red>colored text</c> or <size=20>larger text</size>.
```

**Implementation Strategy:**

1. **Extend PlatformFont::RenderText():**
   ```cpp
   // Current: Renders entire string with one font style
   SDL_Surface * RenderText(const std::string & text);

   // New: Render with inline formatting
   SDL_Surface * RenderFormattedText(const std::string & text);
   ```

2. **Add Formatting Parser:**
   ```cpp
   struct TextSegment {
       std::string text;
       TTF_Font* font;        // Different font for bold/italic
       SDL_Color color;
       int size;
   };

   std::vector<TextSegment> ParseFormatting(const std::string & text);
   ```

3. **Modify PlatformTextBox::RenderText():**
   - Parse each line for formatting codes
   - Render segments separately
   - Blit segments side-by-side

**Advantages:**
- ✅ Works with existing architecture
- ✅ Backward compatible (plain text still works)
- ✅ No external dependencies
- ✅ Works on all platforms (native, Emscripten, Windows)
- ✅ Relatively simple implementation

**Disadvantages:**
- ❌ Custom format (not standard Markdown)
- ❌ Limited formatting options
- ❌ Requires maintaining parser code

**Estimated Complexity:** Medium (2-3 days)

---

### Option B: Markdown Parser Integration

**Concept:** Integrate md4c (C markdown parser) + custom SDL renderer

**Library:** [md4c](https://github.com/mity/md4c)
- ✅ Pure C, no dependencies
- ✅ CommonMark 0.31 compliant
- ✅ Very fast, compact (one .c + one .h file)
- ✅ SAX-like parser interface
- ✅ Should work with Emscripten

**Implementation Strategy:**

1. **Add md4c to PEBL:**
   ```
   src/utility/md4c.c
   src/utility/md4c.h
   ```

2. **Create Markdown Renderer:**
   ```cpp
   class MarkdownRenderer {
   public:
       SDL_Surface* RenderMarkdown(const std::string& md_text,
                                     PlatformFont* font);
   private:
       static int enter_block_callback(MD_BLOCKTYPE, void*, void*);
       static int leave_block_callback(MD_BLOCKTYPE, void*, void*);
       static int enter_span_callback(MD_SPANTYPE, void*, void*);
       static int leave_span_callback(MD_SPANTYPE, void*, void*);
       static int text_callback(MD_TEXTTYPE, const MD_CHAR*, MD_SIZE, void*);
   };
   ```

3. **New PEBL Object:**
   ```cpp
   // Option 1: Extend PlatformTextBox
   void PlatformTextBox::SetMarkdown(bool enable);

   // Option 2: New class
   class PMarkdownBox : public PlatformTextBox {
       bool RenderText() override;  // Uses MarkdownRenderer
   };
   ```

**Advantages:**
- ✅ Standard Markdown format
- ✅ Rich formatting (headers, lists, links, code blocks)
- ✅ Minimal dependencies (md4c is tiny)
- ✅ Cross-platform compatible
- ✅ Users familiar with Markdown

**Disadvantages:**
- ❌ More complex implementation
- ❌ Requires layout engine for block elements
- ❌ May not support all Markdown in initial version
- ❌ Additional library to maintain

**Estimated Complexity:** High (1-2 weeks)

---

### Option C: Separate PFormattedTextBox Class

**Concept:** Create entirely new class separate from PTextBox

**Implementation:**
```cpp
class PFormattedTextBox : public PWidget {
public:
    void SetContent(const std::string& content);
    void SetFormat(FormatType type);  // PLAIN, MARKDOWN, CUSTOM

private:
    FormatType mFormat;
    std::vector<FormattedLine> mLines;
};
```

**Advantages:**
- ✅ Clean separation from existing code
- ✅ Doesn't risk breaking existing textboxes
- ✅ Can implement sophisticated layouts

**Disadvantages:**
- ❌ Duplicates much of PTextBox functionality
- ❌ Users need to know when to use which
- ❌ More code to maintain

**Estimated Complexity:** High (2 weeks)

---

## Recommended Approach

### Phase 1: Inline Formatting (Short-term)

Implement **Option A** as a quick win:

1. Add simple tag parser to `PlatformFont`
2. Support basic tags: `<b>`, `<i>`, `<c=color>`, `<size=N>`
3. Extend `PlatformTextBox::RenderText()` to handle formatted segments
4. Maintain backward compatibility (plain text works unchanged)

**PEBL Script Usage:**
```pebl
tb <- MakeTextBox("This is <b>bold</b> and <i>italic</i> text.", font, 400, 200)
SetProperty(tb, "FORMATTED", 1)  ## Enable formatting
```

### Phase 2: Markdown Support (Long-term)

Implement **Option B** as a follow-up:

1. Integrate md4c library
2. Create `MarkdownRenderer` class
3. Add `SetMarkdown(true)` method to PTextBox
4. Implement subset of Markdown initially:
   - **Bold**, *italic*, `code`
   - Headers (# H1, ## H2)
   - Bulleted lists
   - Paragraphs

**PEBL Script Usage:**
```pebl
tb <- MakeTextBox("# Heading\n\nThis is **bold** and *italic*.", font, 400, 300)
SetProperty(tb, "MARKDOWN", 1)
```

---

## Technical Considerations

### Font Management for Styled Text

Current limitation: One TTF_Font per textbox

**Solution:** Pre-create font variants:
```cpp
struct FontSet {
    TTF_Font* regular;
    TTF_Font* bold;
    TTF_Font* italic;
    TTF_Font* bold_italic;
};
```

### Line Breaking with Inline Styles

Challenge: Line wrapping needs to account for formatting codes

**Solution:** Strip formatting tags when calculating line width:
```cpp
std::string StripFormatting(const std::string& text);
int GetTextWidthIgnoringTags(const std::string& text);
```

### Cursor Positioning with Formatting

Challenge: Cursor byte position vs. visual position

**Solution:** Map logical positions to visual positions:
```cpp
int GetVisualPosition(int logical_pos, const std::string& formatted_text);
```

### Backward Compatibility

**Critical:** Don't break existing PEBL scripts

**Strategy:**
- Formatting disabled by default
- `SetProperty(obj, "FORMATTED", 1)` to enable
- Plain text continues to work exactly as before

---

## Testing Plan

### Test Cases

1. **Plain text** (no regression)
2. **Basic formatting** (`<b>`, `<i>`)
3. **Nested formatting** (`<b><i>text</i></b>`)
4. **Color formatting**
5. **Mixed LTR/RTL with formatting**
6. **Line wrapping** with formatted text
7. **Cursor positioning** in formatted text
8. **Copy/paste** behavior
9. **Emscripten compatibility**

---

## Conclusion

### Immediate Recommendation

**Start with Option A (Inline Formatting)**

**Why:**
- Quick to implement (2-3 days)
- Low risk (backward compatible)
- Cross-platform (native, Emscripten, Windows)
- Solves 80% of use cases
- No external dependencies

**Future Enhancement:**
- Add Markdown support (Option B) later if needed
- md4c is small enough to include directly
- Can coexist with inline formatting

**Don't use SDL_rtf:**
- Wrong format (RTF != Markdown)
- Wrong SDL version (SDL3 != SDL2)
- Abandoned project
- Not available for Emscripten

---

## Implementation Checklist

### Phase 1: Inline Formatting

- [ ] Create `FormattingParser` class in `PlatformFont.cpp`
- [ ] Add tag definitions (`<b>`, `<i>`, `<c=color>`, `<size=N>`)
- [ ] Implement `ParseFormattedText()` → vector of segments
- [ ] Modify `RenderText()` to render segments
- [ ] Update `GetTextWidth()` to handle formatted text
- [ ] Update `FindNextLineBreak()` for formatted text
- [ ] Add `FORMATTED` property to PTextBox
- [ ] Test on native Linux
- [ ] Test on Emscripten
- [ ] Update documentation

### Phase 2: Markdown Support (Future)

- [ ] Add `src/utility/md4c.c` and `.h`
- [ ] Create `MarkdownRenderer` class
- [ ] Implement block elements (paragraphs, headers, lists)
- [ ] Implement inline elements (bold, italic, code)
- [ ] Add `MARKDOWN` property to PTextBox
- [ ] Test rendering
- [ ] Update documentation

---

## References

- SDL_rtf: https://github.com/libsdl-org/SDL_rtf
- md4c: https://github.com/mity/md4c
- Emscripten ports: https://github.com/emscripten-ports
- PEBL font rendering: `src/platforms/sdl/PlatformFont.cpp`
- PEBL textbox rendering: `src/platforms/sdl/PlatformTextBox.cpp`
