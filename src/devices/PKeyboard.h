//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PKeyboard.h
//    Purpose:    Primary Keyboard Interface Device
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
#ifndef __PKEYBOARD_H__
#define __PKEYBOARD_H__

//#include "SDL_stdinc.h"
//#include "SDL_scancode.h"
#include <SDL_keycode.h>
#include "PDevice.h"

#include <list>
#include <string>

class ValueState;


//The following mimics the SDL almost exactly. 


//#include <SDL_scancode.h>


/* Enumeration of valid key mods (possibly OR'd together) */
enum PEBLModKey
    {
        PEBLMOD_NONE  = 0x0000,
        PEBLMOD_LSHIFT= 0x0001,
        PEBLMOD_RSHIFT= 0x0002,
        PEBLMOD_LCTRL = 0x0040,
        PEBLMOD_RCTRL = 0x0080,
        PEBLMOD_LALT  = 0x0100,
        PEBLMOD_RALT  = 0x0200,
        PEBLMOD_LMETA = 0x0400,
        PEBLMOD_RMETA = 0x0800,
        PEBLMOD_NUM   = 0x1000,
        PEBLMOD_CAPS  = 0x2000,
        PEBLMOD_MODE  = 0x4000,
        PEBLMOD_RESERVED = 0x8000
    };

#define PEBLMOD_CTRL	(PEBLMOD_LCTRL|PEBLMOD_RCTRL)
#define PEBLMOD_SHIFT	(PEBLMOD_LSHIFT|PEBLMOD_RSHIFT)
#define PEBLMOD_ALT    (PEBLMOD_LALT|PEBLMOD_RALT)
#define PEBLMOD_META	(PEBLMOD_LMETA|PEBLMOD_RMETA)



//This is a direct translation of SDL_SCANCODE,
//with ANYKEY included.



enum PEBL_Keycode
{
    PEBL_KEYCODE_UNKNOWN = SDLK_UNKNOWN ,
    PEBL_KEYCODE_RETURN =    SDLK_RETURN ,
    PEBL_KEYCODE_ESCAPE =    SDLK_ESCAPE ,
    PEBL_KEYCODE_BACKSPACE =    SDLK_BACKSPACE ,
    PEBL_KEYCODE_TAB =    SDLK_TAB ,
    PEBL_KEYCODE_SPACE =    SDLK_SPACE ,
    PEBL_KEYCODE_EXCLAIM =    SDLK_EXCLAIM ,
    PEBL_KEYCODE_QUOTEDBL =    SDLK_QUOTEDBL ,
    PEBL_KEYCODE_HASH =    SDLK_HASH ,
    PEBL_KEYCODE_PERCENT =    SDLK_PERCENT ,
    PEBL_KEYCODE_DOLLAR =    SDLK_DOLLAR ,
    PEBL_KEYCODE_AMPERSAND =    SDLK_AMPERSAND ,
    PEBL_KEYCODE_QUOTE =    SDLK_QUOTE ,
    PEBL_KEYCODE_LEFTPAREN =    SDLK_LEFTPAREN ,
    PEBL_KEYCODE_RIGHTPAREN =    SDLK_RIGHTPAREN ,
    PEBL_KEYCODE_ASTERISK =    SDLK_ASTERISK ,
    PEBL_KEYCODE_PLUS =    SDLK_PLUS ,
    PEBL_KEYCODE_COMMA =    SDLK_COMMA ,
    PEBL_KEYCODE_MINUS =    SDLK_MINUS ,
    PEBL_KEYCODE_PERIOD =    SDLK_PERIOD ,
    PEBL_KEYCODE_SLASH =    SDLK_SLASH ,
    PEBL_KEYCODE_0 =    SDLK_0 ,
    PEBL_KEYCODE_1 =    SDLK_1 ,
    PEBL_KEYCODE_2 =    SDLK_2 ,
    PEBL_KEYCODE_3 =    SDLK_3 ,
    PEBL_KEYCODE_4 =    SDLK_4 ,
    PEBL_KEYCODE_5 =    SDLK_5 ,
    PEBL_KEYCODE_6 =    SDLK_6 ,
    PEBL_KEYCODE_7 =    SDLK_7 ,
    PEBL_KEYCODE_8 =    SDLK_8 ,
    PEBL_KEYCODE_9 =    SDLK_9 ,
    PEBL_KEYCODE_COLON =    SDLK_COLON ,
    PEBL_KEYCODE_SEMICOLON =    SDLK_SEMICOLON ,
    PEBL_KEYCODE_LESS =    SDLK_LESS ,
    PEBL_KEYCODE_EQUALS =    SDLK_EQUALS ,
    PEBL_KEYCODE_GREATER =    SDLK_GREATER ,
    PEBL_KEYCODE_QUESTION =    SDLK_QUESTION ,
    PEBL_KEYCODE_AT =    SDLK_AT ,

