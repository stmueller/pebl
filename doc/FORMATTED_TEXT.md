# Formatted Text in PEBL

## Overview

PEBL supports HTML-lite text formatting using simple HTML-like tags. This allows you to create rich text displays with bold, italic, underline, colored, and sized text, as well as headers, bullet lists, horizontal rules, and indentation.

**Two types of formatting are supported:**
- **Inline formatting** - Works in both Labels and TextBoxes: `<b>`, `<i>`, `<u>`, `<c=color>`, `<size=N>`, `<h1>`-`<h6>`, `<br>`
- **Block-level formatting** - Works only in TextBoxes: `<hr>`, `<li>`, `<indent>`

## Supported Tags

### Inline Formatting (Labels and TextBoxes)

#### Basic Text Styles
- `<b>text</b>` - **Bold** text
- `<i>text</i>` - *Italic* text
- `<u>text</u>` - Underlined text
- `<b><i>text</i></b>` - **Bold and italic** combined
- `<b><u>text</u></b>` - **Bold and underlined** combined

#### Colors
- `<c=colorname>text</c>` - Colored text using named colors
- `<c=#RRGGBB>text</c>` - Colored text using hex codes
- `<c=#RGB>text</c>` - Colored text using 3-digit hex codes

**Supported color names:** All 752 X11 color names are supported, including:
- Basic: red, blue, green, yellow, cyan, magenta, orange, purple, pink, brown, navy, gray/grey, black, white
- Extended: darkgreen, darkred, lightblue, cornflowerblue, mediumseagreen, palevioletred, etc.
- Numbered grays: gray0 through gray100 (also grey0-grey100)
- Variations: red1, red2, red3, red4, blue1, blue2, etc.

See `/src/objects/RGBColorNames.h` for the complete list of 752 color names.

**Examples:**
```pebl
"This is <c=red>red</c> and <c=#0000FF>blue</c> text"
"<c=darkgreen>Dark green</c> <c=cornflowerblue>Cornflower blue</c>"
"<c=#F0F>Short hex magenta</c>"
"<c=red><u>Red underline</u></c>"
```

#### Font Sizes
- `<size=N>text</size>` - Text with custom font size (in points)

**Example:**
```pebl
"Normal <size=24>LARGE</size> <size=12>small</size>"
```

#### Headers
- `<h1>text</h1>` - Largest header (bold, 32pt)
- `<h2>text</h2>` - Level 2 header (bold, 28pt)
- `<h3>text</h3>` - Level 3 header (bold, 24pt)
- `<h4>text</h4>` - Level 4 header (bold, 20pt)
- `<h5>text</h5>` - Level 5 header (bold, 18pt)
- `<h6>text</h6>` - Smallest header (bold, 16pt)

**Note:** Header tags are shortcuts that combine bold styling with predefined font sizes.

**Example:**
```pebl
"<h1>Main Title</h1> <h3>Subtitle</h3> Regular text"
```

#### Line Breaks
- `<br>` or `<br/>` - Insert a line break (converted to newline character)

**Example:**
```pebl
"First line<br>Second line<br/>Third line"
```

### Block-Level Formatting (TextBoxes Only)

These tags only work in TextBoxes and are designed for multi-line formatted content:

#### Horizontal Rule
- `<hr>` - Insert a horizontal line across the text width

**Example:**
```pebl
"Section 1<br><hr><br>Section 2"
```

#### Bullet Lists
- `<li>` - Insert a bullet point (•) and indent the following text

**Example:**
```pebl
"<li>First item<br><li>Second item<br><li>Third item"
```

#### Indentation
- `<indent>` - Indent text by 4 characters (default)
- `<indent=N>` - Indent text by N characters

**Note:** Indent values are multiplied by the average character width of the current font.

**Examples:**
```pebl
"<indent>This line is indented by 4 characters"
"<indent=8>This line is indented by 8 characters"
"Normal text<br><indent>Indented paragraph text"
```

## Usage

### Basic Usage with TextBox

```pebl
## Create a textbox with formatted text
font <- MakeFont("DejaVuSans.ttf", 0, 18, MakeColor("black"), MakeColor("white"), 1)
tb <- MakeTextBox("This is <b>bold</b> and <c=red>red</c>", font, 700, 50)

## Enable formatted mode
SetProperty(tb, "FORMATTED", 1)

## Add to window and display
AddObject(tb, gWin)
Draw()
```

### Basic Usage with Label

```pebl
## Create a label with formatted text
font <- MakeFont("DejaVuSans.ttf", 0, 18, MakeColor("black"), MakeColor("white"), 1)
label <- MakeLabel("This is <b>bold</b>, <u>underlined</u>, and <c=red>red</c>", font)

## Enable formatted mode
SetProperty(label, "FORMATTED", 1)

## Add to window and display
AddObject(label, gWin)
Move(label, 400, 300)
Draw()
```

### Using Headers in Labels

```pebl
## Headers work well in labels for titles
font <- MakeFont("DejaVuSans.ttf", 0, 16, MakeColor("black"), MakeColor("white"), 1)
title <- MakeLabel("<h2>Experiment Instructions</h2>", font)
SetProperty(title, "FORMATTED", 1)
AddObject(title, gWin)
Move(title, 400, 100)
```

### Using EasyLabel

