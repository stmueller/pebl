//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformAudioIn.cpp
//    Purpose:    Contains platform-specific audio recording routines
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2011-2025 Shane T. Mueller <smueller@obereed.net>
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

#include "PlatformAudioIn.h"
#include "PlatformAudioOut.h"

//#include "../../devices/PAudioIn.h"

#include "../../utility/PEBLPath.h"
#include "../../utility/PError.h"


#ifdef PEBL_EMSCRIPTEN
#include "../../base/Evaluator2.h"
#else
#include "../../base/Evaluator.h"
#endif

#include "../../base/PList.h"
#include "../../base/PComplexData.h"


#ifdef PEBL_OSX
#include "SDL.h"
#else
#include "SDL.h"
#endif

#ifdef PEBL_AUDIOIN
//#include "SDL_audioin.h"

#include <cmath>
#include <vector>
#include <fstream>

void AudioInCallbackFill(void * udata, Uint8 * stream, int len);
void AudioInCallbackLoop(void * udata, Uint8 * stream, int len);

//initiate static data for callback.
//extern AudioInfo *gWaveStream=NULL;
extern AudioInfo *gAudioBuffer = NULL;

using std::string;
using std::cerr;
using std::cout;
using std::endl;
using namespace std;

PlatformAudioIn::PlatformAudioIn()
    //    PEBLObjectBase(CDT_AUDIOIN)
{
    //Initialize audio device to 0 (uninitialized)
    mAudioDevice = 0;

    //Default audio settings
    mSampleRate = 44100;
    mAudioFormat = AUDIO_S16;
    mBytesPerSample = 2;
    mSamples = 256;
    // mWave is automatically initialized to NULL by counted_ptr constructor
}



PlatformAudioIn::~PlatformAudioIn()
{
    // Close SDL2 audio device
    if(mAudioDevice > 0)
    {
        SDL_CloseAudioDevice(mAudioDevice);
        mAudioDevice = 0;
    }

    // Clear the global buffer pointer if it points to our buffer
    if(mWave.get() && mWave.get() == gAudioBuffer)
    {
        gAudioBuffer = NULL;
    }

    // counted_ptr will automatically handle reference counting and deletion
    // No manual memory management needed!
}




//This must be called after the audio is initialized but before it can
//be used for recording. It opens the audio device for capture.
bool PlatformAudioIn::Initialize(int type)
{
    // SDL2 audio already initialized in main PEBL startup (SDL_Init(SDL_INIT_AUDIO))

    SDL_AudioSpec want, have;
    SDL_zero(want);

    want.freq = mSampleRate;        // 44100
    want.format = mAudioFormat;     // AUDIO_S16
    want.channels = 1;              // Mono
    want.samples = mSamples;        // 256
    want.userdata = &have;

    // Select which callback should be used
    if(type == 1)
    {
        want.callback = AudioInCallbackFill;
    }
    else
    {
        want.callback = AudioInCallbackLoop;
    }

    // List all available recording devices
    int numDevices = SDL_GetNumAudioDevices(SDL_TRUE);
    std::cout << "====================================\n";
    std::cout << "Available audio recording devices: " << numDevices << "\n";
    for(int i = 0; i < numDevices; i++) {
        const char* name = SDL_GetAudioDeviceName(i, SDL_TRUE);
        std::cout << "  Device " << i << ": " << (name ? name : "NULL") << "\n";
    }

    // Try to find the built-in digital microphone (avoid headphone jack)
    // Look for "Digital Microphone" in the device name
    int deviceIndex = 0;  // Default to first device
    for(int i = 0; i < numDevices; i++) {
        const char* name = SDL_GetAudioDeviceName(i, SDL_TRUE);
        if(name && strstr(name, "Digital Microphone")) {
            deviceIndex = i;
            std::cout << "Found Digital Microphone at index " << i << "\n";
            break;
        }
    }

    // Get the selected recording device
    const char* deviceName = SDL_GetAudioDeviceName(deviceIndex, SDL_TRUE);
    if(!deviceName)
    {
        PError::SignalWarning("No audio recording device found");
        return false;
    }

    std::cout << "Opening device " << deviceIndex << ": " << deviceName << "\n";

    // Open capture device (SDL_TRUE = recording)
    mAudioDevice = SDL_OpenAudioDevice(deviceName, SDL_TRUE, &want, &have, 0);
    if(mAudioDevice == 0)
    {
        PError::SignalWarning("Cannot open audio input device: " + Variant(SDL_GetError()));
        return false;
    }

    std::cout << "Device opened successfully!\n";
    std::cout << "  Requested: " << want.freq << "Hz, " << (int)want.channels << " channels, format=" << want.format << "\n";
    std::cout << "  Got:       " << have.freq << "Hz, " << (int)have.channels << " channels, format=" << have.format << "\n";
    std::cout << "  Samples:   " << have.samples << "\n";
    std::cout << "====================================\n";

    // Update actual specs if they differ from requested
    mSampleRate = have.freq;
    mAudioFormat = have.format;

    return true;
}


