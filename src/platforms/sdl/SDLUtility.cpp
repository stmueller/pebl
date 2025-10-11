//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/SDLUtility.h
//    Purpose:    Contains miscellaneous utility functions.
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

#include "SDLUtility.h"
#include "../../objects/PColor.h"
#include "../../utility/PError.h"
#include "../../base/PList.h"
#include "../../base/PComplexData.h"
#include "PlatformWindow.h"
#include "SDL.h"
#include "SDL_image.h"

//#if !defined(PEBL_OSX)
//#include <png.h>
//#endif
#include <math.h>
#include <iostream>


using std::cout;
using std::flush;
using std::endl;

/// These are SDL-specific utilities that don't fit into a single class very well.


SDL_Color  SDLUtility::PColorToSDLColor(PColor pcolor)
{
  SDL_Color scolor;
  scolor.r = pcolor.GetRed();
  scolor.g = pcolor.GetGreen();
  scolor.b = pcolor.GetBlue();
  scolor.a = pcolor.GetAlpha();

  return scolor;
}

PColor SDLUtility::SDLColorToPColor(SDL_Color scolor)
{

  return PColor(scolor.r, scolor.g, scolor.b, scolor.a);
}




///  This sets a pixel to be a certain color.
void SDLUtility::DrawPixel(SDL_Renderer *renderer, PlatformWidget * widget, int x, int y, PColor pcolor)
{
    
    cout << "drawing " << x << ","<< y << endl;
#if 0
    if(renderer)
        cout << "Line renderer: " << renderer << endl;
    else
        cout << "renderer is null\n";

#endif

    cout <<" texture " << widget->GetSDL_Texture() << endl;

    SDL_SetRenderTarget(renderer, widget->GetSDL_Texture());
    //    SDL_Color sdlcolor = PColorToSDLColor(pcolor);
    cout << pcolor.GetRed() <<
        "," << pcolor.GetGreen() << "," <<
        pcolor.GetBlue() << "," <<  pcolor.GetAlpha() << endl;
    
                                                         
                                                         
    SDL_SetRenderDrawColor(renderer, pcolor.GetRed(), pcolor.GetGreen(), pcolor.GetBlue(), pcolor.GetAlpha());
    //this offset may be  needed because renderer references coordinates to the window,
    //not the widget.  It may screw up drawing on center-coordinated widgets
    SDL_RenderPresent(renderer);
    
    //like images and labels.
    SDL_RenderDrawPoint(renderer,(int)(widget->GetX()+x),(int)(widget->GetY()+y));
    
    //    SDL_SetRenderTarget(renderer,NULL);
}

///  This sets a pixel to be a certain color.
void SDLUtility::DrawLine(SDL_Renderer *renderer,
                          PlatformWidget* widget,
                          int x1, int y1, int x2, int y2, PColor pcolor)
{
  //  SDL_Color sdlcolor = PColorToSDLColor(pcolor);
    //to draw to a widget, we need to set render target:
    int out = SDL_SetRenderTarget(renderer, widget->GetSDL_Texture());
    
    if(out<0)
        {
            cerr << "setrendertarget result in drawline: "<< out <<endl;
            cerr << SDL_GetError() <<endl;
        }
    
    SDL_SetRenderDrawColor(renderer,
                           pcolor.GetRed(), pcolor.GetGreen(),
                           pcolor.GetBlue(), pcolor.GetAlpha());
    

    //x and y position are absolute to window, so we need to adjust to
    //widget location
    
    int newX1 = x1;
    int newY1 = y1;
    int newX2 = x2;
    int newY2 = y2;


    SDL_RenderDrawLine(renderer,newX1,newY1,newX2,newY2);
    SDL_SetRenderTarget(renderer,NULL);

}



/*
     * Return the pixel value at (x, y)
     * NOTE: The surface must be locked before calling this!
     */
Uint32 SDLUtility::GetPixel(SDL_Surface *surface, int x, int y)
    {
        int bpp = surface->format->BytesPerPixel;
        /* Here p is the address to the pixel we want to retrieve */
        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

       switch (bpp) {
       case 1:
           return *p;

       case 2:
           return *(Uint16 *)p;

       case 3:
           if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
               return p[0] << 16 | p[1] << 8 | p[2];
           else
               return p[0] | p[1] << 8 | p[2] << 16;

       case 4:
           return *(Uint32 *)p;

     default:
         return 0;       /* shouldn't happen, but avoids warnings */
     } // switch
 }


