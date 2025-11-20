//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/utility/PEBLPath.cpp
//    Purpose:    Utility class storing search path and searching for files there
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

#ifdef PEBL_WIN32
#include <winsock2.h> // Must be first to avoid conflicts
#endif

#include "PEBLPath.h"
#include "../base/Variant.h"
#include "PError.h"
#include "BinReloc.h"
#include "PEBLUtility.h"
#include <sys/stat.h>
#include <list>
#include <string>
#include <iostream>

using std::endl;
using std::string;
using std::ostream;
using std::cout;


#ifdef PEBL_OSX
#include <mach-o/dyld.h>	/* _NSGetExecutablePath */
#include <CoreFoundation/CFBundle.h>
#elif defined(PEBL_WIN32)
#include <windows.h>
#endif

PEBLPath::PEBLPath()
{
}


void PEBLPath::Initialize(std::list<std::string> files)
{
    //	std::cout << "Initializing path\n";


//cout << "STarting peblpath\n";
#if defined( PEBL_LINUX)

    //cout << "***********linux\n";
    //On unix, add the following paths:
    //current working directory,
    AddToPathList("./");

    //Look for absolute pathnames
    AddToPathList("");
    //The directories of each file on the command-line.
    string tmp;
    std::list<string>::iterator i=files.begin();
    i++;  //skip over the command name in UNIX.
    while(i != files.end())
        {
            //For each commandline argument, strip the filename and add it.
            tmp = PEBLUtility::StripFile(*i);
            if(tmp != "")
                {
                    AddToPathList(tmp);
                }
            i++;
        }


#ifdef EXECNAME
#define OP OP2(EXECNAME)
#define OP2(x) OP3(x)
#define OP3(a) #a

    string peblname = OP;
#else
    string peblname ="pebl2";
#endif

    string basedir = "";

    //Initialize the binreloc library (courtesy of autopackage).
    BrInitError error;
    if (br_init (&error) == 0 && error != BR_INIT_ERROR_DISABLED)
        {
            PError::SignalWarning("Warning: BinReloc failed to initialize.\n Will fallback to hardcoded default path.\n");
            basedir = "/usr/local/share/"+peblname+"/";

           // basedir = prefix + string("/share/") + peblname + string("/");
        } else {

            std::cerr << "Executable file located at: [" << br_find_exe("") << "].\n";
            string prefix = br_find_prefix("/usr/local/");
            basedir = prefix + string("/share/") + peblname + string("/");
            std::cerr << "Base resources found at: ["<< basedir <<"]\n";
        }



    //media subdirectories of execution directory.
    //fonts
    AddToPathList(MergePathAndFile(basedir, "media/fonts/"));

    //Sounds
    AddToPathList(MergePathAndFile(basedir, "media/sounds/"));

    //images
    AddToPathList(MergePathAndFile(basedir, "media/images/"));

    //text
    AddToPathList(MergePathAndFile(basedir, "media/text/"));

    //library functions
    AddToPathList(MergePathAndFile(basedir, "pebl-lib/"));
#elif defined (PEBL_EMSCRIPTEN)


    AddToPathList("./");

    
    //Look for absolute pathnames
    AddToPathList("");
    //The directories of each file on the command-line.
    string tmp;
    std::list<string>::iterator i=files.begin();
    i++;  //skip over the command name in UNIX.
    while(i != files.end())
        {
            //For each commandline argument, strip the filename and add it.
            tmp = PEBLUtility::StripFile(*i);
            if(tmp != "")
                {
                    AddToPathList(tmp);
                }
            i++;
        }


    //in emscripten, everything goes in the root of the base directory.
    string basedir = "/usr/local/share/pebl2/";

    //Add a blank root directory, but just for emscripten.
    AddToPathList("");

    //media subdirectories of execution directory.
    //fonts
    AddToPathList(MergePathAndFile(basedir, "media/fonts/"));

    //Sounds
    AddToPathList(MergePathAndFile(basedir, "media/sounds/"));

    //images
    AddToPathList(MergePathAndFile(basedir, "media/images/"));

    //text
    AddToPathList(MergePathAndFile(basedir, "media/text/"));

    //library functions
    AddToPathList(MergePathAndFile(basedir, "pebl-lib/"));


#elif defined PEBL_OSX


    //cout << "***********OSX\n";


	// ----------------------------------------------------------------------------
	// This makes relative paths work in C++ in Xcode by changing directory to the
	//  Resources folder inside the .app bundle
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
		PError::SignalFatalError("Unable to identify resource location.\n");// error!
    }
    CFRelease(resourcesURL);

	std::string basedir = (std::string)path + "/";


    //On OSX, we will keep the things in the Resources/ subdirectory inside the application bundle
    //current working directory,
    AddToPathList("./");

    //Look for absolute pathnames
    AddToPathList("");
    //The directories of each file on the command-line.
    string tmp;
    std::list<string>::iterator i=files.begin();
    i++;  //skip over the command name in UNIX.
    while(i != files.end())
	{
		//For each commandline argument, strip the filename and add it.
		tmp = PEBLUtility::StripFile(*i);
		if(tmp != "")
		{
			AddToPathList(tmp);
		}
		i++;
	}


