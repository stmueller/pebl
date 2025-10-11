//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
///////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLMATH.cpp
//    Purpose:    Built-in math functions for PEBL
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
///////////////////////////////////////////////////////////////////////////////
#include "PEBLMath.h"
#include "../base/Variant.h"
#include "../base/PList.h"
#include "../base/PComplexData.h"
#include "../utility/PEBLUtility.h"
#include "../utility/PError.h"
#include "../utility/Defs.h"
#include "../base/PEBLObject.h"

#include <stdlib.h>          //Used for random numbers
//#include <math.h>
#include <cmath>
#include <list>
#include <string>
#include <sstream>

#ifdef WIN32
#include <time.h>
#endif

using std::string;
using std::cout;
using std::endl;

//Putatively precise to 50 digits:
#define PI 3.141592653589793238462643383279502884197169399375
#define E  2.718281828459045235360287471352662497757247093700

Variant PEBLMath::Recurse(Variant v, Variant (*funcname)(Variant))
{

    //If this function is called, v is a list of elements.
    //We should call the function on each element of the list, creating 
    //a list of the results, turn this into a variant, and return it.

    //Get the list of variants.

    PList * plist = v.GetComplexData()->GetList();
    //Make a results list
    PList * resultslist = new PList();
    //Declare a temporary argument list.
   //Now, apply 


    PList* arglist;
    counted_ptr<PEBLObjectBase> tmpObj;
    Variant tmpVariant;
    PComplexData * tmpPCD;
    

    std::vector<Variant>::iterator p = plist->Begin();    
    while(p != plist->End())
        {

            //Create new variant list to pass to function
            arglist = new PList();

            //Add the first element of plist to it
            arglist->PushBack(*p);
            
            //make it into a PCD
            tmpObj = counted_ptr<PEBLObjectBase>(arglist);
            tmpPCD = (new PComplexData(tmpObj));
            //Create variant to hold it.
            tmpVariant = Variant(tmpPCD);
            
            //tmpPCD is now part of tmpVariant, so we don't need to destroy it.
            //delete tmpPCD;
            tmpPCD=NULL;


            Variant result = funcname(tmpVariant);
            resultslist->PushBack(result);

            //Get rid of arglist so we can make a new one.
            delete arglist;
            arglist = NULL;
            //plist->PopFront();  We need to do something here to handle recursion with vectors instead of lists
            //e.g., an iterator.
            //PError::SignalFatalError("Recursive operation not supported.");
            p++;
        }

    //Now, resultslist is a PList.  Put it into a PCD.
    counted_ptr<PEBLObjectBase> tmp = counted_ptr<PEBLObjectBase>(resultslist);
    PComplexData * pcd = new PComplexData(tmp);
    
    Variant tmp2 = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp2;

}


//This is like the recurse above, but is used when there are other arguments
//of interest, such as logn() or round()
Variant PEBLMath::Recurse2(Variant v, Variant (*funcname)(Variant), Variant argument)
{

    //If this function is called, v is a list of elements.
    //We should call the function on each element of the list, creating 
    //a list of the results, turn this into a variant, and return it.

    //Get the list of variants.
    
    PList * plist = v.GetComplexData()->GetList();
    //Make a results list

    PList * resultslist = new PList();
   

    std::vector<Variant>::iterator p = plist->Begin();

    //Add the argument onto arglist.  argument should be a list of variants.
    //Declare an temporary argument list.
    //counted_ptr<PEBLObjectBase>  tmp2 = argument.GetComplexData()->GetObject();
    //Variant arg2 = ((PList*)(tmp2.get()));

    PList * arglist = NULL;

    PComplexData * pcd;
    while(p != plist->End())
        {
            arglist = new PList();     
            arglist->PushBack(*p);           //Add the critical argument 
            arglist->PushBack(argument);        //add second argument
            counted_ptr<PEBLObjectBase> tmp3 = counted_ptr<PEBLObjectBase>(arglist);

            pcd = new PComplexData(tmp3);
            Variant tmp = Variant(pcd);
            resultslist->PushBack(funcname(tmp));        //Execute

            delete arglist;
            arglist = NULL;
     

            p++;
        }

    //Now, resultslist is a PList.  Put it into a PCD.
    pcd = new PComplexData(counted_ptr<PEBLObjectBase>(resultslist)); 
    Variant tmp3 = Variant(pcd);
    delete pcd;
    pcd=NULL;
    
    return tmp3;
}



