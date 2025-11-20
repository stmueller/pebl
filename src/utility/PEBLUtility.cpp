//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////////////////
//    Name:       utility/PEBLUtility.cpp
//    Purpose:    Miscellaneous Utility Functions used in PEBL
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
#include <winsock2.h> // Must be very first to avoid conflicts
#endif

//Include this before peblutility.h because is it a header-only library,
//and the JSMN_HEADER macro disables loading of functions, so it must
//first be loaded here to compile right.
#include "jsmn.h"

#include "PEBLUtility.h"
#include "PError.h"
#include "rc_ptrs.h"
#include "../base/Variant.h"
#include "../base/PComplexData.h"
#include "../base/PList.h"
#include "../objects/PCustomObject.h"
#include "../devices/PKeyboard.h"
#include "../apps/Globals.h"
#include "md5.h"





#include <errno.h>
#include <ctype.h>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <iostream>
//#include <strstream>  //May be depracated?

#include <algorithm>

//#if !defined(PEBL_OSX)
//#include <png.h>
//#endif

#include <dirent.h>
//#include <errno.h>


#ifdef PEBL_WIN32
//#include <direct.h>  // Not available in MSYS2, use POSIX headers instead
//#include <windows.h>
//#include <bits/types.h>
#include <shlobj.h>
//#include <winbase.h>
#include <sys/stat.h>  // For mkdir on MSYS2
#elif defined(PEBL_LINUX)
#include <sys/stat.h>
#include <unistd.h>
#include <bits/types.h>
#include <pwd.h>
#elif defined (PEBL_EMSCRIPTEN)
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#elif defined (PEBL_OSX)
#include <unistd.h>
#include <pwd.h>
#endif

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h> //For md5file O_RDONLY
#include <sys/types.h>
#include <sys/stat.h>




//there is an included version of mman.h for windows
//compilation--this should be available on linux/osx in
//system libraries.  This is done for the mlock function,
//which is used in the MD5File utility function here.
#if defined(PEBL_LINUX) or defined(PEBL_OSX) or defined(PEBL_EMSCRIPTEN)
#include <sys/mman.h>
#elif defined(PEBL_WIN32)
#include "mman.h"
#endif


#include <SDL_scancode.h>
#include <SDL_locale.h>

//Some math libraries contain this, but let's not take any chances.
#define PI 3.141592653589793238462643383279502884197169399375


using std::cout;
using std::endl;

std::string PEBLUtility::ToUpper(const std::string & text)
{
    std::string newtext(text);
    std::transform(newtext.begin(), newtext.end(), newtext.begin(), ::toupper);
    return newtext;
}



std::string PEBLUtility::ToLower(const std::string & text)
{
    std::string newtext(text);
    std::transform(newtext.begin(), newtext.end(), newtext.begin(), ::tolower);
    return newtext;
}


//When given a filename, this will strip the filename from
//the path and return the path. If given a directory name
// (ending with a '/' or '\'), it won't strip that character.
const std::string PEBLUtility::StripFile(const std::string &  file)
{

#if defined PEBL_UNIX
    char separator = '/';
#else
    char separator = '\\';
#endif

    unsigned long int lastsep  = 0;
    unsigned long int i = file.size();
    //end
    //Start at the end of the filename and move backward
    while(i > 0)
        {
            if(file[i] == separator)
                {
                    lastsep = i;
                    return file.substr(0,lastsep+1);
                }
            i--;
        }
    return "";
}



//When given a filename, this will strip off the path
//and return the base filename.
const std::string PEBLUtility::GetBaseFileName(const std::string &  file)
{

#if defined PEBL_UNIX
    char separator = '/';
#else
    char separator = '\\';
#endif

    unsigned long int lastsep  = 0;
    unsigned long int i = file.size();
    //end
    //Start at the end of the filename and move backward
    while(i > 0)
        {
            if(file[i] == separator)
                {
                    lastsep = i;
                    return file.substr(lastsep+1,-1);
                }
            i--;
        }
    return "";
}



// ///This returns a pointer to an upper-case version of the text.
// ///buffer better be at least n items long.
// void PEBLUtility::ToUpper(const char* text, char* buffer, int n)
// {
//     //Go through each letter, copying the upper-case version to buffer.
//     //Stop before the last letter because we need a \0.
//     int i = 0;
//     while(text[i] != '\0' &&  i < (n-1))
//         {
//             buffer[i] = toupper(text[i]);
//             i++;
//         }
//     //Add an end-of-string character
//     buffer[i] = '\0';
// }



// ///This returns a pointer to a lower-case version of the text.
// void PEBLUtility::ToLower(const char* text, char* buffer, int n)
// {

//     //Go through each letter, copying the lower-case version to buffer.
//     //Stop before the last letter because we need a \0.
//     int i = 0;
//     while(text[i] != '\0' && i < (n-1))
//         {
//             buffer[i] = tolower(text[i]);
//             i++;
//         }

//     //Add an end-of-string character
//     buffer[i] = '\0';


// }


Variant PEBLUtility::Tokenize(const char* line, char separator)
{
    //We have a string with separators in it.  Go through it character by character and
    //form a list out of the tokens.


    PList * plist = new PList;
    int i = 0;
    int begin = 0;
    char * token;

    int tokensize;

    //Go through each item of the string.  Cut a token whenever you get to a separator or
    //a character that might be the end of the line.
    while(true)
        {
            if(line[i] == separator
               || line[i] == '\0'
               || line[i] == 10
               || line[i] == 13
               || separator == 0)
                {

                    //line[i] is the separator; the token is from
                    //begin to line[i-1]

                    //tokensize should be from begin to now, substracting out the separator.
                    tokensize = i-begin+(separator==0);

                    //if separator ==0, tokensize is really one bigger, as the separator width is 0.

                    token = (char*)malloc((tokensize+1) * sizeof(char));
                    strncpy(token, &line[begin], tokensize);
                    token[tokensize]=  '\0';


                    plist->PushBack(Variant(token));
                    begin = i+1;  //move begin to the next piece of text.
                }


            if(line[i] == '\0'|| line[i] == 10  || line[i] == 13)
                break;

            //we need to not copy a final empty element if separator==0
            if(separator==0 and line[begin]=='\0')
                {
                    break;
                }
            i++;

        }

    counted_ptr<PEBLObjectBase> tmpObj = counted_ptr<PEBLObjectBase>(plist);
    PComplexData * pcd  = new PComplexData(tmpObj);
    return Variant(pcd);
}


pDouble PEBLUtility::StringToPDouble(const char * mystring)
{
#if defined(strold)
    return strtold(mystring,0);
#else
    return (pDouble)strtod(mystring,0);
#endif
}



pDouble PEBLUtility::RandomUniform()
{
    return (pDouble)rand() / RAND_MAX;
}


