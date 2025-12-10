//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformWidget.cpp
//    Purpose:    Contains SDL-specific interface GUI objects
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
#include "PlatformWidget.h"

#ifdef PEBL_OSX
#include "SDL.h"
#else
#include "SDL.h"
#endif

#include "SDLUtility.h"
#include "../../utility/PError.h"
#include "../../base/PComplexData.h"
#include <iostream>
#include <cstring>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;

//Standard constructor
PlatformWidget::PlatformWidget():
    mNeedsTexture(true)
{
    mRenderer= NULL;
    mSurface = NULL;
    mTexture = NULL;

    //cout << "Creating null platformwidget\n";
}

PlatformWidget::~PlatformWidget()
{


    //Remove this from its parent (if one exists)
    if(mParent)
        {
            mParent->RemoveSubWidget(this);
        }

    RemoveSubWidgets();


    if(mSurface)
        {

            SDL_FreeSurface(mSurface);
            mSurface = NULL;
        }

    //Don't destroy the 'borrowed' texture of drawobjects.
    if(mNeedsTexture)
        {

            if(mTexture)
                {

                    SDL_DestroyTexture(mTexture);
                }
        }

}



std::ostream & PlatformWidget::SendToStream(std::ostream& out) const
{
    out << "<SDL PlatformWidget> " << std::flush;
    return out;
}




//
//
//
bool PlatformWidget::Draw()
{

    if(mRenderer)
        {
            long unsigned int newWidth =  mWidth;
            long unsigned int newHeight = mHeight;

            if(IsVisible())
                {
                    //To draw a widget, draw each of the window's subwidgets
                    //(using PlatformWidget::Draw(SDLSurface)
                    //This should be done backwards, so that the last item added
                    //(which is on the front) will be the last item drawn.
                    
                    
                    std::list<PWidget *>::iterator p = mSubWidgets.end();
                    while(p != mSubWidgets.begin())
                        {
                            
                            //decrement iterator--moving backward so we draw things in
                            //reverse order.
                            p--;
                            //Draw the subwidget
                            if((*p)->IsVisible())
                                {
                                    
                                    (*p)->Draw();
                                    
                                }
                        }
                    
                    //Once the widget sub-items are drawn,
                    //draw the widget to its parent.
                    
                    if (mParent)
                        {
                            long unsigned int w = mWidth;
                            long unsigned int h = mHeight;
                            
                            if(mNeedsTexture)
                                {
                                    //SDL_QueryTexture(mTexture,NULL,NULL,&w,&h);
                                    w = mTextureWidth;
                                    h = mTextureHeight;
                                }
                            
                            SDL_Rect fromRect = {0,0,(int)w,(int)h};
                            
                            //Check to see if any roto-zooming is needed
                            if(std::abs(mZoomX-1.0)>.001 || std::abs(mZoomY-1.0)>.001 )
                                {
                                    
                                    //We need to reset the sizes.
                                    //Calculate the new  size.
                                    
                                    newWidth =  w * mZoomX;
                                    newHeight = h * mZoomY;
                                    
                                    PWidget::SetProperty("WIDTH",newWidth);
                                    PWidget::SetProperty("HEIGHT",newHeight);
                                    //zoomx and zoomy should take care of setting widths correctly.
                                    
                                    
                                    
                                    //these call PWidget::SetZoom
                                    SetProperty("ZOOMX",mZoomX);
                                    SetProperty("ZOOMY",mZoomY);
                                    
                                    SetPosition(mX,mY); //Reset position so
                                    //objects get centered properly
                                    
                                }
                            else
                                {
                                    
                                    PWidget::SetProperty("WIDTH", mWidth);
                                    PWidget::SetProperty("HEIGHT", mHeight);
                                    PWidget::SetProperty("ZOOMX",1);
                                    PWidget::SetProperty("ZOOMY",1);
                                }
                            

                            
                            //The to rect depends on the type of object...
                            //textboxes don't change their to x value;
                            //all others will adjust to self-center.
                            
                            SDL_Rect  toRect   = {static_cast<int>(mDrawX),
                                                  static_cast<int>(mDrawY),
                                                  (int)newWidth,
                                                  (int)  newHeight};


                            //This renders from the texture onto the base window.
                            //we sort of want to render to parent instead.
                            int result;
                            SDL_Texture * parenttexture =  dynamic_cast<PlatformWidget*>(mParent)->GetSDL_Texture();
                            
                            if(parenttexture)
                                {
                                    result = SDL_SetRenderTarget(mRenderer,parenttexture);
                                    
                                    if(result<0)
                                        {
                                            cerr << " SetRenderTarget failed with error code: " << result << endl;
                                            
                                            cerr << "SDL message: " <<  SDL_GetError() << endl;
                                            cerr << *this << endl;
                                            cerr << "Parent:" << parenttexture << endl;
                                            
                                        }
                                    
                                    result = -1;
                                    int count = 0;
                                    while(result<0 & count++<10)
                                        {
                                            
                                            result = SDL_RenderCopyEx(mRenderer, mTexture,
                                                                      &fromRect, &toRect,
                                                                      -mRotation,NULL,SDL_FLIP_NONE);
                                            
                                            if(result<0)
                                                {
                                                    cerr << "SDL_RenderCopyEx failed with error code: " << result << "|" << count<<  endl;
                                                    cerr << "SDL message: " <<  SDL_GetError() << endl;
                                                    cerr << *this << endl;
                                                    cerr << "Texture:" << mTexture << endl;
                                                    
                                                }
                                        }
                                    
                                    result =SDL_SetRenderTarget(mRenderer,NULL);
                                    
                                    if(result<0)
                                        {
                                            cerr << "Resetting SDL_SetRenderTarget failed with error code: " << result << endl;
                                            cerr << "SDL message: " <<  SDL_GetError() << endl;
                                            cerr << *this << endl;
                                            cerr << "Texture:" << mTexture << endl;
                                            
                                        }
                                } else{
                                
                                
                                
                                //SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
                                result = SDL_RenderCopyEx(mRenderer, mTexture,
                                                          &fromRect, &toRect,
                                                          -mRotation,NULL,SDL_FLIP_NONE);
                                
                                if(result<0)
                                    {
                                        cerr << "Rendering failed to NULL texture using SDL_RenderCopyEx with error code: " << result << endl;
                                        cerr << "SDL message: " <<  SDL_GetError() << endl;
                                        cerr << *this << endl;
                                        cerr << "Texture:" << mTexture << endl;
                                        
                                    }
                                
                            }
                        }
                    else
                        {
                            
                            // IF there is no parent, this is probably a window or an
                            //unassigned widget
                            
                        }
                }
        }else{
        
        //We will sometimes call a Draw() on an object that is not yet assigned
        //a renderer--not attached to a window, etc.
        //cout << "Trying to render widget with no renderer" << *this << endl;
    }
    return true;
}