counted_ptr<AudioInfo> PlatformAudioIn::GetAudioOutBuffer()
{
    return mWave;  // Return copy of counted_ptr (increments reference count)
}




counted_ptr<AudioInfo> PlatformAudioIn::ReleaseAudioOutBuffer()
{
    counted_ptr<AudioInfo> tmp = mWave;  // Copy the counted_ptr
    mWave = counted_ptr<AudioInfo>();  // Reset to NULL, releasing our reference

    return tmp;  // Return the counted_ptr
}

//
//  This attaches a buffer within the PlatformAudioOut to use

bool PlatformAudioIn::UseBuffer( counted_ptr<AudioInfo> buffer )
{
    if(mWave.get())
    {
        PError::SignalFatalError("Attempting to add a buffer to an input stream that already has one.\n");
    }
    else
    {
        // Store the counted_ptr - this increments the reference count
        mWave = buffer;
        gAudioBuffer = mWave.get();

        mSampleRate= mWave->spec.freq;
        mAudioFormat=mWave->spec.format;
        if((buffer->spec.format == AUDIO_U8) |
           (buffer->spec.format == AUDIO_S8) )
        {
            mBytesPerSample = 1;
        }
        else if((buffer->spec.format == AUDIO_S16 )|
                 (buffer->spec.format == AUDIO_U16))
        {
            mBytesPerSample=2;
        }
        mWave->bytesPerSample = mBytesPerSample;
        mSamples = buffer->audiolen/mBytesPerSample;
    }
    return true;
}


// This creates a buffer to capture stuff to.
//size is the size, in ms that needs to be created,
//at a sampling frequency determined by the class
bool PlatformAudioIn::CreateBuffer(int size)
{
    if(mWave.get())
    {
        PError::SignalFatalError("Attempting to add a buffer to an input stream that already has one.\n");
    }

    // Create a new AudioInfo object wrapped in counted_ptr
    mWave = counted_ptr<AudioInfo>(new AudioInfo());

    //Make a SDL_AudioSpec;
    SDL_AudioSpec *spec = (SDL_AudioSpec *) malloc(sizeof(SDL_AudioSpec));
    spec->freq =44100;
    spec->format=AUDIO_S16;
    spec->channels=1;
    spec->silence=0x80;
    spec->samples=256;  //4096
    spec->callback= NULL;  //Don't have a callback for playing here.
    spec->userdata=NULL;

    Uint32 length = spec->freq * size/1000;
    mWave->spec = *spec;

    //allocate the buffer:
    mWave->audio = (Uint8*)malloc(mBytesPerSample*length);
    if(mWave->audio)
    {
        //            cout << "Memory allocated\n";
    }
    else
    {
        PError::SignalFatalError("Unable to allocate audio input buffer\n");
    }

    mWave->bytesPerSample= mBytesPerSample;
    mWave->audiolen = mBytesPerSample*length;
    mWave->audiopos = 0;
    mWave->recordpos = 0;
    mWave->counter = 0;
    mWave->name = NULL;

    //attach the buffer to the extern global buffer so that the callback can use it:
    gAudioBuffer = mWave.get();
#if 0
    cout << "---------------------------\n";
    cout << "Creating buffer: \n";
    cout << "Bytespersample: " << mBytesPerSample << endl;
    cout << "Size (samples):  "  << size << endl;
    cout << "Size (bytes):     " << mWave->audiolen << endl;
    cout << "freq     "<<mWave->spec.freq <<endl;
    cout << "length:  " <<mWave->audiolen<< endl;
    cout << "---------------------------\n";
#endif

    return true;
}


