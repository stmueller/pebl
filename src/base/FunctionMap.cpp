//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/FunctionMap.cpp
//    Purpose:    Structure that holds all user-defined and library function
//                used during execution
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

#include "FunctionMap.h"
#include "PNode.h"
#include "../utility/PEBLUtility.h"
#include "../utility/PError.h"

#include <iostream>
#include <map>
#include <string>
#include <functional>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::string;
using std::map;
using std::pair;


///
///  This class stores pointers to user-defined pnode-parsed functions,
///  which can be accessed by name.  This includes the main function
///  of the PEBL program.

FunctionMap::FunctionMap()
{
    //Initiate anything necessary here.
}


FunctionMap::~FunctionMap()
{

    //cout << "Detelitng functionmap\n";
    //Delete mFunctionMap if necessary.
    //go through each function and delete the opnode,  because
    //it is a pointer that is held nowhere else.
    Destroy();
}

void FunctionMap::Destroy()
{

    //Delete mFunctionMap if necessary.
    //go through each function and delete the opnode,  because
    //it is a pointer that is held nowhere else.
    std::map<std::string, PNode*>::iterator i = mFunctionMap.begin();

    while(i != mFunctionMap.end())
        {
            if(i->second)
                {
                    i->second->DestroyChildren();
                    delete i->second;
                    i->second = NULL;
                }
            i++;
        }
    mFunctionMap.clear();
}

///
/// This method will add a new variable with
/// name varname and value val to the variable map, or (if it already exists)
/// change its value to val.
void FunctionMap::AddFunction(std::string funcname,  OpNode * node)
{

    //Get the uppercase version of the funcname.

    std::string upperfuncname = PEBLUtility::ToUpper(funcname);
    //std::cout << "Adding " <<  upperfuncname << endl;
    map<string, PNode*>::iterator p;
    p = mFunctionMap.find(upperfuncname);

    //If a function is in there already, just signal an error.
    // Maybe we should clean up the PNode tree that
    //is there and add the new one in its place.
    if(p!=mFunctionMap.end())
        {
            cerr << "Error in function '" << upperfuncname << "'.  Function already exists.\n";
        }
    else
        {
            //variable isn't there yet, so add the new value into map
            mFunctionMap.insert(pair<string, PNode* >(upperfuncname, node));
        }
}

/// This method will add a new function with name and code associated with the
/// OpNode node, which is of type PEBL_FUNCTION.

// void FunctionMap::AddFunction(OpNode * node)
// {

//     //Get the left node, which is a FunctionName.
//     DataNode * node1 = (DataNode*)(node->GetLeft());
//     Variant v = node1->GetValue();

//     OpNode * node2 = (OpNode*)(node->GetRight());

//     //Add the name/node pair using previous method.
//     AddFunction(v,node2);

// }


///
/// This method will retrieve the value designated by
/// funcname.  If funcname doesn't exist, it will return
/// a null pointer.
PNode *  FunctionMap::GetFunction(const string & lcasefuncname)
{
    // Convert the function name to uppercase, so that it will find
    // a match when searching through the upper-case functionmap.

    std::string  funcname = PEBLUtility::ToUpper(lcasefuncname);
    map<string,PNode*>::iterator p;

    //Get a the function

    p = mFunctionMap.find(funcname);

    if(p == mFunctionMap.end())
        {
            //This should probably signal an error.
            PError::SignalFatalError(string("Function Name [")+ funcname+string("] not found.\n"));
            DumpValues();
            return NULL;//new DataNode(Variant(PEBL_DATA_NODE),"PEBL SELF-GENERATED OBJECT",-1);
        }
    else
        {

            //Otherwise, just return a pointer to the node.
            //cast it to the appropriate type.
            if((p->second)->GetType() == PEBL_OP_NODE)
                return (OpNode*)(p->second);

            else if((p->second)->GetType() == PEBL_DATA_NODE)
                return (DataNode*)(p->second);

            else
                return p->second;
        }

}



///  This function just determines whether
///  there is a function of the given name.  It is useful because
///  GetFunction() may behave badly when the function doesn't exist.
///  It converts the query to uppercase, because everything in the functionmap should be uppercase.
bool FunctionMap::IsFunction(const string & funcname)
{

    std::string  upperfuncname = PEBLUtility::ToUpper(funcname);
    //Get a the function from the map
    map<string,PNode*>::iterator p;
    p = mFunctionMap.find(upperfuncname);

    //If it isn't in there, return false, otherwise true.
    if(p == mFunctionMap.end())
        return false;
    else
        return true;
}


/// This will erase the topmost value stored in varname
///
void FunctionMap::Erase(const string & funcname)
{
    std::string upperfuncname = PEBLUtility::ToUpper(funcname);
    mFunctionMap.erase(upperfuncname);
}



/// This method is primarily for debugging purposes.
/// It will iterate through the entire variable map, and
/// Display each of the values held in the map.
///
void FunctionMap::DumpValues()
{
    map<string,PNode *>::iterator p;
    cerr << "---------------------------\n    Function Map:\n";
    for(p= mFunctionMap.begin(); p!=mFunctionMap.end(); p++)
        {
            cerr << "Function Name:  [" << std::flush;
            cerr << p->first << ":";
            cerr << ":" << *(p->second) << "]\n" ;

        }

}
