//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformWindow.h
//    Purpose:    Contains platform-specific Main window
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
#ifndef __PLATFORMWINDOW_H__
#define __PLATFORMWINDOW_H__


#include "PlatformWidget.h"
#include "PlatformEnvironment.h"
#include "../../base/PEBLObject.h"
#include "../../objects/PWindow.h"
#include "../../objects/PColor.h"


#include "../../apps/Globals.h"
#include "SDL.h"


/// This class instantiates a PWindow using platform-specific
/// code.
/// 



class PlatformWindow: virtual public PWindow, virtual public PlatformWidget
{

public:
  
    PlatformWindow(PlatformEnvironment * penv);
    ~PlatformWindow();
  
  
    ///This method initiates everything needed to display the main window
    bool Initialize(PEBLVideoMode mode, 
                    PEBLVideoDepth vdepth, 
                    bool windowed,
                    bool resizeable,
                    unsigned int width,
                    unsigned int height);
    
    int GetVideoFlags();

    bool Resize(int w, int h);

    bool Draw();
    long int DrawFor(unsigned int cycles);

    SDL_Window * GetSDLWindow(){return mWindow;};

    int SaveScreenShot(int x, int y, int w, int h,const Variant fname);
    
protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;
    virtual std::string ObjectName() const{return "PlatformWindow";};
    

private:

    int mFlags;
    SDL_Window * mWindow;
};



#endif
