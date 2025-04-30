//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//MeshEdges.cpp

#include "MeshEdges.h"
#include "utility/geom/d1/Polyline3d.h"
#include "domain/mesh/node/Node.h"
#include <set>

//! @brief Constructor.
XC::MeshEdges::MeshEdges(void)
  : CommandEntity() {}

//! @brief returns the loop that starts in vertex passed as parameter.
std::deque<const XC::MeshEdge *> XC::MeshEdges::getLoop(const MeshEdge *first) const
  {
    const Node *firstNode= first->getFirstNode();
    const Node *lastNode= first->getLastNode();
    std::set<const MeshEdge *> visited;
    visited.insert(first);
    std::deque<const MeshEdge *> retval;
    retval.push_back(first);
    const size_t maxSizeLoop= 2*size()-1; //Max. loop size.
    size_t count= 1;
    do
      {
        const MeshEdge *next= lastNode->next(*this,visited);
        if(next)
          {
            retval.push_back(next);
            visited.insert(next);
            first= next;
            lastNode= first->getLastNode();
          }
        else
          {
	    std::cerr << "MeshEdges::getLoop error; next edge not found." 
                      << std::endl;
            break;
          }
        count++;
        if(count>maxSizeLoop) //Something goes wrong.
          {
	    std::cerr << "MeshEdges::getLoop error; error in loop finding." 
                      << std::endl;
            break;
          }
      }
    while(firstNode!=lastNode);
    return retval;
  }

XC::MeshEdges::node_sequence getNodeSequenceFromLoop(const std::deque<const XC::MeshEdge *> &loop)
  {
    XC::MeshEdges::node_sequence retval;
    for(std::deque<const XC::MeshEdge *>::const_iterator i= loop.begin();i!=loop.end();i++)
      {
        const XC::Node *n= (*i)->getFirstNode();
        retval.push_back(n);
      }
    return retval;
  }

Polyline3d getPolylineFromLoop(const std::deque<const XC::MeshEdge *> &loop, const double &factor)
  {
    Polyline3d retval;
    Pos3d pt;
    XC::MeshEdges::node_sequence tmp= getNodeSequenceFromLoop(loop);
    for(XC::MeshEdges::node_sequence::const_iterator i= tmp.begin();i!=tmp.end();i++)
      {
	const XC::Node *n= *i;
        pt= n->getCurrentPosition3d(factor);
        retval.appendVertex(pt);
      }
    pt= retval.front(); //First point.
    retval.appendVertex(pt); //Polyline closed.
    return retval;
  } 


//! @brief Returns closed contours from the edge set.
std::deque<Polyline3d> XC::MeshEdges::getContours(const double &factor) const
  { return XC::getContours(*this,factor); }

//! @brief Returns node sequences corresponding to closed contours from the edge set.
std::deque<XC::MeshEdges::node_sequence> XC::MeshEdges::getContoursNodeSequences(void) const
  { return XC::getContoursNodeSequences(*this); }

//! @brief Returns node sequences corresponding to closed contours from the edge set.
boost::python::list XC::MeshEdges::getContoursNodeSequencesPy(void) const
  {
    boost::python::list retval;
    const std::deque<MeshEdges::node_sequence> tmp= this->getContoursNodeSequences();
    for(std::deque<MeshEdges::node_sequence >::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	boost::python::list retval_i;
	const MeshEdges::node_sequence &p_seq= *i;
	for(MeshEdges::node_sequence::const_iterator j= p_seq.begin(); j!= p_seq.end(); j++)
	  {
	    const Node *n= *j;
	    retval_i.append(boost::ref(n));
	  }
	retval.append(retval_i);
      }
    return retval;
  }

//! @brief returns node sequences corresponding to closed contours from the edge set.
std::deque<XC::MeshEdges::node_sequence> XC::getContoursNodeSequences(MeshEdges edges)
  {
    const size_t max_iter= 100;
    size_t iter= 0;
    std::deque<XC::MeshEdges::node_sequence> retval;
    if(!edges.empty())      
	do
	  {
	    MeshEdges::const_iterator i= edges.begin();
	    const MeshEdge *first= &(*i);
	    std::deque<const MeshEdge *> loop= edges.getLoop(first);
	    retval.push_back(getNodeSequenceFromLoop(loop));
	    edges= edges.getEdgesNotInLoop(loop);
	    iter++;
	    if(iter>max_iter)
	      {
		std::cerr << "getContours error; " << edges.size()
			  << " left, after " << iter
			  << " iterations." << std::endl;
		break;
	      }
	  }
	while(!edges.empty());
    return retval;
  }

//! @brief returns closed contours from the edge set.
std::deque<Polyline3d> XC::getContours(MeshEdges edges, const double &factor)
  {
    const size_t max_iter= 100;
    size_t iter= 0;
    std::deque<Polyline3d> retval;
    do
      {
	MeshEdges::const_iterator i= edges.begin();
        const MeshEdge *first= &(*i);
        std::deque<const MeshEdge *> loop= edges.getLoop(first);
        retval.push_back(getPolylineFromLoop(loop,factor));
	edges= edges.getEdgesNotInLoop(loop);
	iter++;
	if(iter>max_iter)
	  {
	    std::cerr << "getContours error; " << edges.size()
	              << " left, after " << iter << " iterations." << std::endl;
	    break;
	  }
      }
    while(!edges.empty());
    return retval;
  }

//! @brief Returns the edges that result from removing the edges from the loop passed as parameter.
XC::MeshEdges XC::MeshEdges::getEdgesNotInLoop(const std::deque<const MeshEdge *> &loop) const
  {
    MeshEdges retval;
    for(MeshEdges::const_iterator i= begin();i!=end();i++)
      {
	const MeshEdge *edgePtr= &(*i);
        std::deque<const MeshEdge *>::const_iterator j= find(loop.begin(),loop.end(),edgePtr);
	if(j==loop.end()) //Not found.
	  retval.push_back(*edgePtr);
      }
    return retval;
  }

//! @brief Prints edges information.
void XC::MeshEdges::print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      os << *i << ' '; 
  }
