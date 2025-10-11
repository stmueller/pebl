//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformCanvas.h
//    Purpose:    Contains platform-specific interface for a 'canvas'
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2010 Shane T. Mueller <smueller@obereed.net>
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
#ifndef __PLATFORMCANVAS_H__
#define __PLATFORMCANVAS_H__

#include "PlatformWidget.h"
#include "../../objects/PCanvas.h"
#include "SDL.h"
#include "../../utility/rc_ptrs.h"
#include "../../base/PEBLObject.h"

#include <vector>

///
/// This class defines an SDL-specific drawable canvas in which
/// individual pixels can be drawn to.


class PlatformCanvas: virtual  public PlatformWidget, virtual public PCanvas
{
public:

    PlatformCanvas(int width, int height, Variant bg);
    PlatformCanvas(int width, int height);

    ///This copy constructor isn't const because the font is shared.
    PlatformCanvas(const PlatformCanvas & canv);

    virtual ~PlatformCanvas();


    virtual bool SetProperty(std::string, Variant v);
    //These need to be overridden because the text needs to be re-rendered when they are called.

    virtual bool Reset();
    virtual std::string ObjectName()const{return "PlatformCanvas";} ;
    virtual bool Draw();
    virtual void SetWidth(pInt h);
    virtual void SetHeight(pInt h);

    virtual void SetZoomX(double h);
    virtual void SetZoomY(double h);

protected:

    virtual std::ostream & SendToStream(std::ostream& out) const;


private:

    //unstretched canvas size:

    int mCanvasWidth;
    int mCanvasHeight;
    Uint32 * mPixels;

};



#endif
