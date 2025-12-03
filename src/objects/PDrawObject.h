//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
///////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PDrawObject.h
//    Purpose:    Utility class drawing things.
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
#ifndef __PDRAWOBJECT_H__
#define __PDRAWOBJECT_H__

#include "PWidget.h"
#include "PColor.h"
#include "../utility/Defs.h"
#include "../utility/rc_ptrs.h"

#include <vector>

//A DrawObject is parallel to PWidget, but specific to drawing 
//primitives.
class PDrawObject: virtual public PWidget

{

public:

    PDrawObject();
    //PDrawObject(int x, int y, const PColor & fg, const PColor & outline);
    //PDrawObject(int x, int y, const PColor & fg, const PColor & outline, bool filled);
    virtual ~PDrawObject();
    virtual void SetFilled(bool filled);
    virtual void SetColor(Variant  col);
    virtual void SetOutlineColor(Variant ocol);
    virtual void SetAA(bool aa);
    //overloaded generic PEBLObjectBase methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const;

protected:

    ///An inheritable printing class used by PEBLObjectBase::operator<<
    virtual std::ostream & SendToStream(std::ostream& out) const=0;

    // Helper method to get color from property system
    PColor* GetColor() const;
    PColor* GetOutlineColor() const;

    bool mFilled;
    bool mAntiAliased;
private:

};


class PLine: public PDrawObject
{
public:
    PLine(pInt x1, pInt y1, pInt dx, pInt dy, Variant fg);
    virtual ~PLine();
    virtual pInt GetWidth() const{return mDX;};
    virtual pInt GetHeight() const{return mDY;};
    virtual void SetSize(pInt dx, pInt dy);
    virtual std::ostream & SendToStream(std::ostream& out);



    //overloaded generic PEBLObjectBase methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const; 


protected:
    //X1--Y2 are relative to mX and mY
    pInt mDX;  
    pInt mDY;  



private:
};

class PThickLine: public PDrawObject
{
public:
    PThickLine(pInt x1, pInt y1, pInt x2, pInt y2, pInt width, Variant fg);
    virtual ~PThickLine();

    virtual std::ostream & SendToStream(std::ostream& out);

    virtual void SetPosition(pInt x, pInt y);
    virtual void SetEnds(pInt x1, pInt y1,pInt x2, pInt y2);
    virtual void SetThickness(pInt t);


    //overloaded generic PEBLObjectBase methods
    virtual bool SetProperty(std::string, Variant v);
    virtual Variant GetProperty(std::string)const;
    virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
    virtual ObjectValidationError ValidateProperty(std::string)const; 


protected:
    //X1--Y2 are relative to mX and mY

    pInt mX1;  
    pInt mY1;  

    pInt mX2;  
    pInt mY2;  

    pInt mThickness;

private:
};












// class PBezier: public PDrawObject
// {
// };
// class PTriangle: public PPolygon
// {
// public:
//     PTriangle();
//     ~PTriangle();
// protected:
// private:

// };

// class PTrapezoid: public PPolygon
// {
// public:
//     PTrapezoid();
//     ~PTrapezoid();
// protected:
// private:
// };



//For rectangles/squares, the 'position' is at the center of the object.
//
 class PRectangle: public PDrawObject
 {
 public:
     PRectangle(pInt x1, pInt y1, pInt dx, pInt dy, Variant fg, bool filled);
     virtual ~PRectangle();
     virtual pInt GetWidth() const{return mDX;};
     virtual pInt GetHeight() const{return mDY;};
     virtual void SetSize(pInt dx, pInt dy);
     virtual std::ostream & SendToStream(std::ostream& out);

     //overloaded generic PEBLObjectBase methods
     virtual bool SetProperty(std::string, Variant v);
     virtual Variant GetProperty(std::string)const;
     virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
     virtual ObjectValidationError ValidateProperty(std::string)const;


 protected:
     pInt mDX;
     pInt mDY;
 private:
 };

 class PSquare: public PRectangle
 {
 public:
     PSquare(pInt x, pInt y, pInt size, Variant fg, bool filled);
     virtual ~PSquare();
     virtual pInt GetWidth() const{return mDX;};
     virtual pInt GetHeight() const{return mDY;};
     virtual void SetSize(pInt size);
     virtual std::ostream & SendToStream(std::ostream& out);
     


     //overloaded generic PEBLObjectBase methods
     virtual bool SetProperty(std::string, Variant v);
     virtual Variant GetProperty(std::string)const;
     virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
     virtual ObjectValidationError ValidateProperty(std::string)const;

 protected:
 private:
 };