/// It will return a floating-
/// point number which is a sample from the N(0,1) distribution.
/// Values are calculated using the Box-Mueller technique.
pDouble PEBLUtility::RandomNormal()
{

    pDouble x1 = RandomUniform();
    pDouble x2 = RandomUniform();

    return sqrt(-2 * log(x1)) * cos(2 * PI * x2);
}

pDouble PEBLUtility::Log2(pDouble val)
{
#if defined(log2)
    return (pDouble)log2((pDouble)val);
#elif defined(log2l)
    return log2l(val);
#else
    return logl(val) / logl(2);
#endif
}

pInt PEBLUtility::Round(pDouble val)
{
#if defined(round)
    return round((pDouble)val);
#elif defined(roundl)
    return roundl(val);
#else
    return (pInt) floor(val+.5);
#endif
}

pDouble PEBLUtility::Round(pDouble val, pInt prec)
{
    pDouble off = pow(10,prec);
    //cout << "Rounding " << val <<" to : " << off << endl;
#if defined(round)
    return (pDouble)round((pDouble)val*off)/off;
#elif defined(roundl)
    return (pDouble)roundl(val*off)/off;
#else
    return (pDouble) ((pInt)(floor(val*off+.5))/off);
#endif
}

pInt PEBLUtility::Truncate(pDouble val)
{
#if defined(truncl)
    return truncl(val);
#elif defined(trunc)
    return trunc((pDouble)val);
#else
    int sign = val < 0 ? -1: 1;
    return sign * Round(sign * val);
#endif

}



PEBL_Keycode  PEBLUtility::TranslateString(const std::string & let)
{
    std::string letters = PEBLUtility::ToLower(let);

    SDL_Keycode code =SDL_GetKeyFromName(letters.c_str());
    
    //cout <<"testing letters: [" << letters << "]"<<endl;

    if(code==SDLK_UNKNOWN)
        {


            //
            //
            if( letters == " " ||
                letters == "<space>")
                return PEBL_KEYCODE_SPACE;
            if( letters == "<return>")
                return PEBL_KEYCODE_RETURN;
            if(letters == "<esc>"|| letters == "<escape>")
                {
                    //cout << "Translating escape code\n";
                    return PEBL_KEYCODE_ESCAPE;
                }
            if(letters== "<anykey>")
                return PEBL_KEYCODE_ANYKEY;
            if(letters == "<back>"  || letters == "<backspace>" )
                return PEBL_KEYCODE_BACKSPACE;

            if(letters=="<delete>") return PEBL_KEYCODE_DELETE;

            if(letters == "<left>")  return PEBL_KEYCODE_LEFT;
            if(letters == "<right>") return PEBL_KEYCODE_RIGHT;
            if(letters == "<down>")  return PEBL_KEYCODE_DOWN;
            if(letters == "<up>")    return PEBL_KEYCODE_UP;

            if(letters == "<f1>")  return PEBL_KEYCODE_F1;
            if(letters == "<f2>")  return PEBL_KEYCODE_F2;
            if(letters == "<f3>")  return PEBL_KEYCODE_F3;
            if(letters == "<f4>")  return PEBL_KEYCODE_F4;
            if(letters == "<f5>")  return PEBL_KEYCODE_F5;
            if(letters == "<f6>")  return PEBL_KEYCODE_F6;
            if(letters == "<f7>")  return PEBL_KEYCODE_F7;
            if(letters == "<f8>")  return PEBL_KEYCODE_F8;
            if(letters == "<f9>")  return PEBL_KEYCODE_F9;
            if(letters == "<f10>")  return PEBL_KEYCODE_F10;
            if(letters == "<f11>")  return PEBL_KEYCODE_F11;
            if(letters == "<f12>")  return PEBL_KEYCODE_F12;


            /* Key state modifier keys */

            if(letters == "<numlock>")  return PEBL_KEYCODE_NUMLOCK;
            if(letters == "<capslock>")  return PEBL_KEYCODE_CAPSLOCK;
            if(letters == "<scrolllock>")  return PEBL_KEYCODE_SCROLLLOCK;
            if(letters == "<lshift>")    return PEBL_KEYCODE_LSHIFT;
            if(letters == "<rshift>")    return PEBL_KEYCODE_RSHIFT;

            if(letters == "<rctrl>")    return PEBL_KEYCODE_RCTRL;
            if(letters == "<lctrl>")    return PEBL_KEYCODE_LCTRL;
            if(letters == "<ralt>")     return PEBL_KEYCODE_RALT;
            if(letters == "<lalt>")     return PEBL_KEYCODE_LALT;
            //            if(letters == "<rmeta>")    return PEBL_KEYCODE_RMETA;
            //            if(letters == "<lmeta>")    return PEBL_KEYCODE_LMETA;
            //            if(letters == "<lsuper>")   return PEBL_KEYCODE_LSUPER;
            //            if(letters == "<rsuper>")   return PEBL_KEYCODE_RSUPER;
            if(letters == "<mode>")     return PEBL_KEYCODE_MODE;
            //            if(letters == "<compose>")  return PEBL_KEYCODE_COMPOSE;


            if(letters == "<kp_0>") return  PEBL_KEYCODE_KP_0;
            if(letters == "<kp_1>") return  PEBL_KEYCODE_KP_1;
            if(letters == "<kp_2>") return  PEBL_KEYCODE_KP_2;
            if(letters == "<kp_3>") return  PEBL_KEYCODE_KP_3;
            if(letters == "<kp_4>") return  PEBL_KEYCODE_KP_4;
            if(letters == "<kp_5>") return  PEBL_KEYCODE_KP_5;
            if(letters == "<kp_6>") return  PEBL_KEYCODE_KP_6;
            if(letters == "<kp_7>") return  PEBL_KEYCODE_KP_7;
            if(letters == "<kp_8>") return  PEBL_KEYCODE_KP_8;
            if(letters == "<kp_9>") return  PEBL_KEYCODE_KP_9;

            if(letters == "<kp_period>"  ) return  PEBL_KEYCODE_KP_PERIOD;
            if(letters == "<kp_divide>"  ) return  PEBL_KEYCODE_KP_DIVIDE;
            if(letters == "<kp_multiply>") return  PEBL_KEYCODE_KP_MULTIPLY;
            if(letters == "<kp_minus>"   ) return  PEBL_KEYCODE_KP_MINUS;
            if(letters == "<kp_plus>"    ) return  PEBL_KEYCODE_KP_PLUS;
            if(letters == "<kp_equals>"  ) return  PEBL_KEYCODE_KP_EQUALS;
            if(letters == "<kp_enter>"  )  return  PEBL_KEYCODE_KP_ENTER;

            if(letters == "<insert>"  ) return  PEBL_KEYCODE_INSERT;
            if(letters == "<home>"    ) return  PEBL_KEYCODE_HOME;
            if(letters == "<end>"     ) return  PEBL_KEYCODE_END;
            if(letters == "<pageup>"  ) return  PEBL_KEYCODE_PAGEUP;
            if(letters == "<pagedown>") return  PEBL_KEYCODE_PAGEDOWN;
            std::cerr << "Unknown keycode!!!\n";
            return PEBL_KEYCODE_UNKNOWN;

        }
    return (PEBL_Keycode)code;
}



