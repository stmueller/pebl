//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformTimer.h
//    Purpose:    Validator Platform Timer (real implementation using chrono)
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
#ifndef __VALIDATOR_PLATFORMTIMER_H__
#define __VALIDATOR_PLATFORMTIMER_H__

#include "../../devices/PTimer.h"
#include <chrono>

/// Headless platform timer implementation using C++ chrono library
/// This is a real implementation, not a stub, since timing is platform-independent
class PlatformTimer : public PTimer {
public:
    PlatformTimer();
    virtual ~PlatformTimer();

    virtual void Wait(unsigned long int msecs);
    virtual void Sleep(unsigned long int msecs);  // OS sleep (yields CPU)
    virtual unsigned long int GetTime() const;
    virtual void GetTimeOfDay(unsigned long & secs, unsigned long & msecs);
    virtual int GetState(int iface) const;

private:
    virtual std::ostream& SendToStream(std::ostream& out) const;
    std::chrono::steady_clock::time_point mStartTime;
};

#endif
