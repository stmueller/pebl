#include "FormatParser.h"
#include "../objects/PColor.h"
#include <cctype>
#include <algorithm>

namespace FormatParser {

/// Helper to convert string to lowercase
static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

/// Parse a color name or hex string into PColor
/// Uses PEBL's existing PColor system for named colors (752 color names)
/// and supports hex codes #RRGGBB or #RGB
bool ParseColor(const std::string& colorStr, PColor& outColor) {
    // Handle hex colors (#RRGGBB or #RGB)
    if (colorStr.length() > 0 && colorStr[0] == '#') {
        std::string hex = colorStr.substr(1);

        // Convert 3-digit hex (#RGB) to 6-digit (#RRGGBB)
        if (hex.length() == 3) {
            hex = std::string(1, hex[0]) + std::string(1, hex[0]) +
                  std::string(1, hex[1]) + std::string(1, hex[1]) +
                  std::string(1, hex[2]) + std::string(1, hex[2]);
        }

        if (hex.length() == 6) {
            try {
                unsigned int r = std::stoi(hex.substr(0, 2), nullptr, 16);
                unsigned int g = std::stoi(hex.substr(2, 2), nullptr, 16);
                unsigned int b = std::stoi(hex.substr(4, 2), nullptr, 16);

                outColor = PColor(r, g, b, 255);
                return true;
            } catch (...) {
                return false;
            }
        }
        return false;
    }

    // Handle named colors using PEBL's existing color system (752 colors)
    try {
        outColor = PColor(colorStr);  // Uses PColor's SetColorByName() internally
        return true;
    } catch (...) {
        // Color name not found
        return false;
    }
}

/// Parse formatted text into segments
std::vector<FormatSegment> ParseFormattedText(const std::string& input, int charWidth) {
    std::vector<FormatSegment> segments;

    // Current formatting state
    bool boldOn = false;
    bool italicOn = false;
    bool underlineOn = false;
    bool colorOn = false;
    PColor currentColor(0, 0, 0, 255);  // Default black
    bool sizeOn = false;
    int currentSize = 0;
    int currentIndent = 0;  // Current indent in pixels
    Justification currentJustification = JUSTIFY_NONE;  // Current text justification

    std::string currentText;
    size_t pos = 0;

    while (pos < input.length()) {
        // Look for tag start
        if (input[pos] == '<') {
            // Save any accumulated text as a segment
            if (!currentText.empty()) {
                FormatSegment seg;
                seg.text = currentText;
                seg.style = (boldOn ? 1 : 0) + (italicOn ? 2 : 0) + (underlineOn ? 4 : 0);
                seg.hasColorOverride = colorOn;
                seg.colorOverride = currentColor;
                seg.hasSizeOverride = sizeOn;
                seg.sizeOverride = currentSize;
                seg.indentPixels = currentIndent;
                seg.justification = currentJustification;
                segments.push_back(seg);
                currentText.clear();
            }

            // Find tag end
            size_t tagEnd = input.find('>', pos);
            if (tagEnd == std::string::npos) {
                // No closing >, treat as literal text
                currentText += input[pos];
                pos++;
                continue;
            }

            // Extract tag content (between < and >)
            std::string tag = input.substr(pos + 1, tagEnd - pos - 1);
            pos = tagEnd + 1;

            // Parse tag
            bool isClosing = (tag.length() > 0 && tag[0] == '/');
            std::string tagName = isClosing ? tag.substr(1) : tag;

            // Extract just the tag name (first word before space or =)
            // This handles both <tag=value> and <tag attr=value> formats
            std::string param;
            size_t spacePos = tagName.find(' ');
            size_t eqPos = tagName.find('=');

            // If there's a space before an =, the tag has attributes (e.g., <p size=80>)
            // Otherwise it's a simple tag with parameter (e.g., <h1=center>)
            if (spacePos != std::string::npos && (eqPos == std::string::npos || spacePos < eqPos)) {
                // Tag with attributes: extract just the tag name, leave rest in tag
                tagName = tagName.substr(0, spacePos);
            } else if (eqPos != std::string::npos) {
                // Simple tag with parameter: extract param
                param = tagName.substr(eqPos + 1);
                tagName = tagName.substr(0, eqPos);
            }

            tagName = toLower(tagName);

            // Process tag
            if (tagName == "br" || tagName == "br/") {
                // Line break - add newline character to current text
                currentText += '\n';

                // Flush segment with newline
                if (!currentText.empty()) {
                    FormatSegment seg;
                    seg.text = currentText;
                    seg.style = (boldOn ? 1 : 0) + (italicOn ? 2 : 0) + (underlineOn ? 4 : 0);
                    seg.hasColorOverride = colorOn;
                    seg.colorOverride = currentColor;
                    seg.hasSizeOverride = sizeOn;
                    seg.sizeOverride = currentSize;
                    seg.indentPixels = currentIndent;
                    seg.justification = currentJustification;
                    segments.push_back(seg);
                    currentText.clear();
                }

                // Reset indent and justification after line break
                currentIndent = 0;
                currentJustification = JUSTIFY_NONE;
            } else if (tagName == "b") {
                boldOn = !isClosing;
            } else if (tagName == "i") {
                italicOn = !isClosing;
            } else if (tagName == "u") {
                underlineOn = !isClosing;
            } else if (tagName == "c") {
                if (isClosing) {
                    colorOn = false;
                } else if (!param.empty()) {
                    if (ParseColor(param, currentColor)) {
                        colorOn = true;
                    }
                }
            } else if (tagName == "size") {
                // DEPRECATED: <size=N> tag (kept for backward compatibility)
                // Now interpreted as proportional size (100 = base font)
                // For absolute point sizes, the old behavior would break with adaptive textboxes
                if (isClosing) {
                    sizeOn = false;
                } else if (!param.empty()) {
                    try {
                        int size = std::stoi(param);
                        if (size > 0 && size < 1000) {  // Sanity check (now percentage)
                            sizeOn = true;
                            currentSize = size;  // Now proportional (100 = base)
                        }
                    } catch (...) {
                        // Invalid size, ignore
                    }
                }
            } else if (tagName == "h1" || tagName == "h2" || tagName == "h3" ||
                       tagName == "h4" || tagName == "h5" || tagName == "h6") {
                // Header tags - shortcuts for bold + proportional size
                // Can optionally include justification: <h1=center>, <h2=right>, etc.
                if (!isClosing) {
                    boldOn = true;
                    sizeOn = true;
                    // Headers use proportional sizing (100 = base font)
                    // H1=230%, H2=200%, H3=170%, H4=140%, H5=130%, H6=115%
                    int level = tagName[1] - '0';  // Convert '1'-'6' to 1-6
                    if (level == 1) currentSize = 230;
                    else if (level == 2) currentSize = 200;
                    else if (level == 3) currentSize = 170;
                    else if (level == 4) currentSize = 140;
                    else if (level == 5) currentSize = 130;
                    else if (level == 6) currentSize = 115;

                    // Parse optional justification parameter
                    if (!param.empty()) {
                        std::string justifyParam = toLower(param);
                        if (justifyParam == "left") currentJustification = JUSTIFY_LEFT;
                        else if (justifyParam == "center") currentJustification = JUSTIFY_CENTER;
                        else if (justifyParam == "right") currentJustification = JUSTIFY_RIGHT;
                    }
                } else {
                    // Closing header tag - reset formatting and add implicit newline
                    boldOn = false;
                    sizeOn = false;
                    currentJustification = JUSTIFY_NONE;

                    // Headers are block-level elements - add newline after closing tag
                    currentText += '\n';
                }
            } else if (tagName == "indent") {
                // Indent tag - sets absolute horizontal position like a tab stop
                // <indent> defaults to 4 characters, <indent=8> sets to 8 character widths from left
                int indentChars = 4;  // Default
                if (!param.empty()) {
                    try {
                        indentChars = std::stoi(param);
                    } catch (...) {
                        // Invalid param, use default
                    }
                }
                currentIndent = indentChars * charWidth;  // Absolute position, not cumulative
            } else if (tagName == "p") {
                // Paragraph tag - sets text justification and/or proportional size
                // NEW syntax: <p align=center>, <p size=150>, <p align=center size=120>
                // OLD syntax (deprecated): <p=center> (still supported for backward compatibility)

                if (isClosing) {
                    // Closing </p> tag - reset paragraph-level formatting
                    sizeOn = false;
                    currentJustification = JUSTIFY_NONE;
                } else {
                    // Opening <p> tag
                    // Check for old syntax first: <p=center>
                    if (!param.empty() && tag.find("align=") == std::string::npos && tag.find("size=") == std::string::npos) {
                        // Old syntax: <p=center> where param is just "center"
                        std::string justifyParam = toLower(param);
                        if (justifyParam == "left") currentJustification = JUSTIFY_LEFT;
                        else if (justifyParam == "center") currentJustification = JUSTIFY_CENTER;
                        else if (justifyParam == "right") currentJustification = JUSTIFY_RIGHT;
                    } else {
                        // New syntax: parse multiple key=value attributes
                        // Extract align= and size= from the tag
                        size_t alignPos = tag.find("align=");
                        if (alignPos != std::string::npos) {
                            size_t alignStart = alignPos + 6;  // Skip "align="
                            size_t alignEnd = tag.find_first_of(" >", alignStart);
                            if (alignEnd == std::string::npos) alignEnd = tag.length();
                            std::string alignValue = toLower(tag.substr(alignStart, alignEnd - alignStart));

                            if (alignValue == "left") currentJustification = JUSTIFY_LEFT;
                            else if (alignValue == "center") currentJustification = JUSTIFY_CENTER;
                            else if (alignValue == "right") currentJustification = JUSTIFY_RIGHT;
                        }

                        size_t sizePos = tag.find("size=");
                        if (sizePos != std::string::npos) {
                            size_t sizeStart = sizePos + 5;  // Skip "size="
                            size_t sizeEnd = tag.find_first_of(" >", sizeStart);
                            if (sizeEnd == std::string::npos) sizeEnd = tag.length();
                            std::string sizeValue = tag.substr(sizeStart, sizeEnd - sizeStart);

                            try {
                                int size = std::stoi(sizeValue);
                                if (size > 0 && size < 1000) {  // Sanity check (percentage)
                                    sizeOn = true;
                                    currentSize = size;  // Proportional (100 = base)
                                }
                            } catch (...) {
                                // Invalid size, ignore
                            }
                        }
                    }
                }
            } else if (tagName == "hr") {
                // Horizontal rule - create special segment with newline so it has a position
                FormatSegment hrSeg;
                hrSeg.isHorizontalRule = true;
                hrSeg.text = "\n";  // Give it a newline so it occupies a position in stripped text
                hrSeg.indentPixels = currentIndent;
                segments.push_back(hrSeg);
                // Reset indent after horizontal rule
                currentIndent = 0;
            } else if (tagName == "li") {
                // Bullet list item - automatically starts on a new line (like HTML <li>)

                // First, flush any current text (without adding newline)
                if (!currentText.empty()) {
                    FormatSegment seg;
                    seg.text = currentText;  // No newline added here
                    seg.style = (boldOn ? 1 : 0) + (italicOn ? 2 : 0) + (underlineOn ? 4 : 0);
                    seg.hasColorOverride = colorOn;
                    seg.colorOverride = currentColor;
                    seg.hasSizeOverride = sizeOn;
                    seg.sizeOverride = currentSize;
                    seg.indentPixels = currentIndent;
                    seg.justification = currentJustification;
                    segments.push_back(seg);
                    currentText.clear();
                }

                // If there are existing segments, ensure the last one ends with a newline
                if (!segments.empty()) {
                    FormatSegment& lastSeg = segments.back();
                    if (!lastSeg.text.empty() && lastSeg.text.back() != '\n') {
                        lastSeg.text += '\n';
                    }
                    currentIndent = 0;  // Reset indent for new line
                }

                // Indent the bullet itself from the left margin
                currentIndent += 2 * charWidth;  // Indent bullet from left margin

                FormatSegment liSeg;
                liSeg.isBulletItem = true;
                liSeg.text = "• ";  // Unicode bullet character
                liSeg.style = (boldOn ? 1 : 0) + (italicOn ? 2 : 0) + (underlineOn ? 4 : 0);
                liSeg.hasColorOverride = colorOn;
                liSeg.colorOverride = currentColor;
                liSeg.hasSizeOverride = sizeOn;
                liSeg.sizeOverride = currentSize;
                liSeg.indentPixels = currentIndent;
                liSeg.justification = currentJustification;
                segments.push_back(liSeg);

                // Indent the following text further
                currentIndent += 2 * charWidth;  // Total indent is now 4 * charWidth
            } else {
                // Unknown tag, treat as literal text
                currentText += '<';
                currentText += (isClosing ? "/" : "");
                currentText += tagName;
                if (!param.empty()) {
                    currentText += '=';
                    currentText += param;
                }
                currentText += '>';
            }
        } else {
            // Regular character, add to current text
            char ch = input[pos];
            currentText += ch;

            // If we hit a newline, flush the current segment and reset indent
            if (ch == '\n') {
                // Save segment with newline
                if (!currentText.empty()) {
                    FormatSegment seg;
                    seg.text = currentText;
                    seg.style = (boldOn ? 1 : 0) + (italicOn ? 2 : 0) + (underlineOn ? 4 : 0);
                    seg.hasColorOverride = colorOn;
                    seg.colorOverride = currentColor;
                    seg.hasSizeOverride = sizeOn;
                    seg.sizeOverride = currentSize;
                    seg.indentPixels = currentIndent;
                    seg.justification = currentJustification;
                    segments.push_back(seg);
                    currentText.clear();
                }
                // Reset indent and justification for next line
                currentIndent = 0;
                currentJustification = JUSTIFY_NONE;
            }

            pos++;
        }
    }

    // Add final segment if there's remaining text
    if (!currentText.empty()) {
        FormatSegment seg;
        seg.text = currentText;
        seg.style = (boldOn ? 1 : 0) + (italicOn ? 2 : 0) + (underlineOn ? 4 : 0);
        seg.hasColorOverride = colorOn;
        seg.colorOverride = currentColor;
        seg.hasSizeOverride = sizeOn;
        seg.sizeOverride = currentSize;
        seg.indentPixels = currentIndent;
        seg.justification = currentJustification;
        segments.push_back(seg);
    }

    // If no segments were created, return a single empty segment
    if (segments.empty()) {
        segments.push_back(FormatSegment());
    }

    return segments;
}

/// Strip all formatting tags from text
std::string StripFormatting(const std::string& input) {
    std::string result;
    size_t pos = 0;

    while (pos < input.length()) {
        if (input[pos] == '<') {
            // Find tag end
            size_t tagEnd = input.find('>', pos);
            if (tagEnd == std::string::npos) {
                // No closing >, treat as literal
                result += input[pos];
                pos++;
                continue;
            }

            // Extract tag content
            std::string tag = input.substr(pos + 1, tagEnd - pos - 1);
            std::string tagName = tag;

            // Handle closing tags
            if (tagName.length() > 0 && tagName[0] == '/') {
                tagName = tagName.substr(1);
            }

            // Handle tags with parameters
            size_t eqPos = tagName.find('=');
            if (eqPos != std::string::npos) {
                tagName = tagName.substr(0, eqPos);
            }

            tagName = toLower(tagName);

            // Check if it's a recognized formatting tag
            if (tagName == "b" || tagName == "i" || tagName == "u" || tagName == "c" ||
                tagName == "size" || tagName == "br" || tagName == "br/" ||
                tagName == "h1" || tagName == "h2" || tagName == "h3" ||
                tagName == "h4" || tagName == "h5" || tagName == "h6" ||
                tagName == "indent" || tagName == "hr" || tagName == "li" || tagName == "p") {
                // Skip this tag (don't add to result)
                // For <br> tags, add a newline instead
                if (tagName == "br" || tagName == "br/") {
                    result += '\n';
                }
                // For <li> tags, add bullet point
                else if (tagName == "li") {
                    result += "• ";
                }
                // For <hr> tags, add a line of dashes
                else if (tagName == "hr") {
                    result += "--------------------\n";
                }
                pos = tagEnd + 1;
            } else {
                // Unknown tag, treat as literal text
                result += input[pos];
                pos++;
            }
        } else {
            // Regular character
            result += input[pos];
            pos++;
        }
    }

    return result;
}

} // namespace FormatParser
