//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformWidget.cpp
//    Purpose:    Validator Platform Widget Implementation
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
#include "PlatformWidget.h"

PlatformWidget::PlatformWidget() : PWidget() {
}

PlatformWidget::PlatformWidget(pInt x, pInt y, pInt width, pInt height, bool visible)
    : PWidget(x, y, width, height, visible) {
}

PlatformWidget::~PlatformWidget() {
}

bool PlatformWidget::Draw() {
    // No-op: no rendering in validator mode
    return true;
}

bool PlatformWidget::RotoZoom(pDouble angle, pDouble zoomx, pDouble zoomy, pInt smooth) {
    // Store the values but don't render
    mRotation = angle;
    mZoomX = zoomx;
    mZoomY = zoomy;
    return true;
}

bool PlatformWidget::SetPoint(pInt x, pInt y, PColor col) {
    // No-op: no pixel manipulation in validator mode
    return false;
}

PColor PlatformWidget::GetPixel(pInt x, pInt y) {
    // Return black - no pixel data available
    return PColor(0, 0, 0, 255);
}

std::ostream& PlatformWidget::SendToStream(std::ostream& out) const {
    out << "PlatformWidget (validator)";
    return out;
}
