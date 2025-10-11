
//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformMovie.cpp
//    Purpose:    Contains SDL-specific interface for images
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
#include "PlatformMovie.h"
#include "../../utility/PEBLPath.h"
#include "../../utility/PError.h"
#include "../../utility/PEBLUtility.h"

#ifdef PEBL_EMSCRIPTEN
#include "../../base/Evaluator2.h"
#else
#include "../../base/Evaluator.h"
#endif


#include "../../base/PEBLObject.h"

//#include "SDL/SDL.h"
//#include "SDL/SDL_image.h"
//#include "WAAVE.h"

#include <string>
#include <stdio.h>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::list;
using std::ostream;
using std::string;

#ifdef PEBL_MOVIES


///Standard Constructor
PlatformMovie::PlatformMovie():
    PMovie()

{
    mCDT=CDT_MOVIE;
    mSurface = NULL; //this isn't needed
    mRenderer = NULL;
    mStream = NULL;

}



PlatformMovie::PlatformMovie(PlatformMovie & pm):
    PlatformWidget()
{

    mStream = pm.GetStream();
    mStreamObj= pm.GetStreamingObj();

    mCDT=CDT_MOVIE;
    mSurface  = NULL;
}

///Standard Destructor
PlatformMovie::~PlatformMovie()
{



  /* close the opened stream. not mandatory  */
  if(mStream)
      WV_closeStream(mStream);




  /* close the streaming object */
  if(mStreamObj)
    WV_freeStreamRendererObj (mStreamObj);

  /* close the waave engine */
  //This might not be warranted if we have multiple videos in a single experiment.
  //It probably should only be closed on SDL_QUIT or something;
  //maybe in ~PlatformEnvironment???

  //  WV_waaveClose();


}

//Overloaded setposition
void PlatformMovie::SetPosition(int x, int y)
{
    PWidget::SetPosition(x,y);

    SDL_Rect location;
    location.h = mHeight;
    location.w = mWidth;
    location.x = mX;
    location.y = mY;

    if(mStreamObj)
      WV_resetStreamRendererOutput(mStreamObj,mRenderer,   &location);

}

void PlatformMovie::SetWidth(int w)
{
    PWidget::SetWidth(w);
    SDL_Rect location;
    location.h = mHeight;
    location.w = mWidth;
    location.x = mX;
    location.y = mY;

    if(mStreamObj)
      WV_resetStreamRendererOutput(mStreamObj,mRenderer,   &location);

}

void PlatformMovie::SetHeight(int h)
{
    PWidget::SetHeight(h);
    SDL_Rect location;
    location.h = mHeight;
    location.w = mWidth;
    location.x = mX;
    location.y = mY;

    if(mStreamObj)
      WV_resetStreamRendererOutput(mStreamObj,mRenderer,   &location);

}



void PlatformMovie::SetPlaybackPosition(unsigned long int x)
{
  PMovie::SetPlaybackPosition(x);
  uint32_t streamDuration = WV_getStreamDuration(mStream);
  if(x<=streamDuration)
      {
          WV_seekStream(mStream, x);
      }

}



void PlatformMovie::SetVolume(long double vol)
{

    PMovie::SetVolume(vol);
    WV_setVolume(mStream, vol);
}


// Inheritable function that is called by friend method << operator of PComplexData

ostream & PlatformMovie::SendToStream(ostream& out) const
{

    out << "<SDL PlatformMovie>" << flush;
    return out;
}





