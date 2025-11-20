//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////////////////
//    Name:       src/apps/PEBL.cpp
//    Purpose:    The primary PEBL run-time interpreter.
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
// Include winsock2.h first to avoid conflicts with windows.h
#include <winsock2.h>
#endif

#ifdef PEBL_ITERATIVE_EVAL
#include "../base/Evaluator-es.h"
#include "../devices/PEventLoop-es.h"
#else
#include "Globals.h"
#include "../base/Evaluator.h"
#include "../devices/PEventLoop.h"
#endif

#ifdef PEBL_EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>

// Signal test completion to JavaScript launcher for test chains
void SignalTestComplete(const char* status = "completed") {
    EM_ASM({
        var event = new CustomEvent('peblTestComplete', {
            detail: {
                status: UTF8ToString($0),
                timestamp: Date.now()
            }
        });
        document.dispatchEvent(event);
        console.log('PEBL test completed with status:', UTF8ToString($0));
    }, status);
}
#else
// No-op on native builds
inline void SignalTestComplete(const char* status = "completed") {
    // Native PEBL doesn't need completion signaling
}
#endif

#include "../base/grammar.tab.hpp"
#include "../base/PNode.h"
#include "../base/Loader.h"
#include "../base/PComplexData.h"
#include "../base/PList.h"
#include "../base/FunctionMap.h"
#include "../base/VariableMap.h"
#include "../base/Variant.h"

#include "../libs/PEBLObjects.h"

#include "../utility/PError.h"
#include "../utility/PEBLPath.h"
#include "../utility/PEBLUtility.h"
#include "../utility/rc_ptrs.h"
#include "../utility/BinReloc.h"


#include <iostream>
#include <signal.h>
#include <list>
#include <string>
#include <algorithm>
#include <fstream>

//Unix-specific definitions
#if defined(PEBL_UNIX)
//For running at higher priority:
#include <sys/resource.h>
//For better fifo scheduling.
#include <sched.h>

#elif defined(PEBL_WIN32)
//For running at higher priority.
#include <winsock2.h> //avoid collision
#include <windows.h>

STICKYKEYS g_StartupStickyKeys = {sizeof(STICKYKEYS), 0};
TOGGLEKEYS g_StartupToggleKeys = {sizeof(TOGGLEKEYS), 0};
FILTERKEYS g_StartupFilterKeys = {sizeof(FILTERKEYS), 0};




void AllowAccessibilityShortcutKeys( bool bAllowKeys )
{
    if( bAllowKeys )
    {
        // Restore StickyKeys/etc to original state and enable Windows key
      //  STICKYKEYS sk = g_StartupStickyKeys;
      //  TOGGLEKEYS tk = g_StartupToggleKeys;
      //  FILTERKEYS fk = g_StartupFilterKeys;

        SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &g_StartupStickyKeys, 0);
        SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &g_StartupToggleKeys, 0);
        SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &g_StartupFilterKeys, 0);
    }
    else
    {
        // Disable StickyKeys/etc shortcuts but if the accessibility feature is on,
        // then leave the settings alone as its probably being usefully used

        STICKYKEYS skOff = g_StartupStickyKeys;
        if( (skOff.dwFlags & SKF_STICKYKEYSON) == 0 )
        {
            // Disable the hotkey and the confirmation
            skOff.dwFlags &= ~SKF_HOTKEYACTIVE;
            skOff.dwFlags &= ~SKF_CONFIRMHOTKEY;

            SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &skOff, 0);
        }

        TOGGLEKEYS tkOff = g_StartupToggleKeys;
        if( (tkOff.dwFlags & TKF_TOGGLEKEYSON) == 0 )
        {
            // Disable the hotkey and the confirmation
            tkOff.dwFlags &= ~TKF_HOTKEYACTIVE;
            tkOff.dwFlags &= ~TKF_CONFIRMHOTKEY;

            SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tkOff, 0);
        }

        FILTERKEYS fkOff = g_StartupFilterKeys;
        if( (fkOff.dwFlags & FKF_FILTERKEYSON) == 0 )
        {
            // Disable the hotkey and the confirmation
            fkOff.dwFlags &= ~FKF_HOTKEYACTIVE;
            fkOff.dwFlags &= ~FKF_CONFIRMHOTKEY;

            SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fkOff, 0);
        }
    }
}

#endif

#ifdef WIN32
#include <time.h>
#include <objbase.h>
#include <shlobj.h>
#endif



#ifdef PEBL_OSX
#include <mach-o/dyld.h>	/* _NSGetExecutablePath */
#include <CoreFoundation/CFBundle.h>
#endif


#ifdef PEBL_MOVIES
#include "WAAVE.h"
#endif

#include "../platforms/sdl/PlatformEnvironment.h"
#include "../platforms/sdl/SDLUtility.h"


Evaluator * myEval = NULL;//new Evaluator(v,"Start");
PlatformEnvironment * myEnv = NULL;
//myEval=NULL;//new Evaluator(v,"Start");
//myEnv=NULL;


using std::cerr;
using std::endl;


/// This is the main PEBL interpreter program. It takes files as command-line arguments,
/// which it parses with the bison parser, creating a single tree of PNodes.  Then, it feeds
/// this tree into the loader, which loads the individual functions into a function map.  Then,
/// it sets up any global entities:  a graphical environment, a timer, a global variable map.
/// Finally, it locates the 'Start' function and executes it.


//Prototype for c function defined in grammar.y:
PNode *  parse(const char* filename);
std::list<string> GetFiles(int argc, std::vector<std::string> argv);
void  PrintOptions();

