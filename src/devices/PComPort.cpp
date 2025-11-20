/* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PParellelPort.cpp
//    Purpose:    Class for handling parallel port
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2011-2025 Shane T. Mueller <smueller@obereed.net>
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

#ifdef PEBL_USEPORTS
#include "PComPort.h"

#include "../utility/rs232.h"
#include "../utility/PError.h"


#if defined (PEBL_LINUX)
#include <sys/io.h>
#endif

// Undefine Windows API SetPort macro that conflicts with our method name
#ifdef SetPort
#undef SetPort
#endif

using std::ostream;
using std::fstream;
using std::flush;
using std::string;
using std::cout;
using std::endl;


PComPort::PComPort(unsigned int port,
                   unsigned int baud,
                   std::string mode):
    mMode(mode),
    mIsOpen(false)
{
    mCDT = CDT_COMPORT;
    SetPort(port,baud);
}


PComPort::~PComPort()
{
    if(mIsOpen)
        RS232_CloseComport(mPort);
    mIsOpen = false;
}



void PComPort::Init()
{

    //init
    if(!mIsOpen)
        {

            //mode 8N1 was pre-2.0 default
            //flowctrl=0 means no hardware flow control (RTS/CTS disabled)
            int out = RS232_OpenComport(mPort,mBaud,mMode.c_str(),0);

            mIsOpen = true;
            std::cerr << "Initiating comport ["<< mPortName<<"]  Return value :["<<out<<"]\n";


            if(out)
                {
                    PError::SignalFatalError("Unable to Open Com Port.  Make sure script is run with root access.\n");
                }
        }

}

#ifdef SetPort
#undef SetPort  // Undefine Windows API macro that conflicts with our method
#endif

void PComPort::SetPort(unsigned int portnum,unsigned int baud)
{

#ifdef PEBL_LINUX

    int bauds[23] ={50,75,110,134,150,200,300,600,1200,1800,2400,4800,9600,19200,38400,57600,115200,230400,
                    460800,500000,576000,921600,1000000};

    if(portnum < 16)
        {
            mPortName = Variant("ttyS")+Variant((int)portnum);
        }
    else if(portnum < 22)
        {
            mPortName = Variant("ttyUSB")+Variant((int)(portnum-16));
        }
    else
        {
            PError::SignalFatalError(Variant("Attempted to set improper port number:")+Variant((int)portnum));
        }


#else
    int bauds[19] ={0,0,110,0,0,0,300,600,1200,0,2400,4800,9600,19200,38400,57600,115200,128000,256000};

    if(portnum < 16)
        {
            mPortName = Variant("COM")+Variant((int)(portnum+1));
        }
#endif
    //baud rate should be checked against legal bauds on each platform.
    mBaud = baud;
    mPort = portnum;
}


void PComPort::PSendByte(unsigned char byte)
{
    int check = RS232_SendByte(mPort,byte);

    //if check == 1, nothing was sent
}


int PComPort::GetByte(unsigned char& out)
{

    int check = RS232_PollComport(mPort,&out,1);
    return check;

}



unsigned char PComPort::GetByte()
{
    unsigned char out;
    int check = RS232_PollComport(mPort,&out,1);
    if(check==0)
        {
            return 0;
        } else
        {

            return out;
        }
}




Variant PComPort::GetBytes(int n)
{


    unsigned char *out;
    out = new unsigned char[n];

    int check = RS232_PollComport(mPort,out,n);
    Variant got = Variant(out);
    //    int get = n;
    //    Variant got ="";
    //    while(get > 0)
    //        {
    //            int x = get>100?100:get;
    //
    //            int check = PollComport(mPort,&out,get);
    //            got = got + Variant(out[]);
    //            get -= check;
    //        }

    return got;
}

Variant PComPort::GetPortName()
{
    return mPortName;
}

void PComPort::Close()
{

    RS232_CloseComport(mPort);
}




ostream & PComPort::SendToStream(ostream & out) const
{
    out << "<Generic Com Port Object>" << flush;
    return out;
}


#endif