//		_NSGetExecutablePath( pathbuf, (uint32_t*)(&bufsize));
//		cout << "OSX name: " << pathbuf << endl;

//		string prefix = br_find_prefix("");
//		basedir = prefix + string("Resources/");


    //On OSX, we will keep the things in the Resources/ subdirectory inside the application bundle


    //media subdirectories of execution directory.
    //fonts
    AddToPathList(MergePathAndFile(basedir, "media/fonts/"));

    //Sounds
    AddToPathList(MergePathAndFile(basedir, "media/sounds/"));

    //images
    AddToPathList(MergePathAndFile(basedir, "media/images/"));

    //text
    AddToPathList(MergePathAndFile(basedir, "media/text/"));

    //library functions
    AddToPathList(MergePathAndFile(basedir, "pebl-lib/"));


/*#elif defined(PEBL_WIN32) or defined(WIN32)*/
#else

 string basedir = "";

  TCHAR szPath[MAX_PATH];

  if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
   {

      //fallback calculation here szPath = "fallback";
   }

//    cout << "PATH:" << szPath << endl;
    basedir =  PEBLUtility::StripFile(szPath) + "..\\";
//cout << "***********************\n";
	//On Windows add the following paths:

    //current working directory
    AddToPathList(".\\");


    //Absolute Pathnames
    AddToPathList(basedir);



     AddToPathList("");

    //The directories of each file on the command-line.
    // files[0] is the execution directory,
    // the others are filenames.
    string tmp;
    std::list<string>::iterator i=files.begin();
    while(i != files.end())
        {
            //For each commandline argument, strip the filename and add it.
            tmp = PEBLUtility::StripFile(*i);
            if(tmp != "")
                {
                    AddToPathList(tmp);
                }
            i++;
        }


    //media subdirectories of execution directory.
    //fonts
    AddToPathList(MergePathAndFile(basedir, "media\\fonts\\"));

    //Audio
    AddToPathList(MergePathAndFile(basedir, "media\\sounds\\"));

    //images
    AddToPathList(MergePathAndFile(basedir, "media\\images\\"));

    //Text files
    AddToPathList(MergePathAndFile(basedir, "media\\text\\"));

    //PEBL Library functions
    AddToPathList(MergePathAndFile(basedir, "pebl-lib\\"));


#endif


}

PEBLPath::~PEBLPath()
{
    mPathList.clear();
}


//Note: paths should be added with a '/' afterwards
void PEBLPath::AddToPathList(const  string & pathname)
{

    mPathList.push_back(pathname);
}


string  PEBLPath::FindFile(const string & filename)
{
    // cout << "Ylooking for file" << filename << endl;


    struct stat st;

    unsigned long int lengthp;
    unsigned long  int lengthfilename;
    string tmp;
    std::list<string>::const_iterator p = mPathList.begin();

    while(p != mPathList.end())
       {
           lengthp = (*p).size();
           lengthfilename = filename.size();

           tmp = (*p) + filename;


           //Check to see if the file exists.
           //cout << "Checking:" << tmp.c_str() <<":"<< stat(tmp.c_str(), &st)<< endl;
           if(stat(tmp.c_str(), &st)==0)
               {
                   //The file exists, so return it
                   return tmp;
               }

           //otherwise, increment and try again.
           p++;
       }
   //If everything fails, return "".
   return "";
}



const string PEBLPath::MergePathAndFile(const string & path, const string &  file)
{
    return path + file;
}


bool PEBLPath::IsDirectory(const string & pathname)
{
#if defined PEBL_UNIX
    char separator = '/';
#elif defined PEBL_WIN32
    char separator = '\\';
#elif defined WIN32
	char separator = '\\';
#elif defined PEBL_EMSCRIPTEN
    char separator = '/';
#endif


    //This may not be the best way to do it; we maybe
    //should check to see using OS library calls.
    if(pathname[pathname.size()-1] == separator)
        return true;
    else
        return false;

}


ostream & PEBLPath::Print(ostream & out) const
{

    out << "--------------------------------------\n";
    out << "Path List:\n";
    std::list<string>::const_iterator p = mPathList.begin();
    while(p != mPathList.end())
        {
            out << *p << endl;
            p++;
        }

    out << "--------------------------------------\n";
    return out;
}

ostream & operator << (ostream & out, const PEBLPath & path)
{
    return path.Print(out);

}
