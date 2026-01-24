#ifndef __FORMATPARSER_H__
#define __FORMATPARSER_H__

#include <string>
#include <vector>
#include "../objects/PColor.h"

/// FormatParser - Parse simple inline formatting tags in text
/// Supports Phase 1 inline formatting:
///   <b>bold text</b>
///   <i>italic text</i>
///   <c=red>colored text</c>
///   <size=16>sized text</size>
///
/// This is a simple, non-nested tag parser for basic text formatting
/// without requiring external markdown libraries.

namespace FormatParser {

/// Represents one segment of formatted text
/// Each segment has its own text content and formatting properties
struct FormatSegment {
    std::string text;           // The actual text to render
    int style;                  // Font style: 0=normal, 1=bold, 2=italic, 3=bold+italic
    bool hasColorOverride;      // True if color was specified in tag
    PColor colorOverride;       // Color override (if hasColorOverride=true)
    bool hasSizeOverride;       // True if size was specified in tag
    int sizeOverride;           // Font size override (if hasSizeOverride=true)

    FormatSegment() :
        text(""),
        style(0),
        hasColorOverride(false),
        colorOverride(0, 0, 0, 255),  // Default black color
        hasSizeOverride(false),
        sizeOverride(0)
    {
    }
};

/// Parse text with inline formatting tags into segments
/// @param input Text with formatting tags (e.g., "Normal <b>bold</b> text")
/// @return Vector of segments, each with text and formatting properties
///
/// Tag syntax:
///   <b>text</b>        - Bold text
///   <i>text</i>        - Italic text
///   <c=red>text</c>    - Colored text (supports color names and hex #RRGGBB)
///   <size=16>text</size> - Text with specific font size
///
/// Notes:
///   - Tags can be combined: <b><c=red>bold red text</c></b>
///   - Unknown tags are treated as literal text
///   - Unclosed tags extend to end of string
///   - Color names: red, blue, green, black, white, gray, yellow, etc.
///   - Hex colors: #FF0000 for red, #00FF00 for green, etc.
std::vector<FormatSegment> ParseFormattedText(const std::string& input);

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
