//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/Functions.h
//    Purpose:    Defines function tables
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
#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "PEBLEnvironment.h"
#include "PEBLMath.h"
#include "PEBLStream.h"
#include "PEBLObjects.h"
#include "PEBLList.h"
#include "PEBLString.h"

//A little data structure that holds lcase PEBL function names, the actual names of the functions,
//and the number of arguments to the function.
struct PEBL_Function_Type {
    char    *name;
    Variant  (*funcname)(Variant);
    int     minNumArgs;
    int     maxNumArgs;
};


namespace PEBLMath
{

    PEBL_Function_Type  FunctionTable[] =
        {
            {(char*)"LOG10",         Log10,    1, 1},
            {(char*)"LOG2",          Log2,     1, 1},
            {(char*)"LN",            Ln,       1, 1},
            {(char*)"LOGN",          LogN,     2, 2},
            {(char*)"EXP",           Exp,      1, 1},
            {(char*)"POW",           Pow,      2, 2},
            {(char*)"SQRT",          Sqrt,     1, 1},
            {(char*)"NTHROOT",       NthRoot,  2, 2},

            //Trigonometry
            {(char*)"TAN",           Tan,      1, 1},
            {(char*)"SIN",           Sin,      1, 1},
            {(char*)"COS",           Cos,      1, 1},
            {(char*)"ATAN",          ATan,     1, 1},
            {(char*)"ASIN",          ASin,     1, 1},
            {(char*)"ACOS",          ACos,     1, 1},
            {(char*)"DEGTORAD",      DegToRad, 1, 1},
            {(char*)"RADTODEG",      RadToDeg, 1, 1},

            //Rounding
            {(char*)"ROUND",         Round,    1, 2},
            {(char*)"FLOOR",         Floor,    1, 1},
            {(char*)"CEILING",       Ceiling,  1, 1},

            {(char*)"ABSFLOOR",      AbsFloor, 1, 1},

            //Modular Arithmetic
            {(char*)"MOD",           Mod,      2, 2},
            {(char*)"DIV",           Div,      2, 2},

            //Coercion
            {(char*)"TOINTEGER",     ToInt,    1, 1},
            {(char*)"TOFLOAT",       ToFloat,  1, 1},
            {(char*)"TONUMBER",      ToNumber, 1, 1},
            {(char*)"TOSTRING",      ToString, 1, 1},


            //Other
            {(char*)"SIGN",          Sign,     1, 1},
            {(char*)"ABS",           Abs,      1, 1},

            //Statistical (operates on a list; ignores non-numeric elements.)
//             {(char*)"MEAN",          Mean,     1, 1},
//             {(char*)"MIN",           Min,      1, 9999},
//             {(char*)"MAX",           Max,      1, 9999},
//             {(char*)"STDEV",         StDev,    1, 1},
//             {(char*)"MEDIAN",        Median,   1, 1},
//            {(char*)"QUANTILE",      Quantile, 2, 2},

            {(char*)"RANDOMIZETIMER",     RandomizeTimer,    0, 0},
            {(char*)"SEEDRNG",            SeedRNG,           1, 1},
            {(char*)"RANDOM",             Random,            0, 0},
            {(char*)"RANDOMDISCRETE",     RandomDiscrete,    1, 1},
            {(char*)"RANDOMUNIFORM",      RandomUniform,     1, 1},
            {(char*)"RANDOMNORMAL",       RandomNormal,      2, 2},
            {(char*)"RANDOMEXPONENTIAL",  RandomExponential, 1, 1},
            {(char*)"RANDOMLOGISTIC",     RandomLogistic,    1, 1},
            {(char*)"RANDOMLOGNORMAL",    RandomLogNormal,   2, 2},
            {(char*)"RANDOMBINOMIAL",     RandomBinomial,    2, 2},
            {(char*)"RANDOMBERNOULLI",    RandomBernoulli,   1, 1},

            {0,            0,  0, 0}
        };
}



namespace PEBLStream
{

