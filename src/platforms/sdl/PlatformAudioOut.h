//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformAudioOut.h
//    Purpose:    Contains platform-specific sound playing routines
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
#ifndef __PLATFORMAUDIOOUT_H__
#define __PLATFORMAUDIOOUT_H__

#include "../../devices/PAudioOut.h"
#include "../../base/PEBLObject.h"
#include "SDL.h"
#include "SDL_audio.h"
  
#ifdef PEBL_MIXER
#include "SDL_mixer.h"
#endif

#include <string>

// AudioInfo is needed for audio input even when PEBL_MIXER is enabled
// Now inherits from PEBLObjectBase to use counted_ptr reference counting
#if !defined(PEBL_MIXER) || defined(PEBL_AUDIOIN)
class AudioInfo : public PEBLObjectBase
{
public:
    AudioInfo() : PEBLObjectBase(CDT_AUDIOBUFFER),
                  audio(NULL),
                  audiolen(0),
                  audiopos(0),
                  bytesPerSample(2),
                  recordpos(0),
                  counter(0),
                  volume(100),
                  name(NULL),
                  ownsBuffer(true)  // By default, AudioInfo owns the buffer
    {
        SDL_zero(spec);
    }

    virtual ~AudioInfo()
    {
        // Clean up the audio buffer only if we own it
        // Note: We use free() instead of delete[] because the buffer is allocated with malloc()
        // When shared with Mix_Chunk, ownsBuffer will be false
        if(audio && ownsBuffer)
        {
            free(audio);
            audio = NULL;
        }
    }

    // Data members (kept public for backward compatibility)
    SDL_AudioSpec spec;
    Uint8   *audio;			/* Pointer to wave data */
    Uint32  audiolen;		/* Length of wave data */
    Uint32  audiopos;		/* Current play position */

    unsigned int bytesPerSample;  //size of a sample
	Uint32 recordpos;      //current index in the buffer (in bytes)
    Uint32 counter;        //A counter to use that keeps track of samples
                           //since the beginning of recording.

    int   volume;           /* Relative volume. 0-100*/
    const char*    name;
    bool ownsBuffer;       /* Whether AudioInfo should free the buffer in destructor */
};

#endif

class PlatformAudioOut: virtual public PAudioOut, public PEBLObjectBase
{
 public:
    PlatformAudioOut();
    PlatformAudioOut(const std::string & filename);
    virtual ~PlatformAudioOut();
  
    virtual bool LoadSoundFile(const std::string & filename);
    bool LoadSoundFromData( Uint8 *buffer, long unsigned int size, SDL_AudioSpec *spec, Uint32 recordpos = 0);


    virtual bool CreateSineWave(float freq, long unsigned int length,long double volume);
    //    virtual bool CreateSquareWave(float freq, double length, int amplitude);
    //    virtual bool CreateSawtoothWave(float freq, double length, int amplitude);


    virtual bool Play();
    virtual bool PlayForeground();
    virtual bool Stop();
    void SaveBufferToWave(Variant filename);

#ifdef PEBL_MIXER
    virtual bool SetPanning(pDouble left, pDouble right);
    void SetRepeats(int num){mRepeats=num;};
    void SetRecordPos(Uint32 pos){mRecordPos=pos;};  // Update recorded position
#endif

#if !defined(PEBL_MIXER) || defined(PEBL_AUDIOIN)
    bool ConvertAudio(AudioInfo & info);
    counted_ptr<AudioInfo> GetAudioInfo();
    void PrintAudioInfo();
#endif
    virtual bool Initialize();


private:


    //    void PlayCallBack(void * dummy, Uint8 * stream, int len);

   static  bool mLoaded;               //This will be true when a file
                                //has been loaded or a buffer has been
                                //generated.
    int mRepeats;
    std::string mFilename;


#ifdef PEBL_MIXER
    Mix_Chunk * mMixerSample;
    Uint32 mRecordPos;           // Actual recorded size (for audio input buffers)
#else
    AudioInfo mWave;
#endif

};


#endif
