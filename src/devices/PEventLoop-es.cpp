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

#include "PDevice.h"
#include "PEventQueue.h"
#include "../platforms/sdl/PlatformEventQueue.h"
#include "../base/FunctionMap.h"

#include "../apps/Globals.h"

#ifdef PEBL_EMSCRIPTEN
#include "PEventLoop-es.h"
//#include "../base/Evaluator-es.h"
#include "../base/Evaluator.h"
#include "emscripten.h"
#else
#include "PEventLoop.h"
#include "../base/Evaluator.h"
#endif
 
#include "../base/PComplexData.h"
#include "../base/PEBLObject.h"
#include "../base/PNode.h"
#include "../base/grammar.tab.hpp"

#include "../utility/Defs.h"
#include "../utility/PEBLUtility.h"

#include "../libs/PEBLEnvironment.h"
#include <iostream>

using std::flush;
using std::endl;

extern PlatformEventQueue * gEventQueue;
//extern PEventLoop * eloop;

//namespace PEventLoop
//{
//	PEventLoop *eloop = NULL;
//}

/// This is the standard PEventLoop constructor
PEventLoop::PEventLoop()
{

    char s[25];
    char* mTmp;
    sprintf(s,"%d",PEBLUtility::Round(PEBLUtility::RandomUniform()*100));
    mTmp = s;
    //cout << "CREATING AN EVENT LOOP " << mTmp << endl;
    //  eloop = this;
    mNumStates = 0;
    mIsLooping = false;
    mCallbackScheduled = false;
    mCallbackNodeStackSize = 0;
}

