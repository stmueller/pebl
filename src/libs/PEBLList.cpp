//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/libs/PEBLList.cpp
//    Purpose:    List Processing Function Library for PEBL
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
#include "PEBLList.h"
#include "../base/Variant.h"
#include "../base/PComplexData.h"
#include "../base/PList.h"

#include "../utility/PEBLUtility.h"
#include "../utility/PError.h"
#include "../utility/rc_ptrs.h"

#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <iostream>

using std::ostream;
using std::cerr;
using std::endl;
using std::list;
using std::vector;
using PEBLUtility::RandomUniform;
using std::string;


Variant PEBLList::Shuffle (Variant v)
{
    //v is a list.  v[1] should be have a list in it.
     PList * plist = v.GetComplexData()->GetList();

     Variant v1 = plist->First(); //plist->PopFront();

    //v1 should be a list.
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [Shuffled(<list>)]:  ");


    PList * dataList = (PList*)(v1.GetComplexData()->GetObject().get());

    //Now, make a keylist of random numbers, the length of datalist.
    PList keyList;// = PList();

    for(unsigned int i = 0; i < dataList->Length(); i++)
        {
            keyList.PushBack(RandomUniform());
        }


    //Now, sort by the key list
    counted_ptr<PEBLObjectBase> newList = dataList->SortBy(keyList);
    PComplexData * PCD =(new PComplexData(newList));

    Variant tmp = Variant(PCD);
    delete PCD;
    PCD=NULL;
    return tmp;

  }


/// Repeat(value, number)
/// This function returns a list with <value> repeated <number> times
/// The expression is evaluated before it is repeated.
Variant PEBLList::Repeat (Variant v)
{
   //v is a list; v[1] is the object to repeat (can be anything)
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();


    //v[2] is the number of repeats: should be an integer.
    PError::AssertType(plist->Nth(2), PEAT_INTEGER, "Argument error in second parameter of function [Repeat(<object>,<integer>)]:  ");

    int number = plist->Nth(2);// plist->PopFront();


    PList * returnList = new PList();
    for(int i=0;i<number;i++)
        {
            returnList->PushBack(v1);
        }

    counted_ptr<PEBLObjectBase> tmplist = counted_ptr<PEBLObjectBase>(returnList);
    PComplexData * tmpPCD= (new PComplexData(tmplist));
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;

}

/// This creates a list functionally, rather than syntactically using the [] operators.
Variant PEBLList::List(Variant v)
{
    return v;
}


/// RepeatList(list, n)
/// This creates a new list consisting of the original list
/// repeated n times.
Variant PEBLList::RepeatList(Variant v)
{
   //v[1] should be a list.  Just extract the iterators.
    PList * plist = v.GetComplexData()->GetList();



    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [RepeatList(<list>, <int>)]:  ");

    PList * arglist = (PList*)(v1.GetComplexData()->GetObject().get());
    vector<Variant>::iterator p;
    vector<Variant>::iterator pstart = arglist->Begin();
    vector<Variant>::iterator pend   = arglist->End();


    //v[2] is the number of repeats: should be a number.
    PError::AssertType(plist->Nth(2), PEAT_INTEGER, "Argument error in second parameter of function [RepeatList(<list>, <int>)]:  ");
    int number = plist->Nth(2);// plist->PopFront();


    //There is now an iterator p, and and end to compare it to, and
    //an iterator to the pstart for resetting.
    //Make a new list to return.

    PList * returnList = new PList();

    for(int i=0; i<number; i++)
        {
            p=pstart;
            while (p != pend)
                {
                    returnList->PushBack(*p);
                    p++;
                }
        }

    counted_ptr<PEBLObjectBase> tmp2 = counted_ptr<PEBLObjectBase>(returnList);
    PComplexData * tmpPCD= (new PComplexData(tmp2));
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;


}



/// Sequence constructs a sequence of numbers between the low and high,
/// with step-size increments between.  Attempts to stay in integers when
/// the low and step are integers.

