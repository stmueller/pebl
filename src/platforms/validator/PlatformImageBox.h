//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformImageBox.h
//    Purpose:    Validator Platform ImageBox (no image loading stub)
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
#ifndef __VALIDATOR_PLATFORMIMAGEBOX_H__
#define __VALIDATOR_PLATFORMIMAGEBOX_H__

#include "PlatformWidget.h"
#include "../../objects/PImageBox.h"

/// Validator platform image box - no image loading, used only for compilation
class PlatformImageBox : virtual public PlatformWidget, virtual public PImageBox {
public:
    PlatformImageBox();
    PlatformImageBox(const std::string& filename);
    virtual ~PlatformImageBox();

    virtual bool LoadImage(const std::string& imagefilename);
    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformImageBox (validator)"; }

    // Resolve ambiguity by explicitly delegating to PImageBox
    virtual bool SetProperty(std::string name, Variant v) {
        return PImageBox::SetProperty(name, v);
    }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const {
        return PImageBox::ValidateProperty(name, v);
    }
    virtual ObjectValidationError ValidateProperty(std::string name) const {
        return PImageBox::ValidateProperty(name);
    }

protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

#endif
