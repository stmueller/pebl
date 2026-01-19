//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformWindow.cpp
//    Purpose:    Contains SDL-specific interface for the main window class.
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
#include "PlatformWindow.h"
#include "PlatformEnvironment.h"

#include "../../objects/PWindow.h"
#include "../../objects/PColor.h"
#include "../../base/Evaluator.h"
#include "../../base/VariableMap.h"
#include "../../base/PComplexData.h"
#include "../../libs/PEBLEnvironment.h"
#include "SDLUtility.h"

#ifdef PEBL_EMSCRIPTEN
#include "emscripten.h"
#endif



#include "SDL.h"
#include <stdio.h>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::list;
using std::ostream;


///Standard Constructor
PlatformWindow::PlatformWindow(PlatformEnvironment * penv):
    mFlags(0),
    mWindow(NULL),
    PWindow(penv)

{
    mCDT=CDT_WINDOW;
    //    InitializeProperty("COLOR",Variant("grey"));//dummy property setting
    //    PWidget::SetBackgroundColor(PColor(200,200,200,255));
    PColor tmpColor = PColor(0,0,0,255);
    PWidget::SetBackgroundColor(tmpColor);

    //These don't get used for windows:
    mSurface = NULL;
    mTexture = NULL;
    mRenderer = NULL;
}

///Standard Destructor
PlatformWindow::~PlatformWindow()
{
 

    SDL_DestroyWindow(mWindow);
    mWindow = NULL;
 
}

// Inheritable function that is called by friend method << operator of PComplexData
ostream & PlatformWindow::SendToStream(ostream& out) const
{
    out << "<SDL PlatformWindow>" << flush;
    return out;
}

//Hopefully this won't be needed in SDL2
int PlatformWindow::GetVideoFlags()
{
    return 0;

}




