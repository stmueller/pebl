//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
///////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PDrawObject.cpp
//    Purpose:    Platform-generic classes drawing things.
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

#include "PDrawObject.h"
#include "PWidget.h"
#include "../base/PComplexData.h"
#include <list>

using std::list;
using std::cout;
using std::endl;

PDrawObject::PDrawObject()
{

    InitializeProperty("FILLED",Variant(0));
    InitializeProperty("COLOR",Variant(0));
    InitializeProperty("OUTLINECOLOR",Variant(0));
    InitializeProperty("AA",Variant(1));

    SetAA(0);
}


PDrawObject::~PDrawObject()
{
}


bool PDrawObject::SetProperty(std::string name, Variant v)
{
    if(!PWidget::SetProperty(name,v))
    {

        if (name == "FILLED")
            {
                SetFilled(v);
            }
        else if(name == "COLOR")
            {
                SetColor(v);
                
            }
        else  if (name == "OUTLINECOLOR")
            {
                SetOutlineColor(v);
            }
        else  if(name == "AA")
            {
                SetAA(v);
            }
        else
            {
                return false;
            }
    return true;
    
    }
    else
    {
        
        return false;
    }
    
    
}


Variant PDrawObject::GetProperty(std::string name)const
{

    return PEBLObjectBase::GetProperty(name);
}

 
ObjectValidationError PDrawObject::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PDrawObject::ValidateProperty(std::string name)const
{


    ObjectValidationError tmp = PWidget::ValidateProperty(name);
    
    if(tmp == OVE_VALID ) return OVE_VALID;
    else
        {
        if(name=="NAME" ||
           name == "FILLED" ||
           name == "COLOR" ||
           name =="OUTLINECOLOR"||
           name=="AA")
            return OVE_VALID;
        else return OVE_INVALID_PROPERTY_NAME;
        }
}

void PDrawObject::SetFilled(bool filled)
{
    mFilled = filled;
    PEBLObjectBase::SetProperty("FILLED",filled);
}

void PDrawObject::SetAA(bool aa)
{

    PEBLObjectBase::SetProperty("AA",aa);
    mAntiAliased = aa;

}

void PDrawObject::SetColor(Variant col)
{

    PEBLObjectBase::SetProperty("COLOR",col);
    mColor = *dynamic_cast<PColor*>(col.GetComplexData()->GetObject().get());

}

void  PDrawObject::SetOutlineColor(Variant  ocol)
{
    PEBLObjectBase::SetProperty("OUTLINECOLOR",ocol);
    mOutlineColor = *dynamic_cast<PColor*>(ocol.GetComplexData()->GetObject().get());
}


PLine::PLine(pInt x1, pInt y1, pInt dx, pInt dy, Variant col):
    PDrawObject(),
    mDX(dx),
    mDY(dy)
{
    InitializeProperty("NAME",Variant("<LINE>"));
    InitializeProperty("WIDTH",dx);
    InitializeProperty("HEIGHT",dy);

    SetPosition(x1,y1);
    PDrawObject::SetAA(1);
    PDrawObject::SetColor(col);

}

PLine::~PLine()
{
}


void PLine::SetSize(pInt dx, pInt dy)
{

    mDX = dx;
    mDY = dy;
    PEBLObjectBase::SetProperty("WIDTH",dx);
    PEBLObjectBase::SetProperty("HEIGHT",dy);
}


bool PLine::SetProperty(std::string name, Variant v)
{

    if(PDrawObject::SetProperty(name,v))
        {
            return true;
        } else {

        // Note, currently the properties dx and dy are not user-accessible.  Use height and width.
        //
            
            if("WIDTH" == name) 
                SetSize(v,mDY);
            else if ("HEIGHT" == name)
                SetSize(mDX, v);
            else
                return false;
        }
    
    return true;
}


Variant PLine::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);
}


ObjectValidationError PLine::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}


ObjectValidationError PLine::ValidateProperty(std::string name)const
{

   if(OVE_VALID == PDrawObject::ValidateProperty(name))
       {
           return OVE_VALID;
       }
   else
       {
           if("WIDTH" == name || "HEIGHT" == name )
               return OVE_VALID;
           else return OVE_INVALID_PROPERTY_NAME;
       }
}


std::ostream & PLine::SendToStream(std::ostream& out)
{
  out << "PLine";
  return out;
}



