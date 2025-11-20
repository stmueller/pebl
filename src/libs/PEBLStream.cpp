//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
///////////////////////////////////////////////////////////////////////////
//    Name:       libs/PEBLStream.cpp
//    Purpose:    Built-in stream functions for PEBL
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
//    Foundation, Inc., 59 Temple Place, Suite 330,
//    Boston, MA  02111-1307  USA
///////////////////////////////////////////////////////////////////////////

#ifdef PEBL_WIN32
// Prevent Windows from defining macros that conflict with our methods
#define WIN32_LEAN_AND_MEAN  // Reduces windows.h bloat
#define NOGDI                 // Prevents GetObject and other GDI macros
#define NOSERVICE            // Prevents service-related macros
#define NOMCX                 // Prevents MCX macros
#define NOIME                 // Prevents IME macros
// CopyFile is not preventable by NO* macros, so we'll undef it later
// Include winsock2.h first to avoid conflicts with windows.h
#include <winsock2.h>
#endif

#include "PEBLStream.h"

#ifdef PEBL_EMSCRIPTEN
#include "../base/Evaluator-es.h"
#else
#include "../base/Evaluator.h"
#endif

#include "../devices/PStream.h"
#include "../utility/PError.h"
#include "../utility/PEBLPath.h"
#include "../utility/PEBLUtility.h"

#ifdef PEBL_HTTP
#include "../utility/PEBLHTTP.h"
#include "../platforms/sdl/PlatformNetwork.h"

#endif


#include "../devices/PParallelPort.h"
#include "../platforms/sdl/SDLUtility.h"



#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

//This is for pport stuff used for testing.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(PEBL_LINUX)
#include <sys/io.h>
#endif



#define BASE 0x378

const static int BSIZE =4096;

#ifdef PEBL_WIN32
// Undefine Windows API macros that conflict with our method names
// Must be done after all headers are included
#ifdef GetObject
#undef GetObject
#endif
#ifdef SetPort
#undef SetPort
#endif
#ifdef CopyFile
#undef CopyFile
#endif
#endif

using std::cout;
using std::flush;
using std::cerr;
using std::endl;
using std::ios_base;


Variant PEBLStream::Print(Variant v)
{

    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    //No type assertion is needed, because everything should work.
    cout << v1 << endl;

    return v1;
}


Variant PEBLStream::Print_(Variant v)
{

    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    //No type assertion is needed, because everything should work.
    cout << v1 << flush;

    return v1;
}

Variant PEBLStream::Format(Variant v)
{

    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    PError::SignalFatalError("Function [Format()] not implemented.");
    return v1;
}


///This opens a filestream for reading.
Variant PEBLStream::FileOpenRead(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in function FileOpenRead(<string>)]: ");

    ///v1 contains the name of a file to open.
    counted_ptr<PEBLObjectBase> mystream = counted_ptr<PEBLObjectBase>(new PStream(v1,sdRead, stASCII));
    mystream->SetProperty("FILENAME",v1);
    mystream->SetProperty("DIRECTION","READ");
    PComplexData * pcd = new PComplexData(mystream);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
}

///This opens a filestream for writing, replacing current file
Variant PEBLStream::FileOpenWrite(Variant v)
{


    // should be able to create the directory if it doesn't exist.
    //    char sysText[256];
    //    sprintf( systext, "mkdir -p %s", path ); // path is a char * to the string representing the directory you want to create
    //    system( systext );


    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in function FileOpenWrite(<string>)]: ");

    Variant out = PEBLUtility::FileExists(v1);
    Variant v11=Variant(v1);
    if(out)
        {
            // The file exists, so let's do some text munching and
            // create a new filename to use.
            std::string fname =  (v1);
            size_t i = fname.rfind(".");
            std::string base, ext;
            if(i != 0)
                {
                    base = fname.substr(0,i);
                    ext = fname.substr(i+1,fname.length());
                }
            else
                {
                    //No . found
                    base = fname;
                    ext = "";
                }
            //See if the last character of base is numeric
            int version =  base[base.length()];
            if(version>0)
                {
                    base = base.substr(0,base.length()-1);
                }

            v11= Variant(base) + Variant(version) + Variant(".")+Variant(ext);
            while( PEBLUtility::FileExists(v11))
                {

                    v11 = Variant(base) + Variant(version++) + Variant(".")+Variant(ext);

                     }

           PError::SignalWarning(Variant("File [") + v1 + Variant("] already exists.  Using [")+ v11 + Variant("] instead"));
        }
    ///v1 contains the name of a file to open.
    counted_ptr<PEBLObjectBase> mystream = counted_ptr<PEBLObjectBase>(new PStream(v11,sdWrite, stASCII));
    mystream->SetProperty("FILENAME",v11);
    mystream->SetProperty("DIRECTION","WRITE");
    PComplexData * pcd = new PComplexData(mystream);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;

}




///This opens a filestream for writing, replacing current file, without making a backup
Variant PEBLStream::FileOpenOverwrite(Variant v)
{


    // should be able to create the directory if it doesn't exist.
    //    char sysText[256];
    //    sprintf( systext, "mkdir -p %s", path ); // path is a char * to the string representing the directory you want to create
    //    system( systext );


    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in function FileOpenOverWrite(<string>)]: ");

    ///v1 contains the name of a file to open.
    counted_ptr<PEBLObjectBase> mystream = counted_ptr<PEBLObjectBase>(new PStream(v1,sdWrite, stASCII));
    mystream->SetProperty("FILENAME",v1);
    mystream->SetProperty("DIRECTION","WRITE");

    PComplexData * pcd = new PComplexData(mystream);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;

}


