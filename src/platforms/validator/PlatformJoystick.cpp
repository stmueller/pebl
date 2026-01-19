//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
#include "PlatformJoystick.h"

PlatformJoystick::PlatformJoystick() {}
PlatformJoystick::PlatformJoystick(int id) {}
PlatformJoystick::~PlatformJoystick() {}

signed int PlatformJoystick::GetHatState(unsigned int hat) { return 0; }
Variant PlatformJoystick::GetBallState(unsigned int ball) { return Variant(0); }
signed int PlatformJoystick::GetAxisState(unsigned int axis) { return 0; }
signed int PlatformJoystick::GetButtonState(unsigned int button) { return 0; }

std::ostream& PlatformJoystick::SendToStream(std::ostream& out) const {
    out << "PlatformJoystick (validator)";
    return out;
}
