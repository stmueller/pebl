//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PFont.cpp
//    Purpose:    Contains generic specification for a font
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
////////////////////////////////////////////////////////////////////////////////
#include "PFont.h"
#include "PColor.h"
#include "../base/Variant.h"
#include "../base/PComplexData.h"
#include "../utility/rc_ptrs.h"


#include <iostream>
using std::cout;
using std::endl;
///Standard constructor of PFont

PFont::PFont():
    mFontFileName("Vera.ttf"),
    mFontStyle(PFS_Normal),
    mFontSize(16),
    mAntiAliased(false)

{
    mCDT = CDT_FONT;

    InitializeProperty("FILENAME", mFontFileName);
    InitializeProperty("BOLD", Variant(IsBoldFont()));
    InitializeProperty("UNDERLINE",Variant(IsUnderlineFont()));
    InitializeProperty("ITALIC",  Variant(IsItalicFont()));
    InitializeProperty("SIZE", mFontSize);
    InitializeProperty("NAME", Variant("<FONT>"));

    // Store colors ONLY in property map (like PDrawObject, PCanvas, PWindow)
    // Create new color objects for property storage
    counted_ptr<PEBLObjectBase> fgColor(new PColor(0,0,0,255));
    PComplexData * pcd = new PComplexData(fgColor);
    Variant col = Variant(pcd);
    delete pcd;
    pcd=NULL;
    InitializeProperty("FGCOLOR", col);

    counted_ptr<PEBLObjectBase> bgColor(new PColor(0,0,0,255));
    pcd = new PComplexData(bgColor);
    col = Variant(pcd);
    delete pcd;
    pcd=NULL;
    InitializeProperty("BGCOLOR", col);

    InitializeProperty("ANTIALIASED", Variant(mAntiAliased));
}


///Convenience constructor of PFont:
PFont::PFont(const std::string & filename, int style, int size, PColor fgcolor, PColor bgcolor, bool aa):
    mFontFileName(filename),
    mFontStyle(style),
    mFontSize(size),
    mAntiAliased(aa)
{
    mCDT = CDT_FONT;

    InitializeProperty("NAME", Variant("<FONT>"));
    InitializeProperty("FILENAME", mFontFileName);
    InitializeProperty("BOLD", Variant(IsBoldFont()));
    InitializeProperty("UNDERLINE",Variant(IsUnderlineFont()));
    InitializeProperty("ITALIC",  Variant(IsItalicFont()));
    InitializeProperty("SIZE", mFontSize);

    // Store colors ONLY in property map
    counted_ptr<PEBLObjectBase> fgColorPtr(new PColor(fgcolor));
    PComplexData * pcd = new PComplexData(fgColorPtr);
    Variant col = Variant(pcd);
    delete pcd;
    pcd=NULL;
    InitializeProperty("FGCOLOR", col);

    counted_ptr<PEBLObjectBase> bgColorPtr(new PColor(bgcolor));
    pcd = new PComplexData(bgColorPtr);
    col = Variant(pcd);
    delete pcd;
    pcd=NULL;
    InitializeProperty("BGCOLOR", col);

    InitializeProperty("ANTIALIASED", Variant(mAntiAliased));
}


///Copy constructor of PFont:
PFont::PFont(const PFont & font)

