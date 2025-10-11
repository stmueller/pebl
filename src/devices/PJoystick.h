//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PJoystick.h
//    Purpose:    Class for handling Abstract Joysticks
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
#ifndef __PJOYSTICK_H__
#define __PJOYSTICK_H__

#include "PDevice.h"
#include "../base/PEBLObject.h"
#include <fstream>
#include <string>


//This is an abstract joystick class, and holds information that should be present for any 
//joystick implementation.  It must be subclassed with a PlatformJoystick


class PJoystick: public PDevice, virtual public PEBLObjectBase
{
    
public:    


    ///The Standard constructor.  
    PJoystick();
    
    ///The Standard destructor.  
    virtual ~PJoystick();
    
    int GetNumHats(){return mNumHats;};
    int GetNumBalls(){return mNumBalls;};
    int GetNumAxes(){return mNumAxes;};
    int GetNumButtons(){return mNumButtons;};

    virtual signed int GetHatState(unsigned int hat)=0;
    virtual Variant GetBallState(unsigned int ball)=0;
    virtual signed int GetAxisState(unsigned int axis)=0;   
    virtual signed int GetButtonState(unsigned int button)=0;


protected:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const; 

    Variant mName;
    int mNumHats;
    int mNumBalls;
    int mNumAxes;
    int mNumButtons;

    bool mIsOpen;

private:


};



#endif
