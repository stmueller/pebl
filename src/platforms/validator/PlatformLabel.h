//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformLabel.h
//    Purpose:    Validator Platform Label (no rendering stub)
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
#ifndef __VALIDATOR_PLATFORMLABEL_H__
#define __VALIDATOR_PLATFORMLABEL_H__

#include "PlatformWidget.h"
#include "../../objects/PLabel.h"
#include "../../utility/rc_ptrs.h"

/// Validator platform label - no rendering, used only for compilation
class PlatformLabel : virtual public PlatformWidget, virtual public PLabel {
public:
    PlatformLabel(const std::string& text, counted_ptr<PEBLObjectBase> font);
    PlatformLabel(PlatformLabel& label);
    virtual ~PlatformLabel();

    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformLabel (validator)"; }
    virtual void SetFont(counted_ptr<PEBLObjectBase> font) {}  // Stub for validator

    // Resolve ambiguity by explicitly delegating to PLabel
    virtual bool SetProperty(std::string name, Variant v) { return PLabel::SetProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const { return PLabel::ValidateProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name) const { return PLabel::ValidateProperty(name); }

    // Resolve SetPosition ambiguity
    virtual void SetPosition(pInt x, pInt y) { PLabel::SetPosition(x, y); }

protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;

private:
    counted_ptr<PEBLObjectBase> mFontObject;
};

#endif