/// This extracts the color of a pixel.
PColor SDLUtility::GetPixelColor(SDL_Surface *surface, int x, int y)
{
    Uint32 pxl =GetPixel(surface,x,y);
    Uint8 r;
    Uint8 g;
    Uint8 b;

    SDL_GetRGB(pxl, surface->format, &r,&g,&b);

    //    cout <<"Uint pxl:" << pxl << endl;
    PColor col = PColor(r,g,b,0);
    return col;
}



/*
 * Return the pixel value at (x, y)

 */

  //internet indicates that  the texture needs to be in streaming mode,
//and then it needs to be locked.
Uint32 SDLUtility::GetPixel(SDL_Renderer *renderer, SDL_Texture * texture, int x, int y)
    {

        int pitch,w,h;
        void* pixels;
        Uint32 format;
        SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
        SDL_QueryTexture(texture,&format,&w,&h,&pitch);
        SDL_LockTexture(texture,NULL,&pixels,&pitch);
        Uint32 * upixels = (Uint32*) pixels;

        //        int pixel[3];
        //        SDL_Rect rect = {x,y,1,1};
        //        SDL_RenderReadPixels(renderer,&rect,1,(void *)pixel,1*4);

        int i = pitch*(w*x+y);

       //upixels[i] should be the right pixel
        Uint32 pixel = upixels[i];
        SDL_UnlockTexture(texture);
        return pixel;


#ifdef SDL2_DELETE
        int bpp = surface->format->BytesPerPixel;
        /* Here p is the address to the pixel we want to retrieve */
        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

       switch (bpp) {
       case 1:
           return *p;

       case 2:
           return *(Uint16 *)p;

       case 3:
           if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
               return p[0] << 16 | p[1] << 8 | p[2];
           else
               return p[0] | p[1] << 8 | p[2] << 16;

       case 4:
           return *(Uint32 *)p;

     default:

         return 0;       /* shouldn't happen, but avoids warnings */
     } // switch
#endif


 }




/// This extracts the color of a pixel.
PColor SDLUtility::GetPixelColor(SDL_Renderer *renderer, SDL_Texture * texture, int x, int y)
{



        int pitch,w,h;
        void* pixels;
        Uint32  format;


        SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);
        SDL_QueryTexture(texture,&format,&w,&h,&pitch);
        int success = SDL_LockTexture(texture,NULL,&pixels,&pitch);
        if(success<0)
            {
                cout << SDL_GetError() << endl;
            }
        Uint32 * upixels = (Uint32*) pixels;
        SDL_PixelFormat* fmt= SDL_AllocFormat(format);
        //        int pixel[3];
        //        SDL_Rect rect = {x,y,1,1};
        //        SDL_RenderReadPixels(renderer,&rect,1,(void *)pixel,1*4);

        int i = pitch*(w*y+x);

       //upixels[i] should be the right pixel
        Uint32 pixel = upixels[i];
        SDL_UnlockTexture(texture);




    Uint8 r=0;
    Uint8 g=0;
    Uint8 b=0;
    Uint8 a = 0;

    SDL_GetRGBA(pixel,fmt, &r,&g,&b,&a);

    //    cout <<"Uint pxl:" << pxl << endl;
    //#endif

    PColor col = PColor(r,g,b,a);
    return col;
}



