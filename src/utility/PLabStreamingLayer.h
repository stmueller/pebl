//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/utility/PLabStreamingLayer.h
//    Purpose:    PEBL wrapper for Lab Streaming Layer (LSL)
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
#ifndef __PLSL_H__
#define __PLSL_H__

#include <string>

#ifdef PEBL_USE_LSL

// Forward declare LSL C API types to avoid including problematic lsl_cpp.h
// We'll use the C API directly to avoid namespace pollution issues
// Note: lsl_outlet and lsl_streaminfo are already pointer types in lsl_c.h
extern "C" {
    typedef struct lsl_streaminfo_struct_* lsl_streaminfo;
    typedef struct lsl_outlet_struct_* lsl_outlet;
}

/**
 * PLSL - PEBL wrapper for Lab Streaming Layer
 *
 * Uses LSL C API directly to avoid namespace pollution from lsl_cpp.h
 */
class PLSL
{
public:
    // Constructor - creates LSL outlet for string markers
    PLSL(const std::string& name,
         const std::string& type,
         const std::string& source_id);

    // Destructor - closes outlet
    ~PLSL();

    // Send string marker
    void SendMarker(const std::string& marker);

    // Send integer marker (converted to string)
    void SendMarker(int marker);

    // Send marker with explicit timestamp
    void SendMarkerTimed(const std::string& marker, double timestamp);

    // Check if anyone is listening to this stream
    bool HaveConsumers();

    // Wait for consumers to connect (returns true if consumers found)
    bool WaitForConsumers(double timeout);

    // Check if outlet was successfully created
    bool IsValid() const { return mOutlet != NULL; }

private:
    lsl_streaminfo mInfo;
    lsl_outlet mOutlet;
};

#else  // !PEBL_USE_LSL - Stub class declaration when LSL is disabled

/**
 * PLSL - Stub implementation when LSL is not available
 *
 * All methods become no-ops
 */
class PLSL
{
public:
    // Constructor - no-op
    PLSL(const std::string& name,
         const std::string& type,
         const std::string& source_id);

    // Destructor - no-op
    ~PLSL();

    // Send string marker - no-op
    void SendMarker(const std::string& marker);

    // Send integer marker - no-op
    void SendMarker(int marker);

    // Send marker with explicit timestamp - no-op
    void SendMarkerTimed(const std::string& marker, double timestamp);

    // Always returns false when LSL disabled
    bool HaveConsumers();

    // Always returns false when LSL disabled
    bool WaitForConsumers(double timeout);

    // Always false when LSL disabled
    bool IsValid() const { return false; }

private:
    void* mInfo;    // Placeholder
    void* mOutlet;  // Placeholder
};

#endif // PEBL_USE_LSL
#endif // __PLSL_H__
