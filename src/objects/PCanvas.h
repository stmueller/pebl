//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PCanvas.h
//    Purpose:    Contains specs for a drawable canvas
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2010-2025 Shane T. Mueller <smueller@obereed.net>
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
#ifndef __PCANVAS_H__
#define __PCANVAS_H__

#include "../utility/rc_ptrs.h"
#include "PWidget.h"
#include "PColor.h"


///
/// This class is the basic generic text box.

class PCanvas: virtual public PWidget
{
public:
    PCanvas();
    PCanvas(int width, int height, Variant  color);
    PCanvas(int width, int height);
    PCanvas(PCanvas & canvas);
    virtual ~PCanvas();


    //overloaded generic PObject methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;
    
    virtual void SetPosition(pInt x, pInt y);
    virtual void SetHeight(int h);
    virtual void SetWidth(int w);
    virtual void SetColor(Variant col);
    virtual std::string ObjectName() const;

    virtual bool GetDrawBackground()const{return mDrawBackground;};
private:


protected:

    bool mDrawBackground;
    bool mReset;
    virtual std::ostream & SendToStream(std::ostream& out) const{return out;};
    
};


#endif

