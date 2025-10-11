//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformJoystick.h
//    Purpose:    Class for SDL Joysticks
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2011-2025 Shane T. Mueller <smueller@obereed.net>
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

#include "PlatformJoystick.h"
#include "SDL.h"

#include <list>
#include <stdio.h>
#include "../../utility/PError.h"
#include "../../base/PList.h"
#include "../../base/PComplexData.h"

using std::ostream;
using std::flush;

    ///The Standard constructor.
PlatformJoystick::PlatformJoystick(int index)
{
  if((SDL_NumJoysticks()>= (index+1)) &
	 (index>=0))
	{
	  mJoystick = SDL_JoystickOpen(index);
	  mIndex = index;
	  mIsOpen = true;

	}else
	{
	  PError::SignalFatalError(Variant("Joystick unavailable: ") + Variant(index));
	}

  //The joystick API has changed a bit:
#ifdef SDL2_DELETE
  mName = SDL_JoystickName(mIndex);
#endif

  mNumHats =   SDL_JoystickNumHats(mJoystick);
  mNumBalls =	SDL_JoystickNumBalls(mJoystick);
  mNumAxes = 	SDL_JoystickNumAxes(mJoystick);
  mNumButtons =	SDL_JoystickNumButtons(mJoystick);

}

PlatformJoystick::PlatformJoystick(const PlatformJoystick & pjs)
{
  mJoystick = NULL;
}

    ///The Standard destructor.
PlatformJoystick::~PlatformJoystick()
{

  SDL_JoystickClose(mJoystick);

}


signed int PlatformJoystick::GetHatState(unsigned int hat)
 {

   //check to be sure hat exists.
   if(GetNumHats()<hat)
	 {
	   PError::SignalWarning(Variant("Unavailable joystick hat ")+Variant((int)hat)+Variant(" requested"));
	   return 0;
	 }


   SDL_JoystickUpdate();
   return SDL_JoystickGetHat(mJoystick,hat-1);
 }


Variant PlatformJoystick::GetBallState(unsigned int ball)
 {
   if(GetNumBalls()<ball)
	 {
	   PError::SignalWarning(Variant("Unavailable joystick ball ")+Variant((int)ball)+Variant(" requested"));
	   return 0;
	 }


   //check to be sure ball exists.
   SDL_JoystickUpdate();
   int dx;
   int dy;
   SDL_JoystickGetBall(mJoystick,ball-1,&dx,&dy);

   PList * newlist = new PList();
   newlist->PushBack(Variant(dx));
   newlist->PushBack(Variant(dy));
   counted_ptr<PEBLObjectBase> newlist2 = counted_ptr<PEBLObjectBase>(newlist);
   PComplexData *   pcd = new PComplexData(newlist2);
   return Variant(pcd);


 }

signed int PlatformJoystick::GetAxisState(unsigned int axis)
 {

   //   if(GetNumAxes()<axis)
   //	 {
   //	   PError::SignalWarning(Variant("Unavailable joystick axis ")+Variant((int)axis)+Variant(" requested"));
   //	   return 0;
   //	 }

   SDL_JoystickUpdate();
   //check to be sure axis exists.
   return  SDL_JoystickGetAxis(mJoystick,axis-1);
 }

signed int PlatformJoystick::GetButtonState(unsigned int button)
 {

   if(GetNumButtons()<button)
	 {
	   PError::SignalWarning(Variant("Unavailable joystick button ")+Variant((int)button)+Variant(" requested"));
	   return 0;
	 }

   SDL_JoystickUpdate();
   //check to be sure button exists.
   return  SDL_JoystickGetButton(mJoystick,button-1);

 }


// Inheritable function that is called by friend method << operator of PComplexData
ostream & PlatformJoystick::SendToStream(ostream& out) const
{

  out << "<PEBL Joystick: " << mName << ">" << flush;

  return out;
}

