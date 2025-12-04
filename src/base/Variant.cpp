//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
//////////////////////////////////////////////////////////////////////////
//    Name:       src/base/Variant.cpp
//    Purpose:    Contains the Variant Class
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
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
//    USA
//////////////////////////////////////////////////////////////////////////


#include "Variant.h"
#include "PComplexData.h"
#include "../utility/PError.h"
#include "../utility/Defs.h"

#include <iostream>
#include <sstream>

#include <math.h>
#include <string>

#include <stdlib.h>
#include <cmath>
#include <cstring>

#ifdef PEBL_EMSCRIPTEN
#include <emscripten.h>
#endif

using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::ostream;
using std::string;

//Constructor

Variant::Variant():
   mDataType(P_DATA_UNDEFINED),
   mComplexData(NULL),
   mCreationTime(0)
{

    mData.iNumber = 0;
}


//Constructor
Variant::Variant(const pInt i):
    mDataType(P_DATA_NUMBER_INTEGER),
    mComplexData(NULL),
    mCreationTime(0)
{

   mData.iNumber = i;

}


#ifndef PEBL_EMSCRIPTEN
//Constructor
Variant::Variant(int i):
    mDataType(P_DATA_NUMBER_INTEGER),
    mComplexData(NULL),
    mCreationTime(0)
{
    mData.iNumber = (pInt)i;

}
#endif



//Constructor
Variant::Variant(const long unsigned int i):
    mDataType(P_DATA_NUMBER_INTEGER),
    mComplexData(NULL),
    mCreationTime(0)
{

    mData.iNumber = static_cast<pInt>(i);

}



//Constructor
Variant::Variant(const pDouble f):
    mDataType(P_DATA_NUMBER_FLOAT),
    mComplexData(NULL),
    mCreationTime(0)

{

    mData.fNumber = f;
}

#ifndef PEBL_EMSCRIPTEN
//Constructor
Variant::Variant(double f):
    mDataType(P_DATA_NUMBER_FLOAT),
    mComplexData(NULL),
    mCreationTime(0)
{
    mData.fNumber=(pDouble)f;
}
#endif

//Constructor
Variant::Variant(float f):
    mDataType(P_DATA_NUMBER_FLOAT),
    mComplexData(NULL),
    mCreationTime(0)

{

    mData.fNumber = (pDouble)f;

}


//String Constructor
Variant::Variant(const char *  mystring):
    mDataType(P_DATA_STRING),
    mComplexData(NULL),
    mCreationTime(0)
  //  mData(NULL)
{


    //cout << P_DATA_STRING << endl;
    mData.String = strdup(mystring);
    if(!mData.String)
        PError::SignalFatalError("Failed to copy string.");
}


//Character Constructor
Variant::Variant(const char character):
    mDataType(P_DATA_STRING),
    mComplexData(NULL),
    mCreationTime(0)

{

    //Make a dummy string to place the character into.
    char * dummy = (char*)malloc(2*sizeof(char));
    dummy[0]=character;
    dummy[1]='\0';
    mData.String = dummy;

}

//standard string constructor
Variant::Variant(std::string  mystring):
    mDataType(P_DATA_STRING),
    mComplexData(NULL),
    mCreationTime(0)

{

    const char* dummy = mystring.c_str();
    mData.String = strdup(dummy);

}


Variant::Variant(const char* mystring, VariantDataType type):
    mDataType(type),
    mComplexData(NULL),
//    mData((int)0),
    mCreationTime(0)
{

    switch(type)
        {
        case P_DATA_FUNCTION:
            mData.Function= strdup(mystring); break;
        case P_DATA_STRING:
            mData.String= strdup(mystring); break;
        case P_DATA_LOCALVARIABLE:
        case P_DATA_GLOBALVARIABLE:
            mData.Variable= strdup(mystring); break;

        case P_DATA_UNDEFINED:
        case P_DATA_NUMBER_INTEGER:
        case P_DATA_NUMBER_FLOAT:
        case P_DATA_COMPLEXDATA:
        default:
            //If it isn't one of the above, change it into a string
            mDataType = P_DATA_STRING;
            mData.String = strdup(mystring);
            break;
        }
}

Variant::Variant(const PComplexData * pcd):
    mDataType(P_DATA_COMPLEXDATA),
    mComplexData(NULL),
    mCreationTime(0)
{

    mComplexData = new PComplexData(pcd);
}


