//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/Evaluator-es.cpp
//    Purpose:    Defines an class that can evaluate PNodes (Iterative Evaluator)
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003--2017 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//    ARCHITECTURE NOTE - Iterative Evaluator for Emscripten:
//
//    This is the ITERATIVE evaluator implementation. It uses manual stack
//    management (mNodeStack and mStack) instead of C++ call stack recursion to
//    evaluate the PEBL abstract syntax tree (AST). Operations are split into
//    "head" and "tail" phases (e.g., PEBL_ADD -> PEBL_ADD_TAIL), with the
//    evaluator loop managing execution order explicitly.
//
//    WHY THIS EXISTS:
//    This evaluator was specifically designed for Emscripten/WebAssembly builds
//    that use the Asyncify feature. The recursive evaluator (Evaluator.cpp) has
//    a hard recursion depth limit of 1 for PEBL user-defined recursive functions
//    when compiled with Asyncify, crashing at depth >= 2 with "index out of
//    bounds" errors. This occurs because Asyncify transforms the call stack to
//    enable pause/resume operations, breaking the recursive evaluator's function
//    call mechanism.
//
//    This iterative evaluator avoids C++ call stack recursion entirely, using
//    manual stack management that is compatible with Asyncify transformations.
//    Testing confirms it handles arbitrary recursion depth correctly on
//    Emscripten.
//
//    IMPLEMENTATION:
//    A single Evaluate1() loop processes nodes from mNodeStack. Each operation
//    type pushes child nodes and "tail" operations onto mNodeStack, with results
//    stored in mStack. This transforms recursive tree traversal into iterative
//    loop execution, avoiding C++ function call depth issues.
//
//    The PEBL_ITERATIVE_EVAL macro controls which evaluator is compiled.
//    See src/apps/Globals.h for the selection logic.
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


#include "Evaluator-es.h"
#include "PNode.h"
#include "grammar.tab.hpp"
#include "VariableMap.h"
#include "Variant.h"
#include "PComplexData.h"   
#include "PList.h"
#include "FunctionMap.h"
#include "../utility/PEBLUtility.h"
#include "../utility/PError.h"
#include "../utility/rc_ptrs.h"
#include "../utility/Defs.h"

#include "../objects/PCustomObject.h"
#include "../apps/Globals.h"
#include "../devices/PEventLoop-es.h"

#include <iostream>
#include <string>
#include <strstream>
#include <math.h>
#include <algorithm>

#undef PEBL_DEBUG_PRINT
//#define PEBL_DEBUG_PRINT 1

using std::cout;
using std::endl;
using std::flush;
//using std::list;
using std::string;
using std::vector;


Evaluator::Evaluator():
    mStackMax(10000),
    mScope("Base Scope")
{
#ifdef PEBL_DEBUG_PRINT 
    cout << "Creating Evaluator: " << mScope << endl;
#endif


    mEventLoop = new PEventLoop();

    gCallStack.Push(gEvalNode);
}
 

Evaluator::Evaluator(Variant & stacktop, string scope):
    //    mCallStack(callstack),
    mStackMax(10000),
    mScope(scope)
{
#ifdef PEBL_DEBUG_PRINT 
    cout << "Creating Evaluator: " << mScope << endl;
#endif

    //add everything in callstack onto mCallStack
    //mCallStack = callstack;

    mEventLoop = new PEventLoop();

    //Push the current evalnode onto the stack, if
    //it exists.
    if(gEvalNode)
        {
            gCallStack.Push(gEvalNode);

        }
    //Initialize the evaluator scope with a variant which is a list of variables
    Push(stacktop);
}


Evaluator::~Evaluator()
{
#ifdef PEBL_DEBUG_PRINT
    cout << "Deleting Evaluator: " << mScope << endl;
#endif

    //NOTE: gCallStack is a GLOBAL variable shared across all evaluator instances.
    //We should NOT clear it in the destructor, as it may contain entries from other
    //evaluators that are still running. Each function call properly pushes/pops
    //its own entry via PEBL_LAMBDAFUNCTION/PEBL_FUNCTION_TAIL_LIBFUNCTION.
    //
    //while( gCallStack.Size())
    //    gCallStack.Pop();


}

bool Evaluator::Evaluate1(const PNode * node)
{

#ifdef PEBL_EMSCRIPTEN
    
    if(node == NULL) PError::ExitQuietly("Trying to evaluate null node\n");
#else
    
    if(node == NULL) PError::SignalFatalError("Trying to evaluate null node\n");
#endif
    //Set up the globally-accessible structure to allow
    //better error reporting.  Only change it if the new node's
    //line number is greater than -1; if not, it is a PEBL-generated
    //node that won't give very good information.
    if(node->GetLineNumber() > -1)
        gEvalNode = node;



#ifdef PEBL_DEBUG_PRINT 
    cout << "Line: " << node->GetLineNumber() << endl;
    cout << "PDP::Type: " << node->GetType() << endl;
#endif

    if(node->GetType() ==  PEBL_OP_NODE)
        {
            return Evaluate1((OpNode*)node);
        }
    else if (node->GetType() ==  PEBL_DATA_NODE)
        {
            return Evaluate1((DataNode*)node);
        }
    else
        {
#ifdef PEBL_DEBUG_PRINT 
            cout << "ERROR IN GENERIC EVALUATOR::EVALUATE" << endl;
#endif
            return false;
        }

    return true;
}

void Eval1(void*)
{
    myEval->Evaluate1();
}


bool Evaluator::Evaluate1()
{
    const PNode * node =  mNodeStack.top();

#ifdef PEBL_DEBUG_PRINT 
    cout << "---------------async--a--------\n";
#endif


    mNodeStack.pop();

    return Evaluate1(node);
}


///  This method evaluates OpNodes

