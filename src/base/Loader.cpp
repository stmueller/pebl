//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/Loader.cpp
//    Purpose:    Defines an class that loads a parsed PNode tree into PEBL Environment
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

#include "Loader.h"

#include "PNode.h"
#include "grammar.tab.hpp"
#include "Variant.h"
#include "FunctionMap.h"

#ifdef PEBL_EMSCRIPTEN
#include "Evaluator-es.h"
#include <emscripten.h>
#else
#include "Evaluator.h"
#endif

#include "../utility/PEBLUtility.h"
#include "../utility/PError.h"

//Include the function libraries
#include "../libs/Functions.h"
#include "../libs/PEBLEnvironment.h"
#include "../libs/PEBLMath.h"
#include "../libs/PEBLObjects.h"
#include "../libs/PEBLStream.h"
#include "../libs/PEBLString.h"


#include <iostream>
#include <list>
#include <string>

#undef PEBL_DEBUG_PRINT


using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::list;
using std::string;

Loader::Loader()
{

}

Loader::~Loader()
{

    //cout << "Deleting loader\n";
    //DumpFunctionSet();
}





/// This will perform a recursive search through a node tree and place the
/// names of any functions it finds in the list mFunctionSet, for
/// later use by the verify routine. 
void Loader::FindFunctions(const PNode * node)
{


    //First, determine if this is a data node; if it is,
    //see if it is a function name.
    if(node->GetType() == PEBL_DATA_NODE) 
        {
            //cout << "A Data node." << endl;
            if(((DataNode*)node)->GetDataType() == P_DATA_FUNCTION)
                {
                    //cout << "\tA Function Node: " << *node << endl;
                    //If the node is a function name, insert the name into the 
                    //functionname set

                    //This cast should be done in a smarter way:
                    Variant v = ((DataNode*)(node))->GetValue();


                    //Now, convert the function name in v to upper case
                    //It is really just a string.
                    std::string ucasename = PEBLUtility::ToUpper(v);

                    Variant v2 = Variant(ucasename.c_str(), P_DATA_FUNCTION);

                    //cout << "\tValue: " << v2 << "|" << v2.GetDataTypeName() <<  endl;
                    //std::pair<std::set<Variant>::iterator, bool> myPair;
                    //myPair = mFunctionSet.insert(v2);
                     mFunctionSet.insert(v2);
                    
                     // cout << "Result: " << *(myPair.first) << " " <<  (myPair.second) << endl;
                     //DumpFunctionSet();
                }
        }
    else
        {

            //Otherwise, it is an OpNode.  Recurse on the left and right nodes.
   
            PNode * node1 = ((OpNode*)node)->GetLeft();


            if(node1) 
                {
                    FindFunctions(node1);
                }

            node1 = ((OpNode*)node)->GetRight();
            if(node1)
                {
                    FindFunctions(node1);
                }
        }
}


/// This marches through the topmost nodes of the PNode tree and 
/// puts functions in the mFunctionMap.  These nodes will all be either
/// PEBL_FUNCTIONS or PEBL_FUNCTION.
void Loader::LoadUserFunctions(OpNode * node)
{

    //This will only parse down to PEBL_FUNCTION
    //OpNodes, and so should only include two types:
    //PEBL_FUNCTION and PEBL_FUNCTIONS.

    OpNode *node1;  

    //If this is a PEBL_FUNCTIONS node, the left is either a PEBL_FUNCTIONS or
    // a PEBL_FUNCTION and the right is either a  PEBL_FUNCTIONS
    //or a NULL.  If null, we are done.
    

    //Add the PEBL_FUNCTION
    node1 = (OpNode*)(node->GetLeft());
    if(node1->GetOp() == PEBL_FUNCTION)
        {
            
            Evaluator::mFunctionMap.AddFunction( ((DataNode*)(((OpNode*)node1)->GetLeft()))->GetValue(),
                                                 (OpNode*)node1->GetRight());      
        }
    else if (node1->GetOp() == PEBL_FUNCTIONS)
        {
            LoadUserFunctions(node1);
        }
    else
        {
            cerr << "PEBL_FUNCTION is " << PEBL_FUNCTION << endl;
            cerr << "PEBL_FUNCTIONS is " << PEBL_FUNCTIONS << endl;
            cerr << "Unknown Node type in Loader::LoadUserFunctions: " << node1->GetOp() << endl;
            PError::SignalFatalError("Unknown Node Type");
        }
    //Get the PEBL_FUNCTIONS node and recurse
    node1 = (OpNode*)(node->GetRight());
    if(node1) 
        {
            LoadUserFunctions(node1);
        }

}