/// This is the standard pNode destructor
PEventLoop::~PEventLoop()
{
    //cout << "Dstructing veentloop " << mTmp << endl;
    // Standard Destructor

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
    mNumStates++;
    //Make a PNode representing the function. If the function-name is null, push a
    //null PNode onto the vector; this is a short-cut for an end-of-loop event.
    if(function != "")
        {

            //Store the function name for later use
            Variant fname = Variant(function.c_str(),P_DATA_FUNCTION);
            mFunctionNames.push_back(fname);

            //We don't store any node - we'll handle the function call manually
            mNodes.push_back(NULL);

        }
    else
        {

            mNodes.push_back(NULL);
            mFunctionNames.push_back(Variant(""));
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
void PEventLoop::RegisterEvent(DeviceState * state,
                               const std::string &  function,
                               Variant parameters)
{

    //    cout << "Regestering...mNumStates:" << mNumStates << endl;
    //    cout << mStates.size() << endl;
    //Add the state to the states list.
    mStates.push_back(state);
    mNumStates++;
    //Make a PNode representing the function. If the function-name is null, push a
    //null PNode onto the vector; this is a short-cut for an end-of-loop event.
    if(function != "")
        {

            //Store the function name for later use
            Variant fname = Variant(function.c_str(),P_DATA_FUNCTION);
            mFunctionNames.push_back(fname);

            //We don't store any node - we'll handle the function call manually
            mNodes.push_back(NULL);
        }
    else
        {
            mNodes.push_back(NULL);
            mFunctionNames.push_back(Variant(""));
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
    mFunctionNames.clear();
    mParameters.clear();
    mIsEvent.clear();
    mNumStates=0;
    mIsLooping = false;
    //    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Clearning:\n";
    //    cout << *this << endl;
}


/// The following method will initiate an event loop.  It will repetitively
/// cycle through each of the devices-events registered and determine if any are
/// satisfied.  Whenever one is satisfied, it will follow the directive of that
/// event.  It will continue until a STOPEVENTLOOP event is processed.

// For emscripten, this sets up the loop, then issues the loop command which reprograms itself as needed.
//
PEvent PEventLoop::Loop()
{

    mIsLooping = true;

    unsigned int i, result =0;
    PEvent returnval(PDT_UNKNOWN,0,0);

    //Enter a variable into the global variable map.  The loop will exit
    //when this is set false.
    myEval->gGlobalVariableMap.AddVariable("gKeepLooping", 1);

    //    cout <<"*****" <<myEval->gGlobalVariableMap.RetrieveValue("gKeepLooping") << "--"<< mStates.size() << std::endl;


    //Loop until gKeepLooping becomes false
    //In Emscripten, emscripten_sleep() yields to browser between iterations
    while(mNumStates > 0 &&
          (pInt)(myEval->gGlobalVariableMap.RetrieveValue("gKeepLooping")))
    {
        //Process one iteration of event checking
        returnval = Loop1();

#ifdef PEBL_ITERATIVE_EVAL
        //If Loop1() scheduled a callback, execute it now
        //The callback schedules 4 nodes initially, but lambdaNode pushes the entire function body
        //Execute until the callback completes (all scheduled nodes consumed)
        if(mCallbackScheduled)
        {
            //Save the node stack size before we scheduled the callback
            //We want to execute until we're back to this size (all callback nodes consumed)
            size_t targetNodeStackSize = mCallbackNodeStackSize;

            //Execute nodes until the callback completes
            //Continue until node stack is back to the size it was before we scheduled the callback
            while(!myEval->mNodeStack.empty() &&
                  myEval->mNodeStack.size() > targetNodeStackSize)
            {
                myEval->Evaluate1();  //Process one node from the stack
            }
        }
#endif

#ifdef PEBL_EMSCRIPTEN
        //Yield to browser to allow browser events to be processed
        emscripten_sleep(10);
#elif defined(PEBL_UNIX)
        //Unix: nanosleep for 100 microseconds to avoid burning CPU
        struct timespec a, b;
        a.tv_sec = 0;
        a.tv_nsec = 100000;  //100 microseconds
        nanosleep(&a, &b);
#elif defined(PEBL_WIN32)
        //Windows: Use SDL_Delay
        SDL_Delay(1);  //Sleep about 1 ms
#endif
    }

    //Clear the event loop when we're done
    Clear();

    return returnval;
}

/*
void LoopAsync(void* data)
{
    //cout << "async looping\n";
    //    PEventLoop* eeloop = (PEventLoop*)(data);
    //PEventLoop * eeloop = Evaluator::

    //cout << "Name: "  <<     Evaluator::mEventLoop->mTmp << endl;

    Evaluator::mEventLoop->Loop1();
}
*/


//This processes ONE iteration of the event loop.
//Checks all registered events once, schedules any matching callbacks, then returns.
//The caller (Loop) will execute the scheduled callbacks.
PEvent PEventLoop::Loop1()
{
    PEvent returnval(PDT_UNKNOWN,0,0);
    unsigned int result =0;
    bool matched = false;

    //Reset callback flag
    mCallbackScheduled = false;

    // Prime the event queue (process pending SDL events)
    gEventQueue->Prime();

            //Check event queue events FIRST (keyboard, mouse) so they have priority over timers
            for(int i = 0; i < mNumStates; i++)
                {
                    if(!mIsEvent[i])   //Skip non-event states (timers) in this first pass
                        continue;

                    //The test is for an event queue-type event.
                            // Note: 'events' contrast with 'states', handled later.
                            // These are devices which send events through the PEBL Event queue.
                            // So, if the current test is an 'event' state, we need to check the event queue.

                            //Only test the event if the queue is not empty.
                            if(!gEventQueue->IsEmpty())
                                {
                                    //Now, we only should test an event if it is the proper device type.

                                    if(gEventQueue->GetFirstEventType() == mStates[i]->GetDeviceType())
                                        {
                                            //Now, just test the device.
                                            //I don't think any devices support TestDevice currently.
                                            result = mStates[i]->TestDevice();
                                             
                                            if(result)
                                                {
                                                    returnval = gEventQueue->GetFirstEvent();


                                                    if(mFunctionNames[i].GetString() != "")  //Execute callback if function name exists
                                                        {
                                                            //Save node stack size before scheduling callback
                                                            mCallbackNodeStackSize = myEval->mNodeStack.size();

                                                            //Add the parameters, as a list, to the stack.
                                                            //Need to add the returnval (event) to the parameter list
                                                            //just like the recursive evaluator does in PEventLoop.cpp

                                                            Variant parlist = mParameters[i];

                                                            const PList *tmp = parlist.GetComplexData()->GetList();

                                                            PList * list = new PList(*tmp);
                                                            list->PushBack(Variant(returnval));

                                                            counted_ptr<PEBLObjectBase> list2 = counted_ptr<PEBLObjectBase>(list);
                                                            PComplexData * pcd = new PComplexData(list2);  // Heap allocation - will be managed by Variant

                                                            //Save node stack size before scheduling callback
                                                            mCallbackNodeStackSize = myEval->GetNodeStackDepth();

                                                            //Create a DataNode containing the parameter list
                                                            //This will be evaluated by PEBL_FUNCTION and push the list onto the stack
                                                            DataNode * paramsNode = new DataNode(Variant(pcd), "", -1);

                                                            //Create a DataNode for the function name
                                                            DataNode * funcNameNode = new DataNode(mFunctionNames[i], "", -1);

                                                            //Create PEBL_FUNCTION node with parameter DataNode as right child
                                                            //When PEBL_FUNCTION executes, it will evaluate paramsNode which pushes the parameter list
                                                            OpNode * functionCallNode = new OpNode(PEBL_FUNCTION, (PNode*)funcNameNode, (PNode*)paramsNode, "event-callback", -1);

                                                            //Mark that we scheduled a callback (for Loop() to execute)
                                                            mCallbackScheduled = true;

                                                            //Schedule nodes to execute the function and pop its return value

                                                            //Pop the callback's return value (we don't need it)
                                                            const OpNode * popResult = new OpNode(PEBL_STATEMENTS_TAIL1,NULL,NULL,"event-callback",-1);
                                                            myEval->NodeStackPush(popResult);

                                                            //Execute the function call (this will handle all stack management)
                                                            myEval->NodeStackPush(functionCallNode);

                                                        }
                                                    else  //If mNodes[i] is null, terminate
                                                        {

                                                            //cout << "Stopping the loop.  Donk.\n";
                                                            //We are aborting here.  no need to reprogram anything.
                                                            myEval->gGlobalVariableMap.AddVariable("gKeepLooping", 0);

                                                        }
                                                    matched = true;
                                                    break;
                                                }
                                        }
                                }

                            //cout << "END Event code\n";
                }

            //If no event queue match and queue not empty, pop the non-matching event
            if(!matched && !gEventQueue->IsEmpty())
            {
                gEventQueue->PopEvent();
            }

            //If no event queue match, check timer/state events (second pass)
            if(!matched)
            {
                for(int i = 0; i < mNumStates; i++)
                {
                    if(mIsEvent[i])   //Skip event states in this second pass
                        continue;

                    //this is where time events (wait) land.
                    //cout << "STATE type\n";
                    //mStates[i] isn't a device-type state.

                    //The test examines the device's state directly.

                    result = mStates[i]->TestDevice();
                                  if(result)

                                {
                                    //We need to create a 'dummy' event to use here.
                                    PEBL_DummyEvent pde;
                                    pde.value = mStates[i]->GetInterface();


                                    if(mStates[i]->GetDevice()->GetDeviceType() == PDT_TIMER)
                                        {

                                            returnval = PEvent(PDT_TIMER,0,0);
                                            returnval.SetDummyEvent(pde);

                                        }
                                    else
                                        {

                                            //If this was a time-check event, make a PDT_timer
                                            //time needs to go in  the 0 below.
                                            //returnval = PEvent(PDT_DUMMY,0,0);

                                            returnval = PEvent((PEBL_DEVICE_TYPE)pde.value,0,0);
                                            returnval.SetDummyEvent(pde);
                                        }
                                    //If mNodes[i] is null, terminate

                                    if(mFunctionNames[i].GetString() != "")  //Execute callback if function name exists
                                        {
                                            //Save node stack size before scheduling callback
                                            mCallbackNodeStackSize = myEval->mNodeStack.size();

                                            //Add the parameters, as a list, to the stack.
                                            //Need to add the returnval (event) to the parameter list
                                            //just like the recursive evaluator does in PEventLoop.cpp

                                            Variant parlist = mParameters[i];

                                            const PList *tmp = parlist.GetComplexData()->GetList();

                                            PList * list = new PList(*tmp);
                                            list->PushBack(Variant(returnval));

                                            counted_ptr<PEBLObjectBase> list2 = counted_ptr<PEBLObjectBase>(list);
                                            PComplexData * pcd = new PComplexData(list2);  // Heap allocation - will be managed by Variant

                                            //Save node stack size before scheduling callback
                                            mCallbackNodeStackSize = myEval->GetNodeStackDepth();

                                            //Create a DataNode containing the parameter list
                                            //This will be evaluated by PEBL_FUNCTION and push the list onto the stack
                                            DataNode * paramsNode = new DataNode(Variant(pcd), "", -1);

                                            //Create a DataNode for the function name
                                            DataNode * funcNameNode = new DataNode(mFunctionNames[i], "", -1);

                                            //Create PEBL_FUNCTION node with parameter DataNode as right child
                                            //When PEBL_FUNCTION executes, it will evaluate paramsNode which pushes the parameter list
                                            OpNode * functionCallNode = new OpNode(PEBL_FUNCTION, (PNode*)funcNameNode, (PNode*)paramsNode, "event-callback", -1);

                                            //Mark that we scheduled a callback (for Loop() to execute)
                                            mCallbackScheduled = true;

                                            //Schedule nodes to execute the function and pop its return value

                                            //Pop the callback's return value (we don't need it)
                                            const OpNode * popResult = new OpNode(PEBL_STATEMENTS_TAIL1,NULL,NULL,"event-callback",-1);
                                            myEval->NodeStackPush(popResult);

                                            //Execute the function call (this will handle all stack management)
                                            myEval->NodeStackPush(functionCallNode);

                                        }
                                    else
                                        {


                                            myEval->gGlobalVariableMap.AddVariable("gKeepLooping", 0);

                                        }
                                    matched = true;
                                    break;

                                } else
                                {

                                    //cout << "state test is 0; \n";
                                }
                }  //End second for loop (timer/state events)
            }  //End if(!matched)

    // After checking all events, pop matched event queue events
    if(matched && returnval.GetType() != PDT_TIMER)
    {
        //Pop the matched event from queue
        gEventQueue->PopEvent();
    }

    // Return the event (or UNKNOWN if no match)
    // The caller (Loop) will call this again to continue checking events
    return returnval;
}


//Overload of the << operator
std::ostream & operator <<(std::ostream & out, const PEventLoop & loop )
{

    out << "PEBL Event Loop:" << flush;

    return out;
}
