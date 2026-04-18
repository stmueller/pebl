//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLLSL.cpp
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

#ifdef PEBL_USE_LSL

// IMPORTANT: Include PLabStreamingLayer.h FIRST to avoid namespace pollution
// from lsl_cpp.h (which includes lsl_c.h inside namespace lsl)
#include "../utility/PLabStreamingLayer.h"
#include "PEBLLSL.h"
#include "../utility/PError.h"
#include "../base/Variant.h"
#include "../base/PComplexData.h"
#include "../base/PList.h"

// Forward declare LSL C API function
extern "C" {
    double lsl_local_clock();
}

// Global LSL outlet (singleton - one per PEBL instance)
static PLSL* gLSLOutlet = nullptr;

//============================================================================//
// Phase 1: Outlet Functions (Send Markers)
//============================================================================//

/**
 * CreateLSLOutlet - Create an LSL stream outlet
 *
 * Arguments:
 *   name - Stream name (e.g., "PEBL_stroop")
 *   type - Stream type (e.g., "Markers") [optional, default "Markers"]
 *   source_id - Unique identifier (e.g., "pebl_stroop_P001") [optional]
 *
 * Returns:
 *   1 on success, 0 on failure
 */
Variant CreateLSLOutlet(Variant v)
{
    PList* plist = v.GetComplexData()->GetList();

    if (plist->Length() < 1) {
        PError::SignalWarning("CreateLSLOutlet requires at least 1 argument (stream name)");
        return Variant(0);
    }

    std::string name = plist->First().GetString();
    std::string type = plist->Length() > 1 ? plist->Nth(2).GetString() : "Markers";
    std::string source_id = plist->Length() > 2 ? plist->Nth(3).GetString() : "";

    try {
        // Close previous outlet if exists
        if (gLSLOutlet) {
            delete gLSLOutlet;
            gLSLOutlet = nullptr;
        }

        // Create new outlet
        gLSLOutlet = new PLSL(name, type, source_id);

        return Variant(1);  // Success
    }
    catch (std::exception& e) {
        PError::SignalWarning("LSL outlet creation failed: " + std::string(e.what()));
        return Variant(0);  // Failure
    }
}


/**
 * SendLSLMarker - Send a marker through the LSL outlet
 *
 * Arguments:
 *   marker - String or integer marker
 *
 * Returns:
 *   1 on success, 0 on failure
 */
Variant SendLSLMarker(Variant v)
{
    if (!gLSLOutlet) {
        // Silently skip if not initialized (this is intentional - no error)
        return Variant(0);
    }

    PList* plist = v.GetComplexData()->GetList();

    if (plist->Length() < 1) {
        return Variant(0);
    }

    Variant marker = plist->First();

    try {
        if (marker.IsString()) {
            gLSLOutlet->SendMarker(marker.GetString());
        }
        else if (marker.IsInteger()) {
            gLSLOutlet->SendMarker(marker.GetInteger());
        }
        else if (marker.IsFloat()) {
            // Convert float to string
            gLSLOutlet->SendMarker(std::to_string(marker.GetFloat()));
        }
        else {
            // Convert to string as fallback
            gLSLOutlet->SendMarker(marker.GetString());
        }

        return Variant(1);
    }
    catch (std::exception& e) {
        PError::SignalWarning("LSL marker send failed: " + std::string(e.what()));
        return Variant(0);
    }
}


/**
 * CloseLSLOutlet - Close the LSL outlet
 *
 * Returns:
 *   1 on success
 */
Variant CloseLSLOutlet(Variant v)
{
    if (gLSLOutlet) {
        delete gLSLOutlet;
        gLSLOutlet = nullptr;
    }
    return Variant(1);
}


/**
 * LSLHasConsumers - Check if anyone is listening to the stream
 *
 * Returns:
 *   1 if consumers present, 0 otherwise
 */
Variant LSLHasConsumers(Variant v)
{
    if (!gLSLOutlet) {
        return Variant(0);
    }

    return Variant(gLSLOutlet->HaveConsumers() ? 1 : 0);
}


/**
 * LSLLocalClock - Get current LSL synchronized time
 *
 * Returns:
 *   Timestamp in seconds (double)
 */
Variant LSLLocalClock(Variant v)
{
    return Variant(lsl_local_clock());
}

#else  // !PEBL_USE_LSL - Stub implementations when LSL is disabled

#include "PEBLLSL.h"
#include "../base/Variant.h"

// All LSL functions become no-ops that return success/0 when LSL is disabled

Variant CreateLSLOutlet(Variant v)
{
    // No-op: Return success (LSL functions silently disabled)
    return Variant(1);
}

Variant SendLSLMarker(Variant v)
{
    // No-op: Return success (LSL functions silently disabled)
    return Variant(1);
}

Variant CloseLSLOutlet(Variant v)
{
    // No-op: Return success
    return Variant(1);
}

Variant LSLHasConsumers(Variant v)
{
    // No-op: No consumers when LSL disabled
    return Variant(0);
}

Variant LSLLocalClock(Variant v)
{
    // No-op: Return 0.0 when LSL disabled
    return Variant(0.0);
}

#endif // PEBL_USE_LSL