///This opens a filestream for writing, appending to end of file.
Variant PEBLStream::FileOpenAppend(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in function FileOpenAppend(<string>)]: ");


    ///v1 contains the name of a file to open.
    counted_ptr<PEBLObjectBase> mystream = counted_ptr<PEBLObjectBase>(new PStream(v1,sdAppend, stASCII));
    mystream->SetProperty("FILENAME",v1);
    mystream->SetProperty("DIRECTION","WRITE");

    PComplexData * pcd = new PComplexData(mystream);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;


}

///This is a filestream
Variant PEBLStream::FileClose(Variant v)
{
    //v[1] should have the file stream to close
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_FILESTREAM, "Argument error in function [FileClose(<file-stream>)]: ");


    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PStream * mystream = dynamic_cast<PStream*>(tmp2.get());
    return Variant(mystream->Close());
}


Variant PEBLStream::FilePrint(Variant v)
{
    //v[1] should have the file stream
    //v[2] should have the variant to print
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();


    PError::AssertType(v1, PEAT_FILESTREAM, "Argument error in first parameter of function [FilePrint(<file-stream>, <text>)]: ");

    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PStream * mystream = dynamic_cast<PStream*>(tmp2.get());


    Variant v2 = plist->Nth(2); //plist->PopFront();
    std::string s = v2.GetString();

    mystream->WriteString(s + "\n");
    return (v2+Variant("\n"));
}

Variant PEBLStream::FilePrint_(Variant v)
{
    //v[1] should have the file stream
    //v[2] should have the variant to print
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();

    PError::AssertType(v1, PEAT_FILESTREAM, "Argument error in first parameter of function [FilePrint_(<file-stream>, <text>)]: ");

    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PStream * mystream = dynamic_cast<PStream*>(tmp2.get());

    Variant v2 = plist->Nth(2); //plist->PopFront();
    mystream->WriteString(v2);
    return v2;
}

Variant PEBLStream::FileReadCharacter(Variant v)
{
    //v[1] should have the file stream
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();

    PError::AssertType(v1, PEAT_FILESTREAM, "Argument error in function [FileReadCharacter(<file-stream>)]: ");
    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();

    PStream * mystream = dynamic_cast<PStream*>(tmp2.get());
    char c = (mystream->ReadChar());
    return Variant((char)c);
}

Variant PEBLStream::FileReadWord(Variant v)
{
    //v[1] should have the file stream
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();

    PError::AssertType(v1, PEAT_FILESTREAM, "Argument error in function [FileReadWord(<file-stream>)]: ");

    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PStream * mystream = dynamic_cast<PStream*>(tmp2.get());

    return Variant(mystream->ReadToken(' '));
}


//This takes a filestream and returns the next line.
Variant PEBLStream::FileReadLine(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    //v[1] should have the file stream
    Variant v1 = plist->First();// plist->PopFront();

    PError::AssertType(v1, PEAT_FILESTREAM, "Argument error in function [FileReadLine(<file-stream>)]: ");
    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PStream * mystream = dynamic_cast<PStream*>(tmp2.get());

    return Variant(mystream->ReadLineClean());
}




///This function takes a filename as an argument.  It
///Opens the file, and reads in each line of the file into a string,
///which it places in a list that it returns.  It closes the file afterward.
Variant PEBLStream::FileReadList(Variant v)
{

   //v[1] should have the filename
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in function [FileReadList(<filename>)]:  ");

    //Search through the paths for the file, because it may be a 'special' file.
    string filename = Evaluator::gPath.FindFile(v1.GetString());

    if(filename == "")
        PError::SignalFatalError(string("Unable to find file [")  + v1.GetString() + string("]."));

    //It must be good, so open it.
    PStream *  mystream = new PStream(filename,sdRead, stASCII);



    PList * returnlist = new PList();
    std::string  tmpstring;
    while(!mystream->Eof())
        {
            tmpstring = mystream->ReadLineClean();

            if(strcmp("",tmpstring.c_str()) &&       //Ignore blank lines
               strncmp("#",tmpstring.c_str(),1)      //Ignore lines starting with #
               )
                {
                    returnlist->PushBack(tmpstring);
                }
        }
    mystream->Close();
    delete mystream;

    counted_ptr<PEBLObjectBase>  tmp2 = counted_ptr<PEBLObjectBase>(returnlist);
    PComplexData * pcd = new PComplexData(tmp2);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp3;
}


