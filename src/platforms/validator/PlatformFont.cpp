//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformFont.cpp
//    Purpose:    Validator Platform Font Implementation
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
#include "PlatformFont.h"

PlatformFont::PlatformFont(const std::string& filename)
    : PFont(filename, 0, 18, PColor(0,0,0,255), PColor(255,255,255,255), true) {
}

PlatformFont::PlatformFont(const std::string& filename, int style, int size,
                           PColor fgcolor, PColor bgcolor, bool aa)
    : PFont(filename, style, size, fgcolor, bgcolor, aa) {
}

PlatformFont::PlatformFont(PlatformFont& font)
    : PFont(font) {
}

PlatformFont::~PlatformFont() {
}

unsigned int PlatformFont::GetTextWidth(const std::string& text) {
    // Dummy metric: text length * size * 0.6
    return static_cast<unsigned int>(text.length() * mFontSize * 0.6);
}

unsigned int PlatformFont::GetTextHeight(const std::string& text) {
    // Dummy metric: font size
    return static_cast<unsigned int>(mFontSize);
}

std::ostream& PlatformFont::SendToStream(std::ostream& out) const {
    out << "PlatformFont (validator)";
    return out;
}
