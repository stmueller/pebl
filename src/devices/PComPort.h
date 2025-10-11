//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PComPort.h
//    Purpose:    Class for handling com (serial) Port Communication
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
//
//  This class is targeted to the DLP-IO8 device, which is a USB device
//  whose driver (on linux, osx, and windows) makes it appear as a com 
//  device.  It may or may not work well on other devices.

#ifndef __PCOMPORT_H__
#define __PCOMPORT_H__

#ifdef PEBL_USEPORTS

#include "PDevice.h"
#include "../base/PEBLObject.h"
#include <fstream>
#include <string>

typedef signed short temp_t;

//  This data taken from Tyler Montbriand's
//  sensor8 front-end to the DLP-io8
//  http://burningsmell.org/sensor8/

enum
{
	// Reads temperature from a DS18B20 attached to the pin
	CMD_0_TEMP	='9',	CMD_1_TEMP	='0',
	CMD_2_TEMP	='-',	CMD_3_TEMP	='=',
	CMD_4_TEMP	='O',	CMD_5_TEMP	='P',
	CMD_6_TEMP	='[',	CMD_7_TEMP	=']',

	// Reads digital 1 or 0 from the pin
	CMD_0_DIN	='A',	CMD_1_DIN	='S',
	CMD_2_DIN	='D',	CMD_3_DIN	='F',
	CMD_4_DIN	='G',	CMD_5_DIN	='H',
	CMD_6_DIN	='J',	CMD_7_DIN	='K',

	// Reads analog voltage from the pin
	CMD_0_AIN	='Z',	CMD_1_AIN	='X',
	CMD_2_AIN	='C',	CMD_3_AIN	='V',
	CMD_4_AIN	='B',	CMD_5_AIN	='N',
	CMD_6_AIN	='M',	CMD_7_AIN	=',',

	// Sets the pin high (5V)
	CMD_0_HI	='1',	CMD_1_HI	='2',
	CMD_2_HI	='3',	CMD_3_HI	='4',
	CMD_4_HI	='5',	CMD_5_HI	='6',
	CMD_6_HI	='7',	CMD_7_HI	='8',

	// Sets the pin low
	CMD_0_LO	='Q',	CMD_1_LO	='W',
	CMD_2_LO	='E',	CMD_3_LO	='R',
	CMD_4_LO	='T',	CMD_5_LO	='Y',
	CMD_6_LO	='U',	CMD_7_LO	='I',


	// Tells it to report all data human-readable
	CMD_SET_ASCII		=0x60,
	// Tells it to report all data as binary bytes
	// Temperatures get reported as per raw 18B20
	CMD_SET_BINARY		=0x5c,
	// These only matter in ASCII mode
	CMD_SET_FAHRENHEIT	=0x4c,
	CMD_SET_CELSIUS		=0x3b,
	// Asks the USB device to send a REPLY_PING back to us
	CMD_PING		=0x27,
	REPLY_PING		=0x51,
	// If you get this temperature, reread
	TEMP_ERROR		=0xffff,
	TEMP_RESET		=0x0550,
	// 125 degrees C / 0.0625 degrees per bit
	TEMP_MAX		=2000,
	// -55 degrees C / 0.0625 degrees per bit
	TEMP_MIN		=-880
};


class PComPort: public PDevice, virtual public PEBLObjectBase
{
    
public:    

    ///The Standard constructor.  
    PComPort(unsigned int port,unsigned int baud,
             std::string mode);
    
    ///The Standard destructor.  
    virtual ~PComPort();
    
    virtual void Init();
    virtual void SetPort(unsigned int portnum,
                         unsigned int baud);

    virtual void Close();
    
    virtual void PSendByte(unsigned char);
    virtual unsigned char GetByte();
    virtual int GetByte(unsigned char& x);
    virtual Variant GetBytes(int n);
    virtual Variant GetPortName();

protected:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const; 
    Variant mPortName;
    unsigned int mPort;
    unsigned int mBaud;
    std::string mMode;  //mode is the rs232 mode decoded line 139
    bool mIsOpen;

private:


};


#endif
#endif
