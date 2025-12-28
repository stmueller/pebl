//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
#ifndef __VALIDATOR_PLATFORMNETWORK_H__
#define __VALIDATOR_PLATFORMNETWORK_H__

#include "../../devices/PNetwork.h"

class PlatformNetwork : public PNetwork {
public:
    PlatformNetwork();
    virtual ~PlatformNetwork();
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

#endif
