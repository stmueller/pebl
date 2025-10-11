//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PCustomObject.cpp
//    Purpose:     Contains generic specs for all objects containing text
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2013-2025 Shane T. Mueller <smueller@obereed.net>
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
#include "../utility/PError.h"
#include "PCustomObject.h"
#include "../base/PList.h"
#include "../base/PComplexData.h"
#include "../base/Variant.h"
#include <string>
#include <iostream>
using std::cout;
using std::endl;

PCustomObject::PCustomObject():
    PEBLObjectBase(CDT_CUSTOMOBJECT),
    mName("Unknown Custom Object")
{


}



PCustomObject::PCustomObject(const std::string & name):
    PEBLObjectBase(CDT_CUSTOMOBJECT),
    mName(name)
{

}


PCustomObject::PCustomObject( PCustomObject &object):
    PEBLObjectBase(object)

{

}


PCustomObject::~PCustomObject()
{
    mProperties.clear();
}


bool PCustomObject::SetProperty(std::string name, Variant v)
{
    //We won't do any validation here.  Maybe in the future
    //some validation could be done here; allowing the usere
    //to specify things like value type, or a subset of values.
    std::string uname = PEBLUtility::ToUpper(name);
    bool newname = (mProperties.find(uname) == mProperties.end());

    mProperties[uname]=v;

    if(newname){
      mObjectOrder.push_back(std::string(name));
    }

    return true;
}


Variant PCustomObject::GetProperty(std::string name)const
{
    std::string uname = PEBLUtility::ToUpper(name);
    return PEBLObjectBase::GetProperty(uname);
}


ObjectValidationError PCustomObject::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PCustomObject::ValidateProperty(std::string name)const
{
   ObjectValidationError ove = PEBLObjectBase::ValidateProperty(name);
    if(ove == OVE_VALID)
        return ove;
    else
        return OVE_INVALID_PROPERTY_NAME;

}


/// This sends the color descriptions to the specified stream.
std::ostream & PCustomObject::SendToStream(std::ostream& out) const
{
    out << ObjectName()<< std::flush;
    return out;
}


std::string PCustomObject::ObjectName() const
{
    return mName;
}


std::ostream & PCustomObject::PrintProperties(std::ostream& out)
{
    out << "----------\n";
    for(std::list<std::string>::iterator i = mObjectOrder.begin(); i != mObjectOrder.end(); ++i)
    {
        std::string name = (*i);
        std::string uname = PEBLUtility::ToUpper(name);

        out << "[" << name << "]: " <<  GetProperty(uname) << endl;

    }
    out << "----------\n";


    return out;

}




Variant PCustomObject::GetPropertyList()
{

  PList * returnList = new PList();

    for(std::list<std::string>::iterator i = mObjectOrder.begin(); i != mObjectOrder.end(); ++i)
    {
      returnList->PushBack(Variant(*i));
    }

    counted_ptr<PEBLObjectBase> tmpObj = counted_ptr<PEBLObjectBase>(returnList);
    PComplexData * tmpPCD= (new PComplexData(tmpObj));
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;
}
