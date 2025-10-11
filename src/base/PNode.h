//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/PNode.h
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
#ifndef __PNODE_H__
#define __PNODE_H__

#include "Variant.h"         //This contains definition for enum VariantDataType
#include <string>
#include "../utility/Defs.h"
enum PNODE_TYPE {
    PEBL_UNDEFINED_NODE,
    PEBL_OP_NODE,
    PEBL_DATA_NODE
};


/// PNode is a simple class.  It just contains constructors and destructors
/// The evaluator is held in another class, to hopefully make things more
/// easily maintainable.  It may not really be worthwhile.
///

class PNode {

public:

    ///The Standard constructor.
    PNode();

    PNode(const PNode & pn);
    PNode(const std::string & filename, int linenumber);
 //   PNode (const PNode * pn);
    //    PNode(PNODE_TYPE type);
    PNode(PNODE_TYPE type, const std::string & filename, int linenumber);
    ///The Standard destructor.
    virtual ~PNode();

    ///Access mType data
    PNODE_TYPE GetType() const {return mType;};


    //Overload of the << operator
    friend std::ostream& operator<<(std::ostream& out, const PNode & node );

    void SetFileInfo(const std::string & filename, int linenumber);
    std::string  GetFilename()const {return mSourceFile;};
    int GetLineNumber()const {return mLineNumber;};
    void SetFunctionName(const std::string &funcname);
    std::string GetFunctionName()const{return mFunctName;};

    virtual void DestroyChildren();
private:

protected:

    //Inheritable printing Method.
    virtual std::ostream& SendToStream(std::ostream& out)const;


    ///Stores what type of node
    ///This may allow for easier run-time type identification
    PNODE_TYPE mType;


    ///Whether to produce a trace
    int mTrace;


    ///Source file of origin.
    std::string  mSourceFile;

    ///Closest Line number of origin.
    int mLineNumber;

    std::string mFunctName;
};



class OpNode: public PNode
{
public:
    //Constructor
    //OpNode(int type, PNode *left, PNode *right);
    OpNode(int type, PNode *left, PNode *right,const  std::string & filename, int linenumber);
    OpNode(const OpNode &);
    virtual ~OpNode();
    OpNode operator = (const OpNode & op);
    int GetOp()const{return mOp;};
    std::string GetOpName() const;

    PNode*  GetLeft() const {return mLeft;};
    PNode*  GetRight() const{return mRight;};

    virtual void DestroyChildren();
    virtual void DestroyFunctionTree();


private:
protected:

    virtual std::ostream& SendToStream(std::ostream& out) const;
    int mOp;

    ///A link to the left-child node
    PNode *mLeft;

    ///A link to the right-child node
    PNode *mRight;
};


class DataNode: public PNode
{
public:
    //    DataNode(const Variant value);
    //    DataNode();
    //    DataNode(pInt ivalue);
    //    DataNode(pDouble fvalue);
    DataNode(const Variant value, const  std::string & filename, int linenumber);
    DataNode(const std::string & filename, int linenumber);
    DataNode(pInt ivalue, const std::string & filename, int linenumber);
    DataNode(pDouble fvalue, const std::string & filename, int linenumber);


    virtual ~DataNode();
    VariantDataType GetDataType()const{return mValue.GetDataType();};

    //GetValue() returns a const reference to mValue to avoid copy issues in Emscripten
    const Variant& GetValue()const{return mValue;};



private:

protected:
    virtual std::ostream& SendToStream(std::ostream& out) const;


    ///This union stores the primary data for the node.
    ///It will either be numeric value, a string, or a symbol.
    Variant mValue;
};



#endif
