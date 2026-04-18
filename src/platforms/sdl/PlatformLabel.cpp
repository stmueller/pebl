//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformLabel.cpp
//    Purpose:    Contains SDL-specific visual representation of a word
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2004-2026 Shane T. Mueller <smueller@obereed.net>
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
#include "PlatformLabel.h"
#include "../../objects/PLabel.h"
#include "PlatformFont.h"
#include "../../utility/rc_ptrs.h"
#include "../../utility/PError.h"
#include "../../base/PComplexData.h"
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

#include <stdio.h>
#include <algorithm>

// cout removed - use cerr for debug output
using std::cerr;
using std::endl;
using std::flush;
using std::list;
using std::ostream;



PlatformLabel::PlatformLabel(const std::string & text, counted_ptr<PEBLObjectBase> font):
    PlatformWidget(),
    PLabel(text),
    mFontObject(font)
{

    mSurface = NULL;
    mTexture = NULL;
    mRenderer = NULL;

    mCDT=CDT_LABEL;

    SetFont(font);

    SetText(text);

    //Make the font property accessible
    counted_ptr<PEBLObjectBase> myFont = counted_ptr<PEBLObjectBase>(mFontObject);

    PComplexData *  pcd = new PComplexData(myFont);

    InitializeProperty("FONT",Variant(pcd));

    delete pcd;
    pcd=NULL;

    InitializeProperty("WIDTH",Variant(0));
    InitializeProperty("HEIGHT",Variant(0));

    //issue draw() command to be sure the xy center is set right.
    Draw();

}


PlatformLabel::PlatformLabel(PlatformLabel & label):
    PlatformWidget(),
    PLabel(label.GetText())

{
    mCDT=CDT_LABEL;
    mSurface = NULL;
    mRenderer = NULL;
    mTexture = NULL;

    SetFont(label.GetFont());


    counted_ptr<PEBLObjectBase> myFont = counted_ptr<PEBLObjectBase>(mFontObject);
    PComplexData *  pcd = new PComplexData(myFont);
    delete pcd;
    pcd=NULL;

    InitializeProperty("FONT",Variant(pcd));
    InitializeProperty("WIDTH",Variant(label.GetHeight()));
    InitializeProperty("HEIGHT",Variant(label.GetWidth()));
    cerr << "Drawing new label in platformlabel\n";
    Draw();
}


///Standard Destructor
PlatformLabel::~PlatformLabel()
{
    //cerr << "Label destructor\n";
 
}


// Inheritable function that is called by friend method << operator of PComplexData
ostream & PlatformLabel::SendToStream(ostream& out) const
{
    out << "<SDL PlatformLabel: " << mText << " in " << *GetPlatformFont() << ">" <<flush;
    return out;
}

 

/// RenderText is only called once the label is already added to
/// a window/widget, so that it already has a renderer and a texture.

