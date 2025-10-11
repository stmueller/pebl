//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformTimer.h
//    Purpose:    Contains the SDL-Specific Timer interface
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
#ifndef __PLATFORMTIMER_H__
#define __PLATFORMTIMER_H__

#include "../../devices/PTimer.h"
#include "SDL.h"
#include <stdio.h>


///  This device is the SDL-Specific interface for two types of platform functions:
///  Querying the actual time, in msec, since the program started (which will wrap after a while)
///  and waiting a specified period of time.
///  It uses SDL function calls for this.  For better accuracy, use system-specific timer calls,


class PlatformTimer: public PTimer
{


public:
    ///The Standard constructor.
    PlatformTimer();

    ///The Standard destructor.
    virtual ~PlatformTimer();

    virtual void Wait(unsigned long int msecs);
    virtual unsigned long int GetTime() const;
    virtual void GetTimeOfDay(unsigned long & secs, unsigned long & msecs);

    virtual int GetState(int iface) const;
    //    virtual int TestDevice(const DeviceState & state)const;

private:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const;
};

#endif
