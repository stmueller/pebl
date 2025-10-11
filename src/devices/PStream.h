//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PStream.h
//    Purpose:    Primary File Stream Interface Device
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
#ifndef __PSTREAM_H__
#define __PSTREAM_H__

#include "PDevice.h"
#include "../base/PEBLObject.h"
#include <fstream>
#include <string>

//Enumeration used to specify whether this is a write or read stream
enum StreamDirection
    {
        sdRead,
        sdWrite,
        sdAppend
    };

//Enumeration used to specify text or binary access

enum StreamType
    {
        stASCII,
        stBinary
    };



///  This is the base File Stream class.
///  It is used to read and write files.
///  It is also a subclass of PDevice, which means that it can be added to the
///  event loop for interesting between-process communications, and probably even used as
///  a basis for other input devices.

class PStream: public PDevice, virtual public PEBLObjectBase
{

public:


    ///The Standard constructor.
    PStream(const std::string & filename, StreamDirection dir, StreamType type);
    PStream (const PStream & ps);

    ///The Standard destructor.
    virtual ~PStream();


    void Open(const std::string & filename, StreamDirection dir, StreamType type);
    void WriteChar(const char character);
    void WriteString(const std::string & buffer);
    void WriteBuffer(const std::string & buffer, unsigned int length);

    char  ReadChar();
    std::string ReadToken(const char separator);
    std::string ReadLine();
    std::string ReadLineClean();

    bool Eof();
    bool Eol();
    bool Close();
    virtual PEBL_DEVICE_TYPE GetDeviceType(){return PDT_STREAM;};



    //overloaded generic PEBLObjectBase methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;


protected:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const;

private:
    std::string  mStreamFileName;
    std::fstream  * mFileStream;
    StreamDirection mStreamDirection;
    StreamType mStreamType;

};



#endif
