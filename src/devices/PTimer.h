//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PTimer.h
//    Purpose:    Primary Timer Event Device.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003 Shane T. Mueller <smueller@umich.edu>
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
#ifndef __PTIMER_H__
#define __PTIMER_H__

#include "PDevice.h"
#include <iostream>


///  This device is the generic interface for two types of platform functions:
///   Querying the actual time, in msec, since the program started (which will wrap after a while)
///   and waiting a specified period of time.
///  It is an abstract base class, and needs to be inherited and implemented on a specific platform to be of any use.


class PTimer: public PDevice
{

public:
    ///The Standard constructor.
    PTimer();

    ///The Standard destructor.
    virtual ~PTimer();

    virtual void Wait(unsigned long int msecs) = 0;
    virtual unsigned long int GetTime() const =0;
    virtual void GetTimeOfDay(unsigned long & secs, unsigned long & msecs)=0;
    virtual int GetState(int iface) const=0;
    virtual PEBL_DEVICE_TYPE GetDeviceType(){return PDT_TIMER;};
private:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const;
};





#endif
