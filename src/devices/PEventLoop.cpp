//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/devices/PEventLoop.cpp
//    Purpose:    Primary generic timer event device
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
#include "PEventLoop.h"
#include "PDevice.h"
#include "PEventQueue.h"
#ifdef PEBL_VALIDATOR
#include "../platforms/validator/PlatformEventQueue.h"
#else
#include "../platforms/sdl/PlatformEventQueue.h"
#endif
#include "../base/FunctionMap.h"

#include "../apps/Globals.h"

#ifdef PEBL_ITERATIVE_EVAL
#include "PEventLoop-es.h"
#include "../base/Evaluator-es.h"
#else
#include "PEventLoop.h"
#include "../base/Evaluator.h"
#endif

#ifdef PEBL_EMSCRIPTEN
#include "emscripten.h"
#endif


#include "../base/PComplexData.h"
#include "../base/PEBLObject.h"
#include "../base/grammar.tab.hpp"

#include "../utility/Defs.h"

#include "../libs/PEBLEnvironment.h"
#include <iostream>

using std::flush;
using std::endl;

extern PlatformEventQueue * gEventQueue;

/// This is the standard PEventLoop constructor
PEventLoop::PEventLoop()
{
    //cout << "Creating event loop\n";

}

/// This is the standard pNode destructor
PEventLoop::~PEventLoop()
{
    // Standard Destructor
    //cout << "Destroying venet loop\n";
}



/// This function will 'register' a specific event that will be tested for
/// in an event-loop.  This allows multiple tests to be registered (time limit, key press, etc.)
/// and tested within a fairly tight compiled loop, without the user having to write his/her own 
/// interpreted event loop.
void PEventLoop::RegisterState(DeviceState * state,
                               const std::string & function,
                               Variant parameters)
{
    
    //Add the state to the states list.
    mStates.push_back(state);
    //Make a PNode representing the function. If the function-name is null, push a
    //null PNode onto the vector; this is a short-cut for an end-of-loop event.
    if(function != "")
        {

            //Get the node associated with the function name.
            //Note that node will be the right node of a PEBL_FUNCTION node
            //The namenode goes on the left.
            Variant fname = Variant(function.c_str(),P_DATA_FUNCTION);
            DataNode * namenode = new DataNode(fname,"user-generated function",-1);                                        

            //On the right, we need a node representing the lambda function.

            //we need the arglist too.

            PNode * node = Evaluator::mFunctionMap.GetFunction(function);

            PNode * arglist = ((OpNode*)node)->GetLeft();

            PNode * fnode = new OpNode(PEBL_FUNCTION, namenode, arglist, "user-generated", -1);


            mNodes.push_back(fnode);


        }
    else
        {
            
            mNodes.push_back(NULL);
        }
    //Add parameters, for use later.
    //parameters is passed in as a pointer, and must be attached to a counted pointer
    //if we are to maintain it once the original function is gone.


    mParameters.push_back(parameters);
    mIsEvent.push_back(false);
}

/// This function will 'register' a specific event that will be tested for
/// in an event-loop.  This allows multiple tests to be registered (time limit, key press, etc.)
/// and tested within a fairly tight compiled loop, without the user having to write his/her own 
/// interpreted event loop.

/// This method takes over ownership of the DeviceState, and is responsible for cleaning it up 
/// when finished.
void PEventLoop::RegisterEvent(DeviceState * state, const std::string &  function, Variant parameters)
{

 
    //Add the state to the states list.
    mStates.push_back(state);
    //Make a PNode representing the function. If the function-name is null, push a
    //null PNode onto the vector; this is a short-cut for an end-of-loop event.
    if(function != "")
        {

            //Get the node associated with the function name.
            //Note that node will be the right node of a PEBL_FUNCTION node
            //The namenode goes on the left.
            Variant fname = Variant(function.c_str(),P_DATA_FUNCTION);
            DataNode * namenode = new DataNode(fname,"user-generated function",-1);                                        

            //On the right, we need a node representing the lambda function.
            
            PNode * node = Evaluator::mFunctionMap.GetFunction(function);

            PNode * arglist =  ((OpNode*)node)->GetLeft();

            PNode * fnode = new OpNode(PEBL_FUNCTION, namenode, arglist, "user-generated", -1);



            mNodes.push_back(fnode);
        }
    else
        {
            mNodes.push_back(NULL);
        }
    //Add parameters, for use later.
    
    mParameters.push_back(parameters);
    mIsEvent.push_back(true);
}



void PEventLoop::Clear()
{

    //When the eventloop is Clear()ed, the states should be deleted one-by-one,
    //to avoid a memory leak.
    std::vector<DeviceState*>::iterator i = mStates.begin();
    while(i != mStates.end())
        {
            if(*i) delete *i;
            i++;
        }

    mStates.clear();
    mNodes.clear();
    mParameters.clear();
    mIsEvent.clear();
    
}


/// The following method will initiate an event loop.  It will repetitively
/// cycle through each of the devices-events registered and determine if any are 
/// satisfied.  Whenever one is satisfied, it will follow the directive of that
/// event.  It will continue until a STOPEVENTLOOP event is processed.