//Logarithmic Functions
Variant PEBLMath::Log10(Variant v)
{

//    std::cout << "log10a\n";
//    std::cout << "(" << v << ")" << std::endl;
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
//    std::cout << "v1: " << v1 << std::endl;
    if(v1.IsNumber())
        {
            //return Variant((pDouble)log10l(v1.GetFloat()));
            return Variant((pDouble)log10(v1.GetFloat()));

        }
#ifndef PEBL_EMSCRIPTEN
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Log10);

        }
#endif
    else
        {
            PError::SignalFatalError("Non-numeric argument in Log10");
        }

    return 0;
}

Variant PEBLMath::Log2(Variant v)
{
    
    //std::cout <<"Log2: " << v << std::endl;
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            return Variant(PEBLUtility::Log2(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Log2);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in Log2");
            return 0;
        }

}

Variant PEBLMath::Ln(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            //return Variant((pDouble)logl(v1.GetFloat()));
            return Variant((pDouble)log(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Ln);

        }
    else
        {
        
            PError::SignalFatalError("Non-numeric argument in Ln");
            return 0;
        }

}


Variant PEBLMath::LogN(Variant v)
{
    //This has two arguments; the number and the base.
    
    PList * plist = v.GetComplexData()->GetList();
    Variant number = plist->First(); //plist->PopFront();

    //Don't get the second argument right away--if the first argument is
    //a list, we should just pass it along to the recurse2 function.

    Variant base = plist->Nth(2);// plist->PopFront();   //Get the base argument    
    
if(number.IsNumber())
        {                  
            //return Variant((pDouble)(logl(number.GetFloat())/ logl(base.GetFloat())));
            return Variant((pDouble)(log(number.GetFloat())/ log(base.GetFloat())));
        }
    else if(number.GetComplexData()->IsList())
        {

            //counted_ptr<PEBLObjectBase>tmp2 = counted_ptr<PEBLObjectBase>(plist);
            //PComplexData * pcd = new PComplexData(tmp2);

            //Variant args = Variant(pcd);
            //delete pcd;
            //pcd=NULL;
            Variant retval = Recurse2(number, LogN, base);
            return retval;
        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in LogN");
            return 0;
        }
}


Variant PEBLMath::Exp(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            //return Variant((pDouble)expl(v1.GetFloat()));
            return Variant((pDouble)exp(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Exp);

        }
    else
        {

            PError::SignalFatalError("Non-numeric argument in Exp");
            return 0;
        }

}


Variant PEBLMath::Pow(Variant v)
{
    //This has two arguments; the number and the base.
    
    PList * plist = v.GetComplexData()->GetList();
    Variant base = plist->First();// plist->PopFront();

    //Don't get the second argument right away--if the first argument is
    //a list, we should just pass it along to the recurse2 function.
    
    if(base.IsNumber())
        {                  
            Variant power = plist->Nth(2); //plist->PopFront();   //Get the base argument
            //return (pDouble)powl((pDouble)base, (pDouble)power);
            return (pDouble)pow((pDouble)base, (pDouble)power);
        }
    else if(base.GetComplexData()->IsList())
        {

            counted_ptr<PEBLObjectBase> tmp = v.GetComplexData()->GetObject();
            PComplexData * pcd = new PComplexData(tmp);
            Variant args = Variant(pcd);
            delete pcd;
            pcd=NULL;

            Variant retval = Recurse2(base, Pow, args);

            return retval;
        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in Pow");
            return 0;
        }
}

