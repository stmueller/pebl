//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/VCG.h
//    Purpose:    Defines an class that can produce data files
//                interpretable by VCG viewer.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003-2025 Shane T. Mueller <smueller@obereed.net>
//    License:    GPL 2
//
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
#ifndef __VCG_H__
#define __VCG_H__
#include "PNode.h"

#include <iostream>
#include <list>
#include <stdlib.h>

using namespace std;

class VCGGraph;
class VCGNode;
class VCGEdge;

class VCG
{
public:
  
	VCG();
    ~VCG();

    void MakeGraph(const PNode * node); //This will initiate a new VCG
    void PrintGraph();
    void Evaluate(const PNode * node, const char* parent, const int anchor);
  

private:
    VCGGraph * mGraph;
    int mNodeIndex;
    char * mHeader;
};

class VCGGraph
{
public:
    VCGGraph();
    ~VCGGraph();
    friend std::ostream& operator <<(std::ostream& out,VCGGraph & v);
    void SetTitle(const char* title){if(mTitle)free(mTitle); mTitle=strdup(title);};
    void AddEdge(VCGEdge * edge);
    void AddNode(VCGNode * node);
    
    bool GetSplines()const{return mSplines;};
    char * GetTitle()const{return mTitle;}
    int GetWidth()const{return mWidth;};
    int GetHeight()const{return mHeight;};
    int GetX()const{return mX;};
    int GetY()const{return mY;};
    char* GetColor()const{return mColor;};
  
    void OutputNodes( std::ostream & out);
    void OutputEdges( std::ostream & out);

private:

    char* mTitle;
    std::list<VCGEdge> mVCGEdges;
    std::list<VCGNode> mVCGNodes;  
  
    bool mSplines;
    int mWidth;
    int mHeight;
    int mX;
    int mY;
    char* mColor;
  
};

class VCGNode
{
public:
    VCGNode();
    ~VCGNode();
    VCGNode(const char* title, const char* label);
  
    void SetTitle(const char* title){if(mTitle)free(mTitle);mTitle=strdup(title);};
    void SetLabel(const char* label){if(mLabel)free(mLabel);mLabel=strdup(label);};
    void SetShape(const char* shape){if(mShape)free(mShape);mShape=strdup(shape);};
    void SetBorderColor(const char * color){if(mBorderColor)free(mBorderColor); mBorderColor=strdup(color);};
    friend std::ostream& operator <<(std::ostream& out,VCGNode & v);
    char* GetTitle()const {return mTitle;};
    char* GetLabel()const {return mLabel;};
    char* GetShape()const {return mShape;};
    char* GetBorderColor()const {return mBorderColor;};
private:

    char * mTitle;
    char * mLabel;
    char * mShape;
    char * mBorderColor;
};


class VCGEdge
{
public:
    VCGEdge();
    ~VCGEdge();
    VCGEdge(const char* from , const char* to);

    void SetThickness(const int thickness){mThickness=thickness;};
    void SetFrom(const char * from){if(mFrom)free(mFrom);mFrom = strdup(from);};
    void SetTo(const char* to){if(mTo)free(mTo); mTo = strdup(to);};
    void SetAnchor(const int i){mAnchor = i;};
    friend std::ostream& operator <<(std::ostream& out,VCGEdge & v);

    int GetThickness()const {return mThickness;};
    char* GetFrom()const {return mFrom;};
    char* GetTo()const {return mTo;};
    int GetAnchor()const {return mAnchor;};

private:
    int mThickness;
    char* mFrom;   //Source node
    char* mTo;     //Destination Node
    int mAnchor;   //The point it is attaching to

};

#endif
