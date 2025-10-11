//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformTextBox.cpp
//    Purpose:    Contains SDL-specific interface for the text boxes.
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

#include "PlatformTextBox.h"
#include "../../objects/PTextBox.h"
#include "PlatformFont.h"
#include "SDLUtility.h"


#include "../../base/PComplexData.h"
#include "../../devices/PKeyboard.h"

#include "../../utility/rc_ptrs.h"
#include "../../utility/PError.h"
#include "../../utility/PEBLUtility.h"


#ifdef PEBL_OSX
#include "SDL.h"
#include "SDL_ttf.h"
#else
#include "SDL.h"
#include "SDL_ttf.h"
#endif
#include "SDL_scancode.h"

#include <stdio.h>
#include <string>
#include <algorithm>

//Unicode/utf-8 handling.
#include "../../../libs/utfcpp/source/utf8.h"

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::list;
using std::ostream;
using std::string;


PlatformTextBox::PlatformTextBox(string text, counted_ptr<PEBLObjectBase> font, int width, int height):
    PlatformWidget(),
    PTextBox(text, width, height)


{
    //this records if the text is valid UTF8. IF not (maybe it is iso 8859),
    //we will skip all the utf8 multi-byte stuff.
    mIsUTF8=  utf8::is_valid(text.begin(),text.end());
    mCDT = CDT_TEXTBOX;

    mWidth = width;
    mHeight = height;
    mTextureWidth=mWidth;
    mTextureHeight=mHeight;


    mRenderer = NULL;
    mSurface = NULL;
    mTexture = NULL;
    SetFont(font);
    SetText(text);
    mTextChanged = true;
    Draw();
    //    if(!RenderText()) cerr << "Unable to render text.\n";
}


PlatformTextBox::PlatformTextBox(const PlatformTextBox & text):
    PlatformWidget(),
    PTextBox(text.GetText(), (int)(text.GetWidth()), (int)(text.GetHeight()))


{
    mSurface = NULL;
    mTexture = NULL;
    mRenderer = NULL;
    mIsUTF8=  utf8::is_valid(mText.begin(),mText.end());
    mCDT = CDT_TEXTBOX;
    mWidth = text.GetWidth();
    mHeight = text.GetHeight();
    mTextureWidth=mWidth;
    mTextureHeight=mHeight;

    SetFont(text.GetFont());
    mTextChanged = true;
    Draw();
    //    if(!RenderText()) cerr << "Unable to render text.\n";
}


///Standard Destructor
PlatformTextBox::~PlatformTextBox()
{

    // PlatformWidget frees mSurface,
}

// Inheritable function that is called by friend method << operator of PComplexData
ostream & PlatformTextBox::SendToStream(ostream& out) const
{
    out << "<SDL PlatformTextBox: [" << mText << "] in " << *mFont << ">" <<flush;
    return out;
}