Variant PEBLMath::Sqrt(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            //return Variant((pDouble)sqrtl(v1.GetFloat()));
            return Variant((pDouble)sqrt(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Sqrt);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in Sqrt");
            return 0;
        }

}


Variant PEBLMath::NthRoot(Variant v)
{
    //This has two arguments; the number and the root.
    
    PList * plist = v.GetComplexData()->GetList();
    Variant number = plist->First(); //plist->PopFront();

    //Don't get the second argument right away--if the first argument is
    //a list, we should just pass it along to the recurse2 function.
    
    if(number.IsNumber())
        {                  
            Variant root = plist->Nth(2);// plist->PopFront();   //Get the base argument
            //return (pDouble)powl((pDouble)number,(pDouble)( 1 / root.GetFloat()));
            return (pDouble)pow((pDouble)number,(pDouble)( 1 / root.GetFloat()));
        }
    else if(number.GetComplexData()->IsList())
        {
            counted_ptr<PEBLObjectBase> tmp = v.GetComplexData()->GetObject();
            PComplexData * pcd = new PComplexData(tmp);
            Variant args = Variant(pcd);
            delete pcd;
            pcd=NULL;

            Variant retval = Recurse2(number, NthRoot, args);
            return retval;

            
        }
    else
        {

            PError::SignalFatalError("Non-numeric argument in NthRoot");
            return 0;
        }
}



    //Trigonometric Functions.


Variant PEBLMath::Tan(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            //return Variant((pDouble)tanl(v1.GetFloat()));
            return Variant((pDouble)tan(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Tan);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in Tan");

            return 0;
        }

}



///Sin takes one parameter
Variant PEBLMath::Sin(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            //return Variant((pDouble)sinl(v1.GetFloat()));
            return Variant((pDouble)sin(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Sin);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in [Sin]");

            return 0;
        }
}


Variant PEBLMath::Cos(Variant v)
{

    
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            //return Variant((pDouble)cosl(v1.GetFloat()));
            return Variant((pDouble)cos(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Cos);
        }
    else
        {

            PError::SignalFatalError("Non-numeric argument in [Cos]");
            return 0;
        }
}


Variant PEBLMath::ATan(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();
    if(v1.IsNumber())
        {

            return Variant((pDouble)atan(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, ATan);

        }
    else
        {
        
            PError::SignalFatalError("Non-numeric argument in [ATan]");
            return 0;
        }
}

Variant PEBLMath::ASin(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();


    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            return Variant((pDouble)asin(v1.GetFloat()));
            //return Variant((pDouble)asinl(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, ASin);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in [ASin]");
            return 0;
        }
}

Variant PEBLMath::ACos(Variant v)
{   
    
    PList * plist = v.GetComplexData()->GetList();


    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            //            return Variant((pDouble)acosl(v1.GetFloat()));
            return Variant((pDouble)acos(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, ACos);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in [ACos]");
            return 0;
        }
}



//Converts radians to degrees.  Does not truncate at 180.
Variant PEBLMath::DegToRad(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();


    Variant v1 = plist->First(); //plist->PopFront();

    if(v1.IsNumber())
        {
            return Variant((pDouble)(v1.GetFloat() * PI / 180));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, DegToRad);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in [DegToRad]");
            return 0;
        }

  
}


Variant PEBLMath::RadToDeg(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();
    

    Variant v1 = plist->First(); //plist->PopFront();

    if(v1.IsNumber())
        {
            return Variant(v1.GetFloat() * 180/ PI);
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, RadToDeg);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in [RadToDeg]");
            return 0;
        }
}


//Rounding Functions. Most of these operate on a single number, rounding to the nearest integer.
Variant PEBLMath::Round(Variant v)
{

    
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();
    if(v1.IsNumber())
        {

            //plist->PopFront();

            if(plist->Length()==1)
                {
                    return Variant(PEBLUtility::Round(v1.GetFloat()));
                }else{

                Variant v2 =plist->Nth(2);

                PError::AssertType(v2, PEAT_INTEGER,"Second (optional) argument  in [Round(number, precision)] must be an integer: ");            
                return Variant(PEBLUtility::Round(v1.GetFloat(),v2.GetInteger()));
            }

        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Round);
        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in [Round]");
            return 0;
        }
}


