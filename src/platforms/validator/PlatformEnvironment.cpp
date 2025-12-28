//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformEnvironment.cpp
//    Purpose:    Validator Platform Environment Implementation
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
#include "PlatformEnvironment.h"
#include "../../base/PComplexData.h"
#include "../../base/PList.h"

PlatformEnvironment::PlatformEnvironment(PEBLVideoMode mode, PEBLVideoDepth depth,
                                         bool windowed, bool resizeable, bool unicode)
    : mVideoMode(mode), mVideoDepth(depth), mWindowed(windowed),
      mResizeable(resizeable), mUnicode(unicode), mNumJoysticks(0) {
    __SetProps__();
}

PlatformEnvironment::~PlatformEnvironment() {
}

void PlatformEnvironment::Initialize() {
    // Minimal initialization - no SDL needed
    mIsInitialized = true;
}

bool PlatformEnvironment::Draw() {
    // No-op: no actual rendering in validator mode
    return true;
}

Variant PlatformEnvironment::GetScreenModes(int screen) {
    // Return empty list - no screens in validator mode
    PList* returnlist = new PList();
    counted_ptr<PEBLObjectBase> tmp = counted_ptr<PEBLObjectBase>(returnlist);
    PComplexData* pcd = new PComplexData(tmp);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    return tmp3;
}

bool PlatformEnvironment::SetProperty(std::string name, Variant v) {
    // Store property but don't do anything with it
    return PEBLObjectBase::SetProperty(name, v);
}

Variant PlatformEnvironment::GetProperty(std::string name) const {
    return PEBLObjectBase::GetProperty(name);
}

int PlatformEnvironment::ShowCursor(int val) {
    // No-op: no cursor in validator mode
    return 0;
}

int PlatformEnvironment::SetCursorPosition(int x, int y) {
    // No-op
    return 0;
}

Variant PlatformEnvironment::GetCursorPosition() {
    // Return [0, 0] - no cursor position
    PList* returnlist = new PList();
    returnlist->PushBack(Variant(0));
    returnlist->PushBack(Variant(0));
    counted_ptr<PEBLObjectBase> tmp = counted_ptr<PEBLObjectBase>(returnlist);
    PComplexData* pcd = new PComplexData(tmp);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    return tmp3;
}

void PlatformEnvironment::SetKeyRepeat(bool onoff) {
    // No-op
}

Variant PlatformEnvironment::GetMouseState() {
    // Return [0, 0, [0, 0, 0, 0, 0]] - no mouse state
    PList* buttons = new PList();
    for (int i = 0; i < 5; i++) {
        buttons->PushBack(Variant(0));
    }
    counted_ptr<PEBLObjectBase> tmpButtons = counted_ptr<PEBLObjectBase>(buttons);

    PList* returnlist = new PList();
    returnlist->PushBack(Variant(0));  // x
    returnlist->PushBack(Variant(0));  // y
    returnlist->PushBack(Variant(new PComplexData(tmpButtons)));  // buttons

    counted_ptr<PEBLObjectBase> tmp = counted_ptr<PEBLObjectBase>(returnlist);
    PComplexData* pcd = new PComplexData(tmp);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    return tmp3;
}

int PlatformEnvironment::GetNumJoysticks() {
    return 0;  // No joysticks in validator mode
}

Variant PlatformEnvironment::GetJoystick(int index) {
    // Return null variant - no joysticks
    return Variant();
}

std::ostream& PlatformEnvironment::SendToStream(std::ostream& out) const {
    out << "PlatformEnvironment (validator)";
    return out;
}

void PlatformEnvironment::__SetProps__() {
    // Set object properties for introspection
    SetProperty("VIDEOMODE", Variant((int)mVideoMode));
    SetProperty("VIDEODEPTH", Variant((int)mVideoDepth));
    SetProperty("WINDOWED", Variant(mWindowed));
    SetProperty("RESIZEABLE", Variant(mResizeable));
    SetProperty("UNICODE", Variant(mUnicode));
}
