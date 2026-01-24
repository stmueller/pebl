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
std::vector<FormatSegment> ParseFormattedText(const std::string& input) {
    std::vector<FormatSegment> segments;

    // Current formatting state
    bool boldOn = false;
    bool italicOn = false;
    bool colorOn = false;
    PColor currentColor(0, 0, 0, 255);  // Default black
    bool sizeOn = false;
    int currentSize = 0;

    std::string currentText;
    size_t pos = 0;

    while (pos < input.length()) {
        // Look for tag start
        if (input[pos] == '<') {
            // Save any accumulated text as a segment
            if (!currentText.empty()) {
                FormatSegment seg;
                seg.text = currentText;
                seg.style = (boldOn ? 1 : 0) + (italicOn ? 2 : 0);
                seg.hasColorOverride = colorOn;
                seg.colorOverride = currentColor;
                seg.hasSizeOverride = sizeOn;
                seg.sizeOverride = currentSize;
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

            // Handle tags with parameters (e.g., c=red, size=16)
            std::string param;
            size_t eqPos = tagName.find('=');
            if (eqPos != std::string::npos) {
                param = tagName.substr(eqPos + 1);
                tagName = tagName.substr(0, eqPos);
            }

            tagName = toLower(tagName);

            // Process tag
            if (tagName == "br" || tagName == "br/") {
                // Line break - add newline character to current text
                currentText += '\n';
            } else if (tagName == "b") {
                boldOn = !isClosing;
            } else if (tagName == "i") {
                italicOn = !isClosing;
            } else if (tagName == "c") {
                if (isClosing) {
                    colorOn = false;
                } else if (!param.empty()) {
                    if (ParseColor(param, currentColor)) {
                        colorOn = true;
                    }
                }
            } else if (tagName == "size") {
                if (isClosing) {
                    sizeOn = false;
                } else if (!param.empty()) {
                    try {
                        int size = std::stoi(param);
                        if (size > 0 && size < 200) {  // Sanity check
                            sizeOn = true;
                            currentSize = size;
                        }
                    } catch (...) {
                        // Invalid size, ignore
                    }
                }
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
            currentText += input[pos];
            pos++;
        }
    }

    // Add final segment if there's remaining text
    if (!currentText.empty()) {
        FormatSegment seg;
        seg.text = currentText;
        seg.style = (boldOn ? 1 : 0) + (italicOn ? 2 : 0);
        seg.hasColorOverride = colorOn;
        seg.colorOverride = currentColor;
        seg.hasSizeOverride = sizeOn;
        seg.sizeOverride = currentSize;
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
            if (tagName == "b" || tagName == "i" || tagName == "c" ||
                tagName == "size" || tagName == "br" || tagName == "br/") {
                // Skip this tag (don't add to result)
                // For <br> tags, add a newline instead
                if (tagName == "br" || tagName == "br/") {
                    result += '\n';
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
