#ifndef __FORMATPARSER_H__
#define __FORMATPARSER_H__

#include <string>
#include <vector>
#include "../objects/PColor.h"

/// FormatParser - Parse simple HTML-lite formatting tags in text
/// Supports inline formatting:
///   <b>bold text</b>
///   <i>italic text</i>
///   <u>underline text</u>
///   <c=red>colored text</c>
///   <h1> through <h6> - headers with proportional sizes (bold, scaled)
///   <h1=center>, <h2=right> - headers with justification
///   <br> - line break
///
/// Supports block-level formatting (TextBox only):
///   <hr> - horizontal rule
///   <li> - bullet list item
///   <indent> or <indent=8> - indentation (default 4 chars)
///   <p align=left>, <p align=center>, <p align=right> - paragraph justification
///   <p size=150> - proportional font size (100 = base font, 150 = 150% of base)
///   <p align=center size=120> - combine alignment and size
///
/// PROPORTIONAL SIZING: All font sizes are relative to base font (100 = base)
/// This ensures proper scaling with adaptive textboxes.
///
/// This is a simple, non-nested tag parser for basic text formatting
/// without requiring external markdown libraries.

namespace FormatParser {

/// Justification types for paragraph-level alignment
enum Justification {
    JUSTIFY_NONE = 0,    // No justification set (use default)
    JUSTIFY_LEFT = 1,    // Left-aligned
    JUSTIFY_CENTER = 2,  // Center-aligned
    JUSTIFY_RIGHT = 3    // Right-aligned
};

/// Represents one segment of formatted text
/// Each segment has its own text content and formatting properties
struct FormatSegment {
    std::string text;           // The actual text to render
    int style;                  // Font style: 0=normal, 1=bold, 2=italic, 4=underline (bitwise OR)
    bool hasColorOverride;      // True if color was specified in tag
    PColor colorOverride;       // Color override (if hasColorOverride=true)
    bool hasSizeOverride;       // True if size was specified in tag
    int sizeOverride;           // Font size as percentage of base (100 = base font, 150 = 150% of base)

    // Block-level formatting (TextBox only)
    int indentPixels;           // Horizontal indent in pixels (0 = no indent)
    bool isHorizontalRule;      // True if this segment is a <hr>
    bool isBulletItem;          // True if this segment is a <li>
    Justification justification; // Text justification (left, center, right)

    FormatSegment() :
        text(""),
        style(0),
        hasColorOverride(false),
        colorOverride(0, 0, 0, 255),  // Default black color
        hasSizeOverride(false),
        sizeOverride(0),
        indentPixels(0),
        isHorizontalRule(false),
        isBulletItem(false),
        justification(JUSTIFY_NONE)
    {
    }
};

/// Parse text with inline formatting tags into segments
/// @param input Text with formatting tags (e.g., "Normal <b>bold</b> text")
/// @param charWidth Character width in pixels for indent calculations (default 8)
/// @return Vector of segments, each with text and formatting properties
///
/// Inline tag syntax:
///   <b>text</b>        - Bold text
///   <i>text</i>        - Italic text
///   <u>text</u>        - Underlined text
///   <c=red>text</c>    - Colored text (supports color names and hex #RRGGBB)
///   <h1>text</h1>      - Header level 1 (bold, 230% of base font)
///   <h2>text</h2>      - Header level 2 (bold, 200% of base font)
///   <h3>text</h3>      - Header level 3 (bold, 170% of base font)
///   <h4>text</h4>      - Header level 4 (bold, 140% of base font)
///   <h5>text</h5>      - Header level 5 (bold, 130% of base font)
///   <h6>text</h6>      - Header level 6 (bold, 115% of base font)
///   <br>               - Line break
///
/// Block-level tag syntax (TextBox only):
///   <p align=center>   - Center-aligned paragraph
///   <p align=right>    - Right-aligned paragraph
///   <p size=150>       - Proportional font size (150% of base font)
///   <p align=center size=120> - Combine alignment and size
///   <hr>               - Horizontal rule
///   <li>               - Bullet list item (• + text)
///   <indent>           - Indent by 4 characters
///   <indent=8>         - Indent by 8 characters
///
/// Notes:
///   - Inline tags can be combined: <b><c=red>bold red text</c></b>
///   - Unknown tags are treated as literal text
///   - Unclosed tags extend to end of string
///   - Color names: red, blue, green, black, white, gray, yellow, etc. (752 X11 colors)
///   - Hex colors: #FF0000 for red, #00FF00 for green, #F0F for magenta, etc.
///   - All font sizes are PROPORTIONAL (100 = base font, 150 = 150% of base)
///   - This ensures proper scaling with adaptive textboxes
std::vector<FormatSegment> ParseFormattedText(const std::string& input, int charWidth = 8);

/// Parse a color from a string (color name or hex #RRGGBB)
/// @param colorStr Color name (e.g., "red", "darkgreen") or hex (e.g., "#FF0000", "#F0F")
/// @param outColor Output PColor object
/// @return true if color was successfully parsed, false otherwise
///
/// Supports all 752 X11 color names via PEBL's PColor system
bool ParseColor(const std::string& colorStr, PColor& outColor);

/// Strip all formatting tags from text, leaving only displayable content
/// @param input Text with formatting tags
/// @return Text with all tags removed
std::string StripFormatting(const std::string& input);

} // namespace FormatParser

#endif // __FORMATPARSER_H__