```pebl
## EasyLabel also supports formatting
label <- EasyLabel("Formatted <b>text</b> with <c=blue>color</c>", 400, 300, gWin, 18)
SetProperty(label, "FORMATTED", 1)
```

### Complex TextBox Example

```pebl
## Create rich formatted text with all features
text <- "<h2>Formatted Text Demo</h2>" + CR(1) + CR(1) +
        "This textbox demonstrates <b>bold</b>, <i>italic</i>, and <u>underline</u>." + CR(1) + CR(1) +
        "<hr>" + CR(1) + CR(1) +
        "<h4>Features:</h4>" + CR(1) +
        "<li>First feature with <c=blue>colored text</c>" + CR(1) +
        "<li>Second feature with <size=20>larger text</size>" + CR(1) +
        "<li>Third feature with <b><u>combined formatting</u></b>" + CR(1) + CR(1) +
        "<indent>This paragraph is indented to set it apart from the list." + CR(1) + CR(1) +
        "<hr>" + CR(1) +
        "<h6>End of demo</h6>"

tb <- MakeTextBox(text, font, 700, 400)
SetProperty(tb, "FORMATTED", 1)
AddObject(tb, gWin)
Draw()
```

### Nested Formatting

Tags can be combined:

```pebl
"<b><c=navy>bold navy text</c></b>"
"<i><c=purple><size=20>italic purple large</size></c></i>"
"<u><b><c=red>bold red underlined</c></b></u>"
"<h3><u>Underlined header</u></h3>"
```

## Features

### Baseline Alignment

Different font sizes are automatically aligned by their baseline (not top edge), ensuring proper typographic appearance:

```pebl
"The <size=24>BIG</size> and the <size=10>small</size> align on baseline"
```

### FontCache Integration

Formatted text automatically benefits from the FontCache system, sharing font objects when possible for memory efficiency.

## Editing Formatted Text (TextBox Only)

**Important:** When you call `GetInput()` on a formatted textbox, the FORMATTED mode is **automatically disabled** to allow editing. Tags become visible as literal text that you can edit directly.

**Note:** Labels are not editable, so this behavior only applies to TextBoxes.

**Behavior:**
1. Original formatted text: `"Hello <b>World</b>"` (displayed with bold formatting)
2. When `GetInput()` is called:
   - System prints: `"INFO: Disabling formatted mode for editing"`
   - FORMATTED property is set to 0
   - Text becomes: `"Hello <b>World</b>"` (tags visible as literal text)
3. User edits the raw markdown text, including tags
4. Edited text is returned with tags intact

**To re-enable formatting after editing:**
```pebl
tb <- MakeTextBox("Edit: <b>bold</b> <c=red>red</c>", font, 700, 50)
SetProperty(tb, "FORMATTED", 1)
AddObject(tb, gWin)
Draw()

## This will disable formatted mode and show tags as literal text
result <- GetInput(tb, "<return>")
## result contains text with tags: "Edit: <b>bold</b> <c=red>red</c>"

## Re-enable formatting to see the formatted result
SetText(tb, result)
SetProperty(tb, "FORMATTED", 1)
Draw()
```

## Implementation Details

### Parser
- Location: `src/utility/FormatParser.h`, `src/utility/FormatParser.cpp`
- Function: `ParseFormattedText()` - Converts tagged text into segments with formatting metadata
- Function: `StripFormatting()` - Removes all formatting tags, leaving displayable text

### Rendering
- Location: `src/platforms/sdl/PlatformTextBox.cpp`
- Two-pass rendering for baseline alignment:
  1. First pass: Find maximum ascent among all segments
  2. Second pass: Render each segment with y-offset for baseline alignment

### Property
- `FORMATTED` - Integer property (0 or 1)
  - 0: Normal text rendering (tags shown as literal text)
  - 1: Formatted text rendering (tags parsed and applied)

## Testing

Several test files demonstrate the features:

**TextBox tests:**
- `test-format.pbl` - Comprehensive formatting examples in TextBoxes
- `test-baseline-alignment.pbl` - Demonstrates baseline alignment
- `test-br-quick.pbl` - Tests `<br>` tag functionality
- `test-br-and-editing.pbl` - Tests both `<br>` and editing behavior
- `test-htmllite-markup.pbl` - Comprehensive test of all HTML-lite markup features including `<u>`, `<h1>`-`<h6>`, `<hr>`, `<li>`, `<indent>`

**Label tests:**
- `test-formatted-labels.pbl` - Comprehensive formatting examples in Labels
- Tests bold, italic, colors, sizes, baseline alignment
- Includes EasyLabel examples

## Limitations

1. **Editing (TextBox only):** Formatted text is automatically converted to plain text mode when edited via GetInput()
2. **Block-level tags (TextBox only):** The tags `<hr>`, `<li>`, and `<indent>` only work in TextBoxes, not in Labels
3. **Labels are single-line:** Labels don't support multi-line text
4. **No nesting depth limits:** Deeply nested tags work but may be hard to read
5. **Unknown tags:** Unrecognized tags are treated as literal text
6. **No text justification per segment:** Justification applies to entire line

## Future Enhancements (Not Yet Implemented)

- Strikethrough support (`<s>`)
- Background color per segment
- Hyperlinks with click handlers
- Superscript/subscript (`<sup>`, `<sub>`)
- Custom font families per segment
- Table support
