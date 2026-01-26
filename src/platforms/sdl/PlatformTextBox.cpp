//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformTextBox.cpp
//    Purpose:    Contains SDL-specific interface for the text boxes.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003-2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//
//
//     This file is part of the PEBL project.
//
//    PEBL is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    PEBL is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with PEBL; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include "PlatformTextBox.h"
#include "../../objects/PTextBox.h"
#include "PlatformFont.h"
#include "SDLUtility.h"


#include "../../base/PComplexData.h"
#include "../../devices/PKeyboard.h"

#include "../../utility/rc_ptrs.h"
#include "../../utility/PError.h"
#include "../../utility/PEBLUtility.h"
#include "../../utility/FormatParser.h"
#include "../../utility/FontCache.h"


#ifdef PEBL_OSX
#include "SDL.h"
#include "SDL_ttf.h"
#else
#include "SDL.h"
#include "SDL_ttf.h"
#endif
#include "SDL_scancode.h"

#include <stdio.h>
#include <string>
#include <algorithm>
#include <cmath>

//Unicode/utf-8 handling.
#include "../../../libs/utfcpp/source/utf8.h"

// cout removed - use cerr for debug output
using std::cerr;
using std::endl;
using std::flush;
using std::list;
using std::ostream;
using std::string;

// Detect if text contains RTL (Right-to-Left) characters
// Used for auto-adjusting textbox justification
static bool has_rtl_text(const std::string & text) {
    if (!utf8::is_valid(text.begin(), text.end())) {
        return false;
    }

    const unsigned char *bytes = (const unsigned char *)text.c_str();
    while (*bytes) {
        // Hebrew: U+0590 to U+05FF (0xD6 0x90 to 0xD7 0xBF in UTF-8)
        if ((bytes[0] == 0xD6 && bytes[1] >= 0x90) ||
            (bytes[0] == 0xD7 && bytes[1] <= 0xBF)) {
            return true;
        }

        // Arabic: U+0600 to U+06FF (0xD8 0x80 to 0xDB 0xBF in UTF-8)
        if (bytes[0] >= 0xD8 && bytes[0] <= 0xDB) {
            return true;
        }

        // Skip to next codepoint
        if (bytes[0] < 0x80) bytes += 1;
        else if ((bytes[0] & 0xE0) == 0xC0) bytes += 2;
        else if ((bytes[0] & 0xF0) == 0xE0) bytes += 3;
        else if ((bytes[0] & 0xF8) == 0xF0) bytes += 4;
        else bytes += 1;
    }

    return false;
}

// Helper function to determine if a line should be right-justified
// Used in cursor positioning to account for both RTL text and explicit RIGHT justification
static bool is_line_right_justified(const std::string & line_text, const Variant & justify_property) {
    // RTL text is always right-justified
    if (has_rtl_text(line_text)) {
        return true;
    }
    // Check if justification is explicitly set to RIGHT
    if (justify_property == "RIGHT") {
        return true;
    }
    return false;
}


PlatformTextBox::PlatformTextBox(string text, counted_ptr<PEBLObjectBase> font, int width, int height):
    PTextObject(text),  // Must initialize virtual base class directly
    PlatformWidget(),
    PTextBox(text, width, height)


{
    //this records if the text is valid UTF8. IF not (maybe it is iso 8859),
    //we will skip all the utf8 multi-byte stuff.
    mIsUTF8=  utf8::is_valid(text.begin(),text.end());
    mCDT = CDT_TEXTBOX;

    mWidth = width;
    mHeight = height;
    mTextureWidth=mWidth;
    mTextureHeight=mHeight;


    mRenderer = NULL;
    mSurface = NULL;
    mTexture = NULL;
    SetFont(font);
    // Don't call SetText() - text already set by PTextBox(text, width, height) constructor
    // Calling it again during construction causes crashes
    mChanged = true;

    // Note: We need to call FindBreaks() here even without a renderer
    // so that numTextLines and other properties are calculated
    FindBreaks();
}


PlatformTextBox::PlatformTextBox(const PlatformTextBox & text):
    PTextObject(text.GetText()),  // Must initialize virtual base class directly
    PlatformWidget(),
    PTextBox(text.GetText(), (int)(text.GetWidth()), (int)(text.GetHeight()))


{
    mSurface = NULL;
    mTexture = NULL;
    mRenderer = NULL;
    mIsUTF8=  utf8::is_valid(mText.begin(),mText.end());
    mCDT = CDT_TEXTBOX;
    mWidth = text.GetWidth();
    mHeight = text.GetHeight();
    mTextureWidth=mWidth;
    mTextureHeight=mHeight;

    SetFont(text.GetFont());
    mChanged = true;
    // Don't call Draw() here - no renderer yet since object hasn't been added to window
    // Draw() will be called when object is added to window or explicitly drawn
}


///Standard Destructor
PlatformTextBox::~PlatformTextBox()
{

    // PlatformWidget frees mSurface,
}

// Inheritable function that is called by friend method << operator of PComplexData
ostream & PlatformTextBox::SendToStream(ostream& out) const
{
    out << "<SDL PlatformTextBox: [" << mText << "] in " << *GetPlatformFont() << ">" <<flush;
    return out;
}




