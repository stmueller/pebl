//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformEventQueue.h
//    Purpose:    Validator Platform Event Queue (stub - returns no events)
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//   This file is part of the PEBL project.
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
#ifndef __VALIDATOR_PLATFORMEVENTQUEUE_H__
#define __VALIDATOR_PLATFORMEVENTQUEUE_H__

#include "../../devices/PEventQueue.h"

/// Headless platform event queue - always empty, no events
/// Used for validation mode where no actual interaction occurs
class PlatformEventQueue : public PEventQueue {
public:
    PlatformEventQueue();
    virtual ~PlatformEventQueue();

    // Prime does nothing in validator mode - no events to collect
    virtual void Prime();

private:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

#endif
