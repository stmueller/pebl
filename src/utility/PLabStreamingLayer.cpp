//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/utility/PError.cpp
//    Purpose:     Utility code for interfacing with LSL
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2019 Shane T. Mueller <smueller@obereed.net>
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

#include "PLabStreamingLayer.h"
#include "../../../libs/labstreaminglayer/lsl_cpp.h"

#include <iostream>
#include <string>
#include <signal.h>



using std::cout;
using std::cerr;
using std::endl;
using std::string;

extern PlatformEnvironment * myEnv;



void PLSL::PLSL(string name, string type)
{
    lsl::stream_info info(name,type,1, lsl::IRREGULAR_RATE, lsl::cf_string,"PEBL123");
	mStreamOutlet = new stream_outlet(info);


}


void PLSL::SendMessage(string message)
{
    mStreamOutlet->push_sample(message);
}
