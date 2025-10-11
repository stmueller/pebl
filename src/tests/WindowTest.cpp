//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       WindowTest.cpp
//    Purpose:    Tests the basic Window widget class hierarchy
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003 Shane T. Mueller <smueller@umich.edu>
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


#include "../base/Variant.h"
#include "../objects/PObject.h"
#include "../objects/PWidget.h"

#include "../objects/PEnvironment.h"
#include "../objects/PWindow.h"
#include "../devices/PKeyboard.h"
#include "../objects/PImageBox.h"

#include "../platforms/sdl/PlatformEnvironment.h"
#include "../platforms/sdl/PlatformWindow.h"
#include "../platforms/sdl/PlatformKeyboard.h"
#include "../platforms/sdl/PlatformImageBox.h"
#include "../platforms/sdl/PlatformFont.h"
#include "../platforms/sdl/PlatformWord.h"

#include <iostream>


//The following is the entry point for the command-line version
int main(int argc, char **argv)
{

    //This sets the video driver
    setenv("SDL_VIDEODRIVER", "dga",1);
    setenv("SDL_VIDEODRIVER", "x11",1);



    //Make a few new colors.
    cout << "Creating new colors\n";
    PColor  red =  PColor("red");
    PColor  grey = PColor("grey");
    grey.SetAlpha(150);
 
    PlatformEnvironment * myEnv = new PlatformEnvironment();
    myEnv->Initialize();
 
    PlatformWindow * myWindow = new PlatformWindow();
    myWindow->Initialize();
    myWindow->SetBackgroundColor(grey);
    
    PKeyboard * myKeyboard = new PlatformKeyboard();


    //Add window to environment.
    myEnv->AddWindow(myWindow);

    
    //Now make an Image Widget.
    PlatformImageBox * myImage1 = new PlatformImageBox;
    if(!myImage1->LoadImage("media/images/pebl.bmp"))
        cout << "Loading image1 failed\n" ;
    

    //Make another image, just for kicks.
    PlatformImageBox * myImage2 = new PlatformImageBox;
    if(!myImage2->LoadImage("media/images/pebl.png"))
        cout << "Loading image2 failed\n" ;

   //Make a font.
    cout << "Creating a font\n";
    PlatformFont * myFont1 = new PlatformFont("media/fonts/VeraSe.ttf", PFS_Normal, 22, red, grey, true );
    PlatformFont * myFont2 = new PlatformFont("media/fonts/VeraSe.ttf", PFS_Normal, 22, red, grey, false );


    cout << "Creating PlatformWord object\n";
    //Now, make a 'word object.
    PlatformWord * myWord1 = new PlatformWord("Hello World: anti-aliased", myFont1);
    myWord1->SetPosition(30,30);

    PlatformWord * myWord2 = new PlatformWord("Hello World:aliased", myFont2);
    myWord2->SetPosition(30,50);




    myWindow->AddSubWidget(myImage1);
    myWindow->AddSubWidget(myImage2);
    myWindow->AddSubWidget(myWord1);
    myWindow->AddSubWidget(myWord2);

  
    myEnv->Draw();
    myKeyboard->WaitForAnyKeyDown();
    myKeyboard->WaitForAllKeysUp();


    myImage1->SetPosition(10,10);
    myImage2->SetPosition(10,200);
    myEnv->Draw();
    myKeyboard->WaitForAnyKeyDown();
    myKeyboard->WaitForAllKeysUp();

    

    myImage1->SetPosition(100,20);
    myImage2->SetPosition(100,300);
    myEnv->Draw();
    myKeyboard->WaitForAllKeysUp();
    myKeyboard->WaitForAnyKeyDown();



    myImage1->SetPosition(200,200);
    myImage2->SetPosition(200,400);
    myEnv->Draw();
    myKeyboard->WaitForAllKeysUp();

    myKeyboard->WaitForAnyKeyDown();


    for(int i= 0; i<500;i+=10){      
        myImage1->SetPosition(i,400);
        myEnv->Draw();
    }


    myKeyboard->WaitForAllKeysUp();        
    myKeyboard->WaitForAnyKeyDown();
    cout << "Done\n";
    

    delete myImage1, myImage2;
    delete myWindow;
    delete myEnv;

    return 0;
}
