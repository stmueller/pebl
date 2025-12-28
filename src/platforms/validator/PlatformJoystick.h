//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
#ifndef __VALIDATOR_PLATFORMJOYSTICK_H__
#define __VALIDATOR_PLATFORMJOYSTICK_H__

#include "../../devices/PJoystick.h"

class PlatformJoystick : public PJoystick {
public:
    PlatformJoystick();
    PlatformJoystick(int id);
    virtual ~PlatformJoystick();
    virtual signed int GetHatState(unsigned int hat);
    virtual Variant GetBallState(unsigned int ball);
    virtual signed int GetAxisState(unsigned int axis);
    virtual signed int GetButtonState(unsigned int button);
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

#endif