Variant PEBLMath::Floor(Variant v)
{

    
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            //return Variant((pInt)(floorl(v1.GetFloat())));
            return Variant((pInt)(floor(v1.GetFloat())));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Floor);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in [Floor]");
            return 0;
        }
}


Variant PEBLMath::Ceiling(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            //return Variant((pInt)(ceill(v1.GetFloat())));
            return Variant((pInt)(ceil(v1.GetFloat())));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Ceiling);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in [Ceiling]");
            return 0;
        }
}

Variant PEBLMath::AbsFloor(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            return Variant(PEBLUtility::Truncate(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, AbsFloor);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in [AbsFloor]");
            return 0;
        }
    
}



//Modular Arithmetic:  This will even work for non-integers.
Variant PEBLMath::Mod(Variant v)
{
     
    PList * plist = v.GetComplexData()->GetList();

    
    PError::AssertType( plist->First(), PEAT_NUMBER, "Argument error in first parameter of Mod(<number>,<number>)");
    pInt v1 = plist->First(); //plist->PopFront(); 


    PError::AssertType( plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of Mod(<number>,<number>)");
    pInt v2 = plist->Nth(2); //plist->PopFront(); 

 
    double remainder =((pDouble)v1/v2 - (v1/v2)); 
    if( remainder < 0 ) remainder += 1.0;
    if(remainder >= 1.0) remainder = 0;
    
    return Variant((pInt)(remainder *  v2 + .00001));
}


//Modular Arithmetic
Variant PEBLMath::Div(Variant v)
{
    PError::SignalFatalError("Function [Div] Not implemented.");
    return Variant(0);
}


//Coercion
Variant PEBLMath::ToInt(Variant v)
{
    
    
    PList * plist = v.GetComplexData()->GetList();
    return Variant(plist->First().GetInteger());
}

Variant PEBLMath::ToFloat(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    return Variant(plist->First().GetFloat());

}

Variant PEBLMath::ToString(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    return Variant(plist->First().GetString());
}


Variant PEBLMath::ToNumber(Variant v)
{
   
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();
    
    //If v is a string, then turn it either into an integer or a float,
    //depending on whether there is a '.' in it.
    
    if(v1.IsString())
        {
            bool hasPoint = false;
            
            std::string tmp = v.GetString();
            std::string::iterator i = tmp.begin();
            
            while(i != tmp.end() && hasPoint == false)
                {
                    if(*i  == '.')
                        hasPoint = true;
                    i++;
                }
            
            if(hasPoint)
                {
                    return Variant(v1.GetFloat());
                }
            else
                {
                    return Variant(v1.GetInteger());
                }
                     
        }
    
   return Variant(v1.GetFloat());
}

//Other
Variant PEBLMath::Sign(Variant v)
{

    
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            return (v1.GetFloat()<0)? -1: 1;
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Sign);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in Function [Sign]");
            return 0;
        }
    
}




Variant PEBLMath::Abs(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();
    if(v1.IsNumber())
        {
            return Variant((pDouble)fabsl(v1.GetFloat()));
        }
    else if(v1.GetComplexData()->IsList())
        {
            return Recurse(v1, Abs);

        }
    else
        {
            PError::SignalFatalError("Non-numeric argument in Function [AbsFloor]");
            return 0;
        }
    
}


// statistical functions replaced with PEBL functions.
//
// //Statistical (operates on a list; ignores non-numeric elements.)
// Variant PEBLMath::Mean(Variant v)
// {
//    PError::SignalFatalError("Function [Mean] Not implemented."); 
//    return Variant(0);
// }

// Variant PEBLMath::Min(Variant v)
// {
//     
//     PList * plist = v.GetComplexData()->GetList();