{
    mCDT = CDT_FONT;

    mFontFileName    = font.GetFontFileName();
    mFontStyle       = font.GetFontStyle();
    mFontSize        = font.GetFontSize();
    mAntiAliased     = font.GetAntiAliased();

    InitializeProperty("FILENAME", mFontFileName);
    InitializeProperty("BOLD", Variant(IsBoldFont()));
    InitializeProperty("UNDERLINE",Variant(IsUnderlineFont()));
    InitializeProperty("ITALIC",  Variant(IsItalicFont()));
    InitializeProperty("SIZE", mFontSize);
    InitializeProperty("NAME", Variant("<FONT>"));

    // Store colors ONLY in property map - copy from source font
    PColor fgcolor = font.GetFontColor();
    PColor bgcolor = font.GetBackgroundColor();

    counted_ptr<PEBLObjectBase> fgColorPtr(new PColor(fgcolor));
    PComplexData * pcd = new PComplexData(fgColorPtr);
    Variant col = Variant(pcd);
    delete pcd;
    pcd=NULL;
    InitializeProperty("FGCOLOR", col);

    counted_ptr<PEBLObjectBase> bgColorPtr(new PColor(bgcolor));
    pcd = new PComplexData(bgColorPtr);
    col = Variant(pcd);
    delete pcd;
    pcd=NULL;
    InitializeProperty("BGCOLOR", col);

    InitializeProperty("ANTIALIASED", Variant(mAntiAliased));
}


///Standard destructor of PFont
PFont::~PFont()
{

}


//overloaded generic PEBLObjectBase methods
bool PFont::SetProperty(std::string name, Variant v)
{
    if(name == "FILENAME") SetFontFileName(v);
    else if (name == "BOLD" )
        {
            int b,u,i;
            if(v.GetInteger()) b = PFS_Bold;
            else b = PFS_Normal;

            if(IsItalicFont()) i = PFS_Italic;
            else i = PFS_Normal;
            
            if(IsUnderlineFont()) u = PFS_Underline;
            else u = PFS_Normal;
            
            SetFontStyle( b | i | u);

        }
    else if (name == "ITALIC" )
        {
            int b,u,i;
            if(IsBoldFont()) b = PFS_Bold;
            else b = PFS_Normal;

            if(v.GetInteger()) i = PFS_Italic;
            else i = PFS_Normal;
            
            if(IsUnderlineFont()) u = PFS_Underline;
            else u = PFS_Normal;
            
            SetFontStyle( b | i | u);

        }
    else if (name == "UNDERLINE" )
        {
            int b,u,i;
            if(IsBoldFont()) b = PFS_Bold;
            else b = PFS_Normal;

            if(IsItalicFont()) i = PFS_Italic;
            else i = PFS_Normal;
            
            if(v.GetInteger()) u = PFS_Underline;
            else u = PFS_Normal;
            
            SetFontStyle( b | i | u);
        }
    else if (name == "SIZE") SetFontSize(v) ;

    
    else if (name == "FGCOLOR")
        {
            // Extract the new color and update our internal color object
            // Don't call PEBLObjectBase::SetProperty - the property map already points to mFontColor
            PColor newColor = *(dynamic_cast<PColor*>(v.GetComplexData()->GetObject().get()));
            SetFontColor(newColor);
        }
    else if (name == "BGCOLOR")
        {
            // Extract the new color and update our internal color object
            // Don't call PEBLObjectBase::SetProperty - the property map already points to mBackgroundColor
            PColor newColor = *(dynamic_cast<PColor*>(v.GetComplexData()->GetObject().get()));
            SetBackgroundColor(newColor);
        }
    else if (name == "ANTIALIASED")SetAntiAliased(v.GetInteger());
    else return false;
    
    return true;
}

Variant PFont::GetProperty(std::string name)const
{
    return PEBLObjectBase::GetProperty(name);
}


ObjectValidationError PFont::ValidateProperty(std::string name, Variant v)const
{
    return ValidateProperty(name);
}
ObjectValidationError PFont::ValidateProperty(std::string name)const
{
    return OVE_SUCCESS;
}

std::string PFont::ObjectName()const
{
    return "PFont";
}



///Set methods for all of the data in font
void PFont::SetFontFileName(const std::string & name)  
{
    mFontFileName = name;
    PEBLObjectBase::SetProperty("FILENAME", mFontFileName);
}

