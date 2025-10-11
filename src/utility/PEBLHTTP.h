//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       utility/PEBLHTTP.h
//    Purpose:    Class enabling HTTP GET and POST commands.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2013-2025 Shane T. Mueller <smueller@obereed.net>
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
#ifndef __PEBLHTTP_H__
#define __PEBLHTTP_H__


#define PEBL_HAPPY 1
#define PEBL_CURL 2
#define PEBL_FETCH 3

#ifdef 	PEBL_EMSCRIPTEN
#define HTTP_LIB PEBL_FETCH
#else
//#define HTTP_LIB PEBL_HAPPY //simple but not hardy
#define HTTP_LIB PEBL_CURL
#endif


#ifdef PEBL_HTTP

#if HTTP_LIB == PEBL_HAPPY
#include "happyhttp.h"
#elif HTTP_LIB == PEBL_CURL
#include <curl/curl.h>
#endif

#include "../base/Variant.h"
#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <cstring>


// This uses a simple http library called 'happyhttp'
// it is a slim wrapper around that, but we abstract from.
// aLSO , this should enable using PEBLPaths, PErrors, and such.
// the library to maybe someday use curl or the wget commands
// in emscripten.

class PEBLHTTP
{

public:
    PEBLHTTP(Variant host,int port=80);  //with no port, default to 80
    virtual ~PEBLHTTP();

    virtual int GetHTTPFile(Variant fname, Variant savename);
    virtual std::string GetHTTPText(Variant fname);

    //  virtual int PostHTTPold(Variant name, Variant headers,Variant body); //original; for posterity.
    virtual Variant PostHTTP(Variant name, Variant args,
                             Variant body);
    virtual Variant PostMulti(Variant pagename, Variant args,
                               Variant uploadname,Variant name);

    virtual FILE * GetFileObject(){return mFile;};
    virtual std::string  * GetTextObject(){return &mText;};

    virtual void SetByteCount(int n){mByteCount = n;}
    virtual  int GetByteCount(){return mByteCount;};

    virtual void SetStatus(int status){mStatus= status;};
    virtual int GetStatus(){return mStatus;};

    virtual  void SetReason(std::string reason){mReason = reason;};


    FILE*  mFile; //file to stream to.



 private:
    int mStatus;  //http status number
    std::string mReason;

    std::string mText;
    Variant mHost;
    int mPort;


#if HTTP_LIB == PEBL_CURL
    CURL * mCurl;
#endif


    Variant mFileName;
    Variant mSaveName;



    int mByteCount;
};
#endif
#endif
