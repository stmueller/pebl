//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/PNode.cpp
//    Purpose:    Primary data structure for code
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
#include "PNode.h"
#include "grammar.tab.hpp"
#include "../utility/Defs.h"
#include <iostream>
#include <string>
#include <stdlib.h>

using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::ostream;
using std::string;

#undef VERBOSE_PNODE_CONSTRUCTION_MESSAGES

/// This is the standard pNode constructor
/*PNode::PNode(PNODE_TYPE type):
    mType(type),
    mTrace(0),
    mSourceFile("<Unknown File>"),
    mLineNumber(0)
 {

};
*/
PNode::PNode():
    mType(PEBL_UNDEFINED_NODE),
    mTrace(0),
    mSourceFile(""),
    mLineNumber(0),
    mFunctName("")
{


}

/// This is the standard pNode constructor
PNode::PNode(PNODE_TYPE type, const string & filename, int linenumber):
    mType(type),
    mTrace(0),
    mSourceFile(filename),
    mLineNumber(linenumber),
    mFunctName("")
{

}


PNode::PNode(const PNode & pn)
{
  mType = (pn.GetType());
  SetFileInfo(pn.GetFilename(),pn.GetLineNumber());

}
/// This is the standard pNode destructor
PNode::~PNode()
{
    // Standard Destructor
}


//Overload of the << operator
ostream& operator<<(ostream& out, const PNode & node)
{
    node.SendToStream(out);
    return out;
}

ostream & PNode::SendToStream(ostream & out) const
{
    out << "<Anonymous PNode of Type: " << GetType() << ">" << flush;
    return out;
}

void PNode::SetFileInfo(const string & filename, int linenumber)
{
    mSourceFile = filename;
    mLineNumber = linenumber;
}
void PNode::SetFunctionName(const std::string & funcname)
{
    mFunctName = funcname;
}

void PNode::DestroyChildren()
{
    //A generic PNode doesn't necessarily have children, so
    //don't do anything.
}

//******************************************************************************
//  OpNode
//******************************************************************************
/*

OpNode::OpNode(int op, PNode *left, PNode *right):
    PNode(PEBL_OP_NODE,"", 0),
    mOp(op),
    mLeft(left),
    mRight(right)

 {

#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cerr << "\tConstructing PNode of type [" << op << "]:[" << GetOpName() <<"]\n";
#endif

}
*/


OpNode::OpNode(int op, PNode *left, PNode *right, const std::string & filename, int linenumber):
    PNode(PEBL_OP_NODE,filename,linenumber),
    mOp(op),
    mLeft(left),
    mRight(right)
{

#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cerr << "\tConstructing PNode of type [" << op << "]:[" << GetOpName() <<"]\n";
#endif


}

OpNode::OpNode(const OpNode & op):
    PNode(op),
    mOp(0),
    mLeft(NULL),
    mRight(NULL)
{
    mOp = op.GetOp();
    mLeft = op.GetLeft();
    mRight = op.GetRight();
}

OpNode OpNode::operator = (const OpNode & op)
{

    mOp = op.GetOp();
    mLeft = op.GetLeft();
    mRight = op.GetRight();
    return *this;
}


void OpNode::DestroyChildren()
{
    //cerr << "destroying children\n";
    if(mLeft)
        {
            mLeft->DestroyChildren();
            delete mLeft;
            mLeft=NULL;
        }
    if(mRight)
        {
            mRight->DestroyChildren();
            delete mRight;
            mRight=NULL;
        }
}


// The original parse tree contains a high-level tree of PEBL_FUNCTIONS
// nodes.  The branches get picked off the tree and placed in a functionmap,
// and get destroyed when the functionmap get destroyed.  But, the top of the
// original parse tree remains, causing a memory leak that never gets cleaned up
// appropriately, unless this is called after Loader::GetFunctions
void OpNode::DestroyFunctionTree()
{

    if(GetOp() == PEBL_FUNCTION)
        {
            //This is a function node, which means we should remove
            //the datanode on the left.  The right node is a lambda function, which
            //gets taken over by the function map.
            if(mLeft)
                {
                    delete mLeft;
                    mLeft = NULL;
                }
        }
    else if(GetOp() == PEBL_FUNCTIONS)
        {
            //This is a PEBL_FUNCTIONS node; it contains links to other PEBL_FUNCTIONS
            //as well as PEBL_FUNCTION nodes.  Destroy them as well.

            if(mLeft)
                {
                    ((OpNode*)mLeft)->DestroyFunctionTree();
                    delete mLeft;
                    mLeft = NULL;
                }
            if(mRight)
                {
                    ((OpNode*)mRight)->DestroyFunctionTree();
                    delete mRight;
                    mRight = NULL;
                }

        }

}



