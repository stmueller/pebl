//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
#ifndef __VALIDATOR_PLATFORMAUDIOOUT_H__
#define __VALIDATOR_PLATFORMAUDIOOUT_H__

#include "../../devices/PAudioOut.h"
#include "../../base/PEBLObject.h"

class PlatformAudioOut : virtual public PAudioOut, public PEBLObjectBase {
public:
    PlatformAudioOut();
    PlatformAudioOut(const std::string& filename);
    virtual ~PlatformAudioOut();
    virtual bool LoadSoundFile(const std::string& filename);
    virtual bool Play();
    virtual bool Stop();
    virtual int SaveBufferToWave(const std::string& filename) { return 0; }  // Stub for validator
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

#endif
