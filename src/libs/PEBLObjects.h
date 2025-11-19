//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLObjects.h
//    Purpose:    Function Library for managing PEBL Objects
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
#ifndef __PEBLOBJECTS_H__
#define __PEBLOBJECTS_H__

#include "../platforms/sdl/PlatformEnvironment.h"
#include "../apps/Globals.h"

class Variant;


namespace PEBLObjects
{

    void MakeEnvironment(PEBLVideoMode mode, PEBLVideoDepth depth, 
                         bool windowed,bool resizeable,bool unicode );
    Variant MakeWindow(Variant v);
    Variant ResizeWindow(Variant v);
    Variant MakeImage(Variant  v);
    Variant MakeLabel(Variant  v);
    Variant MakeColor(Variant v);
    Variant MakeColorRGB(Variant v);
    Variant MakeFont(Variant v);
    Variant MakeTextBox(Variant v);
    Variant MakeCanvas(Variant v);

    Variant AddObject(Variant v);
    Variant RemoveObject(Variant v);


    //These control TextBoxes
    Variant SetCursorPosition(Variant v);
    Variant GetCursorPosition(Variant v);
    Variant GetParent(Variant v);
    Variant SetEditable(Variant v);
    Variant SetText(Variant v);
    Variant GetText(Variant v);


    Variant GetLineBreaks(Variant v);
    
    Variant SetPoint(Variant v);
    Variant GetPixelColor(Variant v);

    Variant SetFont(Variant v);

    Variant PrintProperties(Variant v);
    Variant GetPropertyList(Variant v);
    Variant SetProperty(Variant v);
    Variant GetProperty(Variant v);
    Variant PropertyExists (Variant v);

    Variant Show(Variant v);
    Variant Hide(Variant v);

    Variant Draw(Variant v);
    Variant Move(Variant v);
    Variant GetSize(Variant v);

    Variant DrawFor(Variant v);

    //Functions to create, load and play sounds.
    Variant LoadSound(Variant v);
    Variant PlayBackground(Variant v);
    Variant PlayForeground(Variant v);
    Variant Stop(Variant v);
    Variant SetPanning(Variant v);
    Variant SetPlayRepeats(Variant v);
    Variant MakeSineWave(Variant v);
    Variant MakeSquareWave(Variant v);
    Variant MakeSawtoothWave(Variant v);
    Variant MakeChirp(Variant v);
    Variant MakeAudioInputBuffer(Variant v);
    Variant SaveAudioToWaveFile(Variant v);
    Variant GetVocalResponseTime(Variant v);
    Variant RecordToBuffer(Variant v);

    //Functions to draw things.
    Variant Line(Variant v);
    Variant ThickLine(Variant v);
    Variant Rectangle(Variant v);
    Variant Square(Variant v);
    Variant Ellipse(Variant v);
    Variant Circle(Variant v);
    Variant Polygon(Variant v);
    Variant Bezier(Variant v);

    Variant RotoZoom(Variant v);

    Variant LoadMovie(Variant v);
    Variant LoadAudioFile(Variant v);

    Variant StartPlayback(Variant v);
    Variant PausePlayback(Variant v);


    Variant MakeCustomObject(Variant v);


    Variant ConnectEyeTracker(Variant v);
    Variant GetEyeObject(Variant v);
    
    Variant SetEyeTrackerHandler(Variant v);
}


#endif