///This is an internal data type used by the stack evaluator.
Variant::Variant(const StackSignalType signal):
    mDataType(P_DATA_STACK_SIGNAL),
    mComplexData(NULL),
   // mData((StackSignalType)signal),
    mCreationTime(0)

{
   mData.Signal = signal;
}


Variant::Variant(pFunc pfunc ):
    mDataType(P_DATA_FUNCTION_POINTER),
    mComplexData(NULL),
    mCreationTime(0)
{
    mData.pFunction = pfunc;
}



//Constructor
Variant::Variant(bool b):
    mDataType(P_DATA_NUMBER_INTEGER),
    mComplexData(NULL),
    mCreationTime(0)

{

    mData.iNumber = (pInt)b;
}


///
/// Copy Constructor for variant
///

Variant::Variant(const Variant &v):
    mDataType(v.GetDataType()),
    mComplexData(NULL),
    mCreationTime(v.GetCreationTime())
{

    //    cout << "creating/copy10:[" << v << "]("<<GetCreationTime()<<")"<< endl;

    //This should behave differently depending on what type of variant v is
    // mDataType already initialized in initializer list above

#ifdef PEBL_EMSCRIPTEN
    // Validate that mDataType is within valid range
    if(mDataType < 0 || mDataType > P_DATA_COMPLEXDATA) {
        cerr << "ERROR: Variant copy constructor received corrupted source Variant" << endl;
        cerr << "Invalid mDataType value: " << mDataType << " (expected 0-" << P_DATA_COMPLEXDATA << ")" << endl;
        cerr << "Source address: " << &v << endl;
        cerr << "Source mComplexData: " << v.mComplexData << endl;
        cerr << "Source mCreationTime: " << v.mCreationTime << endl;

        // Print stack trace using Emscripten
        EM_ASM({
            console.error("Stack trace at corruption point:");
            console.trace();
        });

        cerr << "This indicates memory corruption or uninitialized Variant" << endl;
        // Set to undefined and continue rather than crash
        mDataType = P_DATA_UNDEFINED;
        mData.iNumber = 0;
        return;
    }
#endif

    switch(mDataType)
        {

        case P_DATA_NUMBER_INTEGER:  // an integer
            mData.iNumber = v.GetInteger();
            break;

        case P_DATA_NUMBER_FLOAT:    // a float
            mData.fNumber = v.GetFloat();
            break;

        case P_DATA_STRING:
            //            cout << "testing case string:<" << v << ">"<<endl;
            mData.String = strdup(v.GetString().c_str());
            break;

        case P_DATA_LOCALVARIABLE:        // a variable name
        case P_DATA_GLOBALVARIABLE:        // a variable name
            mData.Variable = strdup(v.GetVariableName().c_str());
            break;

        case P_DATA_FUNCTION:
            {
            //Memory problem here, diagnosed by efence:
                std::string tmpfname = v.GetFunctionName();
                //   const char* tmpfname2 = tmpfname.c_str();
                char* cstr = new char[tmpfname.length()+1];
                strcpy(cstr,tmpfname.c_str());
                mData.Function = cstr;

            }

            break;

        case P_DATA_FUNCTION_POINTER:
            mData.pFunction = v.GetFunctionPointer();
            break;


        case P_DATA_STACK_SIGNAL:
            mData.Signal = v.GetSignal();
            break;

            //This needs to  make a deep copy

        case P_DATA_COMPLEXDATA:
            {

                //cout<<"TYPE:" << v << endl;
                PComplexData * pcd = v.GetComplexData();

                if(pcd)
                    mComplexData  = new PComplexData(*pcd);
                else
                    mComplexData = NULL;

            }
            break;

        case P_DATA_UNDEFINED: // undefined, not an error
            break;
        default:

            cerr << "Undefined Data Type in Variant copy constructor. Type: " << mDataType << endl;
            cerr << v << endl;
            PError::SignalFatalError("Undefined Data Type.");
        break;

        }

}



///
/// Standard destructor.  This should take care of cleaning
/// up after data subtypes that put stuff on the stack.
Variant::~Variant()
{
    //This is needed to destroy counted pointers, but it leads to a
    //crash later.
    free_mData();
}