/// This method should be called when the font is initialized or the text is changed.
/// It will make the mSurface pointer hold the appropriate image.
bool  PlatformTextBox::RenderText()
{

    //free the memory if it is currently pointing at something.
    //#ifdef SDL2_DELETE
    if(mSurface)  SDL_FreeSurface(mSurface);
    //#endif

    //create a new surface on which to render the text.

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x00000000;

#else

    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0x00000000;

#endif

    //we might get a Draw() command before the renderer is set,
    //such as if the text is set before the object is added to a
    //parent that has a renderer.


    if(!mRenderer)
        {
            //cout << "No renderer " << SDL_GetTicks() << endl;
           
            return  false;
        }

    //cout << "creating new surface in platformtextbox::rendertext\n";

    //Make a surface of the prescribed size.
    mSurface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                    (int)mTextureWidth,
                                    (int)mTextureHeight, 32,
                                    rmask, gmask, bmask, amask);
    if(!mSurface)  PError::SignalFatalError("Surface not created in TextBox::RenderText.");

    //cout << "fillingbackground rec platformtextbox::rendertext\n";
    //Fill the box with the background color of the font.
    SDL_FillRect(mSurface, NULL, SDL_MapRGBA(mSurface->format,
                                             mBackgroundColor.GetRed(),
                                             mBackgroundColor.GetGreen(),
                                             mBackgroundColor.GetBlue(),
                                             mBackgroundColor.GetAlpha()));


    //First, find the height of the text when rendered with the font.
    int height = mFont->GetTextHeight(mText);

    //Now, go through the text letter by letter and word by word until it won't fit on a line any longer.
    unsigned int linestart = 0;
    unsigned int linelength = 0;
    unsigned int totalheight = 0;

    SDL_Surface * tmpSurface=NULL;
    std::vector<int>::iterator i = mBreaks.begin();
    linestart  = 0;

    //cout << "Textbox: "<< mHeight << " " <<totalheight << endl;

    while(i != mBreaks.end() && totalheight < (unsigned int) mHeight)
        {


            //mBreaks holds the 'starting' positions of each line.
            linelength = *i - linestart;
            if(linelength>0)
                {

                    SDL_Rect to;

                    if(mJustify == "RIGHT")
                        {
                            //right flush
                            tmpSurface = mFont->RenderText(mText.substr(linestart, linelength).c_str());
                            SDL_Rect tmprect = {mSurface->w - tmpSurface->w,
                                                static_cast<int>(totalheight),
                                                tmpSurface->w,tmpSurface->h};
                            to = tmprect;


                        }
                    else if (mJustify == "CENTER"){
                        //centered

                            tmpSurface = mFont->RenderText(mText.substr(linestart, linelength).c_str());
                            int xval = (mSurface->w - tmpSurface->w)/2;
                            SDL_Rect tmprect = {xval,static_cast<int>(totalheight),
                                                tmpSurface->w,tmpSurface->h};
                            to = tmprect;
                        
                        
                    }else{
                        //if(mJustify == "LEFT")   {//fallback to left-justify
                            tmpSurface = mFont->RenderText(mText.substr(linestart, linelength).c_str());
                            SDL_Rect tmprect = {0,static_cast<int>(totalheight),tmpSurface->w,tmpSurface->h};
                            to = tmprect;

                            //cout << "Rendering text :" << mText.substr(linestart,linelength)
                            //<<endl;
                        }


                    if(0)
                        {
                        //This was originally used for international right-to-left font layout, but
                        //it never worked.
                            std::string tmptext = mText.substr(linestart,linelength);

                            //
                            std::string rtext = PEBLUtility::strrev_utf8(tmptext);



                            //Re-render the text using the associated font.
                            tmpSurface = mFont->RenderText(rtext.c_str());


                            SDL_Rect tmprect = {(mSurface->w - tmpSurface->w),static_cast<int>(totalheight),
                                                tmpSurface->w, tmpSurface->h};
                            to = tmprect;

                        }


                    SDL_BlitSurface(tmpSurface, NULL, mSurface,&to);
                    SDL_FreeSurface(tmpSurface);
                }

            totalheight += height;
            linestart = *i;
            i++;

        }

    if(mTexture)
        {


            SDL_DestroyTexture(mTexture);
            mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                         (int)mTextureWidth,
                                         (int)mTextureHeight);
        }
    else
        {

            mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                         (int)mTextureWidth,(int)mTextureHeight);

        }




    //This will work except for drawing the cursor line on it. instead, create
    //a texture from the surface, then copy it to mtexture.
    //mTexture  = SDL_CreateTextureFromSurface(mRenderer, mSurface);

    SDL_Texture * tmp = SDL_CreateTextureFromSurface(mRenderer, mSurface);
    SDL_FreeSurface(mSurface);
    mSurface = NULL;
    SDL_SetRenderTarget(mRenderer, mTexture);
    SDL_RenderCopy(mRenderer, tmp, NULL, NULL);
    SDL_SetRenderTarget(mRenderer,NULL);
    SDL_DestroyTexture(tmp);

    if(mEditable)
        {
            DrawCursor();
        }


    //If mTexture is null, then rendering failed.
    if(mTexture)
        {

            mTextChanged = false;
            return true;
        }
    else
        {

            mTextChanged = true;
            return false;
        }
}


