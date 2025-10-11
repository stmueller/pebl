//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformFont.cpp
//    Purpose:    Contains SDL-specific Font Classes
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
#include "PlatformFont.h"
#include "../../objects/PFont.h"
#include "../../objects/PColor.h"

#include "../../utility/PEBLUtility.h"
#include "../../utility/PEBLPath.h"
#include "../../utility/PError.h"

#ifdef PEBL_EMSCRIPTEN
#include "../../base/Evaluator-es.h"
#else
#include "../../base/Evaluator.h"
#endif

//this is for right-to-left rendering supported by harfbuzz+freetype.
#ifdef PEBL_RTL
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#endif


#if defined(PEBL_OSX) | defined(PEBL_EMSCRIPTEN)
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_rwops.h"
#else
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_rwops.h"
#endif

#include <stdio.h>
#include <iostream>

using std::string;
using std::cout;
using std::endl;




//Stolen from
// http://stackoverflow.com/questions/5134404/c-read-binary-file-to-memory-alter-buffer-write-buffer-to-file


long unsigned int getFileSize(FILE **file){
    long unsigned int size;
    if(fseek(*file, 0, SEEK_END) == -1){ return -1; }
    size = ftell(*file);
    fseek(*file, 0, SEEK_SET);
    return size;
}



char * getFileBuffer(FILE **file, unsigned int fileSize){
    char *buffer = (char*)malloc(fileSize + 1);
    fread(buffer, sizeof(char),fileSize,*file);
    return buffer;
}

unsigned long int readFileToMemory(const char path[], char ** buffr){

    unsigned long int fileSize;
    FILE *file = fopen(path, "rb");
    if(file != NULL){
        fileSize = getFileSize(&file);
        //cout << "FIlesize: " << fileSize << endl;
        char* buff = getFileBuffer(&file,(unsigned int)fileSize);
       

        (*buffr) = buff;


        fclose(file);
        return fileSize;
    }else{
        *buffr = NULL;
        return 0;
    }
}



///Convenience constructor of PlatformFont:
PlatformFont::PlatformFont(const std::string & filename, int style, int size, PColor fgcolor, PColor bgcolor, bool aa):
    PFont(filename, style, size, fgcolor, bgcolor, aa)

{

    string fontname = Evaluator::gPath.FindFile(mFontFileName);
    if(fontname == "")
        PError::SignalFatalError(string("Unable to find font file [")  + mFontFileName + string("]."));

    //These convert above properties to sdl-specific font
    //Open the font.  Should do error checking here.

    int tries = 0;
    mTTF_Font = NULL;

    while(!mTTF_Font & (tries < 10))
        {


            unsigned long int size =  readFileToMemory(fontname.c_str(), &mBuffer);

            SDL_RWops *rw = SDL_RWFromMem(mBuffer,(int)size);
            //            SDL_RWops * src = NULL;

            //            if (rw != NULL) {
            //
            //                SDL_RWread(rw, src, sizeof (buf));
            //                SDL_RWclose(rw);
            //            }
            //            SDL_RWops * src = SDL_RWFromFile(fontname.c_str(),"r");

            //src->close();

            mTTF_Font = TTF_OpenFontRW(rw,1,mFontSize); //0 indicates to leave the RW memory stream open 
            //SDL_RWclose(rw);  //close the memory stream immediately. file when we are done.
            //free( buffr);
            tries++;
        }

    if(!mTTF_Font)
        {

            printf("Oh My Goodness, an error : [%s]\n", TTF_GetError());
            PError::SignalFatalError("Failed to create font\n");
        }

#ifndef PEBL_EMSCRIPTEN
    TTF_SetFontStyle(mTTF_Font, mFontStyle);
#endif

   //Translate PColor to SDLcolor for direct use in rendering.
    mSDL_FGColor = SDLUtility::PColorToSDLColor(mFontColor);
    mSDL_BGColor = SDLUtility::PColorToSDLColor(mBackgroundColor);

}



///Copy constructor of PlatformFont:
 PlatformFont::PlatformFont(PlatformFont & font)

