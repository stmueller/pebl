//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/utility/PError.h
//    Purpose:    Utility class for signaling warnings and errors
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003-2025 Shane T. Mueller <smueller@obereed.net>
//
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
#ifndef __PERROR_H__
#define __PERROR_H__

#include <string>
#include <list>
#include "../base/Variant.h"
#include "../base/PNode.h"

enum PErrorAssertType
    {
        //These are based on variant types:
        PEAT_UNDEFINED = 0,    // undefined, error
        PEAT_STACK_SIGNAL     = 1,     // an enum that signals stuff to the evaluator stack, for internal use only.
        PEAT_FUNCTION         = 2,     // the name of a function.
        PEAT_FUNCTION_POINTER = 4,     // A pointer to a function.  Used for compiled library functions.
        PEAT_NUMBER           = 8,     // any number.
        PEAT_INTEGER          = 16,    // an integer
        PEAT_FLOAT            = 32,    // a float
        PEAT_STRING           = 64,    // a string of characters
        PEAT_LOCALVARIABLE    = 128,   // a 'variable'; i.e. a char* symbolizing another piece of data.
        PEAT_GLOBALVARIABLE   = 256,   // a 'variable'; i.e. a char* symbolizing another piece of data.
        PEAT_VARIABLE         = 512,   // either a local or global variable
        PEAT_COMPLEXDATA      = 1024,  // Any one of a number of compex data 'objects'.

        //These are based on PComplexData types:
        PEAT_AUDIOOUT         = 2048,
        PEAT_COLOR,
        PEAT_ENVIRONMENT,
        PEAT_FILESTREAM,
        PEAT_FONT,
        PEAT_IMAGEBOX,
        PEAT_JOYSTICK,
        PEAT_KEYBOARD,
        PEAT_LIST,
        PEAT_NETWORKCONNECTION,
        PEAT_OBJECT,
        PEAT_PARALLELPORT,
        PEAT_COMPORT,
        PEAT_TEXTOBJECT,
        PEAT_TEXTBOX,
        PEAT_LABEL,
        PEAT_WIDGET,
        PEAT_WINDOW,
        PEAT_MOVIE
    };



namespace PError
{

    //These functions handle error detection and reporting.  To use, include this file
    //in a source file, and then call PError::Signal....().  SignalFatalError will cause the program
    //to close immediately, printing message as an error message to stderr. SignalWarning()
    //will only print an error message.  If you use the variety with the PNode argument, the
    //message will report information about the original code, if it is available.


    void SignalFatalError(const std::string & message);

    void SignalWarning(const std::string & message);

    void AssertType(Variant v, int type, const std::string & outsidemessage );
    std::string GetTypeName(Variant v);
    void ExitQuietly(const std::string & message);

}


class PCallStack
{
public:
    PCallStack(){};
    ~PCallStack(){};
    void Push(const PNode* node){mNodes.push_back(node);};
    void Pop(){mNodes.pop_back();};
    int Size(){return (int)(mNodes.size());};
    std::ostream & PrintCallStack(std::ostream & out) const
    {
        std::string indent = "";
        std::list<const PNode*>::const_iterator i = mNodes.begin();

        while(i != mNodes.end())
            {
                out << indent << "Called from function ["<< (*i)->GetFunctionName()<< "] on line ["<<(*i)->GetLineNumber() << "] of file [" << (*i)->GetFilename() << "]\n";
                indent = indent + "   ";
                i++;
            }
        return out;
    }

private:
    std::list<const PNode*> mNodes;
};




#endif
