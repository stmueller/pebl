//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/DeviceState.cpp
//    Purpose:    Data structure allowing a device's state to be tested.
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
#include "PDevice.h"
#include "DeviceState.h"
#include <iostream>

using std::ostream;

DeviceState::DeviceState(DeviceTest test, int intface, PDevice * device, PEBL_DEVICE_TYPE pdt):
    mInterface(intface),
    mDeviceTest(test),
    mDevice(device),
    mPDT(pdt)
{

    
}

DeviceState::~DeviceState()
{

}

int DeviceState::GetInterface() const
{
    return mInterface;
}


int DeviceState::GetState(int iface) const
{
    return false;
}

int DeviceState::TestDevice() const
{
    //cout << "devicestate Dummy device tester\n";
    return true;
}


//Overload of the << operator
ostream & operator <<(ostream & out, const DeviceState & device )
{ 
    device.SendToStream(out);
    return out;
}



ostream & DeviceState::SendToStream(ostream & out) const
{
    out << "<Anonymous DeviceState>" << std::flush;
    return out;
}

ValueState::ValueState(int value, 
                       DeviceTest test, 
                       int intface, 
                       PDevice * device, 
                       PEBL_DEVICE_TYPE pdt):
    DeviceState(test, intface,device, pdt),
    mValue(value)
{
    //cout << "Creating new valuestate; interface:" << mInterface << std::endl;
}

ValueState::~ValueState()
{
}




int ValueState::TestDevice() const
{
    

    int value = mDevice->GetState(mInterface);
    //cout << "test value|" << mValue << "==" <<"device value|"<<value<< std::endl;

    switch(mDeviceTest)
        {

        case DT_NOT_EQUAL:
        case DT_OUTSIDE:
            return value != mValue;

        case DT_EQUAL:
        case DT_NOT_OUTSIDE:
        case DT_ON_EDGE:
            return value == mValue;
            
        case DT_LESS_THAN:
            return value < mValue;

        case DT_LESS_THAN_OR_EQUAL:
            return value <= mValue;

        case DT_GREATER_THAN:
            return value > mValue;

        case DT_GREATER_THAN_OR_EQUAL:
            return value >= mValue;

        case DT_NOT_INSIDE:           //a value is always 'not inside' another value (even if they are the same).
        case DT_TRUE:
            return 1;

        case DT_INSIDE:              //a value is never inside another value (even if they are the same).
        case DT_FALSE:
        default:
            return 0;
            
        }
}


ostream & ValueState::SendToStream(ostream & out) const
{
    out << "<ValueState: " << mValue << "|"<<  mInterface << ">" << std::flush;
    return out;
}



IntervalState::IntervalState(int low, int high, DeviceTest test, int intface, PDevice * device, PEBL_DEVICE_TYPE pdt):
    DeviceState(test, intface, device, pdt),
    mLow(low),
    mHigh(high)
{
}

IntervalState::~IntervalState()
{
}

int IntervalState::TestDevice() const
{


    long int value = mDevice->GetState(mInterface);
    switch(mDeviceTest)
        {

        case DT_NOT_EQUAL:
        case DT_OUTSIDE:
            return value > mHigh || value < mLow;

        case DT_EQUAL:
        case DT_NOT_OUTSIDE:
            return value <= mHigh && value > mLow;

        case DT_NOT_INSIDE:           
            return value <= mLow || value >= mHigh;

        case DT_INSIDE:             
            return value < mHigh && value > mLow;

        case DT_ON_EDGE:
            return value == mLow || value == mHigh;
            
        case DT_LESS_THAN:                //Interpretation: less than the lower bound
            return value < mLow;

        case DT_LESS_THAN_OR_EQUAL:         //Interpretation: less than or equal to the upper bound
            return value <= mHigh;

        case DT_GREATER_THAN:
            return value > mHigh;       //Interpretation: greater than the upper bound

        case DT_GREATER_THAN_OR_EQUAL:      //Interpretation: greater than or equal to the lower  bound
            return value >= mLow;

        case DT_TRUE:
            return 1;
        case DT_FALSE:
        default:
            return 0;
        }
}






ostream & IntervalState::SendToStream(ostream & out) const
{
    out << "<IntervalState: "<< mLow << " to " << mHigh << ">" <<  std::flush;
    return out;
}



RegionState::RegionState(int lowX, int highX, int lowY, int highY, DeviceTest test, int intface, PDevice * device, PEBL_DEVICE_TYPE pdt):
    DeviceState(test, intface, device, pdt),
    mLowX(lowX),
    mHighX(highX),
    mLowY(lowY),
    mHighY(highY)
{
}

RegionState::~RegionState()
{
}



int RegionState::TestDevice() const
{

    int valueX = mDevice->GetState(mInterface);
    int valueY = mDevice->GetState(mInterface+1);

    switch(mDeviceTest)
        {
            
        case DT_NOT_EQUAL:
        case DT_OUTSIDE:
            return valueX > mHighX || valueX < mLowX || valueY > mHighY || valueY < mLowY;
            
        case DT_EQUAL:
        case DT_NOT_OUTSIDE:
            return valueX <= mHighX &&  valueX >= mLowX && valueY <= mHighY && valueY >= mLowY;


        case DT_NOT_INSIDE:           
            return valueX >= mHighX || valueX <= mLowX || valueY >= mHighY || valueY <= mLowY;

        case DT_INSIDE:             
            return valueX < mHighX &&  valueX > mLowX && valueY < mHighY && valueY > mLowY;

        case DT_ON_EDGE:
            return ((valueX == mLowX || valueX == mHighX) && valueY >= mLowY && valueY <= mHighY) ||
                ((valueY == mLowY || valueY == mHighY) && valueX >= mLowX && valueX <= mHighX);
            
        case DT_LESS_THAN:                //Interpretation: less than both lower bounds
            return valueX < mLowX && valueY < mLowY;

        case DT_LESS_THAN_OR_EQUAL:         //Interpretation: less than or equal to both upper bounds
            return valueX <= mHighX && valueY <= mHighY;

        case DT_GREATER_THAN:             //Interpretation: greater than the upper bounds
            return valueX > mHighX && valueY > mHighY;  

        case DT_GREATER_THAN_OR_EQUAL:      //Interpretation: greater than or equal to the lower  bounds
            return valueX >= mLowX && valueY >= mLowY;

        case DT_TRUE:
            return 1;
        case DT_FALSE:
        default:
            return 0;
        }

}


ostream & RegionState::SendToStream(ostream & out) const
{
    out << "<RegionState: x:"<< mLowX << " to " << mHighX <<" y: " << mLowY << " to " << mHighY <<  ">" <<  std::flush;
    return out;
}