{

    mFontFileName    = font.GetFontFileName();
    mFontStyle       = font.GetFontStyle();
    mFontSize        = font.GetFontSize();
    mFontColor       = font.GetFontColor();
    mBackgroundColor = font.GetBackgroundColor();
    mAntiAliased     = font.GetAntiAliased();


    //These convert above properties to sdl-specific font
    //Open the font.  Should do error checking here.
    mTTF_Font  = TTF_OpenFont(mFontFileName.c_str(), mFontSize);
    TTF_SetFontStyle(mTTF_Font, mFontStyle);

    //Translate PColor to SDLcolor for direct use in rendering.
    mSDL_FGColor = SDLUtility::PColorToSDLColor(mFontColor);
    mSDL_BGColor = SDLUtility::PColorToSDLColor(mBackgroundColor);

}


///Fonts are not geting cleaned up, I think.
///Standard destructor of PlatformFont
PlatformFont::~PlatformFont()
{


    TTF_CloseFont(mTTF_Font);
    mTTF_Font = NULL;

    free( mBuffer);
    mBuffer=NULL;
}



///Set*Color needs to be overridden because it doesn't change the SDL_Color data.
void PlatformFont::SetFontColor(PColor color)
{
    //Chain up to parent method
    PFont::SetFontColor(color);

    //Set child member data.
    mSDL_FGColor = SDLUtility::PColorToSDLColor(mFontColor);
}



///Set*Color needs to be overridden because it doesn't change the SDL_Color data.
void PlatformFont::SetBackgroundColor(PColor color)
{
    //Chain up to parent method
    PFont::SetBackgroundColor(color);

    //Set child member data.
    mSDL_BGColor = SDLUtility::PColorToSDLColor(mBackgroundColor);
}



SDL_Surface * PlatformFont::RenderText(const std::string & text)
{

    int maxchars = 1000;
#if 0
    cout << "About to render text [" << text  << "] with font " << *this << endl;

    int i  = 0;
    while(i < text.length())
    {

        cout << "[" << text[i] << "|" << (unsigned int)(text[i]) << "]";
        i++;
    }

    cout << endl;
#endif

    //If there is no text, return a null surface.
    // if(text=="") return NULL; don't do this;render anyway because this causes updates
    // to blank labels to fail in 2.0




    //Get a temporary pointer that we return
    std::string toBeRendered = StripText(text);
    SDL_Surface * tmpSurface = NULL;

    if(toBeRendered.length()>maxchars)
        {
            toBeRendered=toBeRendered.substr(0,maxchars);
        }
    //The text renderer doesn't like to render empty text.
    if(toBeRendered.length() == 0) toBeRendered = " ";

    //Using the RenderUTF8 stuff below has a hard time with 'foreign' characters; possibly because
    //the toberendered needs to be converted to UTF-8????

    
#ifdef PEBL_EMSCRIPTEN
    tmpSurface =  TTF_RenderText_Blended(mTTF_Font, toBeRendered.c_str(), mSDL_FGColor);
#else

    //Note, renderUTF paths are not available in EMSCRIPTEN.
    if(mAntiAliased)
        {

            // toBeRendered might need to be converted to UTF8
            if(PEBLUtility::is_utf8(toBeRendered))
              {
                  tmpSurface = TTF_RenderUTF8_Shaded(mTTF_Font, toBeRendered.c_str(), mSDL_FGColor, mSDL_BGColor);
              } else {
                  tmpSurface = TTF_RenderText_Shaded(mTTF_Font, toBeRendered.c_str(), mSDL_FGColor, mSDL_BGColor);
              }

        }
    else
        {
           // tmpSurface = TTF_RenderText_Blended(mTTF_Font,toBeRendered.c_str(), mSDL_FGColor);
            if(PEBLUtility::is_utf8(toBeRendered) )
             {
               tmpSurface = TTF_RenderUTF8_Blended(mTTF_Font,toBeRendered.c_str(), mSDL_FGColor);
             }
            else
             {
                tmpSurface =  TTF_RenderText_Blended(mTTF_Font, toBeRendered.c_str(), mSDL_FGColor);
             }
        }
#endif
    //
    //TTF_RenderText_Blended(
    //TTF_Font *font, // This is the TTF_Font to use.
    //char *cstr, // This is the text to render.
    //                     SDL_Color &clr, // This is the color to use.
    //                     );




    if(tmpSurface)
        {
            return tmpSurface;
            //we need to copy the surface to the texture, render it, and return I think.

        }
    else
        {
            string message =   "Unable to render text  [" +  toBeRendered + "] in PlatformFont::RenderText. Attempting to render '' instead\n";
            PError::SignalWarning(message);

            //We have a problem, probably because we are trying to render garbage. Let's try to render "" instead, and
            //signal a waring.  If there is still an error, we will signal a fatal error.
            std::string tmp = "";
            tmpSurface =  TTF_RenderText_Blended(mTTF_Font, tmp.c_str(), mSDL_FGColor);

           if(tmpSurface)
            {
                return tmpSurface;
            }
            else
            {
              string message =   "Unable to render text in PlatformFont::RenderText";
              PError::SignalFatalError(message);
            }
        }
    return NULL;
}


