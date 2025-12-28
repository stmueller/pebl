//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
#include "PlatformNetwork.h"

PlatformNetwork::PlatformNetwork() {}
PlatformNetwork::~PlatformNetwork() {}

std::ostream& PlatformNetwork::SendToStream(std::ostream& out) const {
    out << "PlatformNetwork (validator)";
    return out;
}
