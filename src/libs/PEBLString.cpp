//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLString.cpp
//    Purpose:    String Processing Function Library for PEBL
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2004-2025 Shane T. Mueller <smueller@obereed.net>
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
#include "PEBLString.h"
#include "../base/Variant.h"
#include "../base/PComplexData.h"
#include "../base/PList.h"

#include "../utility/PEBLUtility.h"
#include "../utility/PError.h"

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cctype>

using std::ostream;
using std::cerr;
using std::endl;
using std::list;
using std::string;




Variant PEBLString::Uppercase(Variant v)
{

   //v[1] should have the parameter: a list of keys to wait for.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in function Uppercase(<string>)]: ");  

    std::string s=plist->First();    
    transform(s.begin(), s.end(), s.begin(), toupper);

    return Variant(s);
}



Variant PEBLString::Lowercase(Variant v)
{

   //v[1] should have the parameter: a list of keys to wait for.
    PList * plist = v.GetComplexData()->GetList();
    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in function Uppercase(<string>)]: ");  

    std::string s=plist->First();    
    transform(s.begin(), s.end(), s.begin(), tolower);

    return Variant(s);
}


Variant PEBLString::ToASCII (Variant v)
{

    PList * plist = v.GetComplexData()->GetList();
    PError::AssertType(plist->First(), PEAT_INTEGER, "Argument error in function ToASCII(<integer>)]: ");  

    std::string s = "";
    s[0] = (plist->First()).GetInteger();    
    
    return Variant((std::string(s)));

}


Variant PEBLString::StringLength(Variant v)
{

   //v[1] should have the parameter: a list of keys to wait for.
    PList * plist = v.GetComplexData()->GetList();
    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in function StringLength(<string>)]: ");  
    std::string s=plist->First();    

    return Variant((int)(s.length()));
}


//Not implemented:
#if 0
Variant PEBLString::Strip(Variant v)
{

   //v[1] should have the parameter: a list of keys to wait for.
    PList * plist = v.GetComplexData()->GetList();
    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in function StringLength(<string>)]: ");  
    std::string s=plist->First();    

    return Variant((int)(s.length()));
}
#endif

Variant PEBLString::SubString(Variant v)
{

   //v[1] should have the parameter: a list of keys to wait for.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in first parameter of function SubString(<string>, <index>, <length>)]: ");  
    std::string string1 = plist->First(); //plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_INTEGER, "Argument error in Second parameter of function SubString(<string>, <index>, <length>)]: ");  
    int position = plist->Nth(2);// plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_INTEGER, "Argument error in Third parameter of function SubString(<string>, <index>, <length>)]: ");  
    int length = plist->Nth(3);// plist->PopFront();

    if(position < 1)
        {
            PError::SignalFatalError("Trying to get substring with index less than 1");
        }
    if(string1.length()<position)
        {
            PError::SignalFatalError("Trying to get substring with index longer than string");
        }
    //cout << "Length: " << string1.length() << "pos: " << position << "len1:"<< length << endl;
    if(string1.length() < position-1+length)
        {
            length = -1;
        }
    
    return Variant(string1.substr(position-1,length));
}



// This finds the first index of a substring within a string.  It returns
// 0 if the character is not found.
Variant PEBLString::FindInString(Variant v)
{
   //v[1] should have the parameter: a list of keys to wait for.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in first parameter of function FindInString(<string>,<substring>,<pos>)]: ");  
    std::string str1 = plist->First();// plist->PopFront();
    
    
    PError::AssertType(plist->Nth(2), PEAT_STRING, "Argument error in second parameter of function FindInString(<string>,<substring>,<pos>)]: ");  
    std::string str2 = plist->Nth(2); //plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_INTEGER, "Argument error in Third parameter of function FindInString(<string>,<substring>,<pos>)]: ");  
    int pos = plist->Nth(3); //plist->PopFront();


    string::size_type newpos = str1.find(str2,pos-1);
    if (newpos == string::npos) 
        return Variant(0);
    else
        return Variant((int)newpos+1);
}




// This splits the string by another string.  If second string is "", it splits ever character.
Variant PEBLString::SplitString(Variant v)
{
   //v[1] should have the parameter: a list of keys to wait for.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in first parameter of function SplitString(<string>,<substring>)]: ");  
    std::string str1 = plist->First();// plist->PopFront();
    
    
    PError::AssertType(plist->Nth(2), PEAT_STRING, "Argument error in second parameter of function SplitString(<string>,<substring>)]: ");  
    std::string str2 = plist->Nth(2); //plist->PopFront();


    std::string tmpstring = str1;
    std::string sep = str2;
    Variant  ret  =  PEBLUtility::Tokenize(tmpstring.c_str(),*(sep.c_str()));

    return ret;
}





// This copies text to the OS clipboard.
Variant PEBLString::CopyToClipboard(Variant v)
{
   //v[1] should have the parameter: a list of keys to wait for.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in function CopyToClipboard(<string>)]: ");  
    std::string str1 = plist->First();// plist->PopFront();
    

    PEBLUtility::CopyToClipboard(str1);
    return Variant(1);
}

