//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/portaudio/PlatformAudio.cpp
//    Purpose:    Contains platform-specific audio playing/recording routines
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

#include "PlatformAudioOut.h"
#include "../../devices/PAudioOut.h"

#include "../../utility/PEBLPath.h"
#include "../../utility/PError.h"
#include "../../base/Evaluator.h"

#include "portaudio.h"

int PEBLAudioCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      const PaStreamCallbackFlags statusFlags, void *userData );


using std::string;
using std::cout;

PlatformAudioOut::PlatformAudioOut()
{
}



PlatformAudioOut::PlatformAudioOut(const char * soundfilename)
{

    //Check to see if we can find the sound file; if not, call everything off.
    string filename = Evaluator::gPath.FindFile(soundfilename);

    if(filename == "")
        PError::SignalFatalError(string("Unable to find sound file [")  + soundfilename + string("]."));

    
    LoadSoundFile(filename.c_str());
    Initialize();    
}


PlatformAudioOut::~PlatformAudioOut()
{
    SDL_FreeWAV(mWave.audio);

}


bool PlatformAudioOut::LoadSoundFile(const char* soundfilename)
{
    //Check to see if we can find the sound file; if not, call everything off.
    string filename = Evaluator::gPath.FindFile(soundfilename);

    if(filename == "")
        PError::SignalFatalError(string("Unable to find sound file [")  + soundfilename + string("]."));

 
	/* Load the wave file into memory */
	if ( SDL_LoadWAV(filename.c_str(), &mWave.spec, &mWave.audio, &mWave.audiolen) == NULL )
        {
            fprintf(stderr, "Couldn't load %s: %s\n", mFileName, SDL_GetError());
            return false;
        }

    cout << "------------------------------------\n";
    cout << "Loading Sound File.  Specs:\n";
    cout << "Frequency:   [" << mWave.spec.freq << "]\n";
    cout << "Format:      [" << mWave.spec.format << "]\n";
    cout << "Channels:    [" << mWave.spec.channels << "]\n";
    cout << "Silence:     [" << mWave.spec.silence  << "]\n";
    cout << "Samples:     [" << mWave.spec.samples  << "]\n";
    cout << "Size:        [" << mWave.spec.size     << "]\n";
    cout << "------------------------------------\n";
    mLoaded = true;
    mWave.spec.callback = PlayCallBack;
    mWave.spec.userdata = &mWave;
    return true;
}


//This must be called after the audio is initialized but before it can
//be played.  It actually opens the audio device for playing.
bool PlatformAudioOut::Initialize()
{	
    
    if (mLoaded &&  SDL_OpenAudio(&mWave.spec, NULL) < 0 ) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        return false;
	}
    
    //Reset the position to the beginning.
    mWave.audiopos = 0;

    return true;
}


bool PlatformAudioOut::Play()
{
    SDL_CloseAudio();  //Close the current audio stream just in case something is playing.
    Initialize();
    SDL_PauseAudio(0);
    
    return true;
}


//This will play the file, not returning until it is complete.
//Do not count on immediate return, as the function goes to sleep
bool PlatformAudioOut::PlayForeground()
{

    SDL_CloseAudio();
    Initialize();
    SDL_PauseAudio(0);
    while(SDL_GetAudioStatus() == SDL_AUDIO_PLAYING)
        {
            //Wait at least 10 ms before checking again.
            SDL_Delay(10);
        }
    SDL_CloseAudio();

    return true;
}

bool PlatformAudioOut::Stop()
{
    SDL_PauseAudio(1);
    //Set the audio stream back to the beginning.
    SDL_CloseAudio();
    mWave.audiopos=0;
    return true;
}

void PlayCallBack(void * udata, Uint8 * stream, int len)
{

    //    cout << "Callback Called\n" <<endl;
    Uint8 * waveptr;
    int waveleft;

    //Cast udata to a proper form--this is dangerous and nasty.
    AudioInfo * wave = (AudioInfo*)(udata);
    
    //Put pointer at the proper place in the buffer.
    waveptr = wave->audio + wave->audiopos;
    waveleft = wave->audiolen - wave->audiopos;

    //   cout << "waveleft: " <<  waveleft << "  len:" << len << endl;
    if(waveleft >= len)
        {
            SDL_MixAudio(stream, waveptr, len, SDL_MIX_MAXVOLUME);
            wave->audiopos += len;
            
        }
    else
        {
            //This plays the rest of the file and stops playing.
            SDL_MixAudio(stream, waveptr, waveleft, SDL_MIX_MAXVOLUME);
            wave->audiopos += waveleft;
            SDL_PauseAudio(1);
            wave->audiopos=0;  //Reset it back to the beginning.
            
        }
    
}