    PEBL_Function_Type  FunctionTable[] =
        {
            {(char*)"PRINT",                Print,            1, 1},
            {(char*)"PRINT_",               Print_,           1, 1},
            {(char*)"FORMAT",               Format,           1, 1},
            {(char*)"FILEOPENREAD",         FileOpenRead,     1, 1},
            {(char*)"FILEOPENWRITE",        FileOpenWrite,    1, 1},
            {(char*)"FILEOPENOVERWRITE",    FileOpenOverwrite, 1,1},
            {(char*)"FILEOPENAPPEND",       FileOpenAppend,   1, 1},
            {(char*)"FILECLOSE",            FileClose,        1, 1},
            {(char*)"FILEPRINT",            FilePrint,        2, 2},
            {(char*)"FILEPRINT_",           FilePrint_,       2, 2},
            {(char*)"FILEREADCHARACTER",    FileReadCharacter,1, 1},
            {(char*)"FILEREADWORD",         FileReadWord,     1, 1},
            {(char*)"FILEREADLINE",         FileReadLine,     1, 1},
            {(char*)"FILEREADLIST",         FileReadList,     1, 1},
            {(char*)"FILEREADTABLE",        FileReadTable,    1, 2},
            {(char*)"FILEREADTEXT",         FileReadText,     1, 1},

            {(char*)"ENDOFLINE",            EndOfLine,        1, 1},
            {(char*)"ENDOFFILE",            EndOfFile,        1, 1},
            {(char*)"APPENDFILE",           AppendFile,       2, 2},
            {(char*)"COPYFILE",           CopyFile,       2, 2},

            // Network functions - now always available (stub implementations when PEBL_NETWORK not defined)
            {(char*)"CONNECTTOIP", ConnectToIP,               2, 2},
            {(char*)"CONNECTTOHOST", ConnectToHost,           2, 2},

            {(char*)"SETNETWORKPORT",           SetNetworkPort,               1, 1},
            {(char*)"CHECKFORNETWORKCONNECTION", CheckForNetworkConnection,   1, 1},
            {(char*)"OPENNETWORKLISTENER", OpenNetworkListener,   1, 1},

            {(char*)"ACCEPTNETWORKCONNECTION", AcceptNetworkConnection, 2,2},
            {(char*)"WAITFORNETWORKCONNECTION", WaitForNetworkConnection, 1,1},
            {(char*)"CLOSENETWORKCONNECTION", CloseNetworkConnection,    1,1},

            {(char*)"SENDDATA", SendData,                     2,2},
            {(char*)"GETDATA", GetData,                       2,2},
            {(char*)"GETMYIPADDRESS", GetMyIPAddress,         1,1},

#ifdef PEBL_HTTP
            {(char*)"GETHTTPFILE", GetHTTPFile,               4,4},
            {(char*)"GETHTTPTEXT", GetHTTPText,               3,3},
            {(char*)"POSTHTTP", PostHTTP,                     5,5},
            {(char*)"POSTHTTPFILE", PostHTTPFile,             6,6},
#endif

            {(char*)"MD5SUM", MD5Sum,                         1,1},
            {(char*)"MD5FILE", MD5File,                        1,1},


            {(char*)"WRITEPNG", WritePNG,                     3,3},
            {(char*)"PARSEJSON",   ParseJSON,       1,1},
#ifdef PEBL_USEPORTS
            {(char*)"OPENPPORT",OpenPPort,                   1,1},
            {(char*)"SETPPORTSTATE",SetPPortState,           2,2},
            {(char*)"GETPPORTSTATE",GetPPortState,           1,1},
            {(char*)"SETPPORTMODE", SetPPortMode,           2,2},

            {(char*)"OPENCOMPORT",      OpenComPort,          2,3},
            {(char*)"COMPORTSENDBYTE",  ComPortSendByte,      2,2},
            {(char*)"COMPORTGETBYTE",   ComPortGetByte,       1,1},


#endif


            {0,            0,  0, 0}
        };
}


namespace PEBLObjects
{