///This overloads the + operator for Variants.
///for strings, this concatenates them.
Variant Variant::operator +(const Variant & rhs) const
{
    pInt i;
    pDouble f;

    if(this->IsInteger()  && rhs.IsInteger())
        {
            i = this->GetInteger() + rhs.GetInteger();
            return Variant(i);
        }
    else if (this->IsNumber() && rhs.IsNumber())
        {
            f = this->GetFloat() + rhs.GetFloat();
            return Variant(f);
        }
    else if (this->IsString() || rhs.IsString())
        {
            std::string  tmp1, tmp2;
            tmp1 = GetString();
            tmp2 = rhs.GetString();
            tmp1 += tmp2;
            return Variant(tmp1);
        }
    else
        //There should be an error emmitted here
        i=0;
    return Variant(i);

}


///This overloads the - operator for Variants
Variant Variant::operator -(const Variant & rhs) const
{
    pInt i;
    pDouble f;

    if(this->IsInteger()  && rhs.IsInteger())
        {
            i = this->GetInteger() - rhs.GetInteger();
            return Variant(i);
        }
    else if (this->IsNumber() && rhs.IsNumber())
        {

            f = this->GetFloat() - rhs.GetFloat();
            return Variant(f);
        }
    else
        i=0;
    return Variant(i);

}




///This overloads the * operator for Variants
Variant Variant::operator *(const Variant & rhs) const
{
    pInt i;
    pDouble f;

    if(this->IsInteger()  && rhs.IsInteger())
        {
            i = this->GetInteger() * rhs.GetInteger();
            return Variant(i);
        }
    else if (this->IsNumber() && rhs.IsNumber())
        {

            f = this->GetFloat() * rhs.GetFloat();
            return Variant(f);
        }
    else
        i=0;
    return Variant(i);

}



///This overloads the / operator for Variants
Variant Variant::operator /(const Variant & rhs) const
{
    pInt i;
    pDouble f;

    if(this->IsNumber() && rhs.IsNumber())
        {

            f = this->GetFloat() / rhs.GetFloat();
            return Variant(f);
        }
    else
        i=0;
    return Variant(i);

}




/// Primitive Compare Method
///
bool Variant::Equal(const Variant & rhs) const
{
    //std::cout << "variant equal: "<< *this << "==" << rhs << endl;
    //If they are both integers, make equality be exact.
    //If one is a float, make equality be with some tolerance.

    if(this->IsInteger()  && rhs.IsInteger())
        {
            return (this->GetInteger() == rhs.GetInteger());
        }
    else if (this->IsNumber() && rhs.IsNumber())
        {
            return (std::abs(this->GetFloat() - rhs.GetFloat()) < .00000000001);
        }
    else if (this->IsString() && rhs.IsString())
        { //If both are strings, compare them

            //Get copies of the strings.
            std::string str1 = this->GetString();
            std::string str2 = rhs.GetString();

            //strcmp returns 0 if they are identical, a number otherwise
            bool b = (str1 == str2);
            return b;
        }
    else if (this->IsStackSignal() && rhs.IsStackSignal())
        {
            return (this->GetSignal() == rhs.GetSignal());
        }
    return false;
}


///
/// Primitive Comparing Method
bool Variant::Less(const Variant & rhs) const
{

	if (this->IsNumber() && rhs.IsNumber())
		{
			return ((this->GetFloat()) < (rhs.GetFloat()));
		}
	else if (this->IsString() && rhs.IsString())
	    { //If both are strings, compare them

            //Get copies of the strings.
            std::string  str1 = this->GetString();
            std::string  str2 = rhs.GetString();

            return str1 < str2;
	    }
	else if (this->IsFunction() && rhs.IsFunction())
        {
            //Get copies of the strings.

            std::string  str1 = this->GetFunctionName();
            std::string  str2 = rhs.GetFunctionName();

            return (str1 < str2);
        }
	    //This should be handled more elegantly.
	    return false;
}



//The following overloaded operators use the above
//::Equal() and ::Less() methods, and each other.
bool Variant::operator == ( const Variant & rhs) const
{ return Equal(rhs); }



bool Variant::operator != ( const Variant & rhs) const
{ return ! ((*this) == rhs); }



bool Variant::operator < ( const Variant & rhs) const
{ return Less(rhs); }



bool Variant::operator > ( const Variant & rhs) const
{ return rhs < (*this); }



bool Variant::operator <= ( const Variant & rhs) const
{ return ! ((*this) > rhs); }



bool Variant::operator >= ( const Variant & rhs) const
{ return rhs <= (*this); }





