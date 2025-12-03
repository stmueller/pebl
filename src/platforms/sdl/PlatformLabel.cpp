//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformLabel.cpp
//    Purpose:    Contains SDL-specific visual representation of a word
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2004-2025 Shane T. Mueller <smueller@obereed.net>
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
#include "PlatformLabel.h"
#include "../../objects/PLabel.h"
#include "PlatformFont.h"
#include "../../utility/rc_ptrs.h"
#include "../../utility/PError.h"
#include "../../base/PComplexData.h"
#include "../../utility/PEBLUtility.h"
#ifdef PEBL_OSX
#include "SDL.h"
#include "SDL_ttf.h"
#else
#include "SDL.h"
#include "SDL_ttf.h"
#endif

#include <stdio.h>
#include <algorithm>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::list;
using std::ostream;



PlatformLabel::PlatformLabel(const std::string & text, counted_ptr<PEBLObjectBase> font):
    PlatformWidget(),
    PLabel(text),
    mFontObject(font)
{

    mSurface = NULL;
    mTexture = NULL;
    mRenderer = NULL;

    mCDT=CDT_LABEL;

    SetFont(font);

    SetText(text);

    //Make the font property accessible
    counted_ptr<PEBLObjectBase> myFont = counted_ptr<PEBLObjectBase>(mFontObject);

    PComplexData *  pcd = new PComplexData(myFont);

    InitializeProperty("FONT",Variant(pcd));

    delete pcd;
    pcd=NULL;

    InitializeProperty("WIDTH",Variant(0));
    InitializeProperty("HEIGHT",Variant(0));

    //issue draw() command to be sure the xy center is set right.
    Draw();

}


PlatformLabel::PlatformLabel(PlatformLabel & label):
    PlatformWidget(),
    PLabel(label.GetText())

{
    mCDT=CDT_LABEL;
    mSurface = NULL;
    mRenderer = NULL;
    mTexture = NULL;

    SetFont(label.GetFont());


    counted_ptr<PEBLObjectBase> myFont = counted_ptr<PEBLObjectBase>(mFontObject);
    PComplexData *  pcd = new PComplexData(myFont);
    delete pcd;
    pcd=NULL;

    InitializeProperty("FONT",Variant(pcd));
    InitializeProperty("WIDTH",Variant(label.GetHeight()));
    InitializeProperty("HEIGHT",Variant(label.GetWidth()));
    cout << "Drawing new label in platformlabel\n";
    Draw();
}


///Standard Destructor
PlatformLabel::~PlatformLabel()
{
    //cout << "Label destructor\n";
 
}


// Inheritable function that is called by friend method << operator of PComplexData
ostream & PlatformLabel::SendToStream(ostream& out) const
{
    out << "<SDL PlatformLabel: " << mText << " in " << *mFont << ">" <<flush;
    return out;
}

 

/// RenderText is only called once the label is already added to
/// a window/widget, so that it already has a renderer and a texture.

bool  PlatformLabel::RenderText()
{

    //free the memory if it is currently pointing at something.

    SDL_Surface * tmpSurface = NULL;
    if(mDirection == 1)
        {
            //Re-render the text using the associated font.
            tmpSurface = mFont->RenderText(mText.c_str());
        }
    else
        {

            std::string rtext = PEBLUtility::strrev_utf8(mText);
            //Re-render the text using the associated font.
            tmpSurface  = mFont->RenderText(rtext.c_str());
        }
    

    if( tmpSurface)
        {

            //int w, h;
            //SDL_QueryTexture(mTexture, NULL, NULL, &w, &h);
            //            mWidth  = w;
            //            mHeight = h;


            mWidth  = tmpSurface->w; 
            mHeight = tmpSurface->h;
            mTextureWidth = mWidth;
            mTextureHeight = mHeight;
            InitializeProperty("HEIGHT",mHeight);
            InitializeProperty("WIDTH",mWidth);


            //textures get created 
            if(mTexture)
                {

                    SDL_DestroyTexture(mTexture);
                    mTexture = NULL;
                } 

            if(mRenderer)
                {

                    mTexture  = SDL_CreateTextureFromSurface(mRenderer, tmpSurface);
                    // Enable alpha blending for transparency support
                    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);

                    SDL_FreeSurface(tmpSurface);
                    tmpSurface = NULL;
                }

            return true;
        }
    else
        return false;
}


bool PlatformLabel::SetProperty(std::string name, Variant v)
{

    if(name == "TEXT")
        {

            SetText(v);
        }
    else if(PLabel::SetProperty(name,v))
    {
        // If we set it at higher level, don't worry.
    }
    else if (name == "FONT")
        {
            SetFont(v.GetComplexData()->GetObject());
            
        }
    else return false;
    
    return true;
}

void PlatformLabel::SetFont(counted_ptr<PEBLObjectBase> font)
{

    mFontObject = font;
    mFont = dynamic_cast<PlatformFont*>(mFontObject.get());

    // Update the FONT property so nested access works correctly
    PComplexData * pcd = new PComplexData(mFontObject);
    PEBLObjectBase::SetProperty("FONT", Variant(pcd));
    delete pcd;

    mChanged =true;
    Draw();

    //if(!RenderText()) cerr << "Unable to render text.\n";

}


void PlatformLabel::SetText(const std::string & text)
{
    //Chain up to parent method.
    PTextObject::SetText(text);
    mChanged =true;
    Draw();

    //Re-render the text onto mSurface
    //if(!RenderText()) cerr << "Unable to render text.\n";

}


bool PlatformLabel::Draw()
{

    // Check if label text changed OR if font properties changed
    if(mChanged || mFont->HasChanged())
        {

            RenderText();
            //Reposition.  This just recalculates so things are centered
            //correctly; labels are positioned based on their center.

            SetPosition(mX, mY);

            InitializeProperty("HEIGHT",mHeight);
            InitializeProperty("WIDTH",mWidth);
            mFont->ClearChanged();  // Clear font changed flag after re-rendering

            // Only clear mChanged if texture was successfully created
            // If mTexture is NULL, keep mChanged=true to retry when renderer is available
            if(mTexture)
            {
                mChanged = false;
            }
        }
    return  PlatformWidget::Draw();

}