    PEBL_Function_Type  FunctionTable[] =
        {

            {(char*)"MAKEWINDOW",        MakeWindow, 0, 3},
            {(char*)"RESIZEWINDOW",      ResizeWindow, 3, 3},
            {(char*)"MAKEIMAGE",         MakeImage,  1, 1},
            {(char*)"MAKETEXTBOX",       MakeTextBox, 4, 4},
            {(char*)"MAKECANVAS",        MakeCanvas, 2,3},
            {(char*)"MAKECOLOR",         MakeColor,  1, 1},
            {(char*)"MAKECOLORRGB",      MakeColorRGB, 3, 3},
            {(char*)"MAKEFONT",          MakeFont,   6, 6},
            {(char*)"MAKELABEL",         MakeLabel,  2, 2},
            {(char*)"MOVE",              Move,       3, 3},
            {(char*)"GETSIZE",           GetSize,    1, 1},
            {(char*)"ADDOBJECT",         AddObject,  2, 2},
            {(char*)"REMOVEOBJECT",      RemoveObject,  2, 2},
            {(char*)"SETPOINT",          SetPoint, 4,4},
            {(char*)"GETPIXELCOLOR",     GetPixelColor,3,3},
            {(char*)"SETCURSORPOSITION", SetCursorPosition, 2,2},
            {(char*)"GETCURSORPOSITION", GetCursorPosition, 1,1},
            {(char*)"GETPARENT",         GetParent,1,1},
            {(char*)"SETEDITABLE",       SetEditable,  2,2},
            {(char*)"SETTEXT",           SetText,      2,2},
            {(char*)"GETTEXT",           GetText,      1,1},
            {(char*)"GETLINEBREAKS",     GetLineBreaks, 1,1},
            {(char*)"SETFONT",           SetFont,      2,2},
            {(char*)"SHOW",              Show,       1, 1},

            {(char*)"PRINTPROPERTIES",   PrintProperties,  1, 1},
            {(char*)"GETPROPERTYLIST",   GetPropertyList,  1, 1},
            {(char*)"SETPROPERTY",       SetProperty,  3, 3},
            {(char*)"GETPROPERTY",       GetProperty,  2, 2},
            {(char*)"PROPERTYEXISTS",    PropertyExists,  2, 2},

            {(char*)"HIDE",              Hide,       1, 1},
            {(char*)"DRAW",              Draw,       0, 1},
            {(char*)"DRAWFOR",           DrawFor,    2, 2},

            {(char*)"SETPIXEL",          SetPoint, 4,4},
            {(char*)"LOADSOUND",         LoadSound,        1, 1},
            {(char*)"PLAYBACKGROUND",    PlayBackground,   1, 1},
            {(char*)"PLAYFOREGROUND",    PlayForeground,   1, 1},
            {(char*)"STOP",              Stop,             1, 1},
            {(char*)"SETPANNING",        SetPanning,       3, 3},
            {(char*)"SETPLAYREPEATS",    SetPlayRepeats,   2, 2},
            {(char*)"MAKESINEWAVE",      MakeSineWave,     3, 3},

            {(char*)"MAKESQUAREWAVE",    MakeSquareWave,   2, 2},
            {(char*)"MAKESAWTOOTHWAVE",  MakeSawtoothWave, 2, 2},
            {(char*)"MAKECHIRP",         MakeChirp,        3, 3},


            {(char*)"MAKEAUDIOINPUTBUFFER", MakeAudioInputBuffer, 1,1},
            {(char*)"SAVEAUDIOTOWAVEFILE",   SaveAudioToWaveFile, 2,2},
            {(char*)"GETVOCALRESPONSETIME", GetVocalResponseTime, 3,3},
            {(char*)"RECORDTOBUFFER",       RecordToBuffer,      1,2},
            {(char*)"STARTAUDIOMONITOR",    StartAudioMonitor,   1,1},
            {(char*)"STOPAUDIOMONITOR",     StopAudioMonitor,    1,1},
            {(char*)"GETAUDIOSTATS",        GetAudioStats,       2,2},

            {(char*)"LINE",              Line,            5, 5},
            {(char*)"THICKLINE",         ThickLine,       6, 6},

            {(char*)"RECTANGLE",         Rectangle,       6, 6},
            {(char*)"SQUARE",            Square,          5, 5},

            {(char*)"ELLIPSE",           Ellipse,      6, 6},
            {(char*)"CIRCLE",            Circle,       5, 5},
            {(char*)"POLYGON",           Polygon,      6, 6},
            {(char*)"BEZIER",           Bezier,      6, 6},

            {(char*)"ROTOZOOM",          RotoZoom,     5,5},

            {(char*)"LOADMOVIE",      LoadMovie,     4,4},
            {(char*)"LOADAUDIOFILE",  LoadAudioFile, 1,1},
            {(char*)"STARTPLAYBACK",   StartPlayback,  1,1},
            {(char*)"PAUSEPLAYBACK",   PausePlayback,  1,1},

            {(char*)"MAKECUSTOMOBJECT",   MakeCustomObject,  1,1},
            {(char*)"CONNECTEYETRACKER",   ConnectEyeTracker,  0,0},
            {(char*)"GETEYEOBJECT",        GetEyeObject,  1,1},
            {(char*)"SETEYETRACKERHANDLER", SetEyeTrackerHandler, 2,2},

            {0,            0,  0, 0}
        };
}