bool PlatformTextBox::SetProperty(std::string name, Variant v)
{

    if(name == "TEXT")
        {
            SetText(v);
        }
    else if(PTextBox::SetProperty(name,v))
    {
        // If we set it at higher level, don't worry.
    }
    else if (name == "FONT")
        {
            SetFont(v.GetComplexData()->GetObject());
        }
    else if(name=="WIDTH")
        {
            SetWidth(v);
                }
    else if(name=="HEIGHT")
        {
            SetHeight(v);
        }
    else return false;

    return true;
}

//These shadow higher accessors in widget, because
//they need to set the textchanged flag
void PlatformTextBox::SetHeight(int h)
{
    mTextureHeight = h;
    PTextBox::SetHeight(h);

}

void PlatformTextBox::SetWidth(int w)
{
    mTextureWidth = w;
    PTextBox::SetWidth(w);
}


void PlatformTextBox::SetFont(counted_ptr<PEBLObjectBase> font)
{

    mFontObject = font;
    mFont = dynamic_cast<PlatformFont*>(mFontObject.get());
    PWidget::SetBackgroundColor(mFont->GetBackgroundColor());
    mTextChanged = true;
    //Re-render the text onto mSurface
    //if(!RenderText()) cerr << "Unable to render text.\n";

}


void PlatformTextBox::SetText(string text)
{


    //Chain up to parent method.
    PTextObject::SetText(text);
    mIsUTF8=  utf8::is_valid(text.begin(),text.end());

    //mCursorPos = 0;
    mCursorChanged = true;
    mTextChanged = true;

    Draw();


    //Re-render the text onto mSurface
    //    if(!RenderText()) cerr << "Unable to render text.\n";

}



void PlatformTextBox::FindBreaks()
{

    //First, find the height and width of the text when rendered with the font.
    int height = mFont->GetTextHeight(mText);

    //Set this directly as a property; no need to check with PTextBox, which doesn't do anything with it.
    PEBLObjectBase::SetProperty("LINEHEIGHT",Variant(height));
    //Now, go through the text letter by letter and word by word until
    //it won't fit on a line any longer.

    unsigned int linestart = 0;     //The start of the current line
    unsigned int newlinestart = 0;  //The start of the NEXT line
    unsigned int linelength = 0;
    unsigned int totalheight = 0;

    mBreaks.clear();


    //Now, let's reserve space in mBreaks, roughly twice the amount we
    //think we need. This will make adding elements take less time.

    int width = mFont->GetTextWidth(mText);
    mBreaks.reserve(width / mWidth * 2);


    while( (totalheight < (unsigned int) mHeight  &&
            newlinestart < mText.size()))
        {


            linelength   = FindNextLineBreak(linestart);

            //Increment the placekeepers:
            //This is where the next line will start.

            newlinestart = linestart+ linelength;
            totalheight += height;

            mBreaks.push_back(newlinestart);
            linestart=newlinestart;

        }
}


/// This is the 'old' findnextlinebreak, which does not check for valid UTF-8 characters
/// using the utf library.  It will fail to display many CKJ characters, but I'm keeping it here
/// in case of emergency for now.
// #if 0
// int PlatformTextBox::FindNextLineBreakOld(unsigned int curposition)

// {
//     unsigned int sublength = 0;
//     unsigned int lastsep  = 0;
//     unsigned int sep      = 0;
//     std::string tmpstring;

//     //loop through the entire text from curposition on.
//     while (curposition + sublength < mText.size()+1)
//         {


            
//             //Get the width of the line right now.
//             tmpstring = mText.substr(curposition,sublength);
//             int tmpWidth = mFont->GetTextWidth(tmpstring);

//             //  int time1 = SDL_GetTicks();
//             cout << "................findnextlinebreak: " <<mLineWrap<< " "  << (curposition ) << ":" << (sublength ) << ":  "<< tmpWidth << "***" << ( time1) << endl;

//             //Test to see if curposition is a '10' or a '0' (a hard/explicit line break).  If so, this is a line break.
//             if(mText[curposition + sublength] == 10
//                || mText[curposition+sublength]==0
//                || tmpWidth>=mWidth)
//                 //|| curposition + sublength == mText.size())
//                 {