PThickLine::PThickLine(pInt x1, pInt y1, pInt x2, pInt y2, pInt width, Variant col):
    PDrawObject(),
    mX1(x1),
    mY1(y1),
    mX2(x2),
    mY2(y2),
    mThickness(width)
    
{


    InitializeProperty("NAME",Variant("<THICKLINE>"));
    InitializeProperty("X1",x1);
    InitializeProperty("Y1",y1);
    InitializeProperty("X2",x2);
    InitializeProperty("Y2",y2);
    InitializeProperty("THICKNESS",width);

    SetEnds(x1,y1,x2,y2);

    PDrawObject::SetAA(1);
    PDrawObject::SetColor(col);

}

PThickLine::~PThickLine()
{
}


void PThickLine::SetPosition(pInt x, pInt y)
{


    int deltaX =  (int)(x - mX);
    int deltaY =  (int)(y - mY);

    PWidget::SetPosition(x,y);

    SetEnds(mX1 + deltaX,mY1 + deltaY,mX2+deltaX,mY2+deltaY);

}

void PThickLine::SetEnds(pInt x1, pInt y1,pInt x2, pInt y2)
{


    mX1 = x1;
    mY1 = y1;
    mX2 = x2;
    mY2 = y2;
    
    mX = (x1 + x2)/2;
    mY = (y1 + y2)/2;
    
    PEBLObjectBase::SetProperty("X1",x1);
    PEBLObjectBase::SetProperty("X2",x2);
    PEBLObjectBase::SetProperty("Y1",y1);
    PEBLObjectBase::SetProperty("Y2",y2);
 }



void PThickLine::SetThickness(pInt t)
{
    mThickness = t;
    PEBLObjectBase::SetProperty("THICKNESS",t);
 }


bool PThickLine::SetProperty(std::string name, Variant v)
{

    if(PDrawObject::SetProperty(name,v))
        {
            return true;
        } else {

        // Note, currently the properties dx and dy are not user-accessible.  Use height and width.
        //
            
            if("X1" == name) 
                SetEnds(v,mY1,mX2,mY2);
            else if ("Y1" == name)
                SetEnds(mX1,v,mX2,mY2);
            else if ("X2" == name)
                SetEnds(mX1,mY1,v,mY2);
            else if ("Y2" == name)
                SetEnds(mX1,mY1,mX2,v);
            else if ("THICKNESS" == name)
                SetThickness(v);
            else
                return false;
        }
    
    return true;
}


Variant PThickLine::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);
}


ObjectValidationError PThickLine::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}


ObjectValidationError PThickLine::ValidateProperty(std::string name)const
{

   if(OVE_VALID == PDrawObject::ValidateProperty(name))
       {
           return OVE_VALID;
       }
   else
       {
           if("X1" == name || "X2" == name  ||
              "Y1" == name || "Y2" == name  ||
              "THICKNESS" == name )
               return OVE_VALID;
           else return OVE_INVALID_PROPERTY_NAME;
       }
}


std::ostream & PThickLine::SendToStream(std::ostream& out)
{
  out << "PThickLine";
  return out;
}



PRectangle::PRectangle(pInt x1, pInt y1, pInt dx, pInt dy, Variant col, bool filled)
{
    InitializeProperty("NAME",Variant("<RECTANGLE>")); 
    InitializeProperty("WIDTH",dx);
    InitializeProperty("HEIGHT",dy);
    SetSize(dx,dy);
    SetPosition(x1,y1);
    SetFilled(filled);
    PDrawObject::SetColor(col);
    PDrawObject::SetOutlineColor(col);
}


PRectangle:: ~PRectangle()
{
}

void PRectangle::SetSize(pInt dx, pInt dy)
{
    mDX = dx;
    mDY = dy;
    PEBLObjectBase::SetProperty("WIDTH",dx);
    PEBLObjectBase::SetProperty("HEIGHT",dy);
    Draw();
}

bool PRectangle::SetProperty(std::string name, Variant v)
{

    if(!PDrawObject::SetProperty(name,v))
    {
        if("WIDTH" == name)
        {
            SetSize(v,mDY);
        }
        else if("HEIGHT" == name)
        {
            SetSize(mDX,v);
        }
        else
        {
            return false;
        }

        }
    return true;
}


