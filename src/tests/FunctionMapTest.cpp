//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       FunctionMapTest.cpp
//    Purpose:    Tests the FunctionMap Class
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
#include "../base/PNode.h"
#include "../base/FunctionMap.h"
#include "../base/Variant.h"
#include "../base/grammar.tab.hpp"

#include <iostream>
#include <stdio.h>


//The following is the entry point for the command-line version
int main(int argc, char **argv)
{

    using std::cout;
    using std::endl;
    using std::flush;
	
    long int a = 12341;
    long double b = 3352.9933;

    cout << "\n\n\n==========================================================\n";
    cout << "Testing the FunctionMap Class\n";
    cout << "==========================================================\n";


    cout << "\n\n\n==========================================================\n";
    cout << "Making a new FunctionMap\n";
    cout << "==========================================================\n";
    FunctionMap myFMap;  cout << "." << flush;

    cout << "\n\n\n==========================================================\n";
    cout << "Making some OpNodes\n";
    cout << "==========================================================\n"; 
    PNode  *myPNode1 = new OpNode(PEBL_ADD,0,0);  cout << "." << flush;
    PNode  *myPNode2 = new OpNode(PEBL_DIVIDE,0,0);  cout << "." << flush;
    PNode  *myPNode3 = new OpNode(PEBL_ELSE, 0,0);  cout << "." << flush;
    PNode  *myPNode4 = new OpNode(PEBL_GE,0,0);  cout << "." << flush;
    PNode  *myPNode5 = new OpNode(PEBL_EQ,0,0);  cout << "." << flush;
    PNode  *myPNode6;

    cout << *myPNode1 << endl;
    cout << *myPNode2 << endl;
    cout << *myPNode3 << endl;
    cout << *myPNode4 << endl;
    cout << *myPNode5 << endl;

    cout << "\n\n\n==========================================================\n";
    cout << "Adding nodes to map\n";
    cout << "==========================================================\n";
    myFMap.AddFunction("one", (OpNode*)myPNode1);  cout << "." << flush;
    myFMap.AddFunction("two",(OpNode*)myPNode2);  cout << "." << flush;
    myFMap.AddFunction("four",(OpNode*)myPNode3);  cout << "." << flush;
    myFMap.AddFunction("four", (OpNode*)myPNode4);  cout << "." << flush;
    myFMap.AddFunction("four", (OpNode*)myPNode5);  cout << "." << flush;

    cout << "\n\n\n==========================================================\n";
    cout << "Dumping entire values from map.\n"; 
    cout << "==========================================================\n";
    myFMap.DumpValues();
 
    cout << "\n\n\n==========================================================\n";
    cout << "Testing basic retrieval\n";
    cout << "==========================================================\n";
    myPNode6 = myFMap.GetFunction("four");
    cout << " Retrieved four:  " << *myPNode6 << endl;

    myPNode6 = myFMap.GetFunction("two");
    cout << " Retrieved two:  " << *myPNode6 << endl;

    myPNode6 = myFMap.GetFunction("one");
    cout << " Retrieved one:  " << *myPNode6 << endl;

    myPNode6 = myFMap.GetFunction("nobody");
    cout << " Retrieved nobody:  " << *myPNode6 << endl;




    cout << "\n\n\n==========================================================\n";
    cout << "Testing Deleting\n";
    cout << "==========================================================\n";

    myFMap.DumpValues(); cout << "--\n";

    cout << "Deleting 'four'" << endl;
    myFMap.Erase("four"); myFMap.DumpValues();cout << "---------------------------\n";
    cout << "Deleting 'two'" << endl;
    myFMap.Erase("two"); myFMap.DumpValues();cout << "---------------------------\n";
    cout << "Deleting 'one'" << endl;
    myFMap.Erase("one"); myFMap.DumpValues();cout << "---------------------------\n";
    cout << "Deleting 'one'" << endl;
    myFMap.Erase("one"); myFMap.DumpValues();cout << "---------------------------\n";
    cout << "Deleting 'one'" << endl;
    myFMap.Erase("one"); myFMap.DumpValues();cout << "---------------------------\n";
    cout << "Deleting 'four'" << endl;
    myFMap.Erase("four"); myFMap.DumpValues();cout << "---------------------------\n";
    cout << "Deleting 'four'" << endl;
    myFMap.Erase("four"); myFMap.DumpValues();cout << "---------------------------\n";

  


    cout << "==========================================================\n";
    cout << "Finished Function Map Tests" << endl;
    cout << "==========================================================\n";
    cout << "==========================================================\n";
    cout << "==========================================================\n";
    cout << "==========================================================\n";
    cout << "==========================================================\n";
    return 0;
}