namespace PEBLEnvironment
{

    PEBL_Function_Type  FunctionTable[] =
        {
            {(char *)"GETTIME",                         GetTime,           0, 0},
            {(char*) "GETTIMEHP",                       GetTimeHP,         0,0},
            {(char *)"GETTIMEOFDAY",                    GetTimeOfDay,      0, 0},
            {(char *)"WAIT",                            Wait,              1, 1},
            {(char*)"GETOBJECTTIME",                    GetObjectTime,     1,1},

            {(char *)"SHOWCURSOR",                      ShowCursor,        1, 1},
            {(char *)"SETMOUSECURSORPOSITION",          SetCursorPosition,   2,2},
            {(char *)"GETMOUSECURSORPOSITION",          GetCursorPosition,   0,0},



            {(char*)"ISKEYDOWN",                       IsKeyDown,         1, 1},
            {(char*)"ISKEYUP",                         IsKeyUp,           1, 1},
            {(char*)"ISANYKEYDOWN",                    IsAnyKeyDown,      0, 0},
            {(char*)"WAITFORKEYDOWN",                  WaitForKeyDown,    1, 1},
            {(char*)"WAITFORANYKEYDOWN",               WaitForAnyKeyDown, 0, 0},
            {(char*)"WAITFORKEYUP",                    WaitForKeyUp,      1, 1},

            {(char*)"WAITFORKEYPRESS",                 WaitForKeyPress,    1, 1},
            {(char*)"WAITFORANYKEYPRESS",              WaitForAnyKeyPress, 0, 0},
            {(char*)"WAITFORKEYRELEASE",               WaitForKeyRelease,  1, 1},
            {(char*)"WAITFORLISTKEYPRESS",             WaitForListKeyPress,  1, 1},

            {(char*)"WAITFORALLKEYSUP",                WaitForAllKeysUp,  0, 0},
            {(char*)"WAITFORANYKEYDOWNWITHTIMEOUT",    WaitForAnyKeyDownWithTimeout,  1, 1},
            {(char*)"WAITFORANYKEYPRESSWITHTIMEOUT",   WaitForAnyKeyPressWithTimeout, 1, 1},
            {(char*)"WAITFORKEYLISTDOWN",              WaitForKeyListDown, 1, 1},
            {(char*)"WAITFORLISTKEYPRESSWITHTIMEOUT",  WaitForListKeyPressWithTimeout, 2,3},
            {(char*)"WAITFORMOUSEBUTTON",              WaitForMouseButton, 0,1},
            {(char*)"WAITFORMOUSEBUTTONWITHTIMEOUT",   WaitForMouseButtonWithTimeout, 1,2},
            {(char*)"GETMOUSESTATE",                   GetMouseState,           0,0},

            {(char*)"GETNUMJOYSTICKS",                 GetNumJoysticks,      0, 0},
            {(char*)"OPENJOYSTICK",                    GetJoystick,       1, 1},
            {(char*)"GETNUMJOYSTICKAXES",              GetNumJoystickAxes,   1, 1},
            {(char*)"GETNUMJOYSTICKBALLS",             GetNumJoystickBalls,  1, 1},
            {(char*)"GETNUMJOYSTICKBUTTONS",           GetNumJoystickButtons,1, 1},
            {(char*)"GETNUMJOYSTICKHATS",              GetNumJoystickHats,   1, 1},

            {(char*)"GETJOYSTICKBUTTONSTATE",          GetJoystickButtonState, 2, 2},
            {(char*)"GETJOYSTICKAXISSTATE",            GetJoystickAxisState,   2, 2},
            {(char*)"GETJOYSTICKHATSTATE",             GetJoystickHatState,    2, 2},
            {(char*)"GETJOYSTICKBALLSTATE",            GetJoystickBallState,   2, 2},


            {(char*)"CLEAREVENTLOOP",                  ClearEventLoop,     0, 0},
            {(char*)"CALLFUNCTION",                    CallFunction,       2,2},
            {(char*)"GETINPUT",                        GetInput0,           2, 4},

            {(char*)"GETTEXTBOXCURSORFROMCLICK",       GetTextBoxCursorFromClick, 3,3},
            {(char*)"REGISTEREVENT",                   RegisterEvent,      5,6},
            {(char*)"STARTEVENTLOOP",                  StartEventLoop,      0,0},
            {(char*)"CLEAREVENTLOOP",                  ClearEventLoop,      0,0},


            {(char*)"SIGNALFATALERROR",                SignalFatalError,   1,1},
            {(char*)"EXITQUIETLY",                     ExitQuietly, 1,1},
            {(char*)"TRANSLATEKEYCODE",                TranslateKeyCode,  1, 1},
            {(char*)"TRANSLATESTRING",                 TranslateString,  1, 1},
            {(char*)"TIMESTAMP",                       TimeStamp,         0, 0},

            {(char*)"GETCURRENTSCREENRESOLUTION",      GetCurrentScreenResolution,    0, 0},
            {(char*)"GETVIDEOMODES",                   GetVideoModes,    0, 1},
            {(char*)"GETDRIVERS",                      GetDrivers, 0, 0},
            {(char*)"GETPEBLVERSION",                  GetPEBLVersion,    0, 0},

            {(char*)"GETSYSTEMTYPE",                   GetSystemType,     0, 0},
            {(char*)"LAUNCHFILE",                      LaunchFile,        1,1},
            {(char*)"SYSTEMCALL",                      SystemCall,        1,2},
//#ifdef PEBL_WINDOWS
            {(char*)"SYSTEMCALLUPDATE",                SystemCallUpdate,   1,2},
//#endif
            {(char*)"CHECKPROCESSSTATUS",              CheckProcessStatus, 1,1},
            {(char*)"COPYTOCLIPBOARD",                 CopyToClipboard,    1,1},
            {(char*)"COPYFROMCLIPBOARD",               CopyFromClipboard,    0,0},
            {(char*)"VARIABLEEXISTS",                  VariableExists,    1,1},
            {(char*)"GETEXECUTABLENAME",               GetExecutableName,  0,0},


            {(char*)"ISDIRECTORY",                     IsDirectory,        1,1},
            {(char*)"FILEEXISTS",                      FileExists,        1,1},
            {(char*)"GETDIRECTORYLISTING",             GetDirectoryListing,1,1},
            {(char*)"MAKEDIRECTORY",                   MakeDirectory,      1,1},
            {(char*)"GETHOMEDIRECTORY",                GetHomeDirectory,    0,0},
            {(char*)"GETWORKINGDIRECTORY",             GetWorkingDirectory,    0,0},
            {(char*)"SETWORKINGDIRECTORY",             SetWorkingDirectory,    1,1},

            {(char*)"DELETEFILE",                      DeleteFile,      1,1},


            {(char*)"ISTEXT",                          IsText,          1,1},
            {(char*)"ISNUMBER",                        IsNumber,          1,1},
            {(char*)"ISINTEGER",                       IsInteger,          1,1},
            {(char*)"ISFLOAT",                         IsFloat,            1,1},
            {(char*)"ISSTRING",                        IsString,           1,1},
            {(char*)"ISLIST",                          IsList,             1,1},
            {(char*)"ISTEXTBOX",                       IsTextBox,          1,1},
            {(char*)"ISCANVAS",                        IsCanvas,           1,1},
            {(char*)"ISIMAGE",                         IsImage,            1,1},
            {(char*)"ISLABEL",                         IsLabel,            1,1},
            {(char*)"ISAUDIOOUT",                      IsAudioOut,         1,1},
            {(char*)"ISFONT",                          IsFont,             1,1},
            {(char*)"ISCOLOR",                         IsColor,            1,1},
            {(char*)"ISFILESTREAM",                    IsFileStream,       1,1},
            {(char*)"ISWIDGET",                        IsWidget,           1,1},
            {(char*)"ISWINDOW",                        IsWindow,           1,1},
            {(char*)"ISSHAPE",                         IsShape,            1,1},
            {(char*)"ISCUSTOMOBJECT",                  IsCustomObject,     1,1},
            {(char*)"ISPEBLOBJECT",                    IsPEBLObject,       1,1},
            {(char*)"PLAYMOVIE",   PlayMovie,  1,1},


            {0,            0,  0, 0}
        };
}


