//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLEnvironment.h
//    Purpose:    General Environment Function Library for PEBL
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
#ifndef __PEBLENVIRONMENT_H__
#define __PEBLENVIRONMENT_H__

class Variant;



namespace PEBLEnvironment
{


    //Simple time access
    Variant GetTime(Variant v);
    Variant GetTimeHP(Variant v);
    Variant Wait(Variant v);
    Variant GetTimeOfDay(Variant v);
    Variant GetObjectTime(Variant v);
    //Simple key state querying
    Variant IsKeyDown(Variant v);
    Variant IsKeyUp(Variant v);
    Variant IsAnyKeyDown(Variant v);

    Variant ShowCursor(Variant v);
    Variant GetCursorPosition(Variant v);
    Variant SetCursorPosition(Variant v);


    //Simple eventloop construction
    Variant WaitForKeyDown(Variant v);
    Variant WaitForKeyUp(Variant v);
    Variant WaitForAnyKeyDown(Variant v);

    Variant WaitForKeyPress(Variant v);
    Variant WaitForKeyRelease(Variant v);
    Variant WaitForAnyKeyPress(Variant v);
    Variant WaitForListKeyPress(Variant v);

    Variant WaitForAllKeysUp(Variant v);
    Variant WaitForAnyKeyDownWithTimeout(Variant v);
    Variant WaitForAnyKeyPressWithTimeout(Variant v);

    Variant WaitForListKeyPressWithTimeout(Variant v);
    Variant WaitForKeyListDown(Variant v);
    Variant WaitForMouseButton(Variant v);
    Variant WaitForMouseButtonWithTimeout(Variant v);

    Variant GetMouseState(Variant v);

    //basic joystick stuff
    Variant GetNumJoysticks(Variant v);
    Variant GetJoystick(Variant v);
    Variant GetNumJoystickAxes(Variant v);
    Variant GetNumJoystickBalls(Variant v);
    Variant GetNumJoystickButtons(Variant v);
    Variant GetNumJoystickHats(Variant v);


    Variant GetJoystickButtonState(Variant v);
    Variant GetJoystickAxisState(Variant v);
    Variant GetJoystickHatState(Variant v);
    Variant GetJoystickBallState(Variant v);





    //Complex eventloop construction.
    Variant RegisterEvent( Variant v);
    Variant StartEventLoop(Variant v);
    Variant ClearEventLoop(Variant v);
    Variant CallFunction(Variant v);
    //Misc
    Variant GetInput0(Variant v);
    Variant GetInputOld1(Variant v);
    Variant GetInputOld2(Variant v);


    Variant GetInput(Variant v);
    Variant GetTextBoxCursorFromClick(Variant v);


    Variant SignalFatalError(Variant v);
    Variant ExitQuietly(Variant v);
    Variant TranslateKeyCode(Variant v);
    Variant TranslateString(Variant v);
    Variant TimeStamp(Variant v);
    Variant GetCurrentScreenResolution(Variant v);
    Variant GetVideoModes(Variant v);
    Variant GetDrivers(Variant v);
    Variant GetPEBLVersion(Variant v);
    Variant GetSystemType(Variant v);
    Variant GetExecutableName(Variant v);


    Variant LaunchFile(Variant v);
    Variant SystemCall(Variant v);
//#ifdef PEBL_WINDOWS
    Variant SystemCallUpdate(Variant v);
//#endif
    Variant CheckProcessStatus(Variant v);


    Variant IsDirectory(Variant v);
    Variant GetDirectoryListing(Variant v);
    Variant FileExists(Variant v);
    Variant MakeDirectory(Variant v);
    Variant DeleteFile(Variant v);
    Variant GetHomeDirectory(Variant v);
    Variant GetWorkingDirectory(Variant v);
    Variant SetWorkingDirectory(Variant v);

    Variant CopyToClipboard(Variant v);
    Variant CopyFromClipboard(Variant v);


    Variant VariableExists(Variant v);
    Variant IsText(Variant v);
    Variant IsNumber(Variant v);
    Variant IsInteger(Variant v);
    Variant IsFloat(Variant v);
    Variant IsString(Variant v);
    Variant IsList(Variant v);
    Variant IsTextBox(Variant v);
    Variant IsJoystick(Variant v);
    Variant IsCanvas(Variant v);
    Variant IsImage(Variant v);
    Variant IsLabel(Variant v);
    Variant IsAudioOut(Variant v);
    Variant IsFont(Variant v);
    Variant IsColor(Variant v);
    Variant IsFileStream(Variant v);
    Variant IsWidget(Variant v);
    Variant IsWindow(Variant v);
    Variant IsShape(Variant v);
    Variant IsCircle(Variant v);
    Variant IsSquare(Variant v);
    Variant IsRectangle(Variant v);
    Variant IsCustomObject(Variant v);
    Variant PlayMovie(Variant v) ;

}


#endif
