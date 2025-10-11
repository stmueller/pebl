//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/PList.cpp
//    Purpose:    Contains a simple list class
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
#include "PList.h"
#include "Variant.h"
#include "../utility/PError.h"
#include "PEBLObject.h"
//#include <list>
#include <vector>
#include <map>
#include <stdio.h>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
//using std::list;
//using std::list;
using std::vector;
using std::multimap;
using std::pair;
using std::ostream;

///Standard Constructor
PList::PList(): PEBLObjectBase(CDT_LIST)
{

    //    mCDT = CDT_LIST;
}


PList::PList(int reservedsize): PEBLObjectBase(CDT_LIST)
{

      mList.reserve((int)(reservedsize));
}


///Copy constructor.  Should make a deep copy.
PList::PList(const PList & tmpList): PEBLObjectBase(CDT_LIST)
{


    //mCDT=CDT_LIST;
    const std::vector<Variant> * tmp = tmpList.GetList();

    //Make an item-by-item copy of list into mList.
    //Couldn't figure out how to use the stl copy algorithm to
    //do this, so I will do it by hand.
    ///Defunct code attempt: Using STL copy algorithm
    //  std::copy(tmp->begin(), tmp->end(), mList.begin());

    std::vector<Variant>::const_iterator p;
    for(p = tmp->begin(); p!=tmp->end(); p++)
        {
            mList.push_back(Variant(*p));
        }

}

///Cloner.  This makes a item-for-item deep copy and returns
///a pointer to it.
PList * PList::Clone()
{

    cout << "Cloning list\n";
    PList * newlist = new PList();

    //Make an item-by-item copy of list into mList.
    //Couldn't figure out how to use the stl copy algorithm to
    //do this, so I will do it by hand.
    ///Defunct code attempt: Using STL copy algorithm
    //  std::copy(tmp->begin(), tmp->end(), mList.begin());

    std::vector<Variant>::iterator p;
    for(p = mList.begin(); p!=mList.end(); p++)
        {
            // This does not work.  Need to make the contained list and then embed it.
            //       newlist->push_back(Variant(*p));
        }

    return newlist;
}

///Standard Destructor
PList::~PList()
{

}


std::vector<Variant>::iterator  PList::Begin()
{
    return mList.begin();
}

std::vector<Variant>::iterator  PList::End()
{
    return mList.end();
}


//const versions of above
std::vector<Variant>::const_iterator  PList::Begin() const
{
    return mList.begin();
}

std::vector<Variant>::const_iterator  PList::End() const
{
    return mList.end();
}



//void PList::PushFront(const Variant & v)
//{
    //    mList.push_front(v);
    //}

//void PList::PopFront()
//{
//    mList.pop_front();
//}

void PList::PushBack(const Variant &  v)
{

    //Let's try to reduce time overhead by expanding using reserve to 50% larger than the current list.
    //which is really a std::vector
   if(mList.capacity() < mList.size()+1)
   {
    //increase reserve capacity by 25%
       mList.reserve((int)(mList.size()*1.25));
   }
    mList.push_back(v);
}

void PList::PopBack()
{
    mList.pop_back();
}


//returns the first element of the list
Variant PList::First()
{
    if(mList.empty())
        PError::SignalFatalError("Attempting to get First() element of empty list.");

    return mList.front();

}



//This returns the nth item in a list (first item is 1).
Variant PList::Nth(unsigned int n)
{
    //This is messed up--need a NULL variant type
    //or error codes etc.
    if(mList.size() < n)
        PError::SignalFatalError("Attempting to get Nth element element of  too-short list.");
    if(n==0)
        PError::SignalFatalError("Attempting to get 0th element element of  too-short list.");

    //std::vector<Variant>::iterator p = mList[n];
    //.begin();
    //    for(unsigned int i = 1; i< n; i++,p++);
    //return *p;

    //Note: this is 1-indexed, so 1 is 0, etc.
    return mList[n-1];
}



#if 0
unsigned int PList::Length() const
{
    unsigned int length = 0;

    std::vector<Variant>::const_iterator p;
    p = mList.begin();

    //Print out the first item, so comma-ing works out ok.
    while( p != mList.end())
        {
            length++;
            p++;
        }

    return length;
}
#endif



void PList::SetElement(unsigned int n,Variant value)
{

    //This is messed up--need a NULL variant type
    //or error codes etc.
    if(mList.size() < n)
        PError::SignalFatalError("Attempting to get Nth element element of  too-short list.");
    if(n==0)
        PError::SignalFatalError("Attempting to get 0th element element of  too-short list.");
    mList[n-1] = value;

}

Variant PList::Last()
{
    //This is messed up--need a NULL variant type
    //or error codes etc.
    if(mList.empty())
        PError::SignalFatalError("Attempting to get last element element of empty list.");


    std::vector<Variant>::iterator p = mList.end();
    p--;
    return *p;
}


///This sorts the member list by the values in another list.
///It does this by making a multiset, which is implicitly sorted,
///and then making the map back into a list.

counted_ptr<PEBLObjectBase> PList::SortBy(const PList & key)
{
    //This is the map that the variant pairs will be put into
    multimap<Variant, Variant> sortMap;

    //Iterators for the key/data lists.  Woe to he who allows
    //these to be of different sizes.
    std::vector<Variant>::const_iterator keyIterator  = key.Begin();
    std::vector<Variant>::const_iterator dataIterator = Begin();

    //Go through the entire data list, inserting pairs into the map.
    while(dataIterator != End())
        {
            sortMap.insert( pair<Variant,Variant>(*keyIterator, *dataIterator));
            keyIterator++;     //move through both lists.
            dataIterator++;
        }


    //Now, sortmap should be in order of keyIterator. Make
    //a new PList to put the data in.
    PList * newList = new PList();

    multimap<Variant, Variant>::iterator i=sortMap.begin();

    while(i != sortMap.end())
        {
            newList->PushBack(i->second);
            i++;
        }
    return counted_ptr<PEBLObjectBase>(newList);
}


//std::vector<Variant> * PList::GetList()
//{
//
//    return &mList;
//}

ostream & operator <<(std::ostream & out, const PList & list )
{
    list.SendToStream(out);
    return out;
}



///This is used by the friend function of PComplex Data <<, which
/// calls the inheritable function SendToStream().  It just spits out the items of the list
///with spaces as separators.  Since the items are variants, it uses the overloaded <<
///operator for variants.
ostream & PList::SendToStream(ostream& out) const
{
    std::vector<Variant>::const_iterator p;
    p = mList.begin();

     out << "[" ;
    //out << this->Length() << ":";

    //Print out the first item, so comma-ing works out ok.
    if( p != mList.end())
        {
            out << *p ;
            p++;
            while(p != mList.end())
                {
                    //send each item to stream with space and comma in between.
                    out << ", " << *p  << flush;
                    p++;
                }
        }
    out << "]" ;

    return out;
}
