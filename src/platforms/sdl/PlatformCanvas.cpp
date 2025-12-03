//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformTextBox.cpp
//    Purpose:    Contains SDL-specific interface for a draw canvas.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2010-2025 Shane T. Mueller <smueller@obereed.net>
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

#include "PlatformCanvas.h"
#include "../../objects/PCanvas.h"
#include "SDLUtility.h"

#include "../../base/PComplexData.h"


#include "../../utility/rc_ptrs.h"
#include "../../utility/PError.h"
#include "../../utility/PEBLUtility.h"

#include "SDL.h"
#include <stdio.h>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::list;
using std::ostream;
using std::string;


PlatformCanvas::PlatformCanvas(int width, int height, Variant bg):

    PCanvas(width, height,bg),
    mPixels(NULL),
    PlatformWidget()

{

    mTextureWidth = width;
    mTextureHeight = height;

    mCDT = CDT_CANVAS;
    InitializeProperty("BGCOLOR",bg);
    SetColor(bg);

    mSurface = NULL;
    mRenderer = NULL;
    mTexture = NULL;
    mNeedsTexture = true;
    Reset();
    Draw();

}



PlatformCanvas::PlatformCanvas(int width, int height):
    mCanvasWidth(0),
    mCanvasHeight(0),
    mPixels(NULL),
    PlatformWidget(),
    PCanvas(width, height)
{

    mTextureWidth = width;
    mTextureHeight = height;

    mWidth =width;
    mHeight = height;

    mCDT = CDT_CANVAS;

    mSurface = NULL;
    mTexture = NULL;
    mRenderer = NULL;
    mNeedsTexture = true;
    Reset();
    //Draw();

}




PlatformCanvas::PlatformCanvas(const PlatformCanvas & canvas):
    PEBLObjectBase(),
    PlatformWidget(),
    PCanvas()
{

    PCanvas::mDrawBackground = canvas.GetDrawBackground();
    mCDT = CDT_CANVAS;
    mWidth = canvas.GetWidth();
    mHeight = canvas.GetHeight();
    mTextureWidth = mWidth;
    mTextureHeight = mHeight;

    mTexture = NULL;
    mSurface = NULL;
    mNeedsTexture = true;
    Reset();   //reset initializes the canvas
    //Draw();

}


///Standard Destructor
PlatformCanvas::~PlatformCanvas()
{

    // PlatformWidget frees mSurface,
    // and should free children...

}

// Inheritable function that is called by friend method << operator of PComplexData
ostream & PlatformCanvas::SendToStream(ostream& out) const
{
    out << "<SDL PlatformCanvas>" <<flush;
    return out;
}




/// This method should be called when the canvas is initialized or something is changed.
/// It will make the mSurface pointer hold the appropriate image.
bool  PlatformCanvas::Reset()
{



    //create a new surface on which to render the text.

    if(mSurface)
        {
            delete mSurface;
            mSurface = NULL;
        }

    //Make a transparent surface of the prescribed size.

#if 0
    SDL_Surface * tmp = NULL;
    //    SDL_CreateRGBSurface(SDL_SWSURFACE,
    //                         mWidth, mHeight,
    //                         32, rmask, gmask, bmask, amask);
    if(!tmp)  PError::SignalFatalError("Surface not created in Canvas::Reset.");

#endif

#ifdef SD2_DELETE

    SDL_SetAlpha(mSurface,0,SDL_ALPHA_TRANSPARENT);
    //   SDL_SetAlpha( tmp, 0, SDL_ALPHA_OPAQUE );
#endif



    //Fill the box with the background color
    //    std::cout << mBackgroundColor << "|alpha:|" << mBackgroundColor.GetAlpha() << std::endl;



    //    if(mBackgroundColor.GetAlpha() <.000001)

    //        {
    //            std::cout << "Setting transparency\n";
    //SDL_SetAlpha(mSurface,0,0);

    //        }
    mReset = false;  //Reset the reset flag.


    //we can only render onto a texture if renderer exists.
    //i.e., the canvas needs to be on a window.

    if(mTexture)
        {

            SDL_DestroyTexture(mTexture);
            mTexture =NULL;
            mReset = true;
        }
    else
        {
            mReset = true;
        }

    if(mRenderer)
        {


            //we create texture from hidden surface to allow drawing on surface.
            //mTexture =  SDL_CreateTextureFromSurface(mRenderer,tmp);

            mTexture =   SDL_CreateTexture(mRenderer,
                                           SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_TARGET,
                                           (int)mTextureWidth,(int)mTextureHeight);

            SDL_Rect  screensize = {0,0,(int)(mTextureWidth),
                (int)mTextureHeight};
            SDL_SetRenderTarget(mRenderer,mTexture);
            SDL_RenderClear( mRenderer );

            // Get background color from property system (in case it was modified via nested properties)
            Variant backgroundColor = PEBLObjectBase::GetProperty("BGCOLOR");
            PColor* bgColor = nullptr;
            if(backgroundColor.GetComplexData())
            {
                bgColor = dynamic_cast<PColor*>(backgroundColor.GetComplexData()->GetObject().get());
            }

            // Use property color if available, otherwise fall back to mBackgroundColor
            if(!bgColor)
            {
                bgColor = &mBackgroundColor;
            }

            SDL_SetRenderDrawColor(mRenderer, bgColor->GetRed(),
                                   bgColor->GetGreen(),
                                   bgColor->GetBlue(),
                                   bgColor->GetAlpha());

            SDL_RenderFillRect(mRenderer,&screensize);


            SDL_SetRenderTarget(mRenderer,NULL);

            //SDL_FreeSurface(tmp);

        }else{
        mReset = true;
    }


    if(mTexture)
        return true;
    else
        {
            mReset = true;
            return false;
        }
}