//This returns a text-valued description of the key pressed.
std::string PEBLUtility::TranslateKeycode(const PEBL_Keycode code, int modkeys)
{



    switch(code)
        {

        case  PEBL_KEYCODE_SPACE: return " ";
        case PEBL_KEYCODE_UNKNOWN:        return "<unknown>";
        case PEBL_KEYCODE_ANYKEY:         return "<anykey>";
        case PEBL_KEYCODE_BACKSPACE:      return "<backspace>";
        case PEBL_KEYCODE_TAB:            return "<tab>";
        case PEBL_KEYCODE_CLEAR:          return "<clear>";
        case PEBL_KEYCODE_RETURN:         return "<return>";
        case PEBL_KEYCODE_RETURN2:         return "<return>";
        case PEBL_KEYCODE_KP_ENTER:         return "<return>";

        case PEBL_KEYCODE_PAUSE:          return "<pause>";
        case PEBL_KEYCODE_ESCAPE:         return "<esc>";

        /* Arrows + Home/End pad */
        case PEBL_KEYCODE_UP:         return "<up>";
        case PEBL_KEYCODE_DOWN:       return "<down>";
        case PEBL_KEYCODE_RIGHT:      return "<right>";
        case PEBL_KEYCODE_LEFT:       return "<left>";
        case PEBL_KEYCODE_INSERT:     return "<insert>";
        case PEBL_KEYCODE_HOME:       return "<home>";
        case PEBL_KEYCODE_END:        return "<end>";
        case PEBL_KEYCODE_PAGEUP:     return "<pageup>";
        case PEBL_KEYCODE_PAGEDOWN:   return "<pagedown>";

        /* Function keys */
        case PEBL_KEYCODE_F1:         return "<f1>";
        case PEBL_KEYCODE_F2:         return "<f2>";
        case PEBL_KEYCODE_F3:         return "<f3>";
        case PEBL_KEYCODE_F4:         return "<f4>";
        case PEBL_KEYCODE_F5:         return "<f5>";
        case PEBL_KEYCODE_F6:         return "<f6>";
        case PEBL_KEYCODE_F7:         return "<f7>";
        case PEBL_KEYCODE_F8:         return "<f8>";
        case PEBL_KEYCODE_F9:         return "<f9>";
        case PEBL_KEYCODE_F10:         return "<f10>";
        case PEBL_KEYCODE_F11:         return "<f11>";
        case PEBL_KEYCODE_F12:         return "<f12>";


        /* Key state modifier keys */
        case PEBL_KEYCODE_NUMLOCK:      return "<numlock>";
        case PEBL_KEYCODE_CAPSLOCK:     return "<capslock>";
        case PEBL_KEYCODE_SCROLLLOCK:    return "<scrolllock>";
        case PEBL_KEYCODE_RSHIFT:       return "<rshift>";
        case PEBL_KEYCODE_LSHIFT:       return "<lshift>";
        case PEBL_KEYCODE_RCTRL:        return "<rctrl>";
        case PEBL_KEYCODE_LCTRL:        return "<lctrl>";
        case PEBL_KEYCODE_RALT:         return "<ralt>";
        case PEBL_KEYCODE_LALT:         return "<lalt>";
        case PEBL_KEYCODE_MODE:         return "<mode>";


        /* Miscellaneous function keys */
        case PEBL_KEYCODE_HELP:         return "<help>";
        case PEBL_KEYCODE_SYSREQ:       return "<sysreq>";
        case PEBL_KEYCODE_MENU:         return "<menu>";
        case PEBL_KEYCODE_POWER:        return "<power>";
        case PEBL_KEYCODE_UNDO:         return "<undo>";


        /*Keypadcodes:*/

        case PEBL_KEYCODE_KP_DIVIDE:    return "/";
        case PEBL_KEYCODE_KP_MULTIPLY:  return "*";
        case PEBL_KEYCODE_KP_MINUS:     return "-";
        case PEBL_KEYCODE_KP_PLUS:      return "+";

        case PEBL_KEYCODE_KP_1: return "1";
        case PEBL_KEYCODE_KP_2: return "2";
        case PEBL_KEYCODE_KP_3: return "3";
        case PEBL_KEYCODE_KP_4: return "4";
        case PEBL_KEYCODE_KP_5: return "5";
        case PEBL_KEYCODE_KP_6: return "6";
        case PEBL_KEYCODE_KP_7: return "7";
        case PEBL_KEYCODE_KP_8: return "8";
        case PEBL_KEYCODE_KP_9: return "9";
        case PEBL_KEYCODE_KP_0: return "0";
        case PEBL_KEYCODE_KP_PERIOD: return ".";

        default:

            std::string ltrs =  SDL_GetKeyName((SDL_Keycode)code);
            return ShiftSwitch(modkeys, ToLower(ltrs),ToUpper(ltrs));
        }

}




PEBLVideoMode PEBLUtility::GetVideoMode(std::string modeline)
{

	PEBLVideoMode mode;

	if(modeline ==        "512x384") mode =PVM_512_384;
	else if (modeline ==  "640x480") mode =	PVM_640_480;
	else if (modeline ==  "800x600")	mode =	PVM_800_600;
	else if (modeline ==	"960x720")	mode =	PVM_960_720;
	else if (modeline ==	"1024x768")	mode =	PVM_1024_768;
	else if (modeline ==	"1152x864")	mode =	PVM_1152_864;
	else if (modeline ==	"1280x1024") mode =	PVM_1280_1024;
	else mode  = PVM_800_600;
	return mode;
}


PEBLVideoDepth PEBLUtility::GetVideoDepth(std::string depthline)
{
	PEBLVideoDepth depth;

	if(depthline == "2")       depth =PVD_2;
	else if (depthline =="15") depth =PVD_15;
	else if (depthline =="16") depth =PVD_16;
	else if (depthline =="24") depth =PVD_24;
	else if (depthline =="32") depth =PVD_32;
	else depth  = PVD_16;

	return depth;
}



//This returns upper if a shift key is pressed, otherwise it returns lower.
std::string PEBLUtility::ShiftSwitch(int modkeys, std::string lower, std::string upper)
{
    if( modkeys & PEBLMOD_SHIFT)
        return upper;
    else
        return lower;
}