bool Evaluator::Evaluate1(const OpNode * node)
{

    int numargs = -1;
    if(node == NULL) PError::SignalFatalError("Trying to evaluate null node\n");
    //Set up the globally-accessible structure to allow
    //better error reporting.
    if(node->GetLineNumber() > -1)
        gEvalNode = node;

#ifdef PEBL_DEBUG_PRINT
    cout << "---------------async--b--------Evaluating OpNode ["<< node->GetOp() << "] of Type: " << node->GetOpName() << "------------\n";

#endif



    switch(node->GetOp()) 
        {
            
        case NULL:
            break;

            /******************************
              Handled
            ***********************************/
            
        case PEBL_ASSIGN:
            {
                //The left node should contain a DataNode of Variant type Variable.
                //The right node should contain an expression that evaluates to a number
                //that should be assigned to the datanode.
                
                
                const PNode * node2 = node->GetRight();
                //Evaluate(node2);
                const OpNode * tail = new OpNode(PEBL_ASSIGN_TAIL,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());


                const PNode * variablenode = node->GetLeft();

                mNodeStack.push(variablenode);
                mNodeStack.push(tail);
                mNodeStack.push(node2);

            }

            break;

            case PEBL_ASSIGN_TAIL:
                {
                    //The evaluated expression is now at the top of the stack.
                    //Leave it there, because this statement should return that value; but assign
                    //it to v2.
                    Variant v2 = mStack.top();

                    const PNode * node1 = mNodeStack.top();
                    mNodeStack.pop();
                
                    //Extract the variable name from the node.
                    Variant v1=((DataNode*)node1)->GetValue();


#ifdef PEBL_DEBUG_PRINT
                    cout << "Initial Variable Name: [" << v1.GetVariableName() << "]" << endl;
#endif

                    //Get the name of property being 
                    string property =v1.GetVariablePropertyName();

                    //Add the variable name/value pair to the appropriate map structure
                    if(v1.IsLocalVariable())
                        {

                            if(property == "")
                                {
                                    mLocalVariableMap.AddVariable(v1.GetVariableName(), v2);
                                }
                            else
                                {
                                    //otherwise get the object from the variable store 
                                    //and set its property.

                                    Variant v3 = mLocalVariableMap.RetrieveValue(v1.GetVariableBaseName());
                                
                                    PComplexData * pcd = v3.GetComplexData();
                                    if(pcd != NULL)  
                                        {
                                            pcd->SetProperty(property, v2);
                                        }
                                }


                        }
                    else
                        {

                            if(property == "")
                                {
                                    gGlobalVariableMap.AddVariable(v1.GetVariableName(),v2);
                                }
                            else
                                {
                                    //otherwise get the object from the variable store 
                                    //and set its property.
                                    Variant v3 = gGlobalVariableMap.RetrieveValue(v1.GetVariableBaseName());
                                    PComplexData * pcd = v3.GetComplexData();
                                    if(pcd  != NULL)
                                        {
                                            pcd->SetProperty(property, v2);
                                        }
                                }

                        }


                }
                break;
	  
                /*****************************
             HANDLED ITERATIVE
                ************************************/
            case PEBL_ADD:
                {
                    //Execute left and right nodes, which puts results on stack
                    const PNode * node1 = node->GetLeft();
                    const PNode * node2 = node->GetRight();
                    //Create new opnode to handle tail...
                    const OpNode * tail = new OpNode(PEBL_ADD_TAIL,NULL,NULL,node->GetFilename(), node->GetLineNumber());

                    mNodeStack.push(tail);
                    mNodeStack.push(node2);
                    mNodeStack.push(node1);
	
                }
                break;


            case PEBL_ADD_TAIL:
                {
                    //Get the top two items from the stack.  The rightmost will
                    //be on top.
                    Variant v2 = Pop();
                    Variant v1 = Pop();

                    //There maybe should be more error checking to ensure
                    //that the atoms are numbers.
                
                    Push(v1+v2);

                }
                break;

                /*******************************
               ITERATED COMPLETE:
                *******************************/
            case PEBL_DIVIDE:
                {
                    //Execute left and right nodes, which puts results on stack
                    const PNode * node1 = node->GetLeft();
                    const PNode * node2 = node->GetRight();
                    const OpNode * tail = new OpNode(PEBL_DIVIDE_TAIL,NULL,NULL,
                                                     node->GetFilename(), node->GetLineNumber());
                
                    mNodeStack.push(tail);
                    mNodeStack.push(node2);
                    mNodeStack.push(node1);
                
                }
                break;
            
            case PEBL_DIVIDE_TAIL:
                {
                    //Get the top two items from the stack
                    Variant v2 = Pop();	
                    Variant v1 = Pop();
                
                    Push(v1/v2);
                }
                break;


                /***************************
              ITERATED!!!
                **************************/
            case PEBL_MULTIPLY:
                {
                    //Execute left and right nodes, which puts results on stack
                    const PNode * node1 = node->GetLeft();
                    const PNode * node2 = node->GetRight();
                    const OpNode * tail = new OpNode(PEBL_MULTIPLY_TAIL,NULL,NULL,
                                                     node->GetFilename(), node->GetLineNumber());
                
                    mNodeStack.push(tail);
                    mNodeStack.push(node2);
                    mNodeStack.push(node1);
                }
                break;
            case PEBL_MULTIPLY_TAIL:
                {
                    //Get the top two items from the stack.  The right will be on top
                    Variant v2 = Pop();	
                    Variant v1 = Pop();
                
                    Push(v1*v2);
                }
                break;
            

                /******************************
              ITERATED!!!
                ***********************************/
            case PEBL_POWER:
                {
                    //Execute left and right nodes, which puts results on stack
                    const PNode * node1 = node->GetLeft();
                    const PNode * node2 = node->GetRight();
                    const OpNode * tail = new OpNode(PEBL_POWER_TAIL,NULL,NULL,
                                                     node->GetFilename(), node->GetLineNumber());
                    mNodeStack.push(tail);
                    mNodeStack.push(node2);
                    mNodeStack.push(node1);

                }
                break;
            case PEBL_POWER_TAIL:
                {
                
                    //Get the top two items from the stack
                    Variant v2 = Pop();	
                    Variant v1 = Pop();

                    Push(Variant(pow((pDouble)v1,(pDouble)v2)));
                }
                break;


                /******************************
              ITERATED!!!
                ***********************************/
            case PEBL_SUBTRACT:
                {
                    //Execute left and right nodes, which puts results on stack
                    const PNode * node1 = node->GetLeft();
                    const PNode * node2 = node->GetRight();
        
                    const OpNode * tail = new OpNode(PEBL_SUBTRACT_TAIL,NULL,NULL,
                                                     node->GetFilename(), node->GetLineNumber());
                    mNodeStack.push(tail);
                    mNodeStack.push(node2);
                    mNodeStack.push(node1);
                
                }
                break;
            case PEBL_SUBTRACT_TAIL:
                {
                    //Get the top two items from the stack.  The right will be on top
                    Variant v2 = Pop();	
                    Variant v1 = Pop();
                
                    Push(v1 - v2);
                }
                break;
	

                /**************************
                   HANDLED
                **************************/
            case PEBL_AND:
                {
                    //Evaluate left and right nodes, and do an AND of them.
      
                    //Execute left and right nodes, which puts results on stack
                    const PNode * node1 = node->GetLeft();
                    const PNode * node2 = node->GetRight();
                    const OpNode * tail = new OpNode(PEBL_AND_TAIL,NULL,NULL,
                                                     node->GetFilename(), node->GetLineNumber());

                    mNodeStack.push(tail);
                    mNodeStack.push(node2);
                    mNodeStack.push(node1);
                
                }
                break;
            case PEBL_AND_TAIL:
                {
                    //Get the top two items from the stack.  The right will be on top
                    Variant v2 = Pop();	
                    Variant v1 = Pop();
                
                    Push(v1 && v2);
                }
                break;
            
                /*****************************
                   HANDLED
                ************************************/
            case PEBL_OR:
                {
                    //Evaluate left and right nodes, and do an or on them.
                
                    //Execute left and right nodes, which puts results on stack
                    const PNode * node1 = node->GetLeft();
                    const PNode * node2 = node->GetRight();
                    const OpNode * tail = new OpNode(PEBL_OR_TAIL,NULL,NULL,
                                                     node->GetFilename(), node->GetLineNumber());

                    mNodeStack.push(tail);
                    mNodeStack.push(node2);
                    mNodeStack.push(node1);

                }
                break;
            case PEBL_OR_TAIL:
                {
                    //Get the top two items from the stack.  The right will be on top
                    Variant v2 = Pop();	
                    Variant v1 = Pop();
                
                    Push(v1 || v2);

                }
                break;
            
            /*******************************
             HANDLED:
            **********************************/
            case PEBL_NOT:
                {
                    //Evaluate left nodes, and negate it.
                    
                    //Execute left and right nodes, which puts results on stack
                    const PNode * node1 = node->GetLeft();
                    const OpNode * tail = new OpNode(PEBL_NOT_TAIL,NULL,NULL,
                                                     node->GetFilename(), node->GetLineNumber());
                    
                    mNodeStack.push(tail);
                    mNodeStack.push(node1);
                    
                    
                }
                break;
                
            case PEBL_NOT_TAIL:
                {
                    //Get the top two items from the stack.  The right will be on top
                    Variant v1 = Pop();
                    Push(!v1);
                }
                break;
                
                
                /**************************
                    HANDLED
          *********************************/
            case PEBL_IF:
                {

                    //Left node is the expression test;
                    //Right node is the code block to execute if true.

                    const PNode * node1 = node->GetLeft();
                    const OpNode * tail = new OpNode(PEBL_IF_TAIL,NULL,NULL,
                                                     node->GetFilename(), node->GetLineNumber());

                    const PNode * codeblock = node->GetRight();

                    //Put the codeblock on the stack; we will remove it
                    //later if need be.

                    mNodeStack.push(codeblock);
                    mNodeStack.push(tail);
                    mNodeStack.push(node1);

                }
                break;

            case PEBL_IF_TAIL:
                {
                    Variant v1 = Pop();
                    if(v1)
                        {
                            //The test was true, so execute the codeblock.
                            //We need a tail to check if the codeblock returned a STACK_BREAK
                            const OpNode * tail2 = new OpNode(PEBL_IF_TAIL2,NULL,NULL,
                                                              node->GetFilename(), node->GetLineNumber());
                            mNodeStack.push(tail2);
                            //The codeblock is already on the node stack and will execute next

                        }
                    else
                        {
                            //The code block we want to execute is on top...
                            //remove it because the test failed.

                            mNodeStack.pop();

                            //Put a dummy value on the stack as the return value.
                            Push(0);
                        }
                }
                break;

            case PEBL_IF_TAIL2:
                {
                    //The codeblock has executed. Check if it returned a STACK_BREAK
                    //If so, propagate it; otherwise leave the result as-is
                    //Actually, we don't need to do anything here - the result is already
                    //on the stack and will be checked by the enclosing STATEMENTS node
                }
                break;
                
            /*****************************************************************/
            case PEBL_IFELSE:
                {
                    //Left node is the expression test;
                    //Right node is a PEBL_ELSE node, which 
                    //has both code blocks on it, and 
                    //decides what to do based on what is
                    //on the top o' the stack.

                
                //Execute left node, which puts results on stack
                const PNode * node1 = node->GetLeft();
                const PNode * node2 = node->GetRight();

                mNodeStack.push(node2);
                mNodeStack.push(node1);
                    
            }
            break;

            /****************************

                 HANDLED
    *************************************/
        case PEBL_ELSE:
            {
                //This looks on the top of the stack and
                //executes the left node if true; right node if false.

                Variant v1 = Pop();

                const PNode * node1;
                if(v1)
                    {
                        node1 = node->GetLeft();
                    }
                else
                    {
                        node1 = node->GetRight();
                    }

                //Add a tail to check if the codeblock returns a STACK_BREAK
                const OpNode * tail = new OpNode(PEBL_ELSE_TAIL,NULL,NULL,
                                                  node->GetFilename(), node->GetLineNumber());
                mNodeStack.push(tail);
                mNodeStack.push(node1);
            }
            break;

        case PEBL_ELSE_TAIL:
            {
                //The codeblock has executed. Check if it returned a STACK_BREAK
                //If so, propagate it; otherwise leave the result as-is
                //The result is already on the stack and will be checked by the enclosing STATEMENTS node
            }
            break;

            /*****************************************************************/
        case PEBL_LAMBDAFUNCTION:
            {
                // This is the top node of an anonymous function.  It is a 
                // node with two children: on the left is a variable list, and
                // on the right is a code block.  To make it a named function, just
                // the parent node will be a PEBL_FUNCTION OpNode with a left child
                // which is a function datanode.
                
                // When this function is called, the top of the stack is a list that contains
                // the bindings for the variables. Get the list and assign the values to the 
                // variables one-by-one, then execute the code block.
                
                //Get the variable list.
                const PNode * node1 = node->GetLeft();

                //Get the argument list.
                Variant v1 = Pop();

                //Create a list to use.
                //If v1 is a stacksignal list_head, there are no arguments
                //provided.


                counted_ptr<PEBLObjectBase> tmpList;

                if( v1.IsStackSignal() && v1.GetSignal() == STACK_LIST_HEAD)
                    {
                        //v1 is empty, so make a dummy parameter list to send.

                        tmpList = counted_ptr<PEBLObjectBase>(new PList());
                    }


                if( v1.IsComplexData())
                    {

                        //extract the object out of v1 to send.
                        tmpList = v1.GetComplexData()->GetObject(); 
                    }




                Variant v2 = 0;


                //iterate through the lists and assign values to variables.
                PList * tmp = (PList*)(tmpList.get());

                vector <Variant>::iterator p = tmp->Begin();
                Variant vdef = 0;  // Initialize to prevent uninitialized reads

                while(node1)
                    {
                        //Each variable could be either a variable name (global or local)
                        //or a varpair--a node pairing a global/local with a value.
                        vdef = 0;  // Reset to 0 each iteration

                        //This loads the default values initially when they appear,
                        //and then overwrites them if actual values exist.  This means
                        //the default value(if global) needs to exist--it probably shoudn't have to
                        bool hasdefault = false;
                        if(((OpNode*)(((OpNode*)node1)->GetLeft()))->GetOp()==PEBL_VARPAIR)
                            {
                           //this variable has a default value....
                                hasdefault = true;
                                //v2 is the variable name.
                                v2 =   ((DataNode*) (  ((OpNode*)(((OpNode*)node1)->GetLeft()))->GetLeft()))->GetValue();

                                if(((((OpNode*)(((OpNode*)node1)->GetLeft()))->GetRight()))->GetType()==PEBL_OP_NODE)
                                    {
                                        //Possible, we could parse/evaluate this node to get the value.
                                        PError::SignalFatalError("Error in function definition: default parameter value must be a data value");
                                    }

                                //It is a data node--default value.
                                vdef =  ((DataNode*)(((OpNode*)(((OpNode*)node1)->GetLeft()))->GetRight()))->GetValue();

                            }
                        else

                            {
                                //it is a pure value here
                                v2 = ((DataNode*)(((OpNode*)node1)->GetLeft()))->GetValue();

                            }



                        if((p==tmp->End()) & (!hasdefault))   //check for too few parameters.
                            {

                                //We have run out of parameters passed in, but there are more
                                //arguments to the function.  This would be OK if the next argument
                                //is optional (hasdefault is true), but otherwis we should have an
                                //error message.


                                //Too few arguments.
                                string message =  "Too few arguments passed to function [" + mScope + "].";


                                if(mScope == "Start")
                                    message += " (Make sure Start function has only one variable).";

                                PError::SignalFatalError(message);

                            }

                        //Get the value,

                        if(p != tmp->End())
                            {
                                //Add pair to variable map.  This should always be a local variable map.
                                mLocalVariableMap.AddVariable(v2, *p);

                                //remove it from the front of the list.
                                p++;//arglist->PopFront();
                                //Move to the next item.
                                node1 = ((OpNode*)node1)->GetRight();
                            }else{


                            if(hasdefault)
                                {
                                    //p is the end of the argument list, but we may have a vdef value.
                                    //cout << "ASsigning default value:" << v2 <<"|" << vdef << endl;
                                    //cout << vdef.GetDataTypeName() <<endl;
                                    if(vdef.IsGlobalVariable())
                                        {
                                            vdef=  gGlobalVariableMap.RetrieveValue(vdef);
                                        }
                                    else if (vdef.IsLocalVariable())
                                        {
                                            //a local variable may be bound to a previously-specified variable
                                            vdef = mLocalVariableMap.RetrieveValue(vdef);
                                        }
                                    mLocalVariableMap.AddVariable(v2,vdef);  //add the default value to start.
                                    node1 = ((OpNode*)node1)->GetRight();
                                } else{


                                //end the mapping
                                node1 = NULL;
                            }
                        }
                    }



                //if(tmp && tmp->Length() > 0)
                if(p != tmp->End())
                    {
                        //Too many arguments.
                        string message = string("Too many arguments passed to function [" + mScope + "].");

                        if(mScope == "Start") message += " (Make sure Start function has a variable).";
                        PError::SignalFatalError(message);
                    }
                    

                //Now, get the code block and execute it.
                const PNode * node2 = node->GetRight();

                //No tail function needed?
                mNodeStack.push(node2);
            }
            break;
            
            /*******************************
              HANDLED (Nothing needed)
          **********************************/
        case PEBL_LIBRARYFUNCTION:
            {


                // This type of function is built in and precompiled.  The loader examines
                // the parse tree and
                // identifies each function that is used.
                
                //The left child of a PEBL_LIBRARYFUNCTION contains an PEBL_AND node containing two datanodes 
                //each containing integers describing the min and max  number of arguments, respectively.


                const OpNode * node0 =(OpNode*)(node->GetLeft());
                //We should be able to tell the name of the function

                std::string name = node->GetFunctionName();




                const unsigned int min = ((DataNode*)(node0->GetLeft()))->GetValue().GetInteger();
                const unsigned int max = ((DataNode*)(node0->GetRight()))->GetValue().GetInteger();

                

                //The right child of a PEBL_LIBRARYFUNCTION contains a datanode which
                //has a function pointer in it.

                const PNode * node1 = node->GetRight();
                Variant v1 = ((DataNode*)node1)->GetValue();

                //All built-in functions take a single parameter: a Variant list.  
                //This variant should be on the top of the stack right now. So get it.
                Variant v2 = Pop();

                //Before we execute, check to see if v2 has a length  between min and max.


                PList * tmp = NULL;
                if (v2.IsStackSignal())
                    numargs = 0;
                else
                    {
                        if(v2.IsComplexData())
                            {
                                if((v2.GetComplexData())->IsList())
                                    {
                                        //#if !defined(PEBL_EMSCRIPTEN)
                                        
                                        //Dont check parameter numbers in EMSCRIPTEN

                                        tmp = (PList*)(v2.GetComplexData()->GetObject().get());
                                        numargs = tmp->Length();
                                        //#endif
                                    }
                                else
                                    {
                                        cerr << "UNHANDLED ELSE CASE. NOT A LIST\n" ;
                                    }
                            }
                    }

                //#if !defined(PEBL_EMSCRIPTEN)
                if(numargs < min || numargs > max)
                    {

                        Variant message = Variant("In scope [") + mScope + Variant("]:") +
                            Variant("function [") +name+Variant("]:")+
                            Variant("Incorrect number of arguments..  Wanted between ")+
                            Variant((int)min) + Variant(" and ") +Variant((int) max) + Variant(" but got ") + Variant( (int)numargs);

                        PError::SignalFatalError(message);
                    }
                //#endif
                


                //Execute the functionpointer and push the results onto the stack.
                //Note that this will happen 'synchronously'; not through the eval loop.
                Push((v1.GetFunctionPointer())(v2));
            }

            break;


            /*********************************
                   HANDLED (Nothing needed directly)
             ********************************/
        case PEBL_FUNCTION:
            {
                // This controls the execution of a function. (not the
                // loading of a defined function).  A function node
                // has a function datanode on its left child (specifies the function name)
                // and an arglist (the head of a list of arguments) on its right child.  
                
                //Note that there is no direct link to the actual function code--it needs
                //to be accessed via a lookup map. Once that code is identified and the paramaters are handled 
                //here, the rest is managed via a PEBL_LAMBDAFUNCTION node above. 


                //We need to evaluate the arguments on the right, then pick up later.

                const PNode *node1 = node->GetRight();
                
                const OpNode * tail = new OpNode(PEBL_FUNCTION_TAIL1,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());


                //get the function name now, and put it on the stack, to get back later.
                Variant funcname = dynamic_cast<DataNode*>(node->GetLeft())->GetValue();
            
               Push(funcname); //Put the function name on the stack.
            
            
                mNodeStack.push(tail);
                mNodeStack.push(node1);
            }
            break;

        case PEBL_FUNCTION_TAIL1:
            {

                //The arguments should have been evaluated, and are at the top of the stack.
                //Right below them is the function name.

                // The parameters for a function are in a list on the top of the stack.
                // A function should pull the list off the stack and push it onto
                // the stack of the new evaluator scope.

                //    cout << dynamic_cast<DataNode*>(node->GetLeft())->GetValue() << endl;
                //Get the name of the function.

                Variant args = Pop();

                Variant funcname = Pop(); //We would just put this back later, so only take a look.


                //Check for custom object method dispatch before looking up the function
                //If the first argument is a custom object with a property matching the function name,
                //redirect to that function name instead
                if(args.IsComplexData())
                {
                    PList *plist = dynamic_cast<PList*>(args.GetComplexData()->GetObject().get());
                    if(plist && plist->Length() > 0)
                    {
                        Variant first = plist->First();

                        if(first.IsComplexData())
                        {
                            if(first.GetComplexData()->IsCustomObject())
                            {
                                PCustomObject * pco = dynamic_cast<PCustomObject*>(first.GetComplexData()->GetObject().get());

                                if(OVE_SUCCESS == pco->ValidateProperty(funcname))
                                {
                                    funcname = Variant(pco->GetProperty(funcname).GetString().c_str(), P_DATA_FUNCTION);
                                }
                            }
                        }
                    }
                }

                //put the arguments back on top of the stack for the second tail, in
                //reverse order.
                Push(args);


                //The funcname is just used to update the scope variable.  Maybe we could do that now instead
                //of putting it on the stack?
                Push(funcname);


                //now, get the actual function code from the function map:
                const PNode * node2 = mFunctionMap.GetFunction(funcname);

            
#ifdef PEBL_DEBUG_PRINT
                cout << "Calling a function with argument list. " <<  endl;
#endif 
                
         
                //Now, node2 will either be a PEBL_LAMBDAFUNCTION or a PEBL_BUILTINFUNCTION
                //Lambda functions are just code blocks, but need to be executed in 
                //a new scope.  A built-in function is precompiled
                //and doesn't need its own new scope, so don't create one in that case.
                

                mNodeStack.push(node2);
               


                const OpNode * tail2 = new OpNode(PEBL_FUNCTION_TAIL2,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());
                
                //schedule handling by the base function code. It will either 
                //handle the lamdafunction path or the builtinfunction path.
                mNodeStack.push(tail2);

            }
            break;

        case PEBL_FUNCTION_TAIL2:
            {
                //function name is on top; next are the arguments.
                Variant funcname = Pop();
                
                //right now, the top node should be the result of node->GetOp()...
                
                const OpNode * node2 = (OpNode*)(mNodeStack.top());
                mNodeStack.pop();
                
#ifdef PEBL_DEBUG_PRINT 
                cout << "In function_tail2  OpNode ["<< node2->GetOp() << "] of Type: " << node2->GetOpName() << "------------\n";
#endif
                
                
                switch(((OpNode*)node2)->GetOp())
                    {



                    case PEBL_LIBRARYFUNCTION:
                        {

                            mNodeStack.push(node2);

                        }
                        break;


                    case PEBL_LAMBDAFUNCTION:
                        {   //Need to create a new scope to allow for variable declaration
                            //within case statement

                            //We want to avoid creating multiple evaluators so that we can
                            //more easily async the evaluator loop.  So, create a stack
                            //to hold them.

                            mVariableMapStack.push(mLocalVariableMap);
                            //VariableMap & tmpmap = mLocalVariableMap;


                            mScopeStack.push(mScope);


                            mScope = funcname.GetFunctionName();

                            //Push the call site onto the call stack for error reporting
                            gCallStack.Push(node);

                            //This cleans up the return arguments of the lambda function.
                            const OpNode * tail = new OpNode( PEBL_FUNCTION_TAIL_LIBFUNCTION,NULL,NULL,
                                                              node->GetFilename(), node->GetLineNumber());

                            mNodeStack.push(tail);

                            //Add this last so it gets executed first
                            mNodeStack.push(node2);

                        }
                        break;

                  

                    default:
                        PError::SignalFatalError("Unknown Function Type in PEBL_FUNCTION_TAIL2");
                        break;
                    }
                
            }

            break;

        case PEBL_FUNCTION_TAIL_LIBFUNCTION:
            {
                //Pop the call site from the call stack for error reporting
                if(gCallStack.Size() == 0) {
                    PError::SignalFatalError("gCallStack is empty in PEBL_FUNCTION_TAIL_LIBFUNCTION");
                }
                gCallStack.Pop();

                //Go to the previous context/scope label and variables.
                if(mScopeStack.size() == 0) {
                    PError::SignalFatalError("mScopeStack is empty in PEBL_FUNCTION_TAIL_LIBFUNCTION");
                }

                mScope = mScopeStack.top();
                mScopeStack.pop();

                if(mVariableMapStack.size() == 0) {
                    PError::SignalFatalError("mVariableMapStack is empty in PEBL_FUNCTION_TAIL_LIBFUNCTION");
                }

                mLocalVariableMap = mVariableMapStack.top();
                mVariableMapStack.pop();

            }
            break;
            

            /*****************************************************************/
        case PEBL_WHILE:
            {


                //we might have gotten here from an iteration of while
                //that contained a break command. Check this
                if(GetStackDepth()>0)
                    {
                        Variant results = Peek();
                        if(results.GetDataType() == P_DATA_STACK_SIGNAL &&
                           results == Variant(STACK_BREAK))
                            {
                                Pop();
                                Push(Variant(1)); //Put a dummy variant on the stack so the 
                                //stop signal does not get propogated.
                                break;
                            }
                    }


                // The left child node is an expression to test
                // The right node is a code block.
                // Test the left node, then evaluate the right node if true, 
                //then re-program a while node to repeat until not successful.




                const PNode * node1 = node->GetLeft();

                const OpNode * tail = new OpNode( PEBL_WHILE_TAIL,NULL,NULL,
                                                  node->GetFilename(), node->GetLineNumber());
                

                mNodeStack.push(node);  //Push the while node back on the stack...we need this later.
                mNodeStack.push(tail);  //the while_tail which checks the value...
                mNodeStack.push(node1); //first evaluate the left node value.
                    
            }
            break;
            
            case PEBL_WHILE_TAIL:
                {
                    
                    //Get the result of the evaluation
                    Variant v1 = Pop();
            
                    //If the evaluation is false, break out of loop/switch
                    if(v1) 
                        {

                            //This should handle a 'break'
                            if(v1.GetDataType() == P_DATA_STACK_SIGNAL &&
                               v1 == Variant(STACK_BREAK))
                                {
                                    Variant results = Pop();
                                    Push(results);
                                    break;
                                }

                            //Ok, the test was true, so Evaluate the right node
                            //of the while(), which is on the top of the stack right now.

                            OpNode * whilenode = (OpNode*)(mNodeStack.top());
                            OpNode * node2 = (OpNode*)(whilenode->GetRight());
                            
                            const OpNode * whiletail2 = new OpNode( PEBL_WHILE_TAIL2,NULL,NULL,
                                                                    node->GetFilename(), node->GetLineNumber());

                            //Execute the code, then it will re-execute the while node
                            //which is still on top of the stack.
                            mNodeStack.push(whiletail2);
                            mNodeStack.push(node2);
                            
                            break;
                    
                            
                        }
                    else                    
                        {       
                            //Remove the pending while node from the stack.
                            mNodeStack.pop();
                            Push(Variant(1));  //Add a value to the stack that will get popped at the next statement.
                            break;
                        }

                }
                break;
            

        case PEBL_WHILE_TAIL2:
            {
                //At the end of executing the code block for a while loop, the result
                //is on top of the stack.  Remove it now.

                Variant value = Pop();

                if(value.IsStackSignal() && value.GetSignal() == STACK_BREAK)
                    {
                        //put the stack break value back on the stack to 
                        //signal the while head.
                        Push(value); 

                    }
            }
            break;

            /*****************************************************************/
        case PEBL_LOOP:
            {
                // The PEBL_LOOP node has two children: the left child is a variable/datum pair,
                // and the right child is a code block that should be evaluated once for each
                // element of the datum, with the variable being set to that element for
                // each iteration.
                
                //Get the variabledatum pair.  Evaluating this will create a list and 
                //put it on top of the stack.
                const PNode * node1 = node->GetLeft();  //A variable-datum node.
                const PNode * codeblock = node->GetRight();
                const OpNode * tail = new OpNode(PEBL_LOOP_TAIL1,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());
             

                const OpNode * tail2 = new OpNode(PEBL_LOOP_TAIL2,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());

                
                mNodeStack.push(codeblock);  //Put the codeblock on the stack for access later.
                mNodeStack.push(tail2);      //This will re-program the loop when the codeblock is done.
                mNodeStack.push(codeblock);  //put this on the stack for execution after variables are bound
                mNodeStack.push(tail);       //execute the tail after the node1 is executed.
                mNodeStack.push(node1);      //first, execute the variable-datum node to put the list 
                //                             and the variable on the stack.

                Push(Variant(1)); //now, add index 0  to the top of the stack.
            }
            break;


        case PEBL_LOOP_TAIL1:
            {
                //Now, the next two items on the stack should be the datum and the variable.

                Variant list = Pop();
                Variant varname = Pop();
                const long unsigned int index = (const long unsigned int)Pop();


                PError::AssertType(varname, PEAT_VARIABLE,"Error: iterator of loop not a variable");

                // Handle integer to list conversion (like Evaluator.cpp does)
                PList * tmp;
                if(list.IsInteger())
                {
                    // Convert integer to list [1, 2, 3, ..., n]
                    tmp = new PList();
                    for(int count=1; count <= list.GetInteger(); count++)
                    {
                        tmp->PushBack(count);
                    }
                    // Update the list on the stack to be this new list
                    counted_ptr<PEBLObjectBase> tmpptr = counted_ptr<PEBLObjectBase>(tmp);
                    PComplexData * pcd = new PComplexData(tmpptr);
                    list = Variant(pcd);
                }
                else
                {
                    PError::AssertType(list, PEAT_LIST,"Second argument of loop(<iterator>, <list>) must be a list.");
                    tmp = (PList*)(list.GetComplexData()->GetObject().get());
                }
                
                if(tmp->Length()<index)
                    {
                        //Looping is over!
                        mNodeStack.pop(); //Get rid of the codeblock node, which should come next
                        mNodeStack.pop();  //Get rid of the pre-programmed tail2 node
                        mNodeStack.pop();  //Get rid of copy 2 of the codeblock.
                        Push(1);         //Handle the stack.  We may need to adjust this for break.
                    } else {



                    const Variant p = tmp->Nth(index); 
                    //Set the variable to the current list element, and increment p

                    if(varname.IsLocalVariable())
                        {
                            mLocalVariableMap.AddVariable(varname, p);
                        }
                    else if (varname.IsGlobalVariable())
                        {
                            gGlobalVariableMap.AddVariable(varname,p);
                        }
                    
                    
                    //Now, the codeblock will execute automatically.  But need to reprogram the loop1
                    //to occur after that.

                    
                    //Add the new index.

                    Push(index+1);

                    //Put the varname back on the stack, for the next time through
                    Push(varname);
                    
                    //Put the list back on the stack
                    Push(list);
                }
            }
            break;


        case PEBL_LOOP_TAIL2:
            {
                //the code block has just been executed. We need to look at the results to
                //handle any breaks.
                //Normally, the top of the stack will be the next index to handle

                Variant results = Pop();

                if(results.GetDataType() == P_DATA_STACK_SIGNAL &&
                   results == Variant(STACK_BREAK))
                    {
                        Pop();  //pop the list
                        Pop();  //pop the variable name
                        Pop();  //pop the next index
                        Push(Variant(0));  //Push on a dummy return value

                        mNodeStack.pop(); //pop the codeblock (the original copy from PEBL_LOOP setup)
                    } else {
                

                    //reprogram another loop.
                    //once again, the variable and the list need to be on top of the stack.

                    //The codeblock is currently at the top of the stack.
                    const PNode * codeblock = mNodeStack.top();

                    //Program the next tail1/tail2 combo, with the codeblock sandwiched between:
                    const OpNode * tail1 = new OpNode(PEBL_LOOP_TAIL1,NULL,NULL,
                                                     node->GetFilename(), node->GetLineNumber());
                    
                    
                    const OpNode * tail2 = new OpNode(PEBL_LOOP_TAIL2,NULL,NULL,
                                                      node->GetFilename(), node->GetLineNumber());


                    mNodeStack.push(tail2);      //This will re-execute the loop when the codeblock is done.
                    mNodeStack.push(codeblock);  //(put this on the stack for later)
                    mNodeStack.push(tail1);       

                    
                    //Right now, the index is on the top of the data stack, so we don't need to push it
                    //back on.
                }                        

            }
            break;


        case PEBL_VARIABLEDATUM:
            {
                //The VARIABLDATUM node is used in the loop function. The variable is the 
                //left child, a list of data in the form of an expression is the right. 

                //This node is executed once at the beginning
                //of the loop; it pushes the variable name and the evaluated list onto the stack.
                //PEBL_LOOP evaluation then gets them from the stack and iterates through the list,
                //setting the variable to each element of the data.
                
                //Get the variable:
                const PNode * node1 = node->GetLeft();

                //Push the variable name directly on the stack (without evaluating it.)
                Push(((DataNode*)node1)->GetValue());
                
                //Get the datum:
                const PNode * node2 = node->GetRight();
                
                //Evaluate the datum--it gets put on the stack.
                mNodeStack.push(node2);
            }
            break;


            /**********************************
                   HANDLED: (no tail needed)
             *******************************/
        case PEBL_ARGLIST:
            {
                // This behaves almost exactly like a listhead. It will end up with a variant
                // on the top of the stack that is a list containing all of the arguments
                // for a function.


                //Create the stack signal variant and push it onto the stack.
                Variant v1 = Variant(STACK_LIST_HEAD);
                Push(v1);
                
                //Get the top node of the list 
                const PNode * node1 = node->GetLeft();

                if(node1)
                    { 
                        //If it exists, evaluate it.
                        mNodeStack.push(node1);
                    }

                //if node1 doesn't exist, it is a null argument list--do nothing more.
                        

            }
            break;

            /***************************
             //Handled, no tail needed
             ***************************/
        case PEBL_LISTHEAD:
            {
                //When we get a list head, we need to evaluate each expression in the list,
                //pushing them onto the stack. When we get to the end of the list, we just need
                //to pop items, adding them to the list,  until we get back up to the top.
                //To do this efficiently and avoid recreating lists iteratively, the bottom
                //item does this in a while loop.  to signal the end of the while loop, the
                //PEBL_LISTHEAD must put a dummy item on the stack so the bottom item
                //knows when the front of the list has been reached.  This is a variant of type STACK_LIST_HEAD
                
                //Create the stack signal variant and push it onto the stack.
                Variant v1 = Variant(STACK_LIST_HEAD);
                Push(v1);
            
                //Get the top node of the list and Evaluate it.
                const PNode * node1 = node->GetLeft();
                if(node1)
                    {

                        mNodeStack.push(node1);

                    }
                else
                    {
                        //If node1 is NULL, we have an empty list.
                        //Get rid of the STACK_LIST_HEAD on the top of the stack
                        Pop();

                        //Make an empty list and push it onto the stack.
                        counted_ptr<PEBLObjectBase> tmpList = counted_ptr<PEBLObjectBase>(new PList());

                        PComplexData  pcd = PComplexData(tmpList);
                        Variant v2 = Variant(&pcd);
                        Push(v2);
                    }
            }
            break;


            /*****************************************************************/
        case PEBL_LISTITEM:
            {
                // for a list item, the list is represented as a parsed tree with the 
                // left node being the data and the right node being another listitem node.
                // This should create the list piece by piece recursively.
                
                // To do this, Evaluate() each item so it gets executed and the value
                // gets pushed onto the stack.  When you come to the end of the list, 
                // pop the items off the list iteratively until you get back where you came from.
                
                //The data: Get it out of the tree and evaluate it, so it gets
                //push onto the stack.


                const PNode * node1 = node->GetLeft();
                const PNode * node2 = node->GetRight();
                
                if(node2)
                    {
                        mNodeStack.push(node2);  //do the right node after the left node.
                    }
                else
                    {//we are at the end of the list, so do the cleanup stuff.
                        const OpNode * tail = new OpNode(PEBL_LISTITEM_TAIL,NULL,NULL,
                                                         node->GetFilename(), node->GetLineNumber());
                        mNodeStack.push(tail);   
                    }

                
                mNodeStack.push(node1);  //do the left node first.

            }
            break;

            case PEBL_LISTITEM_TAIL:
                {
  
                    //We are at the end of the list.  
                    //If node2 is NULL, then we are at the end of the list.
                    //everything has been pushed to the stack.  Just get everything
                    //off the stack, make a list out of it,  and put it back on the stack.


                    //Must make a new list, and create a variant out of it.
                    PList * tmpList = new PList();
                    PList order;
                    int ord = 0;
                    //Now, pop off items from the list until you get to a
                    //P_DATA_STACK_SIGNAL, then if it i
                    Variant v1 = Pop();
                    
                    //we need to create the list from the items on the stack. 
                    //But the top of the stack is the end of the list.

                    while(v1.GetDataType() != P_DATA_STACK_SIGNAL)
                        {
                            
                            //Add the item to the list.
                            tmpList->PushBack(v1);
                            order.PushBack(ord--);
                            //Pop and repeat.
                            v1 = Pop();
                        }

                    
                        //add tmplist (in reverse order) to the stack as the argument list.
                        std::reverse(tmpList->Begin(),tmpList->End());
                        counted_ptr<PEBLObjectBase> pl = counted_ptr<PEBLObjectBase>(tmpList);
                        PComplexData  pcd =  PComplexData(pl);
                        Variant v2 = Variant(&pcd);
                        Push(v2);
                }
        
                break;

            /************************ HANDLED *****************************************/
 case PEBL_LT:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                const PNode * node2 = node->GetRight();
                const OpNode * tail = new OpNode(PEBL_LT_TAIL,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());

                mNodeStack.push(tail);
                mNodeStack.push(node2);
                mNodeStack.push(node1);
                
            }
            break;
 
        case PEBL_LT_TAIL:
            {
                
                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();
                
                Push(v1 < v2);
            }
            break;
	  		
            /*********************************HANDLED   ********************************/
        case PEBL_GT:
            {

                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                const PNode * node2 = node->GetRight();
                const OpNode * tail = new OpNode(PEBL_GT_TAIL,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());
                
                mNodeStack.push(tail);
                mNodeStack.push(node2);
                mNodeStack.push(node1);
            }
            break;

        case PEBL_GT_TAIL:
            {
                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();
                Push(v1 > v2);
            }
            break;
            
            /**********************************HANDLED*******************************/
        case PEBL_GE:
            {
                //Execute left and right nodes, which puts results on stack

                const PNode * node1 = node->GetLeft();
                const PNode * node2 = node->GetRight();
                const OpNode * tail = new OpNode(PEBL_GE_TAIL,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());
                

                mNodeStack.push(tail);
                mNodeStack.push(node2);
                mNodeStack.push(node1);

            }
            break;
 
       case PEBL_GE_TAIL:
           {
               

               //Get the top two items from the stack.  The right will be on top
               Variant v2 = Pop();
               Variant v1 = Pop();
               Push(v1 >= v2);
           }
           break;

            /*******************************HANDLED**********************************/
        case PEBL_LE:
            {
		
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                const PNode * node2 = node->GetRight();
                
                const OpNode * tail = new OpNode(PEBL_LE_TAIL,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());
                
                mNodeStack.push(tail);
                mNodeStack.push(node2);
                mNodeStack.push(node1);
            }
            break;
        case PEBL_LE_TAIL:
            {
                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();
                
                Push(v1 <= v2);
            }
            break;


            /********************************HANDLED*********************************/
        case PEBL_EQ:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                const PNode * node2 = node->GetRight();

                
                const OpNode * tail = new OpNode(PEBL_EQ_TAIL,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());
                
                mNodeStack.push(tail);
                mNodeStack.push(node2);
                mNodeStack.push(node1);
            }
            break;

        case PEBL_EQ_TAIL:
            {
                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();
                Push(v1 == v2);
            }
            break;

            /*******************************  HANDLED  **********************************/
        case PEBL_NE:
            {		
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                const PNode * node2 = node->GetRight();
       
                const OpNode * tail = new OpNode(PEBL_NE_TAIL,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());
                
                mNodeStack.push(tail);
                mNodeStack.push(node2);
                mNodeStack.push(node1);
            }
            break;

        case PEBL_NE_TAIL:
            {
                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();
                
                Push(v1 != v2);
            }
            break;
	  

            /*****************************************************************/
        case PEBL_STATEMENTS:
            {
                //This is a node that connects two statements. 

                // a STATEMENTS node connects a PEBL_STATEMENTS node on the left 
                //with an arbitrary OpNode on the right.
                //Note that if you have a chain of PEBL_STATEMENTS nodes,
                //it works from the bottom up: the right node at the top
                //of the chain is really the last node that will be executed.



#ifdef PEBL_DEBUG_PRINT
                cout << "Checking a PEBL_STATEMENTS: " << GetStackDepth() << endl;
#endif
                
                //If the top of the stack is a STACK_BREAK, we should do nothing.
                if(GetStackDepth()>0)
                    {
             
                        Variant v1 = Peek();


                        if(v1.GetDataType() == P_DATA_STACK_SIGNAL &&
                           v1 == Variant(STACK_BREAK))
                            {
                                //If this is a stack signal, and if 
                                //it is a break, we should just back out
                                //stackbreak is already on top of the stack.  Add a dummy to be taken off.
                                Push(Variant(0));
                                break;
                            }
                    }



                const OpNode * tail = new OpNode(PEBL_STATEMENTS_TAIL1,NULL,NULL,
                                                 node->GetFilename(), node->GetLineNumber());

                //We need to execute the left node, and then process the right node, so 
                //add them to the stack in inverse order.

                mNodeStack.push(node->GetRight());
                mNodeStack.push(tail);
                mNodeStack.push(node->GetLeft());



            }
            break;

            case PEBL_STATEMENTS_TAIL1:
                {

                    //The results of the first statement are on top of the stack.  Get them off:
                    Variant v1 = Pop();


                    if(v1.GetDataType() == P_DATA_STACK_SIGNAL &&
                       v1 == Variant(STACK_BREAK))
                        {
                                //If this is a stack signal, and if
                                //it is a break, we should just back out
                                //But first, we need to pop the Right node from the node stack
                                //to prevent it from executing
                            mNodeStack.pop();  //Remove the right statement from the node stack
                            Push(Variant(STACK_BREAK));
                            break;
                        }

                }
                break;




            /***************************ITERATIVE--NOTHING IS NEEDED HERE
             **************************************/
        case PEBL_BREAK:
            {

                //This exits out of the current loop, while, or function context.
                //it works by adding a STACK_BREAK stacksignalevent onto the top of the stack.
                //all relevant loops look for this type of event and 
                //cleanly abort when that times comes.
                Variant v1 = Variant(STACK_BREAK);
                Push(v1);

#ifdef PEBL_DEBUG_PRINT
                cout << "PEBL_BREAK: pushing break onto stack.\n";
#endif

            }
            break;

            /******************************
               Hnadled, no tail needed?
        ***********************************/
        case PEBL_RETURN:
            {
                //The return keyword is used ONLY at the very end of a
                //function. Its left node is the value to return; its right
                //node should be NULL.  So evaluate the left node; then what's on
                //the stack will be the return value.

                //Push(Variant(STACK_RETURN_DUMMY));

                const PNode * node1 = node->GetLeft();
                mNodeStack.push(node1);

            }
            break;

        default:
            //Signal an error here.
            return false;
            break;

        }
    return true;
}      




