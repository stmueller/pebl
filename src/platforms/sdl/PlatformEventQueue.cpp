//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformEventQueue.cpp
//    Purpose:    Interface to platform-specific event queue.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2004-2025 Shane T. Mueller <smueller@obereed.net>
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

#include "PlatformEventQueue.h"
#include "PlatformEnvironment.h"
#include "../../devices/PEventQueue.h"
#include "../../devices/PEvent.h"
#include "../../utility/PError.h"
#include "SDL.h"
#include "SDL_mouse.h"
#ifdef PEBL_MOVIES
#include "WAAVE.h"
#endif

#include <cstring>
#include "../../apps/Globals.h"
using std::cout;
using std::endl;

extern Evaluator * myEval;
extern PlatformEnvironment * myEnv;


//This should be moved into PEventQueue
PlatformEventQueue::PlatformEventQueue()
{
}

PlatformEventQueue::~PlatformEventQueue()
{
}



/// The Prime method takes any all events waiting in the SDL
/// event queue, converts them to PEvents, stamps them with a timestamp,
/// and puts them into the platform-generic event queue.
void PlatformEventQueue::Prime()
{


	SDL_Event test_event;
    unsigned long int time = SDL_GetTicks();

    if(SDL_HasEvents(SDL_MOUSEBUTTONDOWN,SDL_MOUSEBUTTONUP))
        {
            // cout <<"\n#*#*#*#*#*#*#*#*#*#*#*#*#*#Mouse click/unclick\n";
            
        }
    std::string error  = SDL_GetError();
    if(error!="")
        {
            //  cout <<"SDL Error: "<< error << endl;
            SDL_ClearError();

        }
	//Get the next event in the queue. SDL_PollEvent returns 0
    //when there are no pending events available.

    while(SDL_PollEvent(&test_event))
    //while(SDL_WaitEvent(&test_event))
    
        {

            if(((int)(test_event.type))==1024)
                {
                    //cout << "." << std::flush;
                }else{
                
                //cout << "-------------------------\n";
                //cout << "EVented\n";
                //cout << "Event [" << (int)(test_event.type) << "]\n";
                }

            #if 0
            
            cout << "event types: " << endl
                 << "SDL_KEYDOWN:         " << SDL_KEYDOWN << endl
                 << "SDL_KEYUP:           " << SDL_KEYUP <<endl
                 << "SDL_TEXTEDITING:     " << SDL_TEXTEDITING << endl
                 << "SDL_TEXTINPUT:       " << SDL_TEXTINPUT << endl
                 << "SDL_MOUSEMOTION:     "  << SDL_MOUSEMOTION << endl
                 << "SDL_MOUSEBUTTON.down:" << SDL_MOUSEBUTTONDOWN << endl
                 << "SDL_MOUSEBUTTON.up:  " << SDL_MOUSEBUTTONUP << endl
                 << "SDL_MOUSEWHEEL:      "  << SDL_MOUSEWHEEL << endl;
            
            #endif
            
            //first, check to see if the event is the 'magic' abort event:
            //ctrl-alt-shift-X.

            if(test_event.type == SDL_KEYDOWN)
                {

                    if(test_event.key.keysym.sym == SDLK_BACKSLASH)
                        {
                            if((test_event.key.keysym.mod & KMOD_SHIFT )
                               && (test_event.key.keysym.mod & KMOD_ALT)
                               && (test_event.key.keysym.mod & KMOD_CTRL))
                                {
                                    PError::ExitQuietly("Stopping execution: Received abort key-combo.\n");
                                }
                        }

                }

            //Create a PEBL event from the SDL event, if possible.

            switch(test_event.type)
                {


                case SDL_KEYDOWN:
                    {


                        //Determine which key is being used.

                        PEvent evt(PDT_KEYBOARD, time,test_event.key.windowID);
                        PEBL_KeyboardEvent pke;
                        //cout <<  "Keydown event...PRESSED->[" << SDL_GetKeyName(test_event.key.keysym.sym)<<"] \n";
                        //cout << "Keycode:" <<SDL_SCANCODE_TO_KEYCODE(test_event.key.keysym.scancode) << endl;

                        pke.key =      (PEBL_Keycode)test_event.key.keysym.sym;
                        pke.scancode = (PEBL_Keycode)(test_event.key.keysym.scancode);
                        pke.modkeys = test_event.key.keysym.mod;
                        pke.state = PEBL_PRESSED;


                        //cout << "keystate in platformeventqueue is: " << pke.state << "|" << pke.key << endl;

                        evt.SetKeyboardEvent(pke);
                        mEventQueue.push(evt);

                    }
                    break;

                case SDL_KEYUP:
                    {
                        //  cout << "KEYUP\n";
                        //create a new event to add to the queue.
                        PEvent evt(PDT_KEYBOARD, time,test_event.key.windowID);

                        //The event has a keyboard event inside it.
                        PEBL_KeyboardEvent pke;
                        //pke.key = (PEBLKey)(test_event.key.keysym.sym);

                        pke.key =      (PEBL_Keycode)test_event.key.keysym.sym;
                        pke.scancode = (PEBL_Keycode)(test_event.key.keysym.scancode);
                        pke.modkeys = test_event.key.keysym.mod;

                        pke.state = PEBL_RELEASED;


                        evt.SetKeyboardEvent(pke);
                        mEventQueue.push(evt);

                    }

                    break;

                    //Text editing event...in-progress textinput event.
                case SDL_TEXTEDITING:

                    {
                        //cout << "SDL_TEXTEDITING EVENT\n";
                        /*

                        PEvent evt(PDT_TEXT_EDITING, time);
                        PEBL_TextEditingEvent pee;


                        pee.text =test_event.edit.text;
                        pee.window = 0;

                        evt.SetTextEditingEvent(pee);
                        mEventQueue.push(evt);
                        */
                    }

                    break;

                    //This is a textinput event, which is new
                    //for SDL2.  It handle input from a variety of modes
                    //and generally let's us support international
                    //character inputs.
                case SDL_TEXTINPUT:
                    {

                        //cout << "SDL_TEXTINPUT EVENT\n";
                        PEvent evt(PDT_TEXT_INPUT, time,test_event.edit.windowID);
                        PEBL_TextInputEvent pti;
                        //cout << "Transferring: " << test_event.edit.text << endl;

                        pti.text = strdup(test_event.edit.text);

                        pti.window = 0;
                        pti.start = test_event.edit.start;
                        pti.length = test_event.edit.length;
                        evt.SetTextInputEvent(pti);
                        mEventQueue.push(evt);

                    }

                    break;
                case SDL_QUIT:
                    PError::ExitQuietly("Stopping execution because of quit signal.\n");
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    {
                        //cout << "Mouse button down in platformeventqueue\n";
                        PEvent evt(PDT_MOUSE_BUTTON, time,test_event.button.windowID);
                        PEBL_MouseButtonEvent pme;

                        pme.x= test_event.button.x;
                        pme.y= test_event.button.y;
                        pme.button=test_event.button.button;
                        pme.state =test_event.button.state;


                        pme.state = PEBL_PRESSED;

                        evt.SetMouseButtonEvent(pme);
                        mEventQueue.push(evt);
                        //cout << "mouse down ["<< pme.x << " " << pme.y <<  "--" << pme.button<<"]\n";

                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    {
                        //cout << "Mouse button up in platformeventqueue\n";
                        PEvent evt(PDT_MOUSE_BUTTON, time,test_event.button.windowID);
                        PEBL_MouseButtonEvent pme;

                        pme.x= test_event.button.x;
                        pme.y= test_event.button.y;
                        pme.button=test_event.button.button;
                        pme.state =test_event.button.state;

                        pme.state = PEBL_RELEASED;

                        evt.SetMouseButtonEvent(pme);
                        mEventQueue.push(evt);

                    }
                    break;
                case SDL_MOUSEMOTION:
                    {
                        PEvent evt(PDT_MOUSE_MOVEMENT, time,test_event.motion.windowID);
                        PEBL_MouseMovementEvent pme;
                        //cout << "mousemove\n";
                        pme.x= test_event.motion.x;
                        pme.y= test_event.motion.y;
                        pme.relx =test_event.motion.xrel;
                        pme.rely =test_event.motion.yrel;


                        evt.SetMouseMovementEvent(pme);
                        mEventQueue.push(evt);

                    }
                    break;

                    case SDL_MOUSEWHEEL:
                    {

                        PEvent evt(PDT_MOUSE_WHEEL,time,test_event.wheel.windowID);
                        PEBL_MouseWheelEvent pmwe;

                        int xpos,ypos;
                        //get the mouse position
                        SDL_GetMouseState(&xpos,&ypos);


                        pmwe.x = xpos;
                        pmwe.y = ypos;

                        pmwe.deltax = test_event.wheel.x;
                        pmwe.deltay = test_event.wheel.y;


                        //this may not work on SDL 2.0 sligthly older libraries, or maybe not o n windows.
#ifdef SDL_MOUSEWHEEL_FLIPPED
                        if(test_event.wheel.direction ==  SDL_MOUSEWHEEL_FLIPPED  )
                            pmwe.direction = -1;
                        else
                            pmwe.direction = 1;
#else
                     pmwe.direction = 1;
#endif

                        evt.SetMouseWheelEvent(pmwe);
                        mEventQueue.push(evt);
                    }
                    break;

 //catch-all for various window resize/focus issues.  This is used to address
   //failure to redraw launcher when window minimized then replaced after running a script.
                case SDL_WINDOWEVENT:
                    {
                        switch(test_event.window.event)
                            {
                            case SDL_WINDOWEVENT_RESIZED:
                            case SDL_WINDOWEVENT_SIZE_CHANGED:
                            case SDL_WINDOWEVENT_RESTORED:
                            case SDL_WINDOWEVENT_EXPOSED:
                            case SDL_WINDOWEVENT_SHOWN:
                            case SDL_WINDOWEVENT_HIDDEN:

                                //we want the environment to issue a 'draw' command here.
                                //we need access to the environmnet or the renderer.
                                //PlatformEnvironment::Draw();
                                if(myEnv)
                                    {
                                        myEnv->Draw();
                                        //cout << "My window drawing stuff\n";
                                    }
                                break;
                            }

#ifdef SDL2_DELETE

                        PEvent evt(PDT_WINDOW_RESIZE,time,test_event.resize.windowID);


                        PEBL_WindowEvent pwe;

                        pwe.w= test_event.resize.w;
                        pwe.h= test_event.resize.h;
                        evt.SetWindowEvent(pwe);
                        mEventQueue.push(evt);


                        //Change the video size.
                        myEval->gGlobalVariableMap.AddVariable("gVideoWidth", pwe.w);
                        myEval->gGlobalVariableMap.AddVariable("gVideoHeight", pwe.h);

#endif

                    }
                    break;

                case SDL_RENDER_TARGETS_RESET:
               // case SDL_RENDER_DEVICE_RESET:
                    {
                        PError::SignalWarning("Rendering targets reset error.");
                    }
                default:
#ifdef PEBL_MOVIES
                    //WV_REFRESH_EVENT is not const, so we need to do some acrobatics here.
                    if(test_event.type == WV_REFRESH_EVENT)
                    {
                        //cout <<" XXXXX Refresh movie event\n";
                        //cout << test_event << endl;
                        //PEvent evt(PDT_MOVIE_REFRESH,time);
                        WV_refreshVideoFrame(&test_event);




                    }else if(test_event.type==WV_EOF_EVENT)
                    {

                        //cout << "End of movie:\n";
                        PEvent evt(PDT_MOVIE_END, time, test_event.key.windowID);
                        mEventQueue.push(evt);


                    }
                    break;
#endif


                    //cout << "Unknown event\n";
                    ;
                }
            //cout << "Loop active\n";
            time =  SDL_GetTicks();
            //cout << "time: " << time << endl;
        }
}
