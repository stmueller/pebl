//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PWindow.h
//    Purpose:    Contains generic specs for a Main Window class
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
#ifndef __PWINDOW_H__
#define __PWINDOW_H__


#include "PWidget.h"
#include "PColor.h"
#include <iostream>
#include "PEnvironment.h"
///
/// This class, a subtype of PWidget, is the base window class
/// It is an  'abstract' class, and needs to be inherited by a PlatformClass
/// with its methods overridden to be used. Specifically, abstract methods from PWidget
/// need to be instantiated.


class PWindow: virtual public PWidget
{
public:

    PWindow(PEnvironment * penv);
    virtual ~PWindow();
    virtual bool Draw(){return true;}
//    virtual long int DrawFor(unsigned int cycles){return true;}
    virtual std::string ObjectName() const{return "PWindow";};


    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;

protected:

    virtual std::ostream & SendToStream(std::ostream& out) const{return out;};
    PEnvironment * mEnv;  //reference to the encapsulating environment.

};


#endif
