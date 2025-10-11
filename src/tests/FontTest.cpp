//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       FontTest.cpp
//    Purpose:    Tests the PFont Class
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
#include "../objects/PFont.h"
#include "../objects/PColor.h"
#include "../base/Variant.h"

//#include "../objects/PObject.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

//The following is the entry point for the command-line version
int main(int argc, char **argv)
{

    cout << "\n\n\n==========================================================\n";
    cout << "Testing the PFont Class\n";
    cout << "==========================================================\n";

    cout << "==========================================================\n";
    cout << "Constructing Colors\n";

    PColor fgColor = PColor();
    PColor bgColor = PColor(222,222,222,222);

    cout << "\n\n\n==========================================================\n";
    cout << "Constructing Fonts\n";
    PFont myFont1 = PFont();
    PFont myFont2 = PFont("VeraSe.ttf", PFS_Italic | PFS_Underline, 22, PColor("red"), PColor("black"), false );
    

    cout << "\n\n==========================================================\n";
    cout << "Printing standard font output." << endl;

    cout << myFont1 << endl;
    cout << myFont2 << endl;



    cout << "\n\n==========================================================\n";
    cout << "Printing font information:" << endl;


    cout << "Font Name  :    " << myFont1.GetFontFileName() << endl;
    cout << "Font Style:     " << myFont1.GetFontStyle() << endl;
    cout << "  IsNormal?     " << myFont1.IsNormalFont() << endl;
    cout << "  IsBold?       " << myFont1.IsBoldFont() << endl;
    cout << "  IsItalic?     " << myFont1.IsItalicFont() << endl;
    cout << "  IsUnderlined? " << myFont1.IsUnderlineFont() << endl;
    cout << "AntiAliased?    " << myFont1.GetAntiAliased() << endl;
    cout << "FGColor:        " << myFont1.GetFontColor() << endl;
    cout << "BGColor:        " << myFont1.GetBackgroundColor() << endl;
    cout << "Font Size:      " << myFont1.GetFontSize() << endl;



    cout << "Finished" << endl;
    return 0;
}