///Initiate some static member data.
FunctionMap Evaluator::mFunctionMap;
PEventLoop *Evaluator::mEventLoop=NULL;
VariableMap Evaluator::gGlobalVariableMap;
const PNode * Evaluator::gEvalNode = NULL;
PEBLPath  Evaluator::gPath;
PCallStack Evaluator::gCallStack;


Loader* myLoader;
PNode * head;

//std::list<PNode> PError::gCallStack;


#ifdef PEBL_EMSCRIPTEN

#endif

int PEBLInterpret( int argc, std::vector<std::string> argv )
{

#ifdef PEBL_EMSCRIPTEN
   EM_ASM(console.log("=== PEBLInterpret() called ==="));
#endif

   std::cout << "**************Starting PEBLInterpret\n";
   std::cout << "**************argc:" << argc << endl;

#if defined(PEBL_UNIX) and not defined(PEBL_OSX)
    if(argc==2 && strcmp(argv[1].c_str(), "--install")==0)
        {
            string basedir;
            BrInitError error;
            if (br_init (&error) == 0 && error != BR_INIT_ERROR_DISABLED)
                {
                    PError::SignalWarning("Warning: BinReloc failed to initialize.\n Will fallback to hardcoded default path.\n");
                    //basedir = "/usr/local/share/pebl/";
                    basedir = "/usr/local/share/pebl2";
                }

            string prefix = br_find_prefix("/usr/local/");
            basedir = prefix + string("/share/pebl2/battery/");
            string destdir = string("~/Documents/pebl-exp.") + PEBL_VERSION;

            //Now, copy everything in 'battery' to your documents directory.
            //std::cerr << "Creating Documents/pebl-exp.0.14 Directory\n";
            PEBLUtility::SystemCall("mkdir ~/Documents","");
            PEBLUtility::SystemCall("mkdir "+destdir,"");
            std::cerr << "Copying files to ["+destdir+ "]\n";
            PEBLUtility::SystemCall("cp -R "+ basedir + " " + destdir,"");
            exit(0);
        }
#endif

    PNode * tmp = NULL;

    //Cycle through command-line parameters extracting the files.
    //This does not check for file validity, it just removes any other command-line options,
    //i.e. ones that are of the form <-flag> <option> or whatever.
   //

#if 1
    //THis is just for debugging purposes.
    cout << "************Arguments: "<< argv.size()<<"\n";
    std::vector<std::string>::iterator ii = argv.begin();
     while(ii != argv.end())
     {
         cout << *ii << endl;
         ii++;
         cout << "********\n";
     }
#endif


     //get the test file from the command line:
    std::list<std::string> files = GetFiles(argc, argv);

    //Set up the search path.
    Evaluator::gPath.Initialize(files);

    cerr << "PATH:"  << Evaluator::gPath;

    //Add the built-in PEBL libraries to the files list.
    files.push_back("Design.pbl");
    files.push_back("Utility.pbl");
    files.push_back("Math.pbl");
    files.push_back("Graphics.pbl");
    files.push_back("UI.pbl");
    files.push_back("HTML.pbl");
//This contains two functions for http, but that requires libraries be compiled.
//    files.push_back("Transfer.pbl");

#ifdef PEBL_EMSCRIPTEN
   files.push_back("EM.pbl");

#endif
    //    files.push_back("Taguchi.pbl"); //not ready

   //this was already loaded on the command-line.
   //load/run test.pbl here.
#ifdef PEBL_EMSCRIPTEN
    //std::cout << "Loading filename:[test.pbl]\n";
    //files.push_back("test.pbl");
#endif


    //Process the first command-line argument.
    std::list<std::string>::iterator i = files.begin();
    i++;

    //-----------------------------------------------------------
    //        Process all files on the command-line
    //-----------------------------------------------------------

	std::cerr << "Loading filename:[" << *i << "]\n";
	string inputfilename = Evaluator::gPath.FindFile(*i);
    string otherfilename;

    head = NULL;
    if(inputfilename != "")
        {
            cerr << "Processing PEBL Source File1: " <<  inputfilename << endl;
            head  = parse(inputfilename.c_str());
        }
    else
        {
            PError::SignalFatalError("Unable to find file: [" + inputfilename + "].");
        }
    i++;
    //If there are any more arguments, process them by accomodating them
    //inside a function list.

    //Increment the iterator to move to the second command-line
   // i++;
    while(i != files.end())
        {
            std::cerr << "********************\n";
        	std::cerr << "Loading file name: ["      << *i <<"]"<< endl;
            otherfilename = Evaluator::gPath.FindFile(*i);
            std::cerr << "Resolved as: [" <<otherfilename <<"]"<< endl;
            if(inputfilename != "")
                {
                    cerr << "Processing PEBL Source File2: " <<  otherfilename << endl;

                    //A filename could be a directory (e.g., with media in it.)
                    //If so, don't parse it.
                    if(!Evaluator::gPath.IsDirectory(otherfilename))
                        {
                            //Make a new node.
                            tmp = parse(otherfilename.c_str());

                            //Now, make a new node that contains head and tmp.
                            head = new OpNode(PEBL_FUNCTIONS, head, tmp, "INTERNAL PEBL STRUCTURE", -1);
                        }
                }
            else
                {
                    PError::SignalFatalError("Unable to find file: ["+*i+"] at [" + otherfilename + "]");
                        //ignore mis-loaded files after the first; this is causing us
                            //problems on osx
                }
            i++;
       }

    //       Done processing files.
    //-----------------------------------------------------

    cerr << "---------Loading Program---------" << endl;
    //Now, load it into the environment:

    // Create a loader that will load functions into the functionmap
    myLoader = new Loader();
    myLoader->LoadUserFunctions((OpNode*)head);


    cerr <<"Analyzing code for functions." << endl;
    myLoader->FindFunctions(head);


    cerr << "Loading Library functions." << endl;
    myLoader->LoadLibraryFunctions();

    //This just destroys the function tree, not the
    //parsed node structure that is contained within
    //mFunctionMap.
    //cerr << "Removing residual function tree\n";
    ((OpNode*)head)->DestroyFunctionTree();
    delete head;
    head = NULL;

#if 0
    cerr << "\n\n--------------------------------\n";
    cerr << "Functions used in program: " << endl;
    cerr << "--------------------------------\n";
    Evaluator::mFunctionMap.DumpValues();
    cerr << "--------------------------------\n\n";
#endif

    //Parse command-line arguments.

    PList *  pList =  new PList();
    PList *  arglist = new PList();


    //Use the current screen resolution as a startingp


    //Initialize display size here with a non-interesting one.
    //It may get set by a command-line argument later.
    std::string displaySize="0x0";


    std::string depth = "32";  //used to be 16; does this matter?
    enum PEBLVideoMode displayMode;
    enum PEBLVideoDepth displayDepth;
    bool windowed = true;
    bool vsync = false;    //default vsync to false.  This is reliant on hardware that might not work well on all systems.
    bool  softrender = false;
    bool resizeable = false;
    bool unicode = true;
    bool upload = false;
    bool showHelp = false;
    bool showTestResults = false;
    
    Variant uploadConfigFile = "";
    
    Variant lang = "";
    Variant subnum = 0;


    std::string parpath = PEBLUtility::StripFile(inputfilename);
    std::string pname = PEBLUtility::GetBaseFileName(inputfilename);

    //default the parameter file to ./params/SCRIPTNAME.par

    //Variant oldpfile = Variant("params/")+Variant(inputfilename)+Variant(".par");
    Variant pfile = Variant(parpath) + Variant("params/") + Variant(pname)+Variant(".par");

    //Extract the command-line variables to bind
    for(int j = 1; j < argc; j++)
        {

            if(strcmp(argv[j].c_str(), "-v")==0 ||
                    strcmp(argv[j].c_str(), "-V")==0)
                {
                    Variant tmp = argv[++j];
					//cout <<"Content of passed-in variable " << j << ":" << tmp << endl;
                    arglist->PushBack(tmp);
                }

            else if(strcmp(argv[j].c_str(), "-s")==0 ||
                    strcmp(argv[j].c_str(), "-S")==0)
                {
                    subnum  = argv[++j];
					//cout << "collecting Subject Number "<< subnum << endl;
  				}
            //set the driver directly from the command-line, if necessary.
            else if(strcmp(argv[j].c_str(),"--driver")==0)
                {
                    if(j+1 < argc)
                        {
                            j++;
                            Evaluator::gGlobalVariableMap.AddVariable("gDriverHint",argv[j].c_str());
                        }
                }
            else if(strcmp(argv[j].c_str(),"--display")==0)
                {
                    displaySize = argv[++j];
                }

            else if(strcmp(argv[j].c_str(),"--depth")==0)
                {
                    depth = argv[++j];

                }

            else if (strcmp(argv[j].c_str(),"--fullscreen")==0)
                {
                    windowed = false;
                }
            else if(strcmp(argv[j].c_str(),"--windowed")==0)
                {
                    windowed = true;
                }
            else if(strcmp(argv[j].c_str(),"--unicode")==0)
                {
                    unicode = true;
                }
            else if(strcmp(argv[j].c_str(),"--language")==0)
                {
                    lang = argv[++j];
                }

            else if(strcmp(argv[j].c_str(),"--pfile")==0)
                {
                    std::string pfileArg = argv[++j];
                    // Check if it's a URL - if so, use it directly without prepending "params/"
                    if(pfileArg.compare(0, 7, "http://") == 0 || pfileArg.compare(0, 8, "https://") == 0)
                    {
                        pfile = Variant(pfileArg);
                    }
                    else
                    {
                        pfile = Variant("params/") + Variant(pfileArg);
                    }
                }

            else if(strcmp(argv[j].c_str(),"--upload")==0)
                {

                    upload = true;
                    uploadConfigFile = Variant(argv[++j]);  //Pass the upload config file in
                    cout << "setting upload file: [" << uploadConfigFile << "]\n";
                }

            else if(strcmp(argv[j].c_str(),"--resizeable")==0 ||
                    strcmp(argv[j].c_str(),"--resizable")==0  )
                {
                    if(windowed)
                        resizeable = true;
                }
            else if(strcmp(argv[j].c_str(),"--vsyncon")==0)
                {
                    vsync = true;
                }

            else if(strcmp(argv[j].c_str(),"--vsyncoff")==0)
                {
                    vsync = false;
                }
            else if(strcmp(argv[j].c_str(),"--softrender")==0)
                {
                    softrender = true;
                }
            else if(strcmp(argv[j].c_str(),"--help")==0)
                {
                    showHelp = true;
                }
            else if(strcmp(argv[j].c_str(),"--showtestresults")==0)
                {
                    showTestResults = true;
                }



        }


    //Now, set the display modes variables based on the command-line options.
    displayMode = PEBLUtility::GetVideoMode(displaySize);
    displayDepth = PEBLUtility::GetVideoDepth(depth);



    //This sets the video driver, and other platform-specific stuff.
#if defined(PEBL_UNIX)



    //Now, set the priority to the highest it can go.

    cerr << "Priority set here**************************\n";
     int priority = getpriority(PRIO_PROCESS,0);
    cerr << "Process running at a nice value of " << priority << endl;
    cerr << "attempting to set priority to: " << PRIO_MIN << endl;
    //setpriority(PRIO_PROCESS,0,0);
    setpriority(PRIO_PROCESS,0,PRIO_MIN);
    priority = getpriority(PRIO_PROCESS,0);

     cerr << "Process running at a nice value of " << priority << endl;

    /*
      struct sched_param mysched;
      mysched.sched_priority = sched_get_priority_max(SCHED_FIFO) - 1;
      if( sched_setscheduler( 0, SCHED_RR, &mysched ) == -1 )
      {
      cout << "Unable to enable round-robin scheduling.  Must have root priviledges.\n";
      }
      else
      {
      cout << "Round-robin scheduling enabled.\n";
      }

      struct timespec interval;
      if(sched_rr_get_interval(0,&interval)== -1)
      {
      cout << "Unable to get Round-robin scheduling interval.\n";
      }
      else
      {
      cout << "Round Robin Scheduling Interval: [" <<interval.tv_sec * 1000 + interval.tv_nsec / 1000 <<"] ms.\n";
      }

    */


#elif defined(PEBL_WIN32)
     //Do specific win32 stuff here.

    //SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
    //REALTIME causes program to hang on keyboard input.
    SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);

    //setenv()
