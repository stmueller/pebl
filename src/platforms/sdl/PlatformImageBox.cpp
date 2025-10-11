//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformImageBox.cpp
//    Purpose:    Contains SDL-specific interface for images
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003-2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//
//
//     This file is part of the PEBL project.
//
//    PEBL is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published
//    by the Free Software Foundation; either version 2 of the License,
//    or (at your option) any later version.
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
#include "PlatformImageBox.h"
#include "../../utility/PEBLPath.h"
#include "../../utility/PError.h"

#include "../../base/PEBLObject.h"

#ifdef PEBL_EMSCRIPTEN
#include "../../base/Evaluator-es.h"
#else
#include "../../base/Evaluator.h"
#endif

#include "SDL.h"
#include "SDL_image.h"

#include <string>
#include <stdio.h>

#include <assert.h>
//#include <emscripten.h>


using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::list;
using std::ostream;
using std::string;

#define SDL_IMAGE


///Standard Constructor
PlatformImageBox::PlatformImageBox():
    PImageBox()

{
    mCDT=CDT_IMAGEBOX;
    mTexture = NULL;
    mSurface = NULL;
    mRenderer = NULL;
    mTextureCreated = false;
    mNeedsTexture = true;
}



///Standard Destructor
PlatformImageBox::~PlatformImageBox()
{

}



// Inheritable function that is called by friend method << operator of PComplexData

ostream & PlatformImageBox::SendToStream(ostream& out) const
{

    out << "<SDL PlatformImageBox>" << flush;

    return out;
}





bool PlatformImageBox::LoadImage(const std::string &  imagefilename)
{


    //Check to see if we can find the image; if not, call everything off.
    string filename = Evaluator::gPath.FindFile(imagefilename);

    if(filename == "")
        PError::SignalFatalError(string("Unable to find image file [")  +
                                 imagefilename + string("]."));



    //This uses the SDL_image library to load a variety of
    //image types.

    //If the SDL_image library is being used, we can handle many different types
    //of images.  If it isn't, use SDL's built-in bmp loader.
#ifdef PEBL_EMSCRIPTENX
    
    mSurface = SDL_LoadBMP(filename.c_str());
#else
    
#ifdef SDL_IMAGE
    mSurface  = IMG_Load(filename.c_str());
#else
    mSurface = SDL_LoadBMP(filename.c_str());
#endif

#endif
    
    //Now, set the height and width to be the same as the
    //initial image.

    if(mSurface)
        {

            mWidth  = mSurface->w;
            mHeight = mSurface->h;
            mTextureWidth = mWidth;
            mTextureHeight = mHeight;

            //These need to be set at the PWidget level because
            //they are not mutable at the imagebox level.
            PImageBox::SetProperty("WIDTH", Variant(mWidth));
            PImageBox::SetProperty("HEIGHT", Variant(mHeight));
            return true;
        }
    else
        {

            PWidget::SetProperty("WIDTH", 0);
            PWidget::SetProperty("HEIGHT", 0);


            PError::SignalFatalError(string("Image not created.[")  +
                                     imagefilename + string("].") +
                                     string(IMG_GetError()));
            return false;
        }
}



bool PlatformImageBox::Draw()
{

    if(mRenderer)
        {

            if(!mTexture)
                {

                    mTexture  = SDL_CreateTextureFromSurface(mRenderer, mSurface);
                    mTextureCreated = true;

                }
            else
                {
                    //In this case, a texture already exists...

                }
        } else {

    }
    int out = PlatformWidget::Draw();
    return out==0;
}

//

void PlatformImageBox::SetZoomX(pDouble x)
{

    mZoomX = x;
    PEBLObjectBase::SetProperty("ZOOMX",mZoomX);
    SetProperty("X",mX); //reset X as it matters for images/labels
    SetProperty("WIDTH",(int)(mTextureWidth*mZoomX));
}

///This sets the widget's position on its parent widget.
void PlatformImageBox::SetZoomY(pDouble y)
{

    mZoomY = y;
    PEBLObjectBase::SetProperty("ZOOMY",mZoomY);
    SetProperty("Y",mY);//reset y as it matters for images/labels
    SetProperty("HEIGHT",(int)(mTextureHeight*mZoomY));
}



