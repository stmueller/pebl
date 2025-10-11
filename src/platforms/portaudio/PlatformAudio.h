//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/portaudio/PlatformAudio.h
//    Purpose:    Contains platform-specific sound playing/recording routines
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003-2025 Shane T. Mueller <smueller@obereed.ned>
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
#ifndef __PLATFORMAUDIO_H__
#define __PLATFORMAUDIO_H__

#include "../../devices/PAudioOut.h"
#include "portaudio.h"
  
struct AudioInfo{
    SDL_AudioSpec spec;
    Uint8   *audio;			/* Pointer to wave data */
    Uint32   audiolen;		/* Length of wave data */
    int      audiopos;		/* Current play position */
    int      volume;           /* Relative volume. 0-100*/
};
  
class PlatformAudioOut: public PAudioOut
{
 public:
    PlatformAudioOut();
    PlatformAudioOut(const char * filename);
    ~PlatformAudioOut();
  
    bool LoadSoundFile(const char* filename);

    bool CreateSineWave(float freq, double length, int amplitude);
    bool CreateSquareWave(float freq, double length, int amplitude);
    bool CreateSawtoothWave(float freq, double length, int amplitude);
    bool CreateChirp();

    bool Play();
    bool PlayForeground();
    bool Stop();
    
    
    bool Initialize();

    void AddTrack(char * handle, AudioInfo track);
    void RemoveTrack(char* handle);

    Uint8 * MixTracks();
    

private:
  

    friend void PlayCallBack(void * dummy, Uint8 * stream, int len);
    
    bool mLoaded;               //This will be true when a file 
                                //has been loaded or a buffer has been
                                //generated.
    AudioInfo mWave;
};


#endif