bool  PlatformLabel::RenderText()
{
    // Check if formatted text mode is enabled
    Variant formattedVar = PEBLObjectBase::GetProperty("FORMATTED");
    bool isFormatted = (formattedVar.GetInteger() != 0);

    SDL_Surface * finalSurface = NULL;

    if (isFormatted) {
        // FORMATTED TEXT RENDERING (single-line with baseline alignment)

        // Parse formatted text into segments
        std::vector<FormatParser::FormatSegment> segments = FormatParser::ParseFormattedText(mText);

        // FIRST PASS: Find maximum ascent for baseline alignment
        int maxAscent = 0;
        int totalWidth = 0;

        for (const FormatParser::FormatSegment& seg : segments) {
            if (!seg.text.empty()) {
                PlatformFont* segFont = GetPlatformFont();
                std::string fontFileName = segFont->GetFontFileName();
                int baseSize = segFont->GetFontSize();
                PColor baseFgColor = segFont->GetFontColor();
                PColor baseBgColor = segFont->GetBackgroundColor();
                bool antiAliased = segFont->GetAntiAliased();

                int segStyle = seg.style;
                int segSize = seg.hasSizeOverride ? seg.sizeOverride : baseSize;
                PColor segFgColor = seg.hasColorOverride ? seg.colorOverride : baseFgColor;

                PlatformFont* renderFont = new PlatformFont(
                    fontFileName, segStyle, segSize,
                    segFgColor, baseBgColor, antiAliased);

                // Get ascent and width for this segment
                int ascent = TTF_FontAscent(renderFont->GetTTFFont());
                if (ascent > maxAscent) maxAscent = ascent;

                int segWidth, segHeight;
                TTF_SizeText(renderFont->GetTTFFont(), seg.text.c_str(), &segWidth, &segHeight);
                totalWidth += segWidth;

                delete renderFont;
            }
        }

        // Calculate total height (use max ascent + max descent)
        // Note: TTF_FontDescent() returns negative values for portions below baseline
        int maxDescent = 0;
        for (const FormatParser::FormatSegment& seg : segments) {
            if (!seg.text.empty()) {
                PlatformFont* segFont = GetPlatformFont();
                std::string fontFileName = segFont->GetFontFileName();
                int baseSize = segFont->GetFontSize();
                PColor baseFgColor = segFont->GetFontColor();
                PColor baseBgColor = segFont->GetBackgroundColor();
                bool antiAliased = segFont->GetAntiAliased();

                int segStyle = seg.style;
                int segSize = seg.hasSizeOverride ? seg.sizeOverride : baseSize;
                PColor segFgColor = seg.hasColorOverride ? seg.colorOverride : baseFgColor;

                PlatformFont* renderFont = new PlatformFont(
                    fontFileName, segStyle, segSize,
                    segFgColor, baseBgColor, antiAliased);

                int descent = TTF_FontDescent(renderFont->GetTTFFont());
                // Convert negative descent to positive for comparison
                int absDescent = (descent < 0) ? -descent : descent;
                if (absDescent > maxDescent) maxDescent = absDescent;

                delete renderFont;
            }
        }

        int totalHeight = maxAscent + maxDescent;

        // Create surface for the entire label
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            Uint32 rmask = 0xff000000;
            Uint32 gmask = 0x00ff0000;
            Uint32 bmask = 0x0000ff00;
            Uint32 amask = 0x000000ff;
        #else
            Uint32 rmask = 0x000000ff;
            Uint32 gmask = 0x0000ff00;
            Uint32 bmask = 0x00ff0000;
            Uint32 amask = 0xff000000;
        #endif

        finalSurface = SDL_CreateRGBSurface(0, totalWidth, totalHeight, 32,
                                            rmask, gmask, bmask, amask);

        if (!finalSurface) {
            return false;
        }

        // Fill with transparent background
        SDL_FillRect(finalSurface, NULL, SDL_MapRGBA(finalSurface->format, 0, 0, 0, 0));

        // SECOND PASS: Render segments with baseline alignment
        int xOffset = 0;

        for (const FormatParser::FormatSegment& seg : segments) {
            if (!seg.text.empty()) {
                PlatformFont* segFont = GetPlatformFont();
                std::string fontFileName = segFont->GetFontFileName();
                int baseSize = segFont->GetFontSize();
                PColor baseFgColor = segFont->GetFontColor();
                PColor baseBgColor = segFont->GetBackgroundColor();
                bool antiAliased = segFont->GetAntiAliased();

                int segStyle = seg.style;
                int segSize = seg.hasSizeOverride ? seg.sizeOverride : baseSize;
                PColor segFgColor = seg.hasColorOverride ? seg.colorOverride : baseFgColor;

                PlatformFont* renderFont = new PlatformFont(
                    fontFileName, segStyle, segSize,
                    segFgColor, baseBgColor, antiAliased);

                SDL_Surface* segSurface = renderFont->RenderText(seg.text.c_str());

                if (segSurface) {
                    // Get ascent for this specific font
                    int thisAscent = TTF_FontAscent(renderFont->GetTTFFont());

                    // Calculate y-offset to align baseline with max baseline
                    int yOffset = maxAscent - thisAscent;

                    // Position segment with baseline alignment
                    SDL_Rect segRect = {xOffset, yOffset, segSurface->w, segSurface->h};

                    SDL_BlitSurface(segSurface, NULL, finalSurface, &segRect);
                    SDL_FreeSurface(segSurface);

                    xOffset += segRect.w;
                }

                delete renderFont;
            }
        }

    } else {
        // NORMAL (NON-FORMATTED) RENDERING
        if(mDirection == 1) {
            //Re-render the text using the associated font.
            finalSurface = GetPlatformFont()->RenderText(mText.c_str());
        } else {
            std::string rtext = PEBLUtility::strrev_utf8(mText);
            //Re-render the text using the associated font.
            finalSurface = GetPlatformFont()->RenderText(rtext.c_str());
        }
    }

    if(finalSurface) {
        mWidth  = finalSurface->w;
        mHeight = finalSurface->h;
        mTextureWidth = mWidth;
        mTextureHeight = mHeight;
        InitializeProperty("HEIGHT",mHeight);
        InitializeProperty("WIDTH",mWidth);

        //textures get created
        if(mTexture) {
            SDL_DestroyTexture(mTexture);
            mTexture = NULL;
        }

        if(mRenderer) {
            mTexture = SDL_CreateTextureFromSurface(mRenderer, finalSurface);
            // Enable alpha blending for transparency support
            SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
            // Enable best quality filtering (anisotropic) for zoomed textures
            SDL_SetTextureScaleMode(mTexture, SDL_ScaleModeBest);

            SDL_FreeSurface(finalSurface);
            finalSurface = NULL;
        }

        return true;
    } else {
        return false;
    }
}


bool PlatformLabel::SetProperty(std::string name, Variant v)
{

    if(name == "TEXT")
        {

            SetText(v);
        }
    else if(PLabel::SetProperty(name,v))
    {
        // If we set it at higher level, don't worry.
    }
    else if (name == "FONT")
        {
            SetFont(v.GetComplexData()->GetObject());
            
        }
    else return false;
    
    return true;
}

void PlatformLabel::SetFont(counted_ptr<PEBLObjectBase> font)
{

    mFontObject = font;

    // Update the FONT property so nested access works correctly
    PComplexData * pcd = new PComplexData(mFontObject);
    PEBLObjectBase::SetProperty("FONT", Variant(pcd));
    delete pcd;

    mChanged =true;
    Draw();

    //if(!RenderText()) cerr << "Unable to render text.\n";

}


void PlatformLabel::SetText(const std::string & text)
{
    //Chain up to parent method.
    PTextObject::SetText(text);
    mChanged =true;
    Draw();

    //Re-render the text onto mSurface
    //if(!RenderText()) cerr << "Unable to render text.\n";

}


bool PlatformLabel::Draw()
{

    // Check if label text changed OR if font properties changed
    if(mChanged || GetPlatformFont()->HasChanged())
        {

            RenderText();
            //Reposition.  This just recalculates so things are centered
            //correctly; labels are positioned based on their center.

            SetPosition(mX, mY);

            InitializeProperty("HEIGHT",mHeight);
            InitializeProperty("WIDTH",mWidth);
            GetPlatformFont()->ClearChanged();  // Clear font changed flag after re-rendering

            // Only clear mChanged if texture was successfully created
            // If mTexture is NULL, keep mChanged=true to retry when renderer is available
            if(mTexture)
            {
                mChanged = false;
            }
        }
    return  PlatformWidget::Draw();

}
