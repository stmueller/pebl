//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PWord.h
//    Purpose:    Contains generic specs for all objects containing text
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
#ifndef __PTEXTOBJECT_H__
#define __PTEXTOBJECT_H__


#include "PWidget.h"
#include "PFont.h"
#include "PColor.h"
#include "../utility/rc_ptrs.h"

#include <string>

///  This class simply represent an abstract text-based object.

class PTextObject: virtual public PWidget
{
public:
    PTextObject();
    PTextObject(const std::string & text);
    PTextObject(PTextObject & object);
    virtual ~PTextObject();

    //overloaded generic PObject methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;

    virtual void SetText(const std::string & text);
    virtual void SetDirection(int i);
    virtual std::string GetText()const {return mText;}

    virtual unsigned long int GetNumCharacters(){return mText.length();}


private:


protected:

    
    virtual std::ostream & SendToStream(std::ostream& out) const{return out;};

    bool mTextChanged;     //Determines whether the object should be re-rendered.
    std::string mText;
    virtual std::string ObjectName() const{return "PTextObject";};
    int mDirection;   //1 for ltr, -1 for rtl

};


#endif