/// This method should be called when the font is initialized or the text is changed.
/// It will make the mSurface pointer hold the appropriate image.
bool  PlatformTextBox::RenderText()
{

    //free the memory if it is currently pointing at something.
    //#ifdef SDL2_DELETE
    if(mSurface)  SDL_FreeSurface(mSurface);
    //#endif

    //create a new surface on which to render the text.

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x00000000;

#else

    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0x00000000;

#endif

    //we might get a Draw() command before the renderer is set,
    //such as if the text is set before the object is added to a
    //parent that has a renderer.


    if(!mRenderer)
        {
            //cerr << "No renderer " << SDL_GetTicks() << endl;
           
            return  false;
        }

    //cerr << "creating new surface in platformtextbox::rendertext\n";

    //Make a surface of the prescribed size.
    mSurface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                    (int)mTextureWidth,
                                    (int)mTextureHeight, 32,
                                    rmask, gmask, bmask, amask);
    if(!mSurface)  PError::SignalFatalError("Surface not created in TextBox::RenderText.");

    //cerr << "fillingbackground rec platformtextbox::rendertext\n";
    //Fill the box with the background color of the font (from property system in case it was modified)
    PColor bgcolor = GetPlatformFont()->GetBackgroundColor();  // Gets current color from font property system
    SDL_FillRect(mSurface, NULL, SDL_MapRGBA(mSurface->format,
                                             bgcolor.GetRed(),
                                             bgcolor.GetGreen(),
                                             bgcolor.GetBlue(),
                                             bgcolor.GetAlpha()));


    //First, find the height of the text when rendered with the font.
    int height = GetPlatformFont()->GetTextHeight(mText);

    //Now, go through the text letter by letter and word by word until it won't fit on a line any longer.
    unsigned int linestart = 0;
    unsigned int linelength = 0;
    unsigned int totalheight = 0;

    SDL_Surface * tmpSurface=NULL;
    std::vector<int>::iterator i = mBreaks.begin();
    linestart  = 0;

    //cerr << "Textbox: "<< mHeight << " " <<totalheight << endl;

    // Check if formatted text mode is enabled
    Variant formattedVar = PEBLObjectBase::GetProperty("FORMATTED");
    bool isFormatted = (formattedVar.GetInteger() != 0);

    // Parse formatted text once if needed
    std::vector<FormatParser::FormatSegment> segments;
    if (isFormatted) {
        // Calculate character width for indent calculations
        int charWidth = GetPlatformFont()->GetTextWidth("M");  // Use 'M' as average char width
        segments = FormatParser::ParseFormattedText(mText, charWidth);
    }

    while(i != mBreaks.end() && totalheight < (unsigned int) mHeight)
        {


            //mBreaks holds the 'starting' positions of each line.
            linelength = *i - linestart;
            if(linelength>0)
                {

                     SDL_Rect to;

                    // Auto-detect RTL for THIS LINE and adjust justification
                    // For formatted text, use stripped text for line extraction
                    std::string line_text;
                    if (isFormatted) {
                        line_text = mStrippedText.substr(linestart, linelength);
                    } else {
                        line_text = mText.substr(linestart, linelength);
                    }
                    bool isLineRTL = has_rtl_text(line_text);

                    // Determine effective justification: RTL overrides mJustify
                    Variant effectiveJustify;
                    if (isLineRTL) {
                        effectiveJustify = "RIGHT";
                    } else {
                        effectiveJustify = mJustify;
                    }

                    if (isFormatted) {
                        // Cache fonts for this line to avoid repeated creation/deletion
                        // Key: (style, size, color_as_int) -> Value: PlatformFont*
                        std::map<std::tuple<int, int, unsigned int>, PlatformFont*> fontCache;

                        // FIRST PASS: Find maximum ascent, total width, justification for baseline alignment
                        int maxAscent = 0;
                        int maxLineHeight = 0;  // Track maximum font height on this line
                        unsigned int segmentTextPos = 0;
                        bool hasHorizontalRule = false;
                        int lineIndent = 0;  // Track indent for this line
                        int totalLineWidth = 0;  // Total width of all segments on this line
                        FormatParser::Justification lineJustification = FormatParser::JUSTIFY_NONE;  // Line justification

                        // Get base font properties once
                        PlatformFont* baseFont = GetPlatformFont();
                        std::string fontFileName = baseFont->GetFontFileName();
                        int baseFontSize = baseFont->GetFontSize();
                        PColor baseFgColor = baseFont->GetFontColor();
                        PColor baseBgColor = baseFont->GetBackgroundColor();
                        bool antiAliased = baseFont->GetAntiAliased();

                        for (const FormatParser::FormatSegment& seg : segments) {
                            unsigned int segStart = segmentTextPos;
                            unsigned int segEnd = segmentTextPos + seg.text.length();

                            if (segEnd > linestart && segStart < linestart + linelength) {
                                // Check for horizontal rule
                                if (seg.isHorizontalRule) {
                                    hasHorizontalRule = true;
                                }

                                // Track indent (accumulates across segments on same line)
                                if (seg.indentPixels > 0) {
                                    lineIndent = seg.indentPixels;
                                }

                                // Track justification (use first non-NONE justification found on line)
                                if (lineJustification == FormatParser::JUSTIFY_NONE && seg.justification != FormatParser::JUSTIFY_NONE) {
                                    lineJustification = seg.justification;
                                }

                                unsigned int overlapStart = std::max(segStart, (unsigned int)linestart);
                                unsigned int overlapEnd = std::min(segEnd, (unsigned int)(linestart + linelength));
                                std::string segmentText = seg.text.substr(overlapStart - segStart, overlapEnd - overlapStart);

                                if (!segmentText.empty()) {
                                    int segStyle = seg.style;
                                    int segSize = seg.hasSizeOverride ? seg.sizeOverride : baseFontSize;
                                    PColor segFgColor = seg.hasColorOverride ? seg.colorOverride : baseFgColor;

                                    // Create cache key (color encoded as R*256*256 + G*256 + B)
                                    unsigned int colorKey = segFgColor.GetRed() * 65536 +
                                                           segFgColor.GetGreen() * 256 +
                                                           segFgColor.GetBlue();
                                    auto cacheKey = std::make_tuple(segStyle, segSize, colorKey);

                                    // Get or create font
                                    PlatformFont* renderFont;
                                    auto it = fontCache.find(cacheKey);
                                    if (it != fontCache.end()) {
                                        renderFont = it->second;
                                    } else {
                                        renderFont = new PlatformFont(
                                            fontFileName, segStyle, segSize,
                                            segFgColor, baseBgColor, antiAliased);
                                        fontCache[cacheKey] = renderFont;
                                    }

                                    // Get the ascent (height above baseline) for this font
                                    int ascent = TTF_FontAscent(renderFont->GetTTFFont());
                                    if (ascent > maxAscent) maxAscent = ascent;

                                    // Get the full line height (ascent + descent) for this font
                                    int lineHeight = TTF_FontHeight(renderFont->GetTTFFont());
                                    if (lineHeight > maxLineHeight) maxLineHeight = lineHeight;

                                    // Calculate width for this segment
                                    totalLineWidth += renderFont->GetTextWidth(segmentText);
                                }
                            }
                            segmentTextPos += seg.text.length();
                        }

                        // Calculate justification offset
                        int justifyOffset = 0;
                        if (lineJustification == FormatParser::JUSTIFY_CENTER) {
                            justifyOffset = (mWidth - totalLineWidth) / 2;
                        } else if (lineJustification == FormatParser::JUSTIFY_RIGHT) {
                            justifyOffset = mWidth - totalLineWidth;
                        }

                        // SECOND PASS: Render segments with baseline alignment and justification
                        int xOffset = justifyOffset;  // Start with justification offset
                        segmentTextPos = 0;

                        // Render horizontal rule if present
                        if (hasHorizontalRule) {
                            // Draw a horizontal line across the textbox width
                            PColor lineColor = GetPlatformFont()->GetFontColor();
                            int lineY = static_cast<int>(totalheight) + maxAscent / 2;
                            int lineWidth = mWidth - (lineIndent * 2);  // Leave margins

                            // Draw horizontal line (simple rectangle)
                            SDL_Rect hrRect = {lineIndent, lineY, lineWidth, 2};
                            SDL_FillRect(mSurface, &hrRect,
                                        SDL_MapRGBA(mSurface->format,
                                                   lineColor.GetRed(),
                                                   lineColor.GetGreen(),
                                                   lineColor.GetBlue(),
                                                   255));
                        }

                        for (const FormatParser::FormatSegment& seg : segments) {
                            unsigned int segStart = segmentTextPos;
                            unsigned int segEnd = segmentTextPos + seg.text.length();

                            if (segEnd > linestart && segStart < linestart + linelength) {
                                unsigned int overlapStart = std::max(segStart, (unsigned int)linestart);
                                unsigned int overlapEnd = std::min(segEnd, (unsigned int)(linestart + linelength));

                                std::string segmentText = seg.text.substr(
                                    overlapStart - segStart,
                                    overlapEnd - overlapStart);

                                if (!segmentText.empty()) {
                                    // If this segment has an indent, use it as absolute x-position (but only if greater than current xOffset)
                                    if (seg.indentPixels > xOffset) {
                                        xOffset = seg.indentPixels;
                                    }

                                    int segStyle = seg.style;
                                    int segSize = seg.hasSizeOverride ? seg.sizeOverride : baseFontSize;
                                    PColor segFgColor = seg.hasColorOverride ? seg.colorOverride : baseFgColor;

                                    // Create cache key (same as first pass)
                                    unsigned int colorKey = segFgColor.GetRed() * 65536 +
                                                           segFgColor.GetGreen() * 256 +
                                                           segFgColor.GetBlue();
                                    auto cacheKey = std::make_tuple(segStyle, segSize, colorKey);

                                    // Get font from cache (should always exist since we created it in first pass)
                                    PlatformFont* renderFont = fontCache[cacheKey];

                                    tmpSurface = renderFont->RenderText(segmentText.c_str());

                                    // Get ascent for this specific font
                                    int thisAscent = TTF_FontAscent(renderFont->GetTTFFont());

                                    // Calculate y-offset to align baseline with max baseline
                                    int yOffset = maxAscent - thisAscent;

                                    // Position segment with baseline alignment
                                    SDL_Rect segRect = {xOffset, static_cast<int>(totalheight) + yOffset,
                                                       tmpSurface->w, tmpSurface->h};

                                    SDL_BlitSurface(tmpSurface, NULL, mSurface, &segRect);
                                    SDL_FreeSurface(tmpSurface);

                                    xOffset += segRect.w;
                                }
                            }

                            segmentTextPos += seg.text.length();
                        }

                        // Clean up cached fonts for this line
                        for (auto& pair : fontCache) {
                            delete pair.second;
                        }

                        // For formatted text, use the maximum line height found on this line
                        // instead of the base font height
                        totalheight += maxLineHeight;
                    } else {
                        // Normal (non-formatted) rendering
                        if(effectiveJustify == "RIGHT")
                            {
                                //right flush
                                tmpSurface = GetPlatformFont()->RenderText(line_text.c_str());
                                SDL_Rect tmprect = {mSurface->w - tmpSurface->w,
                                                    static_cast<int>(totalheight),
                                                    tmpSurface->w,tmpSurface->h};
                                to = tmprect;
                            }
                        else if (effectiveJustify == "CENTER"){
                            //centered
                                tmpSurface = GetPlatformFont()->RenderText(line_text.c_str());
                                int xval = (mSurface->w - tmpSurface->w)/2;
                                SDL_Rect tmprect = {xval,static_cast<int>(totalheight),
                                                    tmpSurface->w,tmpSurface->h};
                                to = tmprect;
                            }
                        else{
                            // LEFT justification (default)
                                tmpSurface = GetPlatformFont()->RenderText(line_text.c_str());
                                SDL_Rect tmprect = {0,static_cast<int>(totalheight),tmpSurface->w,tmpSurface->h};
                                to = tmprect;
                            }


                        if(0)
                            {
                            //This was originally used for international right-to-left font layout, but
                            //it never worked.
                                std::string tmptext = mText.substr(linestart,linelength);

                                //
                                std::string rtext = PEBLUtility::strrev_utf8(tmptext);



                                //Re-render the text using the associated font.
                                tmpSurface = GetPlatformFont()->RenderText(rtext.c_str());


                                SDL_Rect tmprect = {(mSurface->w - tmpSurface->w),static_cast<int>(totalheight),
                                                    tmpSurface->w, tmpSurface->h};
                                to = tmprect;

                            }


                        SDL_BlitSurface(tmpSurface, NULL, mSurface,&to);
                        SDL_FreeSurface(tmpSurface);

                        // For normal text, use the base font height
                        totalheight += height;
                    }
                }
            else
                {
                    // No line content, still need to increment height for empty lines
                    totalheight += height;
                }

            linestart = *i;
            i++;

        }

    if(mTexture)
        {


            SDL_DestroyTexture(mTexture);
            mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                         (int)mTextureWidth,
                                         (int)mTextureHeight);
            // Enable best quality filtering (anisotropic) for zoomed textures
            SDL_SetTextureScaleMode(mTexture, SDL_ScaleModeBest);
            SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
        }
    else
        {

            mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                         (int)mTextureWidth,(int)mTextureHeight);
            // Enable best quality filtering (anisotropic) for zoomed textures
            SDL_SetTextureScaleMode(mTexture, SDL_ScaleModeBest);
            SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);

        }




    //This will work except for drawing the cursor line on it. instead, create
    //a texture from the surface, then copy it to mtexture.
    //mTexture  = SDL_CreateTextureFromSurface(mRenderer, mSurface);

    SDL_Texture * tmp = SDL_CreateTextureFromSurface(mRenderer, mSurface);
    SDL_FreeSurface(mSurface);
    mSurface = NULL;
    SDL_SetRenderTarget(mRenderer, mTexture);
    SDL_RenderCopy(mRenderer, tmp, NULL, NULL);
    SDL_SetRenderTarget(mRenderer,NULL);
    SDL_DestroyTexture(tmp);

    if(mEditable)
        {
            DrawCursor();
        }


    //If mTexture is null, then rendering failed.
    if(mTexture)
        {

            mChanged = false;
            return true;
        }
    else
        {

            mChanged = true;
            return false;
        }
}