Variant PRectangle::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);

}


ObjectValidationError PRectangle::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PRectangle::ValidateProperty(std::string name)const
{
    if(OVE_VALID == PDrawObject::ValidateProperty(name))
       {
           return OVE_VALID;
       }
    else
        {
            if("WIDTH" == name 
               || "HEIGHT" == name )
                return OVE_VALID;
            else return OVE_INVALID_PROPERTY_NAME;
        }
    
}

std::ostream & PRectangle::SendToStream(std::ostream& out)
{
    out <<"PRectangle";
    return out;
}




PSquare::PSquare(pInt x, pInt y, pInt size, Variant col, bool filled):
    PRectangle(x,y,size,size,col,filled)
{
    InitializeProperty("NAME",Variant("<SQUARE>")); 
    InitializeProperty("SIZE",size);
    SetSize(size);
}


PSquare:: ~PSquare()
{
}






std::ostream & PSquare::SendToStream(std::ostream& out)
{
    out <<"PSquare";
    return out;
}

void PSquare::SetSize(pInt size)
{
    mDX = size;
    mDY = size;
    PEBLObjectBase::SetProperty("WIDTH",size);
    PEBLObjectBase::SetProperty("HEIGHT",size);
    PEBLObjectBase::SetProperty("SIZE",size);
    
}

bool PSquare::SetProperty(std::string name, Variant v)
{
    if(!PDrawObject::SetProperty(name,v))
        {
            
            if("WIDTH" == name || 
               "HEIGHT" == name || 
               "SIZE" == name)
                {
                    SetSize(v);
                }
            else return false;
        }
    return true;
}


Variant PSquare::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);

}


ObjectValidationError PSquare::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PSquare::ValidateProperty(std::string name)const
{

    if(OVE_VALID == PDrawObject::ValidateProperty(name))
       {
           return OVE_VALID;
       }
    else
        {
            if("WIDTH" == name 
               || "HEIGHT" == name 
               || "SIZE" == name)
                return OVE_VALID;
            else return OVE_INVALID_PROPERTY_NAME;
        }
    
}






PEllipse:: PEllipse(pInt x1, pInt y1, pInt rx, pInt ry, Variant col, bool filled)
{
    SetPosition(x1,y1);
    SetFilled(filled);
    InitializeProperty("NAME",Variant("<ELLIPSE>")); 
    InitializeProperty("RX",rx);
    InitializeProperty("RY",ry);
    SetSize(rx,ry);
    SetColor(col);
    SetOutlineColor(col);
    PDrawObject::SetAA(1);
}

PEllipse::~PEllipse()
{
}
void PEllipse::SetSize(pInt dx, pInt dy)
{
    mRX = dx;
    mRY = dy;
    PEBLObjectBase::SetProperty("RX",dx);
    PEBLObjectBase::SetProperty("RY",dy);
}

std::ostream & PEllipse::SendToStream(std::ostream& out)
{
  out << "PEllipse";
  return out;
}

bool PEllipse::SetProperty(std::string name, Variant v)
{
    
    if(!PDrawObject::SetProperty(name,v))
    {
        
        if("RX" == name)
        {SetSize(v,mRY);
        }
        else if ("RY" == name)
        {SetSize(mRX,v);
        }
        else
        {return false;
        }
    }
    return true;
}


Variant PEllipse::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);

}


ObjectValidationError PEllipse::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PEllipse::ValidateProperty(std::string name)const
{
    
    if(OVE_VALID == PDrawObject::ValidateProperty(name))
       {
           return OVE_VALID;
       }
    else
        {
            if("RX" == name 
               || "RY" == name )
                return OVE_VALID;
            else return OVE_INVALID_PROPERTY_NAME;
        }
}


PCircle::PCircle(pInt x1, pInt y1, pInt r, Variant fg, bool filled)
{
    
    SetPosition(x1,y1);
    SetFilled(filled);
    InitializeProperty("R",r);
    InitializeProperty("NAME",Variant("<CIRCLE>"));
    InitializeProperty("AA",Variant(1));

    SetSize(r);
    SetColor(fg);
    SetOutlineColor(fg);
}

PCircle::~PCircle()
{

}

void PCircle::SetSize(pInt r)
{
    mR = r;
    PEBLObjectBase::SetProperty("R",r);
}


