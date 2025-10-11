//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/SDLUtility.h
//    Purpose:    Contains miscellaneous utility functions.
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
#ifndef __SDLUTILITY_H__
#define __SDLUTILITY_H__


#include "../../objects/PColor.h"
#include "SDL.h"
#include "PlatformWindow.h"


/// These are SDL-specific utilities that don't fit into a single class very well.


namespace SDLUtility
{

    
    ///This converts between a PColor and an SDL color
    SDL_Color PColorToSDLColor(PColor pcolor);
    
    ///This converts between an SDL Color and a PColor    
    PColor SDLColorToPColor(SDL_Color scolor);

    ///
    void DrawPixel(SDL_Renderer *renderer, PlatformWidget*pwidget,int x, int y,  PColor color);
    void DrawLine(SDL_Renderer *renderer, PlatformWidget*pwidget,int x1, int y1, int x2, int y2, PColor color);

    Uint32 GetPixel(SDL_Surface *surface, int x, int y);
    PColor GetPixelColor(SDL_Surface *surface, int x, int y);

    
    Uint32 GetPixel(SDL_Renderer *renderer, SDL_Texture * texture,int x, int y);
    PColor GetPixelColor(SDL_Renderer  *renderer,SDL_Texture*texture, int x, int y);

    int WritePNG(SDL_Renderer * renderer, SDL_Rect * rect, const std::string fname);
    //    int WritePNG(SDL_Renderer * renderer, SDL_Rect * rect,const Variant fname);

    Variant GetCurrentScreenResolution();

    void CopyToClipboard(std::string text);
    Variant CopyFromClipboard();
    
    int PopupErrorBox(PlatformWindow * pwindow,const char* message);

    long double GetTimeHP();
    Variant GetDriverList(bool printout=true);
    
}


#endif
