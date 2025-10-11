//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
///////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformDrawObject.h
//    Purpose:    Platform-specific classes drawing things.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2005-2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//   
//
//     This file is part of the PEBL project.
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
///////////////////////////////////////////////////////////////////////////////
#ifndef __PLATFORMDRAWOBJECT_H__
#define __PLATFORMDRAWOBJECT_H__

#include "PlatformWidget.h"
#include "../../objects/PDrawObject.h"
#include "../../objects/PColor.h"
#include "../../base/PEBLObject.h"
#include "SDL.h"



//A DrawObject is parallel to PWidget, but specific to drawing 
//primitives.  It differs from PWidgets because it doesn't need to be
// directly tied to a SDL_Surface.  If it is constructed with a NULL surface,
// it creates one itself, and cleans up after itself.  If it is created
// with a widget passed in, it does not destroy that widget when it is 
// destructed.  Thus, you can 'draw' on any other widget you want. 

class PlatformDrawObject: virtual public PlatformWidget
{

public:

    PlatformDrawObject(){};
    ~PlatformDrawObject(){};
    
};


class PlatformLine: virtual public PlatformDrawObject, virtual public PLine
{
public:
    PlatformLine(int x1, int y1, int dx, int dy, Variant fg);
    virtual ~PlatformLine();
    virtual bool Draw();
protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;
private:

};



class PlatformThickLine: virtual public PlatformDrawObject, virtual public PThickLine
{
public:
    PlatformThickLine(int x1, int y1, int x2, int y2,int width, Variant fg);
    virtual ~PlatformThickLine();
    virtual bool Draw();
protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;
private:

};

// class PlatformBezier: public PlatformDrawObject
// {
// };

// class PlatformPolygon: public PlatformDrawObject
// {
// public:
//     PlatformPlatformPolygon();
//     ~PlatformPlatformPolygon();
// protected:
// private:
// };

// class PlatformTriangle: public PlatformPolygon
// {
// public:
//     PlatformTriangle();
//     ~PlatformTriangle();
// protected:
// private:

// };

// class PlatformTrapezoid: public PlatformPolygon
// {
// public:
//     PlatformTrapezoid();
//     ~PlatformTrapezoid();
// protected:
// private:
// };


class PlatformRectangle:  virtual public PlatformDrawObject, virtual public PRectangle
 {
 public:
     PlatformRectangle(int x1, int y1, int dx, int dy, Variant fg, bool filled);
     virtual ~PlatformRectangle();
     virtual bool Draw();
 protected:
     virtual std::ostream & SendToStream(std::ostream& out) const;
 private:
 };

class PlatformSquare: virtual public PlatformDrawObject, virtual public PSquare
{
public:
    PlatformSquare(int x, int y, int size, Variant fg, bool filled);
    virtual ~PlatformSquare();
    virtual bool Draw();
protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;
private:
};


class PlatformEllipse:  virtual public PlatformDrawObject, virtual public PEllipse
 {
 public:
     PlatformEllipse(int x1, int y1, int dx, int dy, Variant fg, bool filled);
     virtual ~PlatformEllipse();
     virtual bool Draw();
 protected:
     virtual std::ostream & SendToStream(std::ostream& out) const;
 private:
 };


class PlatformCircle: virtual public PlatformDrawObject, virtual public PCircle
 {
 public:
     PlatformCircle(int x1, int y1, int r, Variant fg, bool filled);
     virtual ~PlatformCircle();
     virtual bool Draw();
 protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;
 private:
 };

 class PlatformPolygon: virtual public PlatformDrawObject, virtual public PPolygon
 {
 public:
     PlatformPolygon(int x, int y, Variant xpoints,Variant ypoints,Variant fg, bool filled);
     virtual ~PlatformPolygon();
     virtual bool Draw();
 protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;
 private:
 };

 class PlatformBezier: virtual public PlatformDrawObject, virtual public PBezier
 {
 public:
     PlatformBezier(int x, int y, Variant xpoints,Variant ypoints,int steps, Variant fg);
     virtual ~PlatformBezier();
     virtual bool Draw();
 protected:
    virtual std::ostream & SendToStream(std::ostream& out) const;
 private:
 };


// class PlatformDrawGroup: public PlatformDrawObject
// {
// public:
//     PlatformDrawGroup();//     ~PlatformDrawGroup();
// protected:
// private:
// };


#endif
