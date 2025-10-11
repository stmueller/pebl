//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       AudioOutTest.cpp
//    Purpose:    Tests the PAudioOut and PlatformAudioOut Classes
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003 Shane T. Mueller <smueller@umich.edu>
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
#include "../devices/PAudioOut.h"
#include "../platforms/sdl/PlatformAudioOut.h"
#include "../base/Loader.h"
#include "../base/PComplexData.h"
#include "../base/PList.h"
#include "../base/FunctionMap.h"
#include "../base/VariableMap.h"
#include "../base/Variant.h"
#include "../base/Evaluator.h"
#include "../devices/PEventLoop.h"

#include "SDL/SDL.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

///Initiate some static member data.
FunctionMap Evaluator::mFunctionMap;
PEventLoop Evaluator::mEventLoop; 
VariableMap Evaluator::gGlobalVariableMap;
const PNode * Evaluator::gEvalNode = NULL;
PEBLPath  Evaluator::gPath;

//The following is the entry point for the command-line version

int main(int argc,  char *argv[])
{

    cout << "\n\n\n==========================================================\n";
    cout << "Testing the PAudioOut class\n";
    cout << "==========================================================\n";

     //Set up the search path.
    std::list<char*> tmp;
    tmp.push_back("./");
    Evaluator::gPath.Initialize(tmp);

       /* Load the SDL library */
    if ( SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0 ) {
      fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
      exit(1);
    }

    atexit(SDL_Quit);

   cout << "Audio Initialized: " << SDL_WasInit(SDL_INIT_AUDIO)  << "--" << SDL_INIT_AUDIO << endl;    


    cout << "Constructing Audio\n";
    PlatformAudioOut * myAudioOut = new PlatformAudioOut();
    PlatformAudioOut * myAudioOut2 = new PlatformAudioOut();

    myAudioOut->LoadSoundFile("buzz500ms.wav");
  

    myAudioOut->PlayForeground();
    cout << "Playing\n";
    SDL_Delay(500);
    myAudioOut->PlayForeground();
    cout << "Playing\n";
    

    char name[32];
    printf("Using audio driver: %s\n", SDL_AudioDriverName(name, 32));



 

 

    cout << "Finished" << endl;
    return 0;
}

