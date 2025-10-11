//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PDevice.h
//    Purpose:    Master Generic Device Class
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
#ifndef __PDEVICE_H__
#define __PDEVICE_H__

#include <iostream>
using std::cerr;
class DeviceState;
class ValueState;
class IntervalState;
class RegionState;



enum PEBL_DEVICE_TYPE
	{
		PDT_UNKNOWN = 0,
		PDT_KEYBOARD,
        PDT_TEXT_INPUT,
        PDT_TEXT_EDITING,
		PDT_MOUSE_MOVEMENT,
		PDT_MOUSE_BUTTON,
        PDT_MOUSE_WHEEL,
		PDT_TIMER,
        PDT_STREAM,
        PDT_EVENT_QUEUE,
        PDT_AUDIO_OUT,
        PDT_JOYSTICK_AXIS,
        PDT_JOYSTICK_BALL,
        PDT_JOYSTICK_BUTTON,
        PDT_JOYSTICK_HAT,
        PDT_PORT,
        PDT_MOVIE_REFRESH,
        PDT_MOVIE_END,
        PDT_WINDOW_RESIZE,
        PDT_DUMMY
	};


///  This is the basic 'Device' class
///  maybe should be better integrated with the Object system, but
///  we'll see about that later.
///  It is used to manage input devices such as keyboard and mouse events.


class PDevice
{


public:
    ///The Standard constructor.
    PDevice();

    ///The Standard destructor.
    virtual ~PDevice();


    //Overload of the << operator
    friend std::ostream & operator <<(std::ostream & out, const PDevice & device );

    virtual PEBL_DEVICE_TYPE GetDeviceType(){return PDT_UNKNOWN;};
    virtual int GetState(int iface) const;
private:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const;

};





#endif
