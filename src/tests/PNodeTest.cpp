//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       PNodeTest.cpp
//    Purpose:    Tests the Variant Class
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
#include "../base/Variant.h"
#include "../base/grammar.tab.hpp"
#include <iostream>

using std::cout;
using std::endl;
using std::flush;


//The following is the entry point for the command-line version
int main(int argc, char **argv)
{
    long int    a = 12341;
    long double b = 3352.9933;


    cout << "\n\n\n==========================================================\n";
    cout << "Testing the PNode Class\n";
    cout << "==========================================================\n";

    /* The PNode test is pretty lame because the class doesn't do much
     * in and of itself.  Probably should use an Evaluator to test it 
     * better.  
     */

    cout << "Constructing Variants" << endl;
    Variant  v1 = 27;               //An integer Variant
    Variant  v2 = 343.1234;         //A float Variant
    Variant  v3 = Variant("Potato",P_DATA_VARIABLE);//A Variable variant


    cout << "\n\n\n---------------------------------------------\n";
    cout << "Creating new PNodes (All Datanodes)";
    cout << "\n\n\n---------------------------------------------\n";

    cout << "Creating p0" << endl;
    PNode * p0 = new DataNode();

 
    cout << "Creating p1" << endl;
    PNode *    p1 = new DataNode(v2);

    cout << "Creating p2" << endl;
    PNode *    p2 = new DataNode(v2);

    cout << "\n\n\n---------------------------------------------\n";
    cout << "Printing PNodes (All Datanodes)\n";
    cout << "---------------------------------------------\n";

    cout << *p0  << endl;
    cout << *p1 << endl;
    cout << *p2 << endl;



    cout << "\n\n\n---------------------------------------------\n";
    cout << "Creating p3" << endl;
    cout << "---------------------------------------------\n";

    DataNode *  p3 = new DataNode(v3);
   

    cout << "Retrieving the variant in P3" << endl;
    v2 =p3->GetValue();
    cout << "value: " << flush << v2 << endl;

   
    cout << "---------------------------------------------\n";
    //-----------------------------------------------
    cout << "Creating p4" << endl;
    PNode * p4 = new OpNode(PEBL_ADD, p0, p1);
   
    cout << "Creating p5" << endl;
    PNode * p5 = new OpNode(PEBL_SUBTRACT, p2, p3);
   
    cout << "Creating p6" << endl;
    PNode * p6 = new OpNode(PEBL_ADD, p4, p5);
   
 
    cout << "\n\n\n---------------------------------------------\n";
    cout << "Printing PNodes (Datanodes and Opnodes)\n";
    cout << "---------------------------------------------\n";

    cout << *p0  << endl;
    cout << *p1 << endl;
    cout << *p2 << endl;


    cout << *p4  << endl;
    cout << *p5 << endl;
    cout << *p6 << endl;


  
    cout << "\n\n\n---------------------------------------------\n";
    cout << "Realigning nodes.\n";
    cout << "---------------------------------------------\n";

    p0 = ((OpNode*)p6)->GetRight();
    p1 = ((OpNode*)p5)->GetRight();

    cout << *p0 << endl;
    cout << *p1 << endl;

    cout << "Finished" << endl;
    return 0;
}

