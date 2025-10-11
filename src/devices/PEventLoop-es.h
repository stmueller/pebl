//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PEventLoop.h
//    Purpose:    Primary Event Loop.
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
#ifndef __PEVENTLOOP_H__
#define __PEVENTLOOP_H__

#include <iostream>
#include <vector>
#include "DeviceState.h"
#include "PEvent.h"
#include "../utility/rc_ptrs.h"
#include "../base/PEBLObject.h"


class PDevice;
class PList;
class PNode;
class PEventQueue;

/// A EventLoop repetitively scans a number of devices for certain pre-specified states,
/// and executes a PEBL function whenever one is true. 

class PEventLoop
{
public:
  
    PEventLoop();
    ~PEventLoop();
      
    //One way to use the event loop is to register condition states of devices. The
    //loop then examines the devices on each loop, determining if the device is 
    //in some certain state. These methods add and remove such states.
    void RegisterState(DeviceState * state, 
                       const std::string & function, 
                       Variant parameters);

    void RegisterEvent(DeviceState * state, 
                       const std::string & function, 
                       Variant parameters);
    void RemoveState(DeviceState state);

    //Another way to use the event loop is to register conditions associated with 'events'
    //that arrive through the event queue. On each iteration, the event loop tests all such 
    //event tests against the top event in the queue

    //The queue needs to be defined at the platform level, but it is a subclassed PDevice, and
    //so is carried along with the DeviceState test.
    void RegisterEventTest(DeviceState * state, const std::string function, PList * parameters);
    void RemoveEventTest(DeviceState state);
    
    //Clears all state/events tests.
    void Clear();

    ///Initiates the looping tests
    PEvent Loop();


    ///Initiates the looping tests
    PEvent Loop1();

    //Overload of the << operator
    friend std::ostream & operator <<(std::ostream & out, const PEventLoop & loop );

    std::string mTmp; //temporary string.
    bool IsLooping(){return mIsLooping;}

protected:
    

    //These structures keep track of normal states with corresponding
    //function nodes and parameters to execute when true.
    std::vector<DeviceState*> mStates;
    std::vector<PNode *> mNodes;
    int mNumStates;

    std::vector<Variant> mParameters;


    //The following structure keep track of whether the 
    //state is a 'normal' device query test (false) or
    //a special event-queue test (true.)
    std::vector<bool> mIsEvent;
   
private:
   bool mIsLooping;  //allows for reentrant loop.  Currently, not used because we look at gKeepLooping.

};



#endif
