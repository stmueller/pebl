//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PWindow.cpp
//    Purpose:    Contains generic specification for a main window class.
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
#include "PWindow.h"
#include "PWidget.h"
#include "PEnvironment.h"

#include <string>
#include <iostream>
using std::cout;
using std::endl;


PWindow::PWindow(PEnvironment * penv):
    mEnv(penv)
{
    InitializeProperty("NAME",Variant("<WINDOW>"));

}

PWindow::~PWindow()
{
    //this should remove itself from the environment.
    mEnv -> RemoveWindow(this);
    //cout << "Deleting PWindow\n";

}

bool PWindow::SetProperty(std::string name, Variant v)
{


    if(PWidget::SetProperty(name,v))
        {
            return true;
        }
    else 
        {
            return PWidget::SetProperty(name,v);
        }
}


Variant PWindow::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);
}


ObjectValidationError PWindow::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PWindow::ValidateProperty(std::string name)const
{
 
   ObjectValidationError ove = PWidget::ValidateProperty(name);
    
    if(ove == OVE_VALID)
        return ove;

    else
        return OVE_INVALID_PROPERTY_NAME;

}