//                     //If the width of the current line is too big, break at the last separator.
//                     if(tmpWidth >= (unsigned int)(mWidth))
//                         {


//                             //if we are now too long, we should back up to the previous
//                             //break; unless that break was the beginning of the line.  If
//                             //that is the case, back up one character, and crudely break
//                             //within a text line.

//                             //we need to return the best linebreak here.
//                             if(sep >0)
//                                 {

//                                     return sep+1;
//                                 }
//                             if(lastsep>0)
//                                 {

//                                     return lastsep+1;
//                                 }
//                             else
//                                 {


//                                     if(mLineWrap)
//                                         {
//                                             cout << "linewrap 4\n";
//                                             cout << "Need to wrap but nowhere to wrap\n" << endl;
//                                             cout << tmpstring << endl;
//                                             //We have no natural break 'sep' on this line,
//                                             //and the current line is too long.
//                                             while(mFont->GetTextWidth(tmpstring) >(unsigned int)mWidth)
//                                                 {

//                                                     //we need to back off, but
//                                                     //sublength might break a character.
//                                                     sublength--;
//                                                     end = start+sublength;
//                                                     tmpstring = mText.substr(start,end);

//                                                 }

//                                             return sublength;
//                                         }
//                                     else
//                                         {
//                                             //We are past the end of the line, but still need to find the next line break on the text.
//                                             while (curposition + sublength < mText.size()+1)
//                                                 {
//                                                     if(mText[curposition + sublength] == 10
//                                                        || mText[curposition+sublength]==0)

//                                                         return sublength+1;
//                                                     else
//                                                         sublength++;
//                                                 }
//                                             return sublength+2;

//                                         }
//                                 }
//                         }
//                     else
//                         {

//                             return sublength+1;
//                         }
//                 }


//             //if we allow line wrapping, allow lines to wrap at other places too.

//             if(mLineWrap)
//                 {
//                     cout << "linerwrap\n";

//                     if(mText[curposition + sublength] == ' '
//                        || mText[curposition + sublength] == '-'
//                        || curposition + sublength == mText.size())
//                         {
//                             //either of these are word breaks; potential line breaks.
//                             //Increment word separator holders
//                             lastsep = sep;
//                             sep = sublength ;

//                             tmpstring = mText.substr(curposition, sublength);
//                             //Check the size of the line.
//                             if(mFont->GetTextWidth(tmpstring) > (unsigned int)mWidth)
//                                 {

//                                     //the text is too big for a single line, so return the last word break, but only
//                                     //if the size is greater than 0. In that case, return the current separator, which
//                                     //will not fit on the line, but it will get chopped off.

//                                     if(lastsep != 0)
//                                         {

//                                             return lastsep+1;
//                                         }
//                                     else
//                                         {

//                                             return sep+1;
//                                         }
//                                 }

//                         }


//                 }
//             else{
//                                     cout << "NO        linerwrap\n";
//             }
//             sublength++;
//         }
//     //The rest of the text must fit in the space allotted; return that number.


//     return sublength-1;
// }

// #endif

//This uses the utf headers to permit handling layout with UTF-8
//encoded text.  The text is maintained in a std::string.  Line breaks
//should return indices of the std::string--not codepoint positions.
//the important thing is that as you move through the string,
//you need to check that the text is legal UTF-8 (and not breaking)
//halfway through a codepoint.  Note that the length of the string is
//not the number of characters in the utf-8 string.