//This transforms an escape-filled text string into its displayable version.
std::string PlatformFont::StripText(const std::string & text)
{
    //First, transform text into the to-be-rendered text.  I.E., replace
    //escape characters etc.
    //This might destroy UTF-formatting and stuff, so we have to be careful.

    std::string toBeRendered;

    for(unsigned int i = 0; i < text.size(); i++)
        {


            if(text[i] == 10 ||
               text[i] == 13 ||
               text[i] == 18)
                {
                    //Do nothing.;

                }
            else if(text[i] == 9)
                {
                    //This is a tab character. First, figure out
                    //what absolute position it should be in: round
                    //the length eof toBeRendered up to the next value
                    //i mod 4.

                    int x = 8*(((int)(toBeRendered.length())+1) /8 + 1 );
                    int diff = (int)x-(int)(toBeRendered.length());
                    string tmp = " ";
                    for(int j = 0; j < diff; j++)
                        {
                            toBeRendered.push_back(tmp[0]);
                        }

                }
            else
                {
                    toBeRendered.push_back(text[i]);
                }

        }

    return toBeRendered;
}


unsigned int PlatformFont::GetTextWidth(const std::string & text)
{
    int height, width;
    std::string toBeRendered = StripText(text.c_str());

    
    if(PEBLUtility::is_utf8(toBeRendered))
        {
            
            TTF_SizeUTF8(mTTF_Font,toBeRendered.c_str(),&width,&height); // should work on all utf8
        }
    else
        TTF_SizeText(mTTF_Font,toBeRendered.c_str(),&width,&height); //breaks on non-ascii text
    
    // TTF_SizeUNICODE(mTTF_Font,toBeRendered.c_str(),&width,&height); //requires conversion to Uint16 array.
   
    unsigned int uwidth = (unsigned int)width;
    return uwidth;
}

unsigned int PlatformFont::GetTextHeight(const std::string & toBeRendered)
{
    int height, width;

    if(PEBLUtility::is_utf8(toBeRendered))
        TTF_SizeUTF8(mTTF_Font,toBeRendered.c_str(),&width,&height); // should work on all utf8
    else
        TTF_SizeText(mTTF_Font,toBeRendered.c_str(),&width,&height); //breaks on non-ascii text

    unsigned int uheight = (unsigned int)height;
    return uheight;
}


//This returns the nearest character to the pixel column specified by x
unsigned int PlatformFont::GetPosition(const std::string & text, unsigned int x)
{

    //Start at 0 and check until the width of the rendered text is bigger than x

    unsigned int lastcutoff = 0;
    unsigned int lastwidth = 0;
    
    unsigned int cutoff = 1;

    std::string::const_iterator start;
    std::string::const_iterator end;

    start = text.begin();
    end = start+1;

    while(cutoff < text.size())
        {

            //the bytes don't align with the glyphs. Be sure we only
            //permit the cutoff to occur between valid utf8 glyphs.
            while(!PEBLUtility::is_utf8(text.substr(0,cutoff)))
                {
                    //end++;
                    cutoff++;
                }
            //If the width of the rendered text is larger than the x argument,
            unsigned int width = GetTextWidth(text.substr(0,cutoff));
            if(width > x)
                {
                    //width is greater that the x position. now, round to the
                    //best cutoff.
                    double prop = (x-lastwidth)/(width-lastwidth);
                    if(prop>.5)
                        {
                            return cutoff;
                        }
                    else
                        {
                            return lastcutoff;
                        }
                    
                  
                }
            lastcutoff = cutoff;
            lastwidth = width;

            cutoff++;
                //end++;
        }

    //If we make it this far, we have run out of letters, so return the last character.

    return (unsigned int)(text.size());
}


 std::ostream & PlatformFont::SendToStream(std::ostream& out) const
{
    out << "<SDL-Specific Font>" << std::flush;
    return out;
}