///This method initiates everything needed to display the main window
bool PlatformWindow::Initialize(PEBLVideoMode mode,
                                PEBLVideoDepth vdepth,
                                bool windowed,
                                bool resizeable,
                                unsigned int width,
                                unsigned int height)
{

    cout << "Initializing " << width << "--" << height << endl;
    SDLUtility::GetDriverList(); //print out driver information.
    Variant v = 0;

    Evaluator * myEval = new Evaluator(v,"Window");
    //gVideoWidth and gVideoHeight may have been set by the user
    //in the script.  First, get these values, and try them.

    Variant vDp =  0;
    bool vsync = true;
    bool softrender = false;




    if(myEval->gGlobalVariableMap.Exists("gVideoDepth"))
        vDp =   myEval->gGlobalVariableMap.RetrieveValue("gVideoDepth");


    if(myEval->gGlobalVariableMap.Exists("gVSync"))
        {
            vsync = (int)((myEval->gGlobalVariableMap.RetrieveValue("gVsync"))) != 0;

        }
    //cout <<" vsync state: " << vsync <<"\n";

    if(myEval->gGlobalVariableMap.Exists("gSoftRender"))
        {
            softrender = (int)((myEval->gGlobalVariableMap.RetrieveValue("gSoftRender"))) != 0;
        }




    //Should be direct3d (windows), opengl, opengles2 opengles,software.
    if(myEval->gGlobalVariableMap.Exists("gDriverHint"))
        {
            SDL_SetHint("SDL_HINT_RENDER_DRIVER",
                        ( myEval->gGlobalVariableMap.RetrieveValue("gDriverHint").GetString().c_str()));
        }


    //some other hints available via SDL
    //SDL_SetHint("SDL_HINT_VIDEO_ALLOW_SCREENSAVER","0");
    if(!windowed)
        {
            //this might not be the best approach.
            SDL_SetHint("SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES","1");
        }


   if(vsync)
   {
       SDL_SetHint("SDL_HINT_RENDER_VSYNC","1");
   }


   SDL_SetHint("SDL_HINT_MOUSE_RELATIVE_MODE_WARP","1");

    //Set the renderer flags.  We are by default accelerated.
    //there we could bump up to vsync, or down to software.
    //If you try to specify both vsync and software, it will just use software.

    int rflags = SDL_RENDERER_TARGETTEXTURE;
    if(softrender)
        {
            rflags |=  SDL_RENDERER_SOFTWARE;
        }else
        {

            rflags |= SDL_RENDERER_ACCELERATED;

#ifndef PEBL_EMSCRIPTEN
            if(vsync)
                {
                    rflags |= SDL_RENDERER_PRESENTVSYNC;

                }
#endif
        }


    int depth  = vDp;

    //    cout << width << "----" << height << " in platformwindow\n";



    if(!(depth > 0))
        depth = (int)vdepth;



    int vflags = 0;//GetVideoFlags();

    if(!windowed)
        {
            vflags |=  SDL_WINDOW_FULLSCREEN ;       // Enable fullscreen
        }

    if(resizeable)
        {
            vflags |= SDL_WINDOW_RESIZABLE;         //IS the window resizeable?
        }

    //Re-store the values
    myEval->gGlobalVariableMap.AddVariable("gVideoWidth", (long unsigned int)width);
    myEval->gGlobalVariableMap.AddVariable("gVideoHeight", (long unsigned int)height);
    myEval->gGlobalVariableMap.AddVariable("gVideoDepth", depth);

    PWidget::SetProperty("WIDTH",(long unsigned int)width);
    PWidget::SetProperty("HEIGHT",(long unsigned int)height);

    delete myEval;

    bool success = 0;
    //INitialize the SDL surface with the appropriate flags.

    Variant scriptname = "PEBL Application";
    if(myEval->gGlobalVariableMap.Exists("gScriptName"))
        {
            scriptname = myEval->gGlobalVariableMap.RetrieveValue("gScriptName");
        }

    std::string sname = scriptname;
    int x= SDL_WINDOWPOS_CENTERED;
    int y= 15;//SDL_WINDOWPOS_CENTERED;
    if(!windowed)
    {
        x = SDL_WINDOWPOS_UNDEFINED;
        y = SDL_WINDOWPOS_UNDEFINED;
    }

    mWindow =SDL_CreateWindow(sname.c_str(),x,y,
                              //SDL_WINDOWPOS_UNDEFINED,
                              //SDL_WINDOWPOS_UNDEFINED,
                              width,height, vflags);

    ///This shouldn't be necessary, but if we specify the same screensize as the window, it goes to fake fullscreen mode.

    mRenderer = SDL_CreateRenderer(mWindow, -1,rflags);
//    mRenderer = SDL_CreateRenderer(mWindow,-1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_PRESENTVSYNC);
    //mRenderer = SDL_CreateSoftwareRenderer(SDL_GetWindowSurface(mWindow));
    //mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);

    if(mRenderer)
        {
            success = true;
        }else{
        PError::SignalFatalError("Failed to create renderer in PlatformWindow\n");
     }


    mFlags = vflags;
#ifdef SDL2_DELETE
    // mSurface = SDL_SetVideoMode(width,height,depth,SDL_HWACCEL | SDL_FULLSCREEN | SDL_DOUBLEBUF);
    if ( mSurface == NULL )
        {
            //If we fail, try to do our best.
            cerr << "Unable to set " << width << "x" << height << ": " << depth << " video mode: " << SDL_GetError() << endl;

            //This needs to be reworked because of new API that handles multi-screens, etc.

            //Get a list of available resolutions and try these out.
            SDL_Rect** modes = SDL_ListModes(NULL,vflags);
            if(modes == (SDL_Rect**)0)
                {
                    cerr << "No Video Modes Available" << endl;
                } else{
                for(int i=0; modes[i];++i)
                    {
                        width=modes[i]->w;
                        height=modes[i]->h;


                        cerr << "Trying resolution:" << width << "x" << height << ": " << depth << " video mode:\n ";
                        mSurface = SDL_SetVideoMode(width,height,depth,vflags);
                        if(mSurface)
                            {
                                cerr << "Using resolution:" << width << "x" << height << ": " << depth << " video mode:\n ";

                                success = true;
                                break;
                            }else{
                            cerr << "..........Resolution failed\n";
                        }
                    }

                cerr<< "Resolution may not be optimal.  Alternate resolutions available on system include:\n";

                modes =SDL_ListModes(NULL,vflags);
                for(int ii=0; modes[ii]; ++ii)
                    {
                        cerr << ii<< ". gVideoWidth <-"<< modes[ii]->w<<"; gVideoHeight<- " << modes[ii]->h <<"\n";
                    }
}




            if(success)
                {
                    //something worked out!
                    myEval->gGlobalVariableMap.AddVariable("gVideoWidth", width);
                    myEval->gGlobalVariableMap.AddVariable("gVideoHeight", height);
                    myEval->gGlobalVariableMap.AddVariable("gVideoDepth", depth);
                }else{

                //None of the resolutions worked out.
                return false;
            }


        }else{
        success = true;
    }
#endif
    if(success)
        {
            cerr << "\n\n--------------------------------------------------------------------------------\n";
            cerr << " Application settings:" << endl;

            cerr << "Script name:                               [" << sname << "]\n";
            cerr << PEBLEnvironment::TimeStamp(Variant(0))  << endl;
            cerr << "--------------------------------------------------------------------------------\n";

            cerr << "Display Mode:  Width  (pixels)                    [" << width << "]\n";
            cerr << "Display Mode:  Height (pixels)                    [" << height << "]\n";
            cerr << "Display Mode:  Color Depth (bits)                 [" << depth << "]\n";
            cerr << "vsync mode:                                       [" << vsync << "]\n";
            cerr << "Software renderer mode:                           [" <<softrender << "]\n";
            cerr << "Windowed:                                         ["<<windowed<< "]\n";
            cerr << "Resizeable:                                       ["<<resizeable<< "]\n";

            cerr << "Driver hint (gDriverHint):                        ";

            if(myEval->gGlobalVariableMap.Exists("gDriverHint"))
                {

                    cerr << "["<<myEval->gGlobalVariableMap.RetrieveValue("gDriverHint") << "]\n";
                }
            else
                {
                    cerr << "[none]\n";
                }



            cerr << "Base font (gPEBLBaseFont):                        ["<<
                myEval->gGlobalVariableMap.RetrieveValue("gPEBLBaseFont") << "]\n";

            cerr << "Base Mono font (gPEBLBaseFontMono):               ["<<
                myEval->gGlobalVariableMap.RetrieveValue("gPEBLBaseFontMono") << "]\n";

            cerr << "Base serif (gPEBLBaseFontSerif):                  ["<<
                myEval->gGlobalVariableMap.RetrieveValue("gPEBLBaseFontSerif") << "]\n";

            cerr << "Language (gLanguage):                             ["<<
                myEval->gGlobalVariableMap.RetrieveValue("gLanguage") << "]\n";

            cerr << "Parameter file (gParamFile):                      ["<<
                myEval->gGlobalVariableMap.RetrieveValue("gParamFile") << "]\n";

            cerr << "Busy/Easy wait: (gSleepEasy):                     ["<<
                myEval->gGlobalVariableMap.RetrieveValue("gSleepEasy") << "]\n";


            cerr << "Executable name: (gExecutableName):               ["<<
                myEval->gGlobalVariableMap.RetrieveValue("gExecutableName") << "]\n";


            cerr << "Resource path: (gPEBLResourcePath):               [" <<
                myEval->gGlobalVariableMap.RetrieveValue("gPEBLResourcePath") << "]\n";


            cerr << "Resource path: (gPEBLBasePath):                   [" <<
            myEval->gGlobalVariableMap.RetrieveValue("gPEBLBasePath") << "]\n";

            cerr << "Working directory: (gWorkingDirectory):           [" <<
            myEval->gGlobalVariableMap.RetrieveValue("gWorkingDirectory") << "]\n";



            SDL_RendererInfo drinfo;
            cerr << "Renderer information:\n";
            SDL_GetRendererInfo(mRenderer, &drinfo);
            cerr << "Driver name:                                      [" << drinfo.name << "]\n";;

            std::cerr << "Software fallback:                                [";

            if (drinfo.flags & SDL_RENDERER_SOFTWARE)
                std::cerr << "yes]\n"; else          std::cerr << "no]\n";

            std::cerr << "Hardware acceleration:                            [";

            if (drinfo.flags & SDL_RENDERER_ACCELERATED)
                std::cerr << "yes]\n";  else        std::cerr << "no]\n";

            std::cerr << "Vsync with refresh rate:                          [";
            if (drinfo.flags & SDL_RENDERER_PRESENTVSYNC)
                std::cerr << "yes]\n"; else std::cerr << "no]\n";

            std::cerr << "Rendering to texture support                      [";

            if (drinfo.flags & SDL_RENDERER_TARGETTEXTURE)
                std::cerr << "yes]\n"; else std::cerr << "no]\n";

            cerr << "--------------------------------------------------------------------------------\n\n";

            //Do an initial Draw()
            Draw();
            return true;
        }
    return false;
}