#endif

    //cout <<"About to create environment\n";

    // We can't use any SDL-related functions before this function is called.
    // But we may want to know current screen resolution before we set displaymode

    PEBLObjects::MakeEnvironment(displayMode, displayDepth, windowed,resizeable,unicode);

    cerr << "Environment created\n";


    //Seed the random number generator with time of day.
    srand((unsigned int)time(0));

    cerr << "---------Creating Evaluator-----" << endl;
    //Create evaluator, because it contains a function map as a static member variable.
    //Create it with the command-line -v parameters as a list  bound to the argument.

#if defined(PEBL_EMSCRIPTEN)
    std::cerr <<"--------o-o-o-o-o-o--\n";
    arglist->PushBack(Variant(0));
    PComplexData * pcd = new PComplexData(counted_ptr<PEBLObjectBase>(arglist));
    pList->PushBack(Variant(pcd));
#else


    //Now, arglist should contain any values specified with the -v flag.
    if(arglist->Length()==0)
        {
            cerr <<"No command line arguments given\n";
            arglist->PushBack(Variant(0));
            PComplexData * pcd = new PComplexData(counted_ptr<PEBLObjectBase>(arglist));
            pList->PushBack(Variant(pcd));
        }
    else
        {
            PComplexData * pcd = new PComplexData(counted_ptr<PEBLObjectBase>(arglist));
            pList->PushBack(Variant(pcd));
        }
