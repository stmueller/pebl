//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformTimer.cpp
//    Purpose:    Validator Platform Timer Implementation
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
#include "PlatformTimer.h"
#include <thread>
#include <sys/time.h>

PlatformTimer::PlatformTimer()
    : mStartTime(std::chrono::steady_clock::now()) {
}

PlatformTimer::~PlatformTimer() {
}

void PlatformTimer::Wait(unsigned long int msecs) {
    std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
}

void PlatformTimer::Sleep(unsigned long int msecs) {
    // For validator platform, Sleep and Wait are identical (both yield CPU)
    std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
}

unsigned long int PlatformTimer::GetTime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - mStartTime);
    return static_cast<unsigned long int>(duration.count());
}

void PlatformTimer::GetTimeOfDay(unsigned long & secs, unsigned long & msecs) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    secs = tv.tv_sec;
    msecs = tv.tv_usec / 1000;
}

int PlatformTimer::GetState(int iface) const {
    return 0;
}

std::ostream& PlatformTimer::SendToStream(std::ostream& out) const {
    out << "PlatformTimer (validator)";
    return out;
}
