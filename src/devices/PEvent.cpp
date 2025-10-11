//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:        src/devices/PEvent.cpp
//    Purpose:     Events processed by Event Loop.
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
#include "PEvent.h"
#include "../utility/PError.h"
#include "../base/PList.h"
#include "../base/PComplexData.h"
#include "../utility/PEBLUtility.h"


#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

PEvent::PEvent(PEBL_DEVICE_TYPE type,
               long unsigned int time,
               unsigned int windowID=0):
	mType(type),
    mEventTime(time)
{
}

PEvent::PEvent(const PEvent & evt)

{

    mEventTime=evt.GetEventTime();
    mType = evt.GetType();
    mWindowID = 0;

    switch(mType)
		{
		case PDT_KEYBOARD:
            {
                PEBL_KeyboardEvent event = evt.GetKeyboardEvent();
                mEvent.keyboardEvent = event;
            }
			break;
        case PDT_TEXT_EDITING:
            {
                PEBL_TextEditingEvent event = evt.GetTextEditingEvent();
                mEvent.textEditingEvent = event;
            }
            break;
        case PDT_TEXT_INPUT:
            {
                PEBL_TextInputEvent event = evt.GetTextInputEvent();
                mEvent.textInputEvent =event;
            }

            break;
		case PDT_MOUSE_MOVEMENT:
            {
                PEBL_MouseMovementEvent event = evt.GetMouseMovementEvent();
                mEvent.mouseMovementEvent = event;
            }
            break;

		case PDT_MOUSE_BUTTON:
            {
                PEBL_MouseButtonEvent event = evt.GetMouseButtonEvent();
                mEvent.mouseButtonEvent = event;
            }
            break;
                
        case PDT_MOUSE_WHEEL:
            {
                PEBL_MouseWheelEvent event = evt.GetMouseWheelEvent();
                mEvent.mouseWheelEvent = event;
            }
            break;
		case PDT_MOVIE_REFRESH:
            {
                PEBL_MovieRefreshEvent event = evt.GetMovieRefreshEvent();
                mEvent.movieRefreshEvent = event;
            }
            break;

		case PDT_MOVIE_END:
            {
                PEBL_MovieEndEvent event = evt.GetMovieEndEvent();
                mEvent.movieEndEvent = event;
            }
            break;
        case PDT_WINDOW_RESIZE:
            {
                //do something here that will cause a redraw.
            }
		case PDT_TIMER:
        case PDT_UNKNOWN:
		default:
            ;
		}

}

void PEvent::SetKeyboardEvent(const PEBL_KeyboardEvent & evt)
{
	mType = PDT_KEYBOARD;
    mEvent.keyboardEvent = evt;
}



void PEvent::SetTextEditingEvent(const PEBL_TextEditingEvent & evt)
{
	mType = PDT_TEXT_EDITING;
    mEvent.textEditingEvent= evt;
}

void PEvent::SetTextInputEvent(const PEBL_TextInputEvent & evt)
{
	mType = PDT_TEXT_INPUT;
    mEvent.textInputEvent = evt;
}


void PEvent::SetMouseMovementEvent(const PEBL_MouseMovementEvent & evt)
{
    mType = PDT_MOUSE_MOVEMENT;
    mEvent.mouseMovementEvent = evt;
}

void PEvent::SetMouseButtonEvent(const PEBL_MouseButtonEvent & evt)
{
    mType = PDT_MOUSE_BUTTON;
    mEvent.mouseButtonEvent = evt;
}



void PEvent::SetMouseWheelEvent(const PEBL_MouseWheelEvent & evt)
{
    mType = PDT_MOUSE_WHEEL;
    mEvent.mouseWheelEvent = evt;
}

void PEvent::SetMovieRefreshEvent(const PEBL_MovieRefreshEvent & evt)
{
    mType = PDT_MOVIE_REFRESH;
    mEvent.movieRefreshEvent = evt;
}

void PEvent::SetMovieEndEvent(const PEBL_MovieEndEvent & evt)
{
    mType = PDT_MOVIE_END;
    mEvent.movieEndEvent = evt;
}

void PEvent::SetWindowEvent(const PEBL_WindowEvent & evt)
{

    mType = PDT_WINDOW_RESIZE;
    mEvent.windowEvent = evt;
}


void PEvent::SetDummyEvent(const PEBL_DummyEvent & evt)
{
    mEvent.dummyEvent = evt;
}

PEvent::~PEvent()
{

    //These .text properties get cleaned up by SDL,
    //as they are borrowed from the SDL event.
    if(PDT_TEXT_INPUT==mType)
        {
            
            //free(mEvent.textInputEvent.text);
        }else if(PDT_TEXT_EDITING==mType)
        {
            //free(mEvent.textEditingEvent.text);
        }
}