///
/// Assignment Operator (overloaded)
Variant Variant::operator = (const Variant & value)
{
    // Debug: std::cout << "[VARIANT] Assignment operator called. Old type: " << mDataType
    //           << " (" << GetDataTypeName() << "), New type: " << value.GetDataType()
    //           << " (" << value.GetDataTypeName() << ")" << std::endl;

    //First, clean up 'this' if it contains data on free store
    // (e.g., a Variable or a string)
    free_mData();

    mDataType=value.GetDataType();
    switch(mDataType)
        {
        case P_DATA_NUMBER_INTEGER:  // an integer
            mData.iNumber = value.GetInteger();
            break;

        case P_DATA_NUMBER_FLOAT:    // a float
            mData.fNumber = value.GetFloat();
            break;

        case P_DATA_STRING:
            mData.String = strdup(value.GetString().c_str());
            break;

        case P_DATA_LOCALVARIABLE:        // a char* variable name
        case P_DATA_GLOBALVARIABLE:        // a char* variable name
            mData.Variable = strdup(value.GetVariableName().c_str());
            break;

        case P_DATA_FUNCTION:
            mData.Function = strdup(value.GetFunctionName().c_str());
            break;

        case P_DATA_FUNCTION_POINTER:
            mData.pFunction = value.GetFunctionPointer();
            break;

        case P_DATA_STACK_SIGNAL:
            mData.Signal = value.GetSignal();
            break;

        case P_DATA_COMPLEXDATA:
            {
            PComplexData * tmp = value.GetComplexData();
            //tmp is a pointer to complex data, which
            //is just a holder for the object.  We want to make a
            //copy of tmp
            mComplexData = new PComplexData(*tmp);
            break;
            }
        case P_DATA_UNDEFINED: // undefined, error
        default:
            PError::SignalFatalError( "Undefined Variant type in Variant::operator = (Variant).");
            break;
        }
    return (*this);
}

Variant Variant::operator = (const pDouble & value)
{
    //First, clean up 'this' if it contains data on free store
    // (e.g., a Variable or a string)
    free_mData();

    mDataType = P_DATA_NUMBER_FLOAT;
    mData.fNumber = value;
    return *this;
}


Variant Variant::operator = (const pInt & value)
{
    //First, clean up 'this' if it contains data on free store
    // (e.g., a Variable or a string)
    free_mData();


    mDataType = P_DATA_NUMBER_INTEGER;
    mData.iNumber = value;
    return *this;
}


Variant Variant::operator = (const long unsigned int & value)
{
    //First, clean up 'this' if it contains data on free store
    // (e.g., a Variable or a string)
    free_mData();


    mDataType = P_DATA_NUMBER_INTEGER;
    mData.iNumber = value;
    return *this;
}

#ifndef PEBL_EMSCRIPTEN
Variant Variant::operator = (const int & value)
{
    //First, clean up 'this' if it contains data on free store
    // (e.g., a Variable or a string)
    free_mData();

    mDataType = P_DATA_NUMBER_INTEGER;
    mData.iNumber = (pInt)value;
    return *this;
}

Variant Variant::operator = (const double & value)
{
    //First, clean up 'this' if it contains data on free store
    // (e.g., a Variable or a string)
    free_mData();

    mDataType = P_DATA_NUMBER_FLOAT;
    mData.fNumber = (pDouble)value;
    return *this;
}
#endif

Variant Variant::operator = (const float & value)
{
    //First, clean up 'this' if it contains data on free store
    // (e.g., a Variable or a string)
    free_mData();

    mDataType = P_DATA_NUMBER_FLOAT;
    mData.fNumber = (pDouble)value;
    return *this;
}




Variant Variant::operator = (const  char * value)
{
    //First, clean up 'this' if it contains data on free store
    // (e.g., a Variable or a string)
    free_mData();

    mDataType = P_DATA_STRING;
    mData.String = strdup(value);
    return *this;
}



Variant Variant::operator = (const  std::string  value)
{
    //First, clean up 'this' if it contains data on free store
    // (e.g., a Variable or a string)
    free_mData();

    mDataType = P_DATA_STRING;
    mData.String = strdup(value.c_str());
    return *this;
}



///overload operator<<

