//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PParallelPort.h
//    Purpose:    Class for handling Parallel Port Communication
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2011 Shane T. Mueller <smueller@obereed.net>
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
#ifndef __PPARALLELPORT_H__
#define __PPARALLELPORT_H__

#include "PDevice.h"
#include "../base/PEBLObject.h"
#include <fstream>
#include <string>



class PParallelPort: public PDevice, virtual public PEBLObjectBase
{
    
public:    


    ///The Standard constructor.  
    PParallelPort();
    
    ///The Standard destructor.  
    virtual ~PParallelPort();
    
    virtual void Init();
    virtual void SetPort(Variant v);
    virtual void Close();
    virtual char GetStatusState();
    virtual char GetDataState();
    virtual void SetDataState(char x);
    virtual void EmulateStandardPort();
    virtual void SetOutputMode();
    virtual void SetInputMode();
    virtual int  GetState(int iface);

protected:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const; 

    unsigned int mPort;
    bool mIsOpen;

private:


};



#endif
