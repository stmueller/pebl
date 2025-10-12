//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLObjects.cpp
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


#include "PEBLObjects.h"
#include "PEBLEnvironment.h"
#include "../base/Variant.h"
#include "../base/PList.h"
#include "../base/PComplexData.h"

#ifdef PEBL_EMSCRIPTEN
#include "../base/Evaluator-es.h"
#include "../devices/PEventLoop-es.h"
#include <emscripten.h>
#else
#include "../base/Evaluator.h"
#include "../devices/PEventLoop.h"
#endif




#include "../base/PEBLObject.h"
#include "../devices/PEyeTracker.h"

#include "../devices/DeviceState.h"



#include "../objects/PColor.h"
#include "../objects/PTextBox.h"
#include "../objects/PDrawObject.h"
#include "../objects/PCustomObject.h"
#include "../utility/PError.h"
#include "../utility/PEBLUtility.h"
#include "../utility/Defs.h"

#include "../platforms/sdl/PlatformEnvironment.h"
#include "../platforms/sdl/PlatformWindow.h"
#include "../platforms/sdl/PlatformImageBox.h"
#include "../platforms/sdl/PlatformDrawObject.h"
#include "../platforms/sdl/PlatformLabel.h"
#include "../platforms/sdl/PlatformFont.h"
#include "../platforms/sdl/PlatformEventQueue.h"
#include "../platforms/sdl/PlatformTextBox.h"
#include "../platforms/sdl/PlatformCanvas.h"
#include "../platforms/sdl/PlatformAudioIn.h"

#ifdef PEBL_MOVIES
#include "../platforms/sdl/PlatformMovie.h"
#endif



#include "PEBLList.h"

#include <string>
using std::string;
using std::cout;
using std::endl;

namespace PEBLObjects
{
}

//Initiate static data
extern PlatformEnvironment * myEnv;
PlatformEventQueue * gEventQueue = NULL;


/// This function instantiates the namespace-viewable
//  PEBLObjects::myEnv, creating a new Environment for displaying
/// stimuli. It is not used directly by functions, but by
/// the program itself.

void PEBLObjects::MakeEnvironment(PEBLVideoMode mode, PEBLVideoDepth depth,
                                  bool windowed,bool resizeable,bool unicode)
{

    myEnv = new PlatformEnvironment(mode, depth, windowed,resizeable,unicode);
    myEnv->Initialize();
    //Initialize the event queue.
    gEventQueue = new PlatformEventQueue();

}



/// This function makes a new root-level window and return
/// a variant to access it.
Variant PEBLObjects::MakeWindow(Variant v)
{

    //Get default window size:
    Variant width = myEval->gGlobalVariableMap.RetrieveValue("gVideoWidth");
    Variant height= myEval->gGlobalVariableMap.RetrieveValue("gVideoHeight");



    //If there is no argument, make the black grey.
    //If there is an argument, it should be the name of a color
    Variant color;
    PColor * pcolor = NULL;
    PList * plist = NULL;

    if(v.IsStackSignal())
        {
            pcolor = new PColor("black");
        }
    else
        {

            plist = v.GetComplexData()->GetList();
            color = plist->First();

            if(color.IsComplexData())
                {
                    if((color.GetComplexData())->IsColor())
                        {
                            pcolor = new PColor(*((PColor*)(color.GetComplexData()->GetObject().get())));

                        }
                    else
                        {

                           PError::SignalFatalError("Wanted color or colorname as first argument of MakeWindow()");
                        }

                } else {

                PError::AssertType(color, PEAT_STRING,"Argument error in [MakeWindow(<color>)]: ");
                    pcolor = new PColor(color.GetString());
                }
               //now, get the width/height if they are there.

            if(plist->Length()>=2)
                {
                    width = plist->Nth(2);
                }

            if(plist->Length()>=3)
                {
                    height = plist->Nth(3);
                }
        }




    //Create a new window, and reference it in a counted pointer.
    PlatformWindow *  myWindow = new  PlatformWindow(myEnv);
    myWindow->SetBackgroundColor(*pcolor);
    PEBLVideoMode mode = myEnv->GetVideoMode();
    PEBLVideoDepth depth = myEnv->GetVideoDepth();
    bool windowed = myEnv->GetWindowed();
    bool resizeable = myEnv->GetResizeable();

    myWindow->Initialize(mode, depth, windowed,resizeable,
                         (unsigned int)(int)(width),
                         (unsigned int)(int)(height));

    //Add the window to the environment
    myEnv->AddWindow(myWindow);

    counted_ptr<PEBLObjectBase> tmp2   =  counted_ptr<PEBLObjectBase> (myWindow);

    PComplexData *  pcd = new PComplexData(tmp2);

    Variant tmp = Variant(pcd);

#ifdef PEBL_EMSCRIPTEN
    // Debug: Use EM_ASM to ensure output isn't optimized away
    EM_ASM({
        console.log("=== C++ MakeWindow() CALLED ===");
    });

    std::string tmpStr = tmp.GetString();
    EM_ASM({
        console.log("C++ MakeWindow: Variant as string: [" + UTF8ToString($0) + "]");
    }, tmpStr.c_str());

    EM_ASM({
        console.log("C++ MakeWindow: tmp.IsComplexData() = " + $0);
    }, tmp.IsComplexData());
#endif

    delete pcd;
    delete pcolor;
    pcd=NULL;
    //cout << "Deleting PCD Window in PEBLOBjects::MakeWindow\n";
    return tmp;
}



Variant PEBLObjects::ResizeWindow(Variant v)
{

       PList * plist = v.GetComplexData()->GetList();

       Variant v2 = plist->First(); //plist->PopFront();
       PError::AssertType(v2, PEAT_WINDOW, "Argument error in first argument of function [ResizeWindow(<window>,<w>,<h>)]: ");
       PlatformWindow * myWindow = dynamic_cast<PlatformWindow*>(v2.GetComplexData()->GetObject().get());

       Variant w = plist->Nth(2);

       Variant h = plist->Nth(3);
       PError::AssertType(w, PEAT_NUMBER, "Argument error in second argument of function [ResizeWindow(<window>,<w>,<h>)]: ");
       PError::AssertType(h, PEAT_NUMBER, "Argument error in third argument of function [ResizeWindow(<window>,<w>,<h>)]: ");


       myWindow->Resize(w,h);
       return Variant(1);
}



Variant PEBLObjects::MakeImage(Variant v)
{
    //v[1] should be have a filename in it.
    PList * plist = v.GetComplexData()->GetList();
    Variant name = plist->First();
    PError::AssertType(name, PEAT_STRING, "Argument error in function [MakeImage(<filename>)]: ");
    PlatformImageBox * myImageBox = new PlatformImageBox();

    myImageBox->LoadImage(name);


    counted_ptr<PEBLObjectBase>tmp2 = counted_ptr<PEBLObjectBase>(myImageBox);
    PComplexData *  pcd = new PComplexData(tmp2);
    Variant tmp = Variant(pcd);

    delete pcd;
    pcd=NULL;
    return tmp;
}



Variant PEBLObjects::MakeLabel(Variant  v)
{
    //v[1] should have text in it,
    //v[2] should have a font in it.

    PList * plist = v.GetComplexData()->GetList();


    //Get the text
    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in first parameter of function [MakeLabel(<text>, <font>)]: ");
    std::string  text = (plist->First()); //plist->PopFront();

    //Get the font
    Variant v2 = plist->Nth(2);// plist->PopFront();
    PError::AssertType(v2, PEAT_FONT, "Argument error in second parameter of function [MakeLabel(<text>, <font>)]: ");

    counted_ptr<PEBLObjectBase> tmpFont = v2.GetComplexData()->GetObject();



    //Create the 'label'
    PlatformLabel * tmpLabel = new PlatformLabel(text,tmpFont);
    //Place it by default at 0,0

    tmpLabel->SetPosition(0,0);

    counted_ptr<PEBLObjectBase> myLabel = counted_ptr<PEBLObjectBase>(tmpLabel);


    //Return a variant containing the label.
    PComplexData *  pcd =     new PComplexData(myLabel);

    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;

}


