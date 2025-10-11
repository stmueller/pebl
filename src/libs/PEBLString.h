//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLString.h
//    Purpose:    List Manipulation Library for PEBL
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
#ifndef __PEBLSTRING_H__
#define __PEBLSTRING_H__

class Variant;


namespace PEBLString
{
    Variant Uppercase (Variant v);
    Variant Lowercase (Variant v);
    Variant ToASCII (Variant v);

    Variant StringLength(Variant v);
    Variant SubString(Variant v);
    Variant FindInString(Variant v);
    Variant SplitString(Variant v);
    Variant CopyToClipboard(Variant v);
}


#endif