Variant PEBLUtility::IsDirectory(std::string path)
{
    DIR *dirp;
    //struct dirent *entry;
    dirp = opendir(path.c_str());


    if(dirp)
        {

			closedir(dirp);
            return Variant(1);
        }
    else
        {
				//closedir(dirp);
            //std::cerr << "Error type: " <<errno << std::endl;
            return Variant(0);
        }
    //entry->d_type;



}

Variant PEBLUtility::FileExists(std::string path)
{

    struct stat stFileInfo;
    //may need to use _stat on windows
    int out = stat(path.c_str(),&stFileInfo);
    //cout << path.c_str()<<"\n";
    //cout << "File info:" << out << endl;
    //  We can get better info about the file
    //  if we look at out.
    return Variant(out==0);
}





Variant PEBLUtility::GetDirectoryListing(std::string path)
{

    //cout << "Getting directory listing\n";
    DIR *dirp;
    struct dirent *entry;
    PList * plist = new PList();
    PComplexData*pcd=NULL;

    dirp = opendir(path.c_str());

    if(dirp)
        {

            //not this is an assignment, not an equality
            while((entry = readdir(dirp)))
                {
                    //cout << entry->d_name << endl;
                    plist->PushBack(Variant(entry->d_name));
                }
        } else {
//ERRNO CODES:
//			EACCES

//				EMFILE
//
//				ENFILE
//				The entire system, or perhaps the file system which contains the directory, cannot support any additional open files at the moment. (This problem cannot happen on the GNU system.)
//				ENOMEM
//				Not enough memory available.

		    Variant codes = "";
			switch (errno) {
            case EACCES:
                codes = "Read permission is denied for the directory named by dirname.(EACCES)";
                break;

			case EMFILE:
                codes = "The process has too many files open (EMFILE).";
                break;
            case ENFILE:
                codes = "The entire system, or perhaps the file system which contains the directory, cannot support any additional open files at the moment. (ENFILE)";
					break;
            case ENOMEM:
                codes = "Not enough memory available. (ENOMEM)";
                break;
            default:
                codes =Variant("Unknown error") + Variant(errno);
                break;
			}

			PError::SignalFatalError(Variant("Unable to get Directory listing at ") + Variant(path) + codes);

    }


    closedir(dirp);

    counted_ptr<PEBLObjectBase> tmplist = counted_ptr<PEBLObjectBase>(plist);
    pcd = new PComplexData(tmplist);
    Variant tmp = Variant(pcd);
    delete pcd;
    return tmp;
}



Variant PEBLUtility::MakeDirectory(std::string path)

{

    if(FileExists(path) && IsDirectory(path))
        {
            return Variant(1);
          }

#if defined(PEBL_UNIX) || defined(PEBL_EMSCRIPTEN)

    if (mkdir(path.c_str(), 0777) == -1)
       {
           PError::SignalFatalError("Unable to create directory: " );//+ Variant(strerror(errno)));
       }

#elif defined(PEBL_WIN32)
    if (mkdir(path.c_str()) == -1)  // Windows mkdir takes only path argument
        {
            //cerr << strerror(errno)<<endl;
            PError::SignalFatalError("Unable to create directory: " + std::string(strerror(errno)));
        }

#endif

    return Variant(1);
}

Variant PEBLUtility::DeleteMyFile(std::string path)
{
   if(FileExists(path))
    {
     if( remove( path.c_str() ) != 0 )
          return Variant(0);
     else
          return Variant(1);
    }
	return Variant(0);

}

Variant PEBLUtility::GetHomeDirectory()
{

#ifdef PEBL_WIN32
  char path[ MAX_PATH ];
  if (SHGetFolderPathA( NULL, CSIDL_PROFILE, NULL, 0, path ) != S_OK)
    {
        PError::SignalFatalError("Unable to find user's home directory!");
    }
#elif defined(PEBL_LINUX) or defined(PEBL_OSX)

  struct passwd *p=getpwuid(getuid());
  std::string path = p->pw_dir;

#elif defined( PEBL_EMSCRIPTEN)

  //this may be wrong?
  std::string path ="~/";

#endif

  return Variant(path);
}

Variant PEBLUtility::GetWorkingDirectory()
{
#if defined(PEBL_WIN32) or  defined(PEBL_OSX)
    //maybe this will work given we compile with g++

    char *path=NULL;
    size_t size = 0;
    path=getcwd(path,size);


#elif defined(PEBL_UNIX) //linux/osx

    char* path = get_current_dir_name();

#elif defined (PEBL_EMSCRIPTEN)
    // Emscripten supports getcwd() with its virtual filesystem (MEMFS)
    char buffer[PATH_MAX];
    char* path = getcwd(buffer, sizeof(buffer));
    if (path == NULL) {
        // If getcwd fails, default to root of virtual filesystem
        return Variant("/");
    }
#endif

 return Variant(path);
}

Variant PEBLUtility::SetWorkingDirectory(std::string path)
{
#ifdef PEBL_WIN32

    if(::SetCurrentDirectory(path.c_str()) == FALSE)
        PError::SignalFatalError("Unable to Set Working Directory: " + path);
    //GetLastError should help more here.
#else
    int result = chdir(path.c_str());
   if(result != 0)
        {
            PError::SignalFatalError("Unable to Set Working Directory: " + path);
        }
#endif

 return Variant(true);
}



Variant PEBLUtility::LaunchFile(std::string file)
{

    int x;
    //cout << "Running launchfile\n";
#if defined(PEBL_WIN32)
    HINSTANCE hInst = ShellExecute(0,
                                   "open",                      // Operation to perform
                                   file.c_str(),                // document name???Application name
                                   NULL,                        // Additional parameters
                                   0,                           // Default directory
                                   SW_SHOW);
    //cout<< "LaunchFile:{" << file <<"}{"<< hInst << endl;

   if ((INT_PTR)hInst == SE_ERR_NOASSOC ||
       (INT_PTR)hInst == SE_ERR_ASSOCINCOMPLETE ||
       (INT_PTR)hInst == SE_ERR_ACCESSDENIED)
    {
      SHELLEXECUTEINFO exeInfo = {0};
      exeInfo.cbSize = sizeof(SHELLEXECUTEINFO);
      exeInfo.lpVerb = "openas";
      exeInfo.lpFile = file.c_str();
      exeInfo.fMask = SEE_MASK_INVOKEIDLIST;
      exeInfo.nShow = SW_SHOWDEFAULT;

      // Open up the open as window
      if (ShellExecuteEx(&exeInfo) == FALSE);
      {
        x = false ;
      }
    } else {

       x = ((INT_PTR)hInst > 32);  //hInst returns an error code > 32 on success.
     }
#elif defined(PEBL_LINUX)


    std::string call2 = "xdg-open " + file;
    int ret = system(call2.c_str());  //do a system call with the argument string.
    x = !ret;



#elif defined(PEBL_OSX)
     std::string call2 = "open " + file;
     int ret  = system(call2.c_str());  //do a s string.
     x = !ret;
#endif

 return Variant(x);
}


