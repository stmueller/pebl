//* -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*- */
/////////////////////////////////////////////////////////////////////////////////
//    Name:       src/base/VCG.cpp
//    Purpose:    Class  members that can produce data files
//                interpretable by VCG viewer.
//    Author:     Shane T. Mueller, Ph.D.
//    Copyright:  (c) 2003--2005 Shane T. Mueller <smueller@obereed.net>
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

#include "PNode.h"
#include "VCG.h"
#include "Variant.h"

#include <list>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <strstream> 

using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::list;
using std::ostream;


///
///Standard Constructor for Main VCG class
VCG::VCG():
    mNodeIndex((int)0),
    mHeader(strdup("/*PEBL VGC-Tool Graph for selected program.\n (C) 2003 Shane T. Mueller, Ph.D.*/\n"))
{
    mGraph = new VCGGraph();
}

///
///Standard Destructor for Main VCG class
VCG::~VCG()
{
    free(mHeader);
    delete mGraph;
}

void VCG::MakeGraph(const PNode * node)
{
    //First, fill up the mGraph structure by parsing the graph
    Evaluate(node, NULL, 1);
}

void VCG::PrintGraph()
{
    //First, put in some identifying information
    cout << mHeader;
    if(mGraph)
        {
            cout << (*mGraph) << endl;
        }
}


void VCG::Evaluate(const PNode * node, const char* parent, const int anchor)
{

    
    //First, generate a unique name for the node
    char * name;
    ostrstream ost1;
    ost1 << "NODE-" << mNodeIndex++;
    ost1.put(0);  //null terminate the ostring-stream
    name = strdup(ost1.str());
    
 
    //Now determine what type of node it is, and what
    //value resides within it.

    //Start making a label; use an ostrstream for easy concatenation

    ostrstream ost2;
    Variant variant;
  
    switch(node->GetType())
        {
        case PEBL_OP_NODE:

            ost2 << "OpNode\\n" << ((OpNode*)node)->GetOpName();
            break;

        case PEBL_DATA_NODE:
	    
            //retrieve tha vlaue from tha data node
            variant = ((DataNode*)node)->GetValue();

            ost2 << variant.GetDataTypeName() << "\\n" << variant;
    
            break;
	
        default:
            ost2 << "Unknown Node Type\\nUnknown Value";
	    
        }

    //Null-terminate the label stream and make it into a char array
    ost2.put(0);
    char* label = strdup(ost2.str());

    //Now, create a new node with all the right trimmings.
    cerr << "About to make node with:  ["<< name << "] and [" << label << "]." << endl;

    VCGNode * tmpNode=new VCGNode(name, label);

    //Change the shape and color of the node if it is an OpNode
    if(node->GetType() == PEBL_OP_NODE)
        {
            tmpNode->SetShape("ellipse");
            tmpNode->SetBorderColor("red");
        }
    else if(node->GetType() == PEBL_DATA_NODE)
        {
            tmpNode->SetShape("box");
            tmpNode->SetBorderColor("black");
      
        }
    else
        {
            tmpNode->SetShape("rhomb");
            tmpNode->SetBorderColor("orange");
	    
        }


    mGraph->AddNode(tmpNode);
 

    //Now, add an edge from this node's parent to the current node
    if(parent)
        {
            VCGEdge * tmpEdge = new VCGEdge(parent,name);
            tmpEdge->SetAnchor(anchor);
            mGraph->AddEdge(tmpEdge);
        }
    
    // Now, if we are an OpNode, evaluate each of the child nodes (if they exist)
    
    if(node->GetType() == PEBL_OP_NODE)
        {
            PNode *left, *right;
            left  = ((OpNode*)node)->GetLeft();
            right = ((OpNode*)node)->GetRight();
	    
            if(left)  Evaluate(left,  name, 1);
            if(right) Evaluate(right, name, 2);
        }

    //Clean up after ourselves
    free(name);
    free(label);
}





///
///Standard Constructor for the 'Graph' class
VCGGraph::VCGGraph():
    mTitle(strdup("VCG Graph File for PEBL-Interpreted PCode Tree")),
    mSplines(true),
    mWidth(700),
    mHeight(700),
    mX(30),
    mY(30),
    mColor(strdup("blue"))
{
  
 
}