#endif

    PComplexData * pcd2 = new PComplexData(counted_ptr<PEBLObjectBase>(pList));
    Variant v = Variant(pcd2);

    if(showHelp)
        {
            PrintOptions();
        }


    
    std::list<PNode> tmpcallstack;

    //myEval is now a global, because we have moved to a single-evaluator model:
    myEval = new Evaluator(v,"Start");

    //Set the executable name here:
#if defined(PEBL_LINUX) || defined(PEBL_UNIX)
    // Check for AppImage first (special case - need .AppImage path, not extracted binary)
    const char* appimage_path = getenv("APPIMAGE");
    if (appimage_path != NULL) {
        // Running from AppImage - use APPIMAGE env var
        myEval->gGlobalVariableMap.AddVariable("gExecutableName", appimage_path);
        cerr << "Running from AppImage: " << appimage_path << endl;
    } else {
        // Standard Linux installation or local build - use BinReloc
        char* exe_path = br_find_exe(argv[0].c_str());
        if (exe_path != NULL) {
            // br_find_exe succeeded - use absolute path
            myEval->gGlobalVariableMap.AddVariable("gExecutableName", exe_path);
            cerr << "Executable path: " << exe_path << endl;
            free(exe_path);  // br_find_exe allocates memory
        } else {
            // Fallback: use argv[0] as-is (shouldn't happen on Linux)
            myEval->gGlobalVariableMap.AddVariable("gExecutableName", argv[0]);
            PError::SignalWarning("Warning: br_find_exe failed, using argv[0]");
        }
    }
#else
    // Windows, macOS, Emscripten - use argv[0]
    myEval->gGlobalVariableMap.AddVariable("gExecutableName", argv[0]);