bool PlatformWindow::Draw()
{



    //if (SDL_MUSTLOCK(mSurface)) SDL_LockSurface(mSurface);


    // Get background color from property system (in case it was modified via nested properties)
    Variant backgroundColor = PEBLObjectBase::GetProperty("BGCOLOR");
    PColor* bgColor = nullptr;
    if(backgroundColor.GetComplexData())
    {
        bgColor = dynamic_cast<PColor*>(backgroundColor.GetComplexData()->GetObject().get());
    }

    // Use property color if available, otherwise fall back to mBackgroundColor
    if(!bgColor)
    {
        bgColor = &mBackgroundColor;
    }

#ifdef SDL2_DELETE
    //First, draw the background
    SDL_FillRect(mSurface, NULL, SDL_MapRGBA(mSurface->format,
                                            bgColor->GetRed(),
                                            bgColor->GetGreen(),
                                            bgColor->GetBlue(),
                                            bgColor->GetAlpha()));
#endif


 int result=   SDL_SetRenderDrawColor(mRenderer,
                           bgColor->GetRed(),
                           bgColor->GetGreen(),
                           bgColor->GetBlue(),
                           bgColor->GetAlpha());


    //    while(p != mSubWidgets.begin())
    //        {
    //            p--;
            //cout << "------drawing Subwidget on window:" << *p <<  "[" << **p << "]" << endl;

    //        }

    SDL_RenderClear(mRenderer);//clear the window
 if(result < 0)
     {
         PError::SignalFatalError("Unable to clear window");
         std::cerr <<SDL_GetError() << endl;
     }


    //cout << "Number of subwidgets: " << mSubWidgets.size() << endl;
    //This handles drawing each subwidget.
    std::list<PWidget *>::iterator p = mSubWidgets.end();

    while(p != mSubWidgets.begin())
        {
            //cout <<"Size"<< mSubWidgets.size() << endl;


            //decrement iterator--moving backward so we draw things in
            //reverse order.
            p--;

            //Draw the subwidget
            if((*p)->IsVisible())
                {
                    (*p)->Draw();
                }

        }

    SDL_RenderPresent(mRenderer);//draw the window.


#ifdef PEBL_EMSCRIPTEN
 //    int result = SDL_Flip(mSurface);
#else
//    int result = SDL_Flip(mSurface);
#endif


 return true;
}