///Give this function a filename, and it will read it into a list
///of lists; each line is a list, and the (optional) token-separated elements
///are the items of the list.
Variant PEBLStream::FileReadTable(Variant v)
{
    //v[1] should have the file stream
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in first parameter of function [FileReadTable(<list>,opt:<sep>)]:  ");

    //Search through the paths for the file, because it may be a 'special' file.
    string filename = Evaluator::gPath.FindFile(v1.GetString());

    if(filename == "")
        PError::SignalFatalError(string("Unable to find file [")  +v1.GetString() + string("]."));

    //It must be good, so open it.
    PStream *   myStream = new PStream(filename,sdRead, stASCII);

    char separator;
    //See if there is another parameter; if there is, it is the token separator.
    if(plist->Length()>1)
        {
            PError::AssertType(plist->Nth(2), PEAT_STRING, "Argument error in second parameter of function [FileReadTable(<list>,opt:<sep>)]:  ");
            std::string tmp = plist->Nth(2);
            separator = tmp[0];

        }
    else
        {
            separator = ' ';
        }

    //Make an outer list to put everything in.

    PList * returnlist = new PList();

    //Make an inner list
    Variant innerlist;

    std::string tmpstring;

    while(!myStream->Eof())
        {

            tmpstring = myStream->ReadLineClean();
            if((strcmp("",tmpstring.c_str()) == 0) ||      //Ignore blank lines
               (strncmp("#",tmpstring.c_str(),1)==0)      //Ignore lines starting with #
               )
                {

                    //This line is garbage, so read it and throw it away.

                }
            else
                {
                    innerlist =  PEBLUtility::Tokenize(tmpstring.c_str(),separator);
                    returnlist->PushBack(innerlist);
                }
        }
    myStream->Close();
    delete myStream;
    counted_ptr<PEBLObjectBase> tmp2 =  counted_ptr<PEBLObjectBase>(returnlist);
    PComplexData * pcd = new PComplexData(tmp2);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp3;

}


///This function takes a filename as an argument.  It
///Opens the file, and reads in the entire file as a string,
///which it  returns, and closes the file afterward. It ignores any
///lines that begin with #, and converts '\n' to [10], which
///is later parsed as a carriage return.
Variant PEBLStream::FileReadText(Variant v)
{

   //v[1] should have the filename
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in function [FileReadText(<filename>)]:  ");

    //Search through the paths for the file, because it may be a 'special' file.
    string filename = Evaluator::gPath.FindFile(v1.GetString());

    if(filename == "")
        PError::SignalFatalError(string("Unable to find file [")  + v1.GetString() + string("]."));

    //It must be good, so open it.
    PStream * mystream = new PStream(filename,sdRead, stASCII);


    Variant returnText = "";
    std::string tmpstring;
    while(!mystream->Eof())
        {
            tmpstring = mystream->ReadLine();

            if( strncmp("#",tmpstring.c_str(),1)      //Ignore lines starting with #
               )
                {
                    //This is wrong--it will add extra space where it is not needed.
                    returnText = returnText +  Variant(tmpstring);
                }
        }
    mystream->Close();
    delete mystream;

    return returnText;
}



///This detects if you are at the end of a line
Variant PEBLStream::EndOfLine(Variant v)
{
   //v[1] should have the file stream
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();

    PError::AssertType(v1, PEAT_FILESTREAM, "Argument error in function [EndOfLine(<file-stream>)]: ");
    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PStream * mystream = dynamic_cast<PStream*>(tmp2.get());
    return Variant(mystream->Eol());
}


///This detects if you are at the end of a file
Variant PEBLStream::EndOfFile(Variant v)
{
   //v[1] should have the file stream
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();

    PError::AssertType(v1, PEAT_FILESTREAM, "Argument error in function [EndOfFile(<file-stream>)]: ");
    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PStream * mystream = dynamic_cast<PStream*>(tmp2.get());
    return Variant(mystream->Eof());
}


//Copies the contents of one file to another.
Variant PEBLStream::AppendFile(Variant v)
{
    //v[1] should have the filename
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in function [AppendFile(<file1>,<file2>)]:  ");

    string filename1 = v1.GetString();


    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_STRING, "Argument error in function [AppendFile(<file1>,<file2>)]:  ");

    //Search through the paths for the file, because it may be a 'special' file.
    string filename2 = Evaluator::gPath.FindFile(v2.GetString());

    if(filename2 == "")
        PError::SignalFatalError(string("Unable to find file [")  + v2.GetString() + string("] in AppendFile"));

    //open files in binary mode so we can do a direct copy.

    std::ifstream in(filename2.c_str(), ios_base::in|ios_base::binary);
    std::ofstream out(filename1.c_str(), ios_base::out|ios_base::app|ios_base::binary);


    if(!in.is_open())
        {
            PError::SignalWarning("Unable to open file ["+filename2+"]. No data copied.");
        }
    if(!out.is_open())
        {
            PError::SignalFatalError("Unable to open file ["+filename1+"]. Check permissions or determine whether directory exists.");
        }


    char buf[BSIZE];
    do {
        in.read(&buf[0],BSIZE);
        out.write(&buf[0],in.gcount());
    }while (in.gcount()>0);

    in.close();
    out.close();
    return Variant(1);
}


