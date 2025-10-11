//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PTimer.cpp
//    Purpose:    Primary generic timer event device
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
#include "PTimer.h"
#include "DeviceState.h"

#include <iostream>



/// This is the standard PTimer constructor
PTimer::PTimer()
{

}

/// This is the standard pNode destructor
PTimer::~PTimer()
{
    // Standard Destructor
}


ostream & PTimer::SendToStream(ostream & out) const
{
    out << "<Anonymous Timer>" << std::flush;
    return out;
}



 
// int PTimer::TestDevice(const DeviceState & state) const
// {
//     int time = GetTime();
//     int result = state.TestDevice(time);
//     return result;
// }
