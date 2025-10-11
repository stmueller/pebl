//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformAudioOut.cpp
//    Purpose:    Contains platform-specific audio playing routines
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
//    PEBL is distributed in the hope that it will be usefu2l,
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


#ifdef PEBL_EMSCRIPTEN
#include "../../base/Evaluator-es.h"
#else
#include "../../base/Evaluator.h"
#endif

#include "SDL.h"
#include "SDL_audio.h"

#ifdef PEBL_MIXER
#include "SDL_mixer.h"
#endif

#include <cmath>
#include <fstream>
#include <memory.h>

void PlayCallBack(void * dummy, Uint8 * stream, int len);

#ifndef PEBL_MIXER
//initiate static data for callback.
extern AudioInfo *gWaveStream=NULL;

#endif

using namespace std;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

bool PlatformAudioOut::mLoaded = false;


PlatformAudioOut::PlatformAudioOut():
    PEBLObjectBase(CDT_AUDIOOUT),
    mRepeats(0)
{

    mChannel=-1;
    mAmplitudeLeft=1.0;   //Reality has a well-know left bias
    mAmplitudeRight=1.0;

#ifdef PEBL_MIXER
    //error message should go here.
 

    if(!mLoaded)
        {
            int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
            if( result < 0 )
                {
                    fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
                    exit(-1);
                }
            Initialize();
            mLoaded = true;
        }
#endif
    
}



PlatformAudioOut::PlatformAudioOut(const string &  soundfilename):
    PEBLObjectBase(CDT_AUDIOOUT),
    mRepeats(0)
{

    //Check to see if we can find the sound file; if not, call everything off.
    string mFilename = Evaluator::gPath.FindFile(soundfilename);

    if(mFilename == "")
        PError::SignalFatalError(string("Unable to find sound file [")  + soundfilename + string("]."));
    
    //    Initialize();
    LoadSoundFile(mFilename);
}


PlatformAudioOut::~PlatformAudioOut()
{

    mLoaded = false;
#ifdef PEBL_MIXER
    if(mMixerSample)
        Mix_FreeChunk(mMixerSample);
    
    Mix_CloseAudio(); //This will be called for each file
    Mix_Quit();
    
#else
    
    SDL_FreeWAV(mWave.audio);
#endif
    
}


bool PlatformAudioOut::LoadSoundFile(const string & soundfilename)
{
    //Check to see if we can find the sound file; if not, call everything off.
    mFilename = Evaluator::gPath.FindFile(soundfilename);

    if(mFilename == "")
        PError::SignalFatalError(string("Unable to find sound file [")  + soundfilename + string("]."));

#ifdef PEBL_MIXER
    //    std::cerr << "Loading   :   " << soundfilename << endl;
    //    std::cerr << "located at:   " << mFilename << endl;

    
    //Mix_LoadWAV destroys the filename, so let's make a copy.
    char * copy = (char*)malloc(strlen(mFilename.c_str()) + 1); 
    strcpy(copy, mFilename.c_str());



    mMixerSample = Mix_LoadWAV(mFilename.c_str());

	/* Load the wave file into memory*/
	if (mMixerSample == NULL)
        {
			std::cerr << "Couldn't load " << copy << ": " << SDL_GetError() << std::endl;
            return false;
        } else {
        

    }
    free(copy);
#else
	/* Load the wave file into memory */
	if ( SDL_LoadWAV(mFilename.c_str(), &mWave.spec, &mWave.audio, &mWave.audiolen) == NULL )
        {
			std::cerr << "Couldn't load " << mFilename << ": " << SDL_GetError() << std::endl;
            return false;
        }



    //If gWaveStream is loaded, we already have a frequency/format/channels set.
    //we need to convert the current sound to that format.


    if(gWaveStream)
        {

            bool samefreq = mWave.spec.freq==gWaveStream->spec.freq;
            bool sameformat = mWave.spec.format==gWaveStream->spec.format;
            bool samechannels = mWave.spec.channels==gWaveStream->spec.channels;


                {
            //Only convert if we have a different format
            if(!(samefreq & sameformat&samechannels & samechannels))
                {
                    PError::SignalWarning("Warning: input file will be converted to new playback format.");
                    ConvertAudio(*gWaveStream);
                }}

        }

    PrintAudioInfo();

    mLoaded = true;

    mWave.name = mFilename.c_str();
    mWave.spec.callback = PlayCallBack;
    mWave.spec.userdata = &mWave;

    //Set the global playback wave to the current wave.
    if(!gWaveStream)
        {
            gWaveStream = &mWave;
        }
#endif
    
    return true;

}

