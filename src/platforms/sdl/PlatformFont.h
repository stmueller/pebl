//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformFont.h
//    Purpose:    Contains platform-specific font class.
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
#ifndef __PLATFORMFONT_H__
#define __PLATFORMFONT_H__

#include "../../objects/PFont.h"
#include "../../objects/PColor.h"
#include "../../utility/rc_ptrs.h"
#include "../../base/PEBLObject.h"

#include "SDLUtility.h"

#ifdef PEBL_OSX
#include "SDL.h"
#include "SDL_ttf.h"
#else
#include "SDL.h"
#include "SDL_ttf.h"
#endif


/// This is a platform-specific font class.  Much of the information is held
/// in the generic PFont class, but this class needs to hold onto an actual 
/// font structure from the font library, and handle the rendering.
///

class PlatformFont:  virtual public PFont
{
    
public:
    //    PlatformFont();
    PlatformFont(const std::string &  filename);
    PlatformFont(const std::string &  filename, int style, int size, PColor fgcolor, PColor bgcolor, bool aa);
    PlatformFont(PlatformFont & font);              ///Copy constructor
    virtual ~PlatformFont();

    virtual void SetFontColor        (PColor color);
    virtual void SetBackgroundColor  (PColor color);
//     virtual PColor GetFontColor      () const {return SDLUtility::SDLColorToPColor(mSDL_FGColor);}
//     virtual PColor GetBackgroundColor() const {return SDLUtility::SDLColorToPColor(mSDL_BGColor);}

    ///This takes care of all the busy work of rendering the text.
    SDL_Surface * RenderText(const std::string & text);
    
    unsigned int GetTextWidth(const std::string & text);
    unsigned int GetTextHeight(const std::string & text);
    unsigned int GetPosition(const std::string & text, unsigned int x);
    virtual std::string ObjectName() const{return "Platform Font";};

protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;

private:
    
    std::string StripText(const std::string & text);

    char* mBuffer;  //buffer to hold font cached memory.
    TTF_Font * mTTF_Font;
    SDL_Color mSDL_FGColor;
    SDL_Color mSDL_BGColor;  
};


#endif
