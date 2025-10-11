//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PObject.h
//    Purpose:    Contains the Definition of the PObject Class
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
#ifndef __POBJECT_H__
#define __POBJECT_H__


#include <list>
#include <iostream>
#include <map>

#include "../base/Variant.h"

enum ObjectValidationError {
    OVE_SUCCESS = 0,
    OVE_VALID = 0,
    OVE_INVALID_PROPERTY_NAME,
    OVE_INVALID_PROPERTY_TYPE,
    OVE_INVALID_PROPERTY_VALUE
};
///
/// This class, a subtype of PComplexData, is the base Object class
/// subtypes include sounds, visual objects, etc.


class PObject
{
public:
  
    PObject();
    virtual ~PObject();
  
    //Overload of the << operator
    friend std::ostream & operator <<(std::ostream & out, const PObject & object );

    //Initializes the valid property names:
    virtual bool InitializeProperty(std::string name, Variant v);
    

    //Sets the value of a valid property name.  Will not set invalid properties.
    virtual bool SetProperty(std::string name, Variant v);

    virtual Variant GetProperty(std::string)const;

    // This method needs to be overridden by child classes so that 
    // want to verify whether a property/string combination is legal.
    // For example, most objects have only a few set legal property names;
    // and trying to set a different property will lead to an error.  Other
    // properties should only have certain types, or the types should only have
    // certain values, and validating will guard against the use of improper types and values.

    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;


protected:
    virtual std::string ObjectName() const;
    virtual std::ostream & PrintProperties(std::ostream& out) const;
    virtual std::ostream & SendToStream(std::ostream& out) const;
    unsigned int mCopies;

    //Any object can contain a set of text-string addressable properties.
    std::map<std::string,Variant> mProperties;
};


#endif