    //       Skip uppercase letters=       Skip uppercase letters,

    PEBL_KEYCODE_LEFTBRACKET =    SDLK_LEFTBRACKET ,
    PEBL_KEYCODE_BACKSLASH =    SDLK_BACKSLASH ,
    PEBL_KEYCODE_RIGHTBRACKET =    SDLK_RIGHTBRACKET ,
    PEBL_KEYCODE_CARET =    SDLK_CARET ,
    PEBL_KEYCODE_UNDERSCORE =    SDLK_UNDERSCORE ,
    PEBL_KEYCODE_BACKQUOTE =    SDLK_BACKQUOTE ,
    PEBL_KEYCODE_a =    SDLK_a ,
    PEBL_KEYCODE_b =    SDLK_b ,
    PEBL_KEYCODE_c =    SDLK_c ,
    PEBL_KEYCODE_d =    SDLK_d ,
    PEBL_KEYCODE_e =    SDLK_e ,
    PEBL_KEYCODE_f =    SDLK_f ,
    PEBL_KEYCODE_g =    SDLK_g ,
    PEBL_KEYCODE_h =    SDLK_h ,
    PEBL_KEYCODE_i =    SDLK_i ,
    PEBL_KEYCODE_j =    SDLK_j ,
    PEBL_KEYCODE_k =    SDLK_k ,
    PEBL_KEYCODE_l =    SDLK_l ,
    PEBL_KEYCODE_m =    SDLK_m ,
    PEBL_KEYCODE_n =    SDLK_n ,
    PEBL_KEYCODE_o =    SDLK_o ,
    PEBL_KEYCODE_p =    SDLK_p ,
    PEBL_KEYCODE_q =    SDLK_q ,
    PEBL_KEYCODE_r =    SDLK_r ,
    PEBL_KEYCODE_s =    SDLK_s ,
    PEBL_KEYCODE_t =    SDLK_t ,
    PEBL_KEYCODE_u =    SDLK_u ,
    PEBL_KEYCODE_v =    SDLK_v ,
    PEBL_KEYCODE_w =    SDLK_w ,
    PEBL_KEYCODE_x =    SDLK_x ,
    PEBL_KEYCODE_y =    SDLK_y ,
    PEBL_KEYCODE_z =    SDLK_z ,
    PEBL_KEYCODE_CAPSLOCK =    SDLK_CAPSLOCK ,
    PEBL_KEYCODE_F1 =    SDLK_F1 ,
    PEBL_KEYCODE_F2 =    SDLK_F2 ,
    PEBL_KEYCODE_F3 =    SDLK_F3 ,
    PEBL_KEYCODE_F4 =    SDLK_F4 ,
    PEBL_KEYCODE_F5 =    SDLK_F5 ,
    PEBL_KEYCODE_F6 =    SDLK_F6 ,
    PEBL_KEYCODE_F7 =    SDLK_F7 ,
    PEBL_KEYCODE_F8 =    SDLK_F8 ,
    PEBL_KEYCODE_F9 =    SDLK_F9 ,
    PEBL_KEYCODE_F10 =    SDLK_F10 ,
    PEBL_KEYCODE_F11 =    SDLK_F11 ,
    PEBL_KEYCODE_F12 =    SDLK_F12 ,
    PEBL_KEYCODE_PRINTSCREEN =    SDLK_PRINTSCREEN ,
    PEBL_KEYCODE_SCROLLLOCK =    SDLK_SCROLLLOCK ,
    PEBL_KEYCODE_PAUSE =    SDLK_PAUSE ,
    PEBL_KEYCODE_INSERT =    SDLK_INSERT ,
    PEBL_KEYCODE_HOME =    SDLK_HOME ,
    PEBL_KEYCODE_PAGEUP =    SDLK_PAGEUP ,
    PEBL_KEYCODE_DELETE =    SDLK_DELETE ,
    PEBL_KEYCODE_END =    SDLK_END ,
    PEBL_KEYCODE_PAGEDOWN =    SDLK_PAGEDOWN ,
    PEBL_KEYCODE_RIGHT =    SDLK_RIGHT ,
    PEBL_KEYCODE_LEFT =    SDLK_LEFT ,
    PEBL_KEYCODE_DOWN =    SDLK_DOWN ,
    PEBL_KEYCODE_UP =    SDLK_UP ,
    PEBL_KEYCODE_NUMLOCKCLEAR =    SDLK_NUMLOCKCLEAR ,
    PEBL_KEYCODE_KP_DIVIDE =    SDLK_KP_DIVIDE ,
    PEBL_KEYCODE_KP_MULTIPLY =    SDLK_KP_MULTIPLY ,
    PEBL_KEYCODE_KP_MINUS =    SDLK_KP_MINUS ,
    PEBL_KEYCODE_KP_PLUS =    SDLK_KP_PLUS ,
    PEBL_KEYCODE_KP_ENTER =    SDLK_KP_ENTER ,
    PEBL_KEYCODE_KP_1 =    SDLK_KP_1 ,
    PEBL_KEYCODE_KP_2 =    SDLK_KP_2 ,
    PEBL_KEYCODE_KP_3 =    SDLK_KP_3 ,
    PEBL_KEYCODE_KP_4 =    SDLK_KP_4 ,
    PEBL_KEYCODE_KP_5 =    SDLK_KP_5 ,
    PEBL_KEYCODE_KP_6 =    SDLK_KP_6 ,
    PEBL_KEYCODE_KP_7 =    SDLK_KP_7 ,
    PEBL_KEYCODE_KP_8 =    SDLK_KP_8 ,
    PEBL_KEYCODE_KP_9 =    SDLK_KP_9 ,
    PEBL_KEYCODE_KP_0 =    SDLK_KP_0 ,
    PEBL_KEYCODE_KP_PERIOD =    SDLK_KP_PERIOD ,
    PEBL_KEYCODE_APPLICATION =    SDLK_APPLICATION ,
    PEBL_KEYCODE_POWER =    SDLK_POWER ,
    PEBL_KEYCODE_KP_EQUALS =    SDLK_KP_EQUALS ,
    PEBL_KEYCODE_F13 =    SDLK_F13 ,
    PEBL_KEYCODE_F14 =    SDLK_F14 ,
    PEBL_KEYCODE_F15 =    SDLK_F15 ,
    PEBL_KEYCODE_F16 =    SDLK_F16 ,
    PEBL_KEYCODE_F17 =    SDLK_F17 ,
    PEBL_KEYCODE_F18 =    SDLK_F18 ,
    PEBL_KEYCODE_F19 =    SDLK_F19 ,
    PEBL_KEYCODE_F20 =    SDLK_F20 ,
    PEBL_KEYCODE_F21 =    SDLK_F21 ,
    PEBL_KEYCODE_F22 =    SDLK_F22 ,
    PEBL_KEYCODE_F23 =    SDLK_F23 ,
    PEBL_KEYCODE_F24 =    SDLK_F24 ,
    PEBL_KEYCODE_EXECUTE =    SDLK_EXECUTE ,
    PEBL_KEYCODE_HELP =    SDLK_HELP ,
    PEBL_KEYCODE_MENU =    SDLK_MENU ,
    PEBL_KEYCODE_SELECT =    SDLK_SELECT ,
    PEBL_KEYCODE_STOP =    SDLK_STOP ,
    PEBL_KEYCODE_AGAIN =    SDLK_AGAIN ,
    PEBL_KEYCODE_UNDO =    SDLK_UNDO ,
    PEBL_KEYCODE_CUT =    SDLK_CUT ,
    PEBL_KEYCODE_COPY =    SDLK_COPY ,
    PEBL_KEYCODE_PASTE =    SDLK_PASTE ,
    PEBL_KEYCODE_FIND =    SDLK_FIND ,
    PEBL_KEYCODE_MUTE =    SDLK_MUTE ,
    PEBL_KEYCODE_VOLUMEUP =    SDLK_VOLUMEUP ,
    PEBL_KEYCODE_VOLUMEDOWN =    SDLK_VOLUMEDOWN ,
    PEBL_KEYCODE_KP_COMMA =    SDLK_KP_COMMA ,
    PEBL_KEYCODE_KP_EQUALSAS400 =    SDLK_KP_EQUALSAS400 ,
    PEBL_KEYCODE_ALTERASE =    SDLK_ALTERASE ,
    PEBL_KEYCODE_SYSREQ =    SDLK_SYSREQ ,
    PEBL_KEYCODE_CANCEL =    SDLK_CANCEL ,
    PEBL_KEYCODE_CLEAR =    SDLK_CLEAR ,
    PEBL_KEYCODE_PRIOR =    SDLK_PRIOR ,
    PEBL_KEYCODE_RETURN2 =    SDLK_RETURN2 ,
    PEBL_KEYCODE_SEPARATOR =    SDLK_SEPARATOR ,
    PEBL_KEYCODE_OUT =    SDLK_OUT ,
    PEBL_KEYCODE_OPER =    SDLK_OPER ,
    PEBL_KEYCODE_CLEARAGAIN =    SDLK_CLEARAGAIN ,
    PEBL_KEYCODE_CRSEL =    SDLK_CRSEL ,
    PEBL_KEYCODE_EXSEL =    SDLK_EXSEL ,
    PEBL_KEYCODE_KP_00 =    SDLK_KP_00 ,
    PEBL_KEYCODE_KP_000 =    SDLK_KP_000 ,
    PEBL_KEYCODE_THOUSANDSSEPARATOR =    SDLK_THOUSANDSSEPARATOR ,
    PEBL_KEYCODE_DECIMALSEPARATOR =    SDLK_DECIMALSEPARATOR ,
    PEBL_KEYCODE_CURRENCYUNIT =    SDLK_CURRENCYUNIT ,
    PEBL_KEYCODE_CURRENCYSUBUNIT =    SDLK_CURRENCYSUBUNIT ,
    PEBL_KEYCODE_KP_LEFTPAREN =    SDLK_KP_LEFTPAREN ,
    PEBL_KEYCODE_KP_RIGHTPAREN =    SDLK_KP_RIGHTPAREN ,
    PEBL_KEYCODE_KP_LEFTBRACE =    SDLK_KP_LEFTBRACE ,
    PEBL_KEYCODE_KP_RIGHTBRACE =    SDLK_KP_RIGHTBRACE ,
    PEBL_KEYCODE_KP_TAB =    SDLK_KP_TAB ,
    PEBL_KEYCODE_KP_BACKSPACE =    SDLK_KP_BACKSPACE ,
    PEBL_KEYCODE_KP_A =    SDLK_KP_A ,
    PEBL_KEYCODE_KP_B =    SDLK_KP_B ,
    PEBL_KEYCODE_KP_C =    SDLK_KP_C ,
    PEBL_KEYCODE_KP_D =    SDLK_KP_D ,
    PEBL_KEYCODE_KP_E =    SDLK_KP_E ,
    PEBL_KEYCODE_KP_F =    SDLK_KP_F ,
    PEBL_KEYCODE_KP_XOR =    SDLK_KP_XOR ,
    PEBL_KEYCODE_KP_POWER =    SDLK_KP_POWER ,
    PEBL_KEYCODE_KP_PERCENT =    SDLK_KP_PERCENT ,
    PEBL_KEYCODE_KP_LESS =    SDLK_KP_LESS ,
    PEBL_KEYCODE_KP_GREATER =    SDLK_KP_GREATER ,
    PEBL_KEYCODE_KP_AMPERSAND =    SDLK_KP_AMPERSAND ,
    PEBL_KEYCODE_KP_DBLAMPERSAND =    SDLK_KP_DBLAMPERSAND ,
    PEBL_KEYCODE_KP_VERTICALBAR =    SDLK_KP_VERTICALBAR ,
    PEBL_KEYCODE_KP_DBLVERTICALBAR =    SDLK_KP_DBLVERTICALBAR ,
    PEBL_KEYCODE_KP_COLON =    SDLK_KP_COLON ,
    PEBL_KEYCODE_KP_HASH =    SDLK_KP_HASH ,
    PEBL_KEYCODE_KP_SPACE =    SDLK_KP_SPACE ,
    PEBL_KEYCODE_KP_AT =    SDLK_KP_AT ,
    PEBL_KEYCODE_KP_EXCLAM =    SDLK_KP_EXCLAM ,
    PEBL_KEYCODE_KP_MEMSTORE =    SDLK_KP_MEMSTORE ,
    PEBL_KEYCODE_KP_MEMRECALL =    SDLK_KP_MEMRECALL ,
    PEBL_KEYCODE_KP_MEMCLEAR =    SDLK_KP_MEMCLEAR ,
    PEBL_KEYCODE_KP_MEMADD =    SDLK_KP_MEMADD ,
    PEBL_KEYCODE_KP_MEMSUBTRACT =    SDLK_KP_MEMSUBTRACT ,
    PEBL_KEYCODE_KP_MEMMULTIPLY =    SDLK_KP_MEMMULTIPLY ,
    PEBL_KEYCODE_KP_MEMDIVIDE =    SDLK_KP_MEMDIVIDE ,
    PEBL_KEYCODE_KP_PLUSMINUS =    SDLK_KP_PLUSMINUS ,
    PEBL_KEYCODE_KP_CLEAR =    SDLK_KP_CLEAR ,
    PEBL_KEYCODE_KP_CLEARENTRY =    SDLK_KP_CLEARENTRY ,
    PEBL_KEYCODE_KP_BINARY =    SDLK_KP_BINARY ,
    PEBL_KEYCODE_KP_OCTAL =    SDLK_KP_OCTAL ,
    PEBL_KEYCODE_KP_DECIMAL =    SDLK_KP_DECIMAL ,
    PEBL_KEYCODE_KP_HEXADECIMAL =    SDLK_KP_HEXADECIMAL ,
    PEBL_KEYCODE_LCTRL =    SDLK_LCTRL ,
    PEBL_KEYCODE_LSHIFT =    SDLK_LSHIFT ,
    PEBL_KEYCODE_LALT =    SDLK_LALT ,
    PEBL_KEYCODE_LGUI =    SDLK_LGUI ,
    PEBL_KEYCODE_RCTRL =    SDLK_RCTRL ,
    PEBL_KEYCODE_RSHIFT =    SDLK_RSHIFT ,
    PEBL_KEYCODE_RALT =    SDLK_RALT ,
    PEBL_KEYCODE_RGUI =    SDLK_RGUI ,
    PEBL_KEYCODE_MODE =    SDLK_MODE ,
    PEBL_KEYCODE_AUDIONEXT =    SDLK_AUDIONEXT ,
    PEBL_KEYCODE_AUDIOPREV =    SDLK_AUDIOPREV ,
    PEBL_KEYCODE_AUDIOSTOP =    SDLK_AUDIOSTOP ,
    PEBL_KEYCODE_AUDIOPLAY =    SDLK_AUDIOPLAY ,
    PEBL_KEYCODE_AUDIOMUTE =    SDLK_AUDIOMUTE ,
    PEBL_KEYCODE_MEDIASELECT =    SDLK_MEDIASELECT ,
    PEBL_KEYCODE_WWW =    SDLK_WWW ,
    PEBL_KEYCODE_MAIL =    SDLK_MAIL ,
    PEBL_KEYCODE_CALCULATOR =    SDLK_CALCULATOR ,
    PEBL_KEYCODE_COMPUTER =    SDLK_COMPUTER ,
    PEBL_KEYCODE_AC_SEARCH =    SDLK_AC_SEARCH ,
    PEBL_KEYCODE_AC_HOME =    SDLK_AC_HOME ,
    PEBL_KEYCODE_AC_BACK =    SDLK_AC_BACK ,
    PEBL_KEYCODE_AC_FORWARD =    SDLK_AC_FORWARD ,
    PEBL_KEYCODE_AC_STOP =    SDLK_AC_STOP ,
    PEBL_KEYCODE_AC_REFRESH =    SDLK_AC_REFRESH ,
    PEBL_KEYCODE_AC_BOOKMARKS =    SDLK_AC_BOOKMARKS ,
    PEBL_KEYCODE_BRIGHTNESSDOWN =    SDLK_BRIGHTNESSDOWN ,
    PEBL_KEYCODE_BRIGHTNESSUP =    SDLK_BRIGHTNESSUP ,
    PEBL_KEYCODE_DISPLAYSWITCH =    SDLK_DISPLAYSWITCH ,
    PEBL_KEYCODE_KBDILLUMTOGGLE =    SDLK_KBDILLUMTOGGLE ,
    PEBL_KEYCODE_KBDILLUMDOWN =    SDLK_KBDILLUMDOWN ,
    PEBL_KEYCODE_KBDILLUMUP =    SDLK_KBDILLUMUP ,
    PEBL_KEYCODE_EJECT =    SDLK_EJECT ,
    PEBL_KEYCODE_SLEEP =    SDLK_SLEEP,