Variant PEBLObjects::MakeTextBox(Variant v)
{
    //v[1] should have text in it,
    //v[2] should have a font in it.
    //v[3] should have a width in it (in pixels)
    //v[4] should have a height in it (in pixels)

    PList * plist = v.GetComplexData()->GetList();
    PError::AssertType(plist->First(), PEAT_STRING,"Argument error in first parameter of function [MakeTextBox(<text>,<font>,<width>,<height>)]: ");

    //Get the text
    string  text = (plist->First()).GetString(); //plist->PopFront();

    //Get the font
    Variant v2 = plist->Nth(2);// plist->PopFront();
    PError::AssertType(v2, PEAT_FONT, "Argument error in second parameter of function [MakeTextBox(<text>,<font>,<width>,<height>)]: ");

    counted_ptr<PEBLObjectBase> myFont = v2.GetComplexData()->GetObject();


    //Get the width
    PError::AssertType(plist->Nth(3), PEAT_NUMBER,"Argument error in third parameter of function [MakeTextBox(<text>,<font>,<width>,<height>)]: ");
    int width = plist->Nth(3);// plist->PopFront();

    //Get the height
    PError::AssertType(plist->Nth(4), PEAT_NUMBER,"Argument error in fourth parameter of function [MakeTextBox(<text>,<font>,<width>,<height>)]: ");

    int height = plist->Nth(4);// plist->PopFront();

   //Create the text box
    PlatformTextBox * tmptb = new PlatformTextBox(text, myFont,width,height);


    //Place it by default at 0,0
    tmptb->SetPosition(0,0);



    Variant lang = PEBLUtility::ToUpper(myEval->gGlobalVariableMap.RetrieveValue("gLanguage"));

    if((lang=="HE") | (lang == "FA") | (lang == "AR"))
        {
            tmptb->SetProperty("JUSTIFY","RIGHT");
        }
    counted_ptr<PEBLObjectBase> myTextBox = counted_ptr<PEBLObjectBase>(tmptb);

    //Return a variant containing the label.
    PComplexData *   pcd = new PComplexData(myTextBox);

    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;


}


Variant PEBLObjects::MakeCanvas(Variant v)
{

    //
    // v[1] should be X, v[2] shoud be Y
    // v[3] should be the color (optional),
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [MakeCanvas(<x>, <y>, <color>)]: ");
    int width = plist->First();// plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function [MakeCanvas(<x>, <y>, <color>)]: ");
    int height = plist->Nth(2);// plist->PopFront();

    PlatformCanvas * pc =NULL;
    if(plist->Length()>2)
        {
            PError::AssertType(plist->Nth(3), PEAT_COLOR, "Argument error in third parameter of function  [MakeCanvas(<x>, <y>, <color>)]: ");

            Variant color = plist->Nth(3);// plist->PopFront();
            pc = new PlatformCanvas(width,height,color);

        }else  {
        pc = new PlatformCanvas(width,height);
    }

    counted_ptr<PEBLObjectBase> myCanvas = counted_ptr<PEBLObjectBase>(pc);
    PComplexData *  pcd = new PComplexData(myCanvas);

    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
}



Variant PEBLObjects::MakeColor(Variant v)
{
    //v[1] should be a color name
    //Access for rgb and rgba colors should be provided.


    PList * plist = v.GetComplexData()->GetList();

    std::string name = plist->First();// plist->PopFront();

    counted_ptr<PEBLObjectBase> myColor = counted_ptr<PEBLObjectBase>(new PColor(name));
    PComplexData *  pcd = new PComplexData(myColor);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;

}

Variant PEBLObjects::MakeColorRGB(Variant v)
{
    //v[1] [2] [3] should be RGB values between 0 and 255
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_INTEGER, "Argument error in first parameter of function [MakeColorRGB(<red>, <green>, <blue>)]: ");
    int red  = plist->First();// plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_INTEGER, "Argument error in second parameter of function [MakeColorRGB(<red>, <green>, <blue>)]: ");
    int green  = plist->Nth(2);// plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_INTEGER, "Argument error in third parameter of function [MakeColorRGB(<red>, <green>, <blue>)]: ");
    int blue  = plist->Nth(3);// plist->PopFront();

    PColor * tmpColor = new PColor(red,green,blue, 255);
    counted_ptr<PEBLObjectBase> myColor = counted_ptr<PEBLObjectBase>(tmpColor);
    PComplexData *  pcd = new PComplexData(myColor);


    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;

}



Variant PEBLObjects::MakeFont(Variant v)
{
    //v[1-6] should be have font parameters in it.
    PList * plist = v.GetComplexData()->GetList();

    //First comes the filename of the font.
    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in first parameter of function [MakeFont(<filename>, <style>, <size>, <fg>, <bg>, <aa>)]: ");
    std::string name = plist->First(); //plist->PopFront();

    //Next comes the style, an integerized code for normal, bold, italic, underline.
    PError::AssertType(plist->Nth(2), PEAT_INTEGER, "Argument error in second parameter of function [MakeFont(<filename>, <style>, <size>, <fg>, <bg>, <aa>)]: ");
    int style = plist->Nth(2); //plist->PopFront();

    //Next is the point size.
    PError::AssertType(plist->Nth(3), PEAT_INTEGER, "Argument error in third parameter of function [MakeFont(<filename>, <style>, <size>, <fg>, <bg>, <aa>)]: ");
    int size = plist->Nth(3);// plist->PopFront();

    //The next parameter should be a PColor--foreground color
    Variant vfg = plist->Nth(4); //plist->PopFront();
    PError::AssertType(vfg, PEAT_COLOR, "Argument error in fourth parameter of function [MakeFont(<filename>, <style>, <size>, <fg>, <bg>, <aa>)]: ");
    PColor fgcolor = *((PColor*)(vfg.GetComplexData()->GetObject().get()));


    //The next parameter should be a PColor--background color
    Variant vbg = plist->Nth(5); //plist->PopFront();
    PError::AssertType(vbg, PEAT_COLOR, "Argument error in fifth parameter of function [MakeFont(<filename>, <style>, <size>, <fg>, <bg>, <aa>)]: ");

    PColor bgcolor = *((PColor*)(vbg.GetComplexData()->GetObject().get()));
    //Whether the font should be rendered with anti-aliasing against background.

    PError::AssertType(plist->Nth(6), PEAT_INTEGER, "Argument error in sixth parameter of function [MakeFont(<filename>, <style>, <size>, <fg>, <bg>, <aa>)]: ");
    bool aa = plist->Nth(6);// plist->PopFront();

    //Make the font and wrap it up in a Variant to return it.
    PlatformFont * tmpFont = new PlatformFont(name, style, size, fgcolor, bgcolor, aa);
    counted_ptr<PEBLObjectBase> myFont = counted_ptr<PEBLObjectBase>(tmpFont);

    PComplexData *  pcd = new PComplexData(myFont);
    Variant tmp = Variant(pcd);

    //delete pcd;  //without deleting here, fonts will leak memory
    pcd=NULL;    //But deleting causes a segfault I haven't figured out.
    //that appears to be due to an SDL issue.  Loading font file using RWOps
    //helps increase the number of fonts we can have, but there is
    // still a memory leak; fonts cannot get destroyed in SDL 1.2

    //delete pcd;  //without deleting here, fonts will leak memory
    //pcd=NULL;    //But deleting causes a segfault I haven't figured out.
    //NOTE: the crash happens when the font object is being destroyed,
    return tmp;

}