int PlatformTextBox::FindNextLineBreak(unsigned int curposition)
{
    unsigned int sublength = 0;
    unsigned int lastsep  = 0;
    unsigned int sep      = 0;
    std::string tmpstring;


    bool cont = true;
    std::string::iterator start;
    std::string::iterator end;

    //loop through the entire text from curposition on.
    while (curposition + sublength < mText.size()+1)
        {
            //cout << mText << endl;
            //cout << curposition << "->" << (curposition+sublength) << "==<" << mText.size() <<endl;
            
            //Get the width of the line right now.
            tmpstring = mText.substr(curposition,sublength);
            int tmpWidth = mFont->GetTextWidth(tmpstring);

            //  int time1 = SDL_GetTicks();


            //Test to see if curposition is a '10' or a '0' (a hard/explicit line break).
            //If so, this is a line break.
            if(mText[curposition + sublength] == 10
               || mText[curposition+sublength]==0
               || tmpWidth>=mWidth)
                //|| curposition + sublength == mText.size())
                {

                    
                    //If the width of the current line is too big, break at the last separator.
                    if(tmpWidth >= (unsigned int)(mWidth))
                        {


                            //if we are now too long, we should back up to the previous
                            //break; unless that break was the beginning of the line.  If
                            //that is the case, back up one character, and crudely break
                            //within a text line.

                            //we need to return the best linebreak here.
                            if(sep >0)
                                {

                                    return sep+1;
                                }
                            if(lastsep>0)
                                {

                                    return lastsep+1;
                                }
                            else
                                {


                                    if(mLineWrap)
                                        {

                                            //We have no natural break 'sep' on this line,
                                            //and the current line is too long.
                                            while(mFont->GetTextWidth(tmpstring) >(unsigned int)mWidth)
                                                {

 
                                                    //we need to back off, but
                                                    //sublength might break a character.
                                                    cont = true;
                                                    start =mText.begin()+curposition;
                                                    end = start+sublength;
                                                    
                                                    while(cont)
                                                        {
                                                            
                                                            sublength--;
                                                            end = start+sublength;
                                                            if(mIsUTF8)
                                                                cont = false;
                                                            else
                                                                cont =  !utf8::is_valid(start,end);
                                                        }
                                                    
                                                    tmpstring = mText.substr(curposition,sublength);
                                                    
                                                    
                                                }

                                            
                                            return sublength;
                                        }
                                    else
                                        {

                                            //No line-wrap here.
                                            //We are past the end of the line, but still need to find
                                            //the next line break on the text.
                                            while (curposition + sublength < mText.size()+1)
                                                {
                                                    
                                                    if(mText[curposition + sublength] == 10
                                                       || mText[curposition+sublength]==0)
                                                        {
                                                            
                                                            
                                                            return sublength+1;
                                                        }
                                                    else
                                                        {
                                                            
                                                            
                                                            cont = true;
                                                            start =mText.begin()+curposition;
                                                            end = start+sublength;

                                                            if(mIsUTF8)
                                                                {
                                                                    //This will advance one legal (utf) character:
                                                                    while(cont)
                                                                        {
                                                                            
                                                                            sublength++;
                                                                            end = start+sublength;
                                                                            cont =  !utf8::is_valid(start,end);
                                                                            
                                                                        }
                                                                }else{
                                                                
                                                                sublength++;
                                                                end=start+sublength;
                                                            }
                                                        }
                                                }
                                            
                                            return sublength+2;
                                            
                                        }
                                }}
                    
                    else  
                        {
                            

                            //originally sublength+1
                            return sublength+1;
                        }
                }
            

            //if we allow line wrapping, allow lines to wrap at other places too.

            
            if(mLineWrap)
                {


                    if(mText[curposition + sublength] == ' '
                       || mText[curposition + sublength] == '-'
                       || curposition + sublength == mText.size())
                        {
                            //either of these are word breaks; potential line breaks.
                            //Increment word separator holders
                            lastsep = sep;
                            sep = sublength ;

                            tmpstring = mText.substr(curposition, sublength);
                            //Check the size of the line.
                            if(mFont->GetTextWidth(tmpstring) > (unsigned int)mWidth)
                                {

                                    //the text is too big for a single line, so return the last word break, but only
                                    //if the size is greater than 0. In that case, return the current separator, which
                                    //will not fit on the line, but it will get chopped off.

                                    if(lastsep != 0)
                                        {

                                            return lastsep+1;
                                        }
                                    else
                                        {

                                            return sep+1;
                                        }
                                }

                        }


                }else{
                //here, we don't need to worry about mid-line linewraps.

                
            }


            
            //This is a 'normal' increment of sublength. Increment until the thing is legal UTF.

            cont = true;
            start =mText.begin()+curposition;
            end = start+sublength;


            while(cont)
                {
                    //   cout << "Checking:["<< mText.substr(curposition,sublength) << "]" << curposition  << "|" << sublength << ">>" <<  mText.length()<< "\n";
                    sublength++;
                    end = start+sublength;
                    if(mIsUTF8)
                        cont =  !utf8::is_valid(start,end);
                    else
                        cont = false;
                    
                    // cont = cont & (curposition+sublength)< mText.length(); //end when you get to the end of the text.

                }
        }
    //The rest of the text must fit in the space allotted; return that number.
            
            
    return sublength-1;
}