#endif
    myEval->gGlobalVariableMap.AddVariable("gScriptName", Variant(inputfilename));
    //Set the default screen resolution based on the current one.
    Variant cursize = SDLUtility::GetCurrentScreenResolution();

    // Extract width and height from SDL display mode for both native and Emscripten builds
    PList * plist = cursize.GetComplexData()->GetList();
    Variant width = plist->First();
    Variant height = plist->Nth(2);

    delete plist;
    cursize = 0;

    myEval->gGlobalVariableMap.AddVariable("gVideoWidth", width);
    myEval->gGlobalVariableMap.AddVariable("gVideoHeight", height);


    myEval->gGlobalVariableMap.AddVariable("gShowTestResults",Variant(showTestResults));
    //displaysize may have been set at the command line.  If so, we will need to
    //override it.  It is currently a string called displaysize.


    size_t found = displaySize.find("x");
    if(found == string::npos)
        {
            //Nothing is found.  Use 0s to indicate an invalid displaysize
            width = 0;
            height = 0;
        } else
        {
            cerr <<"Size from command line argument: "  << displaySize.substr(0,found)<< "|"<< displaySize.substr(found+1) <<endl;
            //something was found.
            width =  atoi(displaySize.substr(0,found).c_str());
            height = atoi(displaySize.substr(found+1).c_str());
        }


    if((pInt)width>0  & (pInt)height>0)
        {
            Evaluator::gGlobalVariableMap.AddVariable("gVideoWidth",width);
            Evaluator::gGlobalVariableMap.AddVariable("gVideoHeight",height);
        }


    //This lets you change vsync within the script.
    Evaluator::gGlobalVariableMap.AddVariable("gVSync",Variant(vsync));


    Evaluator::gGlobalVariableMap.AddVariable("gSoftRender",Variant(softrender));

    //Add the subject identifier.
    Evaluator::gGlobalVariableMap.AddVariable("gSubNum",subnum);
    //If this is set to 1, we have reset the subject code, and
    //it is presumably good for all future resets.
    Evaluator::gGlobalVariableMap.AddVariable("gResetSubNum",0);

    //whether to automatically attempt upload.
    //if 0 it doesn't upload.
    //if non-0, that specifies the upload config file
    Evaluator::gGlobalVariableMap.AddVariable("gUpload",Variant(upload));
    Evaluator::gGlobalVariableMap.AddVariable("gUploadFile",uploadConfigFile); //"" if upload is false
    
    //Translate lang to the uppercase 2-letter code
    std::string tmps =lang;
    transform(tmps.begin(),tmps.end(),tmps.begin(),toupper);
    Evaluator::gGlobalVariableMap.AddVariable("gLanguage",Variant(tmps));


    //this global can't be set in-script; changing it will have no impact.
    Evaluator::gGlobalVariableMap.AddVariable("gFullscreen",Variant(1-windowed));

    //Add a special 'quote' character.
    Evaluator::gGlobalVariableMap.AddVariable("gQuote",Variant("\""));


    //Add the default 'base font' names based on language
    //Uses Noto fonts for comprehensive international coverage, DejaVu for Western scripts
    //ISO 639-1 two-letter language codes
    if (tmps == "AR") {
        // Arabic
        // Use DejaVu which has Arabic support AND Latin/symbols for mixed content
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFont",Variant("DejaVuSans.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMono",Variant("DejaVuSansMono.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerif",Variant("DejaVuSerif.ttf"));
    }
    else if (tmps == "HE" || tmps == "IW") {
        // Hebrew (he=modern, iw=deprecated ISO 639-1 code)
        // Use DejaVu which has Hebrew support AND Latin/symbols for mixed content
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFont",Variant("DejaVuSans.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMono",Variant("DejaVuSansMono.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerif",Variant("DejaVuSerif.ttf"));
    }
    else if (tmps == "TH") {
        // Thai
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFont",Variant("NotoSansThai-Regular.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMono",Variant("NotoSansMono-Regular.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerif",Variant("NotoSerif-Regular.ttf"));
    }
    else if (tmps == "HI" || tmps == "MR" || tmps == "NE") {
        // Devanagari script (Hindi, Marathi, Nepali)
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFont",Variant("NotoSansDevanagari-Regular.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMono",Variant("NotoSansMono-Regular.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerif",Variant("NotoSerif-Regular.ttf"));
    }
    else if (tmps == "BN") {
        // Bengali
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFont",Variant("NotoSansBengali-Regular.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMono",Variant("NotoSansMono-Regular.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerif",Variant("NotoSerif-Regular.ttf"));
    }
    else if (tmps == "KA") {
        // Georgian
        // Use DejaVu which has Georgian support AND Latin/symbols for mixed content
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFont",Variant("DejaVuSans.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMono",Variant("DejaVuSansMono.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerif",Variant("DejaVuSerif.ttf"));
    }
    else if (tmps == "ZH" || tmps == "CN" || tmps == "TW") {
        // Chinese (Simplified/Traditional)
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFont",Variant("NotoSansCJK-Regular.ttc"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMono",Variant("NotoSansMono-Regular.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerif",Variant("NotoSerif-Regular.ttf"));
    }
    else if (tmps == "JA" || tmps == "JP") {
        // Japanese
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFont",Variant("NotoSansCJK-Regular.ttc"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMono",Variant("NotoSansMono-Regular.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerif",Variant("NotoSerif-Regular.ttf"));
    }
    else if (tmps == "KO" || tmps == "KR" || tmps == "KP") {
        // Korean
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFont",Variant("NotoSansCJK-Regular.ttc"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMono",Variant("NotoSansMono-Regular.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerif",Variant("NotoSerif-Regular.ttf"));
    }
    else {
        // Default: Western scripts (Latin, Cyrillic, Greek) - DejaVu has excellent coverage
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFont",Variant("DejaVuSans.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMono",Variant("DejaVuSansMono.ttf"));
        Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerif",Variant("DejaVuSerif.ttf"));
    }

    // Always set fallback fonts to DejaVu (optimal for Western/Latin scripts)
    // These are used when translations aren't available, avoiding issues like:
    // - RTL languages (AR/HE) right-justifying English text
    // - CJK languages rendering Latin text in CJK fonts
    Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontFallback",Variant("DejaVuSans.ttf"));
    Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontMonoFallback",Variant("DejaVuSansMono.ttf"));
    Evaluator::gGlobalVariableMap.AddVariable("gPEBLBaseFontSerifFallback",Variant("DejaVuSerif.ttf"));

    //load the parameter file into a global variable
    Evaluator::gGlobalVariableMap.AddVariable("gParamFile",Variant(pfile));


    //Do easy-sleep by default:
    Evaluator::gGlobalVariableMap.AddVariable("gSleepEasy",Variant(1));
    //Now, everything should be F-I-N-E fine.
    head = myLoader->GetMainPEBLFunction();

    if(head)
        {
            cerr << "---------Evaluating Program-----" << endl;
            //Execute everything


#ifdef PEBL_ITERATIVE_EVAL
            // Iterative evaluator - start at head node and run until stack is empty
            cout << "Starting evaluation with iterative evaluator\n";

            // Wrap Start() call in proper PEBL_FUNCTION node to ensure call stack is managed correctly
            // This matches how all other lambda function calls work (PEBL_FUNCTION -> PEBL_FUNCTION_TAIL1 -> PEBL_FUNCTION_TAIL2 -> PEBL_LAMBDAFUNCTION)
            // Without this wrapper, the call stack push in PEBL_FUNCTION_TAIL2 (line 1103) is skipped,
            // causing gCallStack to be empty when PEBL_FUNCTION_TAIL_LIBFUNCTION tries to pop (line 1134)

            // Create argument for Start() - pass the command-line arguments list (same as recursive evaluator)
            // If no args, this will be a list containing 0
            DataNode* argNode = new DataNode(v, "", 0);  // v is the Variant created above (pcd2 containing pList)
            OpNode* argList = new OpNode(PEBL_LISTITEM, argNode, NULL, "", 0);
            OpNode* args = new OpNode(PEBL_ARGLIST, argList, NULL, "", 0);

            DataNode* funcNameNode = new DataNode(Variant("START", P_DATA_FUNCTION), "", 0);
            OpNode* startCall = new OpNode(PEBL_FUNCTION, funcNameNode, args, "", 0);

            myEval->Evaluate1(startCall);

            cout << "Running evaluator loop\n";
            while(myEval->GetNodeStackDepth() > 0)
            {
                myEval->Evaluate1();
            }
#else
            // Recursive evaluator - traditional single-call evaluation
            cout << "Starting evaluation with recursive evaluator\n";
            ::myEval->Evaluate(head);
#endif

#ifdef PEBL_EMSCRIPTEN
            // Emscripten: Early return without cleanup (browser manages lifecycle)
            cout << "========================================" << endl;
            cout << "PEBL program completed successfully." << endl;
            cout << "========================================" << endl;

            // Signal completion to JavaScript launcher (for test chains)
            SignalTestComplete("completed");

            return 0;
#else
            // Native platforms: Perform full cleanup
            Evaluator::gGlobalVariableMap.Destroy();

            if(myLoader) delete myLoader;
            if(myEnv) delete myEnv;
            Evaluator::mFunctionMap.Destroy();

            delete ::myEval;
            ::myEval = NULL;
            //Evaluator::gGlobalVariableMap.DumpValues();

#ifdef PEBL_MOVIES
            //Close the wave player library.
            WV_waaveClose();
#endif

            //Be sure SDL quits.  We need to be sure everything is tidied up,
            //or SDL_Quit will segfault.

            SDL_Quit();
#endif


          //Let's clean up any remaining counted pointers here.

            v = 0;

            return 0;
        }
    else
        {
            cerr << "Error: Can't evaluate program" << endl;

            if(myLoader) delete myLoader;
            return 1;

        }

    return 0;

}