Variant PEBLList::Sequence(Variant v)
{
    //v is a list; v[1] is the bottom of the sequence
     PList * plist = v.GetComplexData()->GetList();

    PError::AssertType(plist->First(), PEAT_NUMBER, "Argument error in first parameter of function [Sequence(<first>, <last>, <step>)]");
    Variant start = plist->First();// plist->PopFront();

    //v[2] is the last number in the sequence
    PError::AssertType(plist->Nth(2), PEAT_NUMBER, "Argument error in second parameter of function [Sequence(<first>, <last>, <step>)]:  ");
    Variant end = plist->Nth(2);// plist->PopFront();

    //v[3] is the increment step of the sequence.
    PError::AssertType(plist->Nth(3), PEAT_NUMBER, "Argument error in second parameter of function [Sequence(<first>, <last>, <step>)]:  ");
    Variant step = plist->Nth(3);// plist->PopFront();


    //Make sure that the step is not equal to 0--we will never end if it is.
    if(step == Variant(0))
        {
            PError::SignalFatalError("Step size in Sequence(<start>, <end>, <step>) is 0.");
        }

    Variant tolerance = .00000001;

    PList * returnList = new PList();
    //returnList->Clear();

    Variant current;

    //We need to operate differently if step is positive vs. negative.

    if(step > Variant(0))
        {
            //If step is positive, end better be greater than start.
            if(end < start)
                {
                    PError::SignalFatalError("For positive <step>s, <end> must be greater than <start> in [Sequence(<start>, <end>, <step>)]");
                }

            //Step is positive, continue while less-than end
            for(current = start; current <= end + tolerance; current = current + step)
                {
                    returnList->PushBack(current);
                }
        }
    else
        {
            //If step is negative, end better be smaller than start.
            if(end > start)
                {
                    PError::SignalFatalError("For negative <step>s, <end> must be less than <start> in [Sequence(<start>, <end>, <step>)]");
                }

            //Step is negative, continue while greater-than end.
            for(current = start; current >= end - tolerance ; current = current + step)
                {
                    returnList->PushBack(current);
                }
        }

    counted_ptr<PEBLObjectBase> tmp2 = counted_ptr<PEBLObjectBase>(returnList);
    PComplexData * tmpPCD= (new PComplexData(tmp2));
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;


}


/// RepeatExpression(<name_of_function>, <list_of_parameters>, <number>)
/// This function returns a list <number> long that is produced by the
/// return value of <name_of_function>, evaluated with <list_of_parameters>
/// as parameters. <name_of_function> can be either a standard library function
/// or a user-defined function.
///  This is most useful for generating randomized
/// values, but can also be used to created complex designs by storing
/// global variables.
Variant PEBLList::RepeatExpression (Variant v)
{
   //v[1] should be a list
    /*   counted_ptr<PList> plist = (v.GetComplexData())->GetList();
         Variant v1 = plist->First(); plist->PopFront();
         counted_ptr<PList> myList = v1.GetComplexData()->GetList();
    */

    PError::SignalFatalError("Function [RepeatExpression] not implemented");
    return Variant(true);
}



/// This takes two lists as parameters, and returns a nested
/// list of lists that includes the full counterbalancing of a and b.
/// Use cautiously; this gets mxn large
Variant PEBLList::DesignFullCounterbalance (Variant v)
{
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    //v[1] should be a list.  Just extract the iterators.
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [DesignFullCounterbalance(<list>, <list>)]:  ");

    PList * arglist1 = (PList*)(v1.GetComplexData()->GetObject().get());
    vector<Variant>::iterator p1 = arglist1->Begin();
    vector<Variant>::iterator p1end = arglist1->End();


    Variant v2 = plist->Nth(2);// plist->PopFront();
    //v[1] should be a list.  Just extract the iterators.
    PError::AssertType(v2, PEAT_LIST, "Argument error in second parameter of function [DesignFullCounterbalance(<list>, <list>)]:  ");

    PList * arglist2 = (PList*)(v2.GetComplexData()->GetObject().get());
    vector<Variant>::iterator p2start = arglist2->Begin();
    vector<Variant>::iterator p2;
    vector<Variant>::iterator p2end   = arglist2->End();

    //There are now 2 list iterators: p1 and p2, and two ends to compare them to,
    //and an iterator to the p2start for resetting.
    //Make a new list to return.

    PList * returnList = new PList();
    PList * tmpList = NULL;
    counted_ptr<PEBLObjectBase> tmpobj;
    Variant tmpVariant;
    PComplexData * tmpPCD;
    while(p1 != p1end)
        {
            p2 = p2start;
            while (p2 != p2end)
                {
                    ///Make sublist with the pair in it.
                    tmpList = new PList();
                    tmpList->PushBack(*p1);
                    tmpList->PushBack(*p2);

                    tmpobj = counted_ptr<PEBLObjectBase>(tmpList);

                    tmpPCD = (new PComplexData(tmpobj));
                    tmpVariant = Variant(tmpPCD);
                    delete tmpPCD;
                    tmpPCD=NULL;

                    //Put the sublist in the outer list
                    returnList->PushBack(tmpVariant);
                    p2++;
                }

            p1++;
        }

    tmpobj = counted_ptr<PEBLObjectBase>(returnList);
    tmpPCD= (new PComplexData(tmpobj));
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;

}