/// Given an x,y position, this will return an integer specifying
/// the character in mText before which the cursor should be n.
/// But, the text may be UTF-8, so an particular character may be several bytes wide
/// so we need to take care to compute cursor position correctly.
/// Cursor position should be related to the byte is the string,
/// but it should not allow half-byte character positions.
int PlatformTextBox::FindCursorPosition(long int x, long int y)
{


    if(mText.length()==0)
        {
            return 0;
        }

    //Find the height of a line.
    int height = mFont->GetTextHeight(mText);

    if(y > mHeight) y = mHeight;
    if(y < 0) y = 0;

    //The line will just be y / height, rounded down
    unsigned long int linenum = y / height;  //this is 0-based.


    //Change the line number to the last one if it is too large.
    //The last element of mBreaks is the 'end' of the text; not really a break for
    //our purposes.

    if(linenum > mBreaks.size())
        linenum = mBreaks.size();


    //find the starting character on the line we care about.
    int startchar;
    if(linenum==0)
        startchar = 0;
    else
        startchar = mBreaks[linenum-1];



    if(startchar >= mBreaks[mBreaks.size()-1])
        {
            return startchar;
        }

    //find the length in bytes of the current line:
    int length = mBreaks[linenum] - startchar;

    
    int charnum = mFont->GetPosition(mText.substr(startchar, length),
                                     (unsigned int)x);

    //finally, if the current cursor position is a non-printing character (i.e. a carriage return)
    //back up one

    if(!AtPrintableCharacter(charnum + startchar -1))
    {

        if(charnum + startchar > 0)
            charnum--;
    }


    return charnum + startchar;
}


//This will draw a 'cursor' at a specified character.
void PlatformTextBox::DrawCursor()
{
    //Find x and y of position.
    unsigned int x = 0;
    unsigned int y = 0;

    unsigned int height = mFont->GetTextHeight(mText);
    unsigned int width =  (unsigned int)GetWidth();
    unsigned int i = 0;
    int linestart = 0;


    x = width-1;    //Initialize x with the biggest value it can have.
    if(mCursorPos==0)
        {
            x=0;

        } else {
        //The cursor is not at the beginning.
        //mBreaks has found the line breaks already.
            while(i < mBreaks.size())
                {
                    //increment x,y while we go, so that if we get orphaned text,
                    //we still get a decent cursor position.
                    y = i * height;

                    //mBreaks is the position of the first character on each line
                    if(mBreaks[i] >= mCursorPos)
                        {

                            std::string subst = mText.substr(linestart, mCursorPos - linestart);

                            x =  mFont->GetTextWidth(subst);
                            x = ( x >= width ) ? width-1: x;
                            break;
                        }

                    linestart = mBreaks[i];
                    i++;
                }
            //x should be accurate, unless the cursor is at the last character,
            //and that last character is a carriage return.


        }

    //The current position should be OK, UNLESS the character at mCursorPos is a CR.
    //Then, we actually want to render CR on the next line.

    if(mText[mCursorPos-1]== 10)
        {
            y+=height;
            x=1;
        }



    //x,y specifies the top of the cursor.


    // here, if the textbox is attached to the window, things render fine.
    // If the textbox is a child of another widget (a canvas),
    // everything falls apart.


    SDLUtility::DrawLine(mRenderer, this,x, y, x, y+height,
                         (mFont->GetFontColor()));


}