bool PlatformTextBox::SetProperty(std::string name, Variant v)
{

    if(name == "TEXT")
        {
            SetText(v);
        }
    else if(PTextBox::SetProperty(name,v))
    {
        // If we set it at higher level, don't worry.
        // This includes FORMATTED property which is handled in PTextBox
    }
    else if (name == "FONT")
        {
            SetFont(v.GetComplexData()->GetObject());
        }
    else if(name=="WIDTH")
        {
            SetWidth(v);
                }
    else if(name=="HEIGHT")
        {
            SetHeight(v);
        }
    else return false;

    return true;
}

Variant PlatformTextBox::GetProperty(std::string name)const
{
    if(name == "NUMTEXTLINES")
    {
        return Variant((int)mBreaks.size());
    }
    else
    {
        return PTextBox::GetProperty(name);
    }
}

//These shadow higher accessors in widget, because
//they need to set the textchanged flag
void PlatformTextBox::SetHeight(int h)
{
    mTextureHeight = h;
    PTextBox::SetHeight(h);

}

void PlatformTextBox::SetWidth(int w)
{
    mTextureWidth = w;
    PTextBox::SetWidth(w);
}


void PlatformTextBox::SetFont(counted_ptr<PEBLObjectBase> font)
{

    mFontObject = font;

    // Update the FONT property so nested access works correctly
    PComplexData * pcd = new PComplexData(mFontObject);
    PEBLObjectBase::SetProperty("FONT", Variant(pcd));
    delete pcd;

    // Set requestedFontSize when font is set (needed for adaptive textbox scaling)
    // Only set it if it hasn't been set before (is 0)
    Variant currentRequestedSize = PEBLObjectBase::GetProperty("REQUESTEDFONTSIZE");
    if (currentRequestedSize.GetInteger() == 0) {
        int fontSize = GetPlatformFont()->GetFontSize();
        PEBLObjectBase::SetProperty("REQUESTEDFONTSIZE", Variant(fontSize));
    }

    PWidget::SetBackgroundColor(GetPlatformFont()->GetBackgroundColor());
    mChanged = true;
    //Re-render the text onto mSurface
    //if(!RenderText()) cerr << "Unable to render text.\n";

}