///This function takes a single list, and returns a list of
///all pairs, excluding the pairs that have two of the same item.
/// CrossFactorWithoutDuplicates([a,b,c]) = [[a,b],[a,c],[b,a],[b,c],[c,a],[c,b]]
/// To achieve the same effect but include the duplicates, use DesignFullCounterBalance(x,x)
Variant PEBLList::CrossFactorWithoutDuplicates(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();

   //v[1] should be a list.  Just extract the iterators.

    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [CrossFactorWithoutDuplicates(<list>)]:  ");
    PList * arglist = (PList*)(v1.GetComplexData()->GetObject().get());
    vector<Variant>::iterator p1 = arglist->Begin();
    vector<Variant>::iterator p2;
    vector<Variant>::iterator pstart = p1;
    vector<Variant>::iterator pend = arglist->End();

    //There are now 2 list iterators: p1 and p2,
    //and an iterator to the p2start for resetting.
    //Make a new list to return.

    PList * returnList = new PList();
    PList * tmpList;
    counted_ptr<PEBLObjectBase> tmpObj;
    Variant tmpVariant;
    PComplexData * tmpPCD;
    int i1=0;
    int i2=0;
    while(p1 != pend)
        {
            p2 = pstart;
            i2=0;
            while (p2 != pend)
                {
                    if(i1 != i2)
                        {
                            ///Make sublist with the pair in it.
                            tmpList = new PList();
                            tmpList->PushBack(*p1);
                            tmpList->PushBack(*p2);


                            tmpObj = counted_ptr<PEBLObjectBase>(tmpList);

                            tmpPCD = (new PComplexData(tmpObj));
                            tmpVariant = Variant(tmpPCD);
                            delete tmpPCD;
                            tmpPCD=NULL;
                            //Put the sublist in the outer list
                            returnList->PushBack(tmpVariant);
                        }
                    p2++;
                    i2++;
                }

            p1++;
            i1++;
        }

    tmpObj = counted_ptr<PEBLObjectBase>(returnList);
    tmpPCD= new PComplexData(tmpObj);
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;

}

Variant PEBLList::Rotate(Variant v)
{
    //v[1] should be a list.  Just extract the iterators.
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [Rotate(<list>, <n>)]:  ");

    Variant v2 = plist->Nth(2);// plist->PopFront();
    PError::AssertType(v2, PEAT_INTEGER, "Argument error in second parameter of function [Rotate(<list>, <n>)]:  ");


    PList * arglist = v1.GetComplexData()->GetList();

    vector<Variant>::iterator pi = arglist->Begin();
    vector<Variant>::iterator pStart = pi;
    vector<Variant>::iterator pListStart = pi;
    vector<Variant>::iterator pEnd = arglist->End();


    //startpoint is the 0-based index of which item to start the new list with.
    int rotation = (int)v2;
    
    long int length =  arglist->Length();

    //We need to find a startpoint as a positive number between 0 and length.
    //The normal % and mod functions will not produce this.

    int rem1 = rotation % length;

    if(rem1<0)
        {
            rem1 = rem1 + (length);
        }
    pListStart += (rem1);


    
    //Make a new list, starting with pListStart, and moving to
    //the end, and then attaching pStart to pListStart.
    
    PList * returnList = new PList();
    counted_ptr<PEBLObjectBase> tmpObj;
    Variant tmpVariant;
    
    //Adjust the iterator to the start of the list.
    pi=pListStart;
    while(pi != pEnd)
        {
            
            //Put the item on the list
            returnList->PushBack(*pi);
            pi++;
        }
    
    pi=pStart;
    while(pi != pListStart)
        {
            
            //Put the item on the list
            returnList->PushBack(*pi);
            pi++;
        }
    
    tmpObj = counted_ptr<PEBLObjectBase>(returnList);
    PComplexData * tmpPCD= (new PComplexData(tmpObj));
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;
    
    
}

