//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/objects/PTextBox.cpp
//    Purpose:    Contains generic specification for a read-only text box.
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

#include "PTextBox.h"
#include "PFont.h"
#include "../utility/rc_ptrs.h"
#include "../utility/PEBLUtility.h"
#include "../devices/PKeyboard.h"
#include "../libs/PEBLEnvironment.h"
#include "../../libs/utfcpp/source/utf8.h"


//#include "SDL/SDL.h"   //For Uint16 only
#include <iostream>

using std::cout;
using std::endl;

PTextBox::PTextBox():
    PTextObject(""),
    mEditable(false),
    mCursorPos(0),
    mCursorChanged(true),
    mLineWrap(true),
    mJustify(1)
{
    InitializeProperty("TEXT",Variant(""));
    InitializeProperty("WIDTH",Variant(0));
    InitializeProperty("HEIGHT",Variant(0));
    InitializeProperty("EDITABLE",Variant(false));
    InitializeProperty("CURSORPOS",Variant(0));
    InitializeProperty("NAME",Variant("<TEXTBOX>"));
    InitializeProperty("LINEWRAP",Variant(1));
    InitializeProperty("LINEHEIGHT",Variant(0));
    InitializeProperty("NUMTEXTLINES",Variant(0));
    InitializeProperty("TEXTCOMPLETE",Variant(0));
    InitializeProperty("JUSTIFY",Variant("LEFT"));
}



PTextBox::PTextBox(std::string text, int width, int height):
    PTextObject(text),
    mEditable(false),
    mCursorPos(0),
    mCursorChanged(true),
    mLineWrap(true),
    mJustify(1)
{
    InitializeProperty("TEXT",Variant(text));
    InitializeProperty("WIDTH",Variant(width));
    InitializeProperty("HEIGHT",Variant(height));
    InitializeProperty("EDITABLE",Variant(false));
    InitializeProperty("CURSORPOS",Variant(0));
    InitializeProperty("NAME",Variant("<TEXTBOX>"));
    InitializeProperty("LINEWRAP",Variant(1));
    InitializeProperty("LINEHEIGHT",Variant(0));
    InitializeProperty("NUMTEXTLINES",Variant(0));
    InitializeProperty("TEXTCOMPLETE",Variant(0));
    InitializeProperty("JUSTIFY",Variant("LEFT"));
}


PTextBox::PTextBox( PTextBox & text)

{
    mChanged = true;
    mText = text.GetText();
    mEditable = false;
    mCursorPos = 0;
    mLineWrap = true;
    InitializeProperty("NAME",Variant("<TEXTBOX>"));
    InitializeProperty("LINEWRAP",Variant(1));
    InitializeProperty("LINEHEIGHT",Variant(0));
    InitializeProperty("NUMTEXTLINES",Variant(0));
    InitializeProperty("TEXTCOMPLETE",Variant(0));
    InitializeProperty("JUSTIFY",Variant("LEFT"));
}

PTextBox::~PTextBox()
{
}




bool PTextBox::SetProperty(std::string name, Variant v)
{

    if(PTextObject::SetProperty(name,v))
    {
        // If we set it at higher level, don't worry.
    }
    else if(name =="WIDTH")   SetWidth(v);
    else if(name == "HEIGHT") SetHeight(v);
    else if(name == "TEXT"){
        SetText(v.GetString());
        mCursorChanged=true;
        mChanged=true;}
    else if(name == "EDITABLE")SetEditable(v);
    else if(name == "CURSORPOS") SetCursorPosition(v);
    else if(name == "LINEWRAP") SetLineWrap(v);
    else if(name == "JUSTIFY") SetJustify(v);
    else return false;
    return true;
}


Variant PTextBox::GetProperty(std::string name)const
{
    return PTextObject::GetProperty(name);
}


ObjectValidationError PTextBox::ValidateProperty(std::string name, Variant v)const
{

    if(name == "JUSTIFY")
        {
            if(v=="LEFT" | v == "RIGHT" | v == "CENTER")
                {
                    return OVE_VALID;
                }else
                {
                    return OVE_INVALID_PROPERTY_VALUE;
                }
        }
    return ValidateProperty(name);
}

ObjectValidationError PTextBox::ValidateProperty(std::string name)const
{
    if(name == "CURSORPOS"| name=="LINEWRAP"| name == "JUSTIFY" | name == "NUMTEXTLINES" | name == "TEXTCOMPLETE")
        return OVE_VALID;
    else
        return PTextObject::ValidateProperty(name);
}