namespace PEBLList
{

    PEBL_Function_Type  FunctionTable[] =
        {

            {(char*)"SHUFFLE",                   Shuffle, 1, 1},
            {(char*)"REPEAT",                    Repeat,    2, 2},
            {(char*)"REPEATLIST",                RepeatList,2, 2},
            {(char*)"SEQUENCE",                  Sequence,  3, 3},
            {(char*)"DESIGNFULLCOUNTERBALANCE",   DesignFullCounterbalance, 2, 2},
            {(char*)"CROSSFACTORWITHOUTDUPLICATES", CrossFactorWithoutDuplicates, 1, 1},
            {(char*)"ROTATE",                    Rotate,  2, 2},
            {(char*)"LENGTH",                    Length, 1, 1},
            {(char*)"FIRST",                     First, 1, 1},
            {(char*)"SECOND",                    Second, 1, 1},
            {(char*)"THIRD",                     Third, 1, 1},
            {(char*)"FOURTH",                    Fourth, 1, 1},
            {(char*)"FIFTH",                     Fifth, 1, 1},
            {(char*)"MERGE",                     Merge, 2, 2},
            {(char*)"LIST",                      List,  1, 1000},
            {(char*)"APPEND",                    Append, 2, 2},
            {(char*)"PUSHONEND",                 PushOnEnd, 2, 2},
            {(char*)"SETELEMENT",                SetElement, 3, 3},
            {(char*)"SORT",                      Sort, 1, 1},
            {(char*)"SORTBY",                    SortBy, 2, 2},
            {(char*)"NTH",                       Nth, 2, 2},
            {(char*)"LAST",                      Last, 1, 1},
            {(char*)"ISMEMBER",                  IsMember, 2, 2},
            {(char*)"REMOVEDUPLICATES",          RemoveDuplicates, 1, 1},
            {(char*)"MAKEMAP",                   MakeMap, 2, 2},
            {(char*)"TRANSPOSE",                 Transpose,  1, 1},
            {(char*)"SUBLIST",                   SubList, 3, 3},

            {(char*)"REMOVE",                    Remove, 2, 2},
            {(char*)"LISTTOSTRING",              ListToString,1,4},
            {(char*)"MODLIST",                   ModList,1,3},
            {0,            0,  0, 0}
        };
}