// This converts the given audio in mWave to the to the
// audio format info
//
#ifndef PEBL_MIXER
bool PlatformAudioOut::ConvertAudio(AudioInfo & info)
{




    // Code heavily borrowed from some SDL tutorial

    SDL_AudioCVT cvt;           /* audio format conversion structure */
    SDL_AudioSpec loaded = mWave.spec;       /* format of the loaded data */
    SDL_AudioSpec target = info.spec;
    Uint8 *new_buf;


    /* Build a conversion structure for converting the samples.
       This structure contains the data SDL needs to quickly
       convert between sample formats. */

    std::cerr << "Converting from " <<   loaded.format<<"|"<<(int)loaded.channels<<"|"<< loaded.freq<<"to "<<
        target.format<<"|"<< (int)target.channels<<"|"<<target.freq<<std::endl;

    SDL_AudioSpec * desired=(SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
    desired->freq=target.freq;
    desired->format=target.format;
    desired->channels = target.channels;
    desired->samples=target.samples;
    desired->callback=NULL;
    desired->userdata=NULL;


   if (SDL_BuildAudioCVT(&cvt,
                          loaded.format, loaded.channels, loaded.freq,
                          desired->format, desired->channels, desired->freq) < 0)
        {
            PError::SignalFatalError(Variant("Unable to convert sound: ") + Variant(SDL_GetError()));
        }

    if(cvt.needed==0)
        {
            PError::SignalWarning(Variant("Unable to convert sound.  Be sure all of your sounds are saved in the same format.") + Variant(SDL_GetError()));
        }

    //set cvt.len to the size of the sourcedata.
    cvt.len = mWave.audiolen;

    //Allocate a big enough buffer to do the conversion:
    new_buf = (Uint8 *) malloc(cvt.len * cvt.len_mult);
    if (new_buf == NULL)
        {
            PError::SignalFatalError("Memory allocation failed in PlatformAudioOut::ConvertAudio");
        }

    /* Copy the sound samples into the new buffer. */
    memcpy(new_buf, mWave.audio, mWave.audiolen);

    /* Perform the conversion on the new buffer. */
    cvt.buf = new_buf;



#if 0
    cout << "Conversion information: " ;
    std::cout <<"Needed:    "<< cvt.needed << std::endl;
    std::cout <<"srcformat: "<< cvt.src_format << std::endl;
    //   std::cout <<"destformat: "<< cvt.dest_format << std::endl;
    std::cout <<"rate_incr:  "<< cvt.rate_incr << std::endl;
    std::cout <<"len:        "<< cvt.len << std::endl;
    std::cout <<"len_cvt:        "<< cvt.len_cvt << std::endl;
    std::cout <<"len_mult:   "<< cvt.len_mult<< std::endl;
    std::cout <<"ratio:      "<< cvt.len_ratio<< std::endl;

    cout << "CONVERTING\n";
#endif

    if (SDL_ConvertAudio(&cvt) < 0)
        {

            PError::SignalFatalError(Variant("Audio conversion error:")+ Variant(SDL_GetError()));
        }


    /* Swap the converted data for the original. */
    SDL_FreeWAV(mWave.audio);


    mWave.audio =  new_buf;
    mWave.audiolen = mWave.audiolen * cvt.len_mult;


   mWave.spec.freq=info.spec.freq;
   mWave.spec.format=info.spec.format;
   mWave.spec.channels=info.spec.channels;
   mWave.spec.silence=info.spec.silence;
   //   mWave.spec.samples=info.spec.samples;
   //   mWave.spec.size=info.spec.size;


#if 1
   switch(mWave.spec.format)
        {

        case AUDIO_U8:
            mWave.bytesPerSample = 8;
            break;

        case AUDIO_U16:
            mWave.bytesPerSample = 2;
            break;


        case AUDIO_S8:
            mWave.bytesPerSample = 1;
            break;

        case AUDIO_S16:
            mWave.bytesPerSample = 2;
            break;


        default:
            mWave.bytesPerSample = 2;
        }

#endif



    return true;
 }
#endif

bool PlatformAudioOut::CreateSineWave(float freq, long unsigned int mslength, long double amplitude)
{


#ifdef PEBL_MIXER
    return false;
#else
    /* Allocate a desired SDL_AudioSpec */
    SDL_AudioSpec *spec = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));


    if(gWaveStream)
        {
            //Use the already-available spec.
            //cout <<"using preloaded sound spec\n";

            memcpy(spec,&(gWaveStream->spec),sizeof(SDL_AudioSpec));

        }
    else
        {
            //Create a new spec

            /* Allocate space for the obtained SDL_AudioSpec */
            //    obtained = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));

            spec->freq =44100;
            spec->format=AUDIO_U8;
            spec->channels=1;
            spec->silence=0x80;
            spec->samples=4096;
            spec->callback= PlayCallBack;
            spec->userdata=&mWave;
        }

    //mslength is time in ms.
    //compute length in samples

    int bits=0;
    if((spec->format == AUDIO_U8) |
       (spec->format == AUDIO_S8) )
        {
            bits=1;

        }else if((spec->format == AUDIO_S16 )|
                 (spec->format == AUDIO_U16))
        {
            bits =2;

        }

    long unsigned int length = mslength/1000.0*spec->freq*spec->channels*bits;

    Uint8 *data = new Uint8[length];
    int dat;
    double base=0.0;

    for(unsigned int i=0; i<length;i+=spec->channels)
        {

            if(spec->format==AUDIO_U8)
                {
                    //base needs to be different for different formats
                    base = (sin(i*6.28/(spec->freq/freq))*amplitude+1)/2;
                }

            if(base<0)base=0;
            if(base>1)base=1;
            //base is bounded between 0 and 1
            dat = int(base*256);
            //cout << base << "," << amplitude  << ","<< dat << endl;

            //Copy dat to each channel
            for(int j = 0; j < spec->channels;j+=bits)
                data[i+j] = dat;
        }



    LoadSoundFromData(data,length,spec);

    return true;
