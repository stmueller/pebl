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

//look here for hb/sdl example
//https://github.com/lxnt/ex-sdl-freetype-harfbuzz/blob/master/ex-sdl-freetype-harfbuzz.c

#include "PlatformFont.h"
#include "../../objects/PFont.h"
#include "../../objects/PColor.h"


#include "../../utility/PEBLPath.h"
#include "../../utility/PError.h"

#ifdef PEBL_EMSCRIPTEN
#include "../../base/Evaluator2.h"
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

// The following is adapted from code put on
// http://stackoverflow.com/questions/1031645/how-to-detect-utf-8-in-plain-c
// It has an implied public domain license

bool is_utf8(std::string str)
{


    if(str.length()==0)
        return false;

    const unsigned char * bytes = (const unsigned char *)(str.c_str());
    while(*bytes)
    {
        if(     (// ASCII
                        bytes[0] == 0x09 ||
                        bytes[0] == 0x0A ||
                        bytes[0] == 0x0D ||
                        (0x20 <= bytes[0] && bytes[0] <= 0x7E)
                )
        ) {
                bytes += 1;
                continue;
        }

        if(     (// non-overlong 2-byte
                        (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                        (0x80 <= bytes[1] && bytes[1] <= 0xBF)
                )
        ) {
                bytes += 2;
                continue;
        }

        if(     (// excluding overlongs
                        bytes[0] == 0xE0 &&
                        (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                ) ||
                (// straight 3-byte
                        ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
                                bytes[0] == 0xEE ||
                                bytes[0] == 0xEF) &&
                        (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                ) ||
                (// excluding surrogates
                        bytes[0] == 0xED &&
                        (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                )
        ) {
                bytes += 3;
                continue;
        }

        if(     (// planes 1-3
                        bytes[0] == 0xF0 &&
                        (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                        (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                ) ||
                (// planes 4-15
                        (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
                        (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                        (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                ) ||
                (// plane 16
                        bytes[0] == 0xF4 &&
                        (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                        (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                        (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                )
        ) {
                bytes += 4;
                continue;
        }

        return false;
    }

    return true;
}



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

    //initialize freetype library
    FT_Init_FreeType(mLibrary);

    

    
    string fontname = Evaluator::gPath.FindFile(mFontFileName);
    if(fontname == "")
        PError::SignalFatalError(string("Unable to find font file [")  + mFontFileName + string("]."));

    //These convert above properties to sdl-specific font
    //Open the font.  Should do error checking here.

    int tries = 0;
    //mTTF_Font = NULL;
    //load freetype face.
    FT_NewFace(mLibrary,fontname,0,mFace);

    //this line stolen from url below--not clear what it does.
    // https://github.com/wutipong/drawtext-sdl2-freetype2-harfbuzz/blob/master/sdl-ft-harfbuzz-outlinerender/main.cpp
    FT_SetPixelSizes(mFace,0,64);
    mHBFont = hb_ft_font_create(face,0);

    
     
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

            mTTF_Font = TTF_OpenFontRW(rw,0,mFontSize); //0 indicates to leave the RW memory stream open 
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


void RenderTexture(const std::wstring& text) 
{
    //    const SDL_Color& color, //==mSDL_FGColor
    //        const Font& font, // combines mFace and mHBFont
    //        SDL_Renderer*& renderer, //==mRenderer

    //        SDL_Texture*& target, //mTarget???
    //        SDL_Rect& rect)

    const FT_Int32& flags = FT_LOAD_DEFAULT;

    
	hb_buffer_t *buffer = hb_buffer_create();

    //This could be HB_DIRECTION_LTR; HB_DIRECTION_RTL; HB_DIRECTION_TTB, B_DIRECTION_BTT, HB_DIRECTION_INVALID;
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);  //hardcode as LTR for testing

    //could be HB_SCRIPT_ARABIC
    // HB_SCRIPT_LATIN
    // HB_SCRIPT_INVALID
    //    HB_SCRIPT_UNKNOWN, etc.
    //could use hb_scirpt_from_string to select this:
    hb_buffer_set_script(buffer, HB_SCRIPT_THAI);




    
	hb_buffer_add_utf16(buffer, 
		(unsigned short*)(text.c_str()), 
		text.length(),
                        0, text.length());

	hb_shape(mHBFont, buffer, NULL, 0);

	unsigned int glyph_count = hb_buffer_get_length(buffer);
	hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
	hb_glyph_position_t *glyph_positions = hb_buffer_get_glyph_positions(buffer, NULL);

    SDL_Rect rect;  //get size of rendering.

	CalculateSurfaceBound(glyph_infos, 
                          glyph_positions, 
                          glyph_count, 
                          mFace,
                          rect, 
                          flags);

	mTarget = SDL_CreateTexture(mRenderer, 
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_TARGET, 
                                rect.w, 
                                rect.h);

    SDL_SetTextureBlendMode(mTarget, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(mRenderer, mTarget);
	SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0);
	SDL_RenderClear(mRenderer);
	SDL_RenderFillRect(mRenderer, NULL);

	int baseline = -rect.y;
	int x = 0;

	for (unsigned int i = 0; i < glyph_count; i++) 
	{
		FT_Load_Glyph( mFace, 
			glyph_infos[i].codepoint,
			FT_LOAD_RENDER | flags);
		
		SDL_Surface* glyph_surface = CreateSurfaceFromFT_Bitmap(mFace->glyph->bitmap);//used to take a color--maybe this is needed?

		SDL_Rect dest;
		SDL_QueryTexture(glyph_texture, NULL, NULL, &dest.w, &dest.h);
		dest.x = x + (mFace->glyph->metrics.horiBearingX >> 6) + (glyph_positions[i].x_offset >> 6);
		dest.y = baseline - (mFace->glyph->metrics.horiBearingY >> 6) - (glyph_positions[i].y_offset >> 6);


		SDL_RenderCopy(mRenderer, glyph_texture, NULL, &dest);

		x += (glyph_positions[i].x_advance >> 6);

		SDL_DestroyTexture(glyph_texture);
	}

	hb_buffer_destroy(buffer);

	SDL_SetRenderTarget(mRenderer, NULL);
    return 
}



SDL_Surface * CreateSurfaceFromFT_Bitmap(const FT_Bitmap& bitmap)
{
    Uint32 rmask, gmask, bmask, amask;
    int req_format = STBI_rgb_alpha;
    
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = (req_format == STBI_rgb) ? 0 : 0xff000000;

    int depth, pitch;
    if (req_format == STBI_rgb) {
        depth = 24;
        pitch = 3*width; // 3 bytes per pixel * pixels per row
    } else { // STBI_rgb_alpha (RGBA)
        depth = 32;
        pitch = 4*width;
    }

    //we may have to translate buffer to pixels here.

    
    SDL_Surface * tmp = SDL_CreateRGBSurfaceFrom( (void*)(bitmap.buffer),               // dest_buffer from CopyTo
                                                   bitmap.width,        // in pixels
                                                   bitmap.height,       // in pixels
                                                   depth,               // in bits, so should be dest_depth * 8
                                                   pitch,                      // dest_row_span from CopyTo
                                                   rmask,gmask,bmas,amask);        // RGBA masks, see docs

    return tmp;
}

//This takes a FT bitmap and creates a texture directly. This mismatches
//the PEBL/SDL label and texture needs, so we won't move to this unless
//we can prove it works better.

SDL_Texture* CreateTextureFromFT_Bitmap(SDL_Renderer* renderer,
										const FT_Bitmap& bitmap, 
										const SDL_Color& color)
{
	SDL_Texture* output = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		bitmap.width,
		bitmap.rows);

	void *buffer;
	int pitch;
	SDL_LockTexture(output, NULL, &buffer, &pitch);

	unsigned char *src_pixels = bitmap.buffer;
	unsigned int *target_pixels = reinterpret_cast<unsigned int*>(buffer);

	SDL_PixelFormat* pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

	for (int y = 0; y < bitmap.rows; y++)
	{
		for (int x = 0; x < bitmap.width; x++)
		{
			int index = (y * bitmap.width) + x;

			unsigned int alpha = src_pixels[index];
			unsigned int pixel_value =
				SDL_MapRGBA(pixel_format, color.r, color.g, color.b, alpha);

			target_pixels[index] = pixel_value;
		}
	}

	SDL_FreeFormat(pixel_format);
	SDL_UnlockTexture(output);

	return output;
}

void CalculateSurfaceBound(	hb_glyph_info_t *glyph_infos,
							hb_glyph_position_t *glyph_positions, 
							const unsigned int& glyph_count,
							const FT_Face& face, 
							SDL_Rect& rect, 
							const FT_Int32& flags = FT_LOAD_DEFAULT) 
{
	int width = 0;
	int above_base_line = 0;
	int below_base_line = 0;

	for (unsigned int i = 0; i < glyph_count; i++) 
	{
		FT_Load_Glyph(face, glyph_infos[i].codepoint, FT_LOAD_DEFAULT | flags);
		width += (glyph_positions[i].x_advance >> 6);
		int bearing = 
			(face->glyph->metrics.horiBearingY >> 6) + (glyph_positions[i].y_offset >> 6);

		if (bearing > above_base_line)
			above_base_line = bearing;

		int height_minus_bearing = (face->glyph->metrics.height >> 6) - bearing;
		if (height_minus_bearing > below_base_line)
			below_base_line = height_minus_bearing;
	}

	rect.x = 0;
	rect.y = -above_base_line;
	rect.w = width;
	rect.h = above_base_line + below_base_line;

}


void DrawTextHB(conts std::wstring & Text)
{

    //These are the arguments originally passed in to the function:
    //const SDL_Color& color,
    SDL_Color& color = mSDL_FGColor;

    //const int& baseline,
    const int baseline= 0;
    //const int& x_start,
    const int x_start = 0;
    //const FT_Face& face //===mFace
    //hb_font_t* hb_font, //== mHBFont
    //SDL_Renderer*& renderer //= mRenderer

    //this will render directly onto wherever the renderer is currently pointing.
    //This might be smart, but labels and textboxes _expect_ a tmpSurface to be returned.
    //and so we need to create one here to use.


    
    mSurface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                    (int)mTextureWidth,
                                    (int)mTextureHeight, 32,
                                    rmask, gmask, bmask, amask);

    
    if(!mSurface)  PError::SignalFatalError("Surface not created in TextBox::RenderText.");

    
	hb_buffer_t *buffer = hb_buffer_create();

    //This could be HB_DIRECTION_LTR; HB_DIRECTION_RTL; HB_DIRECTION_TTB, B_DIRECTION_BTT, HB_DIRECTION_INVALID;
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);  //hardcode as LTR for testing

    //could be HB_SCRIPT_ARABIC
    // HB_SCRIPT_LATIN
    // HB_SCRIPT_INVALID
    //    HB_SCRIPT_UNKNOWN, etc.
    //could use hb_scirpt_from_string to select this:
    hb_buffer_set_script(buffer, HB_SCRIPT_THAI);


	hb_buffer_add_utf16(buffer, 
                        (unsigned short*)(text.c_str()),
                        text.length(),
                        0, 
                        text.length());

	hb_shape(mHBfont, buffer, NULL, 0);

	const unsigned int glyph_count = hb_buffer_get_length(buffer);
	const hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
	const hb_glyph_position_t *glyph_positions = hb_buffer_get_glyph_positions(buffer, NULL);

    //This renders directly onto the surface; it does not produce a surface.
	SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);

	int x = x_start;

	SpanAdditionData addl;

	addl.color = color;

	for (unsigned int i = 0; i < glyph_count; i++)
	{
		FT_Load_Glyph(mFace, glyph_infos[i].codepoint, FT_LOAD_NO_BITMAP);

		addl.dest.x = x + (glyph_positions[i].x_offset >> 6);
		addl.dest.y = baseline - (glyph_positions[i].y_offset >> 6);

		if (mFace->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
		{
			FT_Raster_Params params;
			memset(&params, 0, sizeof(params));
			params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
			params.gray_spans = DrawSpansCallback;
			params.user = &addl;

			FT_Outline_Render(library, &mFace->glyph->outline, &params);
		}//No fallback
		x += (glyph_positions[i].x_advance >> 6);
	}

	hb_buffer_destroy(buffer);
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


SDL_Surface * PlatformFont::RenderText(const std::wstring & text)
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
    mTexture = DrawTexture(toBeRendered);
    return tmpSurface;
}




SDL_Surface * PlatformFont::RenderText_old(const std::wstring & text)
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
             if(is_utf8(toBeRendered))
              {
                  tmpSurface = TTF_RenderUTF8_Shaded(mTTF_Font, toBeRendered.c_str(), mSDL_FGColor, mSDL_BGColor);
              } else {
                  tmpSurface = TTF_RenderText_Shaded(mTTF_Font, toBeRendered.c_str(), mSDL_FGColor, mSDL_BGColor);
              }

        }
    else
        {
           // tmpSurface = TTF_RenderText_Blended(mTTF_Font,toBeRendered.c_str(), mSDL_FGColor);
            if(is_utf8(toBeRendered) )
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
    
    if(is_utf8(toBeRendered))
        TTF_SizeUTF8(mTTF_Font,toBeRendered.c_str(),&width,&height); // should work on all utf8
    else
        TTF_SizeText(mTTF_Font,toBeRendered.c_str(),&width,&height); //breaks on non-ascii text
    
    // TTF_SizeUNICODE(mTTF_Font,toBeRendered.c_str(),&width,&height); //requires conversion to Uint16 array.
   
    unsigned int uwidth = (unsigned int)width;
    return uwidth;
}

unsigned int PlatformFont::GetTextHeight(const std::string & toBeRendered)
{
    int height, width;

    if(is_utf8(toBeRendered))
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

    unsigned int cutoff = 1;

    while(cutoff < text.size())
        {

            //If the width of the rendered text is larger than the x argument,
            unsigned int width = GetTextWidth(text.substr(0,cutoff));
            if(width > x)
                return cutoff-1;
            //cout << "***"<<width<< "<" << x << ":" << cutoff <<endl;
            cutoff++;
        }

    //If we make it this far, we have run out of letters, so return the last character.

    return (unsigned int)(text.size());
}


 std::ostream & PlatformFont::SendToStream(std::ostream& out) const
{
    out << "<SDL-Specific Font>" << std::flush;
    return out;
}