bool PlatformAudioIn::RecordToBuffer()
{
    if(mAudioDevice == 0)
    {
        PError::SignalWarning("Audio device not initialized");
        return false;
    }

    std::cout << "RecordToBuffer: Unpausing audio device " << mAudioDevice << "\n";
    SDL_PauseAudioDevice(mAudioDevice, 0);  // 0 = unpause/start recording

    // Check the status
    SDL_AudioStatus status = SDL_GetAudioDeviceStatus(mAudioDevice);
    std::cout << "Audio device status after unpause: " << status
              << " (SDL_AUDIO_PLAYING=" << SDL_AUDIO_PLAYING << ")\n";

    return true;
}


bool PlatformAudioIn::Stop()
{
    if(mAudioDevice == 0)
    {
        return false;
    }

    SDL_PauseAudioDevice(mAudioDevice, 1);  // 1 = pause/stop recording
    return true;
}


// simple voicekey.  It will process a buffer, computing power
// for 10-ms windows every 1 ms.  It will 'trip' when 95% of the 1-ms windows
// have power greater than the threshold for sustain, and stop when
// the power goes below the threshold for 95% of the time for sustain/2.
// It then reprocesses the power stream to find the point at which the
// power went above the threshold.

Variant PlatformAudioIn::VoiceKey(double threshold, unsigned int sustain)
{



    //how big a chunk, in samples, will 1 ms of time take up?

    int binspersec=1000;

    //this is the number of samples (not bytes) per chunk
    unsigned int chunksize = mSampleRate/binspersec;
    double msperchunk = (double)mSampleRate/chunksize/1000;

    //number of samples needed for the sustain parameter.
    unsigned int sustainSamples = sustain/msperchunk;


    //This should be conditional:
    //Initialize(1);
    //    CreateBuffer(samples);


    //audio will immediately be filling up the buffer.  When full,
    //recording will stop, but we can potentially stop it any time
    //prior to that too.


    //buffer duration in chunks (roughly 1-ms)
#if 0
    cout << "------------------------\n";
    cout <<"Computing buffer time\n";
    cout << "audiolen: " << mWave->audiolen << endl;
    cout << "bytespersample " << mWave->bytesPerSample << endl;
    cout << "bytes:         " << mBytesPerSample << endl;

    cout << "chunksize: " << chunksize << endl;
#endif

    int buffertime = double(mWave->audiolen)/mBytesPerSample/chunksize;
    //Make a power buffer equal to the number of ms in the sample buffer.
    std::vector<double> powerbins = std::vector<double>(buffertime);

    bool trip = false;
    bool stop = 0;
    unsigned int triptime = 0;
    unsigned int offtime = 0;  //The time the speech stops.
    unsigned int tickID=0;
    unsigned int sampleID=0;

    mWave->recordpos =0;
    mWave->counter = 0;
    //we should blank out the audio that is in here.
    memset(mWave->audio,0,mWave->audiolen);

    //start audio recording.
    RecordToBuffer();
#if 0
    cout << "recording\n";
    //    cout << "timeout:        " << timeout << endl;
    cout << "chunksize:      " << chunksize << endl;
    cout << "msperchunk:     " << msperchunk << endl;
    cout << "sustain:        " << sustain << endl;
    cout << "sustainsamples: " << sustainSamples << endl;
    cout << "power bins:     " << buffertime << endl;

#endif

    int abovecount = 0;

    //This is not thread-safe.


    double powr;
    double energy;
    double power;
    int signs;
    int directions;
    double rmssd;
    while(!stop)
        {


            //process another bin as long as the recording position is greater than
            //one bin ahead.  Transform into samples first.
            //cout << gAudioBuffer->recordpos/mBytesPerSample << ":" << (chunksize+sampleID) << endl;
            while((gAudioBuffer->recordpos/mBytesPerSample) > chunksize+sampleID)
                {
                    //cout <<"    buffering "<< sampleID <<":"<< gAudioBuffer->recordpos <<" " << (gAudioBuffer->recordpos - sampleID)  << "  " << samples << endl;

                    //power[tickID] = ;

                    ComputeStats((Sint16*)(gAudioBuffer->audio+sampleID*mBytesPerSample),chunksize,
                          energy,power,signs,directions,rmssd);
                    powerbins[tickID] = energy;
                    //powr = Power((Sint16*)(gAudioBuffer->audio+sampleID*mBytesPerSample),chunksize);

                    //The following produces 'mini-scopes for each statistic we compute.
#if 0

                    cout << SDL_GetTicks();
               //power
                    cout << "[";
                    int k;
                    for(k = 0; k< 10*power;k++)cout<<" "<<std::flush;
                    //cout << power << endl;
                    cout << "*";
                    for(int j=k; j<10; j++) cout << " ";
                    cout << "]";

                    //energy

                    cout << "[";
                    for(k = 0; k< 10*energy;k++)cout<<" "<<std::flush;
                    cout << "*";
                    for(int j=k; j<10; j++) cout << " ";
                    cout << "]";

                    //signs
                    cout << "[";
                    for(k = 0; k< 10.0*signs/chunksize;k++)cout<<" "<<std::flush;
                    cout << "*";
                    for(int j=k; j<10; j++) cout << " ";
                    cout << "]";

                    //directions
                    cout << "[";
                    for(k = 0; k< 10.0*directions/chunksize;k++)cout<<" "<<std::flush;
                    cout << "*";
                    for(int j=k; j<10; j++) cout << " ";
                    cout << "]";
                    cout << endl;


#endif

                    //                    cout << "X" << powr << " "  << energy << " " << power << " " << signs << " " << directions << " " << rmssd << endl;

                    //if(powerbins[tickID] > threshold)          cout << "********** " << abovecount <<endl;


                    int incoming = (powerbins[tickID]>threshold);
                    int outgoing = (tickID < sustainSamples)? 0:powerbins[tickID-sustainSamples]>threshold;
                    abovecount += incoming - outgoing;
                    //cout << ((double)abovecount)/sustainSamples ;
                    if(((double)abovecount)/sustainSamples > .55  &trip==false)
                                            {
                                                trip = true;
                                                triptime = tickID -(sustainSamples*.55);
                                                //cout << "!!!!!!!!!!!!!!!!!!VOICE KEY TRIPPED!!!!!!!!!!!!!!!!!!\n" ;
                                            }

                    if(trip)
                        {
                            //cout << "*****************";
                            //If we have tripped, see if 50% or more of the
                            //samples are below the threshold.
                            if((double)abovecount/sustainSamples < .2)
                                {
                                    //cout << "<<<<<<<<<<<<<";
                                    //stop recording.
                                    Stop();
                                    stop = true;
                                    offtime = tickID- (sustainSamples*.8);
                                }


                        }
                    //cout << endl;



                    tickID++;
                    sampleID += chunksize;


                    if(sampleID+chunksize >= gAudioBuffer->audiolen/mBytesPerSample)
                        stop = true;


                    //                    cout << sampleID << " > "<< gAudioBuffer->audiolen << " -----";
                    //                    cout <<tickID << " " << stop;
                }
            SDL_Delay(1);
        }



    // triptime = triptime * msperchunk
    // tripped = trip
    // offtime = offtime * msperchunk


    PList * newlist = new PList();
    newlist->PushBack(Variant(triptime * msperchunk));
    newlist->PushBack(Variant(offtime * msperchunk));
    newlist->PushBack(Variant(trip));
    //    cout << "Returning: " << *newlist << endl;

    counted_ptr<PEBLObjectBase> baselist = counted_ptr<PEBLObjectBase>(newlist);
    PComplexData * pcd = new PComplexData(baselist);


    //cout << "Saving to out.wav\n";
    //SaveBufferToWave("out.wav");

    return Variant(pcd);
}