void  CaptureSignal(int signal)
{
    cerr << "Exiting PEBL because of captured signal.\n";

#ifdef WIN32
    // Restore back when going to windowed or shutting down
    AllowAccessibilityShortcutKeys( false );

#endif // WIN32

    // Signal completion to JavaScript launcher (for test chains)
    SignalTestComplete("signal");

    Evaluator::gGlobalVariableMap.Destroy();

    if(myLoader)  delete myLoader;
    if(myEnv) delete myEnv;
    Evaluator::mFunctionMap.Destroy();


    //Evaluator::gGlobalVariableMap.DumpValues();

#ifdef PEBL_MOVIES
    //Close the wave player library.
    WV_waaveClose();
#endif


    //quit SDL here.  It should be killed else
    SDL_Quit();


    //Something is not being cleaned up still.
    raise(signal);
    exit(0);
}


int main(int argc,  char *argv[])
{

#ifdef PEBL_EMSCRIPTEN
    EM_ASM(console.log("=== main() called in PEBL.cpp ==="));
#endif

#ifdef PEBL_WIN32
//This needs to be done after SDL_Init() - Windows only
  freopen( "CON", "w", stdout );
  freopen( "CON", "w", stderr );
#endif // PEBL_WIN32



  int newargc = argc;
    //  Set up some signals to capture
#ifdef SIGHUP
    signal(SIGHUP, CaptureSignal);
#endif

#ifdef SIGKILL
    signal(SIGKILL, CaptureSignal);
#endif

#ifdef SIGSTOP
    signal(SIGSTOP, CaptureSignal);
#endif

    //#ifdef SIGTERM
    //    signal(SIGTERM, CaptureSignal);
    //#endif

    signal(SIGINT, CaptureSignal);


#ifdef SIGQUIT
    signal(SIGQUIT, CaptureSignal);
#endif


    signal(SIGTERM, CaptureSignal);

    //char** new_argv = NULL;
    std::vector<std::string> newargv;


//Put code here that runs on only one of the platforms

#if defined(PEBL_OSX)

    //Find the location of the app bundle; save it to global variables etc.:
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char resourcepath[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)resourcepath, PATH_MAX))
    {
        PError::SignalFatalError("Unable to identify resource location.\n");// error!
    }
    CFRelease(resourcesURL);


    //Identify location for the launcher script, which should be in the resources path.
    std::string script = "/launcher.pbl";
    std::string basedir = (std::string)resourcepath ;
    std::string launch = basedir + script;

    Evaluator::gGlobalVariableMap.AddVariable("gPEBLResourcePath",Variant(resourcepath));
    Evaluator::gGlobalVariableMap.AddVariable("gPEBLBasePath",Variant(basedir));
    std::cerr << "Basedir:" << basedir << endl;
    std::cerr << "launch: " << launch << endl;


