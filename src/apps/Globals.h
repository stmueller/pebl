//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       apps/Globals.h
//    Purpose:    Definitions (should be renamed.)
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
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

//
// EVALUATOR ARCHITECTURE SELECTION:
//
// PEBL has two evaluator implementations:
//
// 1. RECURSIVE (Evaluator.cpp): Uses C++ call stack recursion.
//    - Works correctly on native builds
//    - Has hard recursion depth limit of 1 on Emscripten with Asyncify
//    - Crashes at depth >= 2 with "index out of bounds"
//
// 2. ITERATIVE (Evaluator-es.cpp): Uses manual stack management.
//    - Required for Emscripten builds to handle recursion correctly
//    - Uses explicit mNodeStack and mStack instead of C++ call stack
//    - Compatible with Asyncify call stack transformations
//    - Handles arbitrary recursion depth on all platforms
//
// The PEBL_ITERATIVE_EVAL macro controls which evaluator is included.
// For Emscripten builds (em target), this macro MUST be defined in CXXFLAGS.
//

#ifdef PEBL_ITERATIVE_EVAL
#include "../base/Evaluator-es.h"
#else
#include "../base/Evaluator.h"
#endif


enum PEBLVideoMode: unsigned int
	{
		PVM_512_384,
		PVM_640_480,
		PVM_800_600,
		PVM_960_720,
		PVM_1024_768,
		PVM_1152_864,
		PVM_1280_1024
	};

enum PEBLVideoDepth: unsigned int
	{
		PVD_2  = 2,
		PVD_15 = 15,
		PVD_16 = 16,
		PVD_24 = 24,
		PVD_32 = 32
	};

extern Evaluator * myEval;


//extern PEventLoop * gEventLoop;  //Make the event loop global so we can
                                 //write an async version.
#endif