//Sets a pixel to be a certain color.  This requires the object to have
//an active renderer. Otherwise...toobad.

bool PlatformWidget::SetPoint(int x, int y, PColor col)
{

    SDLUtility::DrawPixel(mRenderer,this,x,y,col);
    return true;
}


PColor PlatformWidget::GetPixel(int x, int y)
{
    if(mSurface)
        {
           return SDLUtility::GetPixelColor(mSurface,x,y);
        }else{

        return PColor(0,0,0,0);
    }

}



bool PlatformWidget::RotoZoom(double angle, double zoomx, double zoomy, int smooth)

{

#ifdef SDL2_DELETE
    //This is probably handled directly by SDL2
    SDL_Surface * tmp = rotozoomSurfaceXY(mSurface, angle,zoomx, zoomy, smooth);
    SDL_FreeSurface(mSurface);
    mSurface = tmp;


    //We need to reset the sizes.

    mWidth  = mSurface->w;
    mHeight = mSurface->h;
    PWidget::SetProperty("WIDTH", mSurface->w);
    PWidget::SetProperty("HEIGHT", mSurface->h);
    if(mSurface)return true;
    else return false;
#endif
    return false;
}


#ifdef SDL2_DELETE
SDL_Surface * PlatformWidget::GetSDL_Surface()
{
    return mSurface;
}
#endif





SDL_Texture * PlatformWidget::GetSDL_Texture()
{
    return mTexture;
}


