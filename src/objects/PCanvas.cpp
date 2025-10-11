//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PCanvas.cpp
//    Purpose:    Contains generic canvas.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2010-2025 Shane T. Mueller <smueller@obereed.net>
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

#include "PCanvas.h"
#include "PColor.h"
#include "../utility/rc_ptrs.h"
#include "../utility/PEBLUtility.h"
#include "../base/PComplexData.h"

#include <iostream>

using std::cout;
using std::endl;

PCanvas::PCanvas()
{
    InitializeProperty("WIDTH",Variant(0));
    InitializeProperty("HEIGHT",Variant(0));
    InitializeProperty("NAME",Variant("<CANVAS>"));

    mReset = false;
}



PCanvas::PCanvas(int width, int height, Variant pcol):
    PWidget(0,0,0,0,false)
{ 
    InitializeProperty("WIDTH",Variant(width));
    InitializeProperty("HEIGHT",Variant(height));
    InitializeProperty("NAME",Variant("<CANVAS>"));
    InitializeProperty("BGCOLOR",pcol);

    SetHeight(height);
    SetWidth(width);
    mReset = false;        
    mDrawBackground = true;
}

PCanvas::PCanvas(int width, int height):
    PWidget(0,0,0,0,false)
{ 
    InitializeProperty("WIDTH",Variant(width));
    InitializeProperty("HEIGHT",Variant(height));
    InitializeProperty("NAME",Variant("<CANVAS>"));

    SetHeight(height);
    SetWidth(width);
    mReset = false;        
    mDrawBackground = false;
    
}


PCanvas::~PCanvas()
{
}


bool PCanvas::SetProperty(std::string name, Variant v)
{

    if(PWidget::SetProperty(name,v))
    {
        // If we set it at higher level, don't worry.
    }
    else if (name =="WIDTH") SetWidth(v);
    else if(name == "HEIGHT") SetHeight(v);
    else if (name == "BGCOLOR") SetColor(v);
    else return false;
    
    return true;
}


// Position of canvas is specified as its center
//
void PCanvas::SetPosition(pInt x, pInt y)
{
    mX = x;
    mY = y;
    mDrawX = x - GetWidth()/2;
    mDrawY = y - GetHeight()/2;
    PEBLObjectBase::SetProperty("X",Variant(mX));
    PEBLObjectBase::SetProperty("Y",Variant(mY));
}

void PCanvas::SetColor(Variant col)
{

    PEBLObjectBase::SetProperty("BGCOLOR",col);
    PWidget::SetBackgroundColor(*dynamic_cast<PColor*>(col.GetComplexData()->GetObject().get()));
                                
    //mBackgroundColor = *dynamic_cast<PColor*>(col.GetComplexData()->GetObject().get());
    mReset = true;
}

Variant PCanvas::GetProperty(std::string name)const
{
    return PWidget::GetProperty(name);
}


ObjectValidationError PCanvas::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PCanvas::ValidateProperty(std::string name)const
{

    return PWidget::ValidateProperty(name);
}





//These shadow higher accessors in widget, because
//they need to set the textchanged flag
void PCanvas::SetHeight(int h)
{


    mHeight = h;
    mReset = true;
}

void PCanvas::SetWidth(int w)
{

    mWidth = w;
    mReset = true;
}



std::string PCanvas::ObjectName()const
{
    return "Canvas";
}