//Standard Destructor.  This doesn't automatically destroy children
//nodes--to do that, use DestroyChildren();
OpNode::~OpNode()
{
#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cout <<" deleting  OpNode: " << *this << endl;
#endif
}


std::string  OpNode::GetOpName() const
{
    switch(GetOp())
        {

        case PEBL_ADD:        return "PEBL_ADD";
        case PEBL_AND:        return "PEBL_AND";
        case PEBL_ARGLIST:    return "PEBL_ARGLIST";
        case PEBL_ASSIGN:     return "PEBL_ASSIGN";
        case PEBL_BREAK:      return "PEBL_BREAK";
        case PEBL_COMMA:      return "PEBL_COMMA";
        case PEBL_DIVIDE:     return "PEBL_DIVIDE";
        case PEBL_DEFINE:     return "PEBL_DEFINE";
        case PEBL_DOT:        return "PEBL_DOT";
        case PEBL_ELSE:       return "PEBL_ELSE";
        case PEBL_END:        return "PEBL_END";
        case PEBL_EQ:         return "PEBL_EQ";
        case PEBL_FUNCTION:   return "PEBL_FUNCTION";
        case PEBL_FUNCTIONS:  return "PEBL_FUNCTIONS";
        case PEBL_GE:         return "PEBL_GE";
        case PEBL_GT:         return "PEBL_GT";
        case PEBL_IF:         return "PEBL_IF";
        case PEBL_IFELSE:     return "PEBL_IFELSE";
        case PEBL_LAMBDAFUNCTION:    return "PEBL_LAMBDAFUNCTION";
        case PEBL_LBRACE:     return "PEBL_LBRACE";
        case PEBL_LBRACKET:   return "PEBL_LBRACKET";
        case PEBL_LIBRARYFUNCTION:   return "PEBL_LIBRARYFUNCTION";
        case PEBL_LISTHEAD:   return "PEBL_LISTHEAD";
        case PEBL_LISTITEM:   return "PEBL_LISTITEM";
        case PEBL_LE:         return "PEBL_LE";
        case PEBL_LOOP:       return "PEBL_LOOP";
        case PEBL_LPAREN:     return "PEBL_LPAREN";
        case PEBL_LT:         return "PEBL_LT";
        case PEBL_MULTIPLY:   return "PEBL_MULTIPLY";
        case PEBL_NE:         return "PEBL_NE";
        case PEBL_NEWLINE:    return "PEBL_NEWLINE";
        case PEBL_NOT:        return "PEBL_NOT";
        case PEBL_OR:         return "PEBL_OR";
        case PEBL_POWER:      return "PEBL_POWER";
        case PEBL_RBRACE:     return "PEBL_RBRACE";
        case PEBL_RBRACKET:   return "PEBL_RBRACKET";
        case PEBL_RETURN:     return "PEBL_RETURN";
        case PEBL_RPAREN:     return "PEBL_RPAREN";
        case PEBL_SEMI:       return "PEBL_SEMI";
        case PEBL_START:      return "PEBL_START";
        case PEBL_STATEMENTS: return "PEBL_STATEMENTS";
        case PEBL_SUBTRACT:   return "PEBL_SUBTRACT";
        case PEBL_UMINUS:     return "PEBL_UMINUS";
        case PEBL_VARLIST:     return "PEBL_VARLIST";
        case PEBL_VARIABLEDATUM: return "PEBL_VARIABLEDATUM";
        case PEBL_WHILE:     return "PEBL_WHILE";

        case PEBL_FLOAT:      return "PEBL_FLOAT";
        case PEBL_INTEGER:    return "PEBL_INTEGER";
        case PEBL_STRING:     return "PEBL_STRING";
        case PEBL_SYMBOL:     return "PEBL_SYMBOL";
        case PEBL_FUNCTIONNAME:  return "PEBL_FUNCTIONNAME";



        case   PEBL_AND_TAIL: return "PEBL_AND_TAIL";
        case   PEBL_ADD_TAIL: return "PEBL_ADD_TAIL";
        case   PEBL_ASSIGN_TAIL: return "PEBL_ASSIGN_TAIL";
        case   PEBL_BREAK_TAIL : return "PEBL_BREAK_TAIL";
        case   PEBL_DIVIDE_TAIL : return  "PEBL_DIVIDE_TAIL";
        case   PEBL_EQ_TAIL: return   "PEBL_EQ_TAIL";
        case   PEBL_GE_TAIL : return   "PEBL_GE_TAIL";
        case   PEBL_GT_TAIL : return   "PEBL_GT_TAIL";
        case   PEBL_IF_TAIL : return "PEBL_IF_TAIL";
        case   PEBL_LE_TAIL : return "PEBL_LE_TAIL";
        case   PEBL_LISTITEM_TAIL : return   "PEBL_LISTITEM_TAIL";
        case   PEBL_LOOP_TAIL1  : return    "PEBL_LOOP_TAIL1";
        case   PEBL_LOOP_TAIL2: return  "PEBL_LOOP_TAIL2";
        case   PEBL_LT_TAIL: return  "PEBL_LT_TAIL";
        case   PEBL_MULTIPLY_TAIL: return  "PEBL_MULTIPLY_TAIL";
        case   PEBL_NE_TAIL: return    "PEBL_NE_TAIL";
        case   PEBL_NOT_TAIL: return    "PEBL_NOT_TAIL";
        case   PEBL_OR_TAIL: return    "PEBL_OR_TAIL";
        case   PEBL_POWER_TAIL: return    "PEBL_POWER_TAIL";
        case   PEBL_RETURN_TAIL: return    "PEBL_RETURN_TAIL";
        case   PEBL_SUBTRACT_TAIL: return    "PEBL_SUBTRACT_TAIL";
        case   PEBL_STATEMENTS_TAIL1: return       "PEBL_STATEMENTS_TAIL1";
        case   PEBL_STATEMENTS_TAIL2: return      "PEBL_STATEMENTS_TAIL2";
        case   PEBL_WHILE_TAIL: return             "PEBL_WHILE_TAIL";
        case   PEBL_WHILE_TAIL2: return            "PEBL_WHILE_TAIL2";
        case   PEBL_FUNCTION_TAIL1: return            "PEBL_FUNCTION_TAIL1";
        case   PEBL_FUNCTION_TAIL2: return           "PEBL_FUNCTION_TAIL2";
        case   PEBL_FUNCTION_TAIL_LIBFUNCTION: return "PEBL_FUNCTION_TAIL_LIBFUNCTION";


        default:              return "UNKNOWN PEBL OPERATION";

        }
}



