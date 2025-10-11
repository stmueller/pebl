//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformTimer.cpp
//    Purpose:    SDL-specific timer event device
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
#include "PlatformTimer.h"
#include "SDL.h"
#include <iostream>
#include "../../devices/DeviceState.h"


#include <sys/time.h>
#if defined(PEBL_UNIX) || defined(PEBL_EMSCRIPTEN)

//Nothing special here

#elif defined( PEBL_WINDOWS)
#include <winsock2.h> //avoid collision

 #include <windows.h>
 #include <iostream>
 #include <iomanip>
 #include <stdio.h>
#endif

#undef PEBL_CHECKTIMER

using namespace std;

/// This is the standard PlatformTimer constructor
PlatformTimer::PlatformTimer()
{

}

/// This is the standard pNode destructor
PlatformTimer::~PlatformTimer()
{
    // Standard Destructor
}




// The following function is not used anymore.
//
void PlatformTimer::Wait(unsigned long int msecs)
    //This does a busy wait; could be augmented with interrupts or RTC calls if necessary.
    //Both version should be nearly identical--all the ifdefs makes it really difficult
    //to understand inline, so I just replicated the function.
#ifdef PEBL_CHECKTIMER
{
    ///This should only run when PEBL_CHECKTIMER is defined.

    //This should check for wrapping at ~49 days.

    unsigned long int timeX = SDL_GetTicks() + msecs;
    unsigned long int missedticks = 0;
    unsigned long int missedms = 0;
    unsigned long int lasttime = 0;

    unsigned long int time=SDL_GetTicks();

    while(true)
        {
            lasttime = time;

            time = SDL_GetTicks();
            if((time-lasttime) > 1)
                {
                    missedticks++;
                    missedms += (time-lasttime);
                }


            if(time > timeX)
                {

                    cerr << "Timer missed [" << missedticks << "] ticks, for a total of [" << missedms << "] ms, on a wait of [" << msecs <<"] ms.\n";
                    break;
                }
        }
}
#else
{
    //This does a busy wait; could be augmented with interrupts or RTC calls if necessary.
    //This should check for wrapping at ~49 days.

    unsigned long int timeX = SDL_GetTicks() + msecs;
    unsigned long int time;

    while(true)
        {
            time = SDL_GetTicks();
            if(time > timeX)
                break;

        }
}

#endif



unsigned long int PlatformTimer::GetTime() const
{
    return SDL_GetTicks();
}

int PlatformTimer::GetState(int iface) const
{
    //There is no interface for the timer.
    return SDL_GetTicks();
}

// int PlatformTimer::TestDevice(const DeviceState & state) const
// {
//     int time = GetTime();
//     int result = state.TestDevice(time);
//     return result;
// }

void PlatformTimer::GetTimeOfDay(unsigned long & secs, unsigned long & msecs)
{
#if defined( PEBL_UNIX) || defined(PEBL_EMSCRIPTEN)
      struct timeval * tp=NULL;
       gettimeofday(tp,NULL);

       time_t        secs1 = tp->tv_sec;
       suseconds_t usecs  = tp->tv_usec;

    secs = secs1;
    msecs = usecs/100;
#else

   SYSTEMTIME st;
    ::GetSystemTime(&st);


    std::cout << std::setw(2) << st.wHour << ':'
              << std::setw(2) << st.wMinute << ':'
              << std::setw(2) << st.wSecond << '.'
              << std::setw(3) << st.wMilliseconds << '\n';

//    return timeGetTime();
#endif
}


ostream & PlatformTimer::SendToStream(ostream & out) const
{
    out << "<SDL Platform Timer>" << std::flush;
    return out;
}