#elif defined(PEBL_WIN32)


    SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &g_StartupStickyKeys, 0);
    SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(TOGGLEKEYS), &g_StartupToggleKeys, 0);
    SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &g_StartupFilterKeys, 0);

    // Disable when usuing PEBL (reenable later?)
    AllowAccessibilityShortcutKeys( false );
    string basedir = PEBLUtility::StripFile(argv[0]) + "..\\";


    std::string resourcepath = basedir;
    std::string launch = "launcher.pbl";
    std::string script = resourcepath+"/pebl-lib/"+launch;


    Evaluator::gGlobalVariableMap.AddVariable("gPEBLResourcePath",Variant(basedir));
    Evaluator::gGlobalVariableMap.AddVariable("gPEBLBasePath",Variant(basedir));


#elif defined (PEBL_LINUX)


    string basedir;
    BrInitError error;
    if (br_init (&error) == 0 && error != BR_INIT_ERROR_DISABLED)
    {
        PError::SignalWarning("Warning: BinReloc failed to initialize.\n Will fallback to current directory.\n");
        basedir = "./";
    } else {
        // Get directory containing the executable
        char* exe_dir = br_find_exe_dir("");
        if (exe_dir != NULL) {
            // Go up one level from bin/ to get base directory
            basedir = string(exe_dir) + string("/../");
            std::cerr << "Executable directory: [" << exe_dir << "]\n";
            free(exe_dir);
        } else {
            basedir = "./";
        }
    }

    std::string resourcepath = basedir;
    std::string launch = "launcher.pbl";
    std::string script = resourcepath+"/pebl-lib/"+launch;

//    cout << "resources:  " << resourcepath << endl;
//    cout << "script:     " << script<< endl;
//    cout << "launch:     " << launch << endl;
    Evaluator::gGlobalVariableMap.AddVariable("gPEBLResourcePath",Variant(resourcepath));
    Evaluator::gGlobalVariableMap.AddVariable("gPEBLBasePath",Variant(basedir));


#elif defined (PEBL_EMSCRIPTEN)

    std::string basedir = "/usr/local/share/pebl2";

    std::string resourcepath = basedir;
    // For Emscripten, script path should always be passed via Module.callMain()
    // No default launcher - require explicit arguments
    std::string launch = "";
    std::string script = "";

    Evaluator::gGlobalVariableMap.AddVariable("gPEBLResourcePath",Variant(resourcepath));
    Evaluator::gGlobalVariableMap.AddVariable("gPEBLBasePath",Variant(basedir));

#endif


      //Identify home directory; it is an environment variable.
      std::string home = "";
      char* val = getenv("HOME");
      if(val)
      {
          home = val;
      }


    //If there are no command-line arguments (including files),
    //we will find the launcher and specify it as a command-line argument.

    if(newargc == 1)
        {
            //This indicates there are no command-line arguments.



            std::string v = (std::string)"-v";

            cerr << "basedir: " << basedir << endl ;
            cerr << "script " << script << endl ;
            cerr << "launch: " << launch << endl ;
            cerr << "resources: " <<resourcepath << endl;

            //Now, everything is ready.  Check for the pebl directory, if it  exists,
            //change to that directory, and select the launcher script to run.
			if(PEBLUtility::FileExists(home + "/Documents/pebl-exp." + PEBL_VERSION + "/"))
			   {



                   //Move to the right directory and run the launcher
                   // script = (std::string)resourcepath + (std::string)"/launcher.pbl";
                   std::string base = home + std::string("/Documents/pebl-exp.") + PEBL_VERSION + "/";
                   PEBLUtility::SetWorkingDirectory(base);



                     //In this case, don't specify resources on the command line, so we only need 2 arguments.
				   newargc = 2;
                   newargv.push_back(argv[0]);
                   newargv.push_back(launch);

                   //argv = new_argv;
                   //cout << newargv[0] << " " << newargv[1]  << endl;

			   } else{
                   //pebl-exp.xx does not exist.  We need to run the launcher using the command line argument of resources to
                   //know where to copy from.
                   newargc = 4;

                   newargv.push_back(argv[0]);
                   newargv.push_back(launch);
                   newargv.push_back(v);
                   newargv.push_back(resourcepath);
                   //argv = new_argv;
                   //cout << newargv[0] << " " << newargv[1] << " " << newargv[2] << " " << newargv[3] << endl;

                }

        } else{
            //This is what happens when argc != 1 (when there ARE arguments), on any platform
            newargc=0;
            for(int i=0; i < argc; i++)
            {
                newargv.push_back(argv[i]);
                newargc++;
                //cout << argv[i] << endl;
            }
        }



    Evaluator::gGlobalVariableMap.AddVariable("gWorkingDirectory",
                                            PEBLUtility::GetWorkingDirectory());


     std::cerr<< "Working directory: " << PEBLUtility::GetWorkingDirectory() << endl;




    return PEBLInterpret(newargc, newargv);
}


