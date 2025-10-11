//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/FunctionMap.h
//    Purpose:    Structure that holds user-defined functions
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003--2016 Shane T. Mueller <smueller@obereed.net>
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

#ifndef __FUNCTIONMAP_H__
#define __FUNCTIONMAP_H__

#include "PNode.h"
#include <map>
#include <string>

class PNode;

/// This is the primary structure for holding user-defined functions.
/// It is an STL map that takes a string-based function  name and stores
/// a PNode.

/// FunctionMap uses an STL multimap, which allows us to have many
/// functions with the same name. The most recently loaded function blocks
/// earlier ones, but earlier ones can be access if the later ones are deleted.

class FunctionMap {
public:
  
    ///The Standard constructor. 
    FunctionMap();
    ///The Standard destructor.  
    ~FunctionMap();

    void AddFunction(std::string funcname, OpNode * node);
    //    void AddFunction(OpNode * node);
    PNode *  GetFunction(const std::string & funcname);
    bool IsFunction(const std::string &  funcname);
    void Erase(const std::string & funcname);
    void DumpValues();
    void Destroy(); //destroys everything in the functionmap, for good.

private:

    ///This map uses less_equal as a comparison function to make sure
    ///new elements are added in front of current elements
    std::map<std::string, PNode* > mFunctionMap;


};


#endif
