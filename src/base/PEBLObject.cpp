//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/PEBLObjectBase.cpp
//    Purpose:    Contains a simple list class, part of the Variant Class
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
#include "PEBLObject.h"

#include "../base/PComplexData.h"
#include "../utility/PError.h"
#include "../utility/PEBLUtility.h"
#include <stdio.h>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::list;
using std::ostream;


///Standard Constructor
PEBLObjectBase::PEBLObjectBase()
{
    mCDT=CDT_UNDEFINED;
}
///Standard Constructor
PEBLObjectBase::PEBLObjectBase(ComplexDataType cdt)
{
    mCDT=cdt;

}

PEBLObjectBase::PEBLObjectBase(const PEBLObjectBase & pob)
{

}

///Standard Destructor
PEBLObjectBase::~PEBLObjectBase()
{
    mProperties.clear();
    //  cout << "Deleting pob\n";
}


bool PEBLObjectBase::SetProperty(std::string name, Variant v)
{

    ObjectValidationError err = ValidateProperty(name,v);

    if(err == OVE_SUCCESS)
        {
            mProperties[name]=v;
            return true;
        }
    else
        {
            Variant msg = Variant("Failing to set property:[") +
                Variant(name)+ Variant("]:[")+v+Variant("]\n");

            PError::SignalFatalError(msg);
            return false;
        }
}


bool PEBLObjectBase::InitializeProperty(std::string name, Variant v)
{
    mProperties[name]=v;
    return true;
}

Variant PEBLObjectBase::GetProperty(const std::string name)const
{

    std::string uname = PEBLUtility::ToUpper(name);


    ObjectValidationError err = PEBLObjectBase::ValidateProperty(uname);



    if((err == OVE_SUCCESS ) | ( err == OVE_VALID ))
       {
           return mProperties.find(uname)->second;
       }
    else
        {
            PError::SignalFatalError("Attempted to get invalid property [" + name + "] of object.");
        }
    return Variant(0);
}


//By default, just check to see if the property exists.
ObjectValidationError PEBLObjectBase::ValidateProperty(std::string name, Variant v)const
{
 
    return ValidateProperty(name);
}

ObjectValidationError PEBLObjectBase::ValidateProperty(std::string name)const
{

    std::string uname = PEBLUtility::ToUpper(name);


#if 0
    cout << "Testing: [" << uname << "]" <<endl;
    cout << "Object is: " << *this << endl;
    cout << "-----------------\n";

    std::map<std::string, Variant>::const_iterator i;
    i = mProperties.begin();
    while(i != mProperties.end())
        {
            cout << "[" << i->first << "]: " <<  i->second << endl;
            i++;
        }
    cout << "----------\n";
#endif


    if(mProperties.find(uname) == mProperties.end())
        {

            return OVE_INVALID_PROPERTY_NAME;
        }
    else
        {

            return OVE_SUCCESS;
        }
}


//Overload of the << operator
std::ostream & operator <<(std::ostream & out, const PEBLObjectBase & object )
{
    object.SendToStream(out);
    return out;
}



std::string PEBLObjectBase::ObjectName() const
{
    return "<Unknown PEBLObjectBase>";
}

ostream & PEBLObjectBase::PrintProperties(ostream& out)
{
    out << "----------\n";
    std::map<std::string, Variant>::const_iterator i;
    i = mProperties.begin();
    while(i != mProperties.end())
        {
            out << "[" << i->first << "]: " <<  i->second << endl;
            i++;
        }
    out << "----------\n";


    return out;

}


Variant PEBLObjectBase::GetPropertyList()
{

    //Create a plist to put the properties in

    PList * newlist = new PList();



    std::map<std::string, Variant>::const_iterator i;
    i = mProperties.begin();
    while(i != mProperties.end())
        {
            newlist->PushBack(i->first);
            i++;
        }

    counted_ptr<PEBLObjectBase> tmpObj;
    tmpObj = counted_ptr<PEBLObjectBase>(newlist);
    PComplexData * tmpPCD = (new PComplexData(tmpObj));
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;

}



// Inheritable function that is called by friend method << operator of PComplexData
ostream & PEBLObjectBase::SendToStream(ostream& out) const
{
    out << ObjectName() << flush;
    return out;
}