void PlatformTextBox::SetText(string text)
{


    //Chain up to parent method.
    PTextObject::SetText(text);
    mIsUTF8=  utf8::is_valid(text.begin(),text.end());

    //mCursorPos = 0;
    mCursorChanged = true;
    mChanged = true;

    // Don't call Draw() here - it will be called when the object is actually drawn
    // Calling Draw() during SetText() can cause crashes if the renderer isn't ready
    // or if this is called during object construction/destruction

    //Re-render the text onto mSurface
    //    if(!RenderText()) cerr << "Unable to render text.\n";

}

void PlatformTextBox::SetEditable(bool val)
{
    // Check if this is a formatted textbox
    Variant formattedVar = PEBLObjectBase::GetProperty("FORMATTED");
    bool isFormatted = (formattedVar.GetInteger() != 0);

    if (val && isFormatted) {
        // Disable formatted mode to allow editing of raw markdown text
        // Tags will become visible as literal text for editing
        std::cerr << "INFO: Disabling formatted mode for editing." << std::endl;
        std::cerr << "  Formatting tags will be visible as text and can be edited directly." << std::endl;

        PEBLObjectBase::SetProperty("FORMATTED", Variant(0));
        mChanged = true;  // Force re-render to show raw tags
    }

    // Call parent implementation
    PTextBox::SetEditable(val);

    // When making a textbox editable, position cursor at end of text
    // This works for all justifications - the rendering code handles visual positioning
    if (val) {
        mCursorPos = mText.length();
        mCursorChanged = true;
    }
}



void PlatformTextBox::FindBreaks()
{

    //First, find the height and width of the text when rendered with the font.
    int height = GetPlatformFont()->GetTextHeight(mText);

    //Set this directly as a property; no need to check with PTextBox, which doesn't do anything with it.
    PEBLObjectBase::SetProperty("LINEHEIGHT",Variant(height));

    //Now, go through the text letter by letter and word by word until
    //it won't fit on a line any longer.

    unsigned int linestart = 0;     //The start of the current line
    unsigned int newlinestart = 0;  //The start of the NEXT line
    unsigned int linelength = 0;
    unsigned int totalheight = 0;

    mBreaks.clear();

    // Check if formatted text mode is enabled
    // If so, we need to work on stripped text for line breaking
    Variant formattedVar = PEBLObjectBase::GetProperty("FORMATTED");
    bool isFormatted = (formattedVar.GetInteger() != 0);

    // For formatted text, we need to find breaks based on displayable text
    int maxIndentPixels = 0;
    int savedWidth = mWidth;
    std::string textForBreaking = mText;  // Text to use for line break calculations

    if (isFormatted) {
        // Calculate character width for indent calculations
        int charWidth = GetPlatformFont()->GetTextWidth("M");  // Use 'M' as average char width

        // Parse formatted text to find maximum indent AND build stripped text
        std::vector<FormatParser::FormatSegment> segments = FormatParser::ParseFormattedText(mText, charWidth);
        mStrippedText.clear();
        for (const FormatParser::FormatSegment& seg : segments) {
            if (seg.indentPixels > maxIndentPixels) {
                maxIndentPixels = seg.indentPixels;
            }
            // Build stripped text by concatenating segment texts
            mStrippedText += seg.text;
        }

        // Reduce available width by maximum indent for line breaking
        // This ensures we account for the space taken by bullets and indents
        if (maxIndentPixels > 0) {
            mWidth = mWidth - maxIndentPixels;
        }

        textForBreaking = mStrippedText;
    }

    //Now, let's reserve space in mBreaks, roughly twice the amount we
    //think we need. This will make adding elements take less time.

    // Temporarily set mText to the text we're using for breaking
    // (FindNextLineBreak uses mText directly)
    std::string originalText = mText;
    mText = textForBreaking;

    int width = GetPlatformFont()->GetTextWidth(mText);
    if (mWidth > 0) {
        mBreaks.reserve(width / mWidth * 2);
    }


    // Calculate line breaks until we either finish the text or exceed the height
    // For adaptive mode, we need ALL breaks to know exact line count
    // For normal mode, we can stop once we know text doesn't fit
    Variant isAdaptiveVar = PEBLObjectBase::GetProperty("ISADAPTIVE");
    bool isAdaptive = isAdaptiveVar.GetInteger() != 0;

    while(newlinestart < mText.size())
        {


            linelength   = FindNextLineBreak(linestart);

            //Increment the placekeepers:
            //This is where the next line will start.

            newlinestart = linestart+ linelength;
            totalheight += height;

            mBreaks.push_back(newlinestart);
            linestart=newlinestart;

            // If not adaptive and we've exceeded height with more text remaining,
            // we know text doesn't fit - no need to calculate more breaks
            if (!isAdaptive && totalheight > (unsigned int)mHeight && newlinestart < mText.size()) {
                break;
            }

        }

    // Restore original text and width
    mText = originalText;
    if (isFormatted) {
        mWidth = savedWidth;
    }

    //Update NUMTEXTLINES property to reflect the number of lines
    PEBLObjectBase::SetProperty("NUMTEXTLINES",Variant((int)mBreaks.size()));

    //Set TEXTCOMPLETE property: 1 if all text rendered, 0 if truncated
    //Text is complete only if we processed all text AND it fits in the height
    bool textComplete = (newlinestart >= mText.size()) && (totalheight <= (unsigned int)mHeight);
    PEBLObjectBase::SetProperty("TEXTCOMPLETE",Variant(textComplete ? 1 : 0));
}