//Inserts text at cursor
//this is sort of broken when char=10 (newline) is inserted.
void PTextBox::InsertText(const std::string text)
{

    
    if(mCursorPos>mText.length())
        mCursorPos=mText.length();

    mText.insert(mCursorPos, text);
    mCursorPos += text.length();

    mChanged= true;
    mCursorChanged = true;
    SetProperty("TEXT",mText);

}

//Deletes text at cursor.  negative numbers indicate
//before the cursor.  Length should be the number of characters;
//not the number of bytes, so we need to delete entire unicode characters
//all at once.

void PTextBox::DeleteText(int length)
{

    int count = 0;
    int bytecount = 1;

    if(length > 0)
        {

            std::string::iterator start = mText.begin();
            std::string::iterator end = start+mCursorPos+1;

            while(count < length)
                {
                    
                    while(!utf8::is_valid(start,end) && end < mText.end())
                        {

                            end++;
                            bytecount++;
                        }
                    count++;
                    bytecount++;
                    }

            //We need to watch deleting at the end. If we try to bite off too much at
            //the end, just don't do it.
            if(bytecount+mCursorPos<mText.length())
                {    
                    mText.erase(mCursorPos, bytecount-1);
                    mChanged= true;
                }
        }
    else if (length < 0)
        {

            //Delete before the cursor.
            std::string::iterator start = mText.begin();
            std::string::iterator end = start+mCursorPos-1;


            //Delete before the cursor.
            while(count < (-length))
                {

                    //crash happens here when deleting from the beginning



                    while(!utf8::is_valid(start,end) && start < end)
                        {

                            bytecount++;
                            end--;   //delete backward to the end of the remaining
                            
                        }
                    count++;
                    bytecount++;
                }
            
            
            bytecount--; //back off one byte because it got double-incremented


            if((int)mCursorPos - bytecount < 0)
                {

                    mText.erase(0,mCursorPos);
                    mChanged= true;
                    mCursorPos = 0;

                }
            else
                {

                    mText.erase(mCursorPos-bytecount, bytecount);
                    mChanged= true;
                    mCursorPos -= bytecount;

                }
        }

    SetProperty("TEXT",mText);
}

long unsigned int PTextBox::IncrementCursor()
{

    
    //this just checks for end-of-line stuff.
    if(0)//!AtPrintableCharacter(mCursorPos))
        {
            mCursorPos ++;
        }
    else
        {
            
            
            if(mCursorPos > (int)(mText.length()))
                mCursorPos = mText.length();
            


            bool cont = true;
            std::string::iterator start = mText.begin()+mCursorPos;
            std::string::iterator end = start+1;
            mCursorPos++;
            
            while(!utf8::is_valid(start,end))
                {

                    mCursorPos += 1;
                    end++;
                }

        }


            mCursorChanged = true;


   return mCursorPos;
}

long unsigned int PTextBox::DecrementCursor()
{

    
    if(mCursorPos>=1)
        {
            

            bool cont = true;
            std::string::iterator start = mText.begin()+mCursorPos;
            std::string::iterator end = start;
            mCursorPos--;
            start--;
            while(!utf8::is_valid(start,end))
                {
                    mCursorPos -= 1;
                    start--;
                }

        }

    
//    if(AtPrintableCharacter(mCursorPos -1))
//        mCursorPos --;
//    else
//        mCursorPos -= 1;

    mCursorChanged = true;
    return mCursorPos;
}

//These shadow higher accessors in widget, because
//they need to set the textchanged flag
void PTextBox::SetHeight(int h)
{
    mHeight = h;
    mChanged = true;
}

void PTextBox::SetWidth(int w)
{
    mWidth = w;
    mChanged = true;
}


void PTextBox::SetLineWrap(bool state)
{
    mLineWrap = state;
}


void PTextBox::SetJustify(Variant j)
{

    mJustify =j;
    mChanged = true;
}


void PTextBox::HandleKeyPress(int keycode, int modkeys, Uint16 unicode)
{
 
    //First, handle special keys
    switch(keycode)
        {
#if 0
            if(0)
                {
                    InsertText(PEBLUtility::TranslateKeycode(PEBL_Keycode(keycode), modkeys));
                }
#endif
            break;
        }
    mCursorChanged=true;
}


//This will handle entry of processed text; not just
//keystrokes, to enable 'modrin' keyboarding.
void PTextBox::HandleTextInput(std::string text)
{


    InsertText(text);
    //we may need to adjust cursor position.
    mCursorChanged=true;
}


bool PTextBox::AtPrintableCharacter(unsigned long int x)
{
    unsigned long int pos=x;

    if(x> mText.length())
        pos = mText.length();

    if (mText[pos] == 10
        || mText[pos] == 13
        || mText[pos] == 18
        )
        return false;
    else
        return true;
}


std::string PTextBox::ObjectName()const
{
    return "TextBox";
}