void PlatformAudioIn::SaveBufferToWave(Variant filename)
{
    //Code here adapted from
    //http://www.codeproject.com/Messages/3208219/How-to-write-mic-data-to-wav-file.aspx

	int bitsPerSample = mBytesPerSample*8;

    //Unclear about these chunk things:
	int subchunk1size = 16;
	int numChannels = mWave->spec.channels;
	int subchunk2size = mWave->recordpos;
	int chunksize = 36+subchunk2size;


	int audioFormat = 1;  //PCM

    int sampleRate = mWave->spec.freq;
	int byteRate = mWave->spec.freq*numChannels*bitsPerSample/8;
	int blockAlign = numChannels*bitsPerSample/8;


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
	myFile.write ((char*) &subchunk2size, 4);			// subchunk2size (NumSamples * NumChannels * BitsPerSample/8)

	myFile.write ((char*)(mWave->audio), mWave->recordpos);	// data


}

void AudioInCallbackFill(void * udata, Uint8 * stream, int len)
{
    static int callbackCount = 0;
    callbackCount++;

    //len is in bytes.

    //SDL_AudioSpec *spec=(SDL_AudioSpec *)udata;
    //Sint16 *sData=(Sint16 *)stream;
    Uint8 * sData = stream;

    // int samples=(len/2);

    if(gAudioBuffer)
        {
            //This gives the number of  left in the buffer.
            int remaininbuffer = (gAudioBuffer->audiolen - gAudioBuffer->recordpos);


            //We want to copy up to len bytes.  But if only as many as remain in the buffer.
            //tocopy is how many bytes we can copy:
            int bytestocopy = (len< remaininbuffer ? len: remaininbuffer);

            if(callbackCount % 100 == 1) {  // Print every 100th callback
                // Check if stream has any non-zero data
                int nonZero = 0;
                for(int i = 0; i < len && i < 100; i++) {
                    if(sData[i] != 0) nonZero++;
                }
                std::cout << "AudioCallback #" << callbackCount
                          << ": len=" << len
                          << " recordpos=" << gAudioBuffer->recordpos
                          << " tocopy=" << bytestocopy
                          << " nonZeroInStream=" << nonZero << "/100" << std::endl;
            }

            //stop copying if the buffer is full.

            if(bytestocopy>0)
                {

                    //copy to the buffer
                    memcpy(gAudioBuffer->audio+(gAudioBuffer->recordpos),
                             sData,
                             bytestocopy);
                    gAudioBuffer->recordpos += bytestocopy;

                }
        }
    else
        {
            if(callbackCount == 1) {
                std::cout << "WARNING: AudioCallback called but gAudioBuffer is NULL!\n";
            }
        }


}