bool PlatformCanvas::SetProperty(std::string name, Variant v)
{

    //we need to handle zooming separately
    if(name == "ZOOMX")    SetZoomX((pDouble)v);
    else if(name=="ZOOMY") SetZoomY((pDouble)v);
    else     return PCanvas::SetProperty(name,v);

    return true;
}


bool PlatformCanvas::Draw()
{

    //Can we only reset if something has changed on a child?
    mReset = true;
    if(mReset)
        {
        }
            Reset();


    bool ret =     PlatformWidget::Draw();
    mReset = false;


    return  ret;
}

void PlatformCanvas::SetHeight(pInt h)
{
    mHeight = h;
    mTextureHeight = h;
    PEBLObjectBase::SetProperty("HEIGHT",mHeight);

}

void PlatformCanvas::SetWidth(pInt w)
{

    mWidth = w;
    mTextureWidth = w;
    PEBLObjectBase::SetProperty("WIDTH",mWidth);

}

void PlatformCanvas::SetZoomX(double x)
{

    mZoomX = x;
    PEBLObjectBase::SetProperty("ZOOMX",mZoomX);
    SetProperty("X",mX); //reset X as it matters for images/labels
    mWidth = (int)(mTextureWidth*mZoomX);
    PEBLObjectBase::SetProperty("WIDTH",mWidth);


}

void PlatformCanvas::SetZoomY(double y)
{

    mZoomY = y;
    PEBLObjectBase::SetProperty("ZOOMY",mZoomY);
    SetProperty("Y",mY); //reset X as it matters for images/labels
    mHeight =(int)(mTextureHeight*mZoomY);
    PEBLObjectBase::SetProperty("HEIGHT",mHeight);


}


#if 0

bool PlatformCanvas::GreyscaleFromMatrix()
{


    //Check to see if we can find the image; if not, call everything off.
    string filename = Evaluator::gPath.FindFile(imagefilename);

    if(filename == "")
        PError::SignalFatalError(string("Unable to find image file [")  + imagefilename + string("]."));


    //This uses the SDL_image library to load a variety of
    //image types.
#ifdef SDL2_DELETE
    if(mSurface)
        {
            SDL_FreeSurface(mSurface);
        }
#endif



    //If the SDL_image library is being used, we can handle many different types
    //of images.  If it isn't, use SDL's built-in bmp loader.

    //uSE THE FOLLOWING:

    //SDL_Surface *SDL_CreateRGBSurfaceFrom(void *pixels,
    //                        int width, int height, int depth, int pitch,
    //                        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);

    //Checking should be done here to insure the proper color depth,
    //bpp, format, etc.



    //Now, set the height and width to be the same as the
    //initial image.
    if( mSurface)
        {

            mWidth  = mSurface->w;
            mHeight = mSurface->h;
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

            return false;
        }
}



#endif