#endif
}




//;unsigned int freq,int size)
bool PlatformAudioOut::LoadSoundFromData( Uint8 *buffer,
                                          long unsigned int size,
                                          SDL_AudioSpec *spec)
{

#ifdef PEBL_MIXER
    return false;
#else 
    
    /* setup audio */
    //SDL_AudioSpec spec;
   // SDL_AudioSpec obtained;


    /* Allocate a desired SDL_AudioSpec */
    SDL_AudioSpec *spec2 = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));
    memcpy(spec2,spec,sizeof(SDL_AudioSpec));

    /* Allocate space for the obtained SDL_AudioSpec */
    //    obtained = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));


    /*
      spec.freq =freq;
      spec.format=AUDIO_U8;
      spec.channels=1;
      spec.silence=0x80;
      spec.samples=4096;
      spec.callback= PlayCallBack;
      spec.userdata=&mWave;
    */


    mWave.spec = *spec2;
    mWave.audio=buffer;
    mWave.audiopos=0;
    mWave.audiolen=(unsigned int)size;
    mWave.volume=100;
    mWave.name="Generated data";


	/* Load the wave file into memory */
    //	if ( SDL_LoadWAV_RW(rw,1, &mWave.spec, &mWave.audio, &mWave.audiolen) == NULL )
    //        {
    //			std::cerr << "Couldn't load created audio data: " << SDL_GetError() << std::endl;
    //            return false;
    //        }


    cerr << "------------------------------------\n";
    cerr << "Loading Sound Data.\n";
    PrintAudioInfo();
    mLoaded = true;




    //Check to see if we can find the sound file; if not, call everything off.
    mFilename = "<INTERNALLY GENERATED>";


    mWave.name = mFilename.c_str();
    mWave.spec.callback = PlayCallBack;
    mWave.spec.userdata = &mWave;


    //Set the global playback wave to the current wave.
    if(!gWaveStream)
        {
            gWaveStream = &mWave;
        }

    return true;
#endif
}


