//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PCustomObject.h
//    Purpose:    Contains specs for a 'custom' object
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2014-2025 Shane T. Mueller <smueller@obereed.net>
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
#ifndef __PCUSTOMOBJECT_H__
#define __PCUSTOMOBJECT_H__

#include "../base/PEBLObject.h"
#include "../utility/rc_ptrs.h"
#include <string>

///  This class simply represent an abstract text-based object.

class PCustomObject: virtual public PEBLObjectBase
{
public:
    PCustomObject();
    PCustomObject(const std::string & text);
    PCustomObject(PCustomObject & object);
    virtual ~PCustomObject();

    //overloaded generic PObject methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;

    std::list<std::string> GetObjectOrder (){return mObjectOrder;};

private:


protected:

    std::string mName;
    std::list<std::string> mObjectOrder;  //keeps track of the order of objects, so it is not
                                          //alphabetical; used only for printout etc.
    virtual std::ostream & SendToStream(std::ostream& out) const;
    virtual std::string ObjectName()const;
    virtual std::ostream & PrintProperties(std::ostream& out);
    virtual Variant GetPropertyList();


};


#endif