/// This is the 'old' findnextlinebreak, which does not check for valid UTF-8 characters
/// using the utf library.  It will fail to display many CKJ characters, but I'm keeping it here
/// in case of emergency for now.
// #if 0
// int PlatformTextBox::FindNextLineBreakOld(unsigned int curposition)

// {
//     unsigned int sublength = 0;
//     unsigned int lastsep  = 0;
//     unsigned int sep      = 0;
//     std::string tmpstring;

//     //loop through the entire text from curposition on.
//     while (curposition + sublength < mText.size()+1)
//         {


            
//             //Get the width of the line right now.
//             tmpstring = mText.substr(curposition,sublength);
//             int tmpWidth = GetPlatformFont()->GetTextWidth(tmpstring);

//             //  int time1 = SDL_GetTicks();
//             cerr << "................findnextlinebreak: " <<mLineWrap<< " "  << (curposition ) << ":" << (sublength ) << ":  "<< tmpWidth << "***" << ( time1) << endl;

//             //Test to see if curposition is a '10' or a '0' (a hard/explicit line break).  If so, this is a line break.
//             if(mText[curposition + sublength] == 10
//                || mText[curposition+sublength]==0
//                || tmpWidth>=mWidth)
//                 //|| curposition + sublength == mText.size())
//                 {

//                     //If the width of the current line is too big, break at the last separator.
//                     if(tmpWidth >= (unsigned int)(mWidth))
//                         {


//                             //if we are now too long, we should back up to the previous
//                             //break; unless that break was the beginning of the line.  If
//                             //that is the case, back up one character, and crudely break
//                             //within a text line.

//                             //we need to return the best linebreak here.
//                             if(sep >0)
//                                 {

//                                     return sep+1;
//                                 }
//                             if(lastsep>0)
//                                 {

//                                     return lastsep+1;
//                                 }
//                             else
//                                 {


//                                     if(mLineWrap)
//                                         {
//                                             cerr << "linewrap 4\n";
//                                             cerr << "Need to wrap but nowhere to wrap\n" << endl;
//                                             cerr << tmpstring << endl;
//                                             //We have no natural break 'sep' on this line,
//                                             //and the current line is too long.
//                                             while(GetPlatformFont()->GetTextWidth(tmpstring) >(unsigned int)mWidth)
//                                                 {

//                                                     //we need to back off, but
//                                                     //sublength might break a character.
//                                                     sublength--;
//                                                     end = start+sublength;
//                                                     tmpstring = mText.substr(start,end);

//                                                 }

//                                             return sublength;
//                                         }
//                                     else
//                                         {
//                                             //We are past the end of the line, but still need to find the next line break on the text.
//                                             while (curposition + sublength < mText.size()+1)
//                                                 {
//                                                     if(mText[curposition + sublength] == 10
//                                                        || mText[curposition+sublength]==0)

//                                                         return sublength+1;
//                                                     else
//                                                         sublength++;
//                                                 }
//                                             return sublength+2;

//                                         }
//                                 }
//                         }
//                     else
//                         {

//                             return sublength+1;
//                         }
//                 }


//             //if we allow line wrapping, allow lines to wrap at other places too.

//             if(mLineWrap)
//                 {
//                     cerr << "linerwrap\n";

//                     if(mText[curposition + sublength] == ' '
//                        || mText[curposition + sublength] == '-'
//                        || curposition + sublength == mText.size())
//                         {
//                             //either of these are word breaks; potential line breaks.
//                             //Increment word separator holders
//                             lastsep = sep;
//                             sep = sublength ;

//                             tmpstring = mText.substr(curposition, sublength);
//                             //Check the size of the line.
//                             if(GetPlatformFont()->GetTextWidth(tmpstring) > (unsigned int)mWidth)
//                                 {

//                                     //the text is too big for a single line, so return the last word break, but only
//                                     //if the size is greater than 0. In that case, return the current separator, which
//                                     //will not fit on the line, but it will get chopped off.

//                                     if(lastsep != 0)
//                                         {

//                                             return lastsep+1;
//                                         }
//                                     else
//                                         {

//                                             return sep+1;
//                                         }
//                                 }

//                         }


//                 }
//             else{
//                                     cerr << "NO        linerwrap\n";
//             }
//             sublength++;
//         }
//     //The rest of the text must fit in the space allotted; return that number.


//     return sublength-1;
// }

// #endif

//This uses the utf headers to permit handling layout with UTF-8
//encoded text.  The text is maintained in a std::string.  Line breaks
//should return indices of the std::string--not codepoint positions.
//the important thing is that as you move through the string,
//you need to check that the text is legal UTF-8 (and not breaking)
//halfway through a codepoint.  Note that the length of the string is
//not the number of characters in the utf-8 string.

