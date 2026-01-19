//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformDrawObject.cpp
//    Purpose:    Validator Platform Drawing Primitives Implementation
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
#include "PlatformDrawObject.h"

// All drawing primitives are stubs for validator - no actual rendering

PlatformLine::PlatformLine(int x1, int y1, int dx, int dy, Variant fg)
    : PLine(x1, y1, dx, dy, fg) {}
PlatformLine::~PlatformLine() {}
bool PlatformLine::Draw() { return true; }
std::ostream& PlatformLine::SendToStream(std::ostream& out) const {
    out << "PlatformLine (validator)"; return out;
}

PlatformThickLine::PlatformThickLine(int x1, int y1, int x2, int y2, int width, Variant fg)
    : PThickLine(x1, y1, x2, y2, width, fg) {}
PlatformThickLine::~PlatformThickLine() {}
bool PlatformThickLine::Draw() { return true; }
std::ostream& PlatformThickLine::SendToStream(std::ostream& out) const {
    out << "PlatformThickLine (validator)"; return out;
}

PlatformRectangle::PlatformRectangle(int x1, int y1, int dx, int dy, Variant fg, bool filled)
    : PRectangle(x1, y1, dx, dy, fg, filled) {}
PlatformRectangle::~PlatformRectangle() {}
bool PlatformRectangle::Draw() { return true; }
std::ostream& PlatformRectangle::SendToStream(std::ostream& out) const {
    out << "PlatformRectangle (validator)"; return out;
}

PlatformSquare::PlatformSquare(int x, int y, int size, Variant fg, bool filled)
    : PSquare(x, y, size, fg, filled) {}
PlatformSquare::~PlatformSquare() {}
bool PlatformSquare::Draw() { return true; }
std::ostream& PlatformSquare::SendToStream(std::ostream& out) const {
    out << "PlatformSquare (validator)"; return out;
}

PlatformEllipse::PlatformEllipse(int x1, int y1, int rx, int ry, Variant fg, bool filled)
    : PEllipse(x1, y1, rx, ry, fg, filled) {}
PlatformEllipse::~PlatformEllipse() {}
bool PlatformEllipse::Draw() { return true; }
std::ostream& PlatformEllipse::SendToStream(std::ostream& out) const {
    out << "PlatformEllipse (validator)"; return out;
}

PlatformCircle::PlatformCircle(int x1, int y1, int r, Variant fg, bool filled)
    : PCircle(x1, y1, r, fg, filled) {}
PlatformCircle::~PlatformCircle() {}
bool PlatformCircle::Draw() { return true; }
std::ostream& PlatformCircle::SendToStream(std::ostream& out) const {
    out << "PlatformCircle (validator)"; return out;
}

PlatformPolygon::PlatformPolygon(int x, int y, Variant xpoints, Variant ypoints, Variant fg, bool filled)
    : PPolygon(x, y, xpoints, ypoints, fg, filled) {}
PlatformPolygon::~PlatformPolygon() {}
bool PlatformPolygon::Draw() { return true; }
std::ostream& PlatformPolygon::SendToStream(std::ostream& out) const {
    out << "PlatformPolygon (validator)"; return out;
}

PlatformBezier::PlatformBezier(int x, int y, Variant xpoints, Variant ypoints, int steps, Variant fg)
    : PBezier(x, y, xpoints, ypoints, steps, fg) {}
PlatformBezier::~PlatformBezier() {}
bool PlatformBezier::Draw() { return true; }
std::ostream& PlatformBezier::SendToStream(std::ostream& out) const {
    out << "PlatformBezier (validator)"; return out;
}
