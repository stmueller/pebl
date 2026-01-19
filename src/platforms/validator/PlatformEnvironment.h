//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformEnvironment.h
//    Purpose:    Validator Platform Environment (minimal stub)
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//   This file is part of the PEBL project.
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
#ifndef __VALIDATOR_PLATFORMENVIRONMENT_H__
#define __VALIDATOR_PLATFORMENVIRONMENT_H__

#include "../../objects/PEnvironment.h"
#include "../../apps/Globals.h"
#include "../../base/PEBLObject.h"
#include <list>

class PlatformWindow;

/// Headless platform environment - minimal initialization without SDL
/// Used for validation mode where no actual display is needed
class PlatformEnvironment : virtual public PEnvironment, public PEBLObjectBase {
public:
    PlatformEnvironment(PEBLVideoMode mode, PEBLVideoDepth depth,
                        bool windowed, bool resizeable, bool unicode);
    virtual ~PlatformEnvironment();

    // This method initiates everything needed (minimal in validator mode)
    void Initialize();

    virtual bool Draw();
    PEBLVideoMode GetVideoMode() { return mVideoMode; };
    PEBLVideoDepth GetVideoDepth() { return mVideoDepth; };
    bool GetWindowed() { return mWindowed; };

    Variant GetScreenModes(int screen = -1);
    Variant GetResizeable() { return mResizeable; };

    // Overloaded generic PEBLObjectBase methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string) const;

    virtual int ShowCursor(int val);
    virtual int SetCursorPosition(int x, int y);
    virtual Variant GetCursorPosition();
    virtual void SetKeyRepeat(bool onoff);
    virtual Variant GetMouseState();
    virtual bool GetUnicode() { return mUnicode; };

    virtual int GetNumJoysticks();
    virtual Variant GetJoystick(int index);

    // Platform-specific locale queries (validator stubs - return defaults)
    virtual std::string GetSystemLocale() { return ""; }
    virtual bool IsSystemLocaleRTL() { return false; }

protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;

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
