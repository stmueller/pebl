//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PEnvironment.cpp
//    Purpose:    Contains generic specification for a PEBL Environment
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
#include "PEnvironment.h"


bool PEnvironment::AddWindow( PWindow * window)
{
    mWindows.push_front(window);
    return true;
}
 
bool PEnvironment::RemoveWindow( PWindow * window)
{
    //std::cout << "Removing window from mWindows: " << mWindows.size() <<std::endl;
    mWindows.remove(window);
    //std::cout << "Removed window from mWindows: " << mWindows.size() <<std::endl;
    return true;
}



PWindow * PEnvironment::GetWindow(int index)
{
    //index is not used--it just gets the first window on the list.
    PWindow * retval = NULL;
    
    if(mWindows.size()>0)
        retval= *(mWindows.begin());
    
    return retval;
}

PEnvironment::~PEnvironment()
{


}