ostream & operator << (ostream& out, const Variant& v)
{
    switch(v.GetDataType())
        {

        case P_DATA_NUMBER_INTEGER:  // an integer
            out << v.GetInteger();
            break;

        case P_DATA_NUMBER_FLOAT:    // a float
            out << v.GetFloat();
            break;

        case P_DATA_STRING:          // a string
            out <<  v.GetString();
            break;

        case P_DATA_LOCALVARIABLE:        // a variable
        case P_DATA_GLOBALVARIABLE:        // a variable
            out << v.GetVariableName();
            break;

        case P_DATA_FUNCTION:        // a function name
            out << v.GetFunctionName();
            break;

        case P_DATA_FUNCTION_POINTER:
            out << "Function Pointer" ;
            break;

        case P_DATA_COMPLEXDATA:
            {
                out << *(v.GetComplexData())<<endl;
            }
            break;

        case P_DATA_STACK_SIGNAL:
            out << v.GetSignalName();
            break;


        case P_DATA_UNDEFINED: // undefined, error
        default:
            out << "Trying to print undefined data type in Variant::<<: " << v.GetDataType() << "name: " << v.GetDataTypeName() << endl ;
            break;
        }

    return out;
}


//Casting operators
Variant::operator pInt()
{
    return GetInteger();
}

#ifndef PEBL_EMSCRIPTEN
Variant::operator int()
{
    return static_cast<int>(GetInteger());
}

#endif

Variant::operator long unsigned int()
{
    return static_cast<long unsigned int>(GetInteger());
}



Variant::operator pDouble()
{
    return GetFloat();
}

// Variant::operator const char* ()
// {
//     return GetString();
// }


Variant::operator const std::string () const
{
    return GetString();
}

// Variant::operator char* ()
// {
//     return strdup(GetString());
// }


Variant::operator bool ()
{
    if((*this) == Variant((pInt)0) ||
       (*this) == Variant((pDouble)0.0) ||
       (*this) == Variant("") ||
       (*this) == Variant("F") ||
       (*this) == Variant("FALSE")
       )

        return false;
    else
        return true;
}

///
///Data Accessors
///

///This returns the type as an enum
VariantDataType Variant::GetDataType() const
{
    return mDataType;
}

///This returns the type as a string
std::string Variant::GetDataTypeName() const
{
    switch(mDataType)
        {
        case P_DATA_NUMBER_INTEGER:
            return "PEBL Integer Type";

        case P_DATA_NUMBER_FLOAT:
            return "PEBL Floating-Point Type";

        case P_DATA_STRING:
            return "PEBL String Type";

        case P_DATA_LOCALVARIABLE:
            return "PEBL Local Variable Type";

        case P_DATA_GLOBALVARIABLE:
            return "PEBL Global Variable Type";

        case P_DATA_FUNCTION:
            return "PEBL Function Type";

        case P_DATA_FUNCTION_POINTER:
            return "PEBL Function Pointer Type";

        case P_DATA_STACK_SIGNAL:
            return "PEBL Internal Stack Signal Type";

        case P_DATA_COMPLEXDATA:
            return "PEBL Complex data within Variant";

        case P_DATA_UNDEFINED:
        default:
            return "Undefined PEBL Variant Type";
        }
}


///This tests whether the Variant is a number (i.e., a float or an integer.)
bool Variant::IsNumber() const
{
    return ( mDataType == P_DATA_NUMBER_INTEGER ||
             mDataType == P_DATA_NUMBER_FLOAT);
}


bool Variant::IsFloat() const
{
    return mDataType == P_DATA_NUMBER_FLOAT;
}

bool Variant::IsInteger() const
{
    return  mDataType == P_DATA_NUMBER_INTEGER;

}

bool Variant::IsString() const
{
    return mDataType == P_DATA_STRING;
}


bool Variant::IsVariable() const
{
    return( mDataType == P_DATA_LOCALVARIABLE || mDataType == P_DATA_GLOBALVARIABLE);
}

bool Variant::IsLocalVariable() const
{
    return  mDataType == P_DATA_LOCALVARIABLE;
}

bool Variant::IsGlobalVariable() const
{
    return  mDataType == P_DATA_GLOBALVARIABLE;
}


bool Variant::IsFunction() const
{
    return mDataType == P_DATA_FUNCTION;
}


bool Variant::IsFunctionPointer() const
{
    return mDataType == P_DATA_FUNCTION_POINTER;
}




bool Variant::IsComplexData() const
{
    return mDataType == P_DATA_COMPLEXDATA;
}

bool Variant::IsStackSignal() const
{
    return mDataType == P_DATA_STACK_SIGNAL;
}