///
///  This method evaluates DataNodes
bool Evaluator::Evaluate1(const DataNode * node)
{

    //Set up the globally-accessible structure to allow
    //better error reporting.
    if(node->GetLineNumber() > -1)
        gEvalNode = node;

#ifdef PEBL_DEBUG_PRINT 
    cout << "-------------------------";
    cout << "Evaluating DataNode of Value: " << node->GetValue() << endl;;
    cout << "Line: " << node->GetLineNumber() << endl;
#endif

    Variant v1, v2;

    //A node of type P_DATA_NODE could be an integer, a float, a variable, a string, etc.
    //Grab the variant out of the node.  If it is a Variable, extract the value.
    //Push the initial value or the variable onto the stack.
  
    v1 = node->GetValue();

    switch(v1.GetDataType())
        {

        case P_DATA_LOCALVARIABLE:
            {      

                v2  = mLocalVariableMap.RetrieveValue(v1.GetVariableBaseName());
                //Get the name of property being 
                string property =v1.GetVariablePropertyName();

                if(property!="")
                    {
                        PComplexData * pcd = v2.GetComplexData();
                        v2 = pcd->GetProperty(property);
                    }
                
                Push(v2);
            }
            break;




        case P_DATA_GLOBALVARIABLE:
            {
                v2  = gGlobalVariableMap.RetrieveValue(v1.GetVariableBaseName());
                
                //Get the name of property being 
                string property =v1.GetVariablePropertyName();
                if(property!="")
                    {
                        PComplexData * pcd = v2.GetComplexData();
                        v2 = pcd->GetProperty(property);
                    }
                
                Push(v2);
            }
            break;
 
        case P_DATA_NUMBER_INTEGER:
        case P_DATA_NUMBER_FLOAT:
        case P_DATA_STRING:
        case P_DATA_COMPLEXDATA:
      
#ifdef PEBL_DEBUG_PRINT 
            cout << "Evaluating a normal Variant: ";
            cout <<  v1 << endl;
#endif
            Push(v1);
            break;

        case P_DATA_UNDEFINED:
        default:
            //This should signal an error.
            PError::SignalFatalError("In Function [" + mScope + "Undefined Data Type in Evaluate::Evaluate(DataNode)");
            return false;
            break;
        }

    return true;
}