Variant PEBLUtility::SystemCall(std::string call, std::string args)
{
    //   cout << "Systemcalling inner\n";


#if defined( PEBL_UNIX )

    std::string tmp = call + " " + args;

    const char* call2 = tmp.c_str();
    int x = system(call2);  //do a system call with the argument string.

#elif defined (PEBL_WIN32)
    std::string tmp = "cmd.exe /c " + call + " " + args;

    //   cout << "["<<tmp <<"]"<< std::endl;
    STARTUPINFO info={sizeof(info)};
    PROCESS_INFORMATION processInfo;
    char* callstring = const_cast<char *>(tmp.c_str());

    if (CreateProcess(NULL,callstring, NULL, NULL, TRUE,
                      ABOVE_NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW,
                        NULL, NULL, &info, &processInfo))
//    if (CreateProcess(NULL,callstring, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
        {
            ::WaitForSingleObject(processInfo.hProcess, INFINITE);
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        } else {
        //        cout << "createprocess failed\n";
        std::cerr << GetLastError() << std::endl;
    }
    int x=0;
#elif defined (PEBL_EMSCRIPTEN)
    std::cerr << "Cannot perform system call in web mode\n";
    int x = 0;
#endif

    return Variant(x);

}



#ifdef PEBL_WINDOWS


PROCESS_INFORMATION PEBLUtility::SystemCallAndReturn(std::string call, std::string args)
{

    std::string tmp = "cmd.exe /c " + call + " " + args;

    STARTUPINFO info={sizeof(info)};
    PROCESS_INFORMATION processInfo;


    ZeroMemory( &info, sizeof(info) );
    info.cb = sizeof(info);
    ZeroMemory( &processInfo,sizeof(processInfo) );

    char* callstring = const_cast<char *>(tmp.c_str());

    if (CreateProcess(NULL,callstring, NULL, NULL, TRUE,
                      ABOVE_NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW,
                        NULL, NULL, &info, &processInfo))
//    if (CreateProcess(NULL,callstring, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
        {
            ::WaitForSingleObject(processInfo.hProcess, 10);

        } else {
        //        cout << "createprocess failed\n";
        std::cerr << GetLastError() << std::endl;
    }
    int x=0;

    return processInfo;

}

#endif
// unicode reverse functions adapted from
// http://stackoverflow.com/questions/198199/how-do-you-reverse-a-string-in-place-in-c-or-c
//

#define SWP(x,y) (x^=y, y^=x, x^=y)

void PEBLUtility::strrev(char *p)
{
  char *q = p;
  while(q && *q) ++q; /* find eos */
  for(--q; p < q; ++p, --q) SWP(*p, *q);
}


// The following is adapted from code put on
// http://stackoverflow.com/questions/1031645/how-to-detect-utf-8-in-plain-c
// It has an implied public domain license
//It probably can be replaced by something from the utf8 library we now include.