/// This returns the number of items in the list.
Variant PEBLList::Length (Variant v)
{

    PList * plist = v.GetComplexData()->GetList();

    //v[1] should be a list
    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [Length(<list>)]:  ");

    PList * myList = (PList*)(v1.GetComplexData()->GetObject().get());
    return Variant((long unsigned int)(myList->Length()));
}


/// This function returns the first variant in the PList
/// passed in as an argument.
Variant PEBLList::First(Variant v)
{
   //v[1] should be a list

    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [First(<list>)]:  ");

    PList * myList = (PList*)(v1.GetComplexData()->GetObject().get());

    return Variant(myList->First());
}
Variant PEBLList::Second(Variant v)
{
   //v[1] should be a list

    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [Second(<list>)]:  ");

    PList * myList = (PList*)(v1.GetComplexData()->GetObject().get());
    return myList->Nth(2);
}

Variant PEBLList::Third(Variant v)
{
   //v[1] should be a list

    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [Second(<list>)]:  ");

    PList * myList = (PList*)(v1.GetComplexData()->GetObject().get());
    //vector<Variant>::iterator p = myList->Begin();
    return myList->Nth(3);
}

Variant PEBLList::Fourth(Variant v)
{
   //v[1] should be a list

    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [Fourth(<list>)]:  ");

    PList * myList = (PList*)(v1.GetComplexData()->GetObject().get());
    return myList->Nth(4);
}

Variant PEBLList::Fifth(Variant v)
{
   //v[1] should be a list

    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [Fifth(<list>)]:  ");

    PList * myList = (PList*)(v1.GetComplexData()->GetObject().get());
    return myList->Nth(5);
}

//This returns the nth element of the list, counting from 1.
//if it is longer than the list, it returns the last item.
Variant PEBLList::Nth (Variant v)
{
    PList * plist = v.GetComplexData()->GetList();


    //v[1] should be a list
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [Nth(<list>, <integer>)]:  ");
    PList * myList = (PList*)(v1.GetComplexData()->GetObject().get());

    //v[2] should be an integer
    PError::AssertType(plist->Nth(2), PEAT_INTEGER, "Argument error in second parameter of function [Nth(<list>, <integer>)]:  ");
    int n = plist->Nth(2);// plist->PopFront();
    return Variant(myList->Nth(n));
}


Variant PEBLList::Last(Variant v)
{
   //v[1] should be a list
     PList * plist = v.GetComplexData()->GetList();

     Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [Last(<list>)]:  ");

    PList * myList = (PList*)(v1.GetComplexData()->GetObject().get());

    return Variant(myList->Last());
}

/// This takes two lists and returns a single list
///
Variant PEBLList::Merge (Variant v)
{
    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First();// plist->PopFront();
    //v[1] should be a list.  Just extract the iterators.
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [Merge(<list>, <list>)]:  ");
    PList * tmpList = (PList*)(v1.GetComplexData()->GetObject().get());
    vector<Variant>::iterator p1start = tmpList->Begin();
    vector<Variant>::iterator p1 = p1start;
    vector<Variant>::iterator p1end = tmpList->End();



    Variant v2 = plist->Nth(2);// plist->PopFront();
    //v[1] should be a list.  Just extract the iterators.
    PError::AssertType(v2, PEAT_LIST, "Argument error in second parameter of function [Merge(<list>, <list>)]:  ");

    tmpList =  (PList*)(v2.GetComplexData()->GetObject().get());
    vector<Variant>::iterator p2start = tmpList->Begin();
    vector<Variant>::iterator p2 = p2start;
    vector<Variant>::iterator p2end   = tmpList->End();

    //There are now 2 list iterators: p1 and p2, and two ends to compare them to,
    //and an iterator to the p2start for resetting.
    //Make a new list to return.

    PList * returnList = new PList();

    Variant tmpVariant;

    //Add the first list.
    while(p1 != p1end)
        {
            returnList->PushBack(*p1);
            p1++;
        }

    //Add the second list.
    while (p2 != p2end)
        {
            returnList->PushBack(*p2);
            p2++;
        }

    counted_ptr<PEBLObjectBase> tmpObj = counted_ptr<PEBLObjectBase>(returnList);
    PComplexData * tmpPCD= (new PComplexData(tmpObj));

    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;



}