//




/// This method takes a PEBL_FUNCTION OpNode, which is comprised of
/// a P_DATA_FUNCTION DataNode on the left and a parameter list on
/// the right, Finds the function code in the FunctionMap, and 
/// Evaluates that code.
void Evaluator::CallFunction(const OpNode * node)
{


    // First get the right node (the argument list) and evaluate it.
    // This will end up with a list Variant on top of the stack. 
    const PNode *node1 = node->GetRight();
    
    Evaluate1(node1);


    // The parameters for a function are in a list on the top of the stack.
    // A function should pull the list off the stack and push it onto
    // the stack of the new evaluator scope.


    //Get the name of the function.  

    Variant funcname =dynamic_cast<DataNode*>(node->GetLeft())->GetValue();

    //If the argument is a custom object, the custom object
    //might contain a class-specific method to use instead of the generic function
    //The argument is at the top of the stack right now.

    Variant v = Peek();
    if(v.IsComplexData())
        {
            //if v is a comlpex data, it should be a parameter list.


            PList *plist = dynamic_cast<PList*>(v.GetComplexData()->GetObject().get());
            Variant first = plist->First();
            
            if(first.IsComplexData())
                {

                    if( first.GetComplexData()->IsCustomObject())
                        {

                            PCustomObject * pco = dynamic_cast<PCustomObject*>(first.GetComplexData()->GetObject().get());
                            

                            if(OVE_SUCCESS== pco->ValidateProperty(funcname))
                                {

                                    funcname = Variant(pco->GetProperty(funcname).GetString().c_str(), P_DATA_FUNCTION);

                                }

                        }
                }
        }


    
    const PNode * node2 = mFunctionMap.GetFunction(funcname);

 
#ifdef PEBL_DEBUG_PRINT
    cout << "Calling a function with argument list: " <<  endl;
#endif 
    

    //Now, node2 will either be a PEBL_LAMBDAFUNCTION or a PEBL_BUILTINFUNCTION
    //Lambda functions are just code blocks, but need to be executed in 
    //a new scope, so need their own evaluator.  A built-in function is precompiled
    //and doesn't need its own new scope, so don't create one in that case.
    



    switch(((OpNode*)node2)->GetOp())
        {
        case PEBL_LAMBDAFUNCTION:
            {   //Need to create a new scope to allow for variable declaration
                //within case statement

                //get the top item of the stack.
                Variant v = Pop();
                
                
                //Make a new evaluator for the function scope and v at the top of the stack.
                Evaluator  myEval(v,funcname.GetFunctionName());
                
                //The callstack just keeps track of the series of
                //evaluators for debugging purposes.
                
                //Evaluate the lambda function in new scope.

                
                myEval.Evaluate1(node2);

                //Now that myEval is finished, take the
                //node off the callstack.
                //gCallStack.Pop();
                
                //If myEval has a stack depth of 1, it does not return anything.
                //If myEval has a stack depth of 2, it wants to return the top value.
                if(myEval.GetStackDepth() == 1)
                    {
                        //Add '1' to the stack as the default return value for a function (i.e., one without
                        //an explicit return value.)
                        cout << "Adding dummy value to end of null function\n";
                        Push(1);
                    }
                else if (myEval.GetStackDepth() == 2)
                    {
                        //Get the top of the function evaluator and push it onto the current evaluator.
                        //cout << "Adding real value of subfunction to end of null function\n";
                        Variant v1 = myEval.Pop();
                        Push(v1);
                    }
            }
            break;
            
        case PEBL_LIBRARYFUNCTION:
            Evaluate1(node2);

            break;


        default:
            PError::SignalFatalError("Unknown Function Type in Evaluator::CallFunction");
            break;
                }

}