void PFont::SetFontStyle(const int style)         
{
    mFontStyle=style;
    PEBLObjectBase::SetProperty("BOLD", Variant(IsBoldFont()));
    PEBLObjectBase::SetProperty("UNDERLINE",Variant(IsUnderlineFont()));
    PEBLObjectBase::SetProperty("ITALIC",  Variant(IsItalicFont()));
}

void PFont::SetFontSize(const int size)
{
    mFontSize = size;
    PEBLObjectBase::SetProperty("SIZE", mFontSize);
}

// Helper methods to get color pointers from property system (like PDrawObject)
PColor* PFont::GetFontColorPtr() const
{
    Variant v = PEBLObjectBase::GetProperty("FGCOLOR");
    if(v.GetComplexData())
        return dynamic_cast<PColor*>(v.GetComplexData()->GetObject().get());
    return nullptr;
}

PColor* PFont::GetBackgroundColorPtr() const
{
    Variant v = PEBLObjectBase::GetProperty("BGCOLOR");
    if(v.GetComplexData())
        return dynamic_cast<PColor*>(v.GetComplexData()->GetObject().get());
    return nullptr;
}

// Get methods that return by value
PColor PFont::GetFontColor() const
{
    PColor* ptr = GetFontColorPtr();
    if(ptr) return *ptr;
    return PColor(0,0,0,255); // default black
}

PColor PFont::GetBackgroundColor() const
{
    PColor* ptr = GetBackgroundColorPtr();
    if(ptr) return *ptr;
    return PColor(0,0,0,255); // default black
}

void PFont::SetFontColor(const PColor color)
{
    // Update individual color components to avoid problematic copy assignment
    PColor* ptr = GetFontColorPtr();
    if(ptr) {
        ptr->SetRed(color.GetRed());
        ptr->SetGreen(color.GetGreen());
        ptr->SetBlue(color.GetBlue());
        ptr->SetAlpha(color.GetAlpha());
    }
}

void PFont::SetBackgroundColor(const PColor color)
{
    // Update individual color components to avoid problematic copy assignment
    PColor* ptr = GetBackgroundColorPtr();
    if(ptr) {
        ptr->SetRed(color.GetRed());
        ptr->SetGreen(color.GetGreen());
        ptr->SetBlue(color.GetBlue());
        ptr->SetAlpha(color.GetAlpha());
    }
}

void PFont::SetAntiAliased(const bool aa)  
{
    mAntiAliased = aa;
    PEBLObjectBase::SetProperty("ANTIALIASED", Variant(mAntiAliased));
}



/// This sends the font descriptions to the specified stream.
std::ostream & PFont::SendToStream(std::ostream& out) const
{
    out << "<PFont: Name:        [" << mFontFileName << "]\n";
    out << "        Style:       ["<< mFontStyle<< "]\n";
    out << "        Size:        ["<< mFontSize << "]\n";

    PColor* fgColor = GetFontColorPtr();
    PColor* bgColor = GetBackgroundColorPtr();

    if(fgColor)
        out << "        Color:       ["<< *fgColor << "]\n";
    else
        out << "        Color:       [NULL]\n";

    if(bgColor)
        out << "        BGColor:     ["<< *bgColor<< "]\n";
    else
        out << "        BGColor:     [NULL]\n";

    out << "        Antialiased: ["<< mAntiAliased << "]>\n";

    return out;
}

 
//These get different styles
bool PFont::IsNormalFont() const
{
    return mFontStyle == 0;
}

bool PFont::IsBoldFont() const
{
    //The PFS_Bold acts as a bit filter, if it isn't bold the & should be 0.
    return (mFontStyle & PFS_Bold);
}

bool PFont::IsItalicFont() const
{
    //The PFS_Italic acts as a bit filter, if it isn't bold the & should be 0.
    return (mFontStyle & PFS_Italic);
}

bool PFont::IsUnderlineFont() const
{
    //The PFS_Underline acts as a bit filter, if it isn't bold the & should be 0.
    return (mFontStyle & PFS_Underline);
}

