//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLLSL.h
//    Purpose:    LSL (Lab Streaming Layer) built-in functions for PEBL
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2026 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
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
#ifndef __PEBLLSL_H__
#define __PEBLLSL_H__

#include "../base/Variant.h"

// Phase 1: Outlet functions (send markers)
// Always declared - stub implementations available when PEBL_USE_LSL not defined
Variant CreateLSLOutlet(Variant v);
Variant SendLSLMarker(Variant v);
Variant CloseLSLOutlet(Variant v);
Variant LSLHasConsumers(Variant v);
Variant LSLLocalClock(Variant v);

// Phase 2: Inlet functions (receive triggers) - To be implemented later
// Variant CreateLSLInlet(Variant v);
// Variant WaitForLSLTrigger(Variant v);
// Variant PollLSLSample(Variant v);
// Variant WaitForLSLSample(Variant v);
// Variant CloseLSLInlet(Variant v);

#endif // __PEBLLSL_H__
