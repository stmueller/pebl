//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PJoystick.cpp
//    Purpose:    Class for handling joysticks
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2011 Shane T. Mueller <smueller@obereed.net>
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
#include "PJoystick.h"
#include "../utility/PError.h"


using std::ostream;
using std::fstream;
using std::flush;
using std::string;
using std::cout;

PJoystick::PJoystick()
{
    mCDT = CDT_JOYSTICK;
}


PJoystick::~PJoystick()
{
}


ostream & PJoystick::SendToStream(ostream & out) const
{
    out << "<Generic Joystick Object>" << flush;
    return out;
}