//
//  This is nearly identical to a piece of code in PlatformAudioIn
//
void PlatformAudioOut::SaveBufferToWave(Variant filename)
{
#ifndef PEBL_MIXER

    //Code here adapted from
    //http://www.codeproject.com/Messages/3208219/How-to-write-mic-data-to-wav-file.aspx

	int bitsPerSample = mWave.bytesPerSample*8;

    //Unclear about these chunk things:
	int subchunk1size = 16;
	int numChannels = mWave.spec.channels;

	int subchunk2size = mWave.audiolen;
	int chunksize = 36+subchunk2size;


	int audioFormat = 1;  //PCM

    int sampleRate = mWave
        .spec.freq;
	int byteRate = mWave.spec.freq*numChannels*bitsPerSample/8;
	int blockAlign = numChannels*bitsPerSample/8;


    cout <<"--------------------------------------------\n";
    cout << "saving file        ["<< filename<<"]\n";
    cout << "bitspersample:      " << bitsPerSample <<endl;
    cout << "Channels:           " << numChannels <<endl;
    cout << "frequency:          " << sampleRate << endl;
    cout << "byterate:           " << byteRate << endl;


    std::fstream myFile (filename.GetString().c_str(), ios::out | ios::binary);

	// write the wav file per the wav file format
	myFile.seekp (0, ios::beg);
	myFile.write ("RIFF", 4);					// chunk id
	myFile.write ((char*) &chunksize, 4);	    // chunk size (36 + SubChunk2Size))
	myFile.write ("WAVE", 4);					// format
	myFile.write ("fmt ", 4);					// subchunk1ID
	myFile.write ((char*) &subchunk1size, 4);	// subchunk1size (16 for PCM)
	myFile.write ((char*) &audioFormat, 2);		// AudioFormat (1 for PCM)
	myFile.write ((char*) &numChannels, 2);		// NumChannels
	myFile.write ((char*) &sampleRate, 4);		// sample rate
	myFile.write ((char*) &byteRate, 4);		// byte rate (SampleRate * NumChannels * BitsPerSample/8)
	myFile.write ((char*) &blockAlign, 2);		// block align (NumChannels * BitsPerSample/8)
	myFile.write ((char*) &bitsPerSample, 2);	// bits per sample

	myFile.write ("data", 4);					// subchunk2ID
	myFile.write ((char*) &subchunk2size, 4);	// subchunk2size (NumSamples * NumChannels * BitsPerSample/8)

	myFile.write ((char*)(mWave.audio), mWave.audiolen);	// data

#endif
}




//This must be called after the audio is initialized but before it can
//be played.  It actually opens the audio device for playing.
bool PlatformAudioOut::Initialize()
{


#ifdef PEBL_MIXER

    //This should only get called once
    
    mMixerSample = NULL;
   
   //Initialize with the proper file libraries:
   // Set up the audio stream


    int flags=MIX_INIT_OGG|MIX_INIT_MP3|MIX_INIT_FLAC|MIX_INIT_MOD;
    int initted = Mix_Init(flags);

 cout << "Attempted: " << flags << endl;
    cout << "support code: " << initted << endl;
 	//initted stores the formats currently supported.
    std::string supportOGG = (initted & MIX_INIT_OGG)?"yes":"no";
    std::string supportMP3 = (initted & MIX_INIT_MP3)?"yes":"no";
    std::string supportFLAC = (initted & MIX_INIT_FLAC)?"yes":"no";
    std::string supportMOD = (initted & MIX_INIT_MOD)?"yes":"no";

    
    // get and print the audio format in use
    int numtimesopened, frequency, channels;
    Uint16 format;
    numtimesopened=Mix_QuerySpec(&frequency, &format, &channels);
    if(!numtimesopened) {
        printf("Mix_QuerySpec: %s\n",Mix_GetError());
    }
    else {
        std::string format_str="Unknown";
        switch(format) {
            
            
        case AUDIO_U8: format_str="U8"; break;
        case AUDIO_S8: format_str="S8"; break;
        case AUDIO_U16LSB: format_str="U16LSB"; break;
        case AUDIO_S16LSB: format_str="S16LSB"; break;
        case AUDIO_U16MSB: format_str="U16MSB"; break;
        case AUDIO_S16MSB: format_str="S16MSB"; break;
        }
        
        std::cerr << "------------------------------------------------\n";
        std::cerr << "Loading PEBL Audio Framework using SDL_Mixer\n";
        std::cerr << "Opened (times):       [" << numtimesopened <<"]" << endl;
        std::cerr << "Frequency (Hz):       [" << frequency << "]" <<endl;
        std::cerr << "Format:               [" << format_str << "]" << endl;
        std::cerr << "Channels:             [" << channels << "]" << endl;
        std::cerr << "\nFile formats supported:\n";
        std::cerr << "   .wav: (builtin)    [yes]\n";
        std::cerr << "   .mp3: (mpg123)     ["<< supportMP3<<"]\n";
        std::cerr << "   .ogg: (libvorbis)  ["<< supportOGG<<"]\n";
        std::cerr << "   .flac: (libflac)   ["<< supportFLAC<<"]\n";
        std::cerr << "   .midi (libmikmod)  ["<< supportMOD<<"]\n";
        std::cerr << "------------------------------------------------\n";
    }
    
    
    //This allocates mixing channels. We probably just need
    //fewer, but sixteen should give us flexibility.

    Mix_AllocateChannels(16);

    
#else

    if (mLoaded &&  SDL_OpenAudio(&mWave.spec, NULL) < 0 ) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        return false;
	}


    //Reset the position to the beginning.
    mWave.audiopos = 0;
