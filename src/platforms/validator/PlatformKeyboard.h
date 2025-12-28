//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
#ifndef __VALIDATOR_PLATFORMKEYBOARD_H__
#define __VALIDATOR_PLATFORMKEYBOARD_H__

#include "../../devices/PKeyboard.h"

class PlatformKeyboard : public PKeyboard {
public:
    PlatformKeyboard();
    virtual ~PlatformKeyboard();
    virtual PEBL_Keycode IsKeyDown(PEBL_Keycode code) const;
    virtual bool IsKeyUp(PEBL_Keycode code) const;
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

#endif
