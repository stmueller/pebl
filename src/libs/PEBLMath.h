//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       libs/PEBLMath.h
//    Purpose:    Mathematical Functions Library for PEBL
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
#ifndef __PEBLMATH_H__
#define __PEBLMATH_H__

class Variant;



namespace PEBLMath
{
    Variant Recurse(Variant v, Variant (*)( Variant));

    //This allows one to invoke Recurse() with arguments.
    Variant Recurse2(Variant v, Variant (*)( Variant), Variant argument);

    //Logarithmic Functions
    Variant Log10(Variant v);
    Variant Log2(Variant v);
    Variant Ln(Variant v);
    Variant LogN(Variant v);
    Variant Exp(Variant v);
    Variant Pow(Variant v);
    Variant Sqrt(Variant v);
    Variant NthRoot(Variant v);

    //Trigonometric Functions.
    Variant Tan(Variant v);
    Variant Sin(Variant v);
    Variant Cos(Variant v);
    Variant ATan(Variant v);
    Variant ASin(Variant v);
    Variant ACos(Variant v);
    Variant DegToRad(Variant v);
    Variant RadToDeg(Variant v);


    //Rounding
    Variant Round(Variant v);
    Variant Floor(Variant v);
    Variant Ceiling(Variant v);

    Variant AbsFloor(Variant v);
     
    //Modular Arithmetic
    Variant Mod(Variant v);
    Variant Div(Variant v);
    
    //Coercion
    Variant ToInt(Variant v);
    Variant ToFloat(Variant v);
    Variant ToNumber(Variant v);
    Variant ToString(Variant v);

        
    //Other
    Variant Sign(Variant v);
    Variant Abs(Variant v);


    //Statistical (operates on a list; ignores non-numeric elements.)

    Variant RandomizeTimer(Variant v);
    Variant SeedRNG(Variant v);
    Variant Random (Variant v);
    Variant RandomDiscrete (Variant v);
    Variant RandomUniform(Variant v);
    Variant RandomNormal(Variant v);
    Variant RandomExponential(Variant v);
    Variant RandomLogistic(Variant v);
    Variant RandomLogNormal(Variant v);
    Variant RandomBinomial(Variant v);
    Variant RandomBernoulli(Variant v);
}


#endif