//This returns a list of the files listed on the command-line.
std::list<std::string> GetFiles(int argc,  std::vector<std::string> argv)
{

    std::list<std::string> tmp;
    std::vector<std::string>::iterator i = argv.begin();


//    i++;

    while(i != argv.end())//int i = 1; i < argc; i++) //skip the first argv, which is just executable name.
        {
            if(i->compare( "-v")==0 ||
               i->compare("-V")==0 ||
               i->compare( "-s")==0 ||
               i->compare( "-S") == 0 ||
               i->compare( "--language")==0 ||
               i->compare("--pfile")==0  ||
               i->compare("--upload")==0)

                {
                    //This is the variable switch.  get rid of it and the next argument.
                    i++;
                }
            else if (i->compare("--driver")==0 ||
                     i->compare("--display")==0 ||
                     i->compare("--depth")==0 )

                {
                    //This is a video driver switch.
                    i++;
                }
            else if (i->compare("--windowed")==0 ||
                     i->compare("--fullscreen")==0 ||
                     i->compare("--unicode")==0 ||
                     i->compare("--resizeable")==0 ||
                     i->compare("--resizable")==0||
                     i->compare("--vsyncoff")==0||
                     i->compare("--vsyncon")==0 ||
                     i->compare("--softrender")==0 ||
                     i->compare("--help")==0 ||
                     i->compare("--showtestresults")==0
                     )


                {
                    //Don't bother incrementing 'i'
                }
            else
                {
                    //Any other command line arguments are files to load.
                   // cout << "Adding: [" << *i << "]" << endl;

                    tmp.push_back(std::string(*i));
                }
            i++;
        }
    return tmp;
}

void PrintOptions()
{
    cout << "-------------------------------------------------------------------------------\n";
    cout << "PEBL: The Psychology Experiment Building Language\n";
    cout << "Version " << PEBL_VERSION << "\n";
    cout << "(c) 2003-2025 Shane T. Mueller, Ph.D.\n";
    cout << "smueller@obereed.net   http://pebl.sf.net\n";
    cout << "-------------------------------------------------------------------------------\n";

    cout << "Usage:  Invoke pebl with the experiment script files (.pbl) and command-line\n";
    cout << "arguments.\n\n";
    cout << "example: pebl experiment.pbl -v sub1 --fullscreen --display 800x600 --driver opengl\n\n";
    cout << "COMMAND-LINE OPTIONS:\n";
    cout << "-v VALUE1 -v VALUE2\n";
    cout << "  Invokes script and passes VALUE1 and VALUE2 (and any text immediately\n" ;
    cout << "  following -v) to a list in the argument of the Start() function.\n\n";
    cout << "-s IDENTIFIER\n";
    cout << "  Initiates the global variable gSubNum to IDENTIFIER.  If not set here,\n";
    cout << " gSubNum is initiated to 0.\n\n";
    cout << "--driver <drivername>\n";
    cout << "  Sets the preferred video driver via a hint, alternatives include direct3d opengl\n";
    cout << "  opengles2 opengles and software\n";
    cout << "--display  <widthxheight>\n";
    cout << "  Controls the screen width and height (in pixels). Screen resolution defaults\n";
    cout << "  to the current screen resolution. In fullscreen mode, PEBL will check whether \n";
    cout << "  the resolution is available for the video screen, and use the default mode if not\n";
    cout << "  Note: Custom screen dimensions can be controlled in-script.\n\n";
    cout << "--depth\n";
    cout << "  Controls the pixel depth.  Depends on your video card.  Currently,\n";
    cout << "  depths of 2,8,15,16,24, and 32 are allowed on the command-line.\n";
    cout << "--language <2 char lang code>\n";
    cout << "  Allows you to specify at the command line a language to enable \n selecting different text labels.\n";
    cout << "--windowed\n";
    cout << "--fullscreen\n";
    cout << "  Controls whether the script will run in a window or fullscreen.\n\n";
    cout << "--resizeable\n";
    cout << "--resizable\n";
    cout << "  Controls whether the window will be resizeable (only in windowed mode)\n\n";
    cout << "--unicode\n";
    cout << "  Turns on unicode handling, with slight overhead\n";
    cout << "--pfile <filename>\n";
    cout << "  Specifies which parameter file to use, gets bound to variable gParamFile.\n";
    cout << " --upload <fname>\n";
    cout << "   specifies an upload.json file to use to sync with data server\n";
    cout << " --vsyncon\n";
    cout << "  Turns Vsync ON (for special tasks where you need precise control of video refresh, but may be tfussy on some hardware.\n";

    cout << " --softrender\n";
    cout << " Uses software renderer instead of accelerated hardware fallback.  Disables vsync setting\n";

    cout << " --help\n";
    cout << " Display this output screen\n";

    cout << " --showtestresults\n";
    cout << " Sets global variable gShowTestResults to 1 vs 0. Allows a test to autoatically show a screen with results at the end.\n";

    cout << " Display this output screen\n";


}