/// This function adds a widget to a parent widget.
Variant PEBLObjects::AddObject(Variant v)
{
    //v[1] should have the child widget
    //v[2] should have the parent widget

    PList * plist = v.GetComplexData()->GetList();


    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_WIDGET, "Argument error in first parameter of function [AddObject(<child-widget>, <parent-widget>)]: ");


    PlatformWidget * child = dynamic_cast<PlatformWidget*>((v1.GetComplexData())->GetObject().get());

    Variant v2 = plist->Nth(2); //plist->PopFront();
    PError::AssertType(v2, PEAT_WIDGET, "Argument error in second parameter of function [AddObject(<child-widget>, <parent-widget>)]: ");

    PlatformWidget * parent = dynamic_cast<PlatformWidget*>((v2.GetComplexData())->GetObject().get());


    parent->AddSubWidget(child);
    child->SetProperty("PARENT",v2);

    return Variant(true);
}



/// This removes a child widget from the parent widget.
Variant PEBLObjects::RemoveObject(Variant v)
{
    //v[1] should have the child widget
    //v[2] should have the parent widget
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_WIDGET, "Argument error in first parameter of function [RemoveObject(<child-widget>, <parent-widget>)]: ");


    PlatformWidget * child = dynamic_cast<PlatformWidget*>(v1.GetComplexData()->GetObject().get());

    Variant v2 = plist->Nth(2);//plist->PopFront();
    PError::AssertType(v2, PEAT_WIDGET, "Argument error in second parameter of function [RemoveObject(<child-widgett>, <parent-widget>)]: ");


    PlatformWidget * parent = dynamic_cast<PlatformWidget*>(v2.GetComplexData()->GetObject().get());

    //Removing subwidgets does not destroy the Variant, so we don't need to
    // worry about a memory leak here, even though we are using raw pointers.
    //however,we need to check whether the child is actually a member of the parent,
    //otherwise things could go bad.

    if(!child | !parent)
        {
            return Variant(false);
        }

    if(! child->IsParent(parent))
        {
            PError::SignalFatalError("Trying to remove object from another object that is not its parent\n");
        }


    parent->RemoveSubWidget(child);

    return Variant(true);
}


Variant PEBLObjects::SetPoint(Variant v)
{

    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_WIDGET, "Argument error in first parameter of function [SetPoint(<widget>,<x>, <y>, <color>)]: ");
    PlatformWidget * widget = dynamic_cast<PlatformWidget*>(v1.GetComplexData()->GetObject().get());


    int x = plist->Nth(2);// plist->PopFront();
    PError::AssertType(x, PEAT_NUMBER, "Argument error in second parameter of function [SetPoint(<widget>,<x>, <y>, <color>)]: ");

    int y = plist->Nth(3);// plist->PopFront();
    PError::AssertType(y, PEAT_NUMBER, "Argument error in third parameter of function [SetPoint(<widget>,<x>, <y>, <color>)]: ");

    Variant color = plist->Nth(4);// plist->PopFront();
    PError::AssertType(color, PEAT_COLOR, "Argument error in fourth parameter of function  [SetPoint(<widget>,<x>, <y>, <color>)]: ");


    PColor pcolor = *((PColor*)(color.GetComplexData()->GetObject().get()));
    widget->SetPoint(x,y,pcolor);

    return Variant(true);
}

Variant PEBLObjects::GetPixelColor(Variant v)
{
   PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_WIDGET, "Argument error in first parameter of function [GetPixelColor(<widget>,<x>, <y>)]: ");
    PlatformWidget * widget = dynamic_cast<PlatformWidget*>(v1.GetComplexData()->GetObject().get());


    int x = plist->Nth(2);// plist->PopFront();
    PError::AssertType(x, PEAT_NUMBER, "Argument error in second parameter of function [GetPixelColor(<widget>,<x>, <y>, <color>)]: ");

    int y = plist->Nth(3);// plist->PopFront();
    PError::AssertType(y, PEAT_NUMBER, "Argument error in third parameter of function [GetPixelColor(<widget>,<x>, <y>)]: ");

    PColor  c1 = (widget->GetPixel(x,y));

    PColor * tmpColor = new PColor(c1);

    counted_ptr<PEBLObjectBase> myColor = counted_ptr<PEBLObjectBase>(tmpColor);
    PComplexData *  pcd = new PComplexData(myColor);

    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
}

//This sets the  cursor location (in characters) within a textbox.
Variant PEBLObjects::SetCursorPosition(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_TEXTBOX, "Argument error in first argument of function [SetCursorPosition(<textbox>, <position>)]: ");


    counted_ptr<PEBLObjectBase> textbox = plist->First().GetComplexData()->GetObject(); //plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second argument of function [SetCursorPosition(<textbox>, <position>)]: ");
    Variant v1 = plist->Nth(2);// plist->PopFront();

    (dynamic_cast<PTextBox *>(textbox.get()))->SetCursorPosition(v1.GetInteger());

    return Variant(1);
}

//This gets the cursor location (in characters) within a textbox.
Variant PEBLObjects::GetCursorPosition(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_TEXTBOX, "Argument error in function [GetCursorPosition(<textbox>)]: ");

    counted_ptr<PEBLObjectBase> textbox = plist->First().GetComplexData()->GetObject(); //plist->PopFront();
    return Variant((dynamic_cast<PTextBox *>(textbox.get()))->GetCursorPosition());
}



Variant PEBLObjects::GetParent(Variant v)
{

    PList * plist = v.GetComplexData()->GetList();
    Variant v0 = plist->First();
    PError::AssertType(v0,PEAT_WIDGET,"Error in GetParent(<widget>): argument is not a widget");



    PlatformWidget * obj = dynamic_cast<PlatformWidget*>(v0.GetComplexData()->GetObject().get());

    PWidget * parent = obj->GetParent();

    if(parent)
        {
            counted_ptr<PEBLObjectBase> tmp2 = counted_ptr<PEBLObjectBase>(parent);
            PComplexData *  pcd = new PComplexData(tmp2);
            Variant tmp = Variant(pcd);

            return tmp;
        }else{

        return Variant(0);
    }
}

Variant PEBLObjects::SetEditable(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_TEXTBOX, "Argument error in first argument of function [SetEditable(<textbox>, <boolean>)]: ");

    PlatformTextBox * textbox = dynamic_cast<PlatformTextBox*>(plist->Nth(1).GetComplexData()->GetObject().get()); //plist->PopFront();

    Variant v1 = plist->Nth(2); //plist->PopFront();

    if(bool(v1))
        {
            textbox->SetEditable(true);
            return Variant(true);
        }
    else
        {
            textbox->SetEditable(false);
            return Variant(false);
        }

}



Variant PEBLObjects::SetText(Variant v)
{

    PList * plist = v.GetComplexData()->GetList();


    //This can be used on either a textbox or a label.
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_TEXTOBJECT, "Argument error in first argument of function [SetText(<object>, <text>)]: ");
    //Get the text to change it to.
    Variant v2 = plist->Nth(2);//First(); plist->PopFront();
    PError::AssertType(v2, PEAT_STRING, "Argument error in second argument of function [SetText(<object>, <text>)]: ");


    //Make the change.
    if(v1.GetComplexData()->IsLabel() )
        {
            PlatformLabel * label = dynamic_cast<PlatformLabel*>(v1.GetComplexData()->GetObject().get());
            label->SetText(v2.GetString());
        }
    else
        {
            PlatformTextBox * textbox = dynamic_cast<PlatformTextBox*>(v1.GetComplexData()->GetObject().get());
            textbox->SetText(v2.GetString());

        }

    return Variant(1);
}


Variant PEBLObjects::GetText(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_TEXTBOX, "Argument error in function [GetText(<textbox>)]: ");

    PlatformTextBox * textbox = dynamic_cast<PlatformTextBox*>(plist->First().GetComplexData()->GetObject().get());
    //    plist->PopFront();

    return Variant(textbox->GetText());
}


