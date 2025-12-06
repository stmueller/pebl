//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/Evaluator.cpp
//    Purpose:    Defines an class that can evaluate PNodes (Recursive Evaluator)
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003--2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
//    ARCHITECTURE NOTE - Recursive vs Iterative Evaluator:
//
//    This is the RECURSIVE evaluator implementation. It uses C++ call stack
//    recursion to evaluate the PEBL abstract syntax tree (AST). Each call to
//    Evaluate() recurses through the AST using native C++ function calls.
//
//    EMSCRIPTEN INCOMPATIBILITY:
//    This recursive evaluator is INCOMPATIBLE with Emscripten builds that use
//    the Asyncify feature (required for async I/O and event handling). Testing
//    shows a hard recursion depth limit of 1 for PEBL user-defined recursive
//    functions when compiled to WebAssembly. Any recursion depth >= 2 crashes
//    with "index out of bounds" errors. This occurs because Emscripten's
//    Asyncify transformation rewrites the call stack to enable pause/resume
//    operations, which breaks the recursive evaluator's function call mechanism.
//
//    For Emscripten builds, use the iterative evaluator (Evaluator-es.cpp)
//    instead, which uses manual stack management and handles arbitrary recursion
//    depth correctly.
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

#include "Evaluator.h"

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>

#include "PNode.h"
#include "grammar.tab.hpp"
#include "VariableMap.h"
#include "Variant.h"
#include "PComplexData.h"
#include "PList.h"
#include "FunctionMap.h"

#include "../utility/PError.h"
#include "../utility/rc_ptrs.h"
#include "../utility/Defs.h"

#include "../objects/PCustomObject.h"
#include "../devices/PEventLoop.h"

#ifdef PEBL_EMSCRIPTEN
#include <emscripten.h>
#endif

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


    if(!mEventLoop)
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

    if(!mEventLoop)
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

    //Delete all local variables now.
    if( gCallStack.Size())
        gCallStack.Pop();


}


///
/// This is the generic PNode evaluator
///

