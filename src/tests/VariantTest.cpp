//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       VariantTest.cpp
//    Purpose:    Tests the Variant Class
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003--2005 Shane T. Mueller <smueller@obereed.net>
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
#include "../base/Variant.h"
#include "../base/PComplexData.h"
#include "../base/PList.h"
#include "../base/Evaluator.h"
#include "../base/PNode.h"
#include "../base/VariableMap.h"
#include "../base/FunctionMap.h"

#include "../base/grammar.tab.hpp"

#include "../utility/PEBLPath.h"
#include "../devices/PEventLoop.h"

#include "../libs/PEBLObjects.h"
#include "../base/Evaluator.h"
#include "../base/Loader.h"



#include "../utility/PError.h"
#include "../utility/PEBLPath.h"
#include "../utility/PEBLUtility.h"
#include "../utility/rc_ptrs.h"

#include <iostream>
#include <stdlib.h>

using std::cout;
using std::endl;
using std::flush;


   PNode *  parse(const char* filename);
///Initiate some static member data.

FunctionMap Evaluator::mFunctionMap;
PEventLoop Evaluator::mEventLoop; 
const PNode * Evaluator::gEvalNode = NULL;
PEBLPath  Evaluator::gPath;
VariableMap Evaluator::gGlobalVariableMap;

