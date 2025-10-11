//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       ColorTest.cpp
//    Purpose:    Tests the Color Class
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
#include "../objects/PColor.h"
#include "../objects/PObject.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

//The following is the entry point for the command-line version
int main(int argc, char **argv)
{

    cout << "\n\n\n==========================================================\n";
    cout << "Testing the PColor Class\n";
    cout << "==========================================================\n";

    cout << "Constructing Colors\n";
    PColor myColor1 = PColor();
    PColor myColor2 = PColor(myColor1);
    PColor myColor3 = PColor(1,2,3,4);
    PColor myColor4 = PColor(16712194); //Should be 1,2,3,4
    PColor myColor5 = PColor(0,0,1,0);
    PColor myColor6 = PColor("AliceBlue");        //The first color
    PColor myColor7 = PColor("YELLOW4");          //The last color
    PColor myColor8 = PColor("grey18");          
    PColor myColor9 = PColor("Bugouho");           //Doesn't exist



    cout << "==========================================================\n";
    cout << "Printing Colors\n";
    cout << myColor1 << endl;
    cout << myColor2 << endl;
    cout << myColor3 << endl;
    cout << myColor4 << endl;
    cout << myColor5 << endl;
    cout << myColor6 << endl;
    cout << myColor7 << endl;
    cout << myColor8 << endl;
    cout << myColor9 << endl;


    cout << "==========================================================\n";
    cout << "Printing Integer Colors\n";
    cout << "myColor1: " << myColor1 << "|" << myColor1.GetColor() << endl;
    cout << "myColor2: " << myColor2 << "|" << myColor2.GetColor() << endl;
    cout << "myColor3: " << myColor3 << "|" << myColor3.GetColor() << endl;
    cout << "myColor4: " << myColor4 << "|" << myColor4.GetColor() << endl;
    cout << "myColor5: " << myColor5 << "|" << myColor5.GetColor() << endl;
    cout << "myColor6: " << myColor6 << "|" << myColor6.GetColor() << endl;
    cout << "myColor7: " << myColor7 << "|" << myColor7.GetColor() << endl;
    cout << "myColor8: " << myColor8 << "|" << myColor8.GetColor() << endl;
    cout << "myColor9: " << myColor9 << "|" << myColor9.GetColor() << endl;



    cout << "==========================================================\n";
    cout << "Changing color of: "  << myColor1 << endl;;
    myColor1.SetRed(150);
    myColor1.SetGreen(175);
    myColor1.SetBlue(200);
    myColor1.SetAlpha(225);
    
    cout << "to: " << myColor1 << endl;

    
    cout << "==========================================================\n";
    cout << "Getting specific colors of " << myColor1 << endl;
    cout << "Red:    " << myColor1.GetRed() <<endl;
    cout << "Green:  " << myColor1.GetGreen()<<endl;
    cout << "Blue:   " << myColor1.GetBlue()<<endl;
    cout << "Alpha:  " << myColor1.GetAlpha()<<endl;
    cout <<endl;




    cout << "==========================================================\n";
    cout << "Changing color of: "  << myColor1 << endl;;
    myColor1.SetRed(150);
    myColor1.SetGreen(175);
    myColor1.SetBlue(200);
    myColor1.SetAlpha(225);
    
    cout << "to: " << myColor1 << endl;

    
    cout << "==========================================================\n";
    cout << "Getting specific colors of " << myColor1 << endl;
    cout << "Red:    " << myColor1.GetRed() <<endl;
    cout << "Green:  " << myColor1.GetGreen()<<endl;
    cout << "Blue:   " << myColor1.GetBlue()<<endl;
    cout << "Alpha:  " << myColor1.GetAlpha()<<endl;
    cout <<endl;




    cout << "==========================================================\n";
    cout << "Changing color of: "  << myColor1 << endl;;
    myColor1.SetColorByName("dodgerblue");
 
    
    cout << "to: " << myColor1 << endl;

    
    cout << "==========================================================\n";
    cout << "Getting specific colors of " << myColor1 << endl;
    cout << "Red:    " << myColor1.GetRed() <<endl;
    cout << "Green:  " << myColor1.GetGreen()<<endl;
    cout << "Blue:   " << myColor1.GetBlue()<<endl;
    cout << "Alpha:  " << myColor1.GetAlpha()<<endl;
    cout <<endl;




    cout << "==========================================================\n";
    cout << "Changing color of: "  << myColor1 << endl;;
    myColor1.SetColorByRGBA(252355);
    cout << "to: " << myColor1 << endl;

    
    cout << "==========================================================\n";
    cout << "Getting specific colors of " << myColor1 << endl;
    cout << "Red:    " << myColor1.GetRed() <<endl;
    cout << "Green:  " << myColor1.GetGreen()<<endl;
    cout << "Blue:   " << myColor1.GetBlue()<<endl;
    cout << "Alpha:  " << myColor1.GetAlpha()<<endl;
    cout <<endl;


    cout << "==========================================================\n";
    cout << "Changing color of: "  << myColor1 << endl;;
    myColor1.SetColorByRGBA(15, 33, 199, 53);
    cout << "to: " << myColor1 << endl;

    
    cout << "==========================================================\n";
    cout << "Getting specific colors of " << myColor1 << endl;
    cout << "Red:    " << myColor1.GetRed() <<endl;
    cout << "Green:  " << myColor1.GetGreen()<<endl;
    cout << "Blue:   " << myColor1.GetBlue()<<endl;
    cout << "Alpha:  " << myColor1.GetAlpha()<<endl;
    cout <<endl;

   
    cout << "Finished" << endl;
    return 0;
}