//     if(plist->First().IsComplexData())
//         {
//             if(plist->First().GetComplexData()->IsList())    
//                 {
//                     Variant retval= Recurse(v, Min);
//                     //This is a list; extract the first item and return it.
//                     return retval.GetComplexData()->GetList()->First();
//                 }
//             PError::SignalFatalError("Min() can only take numbers or lists as arguments.");
//             return Variant(0);
//         }
    

//     Variant min = plist->First();
//     std::vector<Variant>::const_iterator i = plist->Begin();
    
//     while(i != plist->End())
//         {
//             PError::AssertType(*i, PEAT_NUMBER, "Arguments of Min must be numbers.");
//             if(*i < min)
//                 min = *i;
//             i++;
//         } 
//     return min;
// }



// Variant PEBLMath::Max(Variant v)
// {
//     
//     PList * plist = v.GetComplexData()->GetList();

//     if(plist->First().IsComplexData())
//         {
//             if(plist->First().GetComplexData()->IsList())    
//                 {
//                     Variant retval = Recurse(v, Max);
//                     return retval.GetComplexData()->GetList()->First();
//                 }
//             PError::SignalFatalError("Min() can only take numbers or lists as arguments.");
//             return Variant(0);
//         }
    
//     Variant max = plist->First();
//     std::vector<Variant>::const_iterator i = plist->Begin();
    
//     while(i != plist->End())
//         {
//             PError::AssertType(*i, PEAT_NUMBER, "Arguments of Max must be numbers.");
//             if(*i > max)
//                 max = *i;
//             i++;
//         } 
//     return max;
// }



// Variant PEBLMath::StDev(Variant v)
// {
//     PError::SignalFatalError("Function [StDev] Not implemented.");
//     return Variant(0);
// }

// Variant PEBLMath::Median(Variant v)
// {
//     PError::SignalFatalError("Function [Median] Not implemented.");
//     return Variant(0);

// } 


///This will explicitly re-seed the RNG with the time of day.  Useful for
/// multi-session experiments where you want to use the same random seed across
/// days to randomize, but want differences in order between days, etc.

Variant PEBLMath::RandomizeTimer(Variant v)
{
   time_t now = time(0);
   srand((unsigned int)now);
   long unsigned int tmp = (long unsigned int)now;
   return Variant(tmp);
    
}
///This explicitly seeds the Random Number Generator.
///By Default, The PEBL Environment will call this on a time-of-day function on start-up.
Variant  PEBLMath::SeedRNG(Variant v)
{
   //v[1] should have the parameter
    
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First().GetInteger(), PEAT_NUMBER, "Argument error in function [SeedRNG(<number>)]:  ");    
    
    pInt seed = plist->First();
    srand((unsigned int)seed);
    
    return Variant(true);
}



/// This function takes no parameters, and returns
/// a floating-point Variant between 0 and 1.
Variant  PEBLMath::Random (Variant v)
{
   //v[1] should have nothing in it.

    return Variant(PEBLUtility::RandomUniform());
}


/// This function takes one parameter, which should be 
/// a positive integer.  It will return an integer between
/// 1 and argument (inclusive), each with equal probability.  If 
/// the argument is a float, it will get truncated down;
/// if it is less than 1,  1 will be returned, but a warning message will be triggered.
Variant  PEBLMath::RandomDiscrete (Variant v)
{
   //v[1] should have the parameter
    
    PList * plist = v.GetComplexData()->GetList();


    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in function [RandomDiscrete(<number>)]:  ");    
    
    int v1 = plist->First();
    if(v1 < 1)
        {
            string message = "Function [RandomDiscrete] requires argument >= 1; received ";
            message += v1;
            message += ". Returning 1.";
            PError::SignalWarning(message);
            return Variant(1);
        }
            
    //Since the most-significant digits are a bit predictable, use the remainder function
    unsigned int r = rand();
    return Variant((pInt)(r % v1)+ 1 );
    
}