class PEllipse: public  PDrawObject
 {
 public:
     PEllipse(pInt x1, pInt y1, pInt rx, pInt ry, Variant fg, bool filled);
     virtual ~PEllipse();
     virtual pInt GetWidth() const{return mRX;};
     virtual pInt GetHeight() const{return mRY;};
     virtual void SetSize(pInt dx, pInt dy);
     virtual std::ostream & SendToStream(std::ostream& out);
     

     //overloaded generic PEBLObjectBase methods
     virtual bool SetProperty(std::string, Variant v);
     virtual Variant GetProperty(std::string)const;
     virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
     virtual ObjectValidationError ValidateProperty(std::string)const;


 protected:
     
     //X1--Y2 are relative to mX and mY
     pInt mRX;
     pInt mRY;  


 private:
};


 class PCircle: public PDrawObject
 {
 public:
     PCircle(pInt x1, pInt y1, pInt r, Variant fg, bool filled);
     virtual ~PCircle();

     virtual pInt GetWidth() const{return (pInt)(2*mR);};
     virtual pInt GetHeight() const{return (pInt)(2*mR);};
     virtual std::ostream & SendToStream(std::ostream& out);

     virtual void SetSize(pInt r);

    //overloaded generic PEBLObjectBase methods
     virtual bool SetProperty(std::string, Variant v);
     virtual Variant GetProperty(std::string)const;
     virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
     virtual ObjectValidationError ValidateProperty(std::string)const;


 protected:

     pDouble mR;
 private:
 };



 class PPolygon: public PDrawObject
 {
 public:
     PPolygon(pInt x, pInt y, Variant xpoints, Variant ypoints,  Variant fg, bool filled);
     virtual ~PPolygon();

     virtual pInt GetWidth() const{return mWidth;};
     virtual pInt GetHeight() const{return mHeight;};

     virtual bool SetXPoints(Variant v);
     virtual bool SetYPoints(Variant v);

     virtual std::ostream & SendToStream(std::ostream& out);


    //overloaded generic PEBLObjectBase methods
     virtual bool SetProperty(std::string, Variant v);
     virtual Variant GetProperty(std::string)const;
     virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
     virtual ObjectValidationError ValidateProperty(std::string)const;


 protected:

     Variant mXPoints;
     Variant mYPoints;

 private:
 };


 class PBezier: public PDrawObject
 {
 public:
     PBezier(pInt x, pInt y, Variant xpoints, Variant ypoints,  pInt steps, Variant fg);
     virtual ~PBezier();

     virtual pInt GetWidth() const{return mWidth;};
     virtual pInt GetHeight() const{return mHeight;};

     virtual bool SetXPoints(Variant v);
     virtual bool SetYPoints(Variant v);
     virtual bool SetSteps(Variant v);

     virtual std::ostream & SendToStream(std::ostream& out);


    //overloaded generic PEBLObjectBase methods
     virtual bool SetProperty(std::string, Variant v);
     virtual Variant GetProperty(std::string)const;
     virtual ObjectValidationError ValidateProperty(std::string, Variant v)const;
     virtual ObjectValidationError ValidateProperty(std::string)const;


 protected:

     Variant mXPoints;
     Variant mYPoints;
     pInt mSteps;
 private:
 };

#endif





