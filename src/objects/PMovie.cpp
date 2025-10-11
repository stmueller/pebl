//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PMovie.cpp
//    Purpose:    Contains generic specification for a movie player widget.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2012-2025 Shane T. Mueller <smueller@obereed.net>
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


#include "PMovie.h"
#include "../utility/Defs.h"
#include <iostream>

using std::cout;

PMovie::PMovie():
    PWidget()
{
    InitializeProperty("NAME",Variant("<MOVIE>"));
    InitializeProperty("FILENAME","UNKNOWN");
    InitializeProperty("DURATION",0);
    InitializeProperty("PLAYBACKPOSITION",0);
    InitializeProperty("VOLUME",1);
    __SetProps__();
 
}

///This sets the imagebox's position on its parent widget.
///It overrides the parent method because images positions
///are set by their centers.
void PMovie::SetPosition(int x, int y)
{


    PWidget::SetPosition(x,y);
    mDrawX = x - GetWidth()/2;
    mDrawY = y - GetHeight()/2;
}

void PMovie::SetSize(int width, int height)
{
    mWidth = width;
    mHeight = height;
    
    PEBLObjectBase::SetProperty("WIDTH",width);
    PEBLObjectBase::SetProperty("HEIGHT",height);
    
}


void PMovie::SetPlaybackPosition(long unsigned int pos)
{
    mPlaybackPosition = pos;
    PEBLObjectBase::SetProperty("PLAYBACKPOSITION",pos);

}


void PMovie::SetVolume(long double v)
{
    PEBLObjectBase::SetProperty("VOLUME",(pDouble)v);
}


//overloaded generic PEBLObjectBase methods
bool PMovie::SetProperty(std::string name, Variant v)
{

    //Width and height are not currently 'settable', in that
    //they are properties of the object.

    if(name == "X") SetPosition(v,mY);
    else if (name == "Y") SetPosition(mX,v);
    else if (name == "HEIGHT") SetSize(v,mWidth);
    else if (name == "WIDTH") SetSize(mHeight, v);
    else if (name == "DURATION")
        PEBLObjectBase::SetProperty("DURATION",v);
    else if (name == "PLAYBACKPOSITION") SetPlaybackPosition(v);
    else if (name == "VOLUME") SetVolume((pDouble)v);
    else if (name == "FILENAME") 
        PEBLObjectBase::SetProperty("FILENAME",v);
    else if (name == "VISIBLE") 
        {
            if(v.GetInteger())
                Show();
            else 
                Hide();
        }
    else return PWidget::SetProperty(name,v);

    return true;
}



Variant PMovie::GetProperty(std::string name)const
{

    return PEBLObjectBase::GetProperty(name);
}


ObjectValidationError PMovie::ValidateProperty(std::string name, Variant v)const
{
    ObjectValidationError code = ValidateProperty(name);
    if((code==OVE_VALID) & (name == "VOLUME"))
        {
            
            if((pDouble)v < 0)
                {
                    code = OVE_INVALID_PROPERTY_VALUE;
                }
        }
    
    return code;
}

ObjectValidationError PMovie::ValidateProperty(std::string name)const
{
    //All valid properties are also valid Widget properties
    ObjectValidationError ove = PWidget::ValidateProperty(name);
    if(ove == OVE_VALID)
        {
            return ove;
        }
    else if(name =="WIDTH" ||
            name == "HEIGHT"||
            name == "DURATION"||
            name == "FILENAME"||
            name == "PLAYBACKPOSITION" ||
            name == "VOLUME" )

        return OVE_VALID;
    else
        return OVE_INVALID_PROPERTY_NAME;
}



//This makes sure that the object properties are synced with the class properties.
void PMovie::__SetProps__()
{

     SetProperty("X",mX);
     SetProperty("Y",mY);
     SetProperty("VISIBLE",mIsVisible);
     SetProperty("WIDTH",mWidth);
     SetProperty("HEIGHT",mHeight);

}