/// This adds an element to a list
/// Given Merge([a,b,c],d), will return [a,b,c,d]
Variant PEBLList::Append (Variant v)
{

    PList * plist = v.GetComplexData()->GetList();


    Variant v1 = plist->First(); //plist->PopFront();
    //v[1] should be a list.  Just extract the iterators.
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [Append(<list>, <list>)]:  ");

    PList * tmpList = (PList*)(v1.GetComplexData()->GetObject().get());
    vector<Variant>::iterator p1 = tmpList->Begin();
    vector<Variant>::iterator p1end = tmpList->End();

    Variant v2 = plist->Nth(2); //plist->PopFront();
    //v[2] can be anything.


    //Make a new list to return.
    PList * returnList = new PList();

    Variant tmpVariant;

    //Add the first list.
    while(p1 != p1end)
        {
            returnList->PushBack(*p1);
            p1++;
        }

    //Append v2.
    returnList->PushBack(v2);
    counted_ptr<PEBLObjectBase> tmpObj = counted_ptr<PEBLObjectBase>(returnList);
    PComplexData * tmpPCD= (new PComplexData(tmpObj));
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;

}

Variant PEBLList::SetElement(Variant v)
{
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First();
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [SetElement(<list>,<index>,<listitem>)]:  ");


    Variant v2 = plist->Nth(2);
    PError::AssertType(v2, PEAT_INTEGER, "Argument error in second parameter of function [SetElement(<list>,<index>,<listitem>)]:  ");


    Variant v3 = plist->Nth(3);
    //No need to check v3 for type.

    PList * pl = v1.GetComplexData()->GetList();
    pl->SetElement((int)v2,v3);

    return Variant(true);
}



// This adds an element to a list
/// Given Merge([a,b,c],d), will return [a,b,c,d]
Variant PEBLList::PushOnEnd (Variant v)
{

    PList * plist = v.GetComplexData()->GetList();

    Variant v1 = plist->First(); //plist->PopFront();
    //v[1] should be a list.  Just extract the iterators.
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [PushOnEnd(<list>, <listitem>)]:  ");

    //Get the list object, and iterators to it:
    PList * tmpList = (PList*)(v1.GetComplexData()->GetObject().get());

    //Get the other object:
    Variant v2 = plist->Nth(2);// plist->PopFront();
    //v[2] can be anything.

    tmpList->PushBack(v2);
    //v should still contain the list, so return it now.
    return v1;
}




///This just sorts the list.
Variant PEBLList::Sort (Variant v)
{
   //v[1] should be a list
    PList * plist = v.GetComplexData()->GetList();


    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [Sort(<list>)]:  ");

    PList * dataList = (PList*)(v1.GetComplexData()->GetObject().get());

    counted_ptr<PEBLObjectBase> newList = dataList->SortBy(*dataList);
    PComplexData * PCD = (new PComplexData(newList));
    Variant tmp = Variant(PCD);
    delete PCD;
    PCD=NULL;
    return tmp;

}


///This sorts the list by another list.
Variant PEBLList::SortBy (Variant v)
{
    //v[1] should be a list
     PList * plist = v.GetComplexData()->GetList();

     Variant v1 = plist->First();// plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [SortBy(<list>, <list>)]:  ");

    PList * dataList = (PList*)(v1.GetComplexData()->GetObject().get());

    //v[2] should be the keys which the list should be sorted by.
    Variant v2 = plist->Nth(2);// plist->PopFront();
    PError::AssertType(v2, PEAT_LIST, "Argument error in second parameter of function [SortBy(<list>, <list>)]:  ");
    PList * keyList = (PList*)(v2.GetComplexData()->GetObject().get());

    counted_ptr<PEBLObjectBase> newList = dataList->SortBy(*keyList);
    PComplexData * PCD = ( new PComplexData(newList));
    Variant tmp = Variant(PCD);
    delete PCD;
    PCD=NULL;
    return tmp;

}



