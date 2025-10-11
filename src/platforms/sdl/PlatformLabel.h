//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformLabel.h
//    Purpose:    Contains SDL-specific interface for text labels
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2004-2025 Shane T. Mueller <smueller@obereed.net>
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
#ifndef __PLATFORMLABEL_H__
#define __PLATFORMLABEL_H__

#include "PlatformWidget.h"
#include "PlatformFont.h"
#include "../../objects/PLabel.h"
#include "../../utility/rc_ptrs.h"
#include "../../base/PEBLObject.h"
#ifdef PEBL_OSX
#include "SDL.h"
#include "SDL_ttf.h"
#else
#include "SDL.h"
#include "SDL_ttf.h"
#endif


///
/// This class defines the SDL Platform-specific representation of a text label.
/// It uses the SDL_ttf library (a wrapper to freetype) to render words in a box.


class PlatformLabel:  virtual public PlatformWidget,virtual public PLabel
{
public:
    PlatformLabel(const std::string & text, counted_ptr<PEBLObjectBase> font);

    ///This copy constructor isn't const because the font is shared.
    PlatformLabel(PlatformLabel & label);

    virtual ~PlatformLabel();


    virtual bool RenderText();

    //These need to be overridden because the text needs to be re-rendered when they are called.
    virtual bool SetProperty(std::string, Variant v);
    virtual void SetFont(counted_ptr<PEBLObjectBase> font);
    virtual void SetText(const std::string & text);

    virtual bool Draw();

    virtual counted_ptr<PEBLObjectBase> GetFont(){return mFontObject;}
    virtual std::string ObjectName()const{return "PlatformLabel";} ;

protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;

private:

    counted_ptr<PEBLObjectBase> mFontObject;
    //This below is just a shortcut to the font.
    PlatformFont * mFont;
};



#endif
