/* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PParellelPort.cpp
//    Purpose:    Class for handling parallel port
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2010-2025 Shane T. Mueller <smueller@obereed.net>
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
#include "PParallelPort.h"
#include "../utility/PError.h"

#if defined (PEBL_LINUX)
#include <sys/io.h>
#endif


#if defined (PEBL_WIN32)
#include <winsock2.h> //avoid collision
#include <windows.h>
//#include <conio.h>  // Not available in MSYS2, commented out if not needed
#include <stdio.h>

//Prototype function typefed for DLL function Inp32:
  typedef short (_stdcall *inpfuncPtr)(short portaddr);
  typedef void (_stdcall *oupfuncPtr)(short portaddr, short datum);


#endif

using std::ostream;
using std::fstream;
using std::flush;
using std::string;
using std::cout;
using std::endl;

//#define mPort 0x378

int Inp32(int i){return 1;};
int Out32(int i,int x ){return 1;};

enum PEBLPPort
    {
        PEBLPPortLPT1=0x378,
        PEBLPPortLPT2=0x278,
        PEBLPPortLPTX=0x3BC
    };

PParallelPort::PParallelPort():
    mPort(PEBLPPortLPT1),
    mIsOpen(false)
{
    mCDT = CDT_PARALLELPORT;
}


PParallelPort::~PParallelPort()
{
    if(mIsOpen)
        Close();
}



void PParallelPort::Init()
{

#ifdef PEBL_LINUX
    //This works on linux: open 3 bytes to access
    int out = ioperm(mPort, 3, 1);
    //int out = iopl(3);
    mIsOpen = true;
    std::cerr << "Initiating parallel port.  Return value :["<<out<<"]\n";


    //    EmulateStandardPort();
    if(out)
        {
            PError::SignalFatalError("Unable to Access Parallel Port.  Make sure script is run with root access.\n");
        }


#elif defined(PEBL_WIN32)
   HINSTANCE hLib;
   inpfuncPtr inp32fp;
   oupfuncPtr oup32fp;
   hLib = LoadLibrary("inpout32.dll");
   if(hLib == NULL)
   {
      PError::SignalFatalError("Unable to load library inpout32.dll.  Cannot initiate parallel port device");
   }

// Get the address of the Input function

     inp32fp = (inpfuncPtr) GetProcAddress(hLib, "Inp32");

     if (inp32fp == NULL)
     {
       PError::SignalFatalError("Unable to get Inp32.");
     }


     oup32fp = (oupfuncPtr) GetProcAddress(hLib, "Out32");

     if (oup32fp == NULL) {

        PError::SignalFatalError("Unable to get oup32");
     }


#endif




}

#ifdef SetPort
#undef SetPort  // Undefine Windows API macro that conflicts with our method
#endif

void PParallelPort::SetPort(Variant v)
{
    if(v=="LPT1")
        {
            mPort = PEBLPPortLPT1;

        }else if(v=="LPT2")
        {
            mPort = PEBLPPortLPT2;
        } else if(v=="LPTX")
        {
            mPort = PEBLPPortLPTX;
        }else{

        //SHould allow to set to arbitrary hex-coded port address here.
        std::cerr << "Cannot set port to ["<<v<<"].  Setting to LPT1.\n";
    }

}


void PParallelPort::Close()
{
#ifdef PEBL_LINUX
    int out =ioperm(mPort,3,0);
#elif defined (PEBL_WIN32)

 //This doesnot work yet:
//    FreeLibrary(hLib);
#endif
    mIsOpen = false;
}


////////////////////////////////////////////////
//Returns a byte (char) with five lowest bits
//indicating state of pins:
// 15, Error
// 13, Select
// 12, Paper empty
// 10, ACK
// 11. Busy

char PParallelPort::GetStatusState()
{
#if defined(PEBL_LINUX)
    return inb(mPort+1);
#elif defined (PEBL_WIN32)
    return Inp32(mPort+1);
#else
    return '\0';
#endif
    //    return (inb(mPort+1)>>3)^0x10;
}

//Gets a byte whos bits are pins 2...9 (for dual-mode ports)
//
char PParallelPort::GetDataState()
{
#if defined(PEBL_LINUX)
    return (inb(mPort));

#elif defined (PEBL_WIN32)
    return Inp32(mPort);
#else
    return '\0';
#endif

}

//sets the data bytes (pins 2..9) to the specified state.
void PParallelPort::SetDataState(char x)
{
#if defined(PEBL_LINUX)
     outb(x,mPort);
#elif defined (PEBL_WIN32)
     Out32(mPort,x);
#endif

}


void PParallelPort::EmulateStandardPort()
{
//Not functional

}


// Sets port to output mode.  This works for dual-mode ports.
//
void PParallelPort::SetOutputMode()
{


#if defined(PEBL_LINUX)
        unsigned char x;
    x = inb(mPort+2);  //get state of control register
    outb(x & ~0x20, mPort+2);
#elif defined (PEBL_WIN32)
        unsigned char x;
    x = Inp32(mPort+2);
    Out32(mPort+2,x&~0x20);
#endif


}


// Sets port to input mode
//
void PParallelPort::SetInputMode()
{

#if defined(PEBL_LINUX)
        unsigned char x = inb(mPort+2);
    //  turns 7th bit of the control byte  on
    outb( x | 0x20, mPort+2);
#elif defined (PEBL_WIN32)
        unsigned char x = Inp32(mPort+2);
    Out32(mPort+2,x|0x20);
#endif


}

ostream & PParallelPort::SendToStream(ostream & out) const
{
    out << "<Generic Parallel Port Object>" << flush;
    return out;
}



//Interface: 0 for data bits; 1 for status bits
int PParallelPort::GetState(int iface)
{
    int out =0;
    if(iface==0)
        {
            SetOutputMode();
            SetDataState((char)0);
            SetInputMode();
            out = GetDataState();
        } else
        if(iface==1)
            {
                out = GetStatusState();
            }

     return out;
}
