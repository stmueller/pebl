//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/Evaluator.h
//    Purpose:    Defines an class that can evaluate PNodes
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003--2013 Shane T. Mueller <smueller@obereed.net>
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

#ifndef __EVALUATOR_H__
#define __EVALUATOR_H__

#include <stack>
#include <list>
//#include "PNode.h"
#include "Variant.h"

#include "FunctionMap.h"
#include "../utility/PEBLPath.h"
#include "VariableMap.h"
#include "../utility/PError.h"
//#include "../devices/PEventLoop.h"


class PNode;
class OpNode;
class DataNode;
class Variant;
class FunctionMap;
class VariableMap;
class PEventLoop;


//
void EvalWrapper(void * node);


/// This class has got everything you need to evaluate stuff.
class Evaluator
{
    // PEventLoop needs access to scope management internals for event callbacks
    friend class PEventLoop;

public:

    Evaluator();
    Evaluator(Variant & stacktop, string scope);
    ~Evaluator();

    //this is in Evaluator.h, but not here

    void CallFunction(const OpNode * node);

     bool Evaluate1();
     bool Evaluate1(const PNode * node);
     bool Evaluate1(const OpNode * node);
     bool Evaluate1(const DataNode * node);



    void Push(Variant v);
    Variant Pop();
    Variant Peek();

    void NodeStackPush(const PNode * node);

    int GetNodeStackDepth(){return mNodeStack.size();};
    int GetStackDepth(){return mStack.size();};


    PEventLoop * GetEventLoop(){return mEventLoop;};

    bool IsVariableName(Variant v);

    /// This holds a pointer to a FunctionMap, which 
    /// is loaded from the initial PNode tree by the loader.
    /// It is static and public, which means that all Evaluators
    /// have access to the same one, and anything else can access it
    /// with Evaluator::mFunctionMap
    static FunctionMap mFunctionMap;
 
    /// The static keyword defines a 'Global' variable map that can be accessed\all
    /// Evaluators. It needs to be initialized somewhere outside the class, however. This
    /// is done in the main program file.
    static VariableMap gGlobalVariableMap;
    
    static PEventLoop * mEventLoop;

    static PEBLPath gPath;



    /// This is used for error detection.  Every time gEvalNode is 
    /// updated, this is set to gEvalNode.  But, it is done for 
    /// all evaluators.  Thus, it always points to the last node
    /// processed, and error reporting routines can look for it and
    /// find out where it came from.
    static const  PNode * gEvalNode;


    static PCallStack gCallStack;


    
private:

    //This list keeps track of the call sequence 
    //of evaluators, so that when an error is found,
    //all the calls can be traced back.  It is a list
    //instead of a stack so that we can examine it easily.



    /// This is the main data stack for the evaluator
    std::stack<Variant> mStack;



    std::stack<const PNode*> mNodeStack;

    /// This stack could be done a little smarter.  Limits
    /// the depth of the stack in case problems happen. Checking only
    /// goes on within Push().
    unsigned int mStackMax;

    /// This holds variables local to the current thread.
    VariableMap mLocalVariableMap; 

    //this keeps a stack of the local variable maps so
    //we can have a single async evaluator.
    std::stack<VariableMap> mVariableMapStack;
   
    /// The name of the current scope; the function name
    /// which the evaluator is invoked for.
    std::string mScope;

    //Keep a scope stack for debugging purposes.
    std::stack<std::string> mScopeStack;
};


#endif
