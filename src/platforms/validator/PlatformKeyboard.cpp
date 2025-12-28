//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
#include "PlatformKeyboard.h"

PlatformKeyboard::PlatformKeyboard() {}
PlatformKeyboard::~PlatformKeyboard() {}

PEBL_Keycode PlatformKeyboard::IsKeyDown(PEBL_Keycode code) const {
    return PEBL_KEYCODE_NOTHING; // No keys pressed in validator mode
}

bool PlatformKeyboard::IsKeyUp(PEBL_Keycode code) const {
    return true; // All keys always up in validator mode
}

std::ostream& PlatformKeyboard::SendToStream(std::ostream& out) const {
    out << "PlatformKeyboard (validator)";
    return out;
}
