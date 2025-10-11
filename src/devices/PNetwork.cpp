//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PNetwork.cpp
//    Purpose:    Class for handling network communication
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2006-2025 Shane T. Mueller <smueller@obereed.net>
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
#include "PNetwork.h"
#include "../utility/PError.h"


using std::ostream;
using std::fstream;
using std::flush;
using std::string;
using std::cout;

PNetwork::PNetwork():
    mHostName(""),
    mHost(0),
    mPort(0),
    mIsOpen(false)
{
    InitializeProperty("NAME","<NETWORK>");
    InitializeProperty("HOSTNAME",0);
    InitializeProperty("HOSTIP",0);
    InitializeProperty("PORT",0);
    InitializeProperty("ISOPEN",0);
    mCDT = CDT_NETWORKCONNECTION;
}


PNetwork::~PNetwork()
{
}


Variant PNetwork::ConvertAddress(unsigned long int address)
{
    //Uint32 address;

    unsigned char octet[4] = {0,0,0,0};
    for (int i=0; i<4; i++)
    {
    octet[i] = (address >> (i*8) ) & 0xFF;
    }
    Variant v = "";
    v = v+Variant(octet[3])+
        Variant(".")+Variant(octet[2])+
        Variant(".")+ Variant(octet[1])+
        Variant(".")+ Variant(octet[0]);
    return v;
}
ostream & PNetwork::SendToStream(ostream & out) const
{
    out << "<Generic Network Object>" << flush;
    return out;
}



void PNetwork::SetHostIP(unsigned int host){
    mHost = host;
    PEBLObjectBase::SetProperty("HOSTIP",ConvertAddress(host));
}

unsigned int PNetwork::GetHost()
{
    return mHost;
}

void PNetwork::SetOpen(bool open)
{
    mIsOpen = open;
    PEBLObjectBase::SetProperty("ISOPEN",open);
}
    
void PNetwork::SetHostName(std::string hostname)
{
    PEBLObjectBase::SetProperty("HOSTNAME",hostname);
    mHostName = hostname;
}

std::string PNetwork::GetHostName()
{
    return mHostName;
}

void PNetwork::SetPort(unsigned int port)
{
    mPort = port;
    PEBLObjectBase::SetProperty("PORT",(int)mPort);
}

 unsigned int PNetwork::GetPort()
 {
     return mPort;
 }
    
bool PNetwork::IsOpen(){
    return mIsOpen;
};
