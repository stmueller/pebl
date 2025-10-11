//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/platforms/sdl/PlatformImageBox.h
//    Purpose:    Contains SDL platform-specific Image interface
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
#ifndef __PLATFORMIMAGEBOX_H__
#define __PLATFORMIMAGEBOX_H__

#include "PlatformWidget.h"
#include "../../objects/PImageBox.h"
#include "../../base/PEBLObject.h"


#ifdef PEBL_OSX
#include "SDL.h"
#include "SDL_image.h"
#else
#include "SDL.h"
#include "SDL_image.h"
#endif


#include <string>
///
/// 
/// 


class PlatformImageBox: virtual public PImageBox, 
                        virtual public PlatformWidget
{
public:
  
    PlatformImageBox();
    virtual ~PlatformImageBox();
  
    virtual bool LoadImage(const std::string & imagefilename);

    virtual bool Draw();
    virtual std::string ObjectName() const{return "PlatformImageBox\n";};
    virtual void SetZoomX(pDouble x);
    virtual void SetZoomY(pDouble x);

protected:
    bool mTextureCreated;
    virtual std::ostream & SendToStream(std::ostream& out) const;

private:    

};


#endif
