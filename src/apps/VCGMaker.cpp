//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/apps/Variant.cpp
//    Purpose:    Makes VCG file from a Pebl program.
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
#include "../base/PNode.h"
#include "../base/grammar.tab.hpp"
#include "../base/VCG.h"

#include "../base/Evaluator.h"
#include "../devices/PEventLoop.h"

#include <iostream>


using std::cerr;
using std::cout;
using std::endl;

PNode *  parse(const char* filename);

///Initiate some static member data.
FunctionMap Evaluator::mFunctionMap;
PEventLoop Evaluator::mEventLoop; 
VariableMap Evaluator::gGlobalVariableMap;
const PNode * Evaluator::gEvalNode = NULL;
PEBLPath  Evaluator::gPath;

int PEBLInterpret( int argc, char **argv )
{ 

    PNode * tmp = NULL;
    
    //Process the first command-line argument.
    cerr << "Processing PEBL Source File: " <<  argv[1] << endl;
    PNode * head  = parse(argv[1]);
    
    //If there are any more arguments, process them by accomodating them
    //inside a function list.
    for(int i = 2; i<argc; i++)
        {
            cerr << "Processing PEBL Source File: " <<  argv[i] << endl;
            
            //Make a new node.
            tmp = parse(argv[i]);
            
            //Now, make a new node that contains head and tmp.
            head = new OpNode(PEBL_FUNCTIONS, head, tmp, "Unknown file",0);
        }
    
    
    //Now, head should point to the top of the tree containing all of the source.
    if(head) 
        {
            cerr << "---------Encoding VCG GRAPH---------" << endl;   
            VCG myVCG;
            myVCG.MakeGraph(head);
      
            cerr << "---------Printing VCG GRAPH---------" << endl;   
            myVCG.PrintGraph();
      
            cerr << "---------FINISHED VCG GRAPH---------" << endl;   

            return 0;
        }
  
    cerr << "Error: parsed node was empty" << endl;
    return 1;
}



//The following is the entry point for the command-line version
int main(int argc, char **argv)
{
    return PEBLInterpret(argc, argv);
}

