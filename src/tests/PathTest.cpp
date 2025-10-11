//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       PathTest.cpp
//    Purpose:    Tests the PEBLPath Class
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
#include "../utility/PEBLPath.h"
#include "../base/Evaluator.h"
#include "../devices/PEventLoop.h"
#include <iostream>

using std::cout;
using std::endl;
using std::flush;




//The following is the entry point for the command-line version
int main(int argc, char **argv)
{
 


    cout << "\n\n\n==========================================================\n";
    cout << "Testing the PEBLPath Class\n";
    cout << "==========================================================\n";

    PEBLPath myPath(argv[0]);

    cout << "==========================================================\n";
    cout << "Adding directories to path\n";
    cout << "==========================================================\n";
    //    myPath.AddToPathList("./");
    //    myPath.AddToPathList("media/");
    //    myPath.AddToPathList("media/fonts/");
     

    cout << "Looking for file Vera.ttf:     [" << myPath.FindFile("Vera.ttf") << "]\n";
    cout << "Looking for file Uppercase.txt:[" << myPath.FindFile("Uppercase.txt") << "]\n";


    cout << "Looking for file Makefile:     [" << myPath.FindFile("Makefile") << "]\n";
    cout << "Looking for file notafile.txt: [" << myPath.FindFile("notafile.txt") << "]\n";

    cout << "Finished" << endl;
    return 0;
}