//Copies the contents of one file to another.
//This makes the copy byte-by-byte.  It is probably better to use a systemcall function
//to make a copy of an entire file at once. This is likely to be slower and possibly error-prone
//(i.e., permissions and other file properties may not copy.)
Variant PEBLStream::CopyFile(Variant v)
{
    //v[1] should have the filename
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in function [CopyFile(<sourcefile>,<destfile>)]:  ");


    string srcfile = Evaluator::gPath.FindFile(v1.GetString());
    if(srcfile == "")
        PError::SignalFatalError(string("Unable to find file [")  + v1.GetString() + string("] in CopyFile"));



    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_STRING, "Argument error in function [CopyFile(<sourcefile>,<destfile>)]:  ");


    string destfile = v1.GetString();


    //open files in binary mode so we can do a direct copy.

    std::ifstream in(srcfile.c_str(), ios_base::in|ios_base::binary);
    std::ofstream out(destfile.c_str(), ios_base::out|ios_base::binary);


    if(!in.is_open())
        {
            PError::SignalWarning("Unable to open file ["+srcfile+"]. File not copied.");
        }
    if(!out.is_open())
        {
            PError::SignalFatalError("Unable to open file ["+destfile+"]. Check permissions or determine whether directory exists.");
        }


    char buf[BSIZE];
    do {
        in.read(&buf[0],BSIZE);
        out.write(&buf[0],in.gcount());
    }while (in.gcount()>0);

    in.close();
    out.close();
    return Variant(1);
}



#ifdef PEBL_NETWORK

Variant PEBLStream::ConnectToIP(Variant v)
{
    //v[1] should have the IP Number as an integer
    //v[2] should have the port number

    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_INTEGER, "Argument error in first parameter of function [ConnectToHost(<hostname>,<port>)]: ");

    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_INTEGER, "Argument error in second parameter of function [ConnectToHost(<hostname>,<port>)]: ");


    counted_ptr<PEBLObjectBase> tmp2 = counted_ptr<PEBLObjectBase>(new PlatformNetwork());
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());

    mynet->Init();
    mynet->SetHostIP((unsigned int)(long unsigned int)v1);
    mynet->SetPort((int)v2);
    mynet->Open();

    PComplexData * pcd = new PComplexData(tmp2);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp3;
}

Variant PEBLStream::ConnectToHost(Variant v)
{

    //v[1] should have the host name
    //v[2] should have the port number
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in first parameter of function [ConnectToHost(<hostname>,<port>)]: ");

    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_INTEGER, "Argument error in second parameter of function [ConnectToHost(<hostname>,<port>)]: ");


    counted_ptr<PEBLObjectBase> tmp2 = counted_ptr<PEBLObjectBase>(new PlatformNetwork());
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());

    mynet->Init();
    mynet->SetHostName(v1);
    mynet->SetPort((int)v2);


    mynet->Open();

    if(mynet->IsOpen())
        {


            PComplexData * pcd = new PComplexData(tmp2);
            Variant tmp3 = Variant(pcd);
            delete pcd;
            pcd=NULL;
            return tmp3;

        }else{

        return Variant(0);
    }


}





Variant PEBLStream::SetNetworkPort(Variant v)
{
    //v[1] should have the port number
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();

    PError::AssertType(v1, PEAT_INTEGER, "Argument error in first parameter of function [SetNetworkPort(<port>)]: ");


    counted_ptr<PEBLObjectBase> tmp2 = counted_ptr<PEBLObjectBase>(new PlatformNetwork());
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());
    mynet->Init();
    mynet->SetHostName("");
    mynet->SetPort((int)v1);
    //    mynet->Accept();

    PComplexData * pcd = new PComplexData(tmp2);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp3;
}


// Once a listener has been opened, this will
//see if anyone is knocking on the door.
//

Variant PEBLStream::CheckForNetworkConnection(Variant v)
{
    //v[1] should have the network object which should have been opened but not yet
    //accepted a connection
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_NETWORKCONNECTION, "Argument error in first parameter of function [CheckFroNetworkConnection(<network>)]: ");
    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());


    bool result ;    //

    //This should already be initialized with a port and hostname.
    //mynet->Init();
    //mynet->SetHostName("");
    //mynet->SetPort((int)v1);

    result = mynet->CheckForConnection();

    //Accept the connection if we have one:
    if(result)
        mynet->Accept(1000);

    //PComplexData * pcd = new PComplexData(tmp2);
    //    Variant tmp3 = Variant(pcd);
    //    delete pcd;
    //    pcd=NULL;
    //    return tmp3;
    return Variant(result);
}



Variant PEBLStream::OpenNetworkListener(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();

    PError::AssertType(v1, PEAT_INTEGER, "Argument error in first parameter of function [OpenNetworkListener(<port>)]: ");


    counted_ptr<PEBLObjectBase> tmp2 = counted_ptr<PEBLObjectBase>(new PlatformNetwork());
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());
    mynet->Init();
    mynet->SetHostName("");
    mynet->SetPort((int)v1);
    mynet->CreateListener();

    PComplexData * pcd = new PComplexData(tmp2);
    Variant tmp3 = Variant(pcd);
    //delete pcd;
    //pcd=NULL;
    return tmp3;
}



Variant PEBLStream::AcceptNetworkConnection(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_NETWORKCONNECTION, "Argument error in function [AcceptNetworkConnection(<network>,<timeout>)]: ");

    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());

    Variant timeout = plist->Nth(2); //plist->PopFront();

    PError::AssertType(timeout, PEAT_INTEGER, "Argument error in second argument of function [AcceptNetworkConnection(<port>,<timeout>)]: ");

    return Variant( mynet->Accept(timeout));

}




