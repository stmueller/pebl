//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PImageBox.cpp
//    Purpose:    Contains generic specification for a imagebox  class.
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

#include "PImageBox.h"
#include <iostream>

using std::cout;

PImageBox::PImageBox():
    PWidget()
{
    InitializeProperty("NAME",Variant("<IMAGEBOX>"));
    __SetProps__();
 
}

///This sets the imagebox's position on its parent widget.
///It overrides the parent method because images positions
///are set by their centers.
void PImageBox::SetPosition(pInt x, pInt y)
{
    

    //mx/my is the specified position.
    mX = x;
    mY = y;
    // PWidget::SetPosition(x,y);
    //mdrawxy is the drawn upper-left corner position.
    mDrawX = x - GetWidth()/2;
    mDrawY = y - GetHeight()/2;

    PEBLObjectBase::SetProperty("X",Variant(mX));
    PEBLObjectBase::SetProperty("Y",Variant(mY));

}

void PImageBox::SetSize(int width, int height)
{


    mWidth = width;
    mHeight = height;
   
    PEBLObjectBase::SetProperty("WIDTH",width);
    PEBLObjectBase::SetProperty("HEIGHT",height);
    //Reset position so it gets centered correctly.
    SetPosition(mX,mY);
}





//overloaded generic PEBLObjectBase methods
bool PImageBox::SetProperty(std::string name, Variant v)
{


    //Width and height are not currently 'settable', in that
    //they are properties of the object.

    if(name == "X") SetPosition(v,mY);
    else if (name == "Y") SetPosition(mX,v);
    else if (name == "HEIGHT") SetSize((int)mWidth,v);
    else if (name == "WIDTH") SetSize(v,(int)mHeight);
    else if (name =="ZOOMX") SetZoomX(v);
    else if (name =="ZOOMY") SetZoomY(v);
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


Variant PImageBox::GetProperty(std::string name)const
{

    //PEBLObjectBase::PrintProperties(cout);
    return PEBLObjectBase::GetProperty(name);
}


ObjectValidationError PImageBox::ValidateProperty(std::string name, Variant v)const
{

    return ValidateProperty(name);
}

ObjectValidationError PImageBox::ValidateProperty(std::string name)const
{
    //All valid properties are also valid Widget properties
    ObjectValidationError ove = PWidget::ValidateProperty(name);
    if(ove == OVE_VALID)
        {
            return ove;
        }
    else if(name =="WIDTH" ||
            name == "HEIGHT")
        return OVE_VALID;
    else
        
        return OVE_INVALID_PROPERTY_NAME;
}



//This makes sure that the object properties are synced with the class properties.
void PImageBox::__SetProps__()
{

     SetProperty("X",mX);
     SetProperty("Y",mY);
     SetProperty("VISIBLE",mIsVisible);
     SetProperty("WIDTH",mWidth);
     SetProperty("HEIGHT",mHeight);
     SetProperty("ZOOMX",mZoomX);
     SetProperty("ZOOMY",mZoomY);
     SetProperty("ROTATION",mRotation);

}
