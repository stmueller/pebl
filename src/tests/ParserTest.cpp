//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       ParserTest.cpp
//    Purpose:    This tests the parser/evaluator
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003 Shane T. Mueller <smueller@umich.edu>
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

#include "../base/Evaluator.h"
#include "../base/grammar.tab.hpp"
#include "../base/PNode.h"
#include "../base/Loader.h"
#include "../base/PComplexData.h"
#include "../base/PList.h"
#include "../libs/PEBLObjects.h"
#include <iostream>

using std::cerr;
using std::endl;

PNode *  parse();

int PEBLInterpret( int argc, char **argv )
{ 

    //This sets the video driver
    setenv("SDL_VIDEODRIVER", "dga",1);  //Requires to run with root privileges, fullscreen.
    setenv("SDL_VIDEODRIVER", "x11",1);

    Loader* myLoader;
    FunctionMap * myFunctionMap= new FunctionMap();

    PNode *node1;
    node1=NULL;
  

    //First, parse the file.
    node1 = parse();
    cerr << "---------Program Parsed---------" << endl;   

    cerr << "---------Loading Program---------" << endl;     
    //Now, load it into the environment.
    myLoader = new Loader(myFunctionMap);
  
    myLoader->LoadUserFunctions((OpNode*)node1);

    cerr <<"Analyzing code for functions." << endl;
    myLoader->FindFunctions(node1);

    cerr << "Loading Library functions." << endl;
    myLoader->LoadLibraryFunctions();

    cerr << "Loaded Functions: " << endl;
    myLoader->DumpFunctionSet();



    cerr << "Creating a PlatformEnvironment\n";
    PEBLObjects::MakeEnvironment();


    cerr << "---------Creating Evaluator-----" << endl;
    
    PList * pList = new PList();
    pList->PushFront(Variant(0));

    cout << "Getting list inside list: " << *pList <<endl;

    PComplexData * pcd = new PComplexData(pList);
    Variant v = Variant(pcd);

    Evaluator myEval = Evaluator(myFunctionMap, v);
    //Now, everything should be F-I-N-E fine.
  
    cerr << "--------Getting main function---" << endl;
    node1 = myLoader->GetMainPEBLFunction();

    if(node1) 
        {
    
            cerr << "---------Evaluating Program-----" << endl;
            //Execute everything
            myEval.Evaluate(node1);
            return 0;
        }
    else
        {
            cerr << "Error: Can't evaluate program" << endl;
            return 1;
        }

}




int main(int argc, char **argv)
{
    cout << "This testing program is deprecated.\n";
    
    //return PEBLInterpret(argc, argv);
    return 0;
}

