//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/VariableMap.h
//    Purpose:    Structure that holds global or local variables
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

#ifndef __VARIABLEMAP_H__
#define __VARIABLEMAP_H__

#include "Variant.h"
#include <map>
#include <string>


/// This is the primary structure for holding variables: the variable map
/// It is an STL map that takes a string-based variable name and stores
/// a variant. Every instance of an Evaluator has one private local VariableMap 
/// that cannot be accessed by other Evaluators, and there is on global VariableMap
/// that is shared between Evaluators.

class VariableMap {
public:
  
    ///The Standard constructor. 
    VariableMap();
    ///The Standard destructor.  
    ~VariableMap();

    void AddVariable(const std::string & varname, Variant val);

    Variant RetrieveValue(const std::string & varname);
    bool Exists(const std::string & varname);
    void Erase(const std::string &  varname);
    void DumpValues();
    void Destroy();  //destroys all data in the map.
private:

    std::map<std::string, Variant> mVariableMap;


};


#endif