#endif
    
    return true;
}

//  This plays the sound using the callback mixer function
//  (in the background)
//
bool PlatformAudioOut::Play()
{

#ifdef PEBL_MIXER

    
    int lefti =  floor(mAmplitudeLeft * 255.9999999);
    int righti = floor(mAmplitudeRight * 255.999999);



    
    mChannel = Mix_PlayChannel(mChannel, mMixerSample, mRepeats);
    //Panning has to happen after the channel has been assigned and
    //I guess is starting to play?
    if(!Mix_SetPanning(mChannel, lefti,righti)) {
        printf("BG Mix_SetPanning: %s\n", Mix_GetError());
        // no panning, is it ok?
    }




#else
    
    SDL_LockAudio();
    mWave.audiopos = 0;
    gWaveStream = &mWave;
    SDL_UnlockAudio();

    //    Initialize();

    SDL_PauseAudio(0);
#endif

    
    return true;
}


//This will play the file, not returning until it is complete.
bool PlatformAudioOut::PlayForeground()
{

#ifdef PEBL_MIXER

    int lefti =  floor(mAmplitudeLeft * 255.9999999);
    int righti = floor(mAmplitudeRight * 255.999999);



  if(!Mix_SetPanning(mChannel, lefti,righti)) {
        printf("FG Mix_SetPanning: %s\n", Mix_GetError());
        // no panning, is it ok?
    }


    mChannel = Mix_PlayChannel(mChannel, mMixerSample, mRepeats);
    
    while(Mix_Playing(mChannel))
        {
         SDL_Delay(5);
        }
    Mix_HaltChannel(mChannel);

#else



    SDL_LockAudio();

    mWave.audiopos = 0;
    gWaveStream = &mWave;

    SDL_UnlockAudio();



    SDL_PauseAudio(0);
    while(SDL_GetAudioStatus() == SDL_AUDIO_PLAYING)
        {
            //Wait at least 10 ms before checking again.
            SDL_Delay(10);
            //cout << "---------- playing    ["<<SDL_GetTicks() << endl;
        }

    SDL_PauseAudio(1);
#endif
    
    return true;

}

#ifdef PEBL_MIXER
bool PlatformAudioOut::SetPanning(pDouble left, pDouble right)
{


    mAmplitudeLeft = left;
    mAmplitudeRight = right;

    
    mAmplitudeLeft = (mAmplitudeLeft>1.0)?1.0:mAmplitudeLeft;
    mAmplitudeLeft = (mAmplitudeLeft)<0.0?0.0:mAmplitudeLeft;
    
    mAmplitudeRight = (mAmplitudeRight>1.0)?1.0:mAmplitudeRight;
    mAmplitudeRight = (mAmplitudeRight<0.0)?0.0:mAmplitudeRight;


    

    
    int lefti =  floor(mAmplitudeLeft * 255.9999999);
    int righti = floor(mAmplitudeRight * 255.999999);



    //If mChannel is -1, don't set it yet because it won't work.
    if(mChannel>=0)
        {
            if(!Mix_SetPanning(mChannel, lefti,righti)) {
                printf("Live Mix_SetPanning on channel [%d]: %s\n", mChannel,Mix_GetError());
            }
        }
    

    return true;
}
#endif


