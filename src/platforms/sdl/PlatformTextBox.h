//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformTextBox.h
//    Purpose:    Contains platform-specific interface for text boxes
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
#ifndef __PLATFORMTEXTBOX_H__
#define __PLATFORMTEXTBOX_H__

#include "PlatformWidget.h"
#include "PlatformFont.h"
#include "../../objects/PTextBox.h"

#ifdef PEBL_OSX
#include "SDL.h"
#include "SDL_ttf.h"
#else
#include "SDL.h"
#include "SDL_ttf.h"
#endif

#include "../../utility/rc_ptrs.h"
#include "../../base/PEBLObject.h"

#include <vector>

///
/// This class defines an SDL-specific text box.
///


class PlatformTextBox: virtual  public PlatformWidget, virtual public PTextBox
{
public:
    PlatformTextBox(std::string text, counted_ptr<PEBLObjectBase> font, int width, int height);

    ///This copy constructor isn't const because the font is shared.
    PlatformTextBox(const PlatformTextBox & textbox);

    virtual ~PlatformTextBox();


    virtual bool RenderText();

    virtual bool SetProperty(std::string, Variant v);
    //These need to be overridden because the text needs to be re-rendered when they are called.
    virtual void SetFont(counted_ptr<PEBLObjectBase> font);
    virtual void SetText(std::string  text);
    virtual void SetHeight(int h);
    virtual void SetWidth(int w);

    //This needs to be overridden so that it returns a PlatformFont.
    virtual counted_ptr<PEBLObjectBase> GetFont()const{return mFontObject;}


    virtual bool Draw();
    virtual void HandleKeyPress(int keycode, int modkey);
    virtual void HandleTextInput(std::string input);
    virtual std::string ObjectName()const{return "PlatformTextBox";} ;
    virtual int FindCursorPosition(long int x, long int y);
    virtual void SetEditable(bool val);
    Variant GetLineBreaks();
protected:

    virtual std::ostream & SendToStream(std::ostream& out) const;

    std::vector<int> mBreaks;  ///This stores linebreaks

private:

    void FindBreaks();
    int FindNextLineBreak(unsigned int curposition);

    void DrawCursor();

    counted_ptr<PEBLObjectBase> mFontObject;
    PlatformFont * mFont;
    bool mIsUTF8;

};



#endif
