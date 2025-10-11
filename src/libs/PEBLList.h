//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLList.h
//    Purpose:    List Manipulation Library for PEBL
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
#ifndef __PEBLLIST_H__
#define __PEBLLIST_H__

class Variant;



namespace PEBLList
{


    Variant Shuffle (Variant v);
    Variant Repeat (Variant v);
    Variant RepeatList (Variant v);
    Variant RepeatExpression(Variant v);
    Variant Sequence(Variant v);
    Variant DesignFullCounterbalance (Variant v);
    Variant CrossFactorWithoutDuplicates(Variant v);
    Variant Rotate(Variant v);

    Variant Length(Variant v);
    Variant Nth (Variant v);
    Variant First (Variant v);
    Variant Second (Variant v);
    Variant Third (Variant v);
    Variant Fourth (Variant v);
    Variant Fifth (Variant v);
    Variant Last (Variant v);
    Variant Merge (Variant v);
    Variant Append(Variant v);
    Variant List(Variant v);
    Variant Sort (Variant v);
    Variant SortBy (Variant v);

    Variant IsMember (Variant v);
    Variant RemoveDuplicates(Variant v);
    Variant MakeMap (Variant v);
    Variant Transpose (Variant v);
    Variant SubList (Variant v);
    Variant Remove (Variant v);

    Variant PushOnEnd(Variant v);
    Variant SetElement(Variant v);
    Variant ListToString(Variant v);
    Variant ModList(Variant v);
}


#endif