pInt Variant::GetInteger() const
{
    switch(mDataType)
        {
        case P_DATA_NUMBER_INTEGER: // an integer
            return mData.iNumber;

        case P_DATA_NUMBER_FLOAT:   // a float
            return (pInt)mData.fNumber;

        case P_DATA_STRING:
            return strtol(mData.String,0,10);

        case P_DATA_STACK_SIGNAL:
        case P_DATA_LOCALVARIABLE:       //
        case P_DATA_GLOBALVARIABLE:       //
        case P_DATA_FUNCTION:       //
        case P_DATA_FUNCTION_POINTER:
        case P_DATA_UNDEFINED:      // undefined, error
        default:
            PError::SignalWarning("Erroneous Data type in Variant::GetInteger(); returning 0");
            return (pInt)0;
            break;
        }

}


pDouble Variant::GetFloat() const
{
    switch(mDataType)
        {
        case P_DATA_NUMBER_INTEGER: // an integer
            return (pDouble)mData.iNumber;

        case P_DATA_NUMBER_FLOAT:   // a float
            return mData.fNumber;

        case P_DATA_STRING:

            return PEBLUtility::StringToPDouble(mData.String);

        case P_DATA_STACK_SIGNAL:
        case P_DATA_LOCALVARIABLE:
        case P_DATA_GLOBALVARIABLE:
        case P_DATA_FUNCTION:
        case P_DATA_FUNCTION_POINTER:
        case P_DATA_UNDEFINED:      // undefined, error
        default:
            PError::SignalWarning("Erroneous Data type in Variant::GetFloat(); returning 0");
            return (pDouble)0;

        }

}




std::string Variant::GetString() const
{

    switch(mDataType)
        {
        case P_DATA_NUMBER_INTEGER: // an integer
            {
                std::ostringstream o;
                o <<  mData.iNumber;
                return o.str();
            }

        case P_DATA_NUMBER_FLOAT:   // a float
            {
                std::ostringstream o;
                o <<  mData.fNumber;
                return o.str();
            }

        case P_DATA_STRING:
            return std::string(mData.String);
        case P_DATA_FUNCTION:
            return mData.Function;


        case P_DATA_LOCALVARIABLE:
        case P_DATA_GLOBALVARIABLE:
            return std::string(mData.Variable);

        case P_DATA_COMPLEXDATA:
            {
                PComplexData * pcd = GetComplexData();
                counted_ptr<PEBLObjectBase> pob = pcd->GetObject();

                //                 cout << *pob << endl;


                 //                 std::string tmp = pob->ObjectName();
                 //                 return tmp;


                 std::ostringstream o;
                 o << *pob << std::flush;

                 return o.str();

//                 std::stringstream tmp;
//                 tmp <<  *pob << std::flush;

//                 std::string tmpstring;
//                 tmp >> tmpstring;

//                 return tmpstring.c_str();
                //                return pcd->GetTypeName().c_str();


//                 cout << *pcd << endl;
//                 cout << "Getting Variant string\n";

//
//                  cout << "[[["<< tmpstring << "]]]"<< endl;
//                  cout << "done Getting Variant string\n";
//                  return tmpstring.c_str();

            }

        case P_DATA_STACK_SIGNAL:
        case P_DATA_FUNCTION_POINTER:

        case P_DATA_UNDEFINED:      // undefined, error
        default:
            cout << mDataType << endl;
            PError::SignalWarning("Erroneous Data type in Variant::GetString");
            return "";

        }


}


//This returns a pointer to the name of the variable.
std::string  Variant::GetVariableName() const
{

    if( mDataType == P_DATA_LOCALVARIABLE || mDataType == P_DATA_GLOBALVARIABLE)
        {
            return std::string(mData.Variable);
        }
    else
        {

            PError::SignalFatalError("Erroneous Data type in Variant::GetVariableName()");
            return NULL;

        }
}





//This returns the name of the variable; if this is a variable.property, it returns
//just the first part, and in all caps.
std::string Variant::GetVariableBaseName() const
{

    if( mDataType == P_DATA_LOCALVARIABLE || mDataType == P_DATA_GLOBALVARIABLE)
        {

            std::string tmp = PEBLUtility::ToUpper(mData.Variable);
            std::string::size_type pos = tmp.find(".");
            if(pos == std::string::npos)
                return tmp;
            else
                return tmp.substr(0, pos);

        }
    else
        {
            PError::SignalFatalError("Erroneous Data type in Variant::GetVariableBaseName()");
            return NULL;

        }
}