/// This function takes one parameter, which should be a positive
/// number.  It will return a floating-point variant between 0 and 
/// the argument.
Variant  PEBLMath::RandomUniform(Variant v)
{

    
    PList * plist = v.GetComplexData()->GetList();

   //v[1] should have the parameter

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in function [RandomUniform(<number>)]:  ");    
    
    pDouble upperbound = plist->First(); //plist->PopFront();
    if(upperbound < 0)
        {
            std::ostringstream  message;
            message << "Function [RandomUniform] requires argument >= 0; received "
                    << upperbound;
            PError::SignalFatalError(message.str().c_str());
        }
    
    return Variant(upperbound * PEBLUtility::RandomUniform());
}




/// This function takes two parameters (mean and stdev),  It will return a floating-
/// point number which is a sample from the N(0,1) distribution.
/// Values are calculated using the Box-Mueller technique.
Variant  PEBLMath::RandomNormal(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();

    //v[1] should be mean, v[2] should be standard deviation.

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [RandomNormal(<mean>, <stdev>)]:  ");    
    pDouble mean = plist->First(); //plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter function [RandomNormal(<mean>, <stdev>)]:  ");    
    pDouble stdev = plist->Nth(2);// plist->PopFront();
 
    if(stdev < 0)  PError::SignalFatalError("Standard Deviation parameter of function [RandomNormal(<mean>,<stdev>)] must be positive.");

    return Variant(mean + PEBLUtility::RandomNormal()*stdev);
}

/// This function takes a single parameter; the mean of
/// of an exponential random variable. It will return a floating-point
/// value which is a sample from this distribution.
Variant  PEBLMath::RandomExponential(Variant v)
{
   //v[1] should be lambda, the mean of the Exponential RV.
    
    PList * plist = v.GetComplexData()->GetList();


    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in function [RandomExponential(<number>)]:  ");    
    pDouble lambda = plist->First(); //plist->PopFront();
       
    //The value of lambda may not be able to vary freely--It may not be able to be negative.
    //This should be checked for.

    return Variant(-lambda * log(PEBLUtility::RandomUniform()));
}


/// This function takes no  parameters; It will return a floating-point
/// value which is a sample from the vanilla logistic distribution f(x) = exp(x)/(1+exp(x))
Variant  PEBLMath::RandomLogistic(Variant v)
{
    //v should have no parameters
    
    double p = PEBLUtility::RandomUniform();
    
    return Variant(log(p/(1-p)));
}


/// This function takes two parameters, the spread and median
/// of a log-normal random variable.
Variant  PEBLMath::RandomLogNormal(Variant v)
{

    
    PList * plist = v.GetComplexData()->GetList();

    //v[1] should be median, v[2] should be the spread
    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter function [RandomLogNormal(<median>, <spread>)]:  ");    
    pDouble median = plist->First(); //plist->PopFront();
    
    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter function [RandomLogNormal(<median>, <spread>)]:  ");    
    pDouble spread = plist->Nth(2);// plist->PopFront();
    
    return Variant(median * exp(spread * PEBLUtility::RandomNormal()));
}

/// This takes a value p and a value N.  It will generate N random
/// numbers and report the count of the numbers that are below p.
Variant  PEBLMath::RandomBinomial(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();
 
   //v[1] should have the parameter

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter function [RandomBinomial(<p>, <N>)]:  ");    
    pDouble p = plist->First();// plist->PopFront();
    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter function [RandomBinomial(<p>, <N>)]:  ");    
    pInt N = plist->Nth(2); //plist->PopFront();
    
    pInt count = 0;
    for(int i = 0; i < N; i++)
        {
            if(PEBLUtility::RandomUniform() < p) count++;
        }
    
 
    return Variant(count);
}


/// This takes a value p and returns either 1 with probability p and 0 
/// with probability 1-p.
Variant  PEBLMath::RandomBernoulli(Variant v)
{
    
    PList * plist = v.GetComplexData()->GetList();

   //v[1] should have the parameter

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter function [RandomBinomial(<p>, <N>)]:  ");    
    pDouble p = plist->First();// plist->PopFront();
    
    if(PEBLUtility::RandomUniform() < p) 
        return Variant(1);
    else 
        return Variant(0);
}
