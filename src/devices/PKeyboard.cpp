//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PKeyboard.cpp
//    Purpose:    Primary Keyboard class
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
#include "PKeyboard.h"
#include "../utility/PEBLUtility.h"
#include "../utility/PError.h"
#include <iostream>
#include <list>
#include <string>

using std::ostream;


/// This is the standard pNode constructor
PKeyboard::PKeyboard()
{
    //Standard Constructor
}

/// This is the standard pNode destructor
PKeyboard::~PKeyboard()
{
    // Standard Destructor
}




ostream & PKeyboard::SendToStream(ostream & out) const
{
    out << "<Generic Keyboard>" << std::flush;
    return out;
}




///This does as best as it can to translate the letter string
///into a keyboard key.
PEBL_Keycode PKeyboard::TranslateString(std::string letter) const
{
    return PEBLUtility::TranslateString(letter);
}

//This returns a text-valued description of the key pressed.
std::string PKeyboard::TranslateKeyCode(const PEBL_Keycode key) const
{
    return PEBLUtility::TranslateKeycode(key, PEBLMOD_NONE);
}

int PKeyboard::GetState(int iface)const
{
     cerr << "*****Error:  Getting generic state with PKeyboard::GetState\n" ;
     return 1;
}


int PKeyboard::GetModKeys() const
{
    PError::SignalFatalError("Getting generic state with PKeyboard::GetModKeys");
    return 0;
}