int PlatformTextBox::FindNextLineBreak(unsigned int curposition)
{
    unsigned int sublength = 0;
    unsigned int lastsep  = 0;
    unsigned int sep      = 0;
    std::string tmpstring;


    bool cont = true;
    std::string::iterator start;
    std::string::iterator end;

    //loop through the entire text from curposition on.
    while (curposition + sublength < mText.size()+1)
        {
            //cerr << mText << endl;
            //cerr << curposition << "->" << (curposition+sublength) << "==<" << mText.size() <<endl;
            
            //Get the width of the line right now.
            tmpstring = mText.substr(curposition,sublength);
            int tmpWidth = GetPlatformFont()->GetTextWidth(tmpstring);

            //  int time1 = SDL_GetTicks();


            //Test to see if curposition is a '10' or a '0' (a hard/explicit line break).
            //If so, this is a line break.
            if(mText[curposition + sublength] == 10
               || mText[curposition+sublength]==0
               || tmpWidth>=mWidth)
                //|| curposition + sublength == mText.size())
                {

                    
                    //If the width of the current line is too big, break at the last separator.
                    if(tmpWidth >= (unsigned int)(mWidth))
                        {


                            //if we are now too long, we should back up to the previous
                            //break; unless that break was the beginning of the line.  If
                            //that is the case, back up one character, and crudely break
                            //within a text line.

                            //we need to return the best linebreak here.
                            if(sep >0)
                                {

                                    return sep+1;
                                }
                            if(lastsep>0)
                                {

                                    return lastsep+1;
                                }
                            else
                                {


                                    if(mLineWrap)
                                        {

                                            //We have no natural break 'sep' on this line,
                                            //and the current line is too long.
                                            while(GetPlatformFont()->GetTextWidth(tmpstring) >(unsigned int)mWidth)
                                                {

 
                                                    //we need to back off, but
                                                    //sublength might break a character.
                                                    cont = true;
                                                    start =mText.begin()+curposition;
                                                    end = start+sublength;
                                                    
                                                    while(cont)
                                                        {
                                                            
                                                            sublength--;
                                                            end = start+sublength;
                                                            if(mIsUTF8)
                                                                cont = false;
                                                            else
                                                                cont =  !utf8::is_valid(start,end);
                                                        }
                                                    
                                                    tmpstring = mText.substr(curposition,sublength);
                                                    
                                                    
                                                }

                                            
                                            return sublength;
                                        }
                                    else
                                        {

                                            //No line-wrap here.
                                            //We are past the end of the line, but still need to find
                                            //the next line break on the text.
                                            while (curposition + sublength < mText.size()+1)
                                                {
                                                    
                                                    if(mText[curposition + sublength] == 10
                                                       || mText[curposition+sublength]==0)
                                                        {
                                                            
                                                            
                                                            return sublength+1;
                                                        }
                                                    else
                                                        {
                                                            
                                                            
                                                            cont = true;
                                                            start =mText.begin()+curposition;
                                                            end = start+sublength;

                                                            if(mIsUTF8)
                                                                {
                                                                    //This will advance one legal (utf) character:
                                                                    while(cont)
                                                                        {
                                                                            
                                                                            sublength++;
                                                                            end = start+sublength;
                                                                            cont =  !utf8::is_valid(start,end);
                                                                            
                                                                        }
                                                                }else{
                                                                
                                                                sublength++;
                                                                end=start+sublength;
                                                            }
                                                        }
                                                }
                                            
                                            return sublength+2;
                                            
                                        }
                                }}
                    
                    else  
                        {
                            

                            //originally sublength+1
                            return sublength+1;
                        }
                }
            

            //if we allow line wrapping, allow lines to wrap at other places too.

            
            if(mLineWrap)
                {


                    if(mText[curposition + sublength] == ' '
                       || mText[curposition + sublength] == '-'
                       || curposition + sublength == mText.size())
                        {
                            //either of these are word breaks; potential line breaks.
                            //Increment word separator holders
                            lastsep = sep;
                            sep = sublength ;

                            tmpstring = mText.substr(curposition, sublength);
                            //Check the size of the line.
                            if(GetPlatformFont()->GetTextWidth(tmpstring) > (unsigned int)mWidth)
                                {

                                    //the text is too big for a single line, so return the last word break, but only
                                    //if the size is greater than 0. In that case, return the current separator, which
                                    //will not fit on the line, but it will get chopped off.

                                    if(lastsep != 0)
                                        {

                                            return lastsep+1;
                                        }
                                    else
                                        {

                                            return sep+1;
                                        }
                                }

                        }


                }else{
                //here, we don't need to worry about mid-line linewraps.

                
            }


            
            //This is a 'normal' increment of sublength. Increment until the thing is legal UTF.

            cont = true;
            start =mText.begin()+curposition;
            end = start+sublength;


            while(cont)
                {
                    //   cerr << "Checking:["<< mText.substr(curposition,sublength) << "]" << curposition  << "|" << sublength << ">>" <<  mText.length()<< "\n";
                    sublength++;
                    end = start+sublength;
                    if(mIsUTF8)
                        cont =  !utf8::is_valid(start,end);
                    else
                        cont = false;
                    
                    // cont = cont & (curposition+sublength)< mText.length(); //end when you get to the end of the text.

                }
        }
    //The rest of the text must fit in the space allotted; return that number.
            
            
    return sublength-1;
}




/// Given an x,y position, this will return an integer specifying
/// the character in mText before which the cursor should be n.
/// But, the text may be UTF-8, so an particular character may be several bytes wide
/// so we need to take care to compute cursor position correctly.
/// Cursor position should be related to the byte is the string,
/// but it should not allow half-byte character positions.
int PlatformTextBox::FindCursorPosition(long int x, long int y)
{


    if(mText.length()==0)
        {
            return 0;
        }

    //Find the height of a line.
    int height = GetPlatformFont()->GetTextHeight(mText);

    if(y > mHeight) y = mHeight;
    if(y < 0) y = 0;

    //The line will just be y / height, rounded down
    unsigned long int linenum = y / height;  //this is 0-based.


    //Change the line number to the last one if it is too large.
    //The last element of mBreaks is the 'end' of the text; not really a break for
    //our purposes.

    if(linenum > mBreaks.size())
        linenum = mBreaks.size();


    //find the starting character on the line we care about.
    int startchar;
    if(linenum==0)
        startchar = 0;
    else
        startchar = mBreaks[linenum-1];



    if(startchar >= mBreaks[mBreaks.size()-1])
        {
            return startchar;
        }

    //find the length in bytes of the current line:
    int length = mBreaks[linenum] - startchar;

    // Get the line text to check for RTL
    std::string line_text = mText.substr(startchar, length);

    // For right-justified text, x is the visual position from left edge of textbox
    // We need to convert it to what GetPosition() expects
    unsigned int x_adjusted = x;
    if (is_line_right_justified(line_text, mJustify)) {
        // Get line width to calculate where text starts
        int line_width = GetPlatformFont()->GetTextWidth(line_text);
        int text_start_x = mWidth - line_width;  // Right justification offset

        if ((int)x >= text_start_x) {
            // Click is within the text area
            // For RTL text, GetPosition() expects x from LEFT edge of text,
            // but RTL text renders right-to-left, so we need to flip:
            // Visual position from left of text = (x - text_start_x)
            // For RTL, we need position from RIGHT = line_width - (x - text_start_x)
            if (has_rtl_text(line_text)) {
                x_adjusted = line_width - ((int)x - text_start_x);
            } else {
                // Explicit RIGHT justification with LTR text
                x_adjusted = (int)x - text_start_x;
            }
        } else {
            // Click is to the left of the text (in the padding area)
            // For RTL, this means beginning of text (which is visually on right)
            // For LTR right-justified, this means before text starts
            x_adjusted = has_rtl_text(line_text) ? line_width : 0;
        }
    }

    int charnum = GetPlatformFont()->GetPosition(line_text, x_adjusted);

    //finally, if the current cursor position is a non-printing character (i.e. a carriage return)
    //back up one

    if(!AtPrintableCharacter(charnum + startchar -1))
    {

        if(charnum + startchar > 0)
            charnum--;
    }


    return charnum + startchar;
}


