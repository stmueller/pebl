//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
#include "PlatformAudioOut.h"

PlatformAudioOut::PlatformAudioOut() : PAudioOut(), PEBLObjectBase(CDT_AUDIOOUT) {}
PlatformAudioOut::PlatformAudioOut(const std::string& filename) : PAudioOut(filename), PEBLObjectBase(CDT_AUDIOOUT) {}
PlatformAudioOut::~PlatformAudioOut() {}

bool PlatformAudioOut::LoadSoundFile(const std::string& filename) { return true; }
bool PlatformAudioOut::Play() { return true; }
bool PlatformAudioOut::Stop() { return true; }

std::ostream& PlatformAudioOut::SendToStream(std::ostream& out) const {
    out << "PlatformAudioOut (validator)";
    return out;
}
