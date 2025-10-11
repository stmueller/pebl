//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PImageBox.h
//    Purpose:    Contains generic specs for an ImageBox class
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
#ifndef __PIMAGEBOX_H__
#define __PIMAGEBOX_H__


#include "PWidget.h"
#include <string>

///
/// This class, a subtype of PComplexData, is the base Object class
/// subtypes include sounds, visual objects, etc.


class PImageBox: virtual public PWidget
{
public:
  
    PImageBox();
    virtual ~PImageBox(){};
  
    

    //overloaded generic PEBLObjectBase methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;
    

    ///This will load a file into the class data, given a file name.
    ///height and width are set automatically.
    virtual bool LoadImage(const std::string  & imagefilename)=0;

    virtual void SetPosition(pInt x, pInt y);
    virtual void SetSize(int width, int height);

    virtual std::string ObjectName() const{return "PImageBox";};
    
protected:

    //These are inherited by child PlatformImageBox class.

    virtual  std::ostream & SendToStream(std::ostream& out) const {return out;};    


private:
    void __SetProps__();
};



#endif