    /* Add any other keys here. */
    PEBL_KEYCODE_ANYKEY = 1000,
    PEBL_KEYCODE_NUMLOCK = 1001,
    PEBL_KEYCODE_NOTHING = 0,    /*reuse 0 for none and nothing*/
    
} ;



///
///  This is the base Keyboard class.  It is 'virtual' and must
///  be overridden by a PlatformKeyboard class, but contains keycodes
///

class PKeyboard: virtual public PDevice
{
    
public:    


    ///The Standard constructor.  
    PKeyboard();
    
    ///The Standard destructor.  
    virtual ~PKeyboard();


    ///Translate a letter string to the appropriate keyboard symbol
    virtual PEBL_Keycode TranslateString(std::string letter) const;

    virtual std::string TranslateKeyCode(const PEBL_Keycode code)const;

    ///This will test whether a specific key is down,
    ///and return true if it is.
    virtual PEBL_Keycode IsKeyDown(PEBL_Keycode code) const = 0;

    ///This will test whether a specific key is up, and
    ///return true if it is.
    virtual bool IsKeyUp(PEBL_Keycode code) const = 0;
    virtual int GetState(int iface)const; 
    //    virtual int TestDevice(const DeviceState & state) const = 0;

    virtual PEBL_DEVICE_TYPE GetDeviceType(){return PDT_KEYBOARD;};
    virtual int GetModKeys() const;  //This gets an or-ed together representation
    //                         //of which mod keys are pressed.

protected:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const; 

private:
};





#endif