bool PlatformWidget::AddToParentWidget(PlatformWidget * parent)
{


    SetParent(parent);


    //Child widgets currently are of one of three types:
    // * images/canvases have internal mSurfaces that may contain
    // data but don't have a texture yet
    // * text objects (labels, textboxes) don't have an msurface,
    // but when it is first draw()ed/render()ed, a temp surface
    // will get created and transferred to mTexture, which will
    //then be used; tmpSurface will be destroyed, and mSurface
    //not used
    // * DrawObjects(), which draw directly to the parent texture.
    // so that no rendercopy'ing is needed.  Here, no mSurface and no     // mTexture is used.




    //Assign us the parent's renderer.  There is only one renderer
    //per window, and so we can't creat the texture until it gets
    //added to the window.

    int out = SetRenderer(parent->GetRenderer());
    return out == 0;
}



bool PlatformWidget::SetRenderer(SDL_Renderer * renderer)
{

    //    cout<<"****************" << *this << "Setting renderer--" << renderer << endl;
    mRenderer = renderer;

    //Be sure renderer is set for all children, recursively
    std::list<PWidget*>::iterator i =  mSubWidgets.begin();
    while(i != mSubWidgets.end())
        {

            PlatformWidget* p =    dynamic_cast<PlatformWidget*>(*i);
            //cout << "setting child renderer:" << **i  <<"|" << mRenderer << endl;
            p->SetRenderer(renderer);

            i++;
        }


    return true;
}


bool PlatformWidget::AddSubWidget(PlatformWidget * child)
{
    //    cout << "Adding subwidget " << *child<<endl;


     if(strcmp(child->ObjectName().c_str(),"PlatformMovie")==0)
        {
            PError::SignalFatalError("Cannot add movie to another widget\n");
        } else {


        //if(! mRenderer)
        //            {
        //                PError::SignalFatalError("To add a child widget to a parent, renderer must exist.");
        //                exit(0);
        //            }


        //This manages the renderer and does a setparent()


        //This resets zorder:
        PWidget::AddSubWidget(child);
        // child->AddToParentWidget(this);
        child->SetParent(this);
        SetRenderer(GetRenderer());

    }

    return true;
}



bool PlatformWidget::RemoveSubWidget(PlatformWidget * child)
{



    PWidget::RemoveSubWidget(child);

    mSubWidgets.remove(dynamic_cast<PWidget*>(child));
    mSubWidgets.remove(child);
    //child->SetParent(NULL); //handled in pwidget::remove
    child->SetRenderer(NULL);

    return true;
}




bool PlatformWidget::RemoveSubWidgets()
{

    if(mSubWidgets.size()>0)
        {

            std::list<PWidget*>::iterator i = mSubWidgets.begin();
            while(i != mSubWidgets.end())
                {
                    //(*i)->SetParent(NULL);
                    //mSubWidgets.pop_front();
                    //This might not be the right thing to do:
                    //see removesubwidget above.
                    mSubWidgets.remove(*i);
                    //(*i)->SetParent(NULL);

                    //RemoveSubWidget(dynamic_cast<PlatformWidget*>(*i));
                    i=mSubWidgets.begin();  //readjust so it points to the start of the list.
                }

            mSubWidgets.clear();
        }

    return true;
}

void PlatformWidget::SetParent(PlatformWidget * parent)
{

    PWidget::SetParent(parent);
}

#ifdef SDL2_DELETE
void PlatformWidget::SetParentSurface(SDL_Surface* surface)
{
    mParentSurface = surface;
}


void PlatformWidget::SetParentTexture(SDL_Texture* texture)
{
    mParentTexture = texture;
}
#endif


void PlatformWidget::PrintSubWidgets(std::ostream & out)const
{

    if(mSubWidgets.size()>0)
        {

            std::list<PWidget*>::const_iterator i = mSubWidgets.begin();
            while(i != mSubWidgets.end())
                {

                    i++;
                }

        }else{

    }

}






///This needs to be used on some platforms/video cards
bool PlatformWidget::LockSurface()
{
#ifdef SDL2_DELETE
    if(SDL_MUSTLOCK(mSurface))
        {

            //cout << "Locking-------->" << endl;
            //The below returns 0 on success, -1 otherwise,
            //so reverse it for t/f
            if(SDL_LockSurface(mSurface)<0)
                {
                    cerr << "Need to lock surface but can't\n";
                    return false;
                }
            return true;
        }

#endif
    return false;

}

///This needs to be used on some platforms/video cards
bool PlatformWidget::UnlockSurface()
{
#ifdef SDL2_DELETE
   if(SDL_MUSTLOCK(mSurface))
        {
            // cout << "------->Unlocking" << endl;
            SDL_UnlockSurface(mSurface);
        }
#endif
   return true;
}