namespace PEBLString
{

    PEBL_Function_Type  FunctionTable[] =
        {

            {(char*)"UPPERCASE",                 Uppercase, 1, 1},
            {(char*)"LOWERCASE",                 Lowercase, 1, 1},
            {(char*)"TOASCII",                   ToASCII,   1,1},
            {(char*)"STRINGLENGTH",              StringLength, 1, 1},
            {(char*)"SUBSTRING",                 SubString, 3, 3},
            {(char*)"FINDINSTRING",              FindInString, 3, 3},
            {(char*)"SPLITSTRING",               SplitString, 2, 2},
            {(char*)"COPYTOCLIPBOARD",           CopyToClipboard,1,1},
            {(char*)"DETECTTEXTSCRIPT",          DetectTextScript, 1, 1},
            {(char*)"ISRTL",                     IsRTL, 1, 1},
            {(char*)"GETFONTFORTEXT",            GetFontForText, 1, 2},
            {(char*)"GETSYSTEMLOCALE",           GetSystemLocale, 0, 0},
            {(char*)"ISSYSTEMLOCALERTL",         IsSystemLocaleRTL, 0, 0},
            //          {(char*)"TOKEN",                     TokenMakeWindow, 1, 1},
            //          {(char*)"FORMAT",        Format, 1, 1},
            {0,            0,  0, 0}
        };
}



#endif