Variant PEBLObjects::GetLineBreaks(Variant v)
{
      PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_TEXTBOX, "Argument error in function [GetLineBreaks(<textbox>)]: ");

    PlatformTextBox * textbox = dynamic_cast<PlatformTextBox*>(plist->First().GetComplexData()->GetObject().get());
    return textbox->GetLineBreaks();
}

Variant PEBLObjects::SetFont(Variant v)
{

    PList * plist = v.GetComplexData()->GetList();

    //This can be used on either a textbox or a label.
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_TEXTOBJECT, "Argument error in first argument of function [SetFont(<object>, <font>)]: ");
    //Get the text to change it to.
    Variant v2 = plist->Nth(2);//First(); plist->PopFront();
    PError::AssertType(v2, PEAT_FONT, "Argument error in second argument of function [SetFont(<object>, <font>)]: ");


    //Make the change.
    if(v1.GetComplexData()->IsLabel() )
        {
            PlatformLabel * label = dynamic_cast<PlatformLabel*>(v1.GetComplexData()->GetObject().get());

            counted_ptr<PEBLObjectBase>tmpFont = v2.GetComplexData()->GetObject();
            label->SetFont(tmpFont);
        }
    else
        {
            PlatformTextBox * textbox =dynamic_cast<PlatformTextBox*>(v1.GetComplexData()->GetObject().get());
            counted_ptr<PEBLObjectBase> tmpFont = v2.GetComplexData()->GetObject();
            textbox->SetFont(tmpFont);
        }

    return Variant(1);
}

Variant PEBLObjects::PrintProperties(Variant v)
{
    //v[1] could be a widget, or an PEBLObject in general.
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    //This should also work for PEBLObjectBase things.
    //It should also work for devices and the like;
    //it probably does not currently do so.

    PError::AssertType(v1, PEAT_OBJECT, "Argument error in function [PrintProperties(<object>)]: ");

    //PlatformWidget * widget = dynamic_cast<PlatformWidget*>(v1.GetComplexData()->GetObject().get());
    PEBLObjectBase* obj = dynamic_cast<PEBLObjectBase*>(v1.GetComplexData()->GetObject().get());
    obj->PrintProperties(std::cout);

    return Variant(true);
}


Variant PEBLObjects::GetPropertyList(Variant v)
{
    //v[1] could be a widget, or an PEBLObject in general.
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    //This should also work for PEBLObjectBase things.
    //It should also work for devices and the like;
    //it probably does not currently do so.

    PError::AssertType(v1, PEAT_OBJECT, "Argument error in function [GetPropertyList(<object>)]: ");

    //PlatformWidget * widget = dynamic_cast<PlatformWidget*>(v1.GetComplexData()->GetObject().get());
    PEBLObjectBase* obj = dynamic_cast<PEBLObjectBase*>(v1.GetComplexData()->GetObject().get());

    return obj->GetPropertyList();
}


Variant PEBLObjects::SetProperty(Variant v)
{


    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    PError::AssertType(v1, PEAT_OBJECT, "Argument error in first parameter of function [SetProperty(<object>,<property>,<value>)]:  ");


    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_STRING, "Argument error in second parameter of function [SetProperty(<object>,<property>,<value>)]:  ");




    Variant v3 = plist->Nth(3);
    //No need to check v3 for type.

    v1.GetComplexData()->SetProperty(PEBLUtility::ToUpper(v2),v3);

    return Variant(true);
}


Variant PEBLObjects::GetProperty (Variant v)
{
    PList * plist = v.GetComplexData()->GetList();

    //v[1] should be a list
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_OBJECT, "Argument error in first parameter of function [GetProperty(<object>, <property>)]:  ");


    Variant prop =PEBLUtility::ToUpper( plist->Nth(2));

    //v[2] should be a string
    PError::AssertType(prop, PEAT_STRING, "Argument error in second parameter of function [Nth(<object>, <property>)]: ");
    return v1.GetComplexData()->GetProperty(prop);

}


Variant PEBLObjects::PropertyExists (Variant v)
{


    PList * plist = v.GetComplexData()->GetList();


    //v[1] should be a list
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_OBJECT, "Argument error in first parameter of function [PropertyExists(<object>, <property>)]:  ");


    Variant prop =PEBLUtility::ToUpper( plist->Nth(2));


    //v[2] should be a string
    PError::AssertType(prop, PEAT_STRING, "Argument error in second parameter of function [PropertyExists(<object>, <property>)]: ");

    return v1.GetComplexData()->PropertyExists(prop);

}



Variant PEBLObjects::Show(Variant v)
{

    //v[1] should have the  widget
    PList * plist = v.GetComplexData()->GetList();


    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_WIDGET, "Argument error in function [Show(<widget>)]: ");


    PlatformWidget * widget = dynamic_cast<PlatformWidget*>(v1.GetComplexData()->GetObject().get());

    widget->Show();

    return Variant(true);
}



Variant PEBLObjects::Hide(Variant v)
{

    //v[1] should have the  widget
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_WIDGET, "Argument error in function [Hide(<widget>)]: ");
    PlatformWidget * widget = dynamic_cast<PlatformWidget*>(v1.GetComplexData()->GetObject().get());

    widget->Hide();

    return Variant(true);
}






///
Variant PEBLObjects::Draw(Variant v)
{
    //v[1] should have the object.  If it is null,
    //redraw the entire environment.

    if(v.IsStackSignal())
        {
            myEnv->Draw();
        }
    else
        {

            PList * plist = v.GetComplexData()->GetList();

            Variant v2 = plist->First(); //plist->PopFront();
            PError::AssertType(v2, PEAT_WIDGET, "Argument error in function [Draw(<widget>)]: ");

            PlatformWidget * myWidget = dynamic_cast<PlatformWidget*>(v2.GetComplexData()->GetObject().get());
            myWidget->Draw();
        }
    return Variant(true);
}


///This calls the Draw() routine a number of times equal to the
///the parameter.  It returns a value that should approximate the
///presentation time fairly well.  It should be given a window.
Variant PEBLObjects::DrawFor(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_WIDGET, "Argument error in first parameter of function [DrawFor(<widndow>, <count>)]: ");

    PlatformWindow * window = dynamic_cast<PlatformWindow*>(v1.GetComplexData()->GetObject().get());
    PError::AssertType(plist->Nth(2), PEAT_INTEGER, "Argument error in second parameter of function [DrawFor(<window>, <count>)]: ");

    int count =plist->Nth(2);

    long unsigned int time = window->DrawFor(count);

    return Variant(time);

}


/// This function moves a widget to a new location.
/// It moves the CENTER of the object to the specified location, which
/// is probably the most common thing we want to do..  To
/// move some other part, use the GetSize(object) function.
Variant PEBLObjects::Move(Variant v)
{
   //v[1] should have the object, v[2] should be X, v[3] shoud be Y
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_WIDGET, "Argument error in first parameter of function [Move(<widget>,<x>, <y>)]: ");

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function [Move(<widget>,<x>, <y>)]: ");
    int x = plist->Nth(2); //plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_NUMBER, "Argument error in third parameter of function [Move(<widget>,<x>, <y>)]: ");
    int y = plist->Nth(3); //plist->PopFront();



    if(v1.GetComplexData()->IsThickLine() )
        {

            PlatformThickLine * tl = dynamic_cast<PlatformThickLine*>(v1.GetComplexData()->GetObject().get());
            tl->SetPosition(x,y);
        }
    else
        {
            //This uses the widget setposition, rather than the child classes overridden version.
            PWidget * widget = dynamic_cast<PWidget*>(v1.GetComplexData()->GetObject().get());


            widget->SetPosition(x , y);
        }

    return Variant(true);
}


