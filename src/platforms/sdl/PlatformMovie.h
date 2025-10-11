//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
///////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformMovie.h
//    Purpose:    Contains SDL platform-specific movie playing
//                code (using waave library)
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2012-2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//
//
//     This file is part of the PEBL project.
//
//    PEBL is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as 
//    published by the Free Software Foundation; either version 2
//    of the License, or (at your option) any later version.
//
//    PEBL is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public
//     License along with PEBL; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//     MA  02111-1307  USA
///////////////////////////////////////////////////////////////////
#ifndef __PLATFORMMOVIE_H__
#define __PLATFORMMOVIE_H__

#include "PlatformWidget.h"
#include "PlatformWindow.h"
#include "../../objects/PMovie.h"
#include "../../base/PEBLObject.h"


#ifdef PEBL_OSX
#include "SDL.h"
#else
#include "SDL.h"
#endif

#ifdef PEBL_MOVIES
#include "WAAVE.h"

#include <string>
///
///
///


class PlatformMovie: virtual public PMovie, virtual public PlatformWidget
{
public:

    PlatformMovie();
    PlatformMovie(PlatformMovie & movie);
    virtual ~PlatformMovie();

    virtual bool LoadMovie(const std::string & imagefilename,
                           PlatformWindow* window,
                           int width, int height);

    virtual bool LoadAudioFile(const std::string & imagefilename);



    virtual void StartPlayback();
    virtual void PausePlayback();

    virtual std::string ObjectName() const{return "PlatformMovie";};

    virtual void RefreshVideo(SDL_Event & event);


    virtual WVStream * GetStream(){return mStream;};
    virtual  WVStreamingObject * GetStreamingObj(){return mStreamObj;};


    virtual int GetState(int interface) const;

    virtual void SetPosition(int x, int y);
    virtual void SetWidth(int w);
    virtual void SetHeight(int h);

    virtual void SetPlaybackPosition(unsigned long int  x);
    virtual void SetVolume(long double vol);

    virtual Variant GetProperty(std::string name)const;
protected:

    virtual std::ostream & SendToStream(std::ostream& out) const;
    SDL_Surface * GetSurface();


private:

    /* playing stream */
    PlatformWindow *mWindow;  //movie needs to be attached to a window!
    //SDL_Overlay * mOverlay;
    WVStream * mStream;
    WVStreamingObject* mStreamObj;




};

#endif
#endif
