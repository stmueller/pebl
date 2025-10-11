//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/PNode.cpp
//    Purpose:    Primary data structure for code
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

#include <iostream>

using std::ostream;
using std::cout;
using std::cerr;

/// This is the standard PDevice constructor
PDevice::PDevice()
{

}

/// This is the standard pNode destructor
PDevice::~PDevice()
{
    // Standard Destructor
}




//Overload of the << operator
ostream & operator <<(ostream & out, const PDevice & device )
{ 
    device.SendToStream(out);
    return out;
}

 

ostream & PDevice::SendToStream(ostream & out) const
{
    out << "<Anonymous Device>" << std::flush;
    return out;
}

int PDevice::GetState(int interface) const
{
    cerr << "Getting improper state in PDevice::GetState\n";
    return 0;
}
