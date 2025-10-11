//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/utility/PEBLPath.h
//    Purpose:    Utility class storing search path and searching for files there
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
#ifndef __PEBLPATH_H__
#define __PEBLPATH_H__

#include <list>
#include <string>
using std::string;

class Variant;

/// This class stores a series of file system locations, which
/// you can use to search through with the FindFile method to locate
/// files.  Paths will be searched in the order in which they are added,
/// and the first match will be used.

class PEBLPath
{
public:

    PEBLPath();
    ~PEBLPath();

    //This sets up default pathnames.  Called by const char constructor; takes the
    //binary filename and uses it on platforms where this indicates location of
    //support files.
    void Initialize(std::list<std::string>);


    void AddToPathList(const string & s);
    std::string FindFile(const string &  filename);

    const string  MergePathAndFile(const string &  path, const string & file);
    bool IsDirectory(const string & pathname);


    friend std::ostream & operator << (std::ostream & out, const PEBLPath & path);
    std::ostream & Print(std::ostream & out) const;
private:

    ///This stores a list of paths that are combined with filenames and searched, in
    ///the order they were added.
    std::list<string> mPathList;


};

#endif