void Evaluator::Push(Variant v)
{

#ifdef PEBL_DEBUG_PRINT
    cout << "Pushing Stack: depth: "<< mStack.size() << "-->" << GetStackDepth() + 1;
#endif 

    if (mStack.size() > mStackMax)
        {
            PError::SignalFatalError("Maximum Stack Depth Exceeded.  Runaway Function?");
        }
    mStack.push(v);
#ifdef PEBL_DEBUG_PRINT
    if(mStack.size())cout << "  [" << mStack.top() << "] is on top.\n";
    else cout << endl;
#endif 

}


Variant Evaluator::Pop()
{
#ifdef PEBL_DEBUG_PRINT
    cout << "Popping Stack: depth: "<< mStack.size() << "-->" << GetStackDepth() - 1;
#endif 
    if(mStack.size() <=0 )
        {
            PError::SignalFatalError("Error: Tried to Pop an empty stack.");
        }
   
    Variant v = mStack.top();
    mStack.pop();
#ifdef PEBL_DEBUG_PRINT
    if(mStack.size())cout << "  [" << mStack.top() << "] is on top.\n";
    else cout << endl;
#endif 
    return v;
}


Variant Evaluator::Peek()
{
#ifdef PEBL_DEBUG_PRINT
    cout << "Peeking at top of stack: "<< mStack.size() << endl;
#endif 
    if(mStack.size() <=0 )
        {
            PError::SignalFatalError("Error: Tried to Peek at an empty stack.");
        }
   
    Variant v = mStack.top();
    return v;
}


bool Evaluator::IsVariableName(Variant v)
{
    return gGlobalVariableMap.Exists(v) || mLocalVariableMap.Exists(v);
}



void Evaluator::NodeStackPush(const PNode * node)
{
    mNodeStack.push(node);
}