bool Evaluator::Evaluate(const PNode * node)
{

    if(node == NULL) PError::SignalFatalError("Trying to evaluate null node\n");
    //Set up the globally-accessible structure to allow
    //better error reporting.  Only change it if the new node's
    //line number is greater than -1; if not, it is a PEBL-generated
    //node that won't give very good information.
    if(node->GetLineNumber() > -1)
        gEvalNode = node;

#ifdef PEBL_DEBUG_PRINT
    cout << "Line: " << node->GetLineNumber() << endl;
    cout << "PDP::Type: " << node->GetType() << endl;
    cout << "PDP::Type: " << node->GetType() << endl;
#endif

    if(node->GetType() ==  PEBL_OP_NODE)
        {
            return Evaluate((OpNode*)node);
        }
    else if (node->GetType() ==  PEBL_DATA_NODE)
        {
            return Evaluate((DataNode*)node);
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


///  This method evaluates OpNodes
bool Evaluator::Evaluate(const OpNode * node)
{

    unsigned long int numargs = 0;
    if(node == NULL) PError::SignalFatalError("Trying to evaluate null node\n");
    //Set up the globally-accessible structure to allow
    //better error reporting.
    if(node->GetLineNumber() > -1)
        gEvalNode = node;

#ifdef PEBL_DEBUG_PRINT
    cout << "-------------------------Evaluating OpNode ["<< node->GetOp() << "] of Type: " << node->GetOpName() << "------------\n";
#endif


    ///This switch could probably be improved with an STL Map
    switch(node->GetOp())
        {

        case NULL:
            break;


        case PEBL_ASSIGN:
            {
                //The left node should contain a DataNode of Variant type Variable.
                //The right node should contain an expression that evaluates to a number
                //that should be assigned to the datanode.


                const PNode * node2 = node->GetRight();
                Evaluate(node2);

                //The evaluated expression is now at the top of the stack.
                //Leave it there, because this statement should return that value; but assign
                //it to v2.
                Variant v2 = mStack.top();

                const PNode * node1 =node->GetLeft();

                //Extract the variable name from the node.

                Variant v1=((DataNode*)node1)->GetValue();


#ifdef PEBL_DEBUG_PRINT
                cout << "Initial Variable Name: [" << v1.GetVariableName() << "]" << endl;
#endif

                //Get the name of property being set
                string property =v1.GetVariablePropertyName();

                //Add the variable name/value pair to the appropriate map structure
                if(v1.IsLocalVariable())
                    {

                        if(property == "")
                            {
                                //Variable is added here.  What
                                //happens if it overwrites something?

                                mLocalVariableMap.AddVariable(v1.GetVariableName(), v2);
                            }
                        else
                            {
                                //otherwise get the object from the variable store
                                //and set its property.

                                Variant v3 = mLocalVariableMap.RetrieveValue(v1.GetVariableBaseName());
                                PEBLUtility::SetPropertyChain(v3, property, v2);
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
                                PEBLUtility::SetPropertyChain(v3, property, v2);
                            }

                    }


            }
            break;

        case PEBL_ADD:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate(node2);

                //Get the top two items from the stack.  The rightmost will
                //be on top.
                Variant v2 = Pop();
                Variant v1 = Pop();

                //There maybe should be more error checking to ensure
                //that the atoms are numbers.

                Push(v1+v2);
            }
            break;


        case PEBL_DIVIDE:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2);



                //Get the top two items from the stack
                Variant v2 = Pop();
                Variant v1 = Pop();

                Push(v1/v2);
            }
            break;


        case PEBL_MULTIPLY:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2 );


                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();

                Push(v1*v2);
            }
            break;


        case PEBL_POWER:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2);



                //Get the top two items from the stack
                Variant v2 = Pop();
                Variant v1 = Pop();

                Push(Variant(pow((pDouble)v1,(pDouble)v2)));
            }
            break;


        case PEBL_SUBTRACT:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2 );


                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();

                Push(v1 - v2);
            }
            break;


        case PEBL_AND:
            {
                //Evaluate left and right nodes, and do an AND of them.

                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2 );

                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();

                Push(v1 && v2);

            }
            break;

        case PEBL_OR:
            {
                //Evaluate left and right nodes, and do an or on them.

                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2 );

                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();

                Push(v1 || v2);


            }
            break;

        case PEBL_NOT:
            {
                //Evaluate left nodes, and negate it.

                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );

                //Get the top two items from the stack.  The right will be on top
                Variant v1 = Pop();

                Push(!v1);
            }
            break;


        case PEBL_IF:
            {

                //Left node is the expression test;
                //Right node is the code block to execute if true.

                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                Variant v1 = Pop();
                if(v1)
                    {
                        PNode * node2 = node->GetRight();
                        Evaluate( node2 );
                    }
                else
                    {
                        //If the IF is not true, we still need to leave something on the
                        // stack; leave a 0.
                        Push(0);

                    }
            }
            break;

        case PEBL_IFELSE:
            {
                //Left node is the expression test;
                //Right node is the THEN code block to execute.

                //Execute left node, which puts results on stack
                const PNode * node1=node->GetLeft();
                Evaluate( node1 );

                const PNode * node2 = node->GetRight();
                Evaluate(node2);
            }
            break;

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
                Evaluate(node1);
            }
            break;

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

                //Get the variable list.  This may have either empty or variable-pair default values.

                const PNode * node1 = node->GetLeft();

                //Get the argument list is at the top of the stack right now.

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


                //now, tmpList is a list containing the sequence of arguments fed into the function call.
                //They should map onto the first N arguments in the template.

                Variant v2 = 0;

                //iterate through the lists and assign values to variables.
                PList * arglist = (PList*)(tmpList.get());

                vector <Variant>::iterator p = arglist->Begin();
                Variant vdef = 0;  // Initialize to prevent uninitialized reads

                while(node1)
                    {
                        //Each variable could be either a variable name (global or local)
                        //or a varpair--a node pairing a global/local with a value.
                        vdef = 0;  // Reset to 0 each iteration (don't re-declare, would shadow outer vdef)

                        //This loads the default values initially when they appear,
                        //and then overwrites them if actual values exist.  This means
                        //the default value(if global) needs to exist--it probably shoudn't have to
                        bool hasdefault = false;
                        if(((OpNode*)(((OpNode*)node1)->GetLeft()))->GetOp()==PEBL_VARPAIR)
                            {
                           //this variable has a default value....
                                hasdefault = true;
                                //v2 is the variable name.
                                DataNode* varNameNode = ((DataNode*)(((OpNode*)(((OpNode*)node1)->GetLeft()))->GetLeft()));
                                v2 = varNameNode->GetValue();

                                // Get the default value node - could be DataNode or OpNode
                                PNode* defaultNode = ((OpNode*)(((OpNode*)node1)->GetLeft()))->GetRight();

                                if(defaultNode->GetType() == PEBL_OP_NODE)
                                    {
                                        // The default value is an expression (e.g., -1, Min(data)).
                                        // Evaluate it in the current function scope to get its value.
                                        // This allows referencing previously-bound parameters.
                                        Evaluate(defaultNode);
                                        vdef = Pop();
                                    }
                                else
                                    {
                                        //It is a data node--default value.
                                        DataNode* defValueNode = (DataNode*)defaultNode;
                                        vdef = defValueNode->GetValue();
                                    }
                            }
                        else

                            {
                                //it is a pure value here
                                v2 = ((DataNode*)(((OpNode*)node1)->GetLeft()))->GetValue();

                            }



                        // Now bind the parameter immediately (either from argument or default)
                        // This allows later parameters' defaults to reference earlier parameters.
                        if(p != arglist->End())
                            {
                                // We have an argument value - use it
                                mLocalVariableMap.AddVariable(v2, *p);
                                p++;
                                node1 = ((OpNode*)node1)->GetRight();
                            }
                        else if(hasdefault)
                            {
                                // No argument provided, but we have a default value
                                // Resolve variable references in the default value
                                if(vdef.IsGlobalVariable())
                                    {
                                        vdef = gGlobalVariableMap.RetrieveValue(vdef);
                                    }
                                else if (vdef.IsLocalVariable())
                                    {
                                        // A local variable may be bound to a previously-specified parameter
                                        vdef = mLocalVariableMap.RetrieveValue(vdef);
                                    }
                                mLocalVariableMap.AddVariable(v2, vdef);
                                node1 = ((OpNode*)node1)->GetRight();
                            }
                        else
                            {
                                // No argument and no default - too few parameters
                                string message = "Too few arguments passed to function [" + mScope + "].";

                                if(mScope == "Start")
                                    message += " (Make sure Start function has only one variable).";

                                PError::SignalFatalError(message);
                            }
                    }



                //if(arglist && arglist->Length() > 0)
                if(p != arglist->End())
                    {
                        //Too many arguments.
                        string message = string("");

                        if(mScope == "Start")
                            message += "Start() function must be Start(p)).";
                        else
                            message += "Too many arguments passed to function [" + mScope + "].";

                        
                        PError::SignalFatalError(message);
                    }


                //Now, get the code block and execute it.
                const PNode * node2 = node->GetRight();
                Evaluate(node2);
            }
            break;


        case PEBL_LIBRARYFUNCTION:
            {

                // This type of function is built in and precompiled.  The loader examines
                // the parse tree and
                // identifies each function that is used.

                //The left child of a PEBL_LIBRARYFUNCTION contains an PEBL_AND node containing two datanodes
                //each containing integers describing the min and max  number of arguments, respectively.


                //We should be able to tell the name of the function
                std::string name = node->GetFunctionName();

                const OpNode * node0 =(OpNode*)(node->GetLeft());
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

                        //      cout << "((("<<name<<"|"<<v2<<")))\n";

                        if(v2.IsComplexData())
                            {
                                if((v2.GetComplexData())->IsList())
                                    {
                                        //#if !defined(PEBL_EMSCRIPTEN)


                                        //Dont check parameter numbers in EMSCRIPTEN
                                        //cout << "[[[<"<<v2<<">]]]\n";
                                        tmp = (PList*)(v2.GetComplexData()->GetObject().get());
                                        numargs = tmp->Length();

                                        //#endif
                                    }
                                else
                                    {
                                        cout << "UNHANDLED ELSE CASE. NOT A LIST\n" ;
                                    }
                            }
                        //cout << "numargs now: " << numargs << endl;
                    }


                //#if !defined(PEBL_EMSCRIPTEN)
                if(numargs < min || numargs > max)
                    {

                        Variant message = Variant("In scope [") + mScope + Variant("]:") +
                            Variant("function [") +name+Variant("]:")+
                            Variant("Incorrect number of arguments..  Wanted between ")+
                            Variant((int)min) + Variant(" and ") +Variant((int)max) + Variant(" but got ") + Variant((int)numargs);


                        cout << message << endl;
                        PError::SignalFatalError(message);
                    }
                //#endif

                //Execute the functionpointer and push the results onto the stack.
                Push((v1.GetFunctionPointer())(v2));
            }

            break;


        case PEBL_FUNCTION:
            // This controls the execution of a function. (not the
            // loading of a defined function).  A function node
            // has a function datanode on its left child and an arglist (the head of a list of
            // arguments) on its right child.  Just pass these  along to the CallFunction Method.

            CallFunction(node);


            break;

        case PEBL_WHILE:
            {
                // The left child node is an expression to test
                // The right node is a code block.
                // Test the left node, then evaluate the right node if true.
                // Repeat until left node is false.

                Variant results=0;
                while(1)
                    {
                        //Execute left node, which puts results on stack
                        const PNode * node1=node->GetLeft();
                        Evaluate( node1 );

                        //Get the result of the evaluation
                        Variant v1 = Pop();

                        //If the evaluation is false, break out of loop/switch
                        if(!v1)
                            {
                                results = Variant(0);
                                break;
                            }


                        //Evaluate the right node.
                        const PNode * node2 = node->GetRight();
                        Evaluate(node2);

                        //Now, the results of the while block are on top of the stack.
                        //Pop them off so the stack doesn't grow; add them back at the end.
                        results = Pop();

                        if(results.GetDataType() == P_DATA_STACK_SIGNAL &&
                           results == Variant(STACK_BREAK))
                            {
                                results = Pop();
                                break;
                            }

                    }
                //Push the last results back onto the stack.
                Push(results);
            }
            break;


        case PEBL_LOOP:
            {
                // The PEBL_LOOP node has two children: the left child is a variable/datum pair,
                // and the right child is a code block that should be evaluated once for each
                // element of the datum, with the variable being set to that element for
                // each iteration.

                //Get the variabledatum pair:
                const PNode * node1 = node->GetLeft();

                Evaluate(node1);

                //Now, the top two items on the stack should be the datum and the variable.

                Variant v1 = Pop();
                Variant v2 = Pop();

                PError::AssertType(v2, PEAT_VARIABLE,"Error: iterator of loop not a variable");

                const PNode * node2 = node->GetRight();

                //Now, iterate through the elements of v1.  If v1 is not a list,
                //just set the variable equal to v1 and execute once.


                PList * tmp;
                if(v1.IsInteger())
                    {
                        tmp = new PList();
                        for(int count=1; count <= v1.GetInteger(); count++)
                            {
                                tmp->PushBack(count);
                            }

                        //make a list here containing numbers 1...v1
                    }else{

                        PError::AssertType(v1, PEAT_LIST,"Second argument of loop(<iterator>, <list>) must be a list.");
                        tmp = (PList*)(v1.GetComplexData()->GetObject().get());
                    }

                //Retrieve an iterator to the items in the list.
                //PComplexData * tmpPCD = v1.GetComplexData();

                vector<Variant>::const_iterator p = tmp->Begin();
                vector<Variant>::const_iterator end = tmp->End();
                Variant results=0;

                while(p != end)
                    {

                        //Set the variable to the current list element, and increment p
                        if(v2.IsLocalVariable())
                            {
                                mLocalVariableMap.AddVariable(v2, *p);
                            }
                        else if (v2.IsGlobalVariable())
                            {
                                gGlobalVariableMap.AddVariable(v2,*p);
                            }

                        p++;

                        //Execute the code block.
                        Evaluate(node2);

                        //Now, the results of the while block are on top of the stack.
                        //Pop them off so the stack doesn't grow; add them back at the end.
                        results = Pop();

                        if(results.GetDataType() == P_DATA_STACK_SIGNAL &&
                           results == Variant(STACK_BREAK))
                            {
                                p = end;
                                Pop();
                                results = Variant(0);

                            }
                    }
                //Push the last results back onto the stack.
                        Push(results);

            }
            break;

        case PEBL_VARIABLEDATUM:
            {
                //The VARIABLDATUM node is used in the loop function. The variable is the
                //left child, a list of data is the right. This node is executed once at the beginning
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
                Evaluate(node2);
            }
            break;


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
                        Evaluate(node1);
                    }

                //if node1 doesn't exist, it is a null argument list--do nothing more.


            }
            break;

        case PEBL_LISTHEAD:
            {
                //When we get a list head, we need to evaluate each expression in the list,
                //pushing them onto the stack. When we get to the end of the list, we just need
                //to pop items, adding them to the list,  until we get back up to the top.
                //To do this efficiently and avoid recreating lists iteratively, the bottom
                //item does this in a while loop.  to signal the end of the while loop, the
                //PEBL_LISTHEAD must put a dummy item on the stack so the bottom item
                //knows when the front of the list has been reached.
                //This is a variant of type STACK_LIST_HEAD

                //Create the stack signal variant and push it onto the stack.
                Variant v1 = Variant(STACK_LIST_HEAD);
                Push(v1);

                //Get the top node of the list and Evaluate it.
                const PNode * node1 = node->GetLeft();
                if(node1)
                    {
                        Evaluate(node1);
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
                Evaluate(node1);

                //The rest of the list;
                const PNode * node2 = node->GetRight();

                // If the right node not null, then we need to evaluate it until we get to the end
                // of the list. If it is null, we are at the end of the list; create
                // the beginning of a list and push it onto the stack.
                if(node2)
                    {
                        //Evaluate the tail of the list
                        Evaluate(node2);

                        //When above is done, a list object will be at the top
                        //of the stack.  don't touch it
                    }
                else
                    {


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

                                //cout << "::::" << v1 << endl;
                                //Add the item to the list.
                                tmpList->PushBack(v1);//This used to be PushFront
                                order.PushBack(ord--);
                                //Pop and repeat.
                                v1 = Pop();
                            }

                        //When tmpList had a list inside it, we could use PushFront to
                        //automatically reverse the list, which is now on the stack with
                        //the last element first.
                        //But now that it is a vector, it is inefficient to push onto the front
                        //And involves recopying every element), so we will just reverse it now
                        //that it is in a handy list format.

                        std::reverse(tmpList->Begin(),tmpList->End());

                        counted_ptr<PEBLObjectBase> pl = counted_ptr<PEBLObjectBase>(tmpList);
                        PComplexData  pcd =  PComplexData(pl);
                        Variant v2 = Variant(&pcd);
                        Push(v2);
                    }
            }
            break;

        case PEBL_LT:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2);


                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();



                Push(v1 < v2);
            }
            break;

        case PEBL_GT:
            {

                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2);

                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();
                Push(v1 > v2);
            }
            break;

        case PEBL_GE:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2);

                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();

                Push(v1 >= v2);
            }
            break;

        case PEBL_LE:
            {

                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2);


                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();

                Push(v1 <= v2);
            }
            break;

        case PEBL_EQ:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2);

                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();
                Push(v1 == v2);
            }
            break;

        case PEBL_NE:
            {
                //Execute left and right nodes, which puts results on stack
                const PNode * node1 = node->GetLeft();
                Evaluate( node1 );
                const PNode * node2 = node->GetRight();
                Evaluate( node2);

                //Get the top two items from the stack.  The right will be on top
                Variant v2 = Pop();
                Variant v1 = Pop();

                Push(v1 != v2);
            }
            break;

        case PEBL_STATEMENTS:
            {
                //This is a node that connects two statements. Since a statement leaves its
                //evaluation on the stack, this should pop the stack after evaluating the LEFT
                //statement. This means that the last statement in a series remains on the stack.

#ifdef PEBL_DEBUG_PRINT
                cout << "Checking a PEBL_STATEMENTS: " << GetStackDepth() << endl;
#endif

                //If the top of the stack is a STACK_BREAK, we should do nothing.
                if(GetStackDepth())
                    {

                        Variant v1 = Peek();


                        if(v1.GetDataType() == P_DATA_STACK_SIGNAL &&
                           v1 == Variant(STACK_BREAK))
                            {
                                //cout << "STACKBREAKING ON RIGHT\n";

                                //If this is a stack signal, and if
                                //it is a break, we should just back out
                                Push(Variant(0));  //why isn't this stack_break?
                                break;
                            }
                    }

                if(node->GetLeft())
                    {
                        Evaluate(node->GetLeft());
                        Variant v1 = Pop();


                        if(v1.GetDataType() == P_DATA_STACK_SIGNAL &&
                           v1 == Variant(STACK_BREAK))
                            {
                                //cout << "STACKBREAKING ON LEFT\n";
                                //If this is a stack signal, and if
                                //it is a break, we should just back out
                                Push(Variant(STACK_BREAK));
                                break;
                            }
                    }

                if(node->GetRight())
                    Evaluate(node->GetRight());
            }

            break;


        case PEBL_BREAK:
            {
                //This exits out of the current loop, while, or function context.
                //it works by adding a STACK_BREAK stacksignalevent onto the top of the stack.
                //all relevant loops look for this type of event and
                //cleanly abort when that times comes.

                //                cout << "PEBL_BREAK HANDLER\n";

                Variant v1 = Variant(STACK_BREAK);
                Push(v1);
                Push(v1);
#ifdef PEBL_DEBUG_PRINT
                cout << "PEBL_BREAK: pushing break onto stack.\n";
#endif

            }
            break;

        case PEBL_RETURN:
            {
                //The return keyword is used ONLY at the very end of a
                //function.  Here, we pushes a dummy variant
                //STACK_RETURN_DUMMY onto the stack and then evaluates
                //the left node.  Consequently, after the return
                //expression is evaluated, the stack depth will be two
                //instead of one. The CallFunction() method then
                //extracts the top of the stack from the function
                //scope and puts it on the caller's stack, otherwise
                //it puts a '1' to indicate that the function has
                //returned successfully.

                //This is done to avoid the potentially confusing
                //construct in LISP and others that the final
                //expression in a function is the return value.

                //This is a brittle way to signal a return value.

                //
                Push(Variant(STACK_RETURN_DUMMY));

                const PNode * node1 = node->GetLeft();
                Evaluate(node1);
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
bool Evaluator::Evaluate(const DataNode * node)
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
                        v2 = PEBLUtility::ResolvePropertyChain(v2, property);
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
                        v2 = PEBLUtility::ResolvePropertyChain(v2, property);
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


/// This method takes a PEBL_FUNCTION OpNode, which is comprised of
/// a P_DATA_FUNCTION DataNode on the left and a parameter list on
/// the right, Finds the function code in the FunctionMap, and
/// Evaluates that code.
void Evaluator::CallFunction(const OpNode * node)
{


    // First get the right node (the argument list) and evaluate it.
    // This will end up with a list Variant on top of the stack.
    const PNode *node1 = node->GetRight();

    Evaluate(node1);


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


                myEval.Evaluate(node2);

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
            Evaluate(node2);

            break;


        default:
            PError::SignalFatalError("Unknown Function Type in Evaluator::CallFunction");
            break;
                }

}

void Evaluator::Push(const Variant & v)
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


const Variant & Evaluator::Peek() const
{
#ifdef PEBL_DEBUG_PRINT
    cout << "Peeking at top of stack: "<< mStack.size() << endl;
#endif
    if(mStack.size() <=0 )
        {
            PError::SignalFatalError("Error: Tried to Peek at an empty stack.");
        }

    return mStack.top();
}


bool Evaluator::IsVariableName(Variant v)
{
    return gGlobalVariableMap.Exists(v) || mLocalVariableMap.Exists(v);
}