//This will draw a 'cursor' at a specified character.
void PlatformTextBox::DrawCursor()
{
    //Find x and y of position.
    unsigned int x = 0;
    unsigned int y = 0;

    unsigned int height = GetPlatformFont()->GetTextHeight(mText);
    unsigned int width =  (unsigned int)GetWidth();
    unsigned int i = 0;
    int linestart = 0;


    x = width-1;    //Initialize x with the biggest value it can have.
    if(mCursorPos==0)
        {
            // For empty textbox, cursor starts at left for LTR, right for RTL
            if (has_rtl_text(mText)) {
                x = width - 1;  // RTL: cursor starts on the right
            } else {
                x = 0;  // LTR: cursor starts on the left
            }

        } else {
        //The cursor is not at the beginning.
        //mBreaks has found the line breaks already.
            while(i < mBreaks.size())
                {
                    //increment x,y while we go, so that if we get orphaned text,
                    //we still get a decent cursor position.
                    y = i * height;

                    //mBreaks is the position of the first character on each line
                    if(mBreaks[i] >= mCursorPos)
                        {

                            std::string subst = mText.substr(linestart, mCursorPos - linestart);

                            // Check if this line is right-justified (RTL text or explicit RIGHT)
                            int line_end = mBreaks[i];
                            std::string full_line = mText.substr(linestart, line_end - linestart);

                            if (is_line_right_justified(full_line, mJustify)) {
                                // Right-justified: text starts at (width - line_width)
                                // For RTL, cursor is at (line_width - text_before_cursor) from right edge of text
                                // For LTR right-justified, cursor is at text_before_cursor from left edge of text
                                int line_width = GetPlatformFont()->GetTextWidth(full_line);
                                int text_before_cursor_width = GetPlatformFont()->GetTextWidth(subst);
                                int text_start_x = width - line_width;  // RIGHT justification offset

                                if (has_rtl_text(full_line)) {
                                    // RTL: cursor position is flipped
                                    x = text_start_x + (line_width - text_before_cursor_width);
                                } else {
                                    // LTR right-justified: cursor position is normal
                                    x = text_start_x + text_before_cursor_width;
                                }
                                x = ( x >= width ) ? width-1: x;
                            } else {
                                // Left-justified: cursor is positioned from the left edge (original behavior)
                                x =  GetPlatformFont()->GetTextWidth(subst);
                                x = ( x >= width ) ? width-1: x;
                            }
                            break;
                        }

                    linestart = mBreaks[i];
                    i++;
                }
            //x should be accurate, unless the cursor is at the last character,
            //and that last character is a carriage return.


        }

    //The current position should be OK, UNLESS the character at mCursorPos is a CR.
    //Then, we actually want to render CR on the next line.

    // Check bounds before accessing mText[mCursorPos-1]
    if(mCursorPos > 0 && mCursorPos <= (int)mText.size() && mText[mCursorPos-1]== 10)
        {
            y+=height;
            x=1;
        }



    //x,y specifies the top of the cursor.


    // here, if the textbox is attached to the window, things render fine.
    // If the textbox is a child of another widget (a canvas),
    // everything falls apart.


    SDLUtility::DrawLine(mRenderer, this,x, y, x, y+height,
                         (GetPlatformFont()->GetFontColor()));


}



//This overrides the parent Draw() method so that
//things can be re-rendered if necessary.
bool PlatformTextBox::Draw()
{
    // Check if font properties changed and update widget bgcolor if needed
    if(GetPlatformFont()->HasChanged())
    {
        PWidget::SetBackgroundColor(GetPlatformFont()->GetBackgroundColor());
        mChanged = true;
        GetPlatformFont()->ClearChanged();
    }

    // CRITICAL: Always call FindBreaks() when text has changed
    // This was in the backup version and is needed for proper rendering
    if(mChanged)
    {
        FindBreaks();
    }

    if(mChanged)
    {
        // Check if adaptive mode is enabled
        Variant isAdaptiveVar = PEBLObjectBase::GetProperty("ISADAPTIVE");
        if (isAdaptiveVar.GetInteger()) {
            // Track whether this is the first time we're creating an adaptive font
            bool hadAdaptiveFont = (mAdaptiveFontObject.get() != NULL);

            // Get the requested font size (original size before adaptation)
            Variant requestedSizeVar = PEBLObjectBase::GetProperty("REQUESTEDFONTSIZE");
            int requestedSize = requestedSizeVar.GetInteger();

            // If requestedFontSize is 0, it hasn't been set yet - use current font size
            if (requestedSize == 0) {
                requestedSize = GetPlatformFont()->GetFontSize();
                PEBLObjectBase::SetProperty("REQUESTEDFONTSIZE", Variant(requestedSize));
            }

            // Iterate to find the right font size
            // Keep previous adaptive font in mIntermediateFonts during iteration
            int maxIterations = 5;
            int iteration = 0;
            bool needsAdaptation = true;

            while (iteration < maxIterations && needsAdaptation) {
                // Calculate line breaks with current font
                FindBreaks();

                // Check if text currently fits
                Variant textCompleteVar = PEBLObjectBase::GetProperty("TEXTCOMPLETE");
                bool textComplete = textCompleteVar.GetInteger() != 0;

                if (textComplete) {
                    // Text fits - stop iteration
                    needsAdaptation = false;
                    break;
                }

                // Text doesn't fit - calculate target font size
                int minFontSize = 8;
                int currentFontSize = GetPlatformFont()->GetFontSize();
                int targetSize = currentFontSize;

                Variant numLinesVar = PEBLObjectBase::GetProperty("NUMTEXTLINES");
                int numLines = numLinesVar.GetInteger();

                if (numLines > 0 && mHeight > 0 && mWidth > 0) {
                    int lineHeight = GetPlatformFont()->GetTextHeight(mText);

                    // Calculate area of box and area of current text
                    int boxArea = mWidth * mHeight;
                    int textArea = mWidth * (numLines * lineHeight);

                    if (textArea > boxArea) {
                        // On first iteration, use area-based calculation
                        // On subsequent iterations, just decrement by 1
                        if (iteration == 0) {
                            // Text doesn't fit - use area ratio for initial estimate
                            // Font area scales as size², so take square root of area ratio
                            double areaRatio = (double)boxArea / (double)textArea;
                            int estimatedSize = (int)(currentFontSize * std::sqrt(areaRatio));
                            targetSize = std::max(estimatedSize, minFontSize);
                        } else {
                            // Subsequent iterations: decrement by 1 point
                            targetSize = std::max(currentFontSize - 1, minFontSize);
                        }

                        // Safety: ensure we're actually shrinking, never growing
                        if (targetSize >= currentFontSize) {
                            targetSize = std::max(currentFontSize - 1, minFontSize);
                        }
                    }
                }

                // Create new font if we calculated a different size
                if (targetSize != currentFontSize && targetSize >= minFontSize) {
                    std::string fontFileName = GetPlatformFont()->GetFontFileName();
                    int fontStyle = GetPlatformFont()->GetFontStyle();
                    PColor fontColor = GetPlatformFont()->GetFontColor();
                    PColor bgColor = GetPlatformFont()->GetBackgroundColor();
                    bool antiAliased = GetPlatformFont()->GetAntiAliased();

                    // Keep previous adaptive font alive during iteration
                    if (mAdaptiveFontObject.get()) {
                        mIntermediateFonts.push_back(mAdaptiveFontObject);
                    }

                    // Create new font and immediately wrap in counted_ptr
                    PlatformFont* newFont = new PlatformFont(fontFileName, fontStyle, targetSize,
                                                             fontColor, bgColor, antiAliased);
                    counted_ptr<PEBLObjectBase> newFontPtr(newFont);

                    // Set as new adaptive font
                    mAdaptiveFontObject = newFontPtr;

                    // Update widget background color
                    PWidget::SetBackgroundColor(GetPlatformFont()->GetBackgroundColor());
                } else {
                    needsAdaptation = false;
                }

                iteration++;
            }

            // If we created an adaptive font, make sure it's set
            if (mAdaptiveFontObject.get()) {
                mChanged = true;
            }

            // Clear intermediate fonts AFTER rendering is complete
            // This happens later in the function after RenderText() is called
        }

        mChanged = true;
    }

    if(mChanged || mCursorChanged)
    {
        RenderText();
    }

    // Don't clear intermediate fonts - let them persist for the textbox's lifetime
    // They will be cleaned up automatically when the textbox destructor runs
    // This avoids triggering destructor cascades during drawing

    mCursorChanged = false;


   bool ret =  PlatformWidget::Draw();

    if(mEditable&false)
       {
            DrawCursor();
       }


   return ret;
}


