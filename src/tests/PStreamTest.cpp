//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       PStreamTest.cpp
//    Purpose:    Tests the ../devices/PStream.cpp functions
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

#include "../devices/PStream.h"
#include <iostream>




using std::cout;
using std::endl;
using std::flush;


int main(int argc, char **argv)
{

    cout << "\n\n\n==========================================================\n";
    cout << "Testing PStream Class\n";
    cout << "==========================================================\n";
  
 

    cout << "Opening file stream 'tmp.dat'\n";
    PStream  mystream("tmp.dat", sdWrite, stASCII); 
    
    cout << "Successfully opened\n";
    cout << "---------------------------------------\n\n\n";

    cout << "Writing text to file using WriteChar()\n";
    mystream.WriteChar('a');
    mystream.WriteChar('b');
    mystream.WriteChar('c');
    mystream.WriteChar('d');
    mystream.WriteChar('e');
    mystream.WriteChar('\n');

    

    cout << "Writing text to file using WriteBuffer():\n"; 


    mystream.WriteBuffer("-------------------\n",20);
    mystream.WriteBuffer("1234567901234567890\n",20);
    mystream.WriteBuffer("-------------------\n",20);
    mystream.WriteBuffer("-------------------\n",20);
    mystream.WriteBuffer("- -- --- ---- -----\n",20);
    mystream.WriteBuffer("htoeas-ueoa-hoeutns\n",20);
    mystream.WriteBuffer("htoeas-ueoa-hoeutns\n",20);
    

    cout << "Text written\n";
    cout << "---------------------------------------\n\n\n";

    cout << "Closing text stream.\n";
    mystream.Close();
    cout << "Text stream Closed.\n";
    cout << "---------------------------------------\n\n\n";


    cout << "Reopening text stream with append\n\n\n";
    mystream.Open("tmp.dat",sdAppend,stASCII);
    cout << "Text stream ReOpened.\n";
    cout << "---------------------------------------\n\n\n";
    
    cout << "Writing to file with WriteBuffer\n";
    mystream.WriteBuffer("-------------------\n",20);

    cout << "Writing to buffer with WriteString\n";
    mystream.WriteString("Another String");
    cout << "WriteString testing complete.\n";
    cout << "---------------------------------------\n\n\n";

    cout << "Closing text stream (again).\n";
    mystream.Close();
    cout << "Text stream Closed (again).\n";
    cout << "---------------------------------------\n\n\n";
    

    cout << "Opening file for reading\n";
    mystream.Open("tmp.dat",sdRead,stASCII);
    cout << "File Opened for reading\n";
    cout << "---------------------------------------\n\n\n";

    cout << "Testing EOF\n";
    if(mystream.Eof())
        {
            cout << "     File At END";
        }
    else
        {
            cout << "     File Not At END";
        }
    cout << "EOF Testing complete\n";
    cout << "---------------------------------------\n\n\n";


    cout << "Reading Characters in a line\n";
    char tmpchar =  mystream.ReadChar();
    while(!mystream.Eof() && tmpchar != 10)
        {
            cout << "[" <<  tmpchar <<  "]   " << flush;
            tmpchar = mystream.ReadChar();
        }
 
    cout << "\nCharacter-reading testing complete\n";
    cout << "---------------------------------------\n\n\n";



    cout << "Reading entire line four times\n";
    cout << "1: ["<< mystream.ReadLine() << "]\n";
    cout << "2: ["<< mystream.ReadLine() << "]\n";
    cout << "3: ["<< mystream.ReadLine() << "]\n";
    cout << "4: ["<< mystream.ReadLine() << "]\n";
    
    cout << "\nEntire line(s)-reading testing complete\n";
    cout << "---------------------------------------\n\n\n";
    

    cout << "Reading Tokens in a line\n";
    char* tmpchar1 = mystream.ReadToken(' ');
    while(!mystream.Eof() && *tmpchar1 != 10)
        {
            cout << "[" << tmpchar1 << "]     " << flush;
            delete(tmpchar1);
            tmpchar1 = mystream.ReadToken(' ');
        }
    
    cout << "Final: [" << tmpchar1 << "]     " << flush;
    delete(tmpchar1);
    cout << "residual: [" << mystream.ReadLine() << "]" << endl;

    cout << "[" << mystream.ReadToken('-') << "]" << endl;
    cout << "[" << mystream.ReadToken('-') << "]" << endl;
    cout << "[" << mystream.ReadToken('-') << "]" << endl;
    cout << "[" << mystream.ReadToken('-') << "]" << endl;
    cout << "[" << mystream.ReadToken('-') << "]" << endl;
    cout << "residual: [" << mystream.ReadLine() << "]" << endl;

    cout << "[" << mystream.ReadToken('-') << "]" << endl;
    cout << "[" << mystream.ReadToken('-') << "]" << endl;
    cout << "[" << mystream.ReadToken('-') << "]" << endl;
    cout << "[" << mystream.ReadToken('-') << "]" << endl;
    cout << "[" << mystream.ReadToken('-') << "]" << endl;
    cout << "residual: [" << mystream.ReadLine() << "]" << endl;


    cout << "\nToken-testing complete\n";
    cout << "---------------------------------------\n\n\n";
    mystream.Close();

   cout << "\n\n\n==========================================================\n";
   cout << "Testing PStream class complete\n";
   cout << "==========================================================\n";


       

    return 0;
}

