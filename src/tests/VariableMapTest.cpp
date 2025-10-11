//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       VariableMapTest.cpp
//    Purpose:    Tests the VariableMap Class
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
#include "../base/FunctionMap.h"
#include "../base/grammar.tab.hpp"
#include "../base/Loader.h"

#include "../base/PComplexData.h"
#include "../base/PList.h"
#include "../base/PNode.h"

#include "../base/Variant.h"
#include "../base/VariableMap.h"


#include "../devices/PEventLoop.h"

#include "../libs/PEBLObjects.h"

#include "../utility/PError.h"
#include "../utility/PEBLPath.h"
#include "../utility/PEBLUtility.h"
#include "../utility/rc_ptrs.h"


#include <iostream>
#include <stdio.h>


using std::cout;
using std::endl;
using std::flush;

  
///Initiate some static member data.
FunctionMap Evaluator::mFunctionMap;
PEventLoop Evaluator::mEventLoop; 
VariableMap Evaluator::gGlobalVariableMap;
const PNode * Evaluator::gEvalNode = NULL;
PEBLPath  Evaluator::gPath;


//The following is the entry point for the command-line version
int main(int argc, char **argv)
{
    VariableMap myVMap;
    Variant v(33);
    myVMap.AddVariable("one", v);
    cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    myVMap.DumpValues();
    cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    cout << "=========================Real test\n";
    cout << "========================="<< myVMap.RetrieveValue("one") << endl;
    Variant v2;
    v2 = myVMap.RetrieveValue("one");
    cout << "========================="<< v2 << endl;

    PlatformImageBox * pib = new PlatformImageBox();
    counted_ptr<PEBLObjectBase> pob = counted_ptr<PEBLObjectBase>(pib);
    PComplexData * pcd = new PComplexData(pob);
    Variant v3 = Variant(pcd);
    myVMap.AddVariable("image",v3);
    cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    myVMap.DumpValues();
    cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    Variant v4 = v3;
    cout << "Copied variant:" <<  v4;
    Variant v5;


    cout <<"Extracted: " <<  myVMap.RetrieveValue("image") << endl;;
    v5 = myVMap.RetrieveValue("image");
    cout << "Extracted/assigned variant: " << v5 << endl;

}
int extra (){
        
    long int a = 12341;
    long double b = 3352.9933;

    cout << "\n\n\n==========================================================\n";
    cout << "Testing the VariableMap Class\n";
    cout << "==========================================================\n";


    cout << "\n\n\n==========================================================\n";
    cout << "Testing basic encoding\n";
    cout << "==========================================================\n";
    
    VariableMap myVMap;  cout << "." << flush;
    
    Variant myVariant1(34);  cout << "." << flush;
    Variant myVariant2(33.5252);  cout << "." << flush;
    Variant myVariant3(0);  cout << "." << flush;
    int i;

    myVMap.AddVariable("one", myVariant1);  cout << "." << flush;
    myVMap.AddVariable("two", myVariant2);  cout << "." << flush;
    myVMap.AddVariable("three", myVariant3);  cout << "." << flush;
    myVMap.AddVariable("four", myVariant1);  cout << "." << flush;


    cout << "\n\n\n==========================================================\n";
    cout << "Testing basic retreival\n";
    cout << "==========================================================\n";

    myVariant3 = myVMap.RetrieveValue("one");
    cout << " Retrieved one:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("two");
    cout << " Retrieved two:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("three");
    cout << " Retrieved three:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("four");
    cout << " Retrieved four:  " << myVariant3 << endl;


    cout << "\n\n\n==========================================================\n";
    cout << "Testing overwriting\n";
    cout << "==========================================================\n";


    myVMap.AddVariable("one", myVariant2);
    myVMap.AddVariable("two", myVariant3);
    myVMap.AddVariable("three", myVariant1);
    myVMap.AddVariable("four", myVariant2+ myVariant1);


    myVariant3 = myVMap.RetrieveValue("one");
    cout << " Retrieved one:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("two");
    cout << " Retrieved two:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("three");
    cout << " Retrieved three:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("four");
    cout << " Retrieved four:  " << myVariant3 << endl;



    cout << "\n\n\n==========================================================\n";
    cout << "Testing retrieval of undefined\n";
    cout << "==========================================================\n";

    myVariant3 = myVMap.RetrieveValue("oney");
    cout << " Retrieved oney:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("TWO");
    cout << " Retrieved TWO:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("about");
    cout << " Retrieved about:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("something");
    cout << " Retrieved something:  " << myVariant3 << endl;

    cout << "\n\n\n==========================================================\n";
    cout << "Testing DumpValues Method\n";
    cout << "==========================================================\n";
  
    myVMap.DumpValues();


    cout << "\n\n\n==========================================================\n";
    cout << "Performance test: 1000000 reencodings\n";
    cout << "==========================================================\n";

    for(i = 0; i<1000000; i++)
        {
            myVMap.AddVariable("one", myVariant2);
            if(i%1000==0) cout << "." << flush;
        }
    cout << "Finished 1000000 reencodings" << endl;
  



    cout << "\n\n\n==========================================================\n";
    cout << "Performance test: 1000000 reencodings with creation of new Variant\n";
    cout << "==========================================================\n";

    for(i = 0; i<1000000; i++)
        {
            myVMap.AddVariable("one", Variant(3435.2));
            if(i%1000==0) cout << "." << flush;
        }
    cout << "Finished 1000000 reencodings" << endl;
  


    cout << "\n\n\n==========================================================\n";
    cout << "Performance test: 1000000 retrievals\n";
    cout << "==========================================================\n";

    for(i = 0; i<1000000; i++)
        {
            myVariant2 = myVMap.RetrieveValue("one");
            if(i%1000==0) cout << "." << flush;
        }
    cout << "Finished 1000000 retrievals" << endl;



    cout << "\n\n\n==========================================================\n";
    cout << "Performance test: 1000000 Unique encodings\n";
    cout << "==========================================================\n";

    char name[20];
    for(i = 0; i<1000000; i++)
        {

            sprintf(name, "%10d",i);
      
            myVMap.AddVariable(name, Variant(i));
            if(i%1000==0) cout << "." << flush;
        }
    cout << "Finished 1000000  Unique encodings" << endl;
  
  

    cout << "\n\n\n==========================================================\n";
    cout << "Performance test: 1000000 Unique retrievals\n";
    cout << "==========================================================\n";


    for(i = 999999; i>=0; i--)
        {
            sprintf(name, "%10d",i);
            myVariant2 = myVMap.RetrieveValue(name);
            if(i%1000==0) cout << "." << flush;
        }
    cout << "Finished 1000000  Unique retrievals" << endl;
  

    cout << "\n\n\n==========================================================\n";
    cout << "Performance test: 100000 Unique erases\n";
    cout << "==========================================================\n";


    for(i = 999999; i>=0; i--)
        {
            sprintf(name, "%10d",i);
            myVMap.Erase(name);
            if(i%1000==0) cout << "." << flush;
        }
    cout << "Finished 1000000  Unique retrievals" << endl;


    cout << "==========================================================\n";
    cout << "Dumping Remaining Values:" << endl;
    cout << "==========================================================\n";
    myVMap.DumpValues();
  



    cout << "\n\n\n==========================================================\n";
    cout << "Testing basic retrieval Again\n";
    cout << "==========================================================\n";

    myVariant3 = myVMap.RetrieveValue("one");
    cout << " Retrieved one:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("two");
    cout << " Retrieved two:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("three");
    cout << " Retrieved three:  " << myVariant3 << endl;
    myVariant3 = myVMap.RetrieveValue("four");
    cout << " Retrieved four:  " << myVariant3 << endl;




    cout << "==========================================================\n";
    cout << "Finished Variable Map Tests" << endl;
    cout << "==========================================================\n";
    cout << "==========================================================\n";
    cout << "==========================================================\n";
    cout << "==========================================================\n";
    cout << "==========================================================\n";
    return 0;
}