bool PlatformAudioOut::Stop()
{

#ifdef PEBL_MIXER
    //cout << "Stopping: " << mChannel << endl;
    if(mChannel>=0)
        Mix_HaltChannel(mChannel);

#else
    SDL_PauseAudio(1);
    //Set the audio stream back to the beginning.
    //SDL_CloseAudio();
    mWave.audiopos=0;
#endif
    return true;
}







#ifndef PEBL_MIXER
AudioInfo * PlatformAudioOut::GetAudioInfo()
{

    
    AudioInfo * tmp = new AudioInfo(mWave);
#if 0

    cout << "---------------------------\n";
    cout << "getting info in pao:getaudioinfo\n";
    cout << "freq     "<<mWave.spec.freq << " -- " << tmp->spec.freq  <<endl;
    cout << "length:  " <<mWave.audiolen<< "--" << tmp->audiolen <<     endl;
    cout << "---------------------------\n";
#endif

    return tmp;
};



void PlayCallBack(void * udata, Uint8 * stream, int len)
{

    SDL_memset(stream, 0, len);

    Uint8 * waveptr;
    int waveleft;

    //Cast udata to a proper form--this is dangerous and nasty.
    AudioInfo * wave = gWaveStream;//(AudioInfo*)(udata);


    //Put pointer at the proper place in the buffer.
    waveptr = wave->audio + wave->audiopos;
    waveleft = wave->audiolen - wave->audiopos;

    //   cerr << "waveleft: " <<  waveleft << "  len:" << len << endl;
    if(waveleft >= len)
        {

            SDL_MixAudio(stream, waveptr, len, SDL_MIX_MAXVOLUME);
            //This may appear in future formats.
            //SDL_MixAudioFormat(stream,waveptr,wave->spec,len,SDL_MIX_MAXVOLUME);
            wave->audiopos += len;

        }
    else
        {
            //This plays the of the file and stops playing.
            SDL_MixAudio(stream, waveptr, waveleft, SDL_MIX_MAXVOLUME);
            //SDL_MixAudioFormat(stream,waveptr,wave->spec,waveleft,SDL_MIX_MAXVOLUME);

            wave->audiopos += waveleft;
            SDL_PauseAudio(1);
            wave->audiopos=0;  //Reset it back to the beginning.

        }

}
#endif

#ifndef PEBL_MIXER
void PlatformAudioOut::PrintAudioInfo()
{

    cerr << "------------------------------------\n";
    cerr << "Filename  : " << mFilename << endl;
    cerr << "Audio specs:\n";
    cerr << "Frequency:   [" << mWave.spec.freq << "]\n";
    Variant form = "";
    switch(mWave.spec.format)
        {

        case AUDIO_U8:
            form="AUDIO_U8";
            break;

        case AUDIO_U16LSB:
            form = "AUDIO_U16LSB";
            break;


        case AUDIO_S8:
            form="AUDIO_S8";
            break;

        case AUDIO_S16:
            form="AUDIO_S16";
            break;


        default:
            form = "UNKNOWN";

        }



    cerr << "Format:      [" << form << "]\n";
    cerr << "Channels:    [" << (int)(mWave.spec.channels) << "]\n";
    cerr << "Silence:     [" << mWave.spec.silence  << "]\n";
    cerr << "Samples:     [" << mWave.spec.samples  << "]\n";
    cerr << "Size:        [" << mWave.spec.size     << "]\n";
    cerr << "Length(bytes)["<<mWave.audiolen<<"]\n";
    cerr << "Bytes per sample: [" << mWave.bytesPerSample <<"]\n";
    cerr << "Total samples:    [" << (double)mWave.audiolen/mWave.bytesPerSample <<"]\n";
    cerr << "Playback:    ["<<mWave.audiopos<<"]\n";
    cerr << "------------------------------------\n";

}
#endif