Variant PEBLStream::WaitForNetworkConnection(Variant v)
{

    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();

    PError::AssertType(v1, PEAT_INTEGER, "Argument error in first parameter of function [WaitForNetworkConnection(<port>)]: ");


    counted_ptr<PEBLObjectBase> tmp2 = counted_ptr<PEBLObjectBase>(new PlatformNetwork());
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());
    mynet->Init();
    mynet->SetHostName("");
    mynet->SetPort((int)v1);

    //The following waits forever, until a network connection
    //opens.  If you want more control, use Accept()
    mynet->Accept();

    PComplexData * pcd = new PComplexData(tmp2);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp3;

}

Variant PEBLStream::CloseNetworkConnection(Variant v)
{
    //v[1] should have the network connection
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_NETWORKCONNECTION, "Argument error in function [CloseNetworkConnection(<network>)]: ");

    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());
    mynet->Close();

    return Variant(1);

}

Variant PEBLStream::SendData(Variant v)
{
    //v[1] should have the Network connection object
    //v[2] should have the data
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_NETWORKCONNECTION, "Argument error in first parameter of function [SendData(<network>,<data>)]: ");
    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());

    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_STRING, "Argument error in second parameter of function [SendData(<network>,<data>)]: ");

    //int tries = 0;  //Try to send up to 10 times
    bool success = mynet->SendString(v2);

    //    struct timespec a,b;
    //    a.tv_sec  = 0;
    //    a.tv_nsec = 10000000; //100 ms

    //    while(!success | ++tries<10)
    //        {
    //            success = mynet->SendString(v2);
    //            nanosleep(&a,&b);
    //        }

    return Variant(success);
}

Variant PEBLStream::GetData(Variant v)
{
    //v[1] should have the Network connection object
    //v[2] should have the max length of the data we are looking for.

    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_NETWORKCONNECTION, "Argument error in first parameter of function [GetData(<network>,<size>)]: ");
    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());

    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_INTEGER, "Argument error in second parameter of function [GetData(<network>,<size>)]: ");

    //cout << "Trying to receive " << v2 << " bytes\n";
    Variant ret = mynet->Receive(v2);
    return ret;

}

Variant PEBLStream::GetIPAddress(Variant v)
{

    //v[1] should have the network connection
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_NETWORKCONNECTION, "Argument error in function [GetIPAddress(<network>)]: ");

    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());
    Variant add = mynet->GetIPAddress();

    return add;
}


Variant PEBLStream::GetMyIPAddress(Variant v)
{

    //v[1] should have the network connection
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_NETWORKCONNECTION, "Argument error in function [GetIPAddress(<network>)]: ");

    counted_ptr<PEBLObjectBase> tmp2 = (v1.GetComplexData())->GetObject();
    PlatformNetwork * mynet = dynamic_cast<PlatformNetwork*>(tmp2.get());
    Variant add = mynet->GetMyIPAddress();

    return add;
}

#else
// Stub implementations when PEBL_NETWORK is not defined
// These allow scripts to call network functions but they return appropriate "not available" values

Variant PEBLStream::ConnectToIP(Variant v)
{
    PError::SignalWarning("Network support not compiled in. ConnectToIP returning 0 (failed).");
    return Variant(0);
}

Variant PEBLStream::ConnectToHost(Variant v)
{
    PError::SignalWarning("Network support not compiled in. ConnectToHost returning 0 (failed).");
    return Variant(0);
}

Variant PEBLStream::SetNetworkPort(Variant v)
{
    PError::SignalWarning("Network support not compiled in. SetNetworkPort has no effect.");
    return Variant(0);
}

Variant PEBLStream::CheckForNetworkConnection(Variant v)
{
    return Variant(0);
}

Variant PEBLStream::OpenNetworkListener(Variant v)
{
    PError::SignalWarning("Network support not compiled in. OpenNetworkListener returning 0 (failed).");
    return Variant(0);
}

Variant PEBLStream::AcceptNetworkConnection(Variant v)
{
    return Variant(0);
}

Variant PEBLStream::WaitForNetworkConnection(Variant v)
{
    PError::SignalWarning("Network support not compiled in. WaitForNetworkConnection returning 0 (failed).");
    return Variant(0);
}

Variant PEBLStream::CloseNetworkConnection(Variant v)
{
    // Silently succeed - nothing to close
    return Variant(1);
}

Variant PEBLStream::SendData(Variant v)
{
    return Variant(0);
}

Variant PEBLStream::GetData(Variant v)
{
    return Variant("");
}

Variant PEBLStream::GetIPAddress(Variant v)
{
    return Variant("0.0.0.0");
}

Variant PEBLStream::GetMyIPAddress(Variant v)
{
    return Variant("0.0.0.0");
}

#endif


#ifdef PEBL_HTTP

//This gets text via http.
Variant PEBLStream::GetHTTPFile(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    PError::AssertType(v1, PEAT_STRING, "Error in first argument of function [GetHTTPFile(<host>,<port>,<fileurl>,<savename>)]: ");

    Variant port = plist->Nth(2);
    PError::AssertType(port, PEAT_INTEGER, "Error in second argument of function [GetHTTPFile(<host>,<port>,<fileurl>,<savename>)]: ");


    Variant v3 = plist->Nth(3);
    PError::AssertType(v3, PEAT_STRING, "Error in third argument of function [GetHTTPFile(<host>,<port>,<fileurl>,<savename>)]: ");

    Variant v4 = plist->Nth(4);
    PError::AssertType(v4, PEAT_STRING, "Error in fourth argument function [GetHTTPFile(<host>,<port>,<fileurl>,<savename>)]: ");

    PEBLHTTP http(v1,port);

    int out =  http.GetHTTPFile(v3,v4);
    unsigned int status = http.GetStatus();

    return Variant((int)status);
}


