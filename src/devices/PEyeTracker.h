//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PEyeTracker.h
//    Purpose:    Class for handling gazelib eyetracker device
//                (TheEyeTribe eyetracker)
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2016 Shane T. Mueller <smueller@obereed.net>
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
//
//  This class is targeted to gazelib developed by theeyetribe, a $100 eyetracker.


#ifdef PEBL_GAZELIB


#ifndef __PEYETRACKER_H__
#define __PEYETRACKER_H__

#include "../base/PEBLObject.h"
#include "../utility/rc_ptrs.h"

#include "../../libs/tet-cpp-client-master/include/gazeapi.h"

#include "../base/Variant.h"



//This creates a class to access GazeAPI.
//This does not create a listener that might be used to automatically
//handle the gaze via an event loop.

class PEyeTracker: virtual public PEBLObjectBase
{
public:
    PEyeTracker();
    virtual ~PEyeTracker();
    virtual gtl::GazeData * GetGazeFrame();
    

    virtual Variant GetProperty(std::string)const;
    virtual bool SetProperty(std::string name, Variant v);
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;
private:
    gtl::GazeApi m_api;
};


    Variant ConvertGazeData(const gtl::GazeData &gd);


// --- Gaze handler definition.  This is used to create a listener function controlled by
//TET, rather than just pulling data when you want it.
class PEBLGaze : public gtl::IGazeListener
{
public:
    PEBLGaze(std::string  functionname);
    ~PEBLGaze();
private:
    // IGazeListener
    void on_gaze_data( gtl::GazeData const & gaze_data );
private:
    gtl::GazeApi m_api;
    std::string mFunctionName;
};




#endif

#endif