// This will draw the current screen repeatedly for cycles cycles and return.
// On platforms where SDL_Flip blocks until it finishes (e.g., maybe like linux),
// then it should return immediately after the vsync, allowing time for removing the
// surface or whatever.  On platforms where SDL_Flip returns immediately, it should
// still block until the _next_ cycle, so it will return right after the n-1 cycle, giving
// time to delete etc before another draw command needs to be issued.
long int PlatformWindow::DrawFor(unsigned int cycles)
{

    //Do an initial syncing flip.
    int result = 0;// = SDL_Flip(mSurface);

    //    cout << "Drawforing cycles: " << cycles << endl;

    //Now, draw the subwidgets.

    unsigned int cyclesleft = cycles;
    unsigned int tstart = SDL_GetTicks();

    while(cyclesleft > 0 )
        {
            //Get the time *AFTER* the first cycle.
            //This method will only really work on a platform wher SDL_Flip blocks.

            //result = SDL_Flip(mSurface);
            Draw();
            //            cout << ".";
            cyclesleft--;
        }

    unsigned int tend = SDL_GetTicks();
    //cout << "\n";
        result = tend-tstart;

    //If the return value is positive, it is the time that
    //this was displayed.  Otherwise, it is an error code.
    return result;

}

bool PlatformWindow::Resize(int w, int h)
{
 //Resize the screen

    //    screen = SDL_SetVideoMode( event.resize.w, event.resize.h, SCREEN_BPP, SDL_SWSURFACE | SDL_RESIZABLE );
    Variant vDp =   myEval->gGlobalVariableMap.RetrieveValue("gVideoDepth");

//rework this for SDL2:
#ifdef SDL2_DELETE
    mSurface = SDL_SetVideoMode(w,h,(int)vDp,mFlags);

    if(mSurface)
        {
            myEval->gGlobalVariableMap.AddVariable("gVideoWidth", w);
            myEval->gGlobalVariableMap.AddVariable("gVideoHeight", h);
        }

#endif
    return true;
}



int PlatformWindow::SaveScreenShot(int x, int y, int w, int h,const Variant fname)
{

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    return SDLUtility::WritePNG(mRenderer,&rect,fname);
    

}