Variant PEBLObjects::GetSize(Variant v)
{

    //v[1] should have the object
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_WIDGET, "Argument error in GetSize(<widget>). ");


    PWidget * widget = dynamic_cast<PWidget*>(v1.GetComplexData()->GetObject().get());

    Variant width = widget->GetWidth();
    Variant height = widget->GetHeight();

    PList * newlist = new PList();

    newlist->PushBack(height);
    newlist->PushBack(width);

    counted_ptr<PEBLObjectBase> newlist2 = counted_ptr<PEBLObjectBase>(newlist);
    PComplexData *   pcd = new PComplexData(newlist2);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
}



///This function loads a sound from a file. It also 'initializes' it
///when appropriate, so that it may be used by the play functions.
Variant PEBLObjects::LoadSound(Variant v)
{
   //v[1] should have the filename
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in function [LoadSound(<filename>)]: ");

    PlatformAudioOut * myAudio = new PlatformAudioOut();
    myAudio->LoadSoundFile(v1);
    // myAudio->Initialize();

    counted_ptr<PEBLObjectBase> audio2 = counted_ptr<PEBLObjectBase>(myAudio);
    PComplexData *  pcd = new PComplexData(audio2);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
}




//This plays a sound object, returning (almost) immediately, allowing
//the sound to play in the background.  If you want the function to only return
//after the sound playing is complete, use PlayForeground();
Variant PEBLObjects::PlayBackground(Variant v)
{
   //v[1] should have the PlatformAudioOut object
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_AUDIOOUT, "Argument error in function PlayBackground(<audio-stream>)]: ");

    PlatformAudioOut * myAudio = dynamic_cast<PlatformAudioOut*>(v1.GetComplexData()->GetObject().get());
    myAudio->Play();
    return Variant(true);
}

Variant PEBLObjects::PlayForeground(Variant v)
{
   //v[1] should have the PlatformAudioOut object
    PList * plist = v.GetComplexData()->GetList();


    Variant v1 = plist->First();// plist->PopFront();

    PError::AssertType(v1, PEAT_AUDIOOUT, "Argument error in function PlayForeground(<audio-stream>)]: ");
    PlatformAudioOut * myAudio = dynamic_cast<PlatformAudioOut*>(v1.GetComplexData()->GetObject().get());
    myAudio->PlayForeground();
    return Variant(true);
}


Variant PEBLObjects::Stop(Variant v)
{
    //v[1] should have the PlatformAudioOut object.
     PList * plist = v.GetComplexData()->GetList();

     Variant v1 = plist->First(); //plist->PopFront();

    PError::AssertType(v1, PEAT_AUDIOOUT, "Argument error in function Stop(<audio-stream>)]: ");

    PlatformAudioOut * myAudio = dynamic_cast<PlatformAudioOut*>(v1.GetComplexData()->GetObject().get());

    myAudio->Stop();
    return Variant(true);
}


Variant PEBLObjects::SetPanning(Variant v)
{

    //v[1] should have the PlatformAudioOut object.
     PList * plist = v.GetComplexData()->GetList();
     Variant v1 = plist->First(); //plist->PopFront();

    PError::AssertType(v1, PEAT_AUDIOOUT, "Argument error in function SetPanning(<audio-stream>,<left>,<right>)]: ");

    PlatformAudioOut * myAudio = dynamic_cast<PlatformAudioOut*>(v1.GetComplexData()->GetObject().get());


#ifdef PEBL_MIXER

    Variant left = (plist->Nth(2));
    Variant right = (plist->Nth(3));

    PError::AssertType(left,PEAT_NUMBER,"In SetPanning, <left> must be a number between 0 and 1");
    PError::AssertType(right,PEAT_NUMBER,"In SetPanning, <right> must be a number between 0 and 1");

    myAudio->SetPanning((pDouble)(left),
                        (pDouble)(right));
#endif

    return Variant(true);

}


Variant PEBLObjects::SetPlayRepeats(Variant v)
{
    //v[1] should have the PlatformAudioOut object.
     PList * plist = v.GetComplexData()->GetList();
     Variant v1 = plist->First(); //plist->PopFront();

    PError::AssertType(v1, PEAT_AUDIOOUT, "Argument error in function SetPlayRepeats(<audio-stream>,<repeats>)]: ");

    PlatformAudioOut * myAudio = dynamic_cast<PlatformAudioOut*>(v1.GetComplexData()->GetObject().get());


    Variant repeats = (plist->Nth(2));


    PError::AssertType(repeats,PEAT_INTEGER,"In SetPlayRepeats, <times> must be an integer");
#ifdef PEBL_MIXER
    myAudio->SetRepeats(repeats);
#endif
    return Variant(true);

}


Variant PEBLObjects::MakeSineWave(Variant v)
{

   //v[1] should have the filename
    PList * plist = v.GetComplexData()->GetList();


    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_NUMBER, "Argument error in first parameter of  function [MakeSineWave(<freq>,<length>,<amplitude>)]: ");

    Variant v2 = plist->Nth(2); //plist->PopFront();
    PError::AssertType(v2, PEAT_INTEGER, "Argument error in second parameter of function [MakeSineWave(<freq>,<length>,<amplitude>)]: ");

    Variant v3 = plist->Nth(3);// plist->PopFront();
    PError::AssertType(v3, PEAT_NUMBER, "Argument error in third parameter of function [MakeSineWave(<freq>,<length>,<amplitude>)]: ");

    if((((pDouble)v3>1.0 )| ((pDouble)v3<-1.0)))
        {
            PError::SignalWarning("amplitude in MakeSineWave(<freq>,<length>,<amplitude> will produce clipping if greater than 1.0");
        }

    PlatformAudioOut * myAudio = new PlatformAudioOut();
    myAudio->CreateSineWave((pDouble)v1,(int)v2, (pDouble)v3);
    myAudio->Initialize();

    counted_ptr<PEBLObjectBase> audio2 = counted_ptr<PEBLObjectBase>(myAudio);
    PComplexData *  pcd = new PComplexData(audio2);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;

}


Variant PEBLObjects::MakeAudioInputBuffer(Variant v)
{


#ifdef PEBL_AUDIOIN
    //
     PList * plist = v.GetComplexData()->GetList();


     Variant v1 = plist->First(); //plist->PopFront();
     PError::AssertType(v1, PEAT_NUMBER, "Argument error in first parameter of  function [MakeAudioInputBuffer(<duration>)]: ");


     PlatformAudioIn * myAudio = new PlatformAudioIn();
     myAudio->CreateBuffer(v1);

     AudioInfo * tmp= myAudio->ReleaseAudioOutBuffer();
     delete myAudio;

     //Variant out = myAudio->VoiceKey(v,(pDouble)v1,(int)v2);
     PlatformAudioOut * myOut = new PlatformAudioOut();
     myOut->LoadSoundFromData((tmp->audio),tmp->audiolen,&(tmp->spec));
     myOut->Initialize();

     counted_ptr<PEBLObjectBase> audio2 = counted_ptr<PEBLObjectBase>(myOut);
     PComplexData *  pcd = new PComplexData(audio2);
     Variant tmpv = Variant(pcd);
     //     delete pcd;
     pcd=NULL;
     return tmpv;
#else
     PError::SignalFatalError("Audio Input not available on this version of PEBL.");
     return false;
#endif

}


Variant PEBLObjects::SaveAudioToWaveFile(Variant v)
{


    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_STRING, "Argument error in first parameter of function [SaveAudioToWaveFile(<filename>,<audio-buffer>)]: ");


    Variant v2 = plist->Nth(2);// plist->PopFront();
    PError::AssertType(v2, PEAT_AUDIOOUT, "Argument error second parameter of function [SaveAudioToWaveFile(<filename>,<audio-buffer>)]: ");

    PlatformAudioOut * myAudio = dynamic_cast<PlatformAudioOut*>(v2.GetComplexData()->GetObject().get());
    myAudio->SaveBufferToWave(v1);
    return Variant(true);


}