//This just repeatedly fills the buffer.

void AudioInCallbackLoop(void * udata, Uint8 * stream, int len)
{


#if 0

#endif

}






// Computes power for a specific range.
double PlatformAudioIn::Power (Sint16 * data, int length)
{

    double sum = 0;
    for(int i=0;i <length; i+=mBytesPerSample)
        {
            double tmp = (double)abs(data[i])/32768 ;
            sum += tmp;
            //cout << "     "  <<data[i] << " "<< tmp << endl;
        }

    //cout << "Power sum on  "<<length << "bytes: " << sum << ": " ;

    double power =(sum)/length;
    return power;
}





// Computes power for a specific range.
void PlatformAudioIn::ComputeStats (Sint16 * data, int length,
                                    double & power,
                                    double & energy,
                                    int & signchanges,
                                    int & dchanges,
                                    double & rmssd )
{

    //This computes several stats related to  detecting
    //onsets of speech:
    //sign change,
    //power
    //RMSSD


    double abssum = 0;
    double sqsum = 0;
    int signsum = 0;
    int dirsum = 0;
    double rmssdsum=0;

    double prev=0;
    double scaled;
    double delta = 0;
    double prevdelta = 0;

     for(int i=0;i <length; i+=mBytesPerSample)
        {


            scaled = ((double)data[i])/32768;
            //cout << "-----"<< scaled << "|" << data[i]<<endl;
            abssum += abs(scaled);
            sqsum += scaled*scaled;
            signsum += (scaled * prev)<0;

            delta = scaled - prev;
            dirsum += (delta * prevdelta) < 0;

            rmssdsum += pow(delta - prevdelta,2);


            //update
            prev = scaled;
            prevdelta = delta;

        }

    //cout << "Power sum on  "<<length << "bytes: " << sum << ": " ;

    int samples = length/mBytesPerSample;

    energy = abssum/samples;
    power  = sqrt(sqsum/samples);
    rmssd  = sqrt(rmssdsum/samples);
    signchanges = signsum;
    dchanges=dirsum;

}

#endif