//This overrides the parent Draw() method so that
//things can be re-rendered if necessary.
bool PlatformTextBox::Draw()
{
    if(mTextChanged)
    {
        FindBreaks();
    }

    if(mTextChanged || mCursorChanged)
    {
        
        RenderText();

    }

    mCursorChanged = false;


   bool ret =  PlatformWidget::Draw();

    if(mEditable&false)
       {
            DrawCursor();
       }


   return ret;
}


//Some key presses can only be handled by the platform-specific code.
//Do this here.
void PlatformTextBox::HandleKeyPress(int keycode, int modkeys)
{
#if 0
    cout << "handling keypress in PlatformTextBox: " << keycode << endl;
    cout << "(" <<  PEBL_KEYCODE_RETURN << "|" << PEBL_KEYCODE_RETURN2 << "|" <<          PEBL_KEYCODE_KP_ENTER << ")"<< std::endl;
    cout << "PEBL_KEYCODE_RIGHT:" << PEBL_KEYCODE_RIGHT << std::endl;
    cout << "PEBL_KEYCODE_LEFT:" << PEBL_KEYCODE_LEFT << std::endl;
#endif
    
    switch(keycode)
        {
        case PEBL_KEYCODE_UP:
        case PEBL_KEYCODE_DOWN:
            {
                int change;
                if(keycode == PEBL_KEYCODE_UP) change = -1;
                else change = 1;

                //Find x and y of position.
                int x = 0;
                int y = 0;

                int height=mFont->GetTextHeight(mText);
                unsigned int i = 0;
                int linestart = 0;


                while(i < mBreaks.size())
                    {
                        if(mBreaks[i] > mCursorPos)
                            {
                                y = i * height;
                                x = mFont->GetTextWidth(mText.substr(linestart, mCursorPos - linestart));
                                break;
                            }
                        linestart = mBreaks[i];
                        i++;
                    }


                mCursorPos = FindCursorPosition(x, y + change * height);
                mCursorChanged = true;
                break;
            }
        case PEBL_KEYCODE_RETURN:
        case PEBL_KEYCODE_RETURN2:
        case PEBL_KEYCODE_KP_ENTER:
            {

                std::string lb =  std::string()  + (char(10));

                InsertText(lb);
                //mCursorPos++;
                if(mCursorPos > (int) (mText.length()))
                    mCursorPos = mText.length();

            }
            break;
        case PEBL_KEYCODE_DELETE:

            DeleteText(1);
            break;
        case PEBL_KEYCODE_BACKSPACE:

            DeleteText(-1);
            break;

        case PEBL_KEYCODE_LEFT:
            DecrementCursor();
            break;

        case PEBL_KEYCODE_RIGHT:
            IncrementCursor();
            break;


            //case PEBL_KEYCODE_BACKSLASH:
            //InsertText("\\");
            //cout << "backslash\n";
            // InsertText(PEBLUtility::TranslateKeycode((PEBL_Keycode)keycode, modkeys));
            //break;

        default:
            //cout << "----------------------\n" << keycode << endl;
            //cout << "["<< (PEBLUtility::TranslateKeyCode(PEBLKey(keycode), modkeys))<<"]"<<endl;
            //cout << "----------------------\n";


            //    InsertText(SDL_GetKeyName(keycode));
            ;
        }
    mCursorChanged=true;
    //PTextBox::HandleKeyPress(keycode, modkeys, unicode);
}



//Some key presses can only be handled by the platform-specific code.
//Do this here.
void PlatformTextBox::HandleTextInput(std::string input)
{

    PTextBox::HandleTextInput(input);
}


Variant PlatformTextBox::GetLineBreaks()
{
    //Not sure about this, but let's not refind line breaks when this is called--just get
    //whatever has been calculated.

    std::vector<int>::iterator i = mBreaks.begin();

    PList * outlist = new PList();
    while(i != mBreaks.end())
        {
            outlist->PushBack(*i);
            i++;
        }


    counted_ptr<PEBLObjectBase> newList2 = counted_ptr<PEBLObjectBase>(outlist);

    PComplexData * PCD =(new PComplexData(newList2));
    Variant tmp = Variant(PCD);
    delete PCD;
    PCD=NULL;
    return tmp;

}