//Some key presses can only be handled by the platform-specific code.
//Do this here.
void PlatformTextBox::HandleKeyPress(int keycode, int modkeys, Uint16 unicode)
{
#if 0
    cerr << "handling keypress in PlatformTextBox: " << keycode << endl;
    cerr << "(" <<  PEBL_KEYCODE_RETURN << "|" << PEBL_KEYCODE_RETURN2 << "|" <<          PEBL_KEYCODE_KP_ENTER << ")"<< std::endl;
    cerr << "PEBL_KEYCODE_RIGHT:" << PEBL_KEYCODE_RIGHT << std::endl;
    cerr << "PEBL_KEYCODE_LEFT:" << PEBL_KEYCODE_LEFT << std::endl;
#endif
    
    switch(keycode)
        {
        case PEBL_KEYCODE_UP:
        case PEBL_KEYCODE_DOWN:
            {
                int change;
                if(keycode == PEBL_KEYCODE_UP) change = -1;
                else change = 1;

                //Find x and y of position.
                int x = 0;
                int y = 0;

                int height=GetPlatformFont()->GetTextHeight(mText);
                unsigned int i = 0;
                int linestart = 0;


                while(i < mBreaks.size())
                    {
                        if(mBreaks[i] > mCursorPos)
                            {
                                y = i * height;

                                // Calculate x position (visual position from left edge of textbox)
                                std::string text_before_cursor = mText.substr(linestart, mCursorPos - linestart);

                                // Get the full line to check justification
                                int line_end = mBreaks[i];
                                std::string full_line = mText.substr(linestart, line_end - linestart);

                                if (is_line_right_justified(full_line, mJustify)) {
                                    // Right-justified: text starts at (width - line_width)
                                    // Cursor is at distance (line_width - text_before_cursor_width) from right edge of text
                                    // Visual x = text_start + (line_width - text_before_cursor_width)
                                    //          = (width - line_width) + (line_width - text_before_cursor_width)
                                    //          = width - text_before_cursor_width
                                    int text_before_cursor_width = GetPlatformFont()->GetTextWidth(text_before_cursor);
                                    x = mWidth - text_before_cursor_width;
                                } else {
                                    // Left-justified: x is simply width of text before cursor
                                    x = GetPlatformFont()->GetTextWidth(text_before_cursor);
                                }
                                break;
                            }
                        linestart = mBreaks[i];
                        i++;
                    }


                mCursorPos = FindCursorPosition(x, y + change * height);
                mCursorChanged = true;
                break;
            }
        case PEBL_KEYCODE_RETURN:
        case PEBL_KEYCODE_RETURN2:
        case PEBL_KEYCODE_KP_ENTER:
            {

                std::string lb =  std::string()  + (char(10));

                InsertText(lb);
                //mCursorPos++;
                if(mCursorPos > (int) (mText.length()))
                    mCursorPos = mText.length();

            }
            break;
        case PEBL_KEYCODE_DELETE:

            DeleteText(1);
            break;
        case PEBL_KEYCODE_BACKSPACE:

            DeleteText(-1);
            break;

        case PEBL_KEYCODE_LEFT:
        case PEBL_KEYCODE_RIGHT:
            {
                // For RTL text, LEFT/RIGHT arrows should move in visual direction, not logical
                // Find which line the cursor is on to check if it's RTL
                bool isRTLLine = false;

                if (mBreaks.size() > 0 && mText.length() > 0) {
                    unsigned int i = 0;
                    int linestart = 0;

                    while(i < mBreaks.size()) {
                        if(mBreaks[i] >= mCursorPos) {
                            // Found the current line
                            int line_end = mBreaks[i];
                            std::string current_line = mText.substr(linestart, line_end - linestart);
                            isRTLLine = has_rtl_text(current_line);
                            break;
                        }
                        linestart = mBreaks[i];
                        i++;
                    }
                }

                // In RTL text, swap the behavior:
                // - LEFT arrow moves visually left (logically forward) = IncrementCursor
                // - RIGHT arrow moves visually right (logically backward) = DecrementCursor
                if (isRTLLine) {
                    if (keycode == PEBL_KEYCODE_LEFT) {
                        IncrementCursor();
                    } else {
                        DecrementCursor();
                    }
                } else {
                    // LTR text: normal behavior
                    if (keycode == PEBL_KEYCODE_LEFT) {
                        DecrementCursor();
                    } else {
                        IncrementCursor();
                    }
                }
            }
            break;


            //case PEBL_KEYCODE_BACKSLASH:
            //InsertText("\\");
            //cerr << "backslash\n";
            // InsertText(PEBLUtility::TranslateKeycode((PEBL_Keycode)keycode, modkeys));
            //break;

        default:
            //cerr << "----------------------\n" << keycode << endl;
            //cerr << "["<< (PEBLUtility::TranslateKeyCode(PEBLKey(keycode), modkeys))<<"]"<<endl;
            //cerr << "----------------------\n";


            //    InsertText(SDL_GetKeyName(keycode));
            ;
        }
    mCursorChanged=true;
    //PTextBox::HandleKeyPress(keycode, modkeys, unicode);
}



//Some key presses can only be handled by the platform-specific code.
//Do this here.
void PlatformTextBox::HandleTextInput(std::string input)
{

    PTextBox::HandleTextInput(input);
}


Variant PlatformTextBox::GetLineBreaks()
{
    //Not sure about this, but let's not refind line breaks when this is called--just get
    //whatever has been calculated.

    std::vector<int>::iterator i = mBreaks.begin();

    PList * outlist = new PList();
    while(i != mBreaks.end())
        {
            outlist->PushBack(*i);
            i++;
        }


    counted_ptr<PEBLObjectBase> newList2 = counted_ptr<PEBLObjectBase>(outlist);

    PComplexData * PCD =(new PComplexData(newList2));
    Variant tmp = Variant(PCD);
    delete PCD;
    PCD=NULL;
    return tmp;

}