Variant PEBLList::IsMember (Variant v)
{
    PList * plist = v.GetComplexData()->GetList();

    //v[1] should be an item to check for membership.
    Variant v1 = plist->First();// plist->PopFront();


    Variant v2 = plist->Nth(2);  //  plist->PopFront();
    PError::AssertType(v2, PEAT_LIST, "Argument error in second parameter of function [IsMember(<list>, <item>)]:  ");

    PList * tmpList  = (PList*)(v2.GetComplexData()->GetObject().get());
    vector<Variant>::iterator p = tmpList->Begin();
    vector<Variant>::iterator pEnd = tmpList->End();

    while(p != pEnd)
        {
            if(*p == v1)
                return Variant(true);
            p++;
        }
    return Variant(false);
}

Variant PEBLList::RemoveDuplicates (Variant v)
{
   //v[1] should be a list
    /*    counted_ptr<PList> plist = (v.GetComplexData())->GetList();
    Variant v1 = plist->First(); plist->PopFront();
    counted_ptr<PList> myList = v1.GetComplexData()->GetList();
    */
    PError::SignalFatalError("Function [RemoveDuplicates] not implemented");
    return Variant(true);
}

Variant PEBLList::MakeMap (Variant v)
{
   //v[1] should be a list
    /*    counted_ptr<PList> plist = (v.GetComplexData())->GetList();
    Variant v1 = plist->First(); plist->PopFront();
    counted_ptr<PList> myList = v1.GetComplexData()->GetList();
    */
    PError::SignalFatalError("Function [MakeMap] not implemented");
    return Variant(true);
}





/// v[1] MUST be a list of lists, and each of the sub-lists
/// must be of the same length.
Variant PEBLList::Transpose (Variant v)
{
   //v[1] should be a list
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in function [Transpose(<list-of-lists>)]:  ");

    PList * tmpList = (PList*)(v1.GetComplexData()->GetObject().get());
    vector<Variant>::iterator pstart = tmpList->Begin();
    vector<Variant>::iterator p;
//    vector<Variant>::iterator pend   = tmpList->End();

    unsigned long int outerLength = tmpList->Length();
    unsigned long int innerLength=0;

    //Create a vector of iterators to elements of each sublist.
    std::vector<vector<Variant>::iterator> listIterators(outerLength,pstart);

    p = pstart;

    for(unsigned int j = 0; j < outerLength; j++)
        {
            PError::AssertType(*p, PEAT_LIST, "Every item in Transpose(<list-of-lists>) must be a list. ");

            tmpList =  (PList*)((*p).GetComplexData()->GetObject().get());
            if(j == 0)
                {
                    innerLength = tmpList->Length();
                }
            else
                {
                    if(tmpList->Length() != innerLength)
                        {
                            PError::SignalFatalError("All sublists must be of the same length in Transpose(<list-of-lists>).");
                        }
                }

            //assign the iterator to the first element of the sublist.
            listIterators[j] = tmpList->Begin();
            p++;
        }


    //Now, everything is in the clear, so transpose.

    PComplexData * tmpPCD;
    PList * returnList = new PList;
    Variant tmpVariant;
    counted_ptr<PEBLObjectBase>  tmpObj;


    for(unsigned int i = 0; i < innerLength; i++)
        {
            tmpList = new PList();

            for(unsigned int j = 0; j < outerLength; j++)
                {
                    tmpList->PushBack(*listIterators[j]); //Grab the item from the list
                    listIterators[j]++;                   //iterate
                }
            //The first item of each list has been added to tmpList, so add
            //it to returnList.
            tmpObj = counted_ptr<PEBLObjectBase>(tmpList);
            tmpPCD = (new PComplexData(tmpObj));
            tmpVariant = Variant(tmpPCD);
            returnList->PushBack(tmpVariant);
            delete tmpPCD;
            tmpPCD=NULL;
        }

    tmpObj = counted_ptr<PEBLObjectBase>(returnList);
    tmpPCD= (new PComplexData(tmpObj));

    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;

    return tmp;
}

