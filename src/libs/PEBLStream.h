//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       libs/PEBLStream.h
//    Purpose:    Stream-based Function Library for PEBL
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
#ifndef __PEBLSTREAM_H__
#define __PEBLSTREAM_H__


#include "../base/Variant.h"
#include "../base/PComplexData.h"
#include "../base/PList.h"
#include "../base/PEBLObject.h"


namespace PEBLStream
{
    ///This prints the argument with a newline character
    Variant Print(Variant v);

     ///This prints the argument without a linebreak at the end
    Variant Print_(Variant v);

    ///This operates like print but without the printing.
    ///Useless right now, but could be used for formatting output,
    ///changing numbers to text, etc.
    Variant Format(Variant v);

    ///This opens a filestream for reading.
    Variant FileOpenRead(Variant v);

    ///This opens a filestream for writing
    Variant FileOpenWrite(Variant v);

    Variant FileOpenOverwrite(Variant v);

    Variant FileOpenAppend(Variant v);

    ///This Closes a filestream
    Variant FileClose(Variant v);

    ///This prints contents of variant to file, followed by a newline
    Variant FilePrint(Variant v);

    ///This prints contents of variant to file, without appending a newline
    Variant FilePrint_(Variant v);

    ///Reads a character from a file
    Variant FileReadCharacter(Variant v);

    ///Reads everything up until the next space/newline, dropping all
    ///initial spaces/newlines
    Variant FileReadWord(Variant v);

    ///Reads until the next line
    Variant FileReadLine(Variant v);

    ///Places all words in file into a list
    Variant FileReadList(Variant v);

    ///Makes a list of lists, with each row contained in a list.
    Variant FileReadTable(Variant v);

    ///This will read all of the text in a file into a single variant.
    Variant FileReadText(Variant v);

    Variant EndOfLine(Variant v);
    Variant EndOfFile(Variant v);

    //Copies the contents of one file to another.
    Variant AppendFile(Variant v);
#ifdef PEBL_WIN32
#ifdef CopyFile
#undef CopyFile  // Undefine Windows CopyFile macro
#endif
#endif
    Variant CopyFile(Variant v);

    // The following control TCP/IP connections and communications.
    // Now always available (stub implementations when PEBL_NETWORK not defined)
    Variant ConnectToIP(Variant v);
    Variant ConnectToHost(Variant v);


    Variant SetNetworkPort(Variant v);//
    Variant CheckForNetworkConnection(Variant v);//
    Variant OpenNetworkListener(Variant v);

    Variant AcceptNetworkConnection(Variant v);
    Variant WaitForNetworkConnection(Variant v);
    Variant CloseNetworkConnection(Variant v);

    Variant SendData(Variant v);
    Variant GetData(Variant v);

    Variant GetIPAddress(Variant v);
    Variant GetMyIPAddress(Variant v);

#ifdef PEBL_HTTP
    Variant GetHTTPFile(Variant v);
    Variant GetHTTPText(Variant v);

    Variant PostHTTP(Variant v);
    Variant PostHTTPFile(Variant v);
#endif
    Variant MD5Sum(Variant v);
    Variant MD5File(Variant v);

    Variant WritePNG(Variant v);
    Variant ParseJSON(Variant v);


#ifdef PEBL_USEPORTS
    //These functions open, send, receive, and close
    //a parallel port object.
    Variant OpenPPort(Variant v);
    Variant SetPPortState(Variant v);
    Variant GetPPortState(Variant v);
    Variant SetPPortMode(Variant v);





    //These functions open, send, receive, and close
    //a serial (com) port; also usb-mounted comport devices
    //ala ftdi chips and the dlp-io8 and relatives.

    Variant OpenComPort(Variant v);
    Variant ComPortSendByte(Variant v);
    Variant ComPortGetByte(Variant v);
    Variant ComPortGetBytes(Variant v);



#endif


}
#endif