bool PlatformMovie::LoadMovie(const std::string &  moviefilename,
			      PlatformWindow * window,
                              int width, int height)
{

#if defined(PEBL_MOVIES)
    //mParent = window;  movie really doesn't have a parent.
    mWindow = window;
    //Check to see if we can find the movie file; if not, call everything off.
    string filename = Evaluator::gPath.FindFile(moviefilename);

    if(filename == "")
        PError::SignalFatalError(string("Unable to find movie file [")  + moviefilename + string("]."));

    //This uses the waave library to load a movie
    //Initialize the library.  We may initialize it multiple times,
    //and I don't know what that will dho.
    WV_waaveInit(WAAVE_INIT_AUDIO|WAAVE_INIT_VIDEO);
    char* fname = (char*)(filename.c_str());
    mStream = WV_getStream(fname);  //shouldn't this be const???

    mRenderer = window->GetRenderer();
    SDL_RenderClear(mRenderer);




    int streamType = WV_getStreamType(mStream);


    if(streamType == WV_STREAM_TYPE_VIDEO || streamType ==WV_STREAM_TYPE_AUDIOVIDEO)
        {
            SDL_Rect location;
            location.h = height;
            location.w = width;
            location.x = 0;
            location.y = 0;

            mStreamObj =WV_getStreamRendererObj(mRenderer,&location,1);
	    WV_setStreamingMethod(mStream,mStreamObj);
            WV_loadStream(mStream);

            mLength = WV_getStreamDuration(mStream);

        }else if(streamType == WV_STREAM_TYPE_AUDIO )
      {
	//just load the audio.
	mStreamObj  = NULL;
	WV_loadStream(mStream);

      }else{
        PError::SignalFatalError(string("file is not a movie file."));
        }


    mWidth = width;
    mHeight = height;
    PMovie::SetProperty("WIDTH", Variant(mWidth));
    PMovie::SetProperty("HEIGHT", Variant(mHeight));
    PMovie::SetProperty("DURATION",
                       Variant((long unsigned int)
                                          WV_getStreamDuration(mStream)));

    PMovie::SetProperty("FILENAME",Variant(filename));



#else
    PError::SignalFatalError("PEBL Not compiled with movie playing support");
#endif
    return false;
}

// This is for loading .mp3 and the like
//
//
bool PlatformMovie::LoadAudioFile(const std::string &  audiofilename)
{

#if defined(PEBL_MOVIES)
    //mParent = window;  movie really doesn't have a parent.

    //Check to see if we can find the movie file; if not, call everything off.
    string filename = Evaluator::gPath.FindFile(audiofilename);

    if(filename == "")
        PError::SignalFatalError(string("Unable to find audio file [")  + audiofilename + string("]."));

    //This uses the waave library to load a movie
    //Initialize the library.  We may initialize it multiple times,
    //and I don't know what that will dho.
    WV_waaveInit(WAAVE_INIT_AUDIO);
    char* fname = (char*)(filename.c_str());
    mStream = WV_getStream(fname);  //shouldn't this be const???


    int streamType = WV_getStreamType(mStream);

    if(streamType == WV_STREAM_TYPE_VIDEO || streamType ==WV_STREAM_TYPE_AUDIOVIDEO)
        {
	  std::cerr << "Warning: trying to load video file using LoadAudioFile\n";

        }else if(streamType == WV_STREAM_TYPE_AUDIO )
      {
	//just load the audio.
	mStreamObj  = NULL;
	WV_loadStream(mStream);

      }else{
        PError::SignalFatalError(string("file is not a known media file."));
        }


    PMovie::SetProperty("WIDTH", Variant(0));
    PMovie::SetProperty("HEIGHT", Variant(0));
    PMovie::SetProperty("DURATION",
                       Variant((long unsigned int)
                                          WV_getStreamDuration(mStream)));

    PMovie::SetProperty("FILENAME",Variant(filename));



#else
    PError::SignalFatalError("PEBL Not compiled with movie playing support");
#endif
    return false;
}




void PlatformMovie::StartPlayback()
{

    if(mStream)
        {
            WV_playStream(mStream);
        }

}

void PlatformMovie::PausePlayback()
{

    if(mStream)
        {
            WV_pauseStream(mStream);
        }

}




//This should be called when the eventloop gets the WV_REFRESH_EVENT
void PlatformMovie::RefreshVideo(SDL_Event &event)
{

  cout << "refreshingvideo\n";
#if PEBL_MOVIES
    WV_refreshVideoFrame(&event);
#endif
}



int PlatformMovie::GetState(int interface) const
{
    //cerr << "Unable to get state of movie\n";
    return 1;
}



Variant PlatformMovie::GetProperty(std::string name)const
{
    if(PEBLUtility::ToUpper(name) == "PLAYBACKPOSITION")
        {
            long unsigned int pos =  WV_getStreamClock(mStream);
            //SetProperty("PLAYBACKPOSITION",pos)
            return Variant(pos);
        }
    return PEBLObjectBase::GetProperty(name);
}

#endif
