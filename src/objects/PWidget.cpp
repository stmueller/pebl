//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PWidget.cpp
//    Purpose:    Contains methods for primary GUI element
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003-2025 Shane T. Mueller <smueller@obereed.net>
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
////////////////////////////////////////////////////////////////////

#include "PWidget.h"
#include "../base/PComplexData.h"
#include "../utility/PError.h"
#include <list>
#include <iostream>

using std::cout;
using std::endl;

PWidget::PWidget():
    mX(0),
    mY(0),
    mDrawX(0),
    mDrawY(0),
    mWidth(0),
    mHeight(0),
    mZoomX(1),
    mZoomY(1),
    mRotation(0),
    mBackgroundColor(PColor(0,0,0,0)),
    mIsVisible(true),
    mParent(NULL)
{
    //    InitializeProperty("NAME",Variant(""));
    InitializeProperty("X",Variant(mX));
    InitializeProperty("Y",Variant(mY));
    InitializeProperty("WIDTH",Variant(mWidth));
    InitializeProperty("HEIGHT",Variant(mHeight));
    InitializeProperty("VISIBLE",Variant(mIsVisible));
    InitializeProperty("ZOOMX",Variant(mZoomX));
    InitializeProperty("ZOOMY",Variant(mZoomY));
    InitializeProperty("ROTATION",Variant(mRotation));
    InitializeProperty("PARENT",Variant(0));

}

PWidget::PWidget(pInt x, pInt y, pInt width, pInt height, bool visible):
    mX(x),
    mY(y),
    mDrawX(x),
    mDrawY(y),
    mWidth(width),
    mHeight(height),
    mZoomX(1.0),
    mZoomY(1.0),
    mRotation(0),
    mBackgroundColor(PColor(0,0,0,0)),
    mIsVisible(visible),
    mParent(NULL)
{
    //    InitializeProperty("NAME",Variant(""));
    InitializeProperty("X",Variant(mX));
    InitializeProperty("Y",Variant(mY));
    InitializeProperty("WIDTH",Variant(mWidth));
    InitializeProperty("HEIGHT",Variant(mHeight));
    InitializeProperty("VISIBLE",Variant(mIsVisible));
    InitializeProperty("ZOOMX",Variant(mZoomX));
    InitializeProperty("ZOOMY",Variant(mZoomY));
    InitializeProperty("ROTATION",Variant(mRotation));
    InitializeProperty("PARENT",Variant(0));

}

PWidget::PWidget(const PWidget & pw):
                 PWidget(pw.GetX(),
                         pw.GetY(),
                         pw.GetWidth(),
                         pw.GetHeight(),
                         pw.IsVisible())
{
    //    InitializeProperty("NAME",Variant(""));
    InitializeProperty("X",Variant(mX));
    InitializeProperty("Y",Variant(mY));
    InitializeProperty("WIDTH",Variant(mWidth));
    InitializeProperty("HEIGHT",Variant(mHeight));
    InitializeProperty("VISIBLE",Variant(mIsVisible));
    InitializeProperty("ZOOMX",Variant(mZoomX));
    InitializeProperty("ZOOMY",Variant(mZoomY));
    InitializeProperty("ROTATION",Variant(mRotation));
    InitializeProperty("PARENT",Variant(0));

}

PWidget::~PWidget()
{



}

bool PWidget::RotoZoom(pDouble angle, pDouble zoomx, pDouble zoomy, pInt smooth)
{

    return false;
}


bool PWidget::SetPoint(pInt x, pInt y, PColor col)
{
    //This should go straight to PlatforwWidget::SetPoint

    return false;
}


PColor PWidget::GetPixel(pInt x, pInt y)
{
    //This should go straight to PlatforwWidget::GetPixel

    return false;
}



bool PWidget::SetProperty(std::string name, Variant v)
{
    ObjectValidationError err = ValidateProperty(name,v);
    if(err == OVE_SUCCESS)
        {
            if (name == "X") SetPosition(v,mY);
            else if (name == "Y") SetPosition(mX,v);
            else if (name == "ZOOMX") SetZoomX(( pDouble)v);
            else if (name == "ZOOMY") SetZoomY(( pDouble)v);
            else if (name == "ROTATION") SetRotation(( pDouble)v);
            else if (name == "WIDTH") SetWidth((pInt)v);
            else if (name == "HEIGHT") SetHeight((pInt)v);
            else if (name == "BGCOLOR")
                {
                    PColor bgcolor = *((PColor*)(v.GetComplexData()->GetObject().get()));
                    SetBackgroundColor(bgcolor);
                }
            else if (name == "VISIBLE")
                {
                    if(v.GetInteger())
                        Show();
                    else
                        Hide();
                }else if (name=="PARENT")
                {

                    PEBLObjectBase::SetProperty("PARENT",v);
                }
            else return false;
        }
    else
        {
            PError::SignalFatalError("Failing to set invalid property: ["+ name +"]" );
            return false;
        }
    return false;
}


