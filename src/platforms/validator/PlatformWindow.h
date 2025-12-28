//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformWindow.h
//    Purpose:    Validator Platform Window (stub - no display)
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
#ifndef __VALIDATOR_PLATFORMWINDOW_H__
#define __VALIDATOR_PLATFORMWINDOW_H__

#include "../../objects/PWindow.h"
#include "PlatformWidget.h"

class PlatformEnvironment;

/// Headless platform window - no actual display
/// Used for validation mode where no window is needed
class PlatformWindow : virtual public PWindow, virtual public PlatformWidget {
public:
    PlatformWindow(PlatformEnvironment* env);
    virtual ~PlatformWindow();

    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformWindow (validator)"; }
    virtual int SaveScreenShot(int x, int y, int w, int h, const Variant fname) { return 0; }
    virtual bool Initialize(PEBLVideoMode mode, PEBLVideoDepth depth, bool windowed, bool resizeable, unsigned int width, unsigned int height) { return true; }
    virtual bool Resize(int w, int h) { return true; }
    virtual long int DrawFor(unsigned int cycles) { return 0; }

    // Resolve ambiguity by explicitly delegating to PWindow
    virtual bool SetProperty(std::string name, Variant v) {
        return PWindow::SetProperty(name, v);
    }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const {
        return PWindow::ValidateProperty(name, v);
    }
    virtual ObjectValidationError ValidateProperty(std::string name) const {
        return PWindow::ValidateProperty(name);
    }

    // Resolve GetWidth/GetHeight ambiguity
    virtual pInt GetWidth() const { return PWindow::GetWidth(); }
    virtual pInt GetHeight() const { return PWindow::GetHeight(); }

    // Resolve SetPosition/SetBackgroundColor ambiguity
    virtual void SetPosition(pInt x, pInt y) { PWindow::SetPosition(x, y); }
    virtual void SetBackgroundColor(const PColor &color) { PWindow::SetBackgroundColor(color); }

protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

#endif
