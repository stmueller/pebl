//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PEven.h
//    Purpose:    Events processed by Event Loop.
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
#ifndef __PEVENTQUEUE_H__
#define __PEVENTQUEUE_H__

#include "PEvent.h"
#include <queue>
#include <iostream>

using std::queue;
using std::cerr;

//entirely on the platform-specific event-handling routines.
class PEventQueue: virtual public PDevice
{
 public:
	PEventQueue();
	virtual ~PEventQueue();

    //This gets the type  of the first event,
	virtual PEBL_DEVICE_TYPE GetFirstEventType();

    //This 'pops' the event at the front of the queue off, destroying it.
    virtual void PopEvent();
    virtual void PushEvent(PEvent & evt);
    //This 'peeks' at the event at the front of the queue
	virtual PEvent GetFirstEvent();

	//This 'primes' the event-queue pump, getting any
	//waiting events from the system, tagging them with a timestamp,
	// and putting them into the PEBL queue
	virtual void Prime(){std::cout << "Standard prime\n";};
    virtual bool IsEmpty(){return mEventQueue.empty();};

    virtual int GetState(int intface) const;

    virtual PEBL_DEVICE_TYPE GetDeviceType(){return PDT_EVENT_QUEUE;};
    virtual void Clear();

 private:

 protected:

	queue<PEvent> mEventQueue;
};





#endif