Variant  PWidget::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);
}



 ObjectValidationError PWidget::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}


 ObjectValidationError PWidget::ValidateProperty(std::string name)const
{


    //This only returns valid for properties that are valid to widgets
    //in general--subclasses need to check on their own.
    if(name == "X" ||
       name == "Y" ||
       name == "VISIBLE" ||
       name == "WIDTH" ||
       name == "HEIGHT" ||
       name == "ZOOMX" ||
       name == "ZOOMY" ||
       name == "BGCOLOR" ||
       name == "ROTATION" ||
       name == "NAME"||
       name=="PARENT"
       )
        return OVE_VALID;
    else
        return OVE_INVALID_PROPERTY_NAME;
}


///This sets the widget's position on its parent widget.

void PWidget::SetPosition(pInt x, pInt y)
{


    mX = x;
    mY = y;
    mDrawX = x;
    mDrawY = y;

    PEBLObjectBase::SetProperty("X",mX);
    PEBLObjectBase::SetProperty("Y",mY);
}



///This sets the widget's position on its parent widget.
void PWidget::SetZoomX(pDouble x)
{

    mZoomX = x;
    PEBLObjectBase::SetProperty("ZOOMX",mZoomX);
    SetProperty("X",mX); //reset X as it matters for images/labels
    //SetProperty("WIDTH",(int)(mWidth*mZoomX));
}


///This sets the widget's position on its parent widget.
void PWidget::SetZoomY(pDouble y)
{

    mZoomY = y;
    PEBLObjectBase::SetProperty("ZOOMY",mZoomY);
    SetProperty("Y",mY);//reset y as it matters for images/labels
    //SetProperty("HEIGHT",(int)(mHeight*mZoomY));
}



void PWidget::SetHeight(pInt h)
{
    mHeight = h;
    PEBLObjectBase::SetProperty("HEIGHT",mHeight);

}

void PWidget::SetWidth(pInt w)
{
    mWidth = w;
    PEBLObjectBase::SetProperty("WIDTH",mWidth);

}

///This sets the widget's position on its parent widget.
void PWidget::SetRotation(pDouble x)
{
    mRotation = x;
    PEBLObjectBase::SetProperty("ROTATION",x);
}

//Inherited from PComplexObject->PEBLObjectBase
std::ostream & PWidget::SendToStream(std::ostream& out) const
{
    out << "<Unknown PWidget>" << std::flush;
    return out;
}


void PWidget::SetBackgroundColor(const PColor & color)
{

    PColor * pc = new PColor(color);
    counted_ptr<PEBLObjectBase> pob = counted_ptr<PEBLObjectBase>(pc);
    PComplexData * pcd = new PComplexData(pob);
    Variant col = Variant(pcd);


    PEBLObjectBase::SetProperty("BGCOLOR",col);

    PColor pc2 = PColor(color.GetRed(),color.GetGreen(),color.GetBlue(),color.GetAlpha());
    //PColor(col);

    mBackgroundColor.SetRed(color.GetRed());
    mBackgroundColor.SetGreen(color.GetGreen());
    mBackgroundColor.SetBlue(color.GetBlue());
    mBackgroundColor.SetAlpha(color.GetAlpha());


    //    InitializeProperty("BGCOLOR", col);

}



//  This will set the parent of *this to widget
//
void PWidget::SetParent(PWidget * widget)
{

    PEBLObjectBase::SetProperty("PARENT",widget);
    mParent = widget;
}


bool PWidget::IsParent(PWidget * testparent)
{

    return testparent == GetParent();
}


bool PWidget::AddSubWidget(PWidget * widget)
{


    //remove the subwidget from wherever it was:
    PWidget* p = widget->GetParent();
    if(p)
        {

            p->RemoveSubWidget(widget);
        }

     widget->SetParent(NULL);



    //now, add it back on:
    mSubWidgets.push_front(widget);
    widget->SetParent(this);

    return true;
}

bool PWidget::RemoveSubWidgets()

{

    if(mSubWidgets.size()>0)
        {

            std::list<PWidget*>::iterator i = mSubWidgets.begin();
            while(i != mSubWidgets.end())
                {

                    (*i)->SetParent(NULL);
                    mSubWidgets.pop_front();
                    i=mSubWidgets.begin();  //readjust so it points to the start of the list.
                }

            mSubWidgets.clear();
        }
    return true;
}


bool PWidget::RemoveSubWidget(PWidget * widget)
{




    mSubWidgets.remove(widget);
    mSubWidgets.remove(dynamic_cast<PlatformWidget*>(widget));
    widget->SetParent(NULL);

    return true;
}

bool PWidget::RemoveLastSubWidget()
{

    mSubWidgets.pop_front();
    return true;
}


void PWidget::Show()
{
    mIsVisible = true;
    PEBLObjectBase::SetProperty("VISIBLE",mIsVisible);

}
void PWidget::Hide()
{
    mIsVisible = false;
    PEBLObjectBase::SetProperty("VISIBLE",mIsVisible);
}