//transforms event into a variant
//that is exposed to the user.

PEvent::operator Variant()
{
    
    Variant ret;

    //cout << "Changing to variant: " << GetType() << endl;
    switch(GetType())
        {
        case PDT_MOUSE_MOVEMENT:
            {
            PEBL_MouseMovementEvent mbe=GetMouseMovementEvent();
            PList * newlist = new PList();
            newlist->PushBack(Variant((int)mbe.x));
            newlist->PushBack(Variant((int)mbe.y));
            newlist->PushBack(Variant((int)mbe.relx));
            newlist->PushBack(Variant((int)mbe.rely));
            counted_ptr<PEBLObjectBase> newlist2 = counted_ptr<PEBLObjectBase>(newlist);
           PComplexData *   pcd = new PComplexData(newlist2);
            
           ret = Variant(pcd);
            }
           break;

           
        case PDT_MOUSE_BUTTON:
            {
            PEBL_MouseButtonEvent mbe=GetMouseButtonEvent();
            PList * newlist = new PList();
            newlist->PushBack(Variant((int)mbe.x));
            newlist->PushBack(Variant((int)mbe.y));
            newlist->PushBack(Variant((int)mbe.button));
            newlist->PushBack(Variant((int)mbe.state));
            counted_ptr<PEBLObjectBase> newlist2 = counted_ptr<PEBLObjectBase>(newlist);
           PComplexData *   pcd = new PComplexData(newlist2);
            
           ret = Variant(pcd);
            }
           break;

        case PDT_MOUSE_WHEEL:
            {
                PEBL_MouseWheelEvent mwe=GetMouseWheelEvent();
                PList * newlist = new PList();
                
                newlist->PushBack(Variant((int)mwe.x));
                newlist->PushBack(Variant((int)mwe.y));
                newlist->PushBack(Variant((int)mwe.deltax));
                newlist->PushBack(Variant((int)mwe.deltay));
                newlist->PushBack(Variant((int)mwe.direction));
                counted_ptr<PEBLObjectBase> newlist2 = counted_ptr<PEBLObjectBase>(newlist);
                PComplexData *   pcd = new PComplexData(newlist2);
                
                ret = Variant(pcd);
            }
           break;

           
        case PDT_KEYBOARD:
            {
            PEBL_KeyboardEvent pke=GetKeyboardEvent();
            ret = Variant(PEBLUtility::TranslateKeycode(pke.key, 0));
            }
            break;
        case PDT_TIMER:
            ret = Variant("<timeout>");
            break;
            
        case PDT_TEXT_INPUT:
            {
            PEBL_TextInputEvent ptie = GetTextInputEvent();
            ret = Variant(ptie.text);
            }
            break;
            
        default:
            {
            ret = Variant("<UNKNOWN_EVENT>");
            }
        }

    ret.SetCreationTime(mEventTime);
    return ret;

}


PEBL_KeyboardEvent PEvent::GetKeyboardEvent() const
{
    if(PDT_KEYBOARD ==  mType )

        return mEvent.keyboardEvent;
    else
        {
            PEBL_KeyboardEvent tmp;

            tmp.scancode = PEBL_KEYCODE_UNKNOWN;
            tmp.key = PEBL_KEYCODE_UNKNOWN;
            tmp.state = PEBL_UNKNOWN;
            tmp.modkeys = 0;
            return tmp;
        }
}


PEBL_TextEditingEvent PEvent::GetTextEditingEvent() const
{
    if(PDT_TEXT_EDITING ==  mType )
        return mEvent.textEditingEvent;
    else
        {
            PEBL_TextEditingEvent tmp;
            tmp.window =0;
            tmp.text = (char*)""; //depracated warning send by this
            return tmp;
        }
}
PEBL_TextInputEvent PEvent::GetTextInputEvent() const
{
    if(PDT_TEXT_INPUT==  mType )
        return mEvent.textInputEvent;
    else
        {

            PEBL_TextInputEvent tmp;
            tmp.window =0;
            tmp.text = (char*)"";
            tmp.start=0;
            tmp.length=0;

            return tmp;
        }
}

PEBL_MouseMovementEvent PEvent::GetMouseMovementEvent() const
{
    if(PDT_MOUSE_MOVEMENT ==  mType )
        return mEvent.mouseMovementEvent;
    else
        {
            PEBL_MouseMovementEvent tmp;
            tmp.x = 0;
            tmp.y = 0;
            tmp.relx = 0;
            tmp.rely = 0;
            return tmp;
        }
}


