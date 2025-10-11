//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PNetwork.h
//    Purpose:    Class for handling network communication (tcpip)
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
#ifndef __PNETWORK_H__
#define __PNETWORK_H__

#include "PDevice.h"
#include "../base/PEBLObject.h"
#include <fstream>
#include <string>



///  This is the base File Stream class.  
///  It is used to read and write files.
///  It is also a subclass of PDevice, which means that it can be added to the 
///  event loop for interesting between-process communications, and probably even used as
///  a basis for other input devices.

class PNetwork: public PDevice, virtual public PEBLObjectBase
{
    
public:    


    ///The Standard constructor.  
    PNetwork();
    
    ///The Standard destructor.  
    virtual ~PNetwork();
    
    virtual void SetHostIP(unsigned int host);
    virtual unsigned int GetHost();
    
    virtual void SetHostName(std::string hostname);
    virtual std::string GetHostName();

    virtual void SetPort(unsigned int port);
    virtual unsigned int GetPort();
    
    virtual bool IsOpen();
    virtual Variant ConvertAddress(unsigned long int address);

    virtual void SetOpen(bool open);
protected:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const; 

    std::string mHostName;
    unsigned int mHost;
    unsigned int mPort;

    bool mIsOpen;

private:


};



#endif
