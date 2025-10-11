//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/utility/PError.cpp
//    Purpose:    Utility class for signaling warnings and errors
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

#include "PError.h"
#include "../base/PNode.h"
#include "../base/Evaluator.h"

#include "../base/Variant.h"
#include "../base/PComplexData.h"
#include "../platforms/sdl/SDLUtility.h"

#include <iostream>
#include <string>
#include <signal.h>


#ifdef PEBL_EMSCRIPTEN
#include <emscripten.h>
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::string;

extern PlatformEnvironment * myEnv;


void PError::SignalFatalError(const string & message)
{

    const PNode * node = Evaluator::gEvalNode;



    
    Evaluator::gCallStack.PrintCallStack(cerr);
    Variant errorMessage;
    
    if(node)
        {
            errorMessage = Variant("\nError near line ") +
                Variant(node->GetLineNumber())+
                Variant(" of file ") +Variant( node->GetFilename()) +Variant(":\n") + message;


        }
    else
        {
            errorMessage = Variant("\nError before files are loaded.\n") + message;

        }


    //At this point, we'd like to, if possible, make a
    //pop-up message box in an SDL window.
    cerr << errorMessage.GetString() << endl;
        
    if(myEnv)
        {

            PlatformWindow* pw = dynamic_cast<PlatformWindow*>(myEnv->GetWindow(0));
            if(pw)
                SDLUtility::PopupErrorBox(pw, errorMessage.GetString().c_str());
        }


    raise(SIGTERM);
    exit(0);
#ifdef PEBL_EMSCRIPTEN
    emscripten_cancel_main_loop();
    emscripten_force_exit( 0);

#endif
}
 

void PError::SignalWarning(const std::string &  message)
{
    const PNode * node = Evaluator::gEvalNode;
    cerr << "Non-fatal Warning in line " << node->GetLineNumber() <<   " of file " << node->GetFilename()  << ":\n\t";
    cerr <<  message << endl;
}

void PError::ExitQuietly(const std::string & message)
{
    cerr << message << endl;
    raise(SIGTERM);
    exit(0);
}

