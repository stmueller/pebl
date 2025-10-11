//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformKeyboard.h
//    Purpose:    Contains SDL-specific Keyboard handler
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
#ifndef __PLATFORMKEYBOARD_H__
#define __PLATFORMKEYBOARD_H__

#include "../../devices/PKeyboard.h"
#include "../../base/PEBLObject.h"
#include "SDL.h"

#include <list>


class ValueState;

/// This class instantiates a Keyboard event handler using platform-specific
/// code.

class PlatformKeyboard: public PKeyboard, virtual public PEBLObjectBase
{
public:
  
    PlatformKeyboard();
    virtual ~PlatformKeyboard();
  
    ///This will test whether a specific key is down,
    ///and return true if it is.
    virtual PEBL_Keycode IsKeyDown(PEBL_Keycode key) const;

    ///This will test whether a specific key is up, and
    ///return true if it is.
    virtual bool IsKeyUp(PEBL_Keycode key) const;

    virtual int GetState(int iface) const;

protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;

private:
   
    ///This will determine if any keys are down. It will return
    ///the code of the first key it comes across that is down.
    virtual PEBL_Keycode IsAnyKeyDown() const;
 
    ///This keeps track of the state of the entire keyboard.
    const Uint8 * mKeyboardState;

};


#endif