//This gets text via http.
Variant PEBLStream::GetHTTPText(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    PError::AssertType(v1, PEAT_STRING, "Error in first argument of function [GetHTTPText(<host>,<port>,<fileurl>)]: ");

    Variant port = plist->Nth(2);
    PError::AssertType(port, PEAT_INTEGER, "Error in second argument of function [GetHTTPText(<host>,<port>,<fileurl>)]: ");

    Variant v3 = plist->Nth(3);
    PError::AssertType(v3, PEAT_STRING, "Error in third argument of function [GetHTTPText(<host>,<port>,<fileurl>)]: ");

    PEBLHTTP http(v1,port);

    std::string out =  http.GetHTTPText(v3);
    unsigned int status = http.GetStatus();


    //return a pair; the status code  + the text


    PList * returnlist = new PList();
    returnlist->PushBack(Variant((int)status));
    returnlist->PushBack(Variant(out));
    counted_ptr<PEBLObjectBase>  tmp2 = counted_ptr<PEBLObjectBase>(returnlist);
    PComplexData * pcd = new PComplexData(tmp2);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp3;
}



Variant PEBLStream::PostHTTP(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Error in first argument of function [PostHTTP(<host>,<port>,<page>,[<list>,<of>,<headers>],<body-args>)]: ");

    Variant port = plist->Nth(2);
    PError::AssertType(port, PEAT_INTEGER, "Error in second argument of function [PostHTTP(<host>,<port>,<page>,[<list>,<of>,<headers>],<body-args>)]: ");

    Variant v3 = plist->Nth(3);
    PError::AssertType(v3, PEAT_STRING, "Error in third argument of function [PostHTTP(<host>,<port>,<page>,[<list>,<of>,<headers>],<body-args>)]: ");


    Variant v4 = plist->Nth(4);
    PError::AssertType(v4, PEAT_LIST, "Error in fourth argument of function [PostHTTP(<host>,<port>,<page>,[<list>,<of>,<headers>],<body-args>)]: ");

    Variant v5 = plist->Nth(5);
    PError::AssertType(v5, PEAT_STRING, "Error in fifth argument of function [PostHTTP(<host>,<port>,<page>,[<list>,<of>,<headers>],<body-args>)]: ");



    PEBLHTTP http(v1,port);



    Variant out =  http.PostHTTP(v3,v4,v5);
    return out;
}



Variant PEBLStream::PostHTTPFile(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Error in first argument of function [PostHTTPFile(<host>,<port>,<page>,<list-of-args>,<filename>,<field>)]: ");

    Variant port = plist->Nth(2);
    PError::AssertType(port, PEAT_INTEGER, "Error in second argument of function [PostHTTPFile(<host>,<port>,<page>,<list-of-args>,<filename>,<field>)]: ");

    Variant page = plist->Nth(3);
    PError::AssertType(page, PEAT_STRING, "Error in third argument of function [PostHTTPFile(<host>,<port>,<page>,<list-of-args>,<filename>,<field>)]: ");


    Variant args = plist->Nth(4);
    PError::AssertType(args, PEAT_LIST, "Error in fourth argument of function [PostHTTPFile(<host>,<port>,<page>,<list-of-args>,<filename>,<field>)]: ");


    Variant fname = plist->Nth(5);
    PError::AssertType(fname, PEAT_STRING, "Error in Fifth argument of function [PostHTTPFile(<host>,<port>,<page>,<list_of_args>,<filename>,<field>]: ");


        Variant field = plist->Nth(6);
    PError::AssertType(field, PEAT_STRING, "Error in Fifth argument of function [PostHTTPFile(<host>,<port>,<page>,<list-of-args>,<filename>,<field>]: ");


    PEBLHTTP http(v1,port);


    Variant out =  http.PostMulti(page,args,fname,field);


    return out;
}

#endif


Variant PEBLStream::MD5Sum(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in first parameter of function [MD5Sum(<text>)]:");
    std::string sum = PEBLUtility::MD5String(v1);
    return Variant(sum);
}


Variant PEBLStream::MD5File(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in first parameter of function [MD5File(<fname>)]:");
    std::string sum = PEBLUtility::MD5File(v1);
    return Variant(sum);
}


