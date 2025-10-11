//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PEventQueue.cpp
//    Purpose:    Interface to platform-specific event queue.
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
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  
//    02111-1307  USA
////////////////////////////////////////////////////////////////////////////

#include "PEventQueue.h"
#include "PEvent.h"

using std::cout;
using std::endl;


//This should be moved into PEventQueue
PEventQueue::PEventQueue()
{

}

PEventQueue::~PEventQueue()
{
}



PEBL_DEVICE_TYPE PEventQueue::GetFirstEventType()
{
 
	return  (mEventQueue.front()).GetType();
}


int PEventQueue::GetState(int iface) const
{
 
    return (mEventQueue.front()).GetState(iface);
}


void PEventQueue::PopEvent()
{

	if(!mEventQueue.empty())
        {
            mEventQueue.pop();
        }
}

void PEventQueue::Clear()
{
    Prime();
    while(!mEventQueue.empty())
        {
            mEventQueue.pop();
        }
}

void PEventQueue::PushEvent(PEvent & evt)
{
    mEventQueue.push(evt);
}


PEvent PEventQueue::GetFirstEvent()
{
	return mEventQueue.front();
}

 
 