PEvent PEventLoop::Loop()
{

    Evaluator * myEval = new Evaluator();
    unsigned int i, result =0;
    PEvent returnval(PDT_UNKNOWN,0,0);

    //Enter a variable into the global variable map.  The loop will exit
    //when this is set false.
    myEval->gGlobalVariableMap.AddVariable("gKeepLooping", 1);

    //    cout <<"*****" <<myEval->gGlobalVariableMap.RetrieveValue("gKeepLooping") << "--"<< mStates.size() << std::endl;

    //while loop stops when gKeepLooping turns false or there are no more states to check for.
    //    bool stop = (mStates.size()==0) ||
    //        ((pInt)(myEval->gGlobalVariableMap.RetrieveValue("gKeepLooping"))==(pInt)0);

    
    while(myEval->gGlobalVariableMap.RetrieveValue("gKeepLooping"))
        {
            //Output time for each event loop cycle.
            //std::cerr <<  "time:"<< SDL_GetTicks() << std::endl;

            //At the beginning of a cycle, the event queue has not yet been primed.
            gEventQueue->Prime();


            //Scan through each test in the mstates vector
            for(i = 0; i < mStates.size(); i++)
                {
                    //cout << i << "/"<<mStates.size() << ":"<<  mNodes[i] << "\n";

                    if(mIsEvent[i])   //The test is for an event queue-type event.
                        {

                            
                            // Note: 'events' contrast with 'states', handled later.
                            // These are devices which send events through the PEBL Event queue.
                            // So, if the current test is an 'event' state, we need to check the event queue.


                            //Only test the event if the queue is not empty.
                            if(!gEventQueue->IsEmpty())
                                {
                                    
                                    //Now, we only should test an event if it is the proper device type.
                                    //cout << "state/devicetype ["<<i<<"]";
                                    //cout << mStates[i]->GetDeviceType() << "---" << std::flush;
                                    //cout <<*(mStates[i]) << endl;
                                    
                                    //cout << "Event: " << PDT_WINDOW_RESIZE <<"|"<< gEventQueue->GetFirstEventType() << endl;
                                    //cout << gEventQueue->GetFirstEventType() << "<>" << mStates[i]->GetDeviceType() <<endl;

                                    if(gEventQueue->GetFirstEventType() == mStates[i]->GetDeviceType())
                                        {
                                            
                                            
                                            //Now, just test the device.
                                            result = mStates[i]->TestDevice();
                                            //cout << "result:" << result << endl;
                                            
                                            if(result)
                                                {
                                                    returnval = gEventQueue->GetFirstEvent();
                                                    

                                                    // The test was successful.  
                                                    
                                                    if(mNodes[i])  //Execute mNodes
                                                        {
                                                            //Add the parameters, as a list, to the stack.

                                                            //need to add the variant-based returnval
                                                            //to the parameters though.  It should be at the END

                                                            Variant parlist = mParameters[i];

                                                            //We need to make a deep copy here,tsktsktsk.

                                                            const PList *tmp = parlist.GetComplexData()->GetList();
                                                            PList * list = new PList(*tmp);

                                                            list->PushBack(Variant(returnval));
                                                            counted_ptr<PEBLObjectBase> list2 = counted_ptr<PEBLObjectBase>(list);
                                                            PComplexData * pcd = new PComplexData(list2);
                                                                
                                                            myEval->Push(Variant(pcd));
                                                            myEval->CallFunction((OpNode*)mNodes[i]);

                                                            //The return value is currently unused.
                                                            // we could use it to remove an event dynamically.                                        
                                                            Variant outcome = myEval->Pop();

                                                            //cout<< "OUTCOME: " << outcome << endl;
                                                            
                                                            if(outcome=="<REMOVE>"| outcome=="<remove>")
                                                                {

                                                                    //remove element i from consideration.
                                                                    //it needs to be removed from:

                                                                    //mStates[i] (the test)
                                                                    //mNodes[i] (the executed code when test succeeds)
                                                                    //and mParameters[i] (the parameters)
                                                                    //and mIsEvent[] (a flag)
                                                                    //but these are vectors, and not easily removed. We
                                                                    //alsoneed to be wary of removing the last/only state.
                                                                    mStates.erase(mStates.begin()+i);
                                                                    mNodes.erase(mNodes.begin()+i);
                                                                    mParameters.erase(mParameters.begin()+i);
                                                                    mIsEvent.erase(mIsEvent.begin()+i);

                                                                    //stop loop in case we have ended.
                                                                    if(mStates.size()<=0)
                                                                        {
                                                                            myEval->gGlobalVariableMap.AddVariable("gKeepLooping", 0);
                                                                        }
                                                                }

                                                        }
                                                    else       //If mNodes[i] is null, terminate
                                                        {

                                                            myEval->gGlobalVariableMap.AddVariable("gKeepLooping", 0);
                                                            
                                                        }
                                            goto end; 
                                                }
                                        }
                                }
                            
                        }
                    else
                        {
                            //mStates[i] isn't a device-type state.

                            //The test examines the device's state directly.
                            //I don't think any devices support TestDevice currently,
                            //except for keyboard states and timer stats.

                            result = mStates[i]->TestDevice();
                            if(result)

                                {
                                    //We need to create a 'dummy' event to use here.
                                    PEBL_DummyEvent pde;

                                    pde.value = mStates[i]->GetState(mStates[i]->GetInterface());
                                    
                                    if(mStates[i]->GetDevice()->GetDeviceType() == PDT_TIMER)
                                        {
                                    
                                            returnval = PEvent(PDT_TIMER,0,0);
                                            returnval.SetDummyEvent(pde);

                                        } 
                                    else if(mStates[i]->GetDevice()->GetDeviceType()==PDT_KEYBOARD)
                                        {
                                            //This is a keyboard state; different form a keypress event!
                                            //cout << "Keyboard state:" << mStates[i]->GetInterface()<< endl;
                                            
                                            pde.value = (PEBL_Keycode)(mStates[i]->GetInterface());


                                            returnval = PEvent(PDT_DUMMY,0,0);
                                            returnval.SetDummyEvent(pde);

                                        }else
                                        {

                                            //If this was a time-check event, make a PDT_timer
                                            //time needs to go in  the 0 below.
                                            //returnval = PEvent(PDT_DUMMY,0);

  
                                            returnval = PEvent((PEBL_DEVICE_TYPE)pde.value,0,0);
                                            returnval.SetDummyEvent(pde);
                                        }
                                    //If mNodes[i] is null, terminate
 
                                    if(mNodes[i])
                                        {
                                            
                                            //Add the parameters, as a list, to the stack.
                                            
                                            //need to add the variant-based returnval
                                            //to the parameters though.  It should be at the END
                                            
                                            Variant parlist = mParameters[i];
                                            //We need to make a deep copy here,
                                            //or the appending will stay around until later. tsktsktsk.
                                            const PList *tmp = parlist.GetComplexData()->GetList();
                                            PList * list = new PList(*tmp);

                                            list->PushBack(Variant(returnval));
                                            counted_ptr<PEBLObjectBase> list2 = counted_ptr<PEBLObjectBase>(list);
                                            PComplexData * pcd = new PComplexData(list2);

                                            myEval->Push(Variant(pcd));
                                            myEval->CallFunction((OpNode*)mNodes[i]);
                                            
                                            //The return value is currently unused.
                                            // we could use it to remove an event dynamically.                                        
                                            Variant outcome = myEval->Pop();
                                                            
                                            
                                            if(outcome=="<REMOVE>")
                                                {
                                                    //remove element i from consideration,
                                                    //it needs to be removed from:
                                                    
                                                    //mStates[i] (the test)
                                                    //mNodes[i] (the executed code when test succeeds)
                                                    //and mParameters[i] (the parameters)
                                                    //and mIsEvent[] (a flag)
                                                    //but these are vectors, and not easily removed. We
                                                    //alsoneed to be wary of removing the last/only state.
                                                    mStates.erase(mStates.begin()+i);
                                                    mNodes.erase(mNodes.begin()+i);
                                                    mParameters.erase(mParameters.begin()+i);
                                                    mIsEvent.erase(mIsEvent.begin()+i);
                                                    
                                                    //stop loop in case we have ended.
                                                    if(mStates.size()<=0)
                                                        {
                                                            myEval->gGlobalVariableMap.AddVariable("gKeepLooping", 0);
                                                         
                                                        }
                                                }
                                        }
                                    else
                                        {

                                            myEval->gGlobalVariableMap.AddVariable("gKeepLooping", 0);

                                        }
                                    goto end;                                    
                                    
                                }
                        }
                end:;
                    //cout << std::endl;
                }


            //end:
            //Get rid of the top item in the event queue
            gEventQueue->PopEvent();

            //Sleep to avoid burning CPU, if the gSleepEasy variable is set.
            //Uses platform-specific sleep via PlatformTimer (nanosleep on Unix, SDL_Delay on Windows, etc.)
            if(myEval->gGlobalVariableMap.Exists("gSleepEasy") )
                {
                    if(myEval->gGlobalVariableMap.RetrieveValue("gSleepEasy"))
                    {
                       PEBLEnvironment::myTimer.Sleep(1);  // 1ms sleep
                    }
                }

           //recompute the stopping criterion
           if(mStates.size()==0) 
               myEval->gGlobalVariableMap.AddVariable("gKeepLooping",0);
           //!(myEval->gGlobalVariableMap.RetrieveValue("gKeepLooping"));
        }

    delete myEval;
    return returnval;
}


//Overload of the << operator
std::ostream & operator <<(std::ostream & out, const PEventLoop & loop )
{

    out << "PEBL Event Loop: " << flush;
    loop.Print(out);
    return out;

}


void PEventLoop::Print(std::ostream & out) const
{
    out << " ---------------\n";
    out << "Number of states:" <<mStates.size() << endl;

}