bool PEBLUtility::is_utf8(std::string str)
{


    if(str.length()==0)
        return false;

    const unsigned char * bytes = (const unsigned char *)(str.c_str());
    while(*bytes)
    {
        if(     (// ASCII
                        bytes[0] == 0x09 ||
                        bytes[0] == 0x0A ||
                        bytes[0] == 0x0D ||
                        (0x20 <= bytes[0] && bytes[0] <= 0x7E)
                )
        ) {
                bytes += 1;
                continue;
        }

        if(     (// non-overlong 2-byte
                        (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                        (0x80 <= bytes[1] && bytes[1] <= 0xBF)
                )
        ) {
                bytes += 2;
                continue;
        }

        if(     (// excluding overlongs
                        bytes[0] == 0xE0 &&
                        (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                ) ||
                (// straight 3-byte
                        ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
                                bytes[0] == 0xEE ||
                                bytes[0] == 0xEF) &&
                        (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                ) ||
                (// excluding surrogates
                        bytes[0] == 0xED &&
                        (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                )
        ) {
                bytes += 3;
                continue;
        }

        if(     (// planes 1-3
                        bytes[0] == 0xF0 &&
                        (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                        (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                ) ||
                (// planes 4-15
                        (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
                        (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                        (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                ) ||
                (// plane 16
                        bytes[0] == 0xF4 &&
                        (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                        (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                )
        ) {
                bytes += 4;
                continue;
        }

        return false;
    }

    return true;
}



void PEBLUtility::strrev_utf8(char *p)
{
  char *q = p;
  strrev(p); /* call base case */

  /* Ok, now fix bass-ackwards UTF chars. */
  while(q && *q) ++q; /* find eos */
  while(p < --q)
    switch( (*q & 0xF0) >> 4 ) {
    case 0xF: /* U+010000-U+10FFFF: four bytes. */
      SWP(*(q-0), *(q-3));
      SWP(*(q-1), *(q-2));
      q -= 3;
      break;
    case 0xE: /* U+000800-U+00FFFF: three bytes. */
      SWP(*(q-0), *(q-2));
      q -= 2;
      break;
    case 0xC: /* fall-through */
    case 0xD: /* U+000080-U+0007FF: two bytes. */
      SWP(*(q-0), *(q-1));
      q--;
      break;
    }
}
std::string PEBLUtility::strrev(std::string p)
{

    std::string q = std::string(p);
    std::reverse(q.begin(),q.end());
    return q;
}

std::string PEBLUtility::strrev_utf8(std::string p)
{


    std::wstring q(p.length(), L' '); // Make room for characters
    // Copy string to wstring.
    std::copy(p.begin(), p.end(), q.begin());

    // reverse:
    std::reverse(q.begin(),q.end());

    cout << "[";
    std::wstring::iterator i;

    for(i = q.begin();i<=q.end();i++)
        {
            cout << *i << "|";
        }
    cout << "]\n";

    std::string ns(q.begin(), q.end());
    cout << ns << std::endl;
    return ns;


//     std::string::iterator qq = q.end();

//   /* Ok, now fix backwards UTF chars. */

//         while(qq >= q.begin())
//             {

//                 switch( (*qq & 0xF0) >> 4 )
//                     {

//                     case 0xF: /* U+010000-U+10FFFF: four bytes. */
//                         std::swap(*qq,*(qq-3));
//                         std::swap(*(qq-1),*(qq-2));
//                         //q.replace(qq,qq-3,q,1);
//                         //q.replace(qq-1,qq-2,q,1);
//                         qq -= 3;
//                         break;

//                     case 0xE: /* U+000800-U+00FFFF: three bytes. */
//                         std::swap(*qq,*(qq-2));
//                         //q.replace(qq,qq-2,q,1);
//                         qq -= 2;
//                         break;

//                     case 0xC: /* fall-through */
//                     case 0xD: /* U+000080-U+0007FF: two bytes. */
//                         std::swap(*qq,*(qq-1));
//                         //q.replace(qq,qq-1,q,1);
//                         qq--;
//                         break;
//                     default:
//                         qq--;
//                     }
//             }
//     return q;
}

// Get the size of the file by its file descriptor
unsigned long get_size_by_fd(int fd) {
    struct stat statbuf;
    if(fstat(fd, &statbuf) < 0) exit(-1);
    return statbuf.st_size;
}

std::string PEBLUtility::MD5File(const std::string & filename)
{

    if(FileExists(filename))
        {

#ifdef PEBL_WIN32
            // Windows: use standard file I/O instead of mmap
            std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
            if (!file.is_open())
                {
                    PError::SignalFatalError("File does not exist in MD5file\n");
                    return "";
                }

            std::streamsize file_size = file.tellg();
            file.seekg(0, std::ios::beg);

            char* file_buffer = new char[file_size];
            if (!file.read(file_buffer, file_size))
                {
                    delete[] file_buffer;
                    PError::SignalFatalError("Failed to read file in MD5file\n");
                    return "";
                }
            file.close();

            MD5 *md5 = new MD5();
            md5->update((unsigned char*)file_buffer,(unsigned int)file_size);
            md5->finalize();
            std::string hex = md5->hexdigest();
            delete md5;
            delete[] file_buffer;
            md5=NULL;
            return hex;
#else
            // Unix/Linux: use mmap
            int file_descript;
            unsigned long int file_size;
            char* file_buffer;

            //printf("using file:\t%s\n", filename);

            file_descript = open(filename.c_str(), O_RDONLY);
            if(file_descript < 0)
                {
                    PError::SignalFatalError("File does not exist in MD5file\n");
                    return "";
                }

            file_size = get_size_by_fd(file_descript);
            //printf("file size:\t%lu\n", file_size);

            file_buffer = (char*)mmap(0, file_size, PROT_READ, MAP_SHARED,
                               file_descript, 0);


            MD5 *md5 = new MD5();
            md5->update((unsigned char*)file_buffer,(unsigned int)file_size);
            //std::string result =  md5((unsigned char*) file_buffer);
            md5->finalize();
            std::string hex = md5->hexdigest();
            delete md5;
            md5=NULL;
            return hex;
#endif
        }else{

        return "0";
    }

}


std::string PEBLUtility::MD5String(const std::string & text)
{
    return md5(text);
}


void PEBLUtility::CopyToClipboard(const std::string & text)
{

}



//This handles the current token/tokens. That is, if the token is a list or
//an object, it handles the entire subsequent set, recursively.

Variant PEBLUtility::ExtractJSONObject(const std::string & text,int remaining,
                                       jsmntok_t **t, int start, int end)
{

    //A primitive is number, a boolean, or NULL.
	if ((*t)->type == JSMN_PRIMITIVE) {
        //cout << "->->->->->->->->->->->->->->->->->-> Parsing primitive\n";
        //cout << "t1: " << (*t)->start << " to " << (*t)->end << " size: "<< (*t)->size << endl;
        //cout << "Primitive: " << (*t)->start << "|" << (*t)->end << "---["
        //<< text.substr((*t)->start,(*t)->end-(*t)->start) <<"]"<< endl;


        
        std::string label = text.substr((*t)->start,(*t)->end - (*t)->start);
        Variant out = 0;

        if(label[0]=='t') //true; this sholudn't be capitalized.
            {
                out = 1;
            }else if(label[0]=='f' || label[0] == 'N')
            {
                //PEBL has no NULL (except empty list), so
                //treat these as 0.  We may need to handl null in an other way.
                out= 0;
            }else{
            out = Variant(PEBLUtility::StringToPDouble(label.c_str()));
        }

        (*t)++;

        //We may need to convert to a number here too.
        return out;


	} else if ((*t)->type == JSMN_STRING) {
        //cout << "->>->->->->->->->->  Parsing string\n";
        //cout << "t2: " << (*t)->start << " to " << (*t)->end << " size: "<< (*t)->size << endl;
        //cout << "String: " << (*t)->start << "|" << ((*t)->end-(*t)->start) << "---[" << text.substr((*t)->start,(*t)->end - (*t)->start) <<"]"<< endl;
        
        Variant out = Variant(text.substr((*t)->start,(*t)->end-(*t)->start));
        (*t)++;  //Increment to next token so all paths do this the same.
        return out;
        

	} else if ((*t)->type == JSMN_OBJECT) {
        //        cout << "------------------->>>_>_>_>_>_>_>_>_Parsing object\n";
        //        cout << "<<<" << text.substr((*t)->start,(*t)->end-(*t)->start) <<">>>"<< endl;

        //Get the top-level object:
        std::string name = "JSON Object";
        PCustomObject *  pobj = new  PCustomObject(name);
        //We need an (inconspicuous) element added to any JSON object called ELEMENTORDER,
        //that will keep track of the order of the element names.  JSON technically has
        //no restriction on the input order, but this will make editing the files more convenient.
        //as a round trip read-write  should maintain order.
        //This needs to be a plist of variants.
        PList * elementOrder = new PList();

        remaining = (*t)->size * 2;//this is how many sub-objects the current object has.
                                   //per high-level entry.
        //cout <<  "Remaining: " << remaining << endl;
        //Advance to the first subtoken:
        (*t)++;
        //We need to iterate through all sub-elements of the object
        while(remaining > 0)
            {
                //cout << ">>>>>>>>>>>>>>>>>.Remaining1:" << remaining << endl;
                //cout << "t3: " << (*t)->start << " to " << (*t)->end << " size: " << (*t)->size << endl;
                
                std::string objname = text.substr((*t)->start,(*t)->end-(*t)->start);
                //cout << "Naame:" << objname << endl;
                elementOrder->PushBack(objname);//add name to end of elementorder object.


                (*t)++;
                remaining-=1;

                //cout << ">>>>>>>>>>>>>>>>>.Remaining2 :" << remaining << endl;
                //cout << "t5: " << (*t)->start << " to " << (*t)->end << " size: " << (*t)->size << endl;


                
                //Call recursively. This should increment t to the next token:
                Variant objValue =PEBLUtility::ExtractJSONObject(text,
                                                                 (*t)->size-1,
                                                                 t,(*t)->start,(*t)->end);

                //cout << "New vaule of t: " << (*t)->start << " to " << (*t)->end << "\n";
                //std::cout <<  "Setting[" << objname  <<  "-->" << objValue << std::endl;
                pobj->SetProperty((objname),objValue);
                remaining--;
                //cout << "t6: " << (*t)->start << " to " << (*t)->end << " size: " <<(*t)->size << endl;
            }


        counted_ptr<PEBLObjectBase> tmpObject = counted_ptr<PEBLObjectBase>(pobj);
        PComplexData *  pcd = new PComplexData(tmpObject);
        Variant tmp = Variant(pcd);
        delete pcd;
        pcd=NULL;
        //cout << "<_<_<_<_<_<_<_<_<_<_<_<_<_<_<_<_<DONE PARSING OBJECT\n";


        return tmp;


	} else if ((*t)->type == JSMN_ARRAY) {
        // cout << "-------------->->->->->->_>->->-Parsing ARRAY\n";
        remaining = (*t)->size ;
        //cout <<  "Remaining: " << remaining << endl;


        //cout << "t7: " << (*t)->start << " to " << (*t)->end << " size: "<< (*t)->size ;
        //cout << "---> <<<"<<  Variant(text.substr((*t)->start,(*t)->end-(*t)->start))<<">>>\n";


        PList * plist = new PList();
        //We need to iterate through all sub-elements of the array.
        (*t)++;
        while(remaining > 0)
            {
                //cout << " Array loop: remaining: " << remaining << endl;
                //cout << "t8: " << (*t)->start << " to " << (*t)->end << " size: "<< (*t)->size << endl;

                //This increments (*t) automatically:
                Variant objValue = PEBLUtility::ExtractJSONObject(text,
                                                                  remaining,
                                                                  t, (*t)->start,(*t)->end);
                //cout << "t8AFTER: " << (*t)->start << " to " << (*t)->end << " size: "<< (*t)->size << endl;
                //cout << "Adding: " << objValue << endl;
                plist->PushBack(objValue);
                //cout << "not incrementing t: \n"  ;
                
                remaining -= 1;  //array size is in objects, not flattened tokens.
            }


        counted_ptr<PEBLObjectBase> tmpObject = counted_ptr<PEBLObjectBase>(plist);
        PComplexData *  pcd = new PComplexData(tmpObject);
        Variant tmp = Variant(pcd);
        delete pcd;
        pcd=NULL;
        //cout << "<<<<-----------------------DONE Parsing ARRAY\n";
        return tmp;

	}
    return Variant(int(0));
}



Variant PEBLUtility::ParseJSON(const std::string &text)
{
	jsmn_parser p;

    jsmntok_t  * t;
    t= (jsmntok_t*) malloc(sizeof(*t)*(size_t)1000); /* We expect no more than 1000 tokens */
                                                     /*There are ways to expand this, if necessary*/
    jsmntok_t ** tt = &t;
    int r;
        std::cout <<"Parsing:" << text << endl;

	jsmn_init(&p);
	r = jsmn_parse(&p, text.c_str(), text.length(),*tt,1000);
    //cout << "number of tokens:" << r << endl;
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}
    //Now, t contains a bunch of parsed tokens.

    /* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
   }

    //skip past the first token, and extract the rest (r-1 remaining; token 1)
    Variant out = ExtractJSONObject(text, r-1,tt,0,text.length());
    return out;
}

// Detect script from UTF-8 text content
// Returns ISO 15924 4-letter script code, or empty string for Latin/unknown
// Used to enable proper text rendering for complex scripts
std::string PEBLUtility::DetectScript(const std::string & text) {
    if (!is_utf8(text)) {
        return "";  // Can't reliably detect in non-UTF8
    }

    // Walk through UTF-8 string looking for script-specific codepoints
    const unsigned char *bytes = (const unsigned char *)text.c_str();
    while (*bytes) {
        // Hebrew: U+0590 to U+05FF (0xD6 0x90 to 0xD7 0xBF in UTF-8)
        if ((bytes[0] == 0xD6 && bytes[1] >= 0x90) ||
            (bytes[0] == 0xD7 && bytes[1] <= 0xBF)) {
            return "Hebr";
        }

        // Arabic: U+0600 to U+06FF (0xD8 0x80 to 0xDB 0xBF in UTF-8)
        if (bytes[0] >= 0xD8 && bytes[0] <= 0xDB) {
            return "Arab";
        }

        // Devanagari: U+0900 to U+097F (0xE0 0xA4 0x80 to 0xE0 0xA5 0xBF)
        if (bytes[0] == 0xE0 && bytes[1] == 0xA4) {
            return "Deva";
        }

        // Thai: U+0E00 to U+0E7F (0xE0 0xB8 0x80 to 0xE0 0xB9 0xBF)
        if (bytes[0] == 0xE0 && bytes[1] == 0xB8) {
            return "Thai";
        }

        // Bengali: U+0980 to U+09FF (0xE0 0xA6 0x80 to 0xE0 0xA7 0xBF)
        if (bytes[0] == 0xE0 && bytes[1] == 0xA6) {
            return "Beng";
        }

        // Georgian: U+10A0 to U+10FF (0xE1 0x82 0xA0 to 0xE1 0x83 0xBF)
        if (bytes[0] == 0xE1 && (bytes[1] == 0x82 || bytes[1] == 0x83)) {
            return "Geor";
        }

        // Hiragana: U+3040 to U+309F (0xE3 0x81 0x80 to 0xE3 0x82 0x9F)
        if (bytes[0] == 0xE3 && (bytes[1] == 0x81 || bytes[1] == 0x82)) {
            return "Hira";  // Japanese Hiragana
        }

        // Katakana: U+30A0 to U+30FF (0xE3 0x82 0xA0 to 0xE3 0x83 0xBF)
        if (bytes[0] == 0xE3 && bytes[1] == 0x83) {
            return "Kana";  // Japanese Katakana
        }

        // Hangul Syllables: U+AC00 to U+D7AF (0xEA 0xB0 0x80 to 0xED 0x9E 0xAF)
        if (bytes[0] >= 0xEA && bytes[0] <= 0xED) {
            return "Hang";  // Korean Hangul
        }

        // CJK Unified Ideographs: U+4E00 to U+9FFF
        // (0xE4 0xB8 0x80 to 0xE9 0xBF 0xBF)
        if (bytes[0] >= 0xE4 && bytes[0] <= 0xE9) {
            return "Hani";  // Han (Chinese/Japanese/Korean)
        }

        // Skip to next codepoint
        if (bytes[0] < 0x80) bytes += 1;
        else if ((bytes[0] & 0xE0) == 0xC0) bytes += 2;
        else if ((bytes[0] & 0xF0) == 0xE0) bytes += 3;
        else if ((bytes[0] & 0xF8) == 0xF0) bytes += 4;
        else bytes += 1;  // Invalid UTF-8, skip
    }

    return "";  // Default: Latin/unknown, no special script needed
}

// Determine if script is right-to-left
bool PEBLUtility::IsRTLScript(const std::string & script) {
    return (script == "Hebr" || script == "Arab");
}

// Get appropriate font for a language code (2-letter ISO 639-1) or script code (4-letter ISO 15924)
// fontType: 0=sans-serif, 1=monospace, 2=serif
// Returns font filename appropriate for the language/script
std::string PEBLUtility::GetFontForLanguageOrScript(const std::string & code, int fontType) {
    std::string upperCode = code;
    std::transform(upperCode.begin(), upperCode.end(), upperCode.begin(), ::toupper);

    // Default fonts (Western scripts - Latin, Cyrillic, Greek)
    std::string defaultSans = "DejaVuSans.ttf";
    std::string defaultMono = "DejaVuSansMono.ttf";
    std::string defaultSerif = "DejaVuSerif.ttf";

    // CJK fonts
    std::string cjkSans = "NotoSansCJK-Regular.ttc";
    std::string cjkMono = "NotoSansMono-Regular.ttf";
    std::string cjkSerif = "NotoSerif-Regular.ttf";

    // Check if it's a 2-letter language code or 4-letter script code
    if (upperCode.length() == 2) {
        // Language code - map to appropriate font
        if (upperCode == "AR") {
            // Arabic - DejaVu has Arabic + Latin support
            return (fontType == 0) ? defaultSans : (fontType == 1) ? defaultMono : defaultSerif;
        }
        else if (upperCode == "HE" || upperCode == "IW") {
            // Hebrew - DejaVu has Hebrew + Latin support
            return (fontType == 0) ? defaultSans : (fontType == 1) ? defaultMono : defaultSerif;
        }
        else if (upperCode == "TH") {
            // Thai
            return (fontType == 0) ? "NotoSansThai-Regular.ttf" :
                   (fontType == 1) ? cjkMono : cjkSerif;
        }
        else if (upperCode == "HI" || upperCode == "MR" || upperCode == "NE") {
            // Devanagari (Hindi, Marathi, Nepali)
            return (fontType == 0) ? "NotoSansDevanagari-Regular.ttf" :
                   (fontType == 1) ? cjkMono : cjkSerif;
        }
        else if (upperCode == "BN") {
            // Bengali
            return (fontType == 0) ? "NotoSansBengali-Regular.ttf" :
                   (fontType == 1) ? cjkMono : cjkSerif;
        }
        else if (upperCode == "KA") {
            // Georgian - DejaVu has Georgian + Latin support
            return (fontType == 0) ? defaultSans : (fontType == 1) ? defaultMono : defaultSerif;
        }
        else if (upperCode == "ZH" || upperCode == "CN" || upperCode == "TW") {
            // Chinese
            return (fontType == 0) ? cjkSans : (fontType == 1) ? cjkMono : cjkSerif;
        }
        else if (upperCode == "JA" || upperCode == "JP") {
            // Japanese
            return (fontType == 0) ? cjkSans : (fontType == 1) ? cjkMono : cjkSerif;
        }
        else if (upperCode == "KO" || upperCode == "KR" || upperCode == "KP") {
            // Korean
            return (fontType == 0) ? cjkSans : (fontType == 1) ? cjkMono : cjkSerif;
        }
        else {
            // Default to Western fonts for unrecognized language codes
            return (fontType == 0) ? defaultSans : (fontType == 1) ? defaultMono : defaultSerif;
        }
    }
    else if (upperCode.length() == 4) {
        // Script code (ISO 15924) - map to appropriate font
        if (upperCode == "ARAB") {
            // Arabic script
            return (fontType == 0) ? defaultSans : (fontType == 1) ? defaultMono : defaultSerif;
        }
        else if (upperCode == "HEBR") {
            // Hebrew script
            return (fontType == 0) ? defaultSans : (fontType == 1) ? defaultMono : defaultSerif;
        }
        else if (upperCode == "THAI") {
            // Thai script
            return (fontType == 0) ? "NotoSansThai-Regular.ttf" :
                   (fontType == 1) ? cjkMono : cjkSerif;
        }
        else if (upperCode == "DEVA") {
            // Devanagari script
            return (fontType == 0) ? "NotoSansDevanagari-Regular.ttf" :
                   (fontType == 1) ? cjkMono : cjkSerif;
        }
        else if (upperCode == "BENG") {
            // Bengali script
            return (fontType == 0) ? "NotoSansBengali-Regular.ttf" :
                   (fontType == 1) ? cjkMono : cjkSerif;
        }
        else if (upperCode == "GEOR") {
            // Georgian script
            return (fontType == 0) ? defaultSans : (fontType == 1) ? defaultMono : defaultSerif;
        }
        else if (upperCode == "HIRA" || upperCode == "KANA" || upperCode == "HANI") {
            // Japanese scripts (Hiragana, Katakana, Han/Kanji)
            return (fontType == 0) ? cjkSans : (fontType == 1) ? cjkMono : cjkSerif;
        }
        else if (upperCode == "HANG") {
            // Korean Hangul script
            return (fontType == 0) ? cjkSans : (fontType == 1) ? cjkMono : cjkSerif;
        }
        else {
            // Default to Western fonts (Latin and unknown scripts)
            return (fontType == 0) ? defaultSans : (fontType == 1) ? defaultMono : defaultSerif;
        }
    }
    else {
        // Invalid code length - return default
        return (fontType == 0) ? defaultSans : (fontType == 1) ? defaultMono : defaultSerif;
    }
}

// Get the system locale from OS settings
// Uses SDL_GetPreferredLocales() to query OS for user's preferred language/locale
// Returns locale string like "ar", "en_US", "zh_CN", "he_IL"
// Returns empty string on error
std::string PEBLUtility::GetSystemLocale() {
    SDL_Locale *locales = SDL_GetPreferredLocales();
    if (!locales) {
        return "";  // Error or not supported on this platform
    }

    // Get the first (primary) locale
    std::string result = "";
    if (locales[0].language) {
        result = locales[0].language;

        // Append country code if available (e.g., "en_US", "zh_CN")
        if (locales[0].country) {
            result += "_";
            result += locales[0].country;
        }
    }

    SDL_free(locales);
    return result;  // e.g., "ar", "en_US", "zh_CN", "he_IL", "ko_KR"
}

// Check if the system locale is RTL (Arabic, Hebrew)
// Useful for setting default text box justification before any text input
bool PEBLUtility::IsSystemLocaleRTL() {
    std::string locale = GetSystemLocale();
    if (locale.empty()) {
        return false;  // Default to LTR if we can't detect
    }

    // Extract language code (first 2 characters)
    std::string langCode = locale.substr(0, 2);
    std::transform(langCode.begin(), langCode.end(), langCode.begin(), ::tolower);

    // Check if it's Arabic or Hebrew
    return (langCode == "ar" || langCode == "he" || langCode == "iw");
}