Variant PEBLStream::WritePNG(Variant v)
{
    //v[1] should have an object/window name.
    //v[2] should have a filename

    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in first parameter of function [WritePNG(<filename>,<object>,<range>)]: ");


    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_WINDOW, "Argument error in second parameter of function [WritePNG(<filename>,<object>,<range>)]: ");
    counted_ptr<PEBLObjectBase> tmp2 = (v2.GetComplexData())->GetObject();


    Variant v3 = plist->Nth(3);


    PError::AssertType(v3, PEAT_LIST, "Argument error in third parameter of function [WritePNG(<filename>,<object>,<range>)]: ");
    // counted_ptr<PEBLObjectBase> tmp3 = (range.GetComplexData())->GetObject();

    PList * range = (PList*)(v3.GetComplexData()->GetObject().get());

    

    PlatformWindow * myWin = dynamic_cast<PlatformWindow*>(tmp2.get());

    //range must have four numeric values
    //start off with entire window.
    int x=0;
    int y=0;
    int w = myWin->GetWidth();
    int h = myWin->GetHeight();

    //if third argument is properly formed, select the subrange
    long int length = range->Length();
    if(length == 4)
        {
            int newx=range->Nth(1);
            int newy=range->Nth(2);
            int neww=range->Nth(3);
            int newh=range->Nth(4);

            if(newx<0 || newx > myWin->GetWidth() ||
               newy < 0 || newy > myWin->GetHeight() ||
               neww < 0 || (neww + newx ) > myWin->GetWidth() ||
               newh < 0 || (newh + newy) > myWin->GetHeight())
                {
                    PError::SignalWarning("Screenshot bounds outside range of screen. Using entire screen");
                } else
                {
                    x = newx;
                    y = newy;
                    h = newh;
                    w = neww;
                        
                }
   

        }

    
    

    int result = myWin->SaveScreenShot(x,y,w,h,v1);


    if(result != 0)
        {
            PError::SignalWarning(Variant("Failed to write png file ") + v2);
        }

    return Variant(result);
}




Variant PEBLStream::ParseJSON(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); 
    PError::AssertType(v1, PEAT_STRING, "Argument error in first parameter of function [ParseJSON(<json-text>)]: ");

    Variant out = PEBLUtility::ParseJSON(v1);
    return out;
}



#ifdef PEBL_USEPORTS
Variant PEBLStream::OpenPPort(Variant v)
{
    //v[1] should have a text string indicating
    //lpt1/lpt2/lpt3 (which may not really map onto
    //those ports for any individual system.


    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in first parameter of function [OpenPPort(<port>)]: ");

    unsigned char data;       /* value of the data register */
    unsigned char control;    /* value of the control register */

#if 0
    if(0)
        {
            //hardcode some parport stuff here for testing.
            if (iopl(3))
                printf("Could't get the port addresses\n");



            /* enable data register output by bringing bit 5 of control */
            /* register low                                             */

            control = inb(BASE + 2);
            //cout << "control byte:" << (int)control << endl;
            outb(control & ~0x20, BASE + 2);

            /* write the value */
            outb((unsigned char) 255, BASE);

            /* Wait a few secs, so we can see it.*/
            struct timespec a,b;
            a.tv_sec  = 5;
            a.tv_nsec = 100000; //1 ms
            int retval = nanosleep(&a,&b);

            /* disable data register output by bringing bit 5 of control */
            /* register high                                             */

            control = inb(BASE + 2);
            //cout << "control byte:" << (int)control << endl;
            outb(control | 0x20, BASE + 2);


            /* read and print the value */
            //printf("Port 0x%x reads 0x%x\n", BASE, inb(BASE));
        }
#endif

    counted_ptr<PEBLObjectBase> tmp2 = counted_ptr<PEBLObjectBase>(new PParallelPort());
    PParallelPort * pport = dynamic_cast<PParallelPort*>(tmp2.get());


    pport->SetPort(v1);
    pport->Init();


    PComplexData * pcd = new PComplexData(tmp2);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp3;

}


//This sets the pport state on the data bits
Variant PEBLStream::SetPPortState(Variant v)
{
   //v[1] should have an pport
   //v[2] should have an integer, interpreted as a  list of bits 0/1

    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_PARALLELPORT, "Argument error in first parameter of function [SetPPortState(<pport>,<integer>)]: ");

    counted_ptr<PEBLObjectBase> tmp1 = (v1.GetComplexData())->GetObject();
    PParallelPort * mypport = dynamic_cast<PParallelPort*>(tmp1.get());


    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_INTEGER, "Argument error in second parameter of function [SetPPortState(<pport>,<integer>)]: ");

    char x = (int)v2;
    mypport->SetDataState(x);


    return Variant(true);
}

Variant ConvertByteToList(int x)
{
    int d = 8;
    //	Variant buffer="";

    PList *list = new PList();
	for (;d>0;d--)
		{
            list->PushBack(Variant((int)(x & 1)));
            //buffer = buffer + Variant("|") + Variant((int)(x & 1));
            x >>= 1;

		}

    //buffer = buffer + Variant("|");

    counted_ptr<PEBLObjectBase> tmplist = counted_ptr<PEBLObjectBase>(list);
    PComplexData * tmpPCD= (new PComplexData(tmplist));
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;

}

Variant PrintByte(int x)
{
    int d = 8;
	Variant buffer="";

	for (;d>0;d--)
		{
            buffer = buffer + Variant("|") + Variant((int)(x & 1));
            x >>= 1;

		}

    buffer = buffer + Variant("|");

  return buffer;
}

// This gets the state of the parallel port.  It may be platform-specific.
//
Variant PEBLStream::GetPPortState(Variant v)
{
    //v[1] should have an pport

    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_PARALLELPORT, "Argument error in first parameter of function [GetPPortState(<pport>]: ");

    counted_ptr<PEBLObjectBase> tmp1 = (v1.GetComplexData())->GetObject();
    PParallelPort * mypport = dynamic_cast<PParallelPort*>(tmp1.get());


    char x1 = mypport->GetDataState();

    Variant bytelist = ConvertByteToList(x1);

    return bytelist;
}



