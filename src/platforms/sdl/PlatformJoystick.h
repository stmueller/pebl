//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PlatformJoystick.h
//    Purpose:    Class for handling Joysticks with SDL
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
#ifndef __PLATFORMJOYSTICK_H__
#define __PLATFORMJOYSTICK_H__


#include "../../devices/PJoystick.h"
#include <fstream>
#include <string>

#include "SDL.h"

class PlatformJoystick: public PJoystick
{

public:


    ///The Standard constructor.
    PlatformJoystick(int i);
    PlatformJoystick(const PlatformJoystick & js);
    ///The Standard destructor.
    virtual ~PlatformJoystick();


    virtual signed int GetHatState(unsigned int hat);
    virtual Variant GetBallState(unsigned int ball);
    virtual signed int GetAxisState(unsigned int axis);
    virtual signed int GetButtonState(unsigned int button);




protected:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const;



private:

    SDL_Joystick * mJoystick;
    int mIndex;

};



#endif
