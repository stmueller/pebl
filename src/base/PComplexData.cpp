//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/PComplexData.h
//    Purpose:   Contains base class for complex data, held by the Variant Class
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
#include "PComplexData.h"

#include <iostream>

#include "../utility/PError.h"
#include "../utility/rc_ptrs.h"


using std::ostream;
using std::cerr;
using std::endl;

///
/// This class is the base class that all complex data types
/// inherit from. A pointer to a PComplexData can be stored in
/// A Variant.

PComplexData::PComplexData()
{


}


PComplexData::PComplexData( counted_ptr <PEBLObjectBase > object )
{
    mPEBLObject = object;
}

PComplexData::~PComplexData()
{
    //cout << "Deleting complex data\n";
}


///This is a generic copy constructor
PComplexData::PComplexData(const PComplexData & pcd )
{

    mPEBLObject = pcd.GetObject();

}

///This is a generic copy constructor
PComplexData::PComplexData(const PComplexData *pcd )
{

    mPEBLObject = pcd->GetObject();

}



//Overload of the << operator
ostream & operator << ( ostream & out, const PComplexData & pcd )
{
    pcd.SendToStream( out );
    return out;
}


//This prints out stuff to a stream.
ostream & PComplexData::SendToStream( ostream & out ) const
{
    out << *mPEBLObject;
    return out;
}


ComplexDataType PComplexData::GetType() const
{
    if(mPEBLObject.get() != NULL) return mPEBLObject->GetType();
    else return CDT_UNDEFINED;
}


counted_ptr<PEBLObjectBase> PComplexData::GetObject() const
{
    return mPEBLObject;

}



counted_ptr<PEBLObjectBase> PComplexData::GetPEBLObject() const
{
    return mPEBLObject;

}
std::string PComplexData::GetTypeName() const
{

  switch (GetType() )
  {
    case CDT_LIST:
      return "Complex Data: List";

    case CDT_ENVIRONMENT:
      return "Complex Data: Environment";

    case CDT_WINDOW:
      return "Complex Data: Window";

    case CDT_COLOR:
      return "Complex Data: Color";

    case CDT_FONT:
      return "Complex Data: Font";

    case CDT_LABEL:
      return "Complex Data: Label";

    case CDT_TEXTBOX:
      return "Complex Data: TextBox";

    case CDT_IMAGEBOX:
      return "Complex Data: ImageBox";

    case CDT_KEYBOARD:
      return "Complex Data: Keyboard";

    case CDT_FILESTREAM:
      return "Complex Data: FileStream";

  case CDT_AUDIOOUT:
      return "Complex Data: Audio Out";

  case CDT_DRAWOBJECT:
      return "Complex Data: Drawing Object";

  case CDT_THICKLINE:
      return "Complex Data: Thick Line";

  case CDT_NETWORKCONNECTION:
      return "Complex Data: Network Connection";

  case CDT_PARALLELPORT:
      return "Complex Data: Parallel Port";

  case CDT_COMPORT:
      return "Complex Data: Com Port";

  case CDT_JOYSTICK:
      return "Complex Data: Joystick";
  case CDT_MOVIE:
      return "Complex Data: Movie";

  case CDT_CUSTOMOBJECT:
      return "Complex Data: Custom Object";

  case CDT_UNDEFINED:
  default:
      return "Complex Data: Undefined";
  }

}



void PComplexData::SetProperty( std::string prop, Variant v )
{

    mPEBLObject->SetProperty(prop,v);
}


Variant PComplexData::GetProperty( std::string prop )const
{
    return mPEBLObject->GetProperty(prop);
}


bool PComplexData::PropertyExists( std::string prop )const
{

    enum ObjectValidationError tmp =  mPEBLObject->ValidateProperty(prop);
    bool success =  tmp == OVE_SUCCESS;

    return success;
}



bool PComplexData::IsList() const
{
  return GetType()  == CDT_LIST;
}


bool PComplexData::IsEnvironment() const
{
  return GetType() == CDT_ENVIRONMENT;
}


bool PComplexData::IsWidget() const
{
  if ( ( GetType() == CDT_WINDOW ) || ( GetType() == CDT_LABEL ) || ( GetType() == CDT_IMAGEBOX )
       || (GetType()==CDT_THICKLINE)
       || ( GetType() == CDT_TEXTBOX ) || ( GetType() == CDT_DRAWOBJECT ) || ( GetType() == CDT_CANVAS ) || GetType() == CDT_MOVIE || GetType() == CDT_CUSTOMOBJECT)
         return true;
  else
    return false;
}


bool PComplexData::IsWindow() const
{
    return GetType() == CDT_WINDOW;
}


bool PComplexData::IsColor() const
{
  return GetType() == CDT_COLOR;
}


bool PComplexData::IsFont() const
{
    return GetType() == CDT_FONT;
}

bool PComplexData::IsTextObject() const
{
  return ( GetType() == CDT_TEXTBOX ) || ( GetType() == CDT_LABEL );
}

bool PComplexData::IsLabel() const
{
  return GetType() == CDT_LABEL;
}

bool PComplexData::IsTextBox() const
{
  return GetType() == CDT_TEXTBOX;
}



bool PComplexData::IsImageBox() const
{
  return GetType() == CDT_IMAGEBOX;
}


bool PComplexData::IsJoystick() const
{
  return GetType() == CDT_JOYSTICK;
}


bool PComplexData::IsKeyboard() const
{
  return GetType() == CDT_KEYBOARD;
}

bool PComplexData::IsFileStream() const
{
  return GetType() == CDT_FILESTREAM;
}

bool PComplexData::IsAudioOut() const
{
  return GetType() == CDT_AUDIOOUT;
}


bool PComplexData::IsDrawObject() const
{
  return GetType() == CDT_DRAWOBJECT;
}


bool PComplexData::IsThickLine() const
{
    //cout << "Checking thickline:" << GetType() << ":" << GetTypeName() << endl;
  return GetType() == CDT_THICKLINE;
}

bool PComplexData::IsCanvas() const
{
  return GetType() == CDT_CANVAS;
}


bool PComplexData::IsNetworkConnection() const
{
  return GetType() == CDT_NETWORKCONNECTION;
}


bool PComplexData::IsParallelPort() const
{
  return GetType() == CDT_PARALLELPORT;
}

bool PComplexData::IsComPort() const
{
  return GetType() == CDT_COMPORT;
}


bool PComplexData::IsMovie() const
{
  return GetType() == CDT_MOVIE;
}


bool PComplexData::IsCustomObject() const
{
    //cout << "checking custom object" << GetType() << "|" << GetTypeName() << endl;

  return GetType() == CDT_CUSTOMOBJECT;
}

PList * PComplexData::GetList()const
{

    if(IsList()) return dynamic_cast<PList*>(mPEBLObject.get());
    else return NULL;

}