///This function makes sure that a value has the proper
///underlying type.  If it doesn't, it will cause a fatal error and 
///report whereabouts the error happened.
void PError::AssertType(Variant v, int type, const string & outsideMessage)
{

	string message;

	switch(type)
		{

		case PEAT_STACK_SIGNAL:
			if( !v.IsStackSignal())
				{
                    message = outsideMessage;
                    message += "Wanted stack signal but got a " + GetTypeName(v) + ": " + v.GetString();
                    SignalFatalError(message);
				}

			break;


		case PEAT_FUNCTION:
			if( !v.IsFunction())
				{
                      message = outsideMessage;
					message += "Wanted function but got a " + GetTypeName(v) + ": "  + v.GetString();
					SignalFatalError(message);
				}

			break;
		case PEAT_FUNCTION_POINTER:
			if( !v.IsFunction())
				{
                    message = outsideMessage;
					message += "Wanted function pointer but got " + GetTypeName(v) + ": " + v.GetString();
					SignalFatalError(message);
				}

			break;
		case PEAT_NUMBER:
			if( !v.IsNumber())
				{
                    message = outsideMessage;
					message += "Wanted number but got " + GetTypeName(v) + ": " + v.GetString();
					SignalFatalError(message);
				}
			break;

		case PEAT_INTEGER:

			if( !v.IsInteger())
				{
                    message = outsideMessage;
                    message += "Wanted integer but got " + GetTypeName(v) + ": " + v.GetString();
					SignalFatalError(message);
				}
			break;
		case PEAT_FLOAT:
			if( !v.IsNumber())
				{
                    message = outsideMessage;
					message +="Wanted floating-point number but got " + GetTypeName(v) + ": "+ v.GetString();
					SignalFatalError(message);
				}
			break;

		case PEAT_STRING:
			if( !v.IsString())
				{
                    message = outsideMessage;
					message +="Wanted string but got " + GetTypeName(v) + ": "+ v.GetString();
					SignalFatalError(message);
				}
			break;

		case PEAT_VARIABLE:
            
			if( v.IsGlobalVariable() || v.IsLocalVariable())
				{
                    return;
                }
            message = outsideMessage;
            message +="Wanted variable but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);
            
			break;


        case PEAT_AUDIOOUT:
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsAudioOut())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted AudioOut stream but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);
			
			break;
        case PEAT_COLOR:
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsColor())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            
            message = outsideMessage;
            message +="Wanted PEBL Color but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);           
          break;
          
        case PEAT_ENVIRONMENT:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsEnvironment())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted Environment but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);
            
            break;

        case PEAT_FILESTREAM:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsFileStream())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted FileStream but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);

          break;
          

        case PEAT_FONT:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsFont())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            
            message = outsideMessage;
            message +="Wanted font but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);
            
            break;


        case PEAT_IMAGEBOX:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsImageBox())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted Image but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);
            
            break;



        case PEAT_JOYSTICK:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsJoystick())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted Joystick but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);
            
            break;


        case PEAT_KEYBOARD:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsKeyboard())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted Keyboard but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);
            
            break;

        case PEAT_LIST:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsList())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted list but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);

          break;


      case PEAT_WIDGET:
          
          if(v.IsComplexData())
              {
                  if((v.GetComplexData())->IsWidget())
                      {
                          //Everything is fine, return without error.
                          return;
                      }
              }
          message = outsideMessage;
          message +="Wanted Widget but got " + GetTypeName(v) + ": "+ v.GetString();
          SignalFatalError(message);
          
          break;


      case PEAT_OBJECT:
          if(v.IsComplexData())
              {
                  //Any complex data is a PEBLObject (except maybe a list?)
                  return;
              }
          message = outsideMessage;
          message +="Wanted Object but got " + GetTypeName(v) + ": "+ v.GetString();
          SignalFatalError(message);
          
          break;
          

      case PEAT_WINDOW:
          
          if(v.IsComplexData())
              {
                  if((v.GetComplexData())->IsWindow())
                      {
                          //Everything is fine, return without error.
                          return;
                      }
              }
          message = outsideMessage;
          message +="Wanted Window but got " + GetTypeName(v) + ": "+ v.GetString();
          SignalFatalError(message);
          
          break;
          
        case PEAT_TEXTOBJECT:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsTextObject())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted TextObject but got " + GetTypeName(v) + ": " + v.GetString();
            SignalFatalError(message);

            break;


        case PEAT_LABEL:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsLabel())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted Label but got " + GetTypeName(v) + ": " + v.GetString();
            SignalFatalError(message);

            break;
            


        case PEAT_TEXTBOX:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsTextBox())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted Text Box but got " + GetTypeName(v) + ": " + v.GetString();
            SignalFatalError(message);

            break;
            
        case PEAT_NETWORKCONNECTION:
             
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsNetworkConnection())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted network but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);

          break;
          
          
        case PEAT_PARALLELPORT:
             
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsParallelPort())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted parallelport but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);

          break;
          
        case PEAT_COMPORT:
             
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsComPort())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted ComPort but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);

          break;

        case PEAT_MOVIE:
            
            if(v.IsComplexData())
                {
                    if((v.GetComplexData())->IsMovie())
                        {
                            //Everything is fine, return without error.
                            return;
                        }
                }
            message = outsideMessage;
            message +="Wanted Movie but got " + GetTypeName(v) + ": "+ v.GetString();
            SignalFatalError(message);
            
            break;

            
        case PEAT_UNDEFINED:
		default:
            
            message = outsideMessage;
            message += string("Undefined type in PErrorAssert: ") + v.GetString();
            SignalFatalError(message);
            break;
        }

}




string PError::GetTypeName(Variant v)
{
	//Get the name of the type: if it a complex data type, extract the name from that structure.
    string typeName;
	if(v.IsComplexData())
        {
            typeName = (v.GetComplexData())->GetTypeName();
        }
	else
        {
            typeName = v.GetDataTypeName();
        }

	return typeName;
}





