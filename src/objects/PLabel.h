//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PLabel.h
//    Purpose:    Contains generic specs for a visually-displayed text.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2004-2025 Shane T. Mueller <smueller@obereed.net>
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
#ifndef __PLABEL_H__
#define __PLABEL_H__


#include "PTextObject.h"
#include "PFont.h"
#include "PColor.h"
#include "../utility/rc_ptrs.h"

#include <string>

///  This class simply represent a visual word.  It contains a pointer to a PFont structure
///  and a character string. It can be used by a higher-level class to represent a string of
///  text; but do so while keeping track of the specific locations of words. 
///  It actually can represent multiple words, but only displays a single line.


class PLabel: virtual public PTextObject
{
public:
    PLabel();
    PLabel(const std::string & text);
    PLabel(const PLabel & label);
    virtual ~PLabel();


    //overloaded generic PObject methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;

    void SetPosition(pInt x, pInt y);
    //This overrides the default method, because labels are centered on their
    //Position.
    virtual std::string ObjectName() const;
private:


protected:

    
    virtual std::ostream & SendToStream(std::ostream& out) const{return out;};


};


#endif
