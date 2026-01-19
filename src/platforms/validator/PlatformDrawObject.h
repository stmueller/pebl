//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       platforms/validator/PlatformDrawObject.h
//    Purpose:    Validator Platform Drawing Primitives (no rendering stubs)
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
#ifndef __VALIDATOR_PLATFORMDRAWOBJECT_H__
#define __VALIDATOR_PLATFORMDRAWOBJECT_H__

#include "PlatformWidget.h"
#include "../../objects/PDrawObject.h"

/// Validator platform drawing primitives - no rendering, used only for compilation

class PlatformDrawObject : virtual public PlatformWidget {
public:
    PlatformDrawObject() {};
    ~PlatformDrawObject() {};
};

class PlatformLine : virtual public PlatformDrawObject, virtual public PLine {
public:
    PlatformLine(int x1, int y1, int dx, int dy, Variant fg);
    virtual ~PlatformLine();
    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformLine (validator)"; }
    virtual bool SetProperty(std::string name, Variant v) { return PLine::SetProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const { return PLine::ValidateProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name) const { return PLine::ValidateProperty(name); }
    virtual void SetPosition(pInt x, pInt y) { PLine::SetPosition(x, y); }
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

class PlatformThickLine : virtual public PlatformDrawObject, virtual public PThickLine {
public:
    PlatformThickLine(int x1, int y1, int x2, int y2, int width, Variant fg);
    virtual ~PlatformThickLine();
    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformThickLine (validator)"; }
    virtual bool SetProperty(std::string name, Variant v) { return PThickLine::SetProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const { return PThickLine::ValidateProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name) const { return PThickLine::ValidateProperty(name); }
    virtual void SetPosition(pInt x, pInt y) { PThickLine::SetPosition(x, y); }
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

class PlatformRectangle : virtual public PlatformDrawObject, virtual public PRectangle {
public:
    PlatformRectangle(int x1, int y1, int dx, int dy, Variant fg, bool filled);
    virtual ~PlatformRectangle();
    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformRectangle (validator)"; }
    virtual bool SetProperty(std::string name, Variant v) { return PRectangle::SetProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const { return PRectangle::ValidateProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name) const { return PRectangle::ValidateProperty(name); }
    virtual void SetPosition(pInt x, pInt y) { PRectangle::SetPosition(x, y); }
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

class PlatformSquare : virtual public PlatformDrawObject, virtual public PSquare {
public:
    PlatformSquare(int x, int y, int size, Variant fg, bool filled);
    virtual ~PlatformSquare();
    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformSquare (validator)"; }
    virtual bool SetProperty(std::string name, Variant v) { return PSquare::SetProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const { return PSquare::ValidateProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name) const { return PSquare::ValidateProperty(name); }
    virtual void SetPosition(pInt x, pInt y) { PSquare::SetPosition(x, y); }
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

class PlatformEllipse : virtual public PlatformDrawObject, virtual public PEllipse {
public:
    PlatformEllipse(int x1, int y1, int rx, int ry, Variant fg, bool filled);
    virtual ~PlatformEllipse();
    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformEllipse (validator)"; }
    virtual bool SetProperty(std::string name, Variant v) { return PEllipse::SetProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const { return PEllipse::ValidateProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name) const { return PEllipse::ValidateProperty(name); }
    virtual void SetPosition(pInt x, pInt y) { PEllipse::SetPosition(x, y); }
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

class PlatformCircle : virtual public PlatformDrawObject, virtual public PCircle {
public:
    PlatformCircle(int x1, int y1, int r, Variant fg, bool filled);
    virtual ~PlatformCircle();
    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformCircle (validator)"; }
    virtual bool SetProperty(std::string name, Variant v) { return PCircle::SetProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const { return PCircle::ValidateProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name) const { return PCircle::ValidateProperty(name); }
    virtual void SetPosition(pInt x, pInt y) { PCircle::SetPosition(x, y); }
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

class PlatformPolygon : virtual public PlatformDrawObject, virtual public PPolygon {
public:
    PlatformPolygon(int x, int y, Variant xpoints, Variant ypoints, Variant fg, bool filled);
    virtual ~PlatformPolygon();
    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformPolygon (validator)"; }
    virtual bool SetProperty(std::string name, Variant v) { return PPolygon::SetProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const { return PPolygon::ValidateProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name) const { return PPolygon::ValidateProperty(name); }
    virtual void SetPosition(pInt x, pInt y) { PPolygon::SetPosition(x, y); }
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

class PlatformBezier : virtual public PlatformDrawObject, virtual public PBezier {
public:
    PlatformBezier(int x, int y, Variant xpoints, Variant ypoints, int steps, Variant fg);
    virtual ~PlatformBezier();
    virtual bool Draw();
    virtual std::string ObjectName() const { return "PlatformBezier (validator)"; }
    virtual bool SetProperty(std::string name, Variant v) { return PBezier::SetProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name, Variant v) const { return PBezier::ValidateProperty(name, v); }
    virtual ObjectValidationError ValidateProperty(std::string name) const { return PBezier::ValidateProperty(name); }
    virtual void SetPosition(pInt x, pInt y) { PBezier::SetPosition(x, y); }
protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;
};

#endif
