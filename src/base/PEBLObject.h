//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/PEBLObject.h
//    Purpose:    Simple base class for objects
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2005-2025 Shane T. Mueller <smueller@obereed.net>
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

#ifndef __PEBLOBJECTBASE_H__
#define __PEBLOBJECTBASE_H__

//#include "PComplexData.h"
#include "Variant.h"
#include <iostream>
#include <map>
#include <string>

enum ObjectValidationError {
    OVE_SUCCESS = 0,
    OVE_VALID = 0,
    OVE_INVALID_PROPERTY_NAME=1,
    OVE_INVALID_PROPERTY_TYPE=2,
    OVE_INVALID_PROPERTY_VALUE=3
};

enum ComplexDataType {
    CDT_UNDEFINED = 0,
    CDT_AUDIOOUT,
    CDT_AUDIOBUFFER,
    CDT_COLOR,
    CDT_ENVIRONMENT,
    CDT_FILESTREAM,
    CDT_FONT,
    CDT_IMAGEBOX,
    CDT_JOYSTICK,
    CDT_KEYBOARD,
    CDT_LIST,
    CDT_TEXTBOX,
    CDT_WINDOW,
    CDT_LABEL,
    CDT_DRAWOBJECT,
    CDT_THICKLINE,
    CDT_CUSTOMOBJECT,
    CDT_CANVAS,
    CDT_NETWORKCONNECTION,
    CDT_PARALLELPORT,
    CDT_COMPORT,
    CDT_MOVIE
};

class PComplexData;

class PEBLObjectBase
{
 public:
    PEBLObjectBase();
    PEBLObjectBase(ComplexDataType cdt);
    PEBLObjectBase(const PEBLObjectBase & pob);
    virtual ~PEBLObjectBase();

    //Overload of the << operator
    friend std::ostream & operator <<(std::ostream & out, const PEBLObjectBase & object );

    //Initializes the valid property names:
    virtual bool InitializeProperty(std::string name, Variant v);


    //Sets the value of a valid property name.  Will not set invalid properties.
    virtual bool SetProperty(std::string name, Variant v);

    Variant GetProperty(std::string)const;

    // This method needs to be overridden by child classes so that
    // want to verify whether a property/string combination is legal.
    // For example, most objects have only a few set legal property names;
    // and trying to set a different property will lead to an error.  Other
    // properties should only have certain types, or the types should only have
    // certain values, and validating will guard against the use of improper types and values.

    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;

    ComplexDataType GetType(){return mCDT;};

    virtual std::string ObjectName() const;
    virtual std::ostream & PrintProperties(std::ostream& out);
    virtual Variant GetPropertyList();
 protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;
    ComplexDataType mCDT;

    //Any object can contain a set of text-string addressable properties.
    std::map<std::string, Variant> mProperties;

};

#endif
 