int SDLUtility::WritePNG(SDL_Renderer * renderer, SDL_Rect * rect, const std::string fname)
{


    
    //SDL_GetRendererOutputSize(renderer,rect->w,rect->h);
    SDL_Surface *sshot = SDL_CreateRGBSurface(0, rect->w,rect->h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(renderer, rect, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
    //SDL_SaveBMP(sshot, fname.c_str());
    //This is a secrit undocumented function in sdl_image
    int result =IMG_SavePNG(sshot, fname.c_str());
    SDL_FreeSurface(sshot);
    
    //returns 0 on success!
    return result;

}

Variant SDLUtility::GetCurrentScreenResolution()
{



  int i;

  // Declare display mode structure to be filled in.
  SDL_DisplayMode current;

  // Get current display mode of all displays.
  for(i = 0; i < SDL_GetNumVideoDisplays(); ++i){

    int should_be_zero = SDL_GetCurrentDisplayMode(i, &current);

    if(should_be_zero != 0)
      // In case of error...
      SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());

    else
      // On success, print the current display mode.
      SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz. \n", i, current.w, current.h, current.refresh_rate);

  }


    //THere is a new function for this, which depends
    // on the actual screen

    SDL_DisplayMode  info;// = SDL_GetVideoInfo();
    int success = SDL_GetCurrentDisplayMode(0,&info);
    //might also use SDL_GetDesktopDisplayMode()
    cout << "SUCCESS at getting display mode?:" <<success << endl;

    int w,h,rate;
    if(success==0)
        {
            cout << info.w << "," << info.h <<"," << info.refresh_rate << endl;
            w = info.w;
            h = info.h;
            rate =info.refresh_rate;

        } else {
        cout << "SDL_Init failed: " << SDL_GetError() << endl;
        PError::SignalFatalError(SDL_GetError());
    }


    PList * newlist = new PList();
    newlist->PushBack(Variant(w));
    newlist->PushBack(Variant(h));

    counted_ptr<PEBLObjectBase> newlist2 = counted_ptr<PEBLObjectBase>(newlist);
    PComplexData * pcd=new PComplexData(newlist2);
    return Variant(pcd);

}


void SDLUtility::CopyToClipboard(std::string text)
{

   int out = SDL_SetClipboardText(text.c_str());
   if(out<0)
       {
           PError::SignalWarning("Failed to copy text to clipboard\n");
       }
}

Variant SDLUtility::CopyFromClipboard()
{
    Variant out="";
    if(SDL_HasClipboardText())
        {
            char * text =   SDL_GetClipboardText();
            out = Variant(text);
            SDL_free(text);            
        }

    return out;
}


int SDLUtility::PopupErrorBox(PlatformWindow * pwindow,
                              const char* message)
{

    SDL_Window * window = pwindow->GetSDLWindow();
    return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                    "An error occurred\n",
                                    message,window);
}


long double  SDLUtility::GetTimeHP()
{
    //    std::cout << SDL_GetPerformanceFrequency() <<std::endl;
    std::cout << SDL_GetPerformanceCounter() <<"/"<< SDL_GetPerformanceFrequency() <<std::endl;
    return  ((long double)SDL_GetPerformanceCounter())/(long double)SDL_GetPerformanceFrequency()*1000.0;
}


Variant SDLUtility::GetDriverList(bool printout)
{

    PList * plist = new PList();
    PComplexData*pcd=NULL;

    if(printout)
        {
            std::cerr << "=================================================\n";
            std::cerr << "    Available drivers\n";
            std::cerr << "=================================================\n";
        }
    
    int numdrivers = SDL_GetNumRenderDrivers ();
    if(printout)   std::cerr << "Render driver count: " << numdrivers << endl;
    for (int i=0; i<numdrivers;   i++)
        {
            SDL_RendererInfo drinfo;
            SDL_GetRenderDriverInfo (i, &drinfo);
            if(printout)
                {
                    cout << "Driver name ("<<i<<"): " << drinfo.name << endl;
                    if (drinfo.flags & SDL_RENDERER_SOFTWARE) std::cerr << " the renderer is a software fallback" << endl;
                    if (drinfo.flags & SDL_RENDERER_ACCELERATED) std::cerr << " the renderer uses hardware acceleration" << endl;
                    if (drinfo.flags & SDL_RENDERER_PRESENTVSYNC) std::cerr << " present is synchronized with the refresh rate" << endl;
                    if (drinfo.flags & SDL_RENDERER_TARGETTEXTURE) std::cerr << " the renderer supports rendering to texture" << endl;
                }
            plist->PushBack(Variant(drinfo.name));
        }

    if(printout)      std::cerr << "=================================================\n";

    counted_ptr<PEBLObjectBase> tmplist = counted_ptr<PEBLObjectBase>(plist);
    pcd = new PComplexData(tmplist);
    Variant tmp = Variant(pcd);
    delete pcd;
   
    return tmp;
}
