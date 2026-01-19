//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformFont.h
//    Purpose:    Validator Platform Font (dummy metrics)
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//   This file is part of the PEBL project.
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
#ifndef __VALIDATOR_PLATFORMFONT_H__
#define __VALIDATOR_PLATFORMFONT_H__

#include "../../objects/PFont.h"

/// Validator platform font - provides dummy text metrics
/// Used only for compilation/linking, never executed
class PlatformFont : virtual public PFont {
public:
    PlatformFont(const std::string& filename);
    PlatformFont(const std::string& filename, int style, int size,
                 PColor fgcolor, PColor bgcolor, bool aa);
    PlatformFont(PlatformFont& font);
    virtual ~PlatformFont();

    // Dummy text metrics - approximate dimensions
    unsigned int GetTextWidth(const std::string& text);
    unsigned int GetTextHeight(const std::string& text);

protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

#endif
