//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/utility/PLabStreamingLayer.cpp
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

#ifdef PEBL_USE_LSL

#include "PLabStreamingLayer.h"
#include <iostream>

// Include LSL C API header from LabRecorder's bundled liblsl
extern "C" {
    #include <lsl_c.h>
}

PLSL::PLSL(const std::string& name,
           const std::string& type,
           const std::string& source_id)
    : mInfo(NULL),
      mOutlet(NULL)
{
    // Create stream info using C API
    // lsl_create_streaminfo(name, type, channel_count, nominal_srate, channel_format, source_id)
    mInfo = lsl_create_streaminfo(
        name.c_str(),
        type.c_str(),
        1,                           // Channel count (1 for single marker channel)
        0.0,                         // LSL_IRREGULAR_RATE - irregular/event-driven
        cft_string,                  // Channel format - string markers
        source_id.empty() ? NULL : source_id.c_str()
    );

    if (!mInfo) {
        std::cerr << "ERROR: Failed to create LSL stream info\n";
        return;
    }

    // Create the LSL outlet
    mOutlet = lsl_create_outlet(mInfo, 0, 360);  // chunk_size=0 (auto), max_buffered=360s

    if (!mOutlet) {
        std::cerr << "ERROR: Failed to create LSL outlet\n";
        lsl_destroy_streaminfo(mInfo);
        mInfo = NULL;
        return;
    }

    std::cerr << "LSL outlet created: [" << name << "] type=[" << type << "] source=["
              << (source_id.empty() ? "auto" : source_id) << "]\n";
}

PLSL::~PLSL()
{
    if (mOutlet) {
        lsl_destroy_outlet(mOutlet);
        mOutlet = NULL;
        std::cerr << "LSL outlet closed\n";
    }
    if (mInfo) {
        lsl_destroy_streaminfo(mInfo);
        mInfo = NULL;
    }
}

void PLSL::SendMarker(const std::string& marker)
{
    if (mOutlet) {
        // Push string sample (LSL automatically timestamps it)
        const char* data = marker.c_str();
        lsl_push_sample_str(mOutlet, &data);
    }
}

void PLSL::SendMarker(int marker)
{
    if (mOutlet) {
        // Convert integer to string and send
        std::string str_marker = std::to_string(marker);
        const char* data = str_marker.c_str();
        lsl_push_sample_str(mOutlet, &data);
    }
}

void PLSL::SendMarkerTimed(const std::string& marker, double timestamp)
{
    if (mOutlet) {
        // Push sample with explicit timestamp
        // pushthrough=1 means push immediately, 0 means use buffering
        const char* data = marker.c_str();
        lsl_push_sample_strtp(mOutlet, &data, timestamp, 1);
    }
}

bool PLSL::HaveConsumers()
{
    return mOutlet ? (lsl_have_consumers(mOutlet) != 0) : false;
}

bool PLSL::WaitForConsumers(double timeout)
{
    return mOutlet ? (lsl_wait_for_consumers(mOutlet, timeout) != 0) : false;
}

#else  // !PEBL_USE_LSL - Stub implementations when LSL is not available

#include "PLabStreamingLayer.h"

// No-op constructor
PLSL::PLSL(const std::string& name,
           const std::string& type,
           const std::string& source_id)
    : mInfo(NULL),
      mOutlet(NULL)
{
    // LSL not available - no-op
}

// No-op destructor
PLSL::~PLSL()
{
    // LSL not available - no-op
}

// No-op marker functions
void PLSL::SendMarker(const std::string& marker)
{
    // LSL not available - no-op
}

void PLSL::SendMarker(int marker)
{
    // LSL not available - no-op
}

void PLSL::SendMarkerTimed(const std::string& marker, double timestamp)
{
    // LSL not available - no-op
}

// Always return false when LSL not available
bool PLSL::HaveConsumers()
{
    return false;
}

bool PLSL::WaitForConsumers(double timeout)
{
    return false;
}

#endif // PEBL_USE_LSL
