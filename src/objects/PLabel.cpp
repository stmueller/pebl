//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PLabel.cpp
//    Purpose:    Contains methods primary visual 'word' structure
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2004-2025 Shane T. Mueller <smueller@obereed.net>
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
#include "PLabel.h"
#include "PFont.h"

#include <string>
#include <iostream>

using std::cout;
using std::endl;

PLabel::PLabel():
    PTextObject("")
{
    InitializeProperty("NAME",Variant("<LABEL>"));
    mCDT=CDT_LABEL;
}



PLabel::PLabel(const std::string & text):
    PTextObject(text)
{
    InitializeProperty("NAME",Variant("<LABEL>"));
    mCDT=CDT_LABEL;
}



PLabel::PLabel(const  PLabel & label):
    PTextObject(label.GetText())
{
    InitializeProperty("NAME",Variant("<LABEL>"));
    mCDT=CDT_LABEL;
}

PLabel::~PLabel()
{
}


bool PLabel::SetProperty(std::string name, Variant v)
{
    if(PTextObject::SetProperty(name,v))
        return true;
    else return false;

}


Variant PLabel::GetProperty(std::string name)const
{
    return PTextObject::GetProperty(name);
}


ObjectValidationError PLabel::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PLabel::ValidateProperty(std::string name)const
{
 
    ObjectValidationError ove = PTextObject::ValidateProperty(name);
    
    if(ove == OVE_VALID)
        return ove;
    else
        return OVE_INVALID_PROPERTY_NAME;
}

void PLabel::SetPosition(pInt x, pInt y)
{
    //Why does this get called 4x with an EasyLabel?

    mX = x;
    mY = y;
    mDrawX = x - GetWidth()/2;
    mDrawY = y - GetHeight()/2;
    //    cout << "Setting label position\n" <<  x << ","<<y<<"|" << mDrawX << "," << mDrawY<<endl;
    PEBLObjectBase::SetProperty("X",Variant(mX));
    PEBLObjectBase::SetProperty("Y",Variant(mY));
}


std::string PLabel::ObjectName()const
{
    return "Text Label";
}