Variant PEBLObjects::GetVocalResponseTime(Variant v)
{
#ifdef PEBL_AUDIOIN

	//
     PList * plist = v.GetComplexData()->GetList();


     Variant v1 = plist->First();// plist->PopFront();
//     PError::AssertType(v1, PEAT_NUMBER, "Argument error in first parameter of  function [MakeSineWave(<freq>,<length>,<amplitude>)]: ");

     Variant v2 = plist->Nth(2); //plist->PopFront();
//     PError::AssertType(v2, PEAT_INTEGER, "Argument error in second parameter of function [MakeSineWave(<freq>,<length>,<amplitude>)]: ");

     Variant v3 = plist->Nth(3); //plist->PopFront();
//     PError::AssertType(v3, PEAT_NUMBER, "Argument error in third parameter of function [MakeSineWave(<freq>,<length>,<amplitude>)]: ");

//     if((((pDouble)v3>1.0 )| ((pDouble)v3<-1.0)))
//         {
//             PError::SignalWarning("amplitude in MakeSineWave(<freq>,<length>,<amplitude> will produce clipping if greater than 1.0");
//         }

     //v1 should be a audiout object, which has our buffer.

     PlatformAudioOut * po = dynamic_cast<PlatformAudioOut*>(v1.GetComplexData()->GetObject().get());
     AudioInfo * tmp = po->GetAudioInfo();

     PlatformAudioIn * myAudio = new PlatformAudioIn();
     myAudio->UseBuffer(tmp);
     myAudio->Initialize(1);
     //delete tmp;
     //     tmp= NULL;

     Variant out = myAudio->VoiceKey((pDouble)v2,(int)v3);

     return out;

#else
     PError::SignalFatalError("Audio Input not available on this version of PEBL.");
     return false;
#endif

}

Variant PEBLObjects::MakeSquareWave(Variant v)
{
    PError::SignalFatalError("Function [MakeSquareWave] Not implemented.");

    return Variant(false);
}


Variant PEBLObjects::MakeSawtoothWave(Variant v)
{
    PError::SignalFatalError("Function [MakeSawtoothWave] Not implemented.");
    return Variant(false);
}

Variant PEBLObjects::MakeChirp(Variant v)
{
    PError::SignalFatalError("Function [MakeChirp] Not implemented.");
    return Variant(false);
}



Variant PEBLObjects::Line(Variant v)
{
    // v[1] should be X, v[2] should be Y
    // v[3] should be dx, v[4] should be dy
    // v[5] should be the color.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [Line(<x>, <y>, <dx>, <dy>, <color>)]: ");
    int x = plist->First(); //plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function  [Line(<x>, <y>, <dx>, <dy>, <color>)]: ");
    int y = plist->Nth(2); //plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_NUMBER, "Argument error in third parameter of function  [Line(<x>, <y>, <dx>, <dy>, <color>)]: ");
    int dx = plist->Nth(3);// plist->PopFront();

    PError::AssertType(plist->Nth(4), PEAT_NUMBER, "Argument error in fourth parameter of function  [Line(<x>, <y>, <dx>, <dy>, <color>)]: ");
    int dy = plist->Nth(4);// plist->PopFront();

    PError::AssertType(plist->Nth(5), PEAT_COLOR, "Argument error in fifth parameter of function  [Line(<x>, <y>, <dx>, <dy>, <color>)]: ");
    Variant color = plist->Nth(5);// plist->PopFront();

    counted_ptr<PEBLObjectBase> myLine = counted_ptr<PEBLObjectBase>(new PlatformLine(x,y,dx,dy,color));
    PComplexData *  pcd = new PComplexData(myLine);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
 }



Variant PEBLObjects::ThickLine(Variant v)
{
    // v[1] should be X, v[2] should be Y
    // v[3] should be dx, v[4] should be dy
    // v[5] should be the color.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [ThickLine(<x1>, <y1>, <x2>, <y2>, <thickness>,<color>)]: ");
    int x1 = plist->First(); //plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function [ThickLine(<x1>, <y1>, <x2>, <y2>, <thickness>,<color>)]: ");
    int y1 = plist->Nth(2); //plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_NUMBER, "Argument error in third parameter of function [ThickLine(<x1>, <y1>, <x2>, <y2>, <thickness>,<color>)]: ");
    int x2 = plist->Nth(3);// plist->PopFront();

    PError::AssertType(plist->Nth(4), PEAT_NUMBER, "Argument error in fourth parameter of function [ThickLine(<x1>, <y1>, <x2>, <y2>, <thickness>,<color>)]: ");
    int y2 = plist->Nth(4);// plist->PopFront();

    PError::AssertType(plist->Nth(5), PEAT_NUMBER, "Argument error in fifth parameter of function [ThickLine(<x1>, <y1>, <x2>, <y2>, <thickness>,<color>)]: ");
    int thickness = plist->Nth(5);// plist->PopFront();


    PError::AssertType(plist->Nth(6), PEAT_COLOR, "Argument error in sixth parameter of function [ThickLine(<x1>, <y1>, <x2>, <y2>, <thickness>,<color>)]: ");
    Variant color = plist->Nth(6);// plist->PopFront();

    counted_ptr<PEBLObjectBase> myLine = counted_ptr<PEBLObjectBase>(new PlatformThickLine(x1,y1,x2,y2,thickness,color));
    PComplexData *  pcd = new PComplexData(myLine);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
 }




Variant PEBLObjects::Rectangle(Variant v)
{

    //
    // v[1] should be X, v[2] shoud be Y
    // v[3] should be dx, v[4] should be dy
    // v[5] should be the color, v[6] should be whether it is filled.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [Rectangle(<x>, <y>, <dx>, <dy>, <color>, <filled>)]: ");
    int x = plist->First(); //plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function  [Rectangle(<x>, <y>, <dx>, <dy>, <color>, <filled>)]: ");
    int y = plist->Nth(2);// plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_NUMBER, "Argument error in third parameter of function  [Rectangle(<x>, <y>, <dx>, <dy>, <color>, <filled>)]: ");
    int dx = plist->Nth(3);// plist->PopFront();

    PError::AssertType(plist->Nth(4), PEAT_NUMBER, "Argument error in fourth parameter of function  [Rectangle(<x>, <y>, <dx>, <dy>, <color>, <filled>)]: ");
    int dy = plist->Nth(4); //plist->PopFront();

    PError::AssertType(plist->Nth(5), PEAT_COLOR, "Argument error in fifth parameter of function  [Rectangle(<x>, <y>, <dx>, <dy>, <color>, <filled>)]: ");
    Variant color = plist->Nth(5);// plist->PopFront();

    PError::AssertType(plist->Nth(6), PEAT_NUMBER, "Argument error in sixth parameter of function  [Rectangle(<x>, <y>, <dx>, <dy>, <color>, <filled>)]: ");
    int filled = plist->Nth(6); //plist->PopFront();

    counted_ptr<PEBLObjectBase> myRect = counted_ptr<PEBLObjectBase>(new PlatformRectangle(x,y,dx,dy,color,filled));
    PComplexData *  pcd = new PComplexData(myRect);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;


}