// This sets the state of the parallel port to either input or output.
//
Variant PEBLStream::SetPPortMode(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_PARALLELPORT, "Argument error in first parameter of function [SetPPortMode(<pport>,<mode>)]: ");

    counted_ptr<PEBLObjectBase> tmp1 = (v1.GetComplexData())->GetObject();
    PParallelPort * mypport = dynamic_cast<PParallelPort*>(tmp1.get());


    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_STRING, "Argument error in second parameter of function [SetPPortState(<pport>,<mode>)]: ");

    if(v2 == "<input>")
        mypport->SetInputMode();
    else if(v2 == "<output>")
        mypport->SetOutputMode();
    else
        PError::SignalFatalError(Variant("Unknown mode type [")+v2+
                                 Variant("] in SetPPortState.  Must be <input> or <output>."));

    return Variant(true);
}


#if 0
// This waits for a change in the PPORT state (on the data bits)
//
Variant PEBLStream::WaitForPPortData(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_PARALLELPORT, "Argument error in first parameter of function [WaitForPPortData(<pport>)]: ");

    counted_ptr<PEBLObjectBase> tmp1 = (v1.GetComplexData())->GetObject();
    PParallelPort * mypport = dynamic_cast<PParallelPort*>(tmp1.get());

    //Create a pport test correspending to keydown.
    //1 is the value (down), DT_EQUAL is the test, key is the interface (e.g., the 'A' key)
    PDevice * device = new PParallelPort(mypport);
    ValueState  * state = new ValueState(1, DT_EQUAL, key, device, PDT_KEYBOARD);

    //NULL,NULL will terminate the looping
    string  funcname = "";
    PList* params = NULL;
    Evaluator::mEventLoop.RegisterState(state,funcname, params);
    PEvent returnval = Evaluator::mEventLoop.Loop();

    //Now, clear the event loop tests
    Evaluator::mEventLoop.Clear();

    return Variant(returnval.GetDummyEvent().value);

}


// This waits for a change in the PPORT state (on the status bits)
//
Variant PEBLStream::WaitForPPortStatus(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_PARALLELPORT, "Argument error in first parameter of function [SetPPortMode(<pport>,<mode>)]: ");

    counted_ptr<PEBLObjectBase> tmp1 = (v1.GetComplexData())->GetObject();
    PParallelPort * mypport = dynamic_cast<PParallelPort*>(tmp1.get());


    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_STRING, "Argument error in second parameter of function [SetPPortState(<pport>,<mode>)]: ");

    if(v2 == "<input>")
        mypport->SetInputMode();
    else if(v2 == "<output>")
        mypport->SetOutputMode();
    else
        PError::SignalFatalError(Variant("Unknown mode type [")+v2+
                                 Variant("] in SetPPortState.  Must be <input> or <output>."));

    return Variant(true);
}
#endif


// This sets the state of the parallel port to either input or output.
//
Variant PEBLStream::OpenComPort(Variant v)
{

    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_INTEGER, "Argument error in first parameter of function [OpenComPort(<portnumber>,<baud>,opt:<mode>)]: ");



    Variant v2 = plist->Nth(2); //plist->PopFront();
    PError::AssertType(v1, PEAT_INTEGER, "Argument error in second parameter of function [OpenComPort(<portnumber>),<baud>,opt:<mode>]: ");

    std::string mode = "8N1";
    if(plist->Length()>2)
        {
            PError::AssertType(plist->Nth(2), PEAT_STRING, "Argument error in third parameter of function [OpenComPort(<portnumber>),<baud>,opt:<mode>]: ");

            std::string mode = plist->Nth(3);

        }




    counted_ptr<PEBLObjectBase> tmp2 = counted_ptr<PEBLObjectBase>(new PComPort((int)v1, (int)v2,mode));
    PComPort * cport = dynamic_cast<PComPort*>(tmp2.get());

    cport->Init();


    PComplexData * pcd = new PComplexData(tmp2);
    Variant tmp3 = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp3;
}



//
//
Variant PEBLStream::ComPortGetByte(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_COMPORT, "Argument error in first parameter of function [ComPortGetByte(<port>)]: ");

    counted_ptr<PEBLObjectBase> tmp1 = (v1.GetComplexData())->GetObject();
    PComPort * cport = dynamic_cast<PComPort*>(tmp1.get());

    unsigned char out;
    int retval= cport->GetByte(out);
    if(retval==0)
        return Variant(-1);
    else
        return Variant((int)out);
}

//
//
Variant PEBLStream::ComPortSendByte(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_COMPORT, "Argument error in first parameter of function [ComPortSendByte(<port>,<int>)]: ");

    Variant v2 = plist->Nth(2);
    //Second argumentcould be a bypte or a character.
    //    PError::AssertType(v2,PEAT_INTEGER,"Argument error in second parameter of function [ComPortSendByte(<port>,<int>)]: ");


    counted_ptr<PEBLObjectBase> tmp1 = (v1.GetComplexData())->GetObject();
    PComPort * cport = dynamic_cast<PComPort*>(tmp1.get());

    unsigned char send = 0;
    if(v2.IsInteger())
        {
            send = v2.GetInteger();
        } else if(v2.IsString())
        {

            std::string tmp = (v2.GetString());
            send = tmp[0];
        }

    cport->PSendByte(send);

    return Variant(0);
}




#endif
