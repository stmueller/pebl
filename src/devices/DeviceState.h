//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/DeviceState.h
//    Purpose:    Manages states of devices for use in the primary event loop.
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
#ifndef __DEVICESTATE_H__
#define __DEVICESTATE_H__

#include <iostream>
#include "PDevice.h"
#include "PEvent.h"

using std::ostream;
using std::cout;

// This is a hypothetical 'state' of a device, which can be tested by a real device
// to see if it is true about that device.

enum DeviceTest 
    {
        DT_NOT_EQUAL             =  0,
        DT_EQUAL                 =  1,
        DT_LESS_THAN             =  2,
        DT_LESS_THAN_OR_EQUAL    =  3,
        DT_GREATER_THAN          =  4,
        DT_GREATER_THAN_OR_EQUAL =  5,
        DT_INSIDE                =  6,
        DT_NOT_OUTSIDE           =  7,
        DT_OUTSIDE               =  8,
        DT_NOT_INSIDE            =  9,
        DT_ON_EDGE               = 10,
        DT_TRUE                  = 11,       //Always returns true
        DT_FALSE                 = 12        //Always returns false
    };


enum DeviceTestType
    {
        DTT_VALUESTATE    ,
        DTT_INTERVALSTATE ,
        DTT_REGIONSTATE   
    };


//This is the generic DeviceState base class.  It is a way to describe a set
//of states of a device could be in, and then test against the actual state
//of the device.
// Subclasses include value state (tests for whether a device has a certain value',
// IntervalState (tests whether a device's interface value lies within some 1-dimensional region,
// and RegionState (tests whether a device's interface value lies within a 2-d region (think mouse location).
// 

class DeviceState
{
public:
    DeviceState(DeviceTest test, int interfaced, PDevice * device, PEBL_DEVICE_TYPE pdt);
    virtual ~DeviceState();
    
    virtual int GetInterface() const;
    virtual int GetState(int iface) const;
    virtual int TestDevice() const;
    PDevice * GetDevice(){return mDevice;};
    PDevice * GetEventQueue();
    virtual PEBL_DEVICE_TYPE GetDeviceType(){return mPDT;};
    //Overload of the << operator
    friend std::ostream & operator <<(ostream & out, const DeviceState & device );

    
protected:
    
    virtual std::ostream & SendToStream(std::ostream & out)const;
    
    /// The 'interface' of a device is one of a number of interfaces a device may have.  
    /// These could include the specific button on a mouse,
    ///  direction of mouse movement (x or y),
    //, a button on a button-box, or a key on the keyboard.
    ///  This is different from the value or values that an interface can take on.
    int mInterface;

    DeviceTest mDeviceTest;
    PDevice * mDevice;

    
    PEBL_DEVICE_TYPE mPDT;  //This keeps track of the ultimate device.  For 
                            //events, this will be different from mDevice, which will be
                            // an eventqueue. 
};


//Determine whether device has a particular value
//Here, mValue
class ValueState: public DeviceState
{
public:
    ValueState(int value, DeviceTest test, int interfaced, PDevice * device, PEBL_DEVICE_TYPE pdt);
    virtual ~ValueState();
    virtual int TestDevice() const; 
   
protected:
    virtual std::ostream &SendToStream(std::ostream & out) const;
private:
    int mValue;
};
    
//Determine whether device has a particular value in a range
class IntervalState: public DeviceState
{
public:
    IntervalState();
    IntervalState(int low, int high, DeviceTest test, int interfaced, PDevice * device, PEBL_DEVICE_TYPE pdt);
    virtual ~IntervalState();
    virtual int TestDevice() const;
protected:
    std::ostream &SendToStream(std::ostream & out) const;
    

private:
    int mLow;
    int mHigh;
    
};

//Determine whether device has a particular value in a 2D range
class RegionState: public DeviceState
{
public:
    RegionState(int lowx, int highx, int lowy, int highy, DeviceTest test, int interfaced, PDevice * device, PEBL_DEVICE_TYPE pdt);
    ~RegionState();
    virtual int TestDevice()const;
protected:
    virtual std::ostream &SendToStream(std::ostream & out) const;
  
private:
    int mLowX;
    int mHighX;
    
    int mLowY;
    int mHighY;
    
};

#endif