Variant PEBLObjects::Square(Variant v)
{
    // v[1] should be X, v[2] shoud be Y
    // v[3] should be dx, v[4] should be dy
    // v[5] should be the color, v[6] should be whether it is filled.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [Square(<x>, <y>, <size>, <color>, <filled>)]: ");
    int x = plist->First(); //plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function  [Square(<x>, <y>, <size>, <color>, <filled>)]: ");
    int y = plist->Nth(2);// plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_NUMBER, "Argument error in third parameter of function  [Square(<x>, <y>, <size>, <color>, <filled>)]: ");
    int size = plist->Nth(3); //plist->PopFront();

    PError::AssertType(plist->Nth(4), PEAT_COLOR, "Argument error in fourth parameter of function  [Square(<x>, <y>, <size>, <color>, <filled>)]: ");

    Variant color = plist->Nth(4);// plist->PopFront();

    PError::AssertType(plist->Nth(5), PEAT_NUMBER, "Argument error in fifth parameter of function  [Square(<x>, <y>, <size>, <color>, <filled>)]: ");
    int filled = plist->Nth(5);// plist->PopFront();

    counted_ptr<PEBLObjectBase> mySquare = counted_ptr<PEBLObjectBase>(new PlatformSquare(x,y,size,color,filled));
    PComplexData *  pcd = new PComplexData(mySquare);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
}


Variant PEBLObjects::Ellipse(Variant v)
{

    // v[1] should be X, v[2] shoud be Y
    // v[3] should be dx, v[4] should be dy
    // v[5] should be the color.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [Ellipse(<x>, <y>, <rx>, <ry>, <color>, <filled>)]: ");
    int x = plist->First();// plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function  [Ellipse(<x>, <y>, <rx>, <ry>, <color>, <filled>)]: ");
    int y = plist->Nth(2); //plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_NUMBER, "Argument error in third parameter of function  [Ellipse(<x>, <y>, <rx>, <ry>, <color>, <filled>)]: ");
    int rx = plist->Nth(3); //plist->PopFront();

    PError::AssertType(plist->Nth(4), PEAT_NUMBER, "Argument error in fourth parameter of function  [Ellipse(<x>, <y>, <rx>, <ry>, <color>, <filled>)]: ");
    int ry = plist->Nth(4);// plist->PopFront();

    PError::AssertType(plist->Nth(5), PEAT_COLOR, "Argument error in fifth parameter of function  [Ellipse(<x>, <y>, <rx>, <ry>, <color>, <filled>)]: ");
    Variant color = plist->Nth(5);// plist->PopFront();

    PError::AssertType(plist->Nth(6), PEAT_NUMBER, "Argument error in sixth parameter of function  [Ellipse(<x>, <y>, <rx>, <ry>, <color>, <filled>)]: ");
    int filled = plist->Nth(6);// plist->PopFront();


    counted_ptr<PEBLObjectBase> myEllipse = counted_ptr<PEBLObjectBase>(new PlatformEllipse(x,y,rx,ry,color,filled));
    PComplexData *  pcd = new PComplexData(myEllipse);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;


}




Variant PEBLObjects::Circle(Variant v)
{

    // v[1] should be X, v[2] shoud be Y
    // v[3] should be dx, v[4] should be dy
    // v[5] should be the color.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [Circle(<x>, <y>, <rx>,  <color>)]: ");
    int x = plist->First();// plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function  [Circle(<x>, <y>, <rx>, <color>)]: ");
    int y = plist->Nth(2);// plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_NUMBER, "Argument error in third parameter of function  [Circle(<x>, <y>, <r>,, <color>)]: ");
    int r = plist->Nth(3);// plist->PopFront();


    PError::AssertType(plist->Nth(4), PEAT_COLOR, "Argument error in fourth parameter of function  [Circle(<x>, <y>, <rx>, <color>)]: ");
    Variant color = plist->Nth(4);// plist->PopFront();

    PError::AssertType(plist->Nth(5), PEAT_NUMBER, "Argument error in fifth parameter of function  [Circle(<x>, <y>, <rx>, <ry>, <color>, <filled>)]: ");
    int filled = plist->Nth(5);// plist->PopFront();


    counted_ptr<PEBLObjectBase> myCircle = counted_ptr<PEBLObjectBase>(new PlatformCircle(x,y,r,color,filled));
    PComplexData *  pcd = new PComplexData(myCircle);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;

}


Variant PEBLObjects::Polygon(Variant v)
{

    // v[1] should be X, v[2] shoud be Y
    // v[3] list-of-x-points
    // v[4] list-of-y-points
    // v[5] should be should be color
    // v[6] should be whether filled
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [Polygon(<x>, <y>, <xpoints>, <ypoints>, <color>, <filled>)]: ");
    int x = plist->First(); //plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function  [Polygon(<x>, <y>,<xpoints>, <ypoints>, <color>, <filled>)]: ");
    int y = plist->Nth(2); //plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_LIST, "Argument error in third parameter of function  [Polygon(<x>, <y>, <xpoints>, <ypoints>, <color>, <filled>)]: ");
    Variant xpoints = plist->Nth(3); //plist->PopFront();

    PError::AssertType(plist->Nth(4), PEAT_LIST, "Argument error in fourth parameter of function  [Polygon(<x>, <y>, <xpoints>, <ypoints>, <color>, <filled>)]: ");
    Variant ypoints = plist->Nth(4);// plist->PopFront();

    PError::AssertType(plist->Nth(5), PEAT_COLOR, "Argument error in fifth parameter of function  [Polygon(<x>, <y>, <xpoints>, <ypoints>, <color>, <filled>)]: ");
    Variant color = plist->Nth(5);// plist->PopFront();

    PError::AssertType(plist->Nth(6), PEAT_NUMBER, "Argument error in sixth parameter of function  [Polygon(<x>, <y>, <xpoints>, <ypoints>, <color>, <filled>)]: ");
    int filled = plist->Nth(6);// plist->PopFront();

    counted_ptr<PEBLObjectBase> myPolygon = counted_ptr<PEBLObjectBase>(new PlatformPolygon(x,y,xpoints,ypoints,color,filled));
    PComplexData *  pcd = new PComplexData(myPolygon);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;

}




Variant PEBLObjects::Bezier(Variant v)
{

    // v[1] should be X, v[2] shoud be Y
    // v[3] should be dx, v[4] should be dy
    // v[5] should be the color.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [Bezier(<x>, <y>, <xpoints>, <ypoints>, <steps>, <color>)]: ");
    int x = plist->First(); //plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function  [Bezier(<x>, <y>,<xpoints>, <ypoints>, <steps>, <color>)]: ");
    int y = plist->Nth(2); //plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_LIST, "Argument error in third parameter of function  [Bezier(<x>, <y>, <xpoints>, <ypoints>, <steps>, <color>)]: ");
    Variant xpoints = plist->Nth(3); //plist->PopFront();

    PError::AssertType(plist->Nth(4), PEAT_LIST, "Argument error in fourth parameter of function  [Bezier(<x>, <y>, <xpoints>, <ypoints>, <steps>, <color>)]: ");
    Variant ypoints = plist->Nth(4);// plist->PopFront();

    PError::AssertType(plist->Nth(5), PEAT_NUMBER, "Argument error in fifth parameter of function  [Bezier(<x>, <y>, <xpoints>, <ypoints>, <steps>, <color>)]: ");
    int steps = plist->Nth(5);// plist->PopFront();

    PError::AssertType(plist->Nth(6), PEAT_COLOR, "Argument error in sixth parameter of function  [Bezier(<x>, <y>, <xpoints>, <ypoints>, <steps>, <color>)]: ");
    Variant color = plist->Nth(6);// plist->PopFront();


    counted_ptr<PEBLObjectBase> myBezier = counted_ptr<PEBLObjectBase>(new PlatformBezier(x,y,xpoints,ypoints,steps,color));
    PComplexData *  pcd = new PComplexData(myBezier);

    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;


}