//The following is the entry point for the command-line version
int main(int argc, char **argv[])
{


    long int a = 12341;
    long double b = 3352.9933;
    char c[15] = "Rock-n-roll"; 

    cout << "\n\n\n==========================================================\n";
    cout << "Testing the Variant Class\n";
    cout << "==========================================================\n";

    
    Variant myVariant1;
    Variant myVariant2;
    Variant myVariant3;



    myVariant1=a;
    myVariant2=a;
    cout << "Variants initiated" << endl;
    //==========================================================
    // Check Basic Assignment Functions
    //==========================================================
    cout << "\n\n\n==========================================================\n";
    cout << "Checking Basic Assignment Functions\n";
    cout << "==========================================================\n";
    
    cout << "myVariant1: ["<< a << "] == [" << (long int)myVariant1 << "] "   << myVariant1.GetDataTypeName() << endl;
    cout << "myVariant1: ["<< a << "] == [" << myVariant1 << "] "             << myVariant1.GetDataTypeName() << endl;
    cout << "myVariant2: ["<< a << "] == [" << (long int)myVariant2  << "]"   << myVariant2.GetDataTypeName() << endl;
    cout << "myVariant2: ["<< a << "] == [" << myVariant2 << "]"            << myVariant2.GetDataTypeName() << endl;
    
    myVariant2 = b;
    cout << "myVariant2: ["<< b << "] == [" << (long int)myVariant2  << "]"   << myVariant2.GetDataTypeName() << endl;
    cout << "myVariant2: ["<< b << "] == [" << myVariant2 << "]"            << myVariant2.GetDataTypeName() << endl;

  
    myVariant3 = c;
    cout << "myVariant3: ["<< c << "] == [" << (char*)myVariant3  << "]"   << myVariant3.GetDataTypeName() << endl;
    cout << "myVariant3: ["<< c << "] == [" << myVariant3 << "]"            << myVariant3.GetDataTypeName() << endl;

    cout << "Implicit Variant:" << Variant("IMPLITIC") << endl;

    //==========================================================
    // Check Basic Arithmetic operations
    //==========================================================
    cout << "\n\n\n==========================================================\n";
    cout << "Checking Basic Arithmetic operations.\n";
    cout << "==========================================================\n";
    
    cout << "V1 + V1: ["<< a + a << "] == [" << myVariant1 + myVariant1  << "]\n";
    cout << "V1 + V2: ["<< a + b << "] == [" << myVariant1 + myVariant2  << "] "  << endl;
    cout << "V1 - V2: ["<< a - b << "] == [" << myVariant1 - myVariant2  << "] "  << endl;
    cout << "V1 * V2: ["<< a * b << "] == [" << myVariant1 * myVariant2  << "] "  << endl;
    cout << "V1 / V2: ["<< a / b << "] == [" << myVariant1 / myVariant2  << "] "  << endl;

    cout << "V3 +  'cat': [" << myVariant3 + Variant("cat")  << "] "  << endl;
    

    
    //==========================================================
    // Check Basic Arithmetic operations with Assignment
    //==========================================================
    cout << "\n\n\n==========================================================\n";
    cout << "Check Basic Arithmetic operations with Assignment\n";
    cout << "==========================================================\n";
    
    myVariant3 = myVariant2 + myVariant1;
    cout << "V3:  " << myVariant3 << "  " << myVariant3.GetDataTypeName() << endl;
    
    myVariant3 = myVariant2 - myVariant1;
    cout << "V3:  " << myVariant3 << "  " << myVariant3.GetDataTypeName() << endl;
    
    myVariant3 = myVariant2 * myVariant1;
    cout << "V3:  " << myVariant3 << "  " << myVariant3.GetDataTypeName() << endl;
    
    myVariant3 = myVariant2 / myVariant1;
    cout << "V3:  " << myVariant3 << "  " << myVariant3.GetDataTypeName() << endl;
    
    myVariant3 = myVariant1 * Variant(2);
    cout << "V3:  " << myVariant3 << "  " << myVariant3.GetDataTypeName() << endl;
    

    cout << "Testing with int" << endl;
    myVariant2 = Variant(333);
    cout << "V2: [" << myVariant2 << "]" << endl;


    cout << "Implicit:" << Variant("Implicit") << endl;

    myVariant2 = "somebody ";
    cout << "V2: [" << myVariant2 << "]" << endl;


    myVariant2 = Variant("anybody ");
    cout << "V2: [" << myVariant2 << "]" << endl;

    //==========================================================
    //Checking <, >, ==, etc.
    //==========================================================
    
    cout << "\n\n\n==========================================================\n";
    cout << "Checking <, >, ==, etc.\n";
    cout << "==========================================================\n";


    myVariant1 = 34252.3;
    myVariant2 = 342;
       
  
    cout << "(myVariant1 <  myVariant2): "<< myVariant1 << " <  " << myVariant2 << "  " << (myVariant1 <  myVariant2) << endl;
    cout << "(myVariant1 >  myVariant2): "<< myVariant1 << " >  " << myVariant2 << "  " << (myVariant1 >  myVariant2) << endl;
    cout << "(myVariant1 <= myVariant2): "<< myVariant1 << " <= " << myVariant2 << "  " << (myVariant1 <= myVariant2) << endl;
    cout << "(myVariant1 >= myVariant2): "<< myVariant1 << " >= " << myVariant2 << "  " << (myVariant1 >= myVariant2) << endl;
    cout << "(myVariant1 == myVariant2): "<< myVariant1 << " == " << myVariant2 << "  " << (myVariant1 == myVariant2) << endl;
    cout << "(myVariant1 == myVariant1): "<< myVariant1 << " == " << myVariant1 << "  " << (myVariant1 == myVariant1) << endl; 
    cout << "(myVariant2 == myVariant2): "<< myVariant2 << " == " << myVariant2 << "  " << (myVariant2 == myVariant2) << endl; 
    cout << "(myVariant2 == myVariant1): "<< myVariant2 << " == " << myVariant1 << "  " << (myVariant2 == myVariant1) << endl;  
    cout << "(myVariant1 != myVariant2): "<< myVariant1 << " != " << myVariant2 << "  " << (myVariant1 != myVariant2) << endl;
    cout << "(myVariant1 != myVariant1): "<< myVariant1 << " != " << myVariant1 << "  " << (myVariant1 != myVariant1) << endl; 
    cout << "(myVariant2 != myVariant2): "<< myVariant2 << " != " << myVariant2 << "  " << (myVariant2 != myVariant2) << endl; 
    cout << "(myVariant2 != myVariant1): "<< myVariant2 << " != " << myVariant1 << "  " << (myVariant2 != myVariant1) << endl;  
    cout << "(myVariant2 <  myVariant1): "<< myVariant2 << " <  " << myVariant1 << "  " << (myVariant2 <  myVariant1) << endl;
    cout << "(myVariant2 >  myVariant1): "<< myVariant2 << " >  " << myVariant1 << "  " << (myVariant2 >  myVariant1) << endl;
    cout << "(myVariant2 <= myVariant1): "<< myVariant2 << " <= " << myVariant1 << "  " << (myVariant2 <= myVariant1) << endl;
    cout << "(myVariant2 >= myVariant1): "<< myVariant2 << " >= " << myVariant1 << "  " << (myVariant2 >= myVariant1) << endl;
  
    myVariant1 = "Something";
    myVariant2 = "Anything";

    cout << "(myVariant1 <  myVariant2): "<< myVariant1 << " <  " << myVariant2 << "  " << (myVariant1 <  myVariant2) << endl;
    cout << "(myVariant1 >  myVariant2): "<< myVariant1 << " >  " << myVariant2 << "  " << (myVariant1 >  myVariant2) << endl;
    cout << "(myVariant1 <= myVariant2): "<< myVariant1 << " <= " << myVariant2 << "  " << (myVariant1 <= myVariant2) << endl;
    cout << "(myVariant1 >= myVariant2): "<< myVariant1 << " >= " << myVariant2 << "  " << (myVariant1 >= myVariant2) << endl;
    cout << "(myVariant1 == myVariant2): "<< myVariant1 << " == " << myVariant2 << "  " << (myVariant1 == myVariant2) << endl;
    cout << "(myVariant1 == myVariant1): "<< myVariant1 << " == " << myVariant1 << "  " << (myVariant1 == myVariant1) << endl; 
    cout << "(myVariant2 == myVariant2): "<< myVariant2 << " == " << myVariant2 << "  " << (myVariant2 == myVariant2) << endl; 
    cout << "(myVariant2 == myVariant1): "<< myVariant2 << " == " << myVariant1 << "  " << (myVariant2 == myVariant1) << endl;  
    cout << "(myVariant1 != myVariant2): "<< myVariant1 << " != " << myVariant2 << "  " << (myVariant1 != myVariant2) << endl;
    cout << "(myVariant1 != myVariant1): "<< myVariant1 << " != " << myVariant1 << "  " << (myVariant1 != myVariant1) << endl; 
    cout << "(myVariant2 != myVariant2): "<< myVariant2 << " != " << myVariant2 << "  " << (myVariant2 != myVariant2) << endl; 
    cout << "(myVariant2 != myVariant1): "<< myVariant2 << " != " << myVariant1 << "  " << (myVariant2 != myVariant1) << endl;  
    cout << "(myVariant2 <  myVariant1): "<< myVariant2 << " <  " << myVariant1 << "  " << (myVariant2 <  myVariant1) << endl;
    cout << "(myVariant2 >  myVariant1): "<< myVariant2 << " >  " << myVariant1 << "  " << (myVariant2 >  myVariant1) << endl;
    cout << "(myVariant2 <= myVariant1): "<< myVariant2 << " <= " << myVariant1 << "  " << (myVariant2 <= myVariant1) << endl;
    cout << "(myVariant2 >= myVariant1): "<< myVariant2 << " >= " << myVariant1 << "  " << (myVariant2 >= myVariant1) << endl;
 
    //==========================================================
    //Checking Variable Type
    //==========================================================
    
    cout << "\n\n\n==========================================================\n";
    cout << "Checking Variable Type\n";
    cout << "==========================================================\n";
    
    char* name = "Apostrophe";
    myVariant2 = Variant(name,P_DATA_LOCALVARIABLE);
    cout << "Apostrophe: "<< name << " " << myVariant2 << myVariant2.GetDataTypeName() << endl;
    
    myVariant3 = Variant("Nonesty",P_DATA_LOCALVARIABLE);
    cout << "Nonesty: "<< flush << myVariant3 << flush << myVariant3.GetDataTypeName() << endl;

 




    //==========================================================
    //Checking List Type
    //==========================================================
    
    cout << "\n\n\n==========================================================\n";
    cout << "Checking List Type\n";
    cout << "==========================================================\n";

    //First, make a list 
   
    counted_ptr<PList> list1;
    myVariant1 = 33;
    
    list1=counted_ptr<PList>(new PList());
    list1->PushFront(myVariant1);
    list1->PushFront(myVariant2);
    list1->PushFront(myVariant3);
   
    PComplexData * pcd = new PComplexData(list1);
    Variant myVariant4 = Variant(pcd);
    
    cout << "MyVariant4 has type: " << myVariant4.GetDataTypeName() << endl;;


    cout << "Printing the list: [" << endl;
    cout << myVariant4 << endl;
    cout << "] List printed." << endl;

    
    //Testing copying
    cout << "Testing copying...." << endl;
    Variant myVariant5 = myVariant4;

    cout << "Type of myVariant5: " << myVariant5.GetDataTypeName() << endl;

      
    cout << "List copied.  Printing copy:" << endl;
    cout << myVariant5 << endl;



    cout << "Finished" << endl;
    return 0;
}

