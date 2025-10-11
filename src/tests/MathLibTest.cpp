//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       VariantTest.cpp
//    Purpose:    Tests the ../libs/PEBLMath.cpp library functions
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
#include "../base/Variant.h"
#include "../base/PComplexData.h"
#include "../base/PList.h"
#include "../libs/Functions.h"
#include "../libs/PEBLMath.h"

#include <iostream>



/// This tests the functions in libs/PEBLMath.cpp
///


using std::cout;
using std::endl;
using std::flush;

using namespace PEBLMath;


int main(int argc, char **argv)
{

    cout << "\n\n\n==========================================================\n";
    cout << "Testing MathLib\n";
    cout << "==========================================================\n";

    
    Variant myVariant1 = 32;
    Variant myVariant2 = 3431252.123;
    Variant myVariant3 = .000002115;
    Variant myVariant4 = 0;


 

    
    PList * list1=new PList();
    list1->PushFront(myVariant1);
    list1->PushFront(myVariant2);
    list1->PushFront(myVariant3);
    list1->PushFront(myVariant4);
     
    PList * list2 = new PList(); list2->PushFront(myVariant1);
    PList * list3 = new PList(); list3->PushFront(myVariant2);
    PList * list4 = new PList(); list4->PushFront(myVariant3);


    Variant v1 = Variant(list1);
    Variant v2 = Variant(list2);
    Variant v3 = Variant(list3);
    Variant v4 = Variant(list4);

    //make a list of variants.
    PList * list5 = new PList();
    list5->PushFront(v1);
    list5->PushFront(v2);
    list5->PushFront(v3);
    list5->PushFront(v4);
    
    Variant v5 = Variant(list5);
    cout << "Variants initiated" << endl;


    cout << "\n\n\n==========================================================\n";
    cout << "Checking Basic Math Functions\n";
    cout << "==========================================================\n";


    cout << "\n\n---------------------Log10-------------------------" << endl;
    cout << v1 << ": <" << Log10(v1) << ">\n";
    cout << v2 << ": <" << Log10(v2)  << ">\n";
    cout << v3 << ": <" << Log10(v3)  << ">\n";
    cout << v4 << ": <" << Log10(v4)  << ">\n";
    cout << v5 << ": <" << Log10(v5)  << ">\n";


    cout << "\n\n---------------------Log2-------------------------" << endl;
    cout << v1 << ": <" << Log2(v1) << ">\n";
    cout << v2 << ": <" << Log2(v2)  << ">\n";
    cout << v3 << ": <" << Log2(v3)  << ">\n";
    cout << v4 << ": <" << Log2(v4)  << ">\n";
    cout << v5 << ": <" << Log2(v5)  << ">\n";

    cout << "\n\n---------------------Ln-------------------------" << endl;
    cout << v1 << ": <" << Ln(v1) << ">\n";
    cout << v2 << ": <" << Ln(v2)  << ">\n";
    cout << v3 << ": <" << Ln(v3)  << ">\n";
    cout << v4 << ": <" << Ln(v4)  << ">\n";
    cout << v5 << ": <" << Ln(v5)  << ">\n";


    cout << "\n\n---------------------Exp-------------------------" << endl;
    cout << v1 << ": <" << Exp(v1) << ">\n";
    cout << v2 << ": <" << Exp(v2)  << ">\n";
    cout << v3 << ": <" << Exp(v3)  << ">\n";
    cout << v4 << ": <" << Exp(v4)  << ">\n";
    cout << v5 << ": <" << Exp(v5)  << ">\n";


    cout << "\n\n---------------------Sqrt-------------------------" << endl;
    cout << v1 << ": <" << Sqrt(v1) << ">\n";
    cout << v2 << ": <" << Sqrt(v2)  << ">\n";
    cout << v3 << ": <" << Sqrt(v3)  << ">\n";
    cout << v4 << ": <" << Sqrt(v4)  << ">\n";
    cout << v5 << ": <" << Sqrt(v5)  << ">\n";


    cout << "\n\n---------------------Tan-------------------------" << endl;
    cout << v1 << ": <" << Tan(v1) << ">\n";
    cout << v2 << ": <" << Tan(v2)  << ">\n";
    cout << v3 << ": <" << Tan(v3)  << ">\n";
    cout << v4 << ": <" << Tan(v4)  << ">\n";
    cout << v5 << ": <" << Tan(v5)  << ">\n";



    cout << "\n\n---------------------Sin-------------------------" << endl;
    cout << v1 << ": <" << Sin(v1) << ">\n";
    cout << v2 << ": <" << Sin(v2)  << ">\n";
    cout << v3 << ": <" << Sin(v3)  << ">\n";
    cout << v4 << ": <" << Sin(v4)  << ">\n";
    cout << v5 << ": <" << Sin(v5)  << ">\n";



    cout << "\n\n---------------------Cos-------------------------" << endl;
    cout << v1 << ": <" << Cos(v1) << ">\n";
    cout << v2 << ": <" << Cos(v2)  << ">\n";
    cout << v3 << ": <" << Cos(v3)  << ">\n";
    cout << v4 << ": <" << Cos(v4)  << ">\n";
    cout << v5 << ": <" << Cos(v5)  << ">\n";



    cout << "\n\n---------------------ATan-------------------------" << endl;
    cout << v1 << ": <" << ATan(v1) << ">\n";
    cout << v2 << ": <" << ATan(v2)  << ">\n";
    cout << v3 << ": <" << ATan(v3)  << ">\n";
    cout << v4 << ": <" << ATan(v4)  << ">\n";
    cout << v5 << ": <" << ATan(v5)  << ">\n";



    cout << "\n\n---------------------ASin-------------------------" << endl;
    cout << v1 << ": <" << ASin(v1) << ">\n";
    cout << v2 << ": <" << ASin(v2)  << ">\n";
    cout << v3 << ": <" << ASin(v3)  << ">\n";
    cout << v4 << ": <" << ASin(v4)  << ">\n";
    cout << v5 << ": <" << ASin(v5)  << ">\n";




    cout << "\n\n---------------------ACos-------------------------" << endl;
    cout << v1 << ": <" << ACos(v1) << ">\n";
    cout << v2 << ": <" << ACos(v2)  << ">\n";
    cout << v3 << ": <" << ACos(v3)  << ">\n";
    cout << v4 << ": <" << ACos(v4)  << ">\n";
    cout << v5 << ": <" << ACos(v5)  << ">\n";



    return 0;
}