///
///Standard Destructor for the 'Graph' class
VCGGraph::~VCGGraph()
{
    free(mTitle);
    free(mColor);
}


void VCGGraph::AddEdge(VCGEdge * edge)
{
    mVCGEdges.push_front(*edge);
}

void VCGGraph::AddNode(VCGNode * node)
{

    mVCGNodes.push_front(*node);
 
}



ostream & operator <<(ostream& out, VCGGraph & v)
{
  
    //Get Initial stuff:
	out << "graph: { title: \"" << v.GetTitle() << "\"\n";
	out << "         splines: no\n";
	out << "         layoutalgorithm: tree\n";
	out << "         smanhattan_edges: yes\n";
	out << "         finetuning: yes\n";
	out << "         portsharing: no\n";
	out << "         treefactor: 0.2\n";
	out << "         width: " << v.GetWidth() << endl;
	out << "         height: " << v.GetHeight() << endl;
	out << "         x: " << v.GetX() << endl;
	out << "         y: " << v.GetY() << endl;
	out << "         color: " << v.GetColor() << endl;

    //Now, output each Node;
    v.OutputNodes(out);

    //Now, output each edge;
    v.OutputEdges(out);


    //Cap off the end and return the entire stream
    out << "\n}" << endl;
    return out;
}

void  VCGGraph::OutputNodes(ostream &out)
{

  
    list<VCGNode>::iterator p = mVCGNodes.begin();
  
    //cerr << "/*Number of Nodes: " << mVCGNodes.size() << "*/"<< endl;
    while(p != mVCGNodes.end())
        {
            out << *p << endl;
            p++;
        }
    out << "\n";

}

void VCGGraph::OutputEdges(ostream & out)
{

    list<VCGEdge>::iterator p = mVCGEdges.begin();
  
    //  cout << "/*Number of Edges: " << mVCGEdges.size() << "*/" << endl;
    while(p != mVCGEdges.end())
        { 
            out << *p << endl;
            p++;
        }
    out << "\n";
 
}




///
///Standard Constructor for the 'Graph' class
VCGNode::VCGNode():
    mTitle(strdup("Default VCG Node Title")),
    mLabel(strdup("Default Label")),
    mShape(strdup("box")),
    mBorderColor(strdup("black"))
{
    //Standard Constructor

}


///
///Alternate  Constructor for the 'Graph' class
VCGNode::VCGNode(const char * title, const char * label ):
    mTitle(strdup(title)),
    mLabel(strdup(label)),
    mShape(strdup("box")),
    mBorderColor(strdup("black"))

{
    //Standard Constructor

}



///
///Standard Destructor for the 'Graph' class
VCGNode::~VCGNode()
{
    free(mTitle);
    free(mLabel);
    free(mShape);
    free(mBorderColor);
}


ostream & operator <<(ostream& out, VCGNode & v )
{
  
    //Output the beginning of a node:
    out << "node: { title: \"" << v.GetTitle() << "\"\n";
    out << "        label: \"" << v.GetLabel() << "\"\n";
    out << "        shape:  " <<  v.GetShape() << "\n";
    out << "        bordercolor: " << v.GetBorderColor() << "\n";
    out << "      }\n";
    return out;
}


///
///Standard Constructor for the 'Edge' class
VCGEdge::VCGEdge():
    mThickness(2),
    mFrom(0),
    mTo(0),
    mAnchor(1)
{

}


///
///Alternate Constructor for the 'Edge' class
VCGEdge::VCGEdge(const char* from, const char* to):
    mThickness(2),
    mFrom(strdup(from)),
    mTo(strdup(to)),
    mAnchor(1)
{
}


///
///Standard Destructor for the 'Edge' class
VCGEdge::~VCGEdge()
{
    free(mFrom);
    free(mTo);
  
}



ostream& operator <<(ostream& out, VCGEdge & v )
{
    //Output the beginning of a node:
    out << "edge: { sourcename: \"" << v.GetFrom() << "\"\n";
    out << "        targetname: \"" << v.GetTo() << "\"\n";
    out << "         thickness: " << v.GetThickness() << "\n";
    // out << "            anchor: " << v.GetAnchor() << "\n";
    out << "      }\n";
    return out;
}