/// This makes a list out of a sublist of a list.  Or something like that.
Variant PEBLList::SubList(Variant v)
{

  //v[1] should be a list.  Just extract the iterators.
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [SubList(<list>, <int>, <int>)]:  ");

    PList * tmpList = (PList*)(v1.GetComplexData()->GetObject().get());
    vector<Variant>::iterator pstart = tmpList->Begin();
    vector<Variant>::iterator p;
    vector<Variant>::iterator pend   = tmpList->End();


    PError::AssertType(plist->Nth(2), PEAT_INTEGER, "Argument error in second parameter of function [SubList(<list>, <int>, <int>)]:  ");
    int start = plist->Nth(2); //plist->PopFront();

    PError::AssertType(plist->Nth(3), PEAT_INTEGER, "Argument error in third  parameter of function [SubList(<list>, <int>, <int>)]:  ");
    int end   = plist->Nth(3); //plist->PopFront();


    //Check to see if everything is in bounds.
    if(start < 1
       || end > (int)(tmpList->Length())
       || start > end )
        {
            Variant message;
            message =   Variant("[SubList] tried to extract items ")+
                Variant((int)start)+ Variant(" to ") + Variant((int)end) +
                Variant(" of a ")  + Variant((int)(tmpList->Length())) + Variant( " item list.");

            PError::SignalFatalError(message);
        }



    //There is now an iterator p, and an end to compare it to, and
    //an iterator to the pstart for resetting.
    //Make a new list to return.

    PList * returnList = new PList();
    Variant tmpVariant;
    counted_ptr<PEBLObjectBase> tmpObj;
    //Start at the beginning of the list. Keep count; if the count is
    //between the two boundaries, add it to the list.
    p=pstart;
    int i = 0;
    while (p != pend)
        {
            i++;
            //If i is big enough, consider adding it to the list
            if(i>=start)
                {
                if( i <=end)  //But only if it isn't too big
                    {
                        returnList->PushBack(*p);
                    }
                else         //If it is too big, get out of the while loop.
                    {
                        break;
                    }
                }
            p++;
        }
    tmpObj = counted_ptr<PEBLObjectBase>(returnList);
    PComplexData * tmpPCD = (new PComplexData(tmpObj));
    Variant tmp = Variant(tmpPCD);
    delete tmpPCD;
    tmpPCD=NULL;
    return tmp;

}



//This will remove the nth item from a list
Variant PEBLList::Remove(Variant v)
{

    PError::SignalFatalError("Function [Remove] not implemented");
    return Variant(1);
}


Variant PEBLList::ListToString(Variant v)
{

   //v[1] should be a list.  Just extract the iterators.
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [ListToString(<list>, <sep>, <pre>, <post>)]:  ");


    PList * tmpList = (PList*)(v1.GetComplexData()->GetObject().get());

    vector<Variant>::iterator pstart = tmpList->Begin();
    vector<Variant>::iterator p;
    vector<Variant>::iterator pend   = tmpList->End();

     Variant sep1 = "";
     Variant pre = "";
     Variant post = "";


    if(plist->Length()>=2) sep1 = plist->Nth(2);
    if(plist->Length()>=3) pre = plist->Nth(3);
    if(plist->Length()>=4) post = plist->Nth(4);

    //cout << "processing List to string: " << sep1 << "|" << pre <<"|" << post << "|" << endl;
     Variant out="";
     Variant sep = "";
     p = pstart;
     while(p!=pend)
     {

         out = out + sep + pre+ Variant(*p) + post;

          if(p == pstart)
              sep =sep1;

              p++;
     }
  return out;
}

//This will add a pre and post string to each element of the list.
//This should non-destrtuctivley cerate a new list with pre/post values on each item
Variant PEBLList::ModList(Variant v)
{
   //v[1] should be a list.  Just extract the iterators.
    PList * plist = v.GetComplexData()->GetList();
    Variant v1 = plist->First(); //plist->PopFront();
    PError::AssertType(v1, PEAT_LIST, "Argument error in first parameter of function [ModList(<list>, <pre>, <post>)]:  ");


    PList * tmpList = (PList*)(v1.GetComplexData()->GetObject().get());
    PList * newList = new PList(*tmpList);

    vector<Variant>::iterator pstart = newList->Begin();
    vector<Variant>::iterator p;
    vector<Variant>::iterator pend   = newList->End();

     Variant pre = "";
     Variant post = "";


    if(plist->Length()>=2) pre = plist->Nth(2);
    if(plist->Length()>=3) post = plist->Nth(3);

     p = pstart;
     while(p!=pend)
     {
          *p = (pre + *p + post);
           p++;
     }
    counted_ptr<PEBLObjectBase> newList2 = counted_ptr<PEBLObjectBase>(newList);

    PComplexData * PCD =(new PComplexData(newList2));
    Variant tmp = Variant(PCD);
    delete PCD;
    PCD=NULL;
    return tmp;

}

