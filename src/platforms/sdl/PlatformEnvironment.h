//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformEnvironment.h
//    Purpose:    Contains platform-specific environment initiation
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
#ifndef __PLATFORMENVIRONMENT_H__
#define __PLATFORMENVIRONMENT_H__

//#include "PlatformWindow.h"


#include "../../objects/PEnvironment.h"
#include "../../apps/Globals.h"
#include "../../base/PEBLObject.h"


#ifdef PEBL_OSX
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#elif defined(PEBL_EMSCRIPTEN)
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#else
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#endif

#include <list>

class PlatformWindow;

/// This class initializes the the SDL environment, which may include 
/// the video and audio interfaces, the ttf renderer, etc.
/// 


class PlatformEnvironment: virtual public PEnvironment, public PEBLObjectBase
{
public:
  
    PlatformEnvironment(PEBLVideoMode mode, PEBLVideoDepth depth, 
                        bool windowed,bool resizeable, bool unicode);
    virtual ~PlatformEnvironment();
  

 
    ///This method initiates everything needed to display the main window
    void Initialize();
    
    virtual bool Draw();
    PEBLVideoMode GetVideoMode(){return mVideoMode;};
    PEBLVideoDepth  GetVideoDepth(){return mVideoDepth;};
    bool GetWindowed(){return mWindowed;};
    
    Variant GetScreenModes(int screen=-1);
    Variant GetResizeable(){return mResizeable;};
    //overloaded generic PEBLObjectBase methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
//     virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
//     virtual ObjectValidationError ValidateProperty(std::string)const;
    

    virtual int ShowCursor(int val);
    virtual int SetCursorPosition(int x, int y);
    virtual Variant GetCursorPosition();
    virtual void SetKeyRepeat(bool onoff);
    virtual Variant  GetMouseState();  //gets the buttons too!!!
    virtual bool GetUnicode(){return mUnicode;};

    virtual int GetNumJoysticks();
    virtual Variant GetJoystick(int index);


protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;    

private:
    
    PEBLVideoMode mVideoMode;
    PEBLVideoDepth mVideoDepth;

    bool mWindowed;
    bool mResizeable;
    bool mUnicode;
    int mNumJoysticks;

    void __SetProps__();

};



#endif