std::ostream & PCircle::SendToStream(std::ostream& out)
{
  out << "PCircle";
  return out;
}

bool PCircle::SetProperty(std::string name, Variant v)
{

    if(!PDrawObject::SetProperty(name,v))
    {
        if("R" == name)
        {SetSize(v);
        }
        else
        {return false;
        }
    }
    return true;
}


Variant PCircle::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);

}


ObjectValidationError PCircle::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PCircle::ValidateProperty(std::string name)const
{
    
    if(OVE_VALID == PDrawObject::ValidateProperty(name))
       {
           return OVE_VALID;
       }
    else
        {
            if("R" == name)
                return OVE_VALID;
            else return OVE_INVALID_PROPERTY_NAME;
        }

}



PPolygon::PPolygon(pInt x, pInt y, Variant xpoints, Variant ypoints, Variant fg, bool filled)
{
    
    SetPosition(x,y);
    SetFilled(filled);
    InitializeProperty("XPOINTS",xpoints);
    InitializeProperty("YPOINTS",ypoints);
    InitializeProperty("NAME",Variant("<POLYGON>"));
    SetXPoints(xpoints);
    SetYPoints(ypoints);
    SetColor(fg);
    SetOutlineColor(fg);
    PDrawObject::SetAA(0);
}

PPolygon::~PPolygon()
{

}


std::ostream & PPolygon::SendToStream(std::ostream& out)
{
  out << "PPolygon";
  return out;
}

bool PPolygon::SetProperty(std::string name, Variant v)
{

    if(!PDrawObject::SetProperty(name,v))
    {
        if("XPOINTS" == name)
        {SetXPoints(v);
        }
        else if("YPOINTS" == name)
        {SetYPoints(v);
        }
        else{
            return false;
        }
    }
    return true;
}


bool PPolygon::SetXPoints(Variant v)
{

    mXPoints = v;
    return true;
}

bool PPolygon::SetYPoints(Variant v)
{


    mYPoints = v;

    return true;
}


Variant PPolygon::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);

}


ObjectValidationError PPolygon::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PPolygon::ValidateProperty(std::string name)const
{
    
    if(OVE_VALID == PDrawObject::ValidateProperty(name))
       {
           return OVE_VALID;
       }
    else
        {
            if("XPOINTS" == name
               || "YPOINTS" == name
               )
                return OVE_VALID;
            else return OVE_INVALID_PROPERTY_NAME;
        }

}



PBezier::PBezier(pInt x, pInt y, Variant xpoints, Variant ypoints, pInt steps, Variant fg)
{
    SetPosition(x,y);
    InitializeProperty("XPOINTS",xpoints);
    InitializeProperty("YPOINTS",ypoints);
    InitializeProperty("STEPS",steps);
    InitializeProperty("NAME",Variant("<BEZIER>"));
    SetXPoints(xpoints);
    SetYPoints(ypoints);
    SetSteps(steps);

    SetColor(fg);
    SetOutlineColor(fg);
}

PBezier::~PBezier()
{

}


std::ostream & PBezier::SendToStream(std::ostream& out)
{
  out << "PBezier";
  return out;
}

bool PBezier::SetProperty(std::string name, Variant v)
{

    if(!PDrawObject::SetProperty(name,v))
    {
        if("XPOINTS" == name)
        {
            
           SetXPoints(v);
        }
        else if("YPOINTS" == name)
        {SetYPoints(v);
        }
        else if ("STEPS" == name)
        {SetSteps(v);
        }
        else
        {
            return false;
        }
    }
    return true;
}


bool PBezier::SetXPoints(Variant v)
{

    mXPoints = v;
    return true;
}

bool PBezier::SetYPoints(Variant v)
{

    mYPoints = v;
    return true;
}

bool PBezier::SetSteps(Variant v)
{

    mSteps = (pInt)v;
    return true;

}


Variant PBezier::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);

}


ObjectValidationError PBezier::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}

ObjectValidationError PBezier::ValidateProperty(std::string name)const
{
    
    if(OVE_VALID == PDrawObject::ValidateProperty(name))
       {
           return OVE_VALID;
       }
    else
        {
            if("XPOINTS" == name
               || "YPOINTS" == name
               || "STEPS" == name
               )
                return OVE_VALID;
            else return OVE_INVALID_PROPERTY_NAME;
        }

}
