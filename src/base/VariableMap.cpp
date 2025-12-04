//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/VariableMap.cpp
//    Purpose:    Structure that holds global or local variables
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003-2025 Shane T. Mueller <smueller@obereed.net>
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

#include "VariableMap.h"

#include "../utility/PError.h"
#include "../utility/PEBLUtility.h"

#include <iostream>
#include <map>
#include <string>


using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::string;


VariableMap::VariableMap()
{
    //cout << "Creating variable map\n";
    //Initiate anything necessary here.
}


VariableMap::~VariableMap()
{

    //cout << "Deleting variablemap\n";
    //cout << "contains " << mVariableMap.size() << " values\n";
    //DumpValues();
    //Delete mVariableMap if necessary.
    //Erase things by hand, for debugging's sake
    //    std::map<std::string, Variant>::iterator i = mVariableMap.begin();

   // while(i !=  mVariableMap.end())
   //     {
   //        i = mVariableMap.begin();
   //        mVariableMap.erase(i);
   //     }
    mVariableMap.clear();

}


void VariableMap::Destroy()
{
    mVariableMap.clear();
}



///
/// This method will add a new variable with
/// name varname and value val to the variable map, or (if it already exists)
/// change its value to val.
void VariableMap::AddVariable(const string & varname, const Variant & val)
{
    string tmpVarName = PEBLUtility::ToUpper(varname);

    map<string, Variant>::iterator p;

    p = mVariableMap.find(tmpVarName);

    //If the variable is in there already, change its value

    if(p!=mVariableMap.end())
        {

            p->second = val;
        }
    else
        {
            //variable isn't there yet, so add the new value into map
            mVariableMap.insert(pair<string, Variant>(tmpVarName, val));
        }

} 



/// 
/// This method will retrieve the value designated by
/// varname.  If varname doesn't exist, it will return 
/// 0, along with a warning.
Variant  VariableMap::RetrieveValue(const string & varname)
{
 
    map<string,Variant>::iterator p;
  
    //Get a the variable 
    string tmpVarName = PEBLUtility::ToUpper(varname);
    
    p = mVariableMap.find(tmpVarName);
 
    if(p == mVariableMap.end())
        {
            string message = "Trying to use an undefined variable:  " + string(varname);
            PError::SignalFatalError(message);
            return Variant(0);  //This really won't happen.
        }
    else
        {
            Variant vt = p->second;
            return vt;
        }
}


//Checks to see if a variable exists in the map.
bool VariableMap::Exists(const string & varname)
{
    string tmpVarName = PEBLUtility::ToUpper(varname);
    map<string,Variant>::iterator p = mVariableMap.find(tmpVarName);
    if(p == mVariableMap.end())
        return false;
    else
        return true;
}

/// This will erase the value stored in varname
///
void VariableMap::Erase(const string & varname)
{
    string tmpVarname = PEBLUtility::ToUpper(varname);
    mVariableMap.erase(tmpVarname);
}



/// This method is primarily for debugging purposes.
/// It will iterate through the entire variable map, and
/// Display each of the values held in the map.
///
void VariableMap::DumpValues()
{
    map<string,Variant>::iterator p;
  
    for(p= mVariableMap.begin(); p!=mVariableMap.end(); p++)
        {
            cout << "VariableName:  [" << p->first << ":"<< (p->first).length() << "] | Value: [" << p->second << "]\n";   
        }
  
}