//This returns the name of the variable; if this is a variable.property, it returns
//just the first part.
std::string  Variant::GetVariablePropertyName() const
{

    if( mDataType == P_DATA_LOCALVARIABLE || mDataType == P_DATA_GLOBALVARIABLE)
        {

            std::string tmp = PEBLUtility::ToUpper(mData.Variable);
            std::string::size_type pos = tmp.find(".");
            if(pos == std::string::npos)
                return "";
            else
                return tmp.substr(pos+1,tmp.size());
        }
    else
        {
            PError::SignalFatalError("Erroneous Data type in Variant::GetVariablePropertyName()");
            return NULL;

        }
}


//This returns the name of the function.
std::string   Variant::GetFunctionName() const
{

    if( mDataType == P_DATA_FUNCTION)
        {

            const char* fname = mData.Function;

            //hmm--memory error here in efence.
            //may have to do with page allocation stuff,
            //affecting NOT pebl itself but PEBL+efence.
            //try cat /proc/sys/vm/max_map_count
            //and something like
            //echo 165535 > /proc/sys/vm/max_map_count
            std::string tmp = std::string(fname);
            //cout <<"tmp obtained" << tmp << endl;
            return tmp;
        }
    else
        {

            PError::SignalFatalError("Erroneous Data type in Variant::GetFunctionName()");
            return NULL;
        }
}


//This returns a pointer to the name of the function.
Variant  Variant::GetSignalName() const
{

    if( mDataType == P_DATA_STACK_SIGNAL)
        {

            int val = GetSignal();
            switch (val)
                {
                case STACK_LIST_HEAD:
                    return Variant(val)+Variant("|STACK_LIST_HEAD");
                case STACK_RETURN_DUMMY:
                    return Variant(val)+Variant("|STACK_RETURN_DUMMY");
                case STACK_TERMINATE_EVENT_LOOP:
                    return Variant(val)+Variant("|STACK_TERMINATE_EVENT_LOOP");
                case STACK_BREAK:
                    return Variant(val)+Variant("|STACK_BREAK");
                case STACK_UNDEFINED:
                default:
                    return Variant(val)+Variant("STACK_UNDEFINED");

                }
        }else{
            return STACK_UNDEFINED;
    }
}



StackSignalType  Variant::GetSignal() const
{

    if( mDataType == P_DATA_STACK_SIGNAL)
        {
            return mData.Signal;
        }else
        {
            return STACK_UNDEFINED;
        }
}



//This returns a pointer to the name of the function.
pFunc Variant::GetFunctionPointer() const
{

    if( mDataType == P_DATA_FUNCTION_POINTER)
        {
            return mData.pFunction;
        }
    else
        {
            return NULL;
        }
}





/// This will return a pointer to the complex data
/// held within the variant.
PComplexData * Variant::GetComplexData() const
{
    return mComplexData;
}



/// This sets the complex data type to the argument.
/// It does not delete the complex data currently held
/// in the pointer.
void Variant::SetComplexData(PComplexData * data)
{
    mComplexData = data;
}



///This method just clears out any part of mData
///That is on the free store, including any PComplexData that is
///held by the variant.  PCDs are 1-1 with variants; they should not be shared
/// by multiple variants, although the data they reference is ref-counted and may be shared.
void Variant::free_mData()
{
    // Debug: std::cout << "[VARIANT] free_mData() called. Type: " << mDataType
    //           << ", mComplexData: " << mComplexData << std::endl;

    if(mComplexData)
     {
         // Debug: std::cout << "[VARIANT] Deleting PComplexData at: " << mComplexData << std::endl;
         delete mComplexData;
         mComplexData =NULL;

         //mDataType=P_DATA_UNDEFINED;
     } else
         {

             if(mDataType == P_DATA_STRING )
                 {
                     if(mData.String)
                         {
                             free(mData.String);
                             mData.String = NULL;
                         }
                 }
             else if(IsVariable())
                 {
                     if(mData.Variable)
                         {
                             free(mData.Variable);
                             mData.Variable = NULL;
                         }
                 }
             else if(mDataType == P_DATA_FUNCTION)
                 {
                     if(mData.Function)
                         {
                             free(mData.Function);
                             mData.Function = NULL;
                         }
                 }

             else if(mDataType == P_DATA_COMPLEXDATA)
                 {
                 }

         }

    //    mDataType = P_DATA_UNDEFINED;
}