//Overload of the << operator

ostream& OpNode::SendToStream(ostream& out) const
{

    out << "<OpNode of Type: " << GetOpName() << ">" <<flush;
    return out;

}


//******************************************************************************
//  DataNode
//******************************************************************************

/*
DataNode::DataNode(const Variant value):
    PNode(PEBL_DATA_NODE, "", 0)
{
    //This automatically makes  a deep copy (I think)
#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cerr << "\tConstructing DataNode of variant value " << flush << value << endl;
#endif
    mValue = value;
}
*/

DataNode::DataNode(const Variant value,const std::string &  filename, int linenumber):
    PNode(PEBL_DATA_NODE, filename, linenumber),
    mValue(0)
{
    //This automatically makes  a deep copy (I think)
#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cerr << "\tConstructing DataNode of variant value " << flush << value << endl;
#endif

    mValue = value;
}

/*
DataNode::DataNode():
    PNode(PEBL_DATA_NODE)
{

#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cerr << "\tConstructing DataNode without value \n";
#endif
}
*/

DataNode::DataNode(const string &  filename, int linenumber):
    PNode(PEBL_DATA_NODE, filename, linenumber),
    mValue((int)0)
{

#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cerr << "\tConstructing DataNode without value \n";
#endif
    mValue = Variant();
}


/*
DataNode::DataNode(pInt ivalue):
    PNode(PEBL_DATA_NODE)
{
    mValue = ivalue;

#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cerr << "\tConstructing DataNode of Integer value " << flush << ivalue << endl;
#endif

};
*/


DataNode::DataNode(pInt ivalue, const string & filename, int linenumber):
    PNode(PEBL_DATA_NODE, filename, linenumber),
    mValue(ivalue)
{
//    mValue = ivalue;

#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cerr << "\tConstructing DataNode of Integer value " << flush << ivalue << endl;
#endif

}

/*
DataNode::DataNode(pDouble fvalue):
    PNode(PEBL_DATA_NODE)
{
    mValue = fvalue;
#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cerr << "\tConstructing DataNode of Float value " << flush << fvalue << endl;
#endif
};
*/


DataNode::DataNode(pDouble fvalue, const string & filename, int linenumber):
    PNode(PEBL_DATA_NODE, filename, linenumber),
    mValue(fvalue)
{
//    mValue = fvalue;
#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cerr << "\tConstructing DataNode of Float value " << flush << fvalue << endl;
#endif
}


DataNode::~DataNode()
{
#ifdef VERBOSE_PNODE_CONSTRUCTION_MESSAGES
    cout <<" deleting  DataNode: " << *this << endl;
#endif
}

//Overload of the << operator
ostream& DataNode::SendToStream(ostream& out) const
{
    out << "<DataNode of Type: " << GetType() << " and Value: " << GetValue()<< ">" << flush;
    return out;
}