PEBL_MouseButtonEvent PEvent::GetMouseButtonEvent() const
{
    if(PDT_MOUSE_BUTTON ==  mType )
        return mEvent.mouseButtonEvent;
    else
        {
            PEBL_MouseButtonEvent tmp;
            tmp.x = 0;
            tmp.y = 0;
            tmp.button = 0;
            tmp.state = 0;
            return tmp;
        }
}

PEBL_MouseWheelEvent PEvent::GetMouseWheelEvent() const
{
   if(PDT_MOUSE_WHEEL == mType)
       return mEvent.mouseWheelEvent;
   
   else{
       PEBL_MouseWheelEvent tmp;
       tmp.x = 0;
       tmp.y = 0;
       tmp.deltax = 0;
       tmp.deltay= 0;
       tmp.direction = 1;
       return tmp;
    }
}


PEBL_MovieRefreshEvent PEvent::GetMovieRefreshEvent() const
{
    if(PDT_MOVIE_REFRESH ==  mType )
        return mEvent.movieRefreshEvent;
    else
        {
            PEBL_MovieRefreshEvent tmp;
            tmp.value = true;
            return tmp;
        }
}

PEBL_MovieEndEvent PEvent::GetMovieEndEvent() const
{
    if(PDT_MOVIE_END ==  mType )
        return mEvent.movieEndEvent;
    else
        {
            PEBL_MovieEndEvent tmp;
            tmp.value = true;
            return tmp;
        }
}


PEBL_DummyEvent PEvent::GetDummyEvent() const
{
    if(PDT_DUMMY == mType)
        return mEvent.dummyEvent;
    else
        {
            PEBL_DummyEvent tmp;
            tmp.value = 0;
            return tmp;
        }
}







PEBL_WindowEvent PEvent::GetWindowEvent()const
{
    if(PDT_WINDOW_RESIZE == mType)
        return mEvent.windowEvent;
    else
        {
            PEBL_WindowEvent tmp;
            tmp.w = 0;
            tmp.h = 0;
            return tmp;
        }

}


int PEvent::GetState(int iface) const
{

    //cout << "Getting state : " << iface << "on type " << mType << " mousetype is " << PDT_MOUSE_WHEEL << endl;

    //  This gets the 'state' described in an event.
    switch(mType)
        {

            //For keyboard events, the interface is the key.
        case PDT_KEYBOARD:
            
            //If the interface is the "anykey", return the key state
            if((PEBL_Keycode)iface == PEBL_KEYCODE_ANYKEY)
                {

                    return mEvent.keyboardEvent.state;
                }


            //Determine which key is being used.

            if(mEvent.keyboardEvent.key == (PEBL_Keycode)iface)
                {

                    return mEvent.keyboardEvent.state;
                }
            else
                return 0;

            break;

        case PDT_TEXT_EDITING:
            {
                return 1;
                //return mEvent.textInputEvent;
            }
            break; 
            
        case PDT_TEXT_INPUT:
            {
                return 1;
                //return mEvent.textEditingEvent;
            }
            break; 

        case PDT_MOUSE_BUTTON:
            {
                

                //cout << "Getting button state in PEvent.cpp:"<<
                //mEvent.mouseButtonEvent.button << "--" << iface << endl;
            if(mEvent.mouseButtonEvent.button == iface)
                {

                    return mEvent.mouseButtonEvent.state;
                }else
                {
                    return PEBL_UNKNOWN;
                }
            
            
            }
            break;
        case PDT_MOUSE_WHEEL:
            {
                //cout << "mouse wheel: " << iface <<"|" << mEvent.mouseWheelEvent.deltax<< "|" <<
                //mEvent.mouseWheelEvent.deltay << endl;
                if(iface==1)
                    {
                        return (int)(mEvent.mouseWheelEvent.deltax);
                    }
                else if(iface==2)
                    {
                        return (int)(mEvent.mouseWheelEvent.deltay);
                    }
                else
                    return PEBL_UNKNOWN;
            }
            break;
            
        case PDT_MOUSE_MOVEMENT:
        case PDT_TIMER:
        case PDT_MOVIE_REFRESH:
        case PDT_MOVIE_END:
        case PDT_UNKNOWN:
            return PEBL_UNKNOWN;

        case PDT_WINDOW_RESIZE:
            {
            //Here, the state should describe the size of the window resize.
            if(iface == PEBL_WINDOWWIDTH)
                {
                    return mEvent.windowEvent.w;
                }else
                {
                    return mEvent.windowEvent.h;
                }
            break;
            }
        default:

            cerr << mType << endl;
            PError::SignalFatalError("Unhandled device type in PEvent::GetState\n");

            break;
        }


    return PEBL_UNKNOWN;
}