Variant PEBLObjects::RotoZoom(Variant v)
{

    // v[1] should be an object,
    // v[2] shoud be rotation
    // v[3] should be xzoom, v[4] should be yzoom
    // v[5] should be whether to anti-alias.

    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_WIDGET, "Argument error in first parameter of function [ROTOZOOM(<widget>,<rotation>,<xzoom>, <yzoom>, <smooth>)]: ");
    Variant v1 = plist->First(); //plist->PopFront();
    PlatformWidget * widget = dynamic_cast<PlatformWidget*>(v1.GetComplexData()->GetObject().get());



    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function [ROTOZOOM(<widget>,<rotation>,<xzoom>, <yzoom>, <smooth>)]: ");
    Variant r = plist->Nth(2); //plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_NUMBER, "Argument error in third parameter of function [ROTOZOOM(<widget>,<rotation>,<xzoom>, <yzoom>, <smooth>)]: ");
    Variant x = plist->Nth(3); //plist->PopFront();


    PError::AssertType(plist->Nth(4), PEAT_NUMBER, "Argument error in fourth parameter of function [ROTOZOOM(<widget>,<rotation>,<xzoom>, <yzoom>, <smooth>)]: ");
    Variant y = plist->Nth(4); //plist->PopFront();

    PError::AssertType(plist->Nth(5), PEAT_NUMBER, "Argument error in fifth parameter of function [ROTOZOOM(<widget>,<rotation>,<xzoom>, <yzoom>, <smooth>)]: ");
    int smooth = plist->Nth(5);// plist->PopFront();

    bool result = widget->RotoZoom((pDouble)r,(pDouble)x,(pDouble)y,smooth);


    if(!result)PError::SignalFatalError("Rotozoom failed.");

    return v1.GetComplexData();
}



//define MakeGabor(size,freq,sd, angle,phase,bglev)

#if 0
Variant PEBLObjects::Gabor(Variant v)
{

    //
    // v[1] should be X, v[2] shoud be Y
    // v[3] should be dx, v[4] should be dy
    // v[5] should be the color, v[6] should be whether it is filled.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [MakeCanvas(<width>, <height>, <color>)]: ");
    int width = plist->First(); //plist->PopFront();

    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function  [MakeCanvas(<width>, <height>, <color>)]: ");
    int height = plist->Nth(2);// plist->PopFront();


    PError::AssertType(plist->Nth(3), PEAT_COLOR, "Argument error in fifth parameter of function  [MakeCanvas(<x>, <y>, <dx>, <dy>, <color>)]: ");
    Variant color = plist->Nth(3);// plist->PopFront();


    int size = 100;
    double freq = 10;
    double sd = 12;
    double angle = 0;
    double phase = 0;
    int bglev = 128;

    //Make a canvas:
    counted_ptr<PEBLObjectBase> myCanvas = counted_ptr<PEBLObjectBase>(new PlatformCanvas(width,height,color));
    PComplexData *  pcd = new PComplexData(myCanvas);



    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;



}

#endif

Variant PEBLObjects::LoadMovie(Variant v)
{

#ifdef PEBL_MOVIES

    // v[1] should be name of movie
    // v[2] should be width; v[3] should be height.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in first parameter of function [LoadMovie(<filename>,<window>,<width>, <height>)]: ");
    Variant filename = plist->First();

    Variant v2 = plist->Nth(2); //plist->PopFront();
    PError::AssertType(v2, PEAT_WIDGET, "Argument error in second parameter of function [LoadMovie(<filename>,<window>,<width>, <height>)]: ");

    PlatformWindow * window = dynamic_cast<PlatformWindow*>(v2.GetComplexData()->GetObject().get());



    PError::AssertType(plist->Nth(3), PEAT_NUMBER, "Argument error in second parameter of function  [LoadMovie(<filename>,<window>,<width>, <height>)]: ");
    int width = plist->Nth(3);


    PError::AssertType(plist->Nth(4), PEAT_NUMBER, "Argument error in second parameter of function  [LoadMovie(<filename>,<window>,<width>, <height>)]: ");
    int height = plist->Nth(4);


    PlatformMovie* myMovie = new PlatformMovie();
    myMovie->LoadMovie(filename,window,width,height);

    //    myMovie->StartPlayback();

    counted_ptr<PEBLObjectBase> tmpMov = counted_ptr<PEBLObjectBase>(myMovie);
    PComplexData *  pcd = new PComplexData(tmpMov);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
#else
    return Variant(false);
#endif

}




Variant PEBLObjects::LoadAudioFile(Variant v)
{
#ifdef PEBL_MOVIES
    // v[1] should be name of movie
    // v[2] should be width; v[3] should be height.
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_STRING, "Argument error in first parameter of function [LoadAudioFile(<filename>)]: ");
    Variant filename = plist->First();


    PlatformMovie* myMovie = new PlatformMovie();
    myMovie->LoadAudioFile(filename);

    //    myMovie->StartPlayback();

    counted_ptr<PEBLObjectBase> tmpMov = counted_ptr<PEBLObjectBase>(myMovie);
    PComplexData *  pcd = new PComplexData(tmpMov);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
#else
    return false;
#endif
}



Variant PEBLObjects::StartPlayback(Variant v)
{
#ifdef PEBL_MOVIES
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_MOVIE, "Argument error in first parameter of function [StartPlayback(<movie>)]: ");

    Variant v1 = plist->First();
    PlatformMovie * myMovie = dynamic_cast<PlatformMovie*>(v1.GetComplexData()->GetObject().get());
    myMovie->StartPlayback();
    return Variant(true);
#else
    return false;
#endif
}


Variant PEBLObjects::PausePlayback(Variant v)
{

#ifdef PEBL_MOVIES
    PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_MOVIE, "Argument error in first parameter of function [PausePlayback(<movie>)]: ");


    Variant v1 = plist->First();
    PlatformMovie * myMovie = dynamic_cast<PlatformMovie*>(v1.GetComplexData()->GetObject().get());
    myMovie->PausePlayback();
    return Variant(true);
#else
    return false;
#endif
}




Variant PEBLObjects::MakeCustomObject(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    std::string name = plist->First();// plist->PopFront();

    PCustomObject* myobj = new PCustomObject(name);
    counted_ptr<PEBLObjectBase> tmpObject = counted_ptr<PEBLObjectBase>(myobj);
    PComplexData *  pcd = new PComplexData(tmpObject);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;

}


//Currently, this works with locally-connected eyetribe
//tracker using gazelib.  In theory, it could work more generally
Variant PEBLObjects::ConnectEyeTracker(Variant v)
{

#ifdef PEBL_GAZELIB
    //this should offer setting the address and port (default is 127.0.0.1/6555)
    PEyeTracker * pet = new PEyeTracker();

    counted_ptr<PEBLObjectBase> tmpObject = counted_ptr<PEBLObjectBase>(pet);

    PComplexData *  pcd = new PComplexData(tmpObject);
    Variant tmp = Variant(pcd);
    delete pcd;
    pcd=NULL;
    return tmp;
#else
    return Variant(0);
#endif

}



// This returns a 'custom' object related to the current state of the
// eye position.
//
Variant PEBLObjects::GetEyeObject(Variant v)
{

#ifdef PEBL_GAZELIB
    PList * plist = v.GetComplexData()->GetList();
    Variant obj  = plist->First();

    PEyeTracker * pet = dynamic_cast<PEyeTracker*>(obj.GetComplexData()->GetObject().get());

    gtl::GazeData * gd = pet->GetGazeFrame();

    Variant obj2 = ConvertGazeData(*gd);
    delete gd;
    gd = NULL;

    return obj2;
#else
    return Variant(0);
#endif
}

Variant PEBLObjects::SetEyeTrackerHandler(Variant v)
{


#ifdef PEBL_GAZELIB


    PList * plist = v.GetComplexData()->GetList();
    Variant obj  = plist->First();

    PEyeTracker * pet = dynamic_cast<PEyeTracker*>(obj.GetComplexData()->GetObject().get());
    Variant fname = plist->Nth(2);

    PEBLGaze * pg = new PEBLGaze(fname);
#endif

    return Variant(1);

}