/// If any functions found by FindFunctions are in libraries, this
/// method will load them properly. The method iterates through mFunctionSet,
/// checking each item first against mFunctionMap to see if it has already been
/// loaded (e.g., user functions), and then each of the standard function libraries
void Loader::LoadLibraryFunctions()
{
    //Get an iterator to the first item
    std::set<Variant>::iterator p;
    p=mFunctionSet.begin();
    
    PEBL_Function_Type * functionTable = NULL;

    while(p != mFunctionSet.end())
        {
            //Check inside mFunctionMap, to make sure it hasn't been found yet; 
            //If it is found, it may be a user-defined function (as in Start), or
            //one that has already been loaded previously.

            string name = (*p).GetFunctionName();
            //cout << "Checking [" << name <<"]"<< endl;
            if(!(Evaluator::mFunctionMap.IsFunction(name)) )
                {
                    //cout <<"isfuncion"<< endl;
                    bool functionNotFound = true;
                    
                    //Go through each library, searching for the function.
                    for(int library = 0; library < 6; library++)
                        {
                            switch(library)
                                {
                                case 0:
                                    //********************Check the Math Library******************
                                    functionTable = PEBLMath::FunctionTable;
                                    break;
                                    
                                case 1:
                                    //********************Check the Stream Library******************
                                    functionTable = PEBLStream::FunctionTable;
                                    break;

                                case 2:
                                    //********************Check the Objects Library******************
                                    functionTable = PEBLObjects::FunctionTable;
                                    break;
                                    
                                case 3:
                                    //********************Check the Environment Library******************
                                    functionTable = PEBLEnvironment::FunctionTable;
                                    break;
                                    
                                case 4:
                                    //********************Check the List Library******************
                                    functionTable = PEBLList::FunctionTable;
                                    break;
                                case 5:
                                    functionTable = PEBLString::FunctionTable;
                                    break;
                                default:
                                    PError::SignalFatalError("Library Not Found.");
                                }
                        

                            int i = 0;

                            while (functionTable[i].name && functionNotFound)
                                {
                                    //cout << "checking name:[" <<name<< "]"<<endl;
                                    bool globalnamespace = true;
                                    string name2 = name;
                                    if(name[0]== ':')
                                        {
                                            name2=name.substr(1,name.length());
                                        }
                                    //cout << "checking name:[" <<name2<< "]"<<endl;
                                    if(functionTable[i].name == name2)
                                        {
                                            //Its a match. Construct a top-level function node with pieces below it
                                            //that can be processed appropriately by the Evaluator. 
                                            
                                            //The filename and linenumber are irrelevant; this is not tied directly
                                            //to the source code.
                                            string filename = "<PEBL STANDARD LIBRARY FILE>";
                                            int linenum = -1;

                                            //Make a DataNode with numargs to signal the number of arguments required by the function; connect
                                            //them with an AND node.
                                            PNode * node0a = new DataNode(Variant(functionTable[i].minNumArgs),filename, linenum);
                                            PNode * node0b = new DataNode(Variant(functionTable[i].maxNumArgs),filename, linenum);
                                            PNode * node0  = new OpNode(PEBL_AND, node0a, node0b,filename, linenum);

                                            //Make a DataNode with a functionpointer variant in it.
                                            PNode * node1 = new DataNode(functionTable[i].funcname,filename, linenum);


                                            if(globalnamespace)
                                                {
                                                    //if we call a :Function(), thise aren't
                                                    //declared specifically, but implicitly created on-demand at this point.

                                                    OpNode *   node2 = new OpNode(PEBL_LIBRARYFUNCTION, node0, node1,filename, linenum);
                                                    Variant newname = name;
                                                    node2->SetFunctionName(newname);
                                                    Evaluator::mFunctionMap.AddFunction(newname,node2);
                                                    // cout << "2\n";

                                                }else{
                                                //this is a normal function
                                                // Make a new OpNode with the *NumArgs on the left and the function pointer on the right.
                                                OpNode * node2 = new OpNode(PEBL_LIBRARYFUNCTION, node0, node1,filename, linenum);
                                                node2->SetFunctionName(*p);
                                                Evaluator::mFunctionMap.AddFunction(*p,node2);
                                            }

                                            
                                            //Add a :Function version that will always be callable, because it can't be overwritteen
                                            //with define().  This lets you override Draw() but call :Draw() inside it.

                                            /*
                                              node2 = new OpNode(PEBL_LIBRARYFUNCTION, node0, node1,filename, linenum);
                                              Variant newname = Variant(":")+(*p);
                                              node2->SetFunctionName(newname);
                                              Evaluator::mFunctionMap.AddFunction(newname,node2);
                                              cout << "2\n";
                                            */
                                            
                                            //break out of the while--we are done.
                                            functionNotFound = false;
                                        }
                                    
                                    i++;
                                }
                        }
                    

                    //At this point, if functionNotFound is still true, we are in trouble.


                    //If this happens, we should parse all .pbl files in pebl-lib,
                    //run FindFunctions on it, and try again.
                    if(functionNotFound == true)
                        {
                            string  message =  "Function [" ;
                            message +=  name;
                            message +=  "] not found in libraries or user-defined functions.";
                            Evaluator::mFunctionMap.DumpValues();
                            std::cerr << "********>>>>>ERROR: " << message << std::endl;
                            std::cerr.flush();
#ifdef PEBL_EMSCRIPTEN
                            EM_ASM({
                                console.error("MISSING FUNCTION: " + UTF8ToString($0));
                            }, message.c_str());
#endif
                            PError::SignalFatalError(message);
                        }
                    
                }
            else
                {


                    
                    //In this case, the named function WAS found in the functionmap.
                }

            //Move on to the next function to load.
            p++;
        }

}

///This method checks to see whether everything is ready for a
///potentially error-free run.  This includes determining whether
///all of the used functions actually exist, and if there is an
///entry function, and perhaps other stuff tbd later.
bool Loader::Verify()
{
  
    return true;
}


PNode * Loader::GetMainPEBLFunction()
{
    // Get the main PEBL Function, returning it 
    // or 0 if it doesn't exist.
    PNode * node = Evaluator::mFunctionMap.GetFunction("START");
    if (node)
        return node;
    else
        return 0;
}



void Loader::DumpFunctionSet()
{
    //Get an iterator to the first item
    std::set<Variant>::iterator p;
    p=mFunctionSet.begin();
    
    cerr << "\t-----------------------------------------------------------\n";
    cerr << "\tPrinting all functions in mFunctionSet\n";
    while(p != mFunctionSet.end())
        {
            cerr << "\t\tFunction: ["<<  *p << "]\n";         
            p++;
        }
    cerr << "\t-----------------------------------------------------------\n";

    
}
