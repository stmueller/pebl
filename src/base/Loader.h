//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/Loader.h
//    Purpose:    Defines an class that loads functions into PEBL environment.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003-2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
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

#ifndef __LOADER_H__
#define __LOADER_H__

#include "PNode.h"
#include "Variant.h"
#include "VariableMap.h"
#include "FunctionMap.h"

#include <set>
#include <map>


/// This class takes a parsed program and loads it into the 
/// evaluation environment.  This involves following the upper
/// levels of the parse tree (which in effect form a list of function/functions
/// nodes) and moving functions into a FunctionMap. It also does
/// preliminary checking, and can be a place to do other debugging stuff and
/// perhaps optimizing.
class Loader
{
public:
    Loader();
    ~Loader();


    /// This will perform a recursive search through a node tree and place the
    /// names of any functions it finds in the list mFunctionList, for
    /// later use by the verify routine.  This probably could be done more quickly during 
    /// the initial parse, or eliminated altogether for faster (riskier) loading.
    void FindFunctions(const PNode * Node);


    /// This will load the parsed PNode tree into mFunctionList, for use by
    /// the Evaluator.
    void LoadUserFunctions(OpNode * node);
  

    /// If any functions found by FindFunctions are in libraries, this
    /// method will load them properly.
    void LoadLibraryFunctions();

    ///This checks to see whether everything loaded into the environment
    ///is legal; i.e, if all functions used exist. 
    bool Verify();
  
  
    /// This looks inside the functionmap for a function titled
    /// "start", which it then retrieves.
    PNode * GetMainPEBLFunction();

    void DumpFunctionSet();

private:
  
    ///This stores a list of all functions used.  It is used to verify 
    ///whether the program will actually run.
    std::set<Variant> mFunctionSet;


};


#endif
