//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PEnvironment.h
//    Purpose:    Contains generic specs for a PEBL Environment
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
#ifndef __PENVIRONMENT_H__
#define __PENVIRONMENT_H__

#include "../base/PEBLObject.h"
//#include "PWindow.h"
class PWindow;
///
/// This class, a subtype of PEBLObjectBase, initiates the interface environment
/// and maintians all widgets, sounds, etc. subtypes include sounds, visual objects, etc.
///  It is a virtual class that needs to be inherited by a PlatformEnvironment, have
///  and have its methods overridden.

class PEnvironment
{
public:

    PEnvironment():mIsInitialized(false){};
    virtual ~PEnvironment();

    virtual void Initialize()=0;
    virtual bool IsInitialized(){return mIsInitialized;}

    virtual bool AddWindow(PWindow * window);

    virtual PWindow * GetWindow(int index=0);
    //virtual std::string GetExecutableName(){return mExecutableName;};
    //virtual void SetExecutableName(std::string name){mExecutableName=name;};

    //This removes a window from the window list.
    virtual bool RemoveWindow(PWindow * window);

    
protected:
    virtual std::ostream & SendToStream(std::ostream& out) const {return out;};

    bool mIsInitialized;

    //std::string mExecutableName;
    // The main environment can
    // hold multiple PWindows; usually just one, but more if
    // you want separate interfaces for the subject and the experimenter.
    std::list<PWindow*> mWindows;

};


#endif
