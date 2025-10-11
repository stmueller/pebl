//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/utility/CountedPounter.h
//    Purpose:    Contains definition for a slim counted pointer template class
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
#ifndef __RC_PTRS_H
#define __RC_PTRS_H

/*

This class is heavily based on a reference-counted pointer implementation by
Mark E.  It included the below copyright statement, as well as a counted pointer
for use with arrays.

  Copyright 1998, 1999
  Mark E. (snowball3@usa.net)
  http://snowball.digitalspace.net/cpp/

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose is hereby granted without fee,
  provided that the above copyright notice appear in all copies and
  that both that copyright notice and this permission notice appear
  in supporting documentation.

  Mark E. makes no representations about the suitability of
  this software for any purpose. It is provided "as is" without
  express or implied warranty.

*/




/*
  This header contains the classes counted_ptr and counted_array_ptr.
  These classes implement reference-counted smart pointers that automatically
  deletes the pointer it contains when no longer needed
  i.e. (reference count drops to zero)
*/
#include <iostream>
#include <stddef.h>

template <class X>
class counted_ptr
{
    //
    // Public typedefs
    //
public:
    typedef X element_type;
    typedef X* pointer_type;
    typedef size_t size_type;

 public:
    explicit counted_ptr(X* p=0) : ptr(p)
    {
        //std::cout <<"INITIATING CPTR "<<std::endl;


        count=new size_type(1);
    }


    counted_ptr (const counted_ptr<X> &r)
    {
        //std::cout <<"CASTING CPTR ["<< r.ptr << "]"<<std::endl;
        //std::cout << *(r.ptr)<< std::endl;
        ptr=r.ptr;
        count=r.count;
        acquire();
    }


    ~counted_ptr() {
        release();
    }


    counted_ptr& operator= (const counted_ptr<X> &r)
    {
        if (this != &r)
            {
                release();
                ptr = r.ptr;
                count = r.count;
                acquire();
            }
        return *this;
    }

    X& operator* () const { return *ptr; }
    X* get () const { return ptr; }
    X* operator-> () const { return ptr; }

    bool unique () const
    {
        return *count==1;
    }

    void PrintCounts()
    {
        std::cout << "Object "<< ptr<<" has "<< *count << " copies" <<std::endl;
    }

protected:
    X* ptr;
    size_type *count;

protected:

    void acquire()
    {
        //std::cout << ">>>>ACQUIRING\n";
        (*count) += 1;
        //PrintCounts();
    }


    void release()
    {
        //debugging printouts:

        if (count)
            {
                //std::cout << "<<<<<RELEASING\n";
                //std::cout << ptr <<":----:"<<  std::endl;
                (*count)--;
                //PrintCounts();

                if((*count)==0)
                    {              
                        //std::cout << "PTR is empti; Deleting: "<< ptr<<std::endl;
                        delete ptr;
                        delete count;
                        ptr = NULL;
                        count = NULL;
                    }
            }
    }
};

#endif

