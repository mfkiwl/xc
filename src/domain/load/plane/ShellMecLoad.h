// -*-c++-*-
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
//ShellMecLoad.h
                                                                        
#ifndef ShellMecLoad_h
#define ShellMecLoad_h


#include "BidimMecLoad.h"

namespace XC {
class FVectorShell;

//! @ingroup ElemLoads
//
//! @brief Mechanical load (forces) over shell elements.
class ShellMecLoad: public BidimMecLoad<3>
  {
  public:
    ShellMecLoad(int tag, int classTag, const ID &theElementTags);
    ShellMecLoad(int tag, int classTag);

    virtual void addReactionsInBasicSystem(const std::vector<double> &,const double &,FVectorShell &) const;
    virtual void addFixedEndForcesInBasicSystem(const std::vector<double> &,const double &,FVectorShell &) const;

    static size_t getMomentVectorDimension(void);
    virtual Vector getLocalMoment(void) const;
    virtual const Matrix &getLocalMoments(void) const;
    virtual const Matrix &getGlobalMoments(void) const;
    virtual const Matrix &getGlobalVectors(const Matrix &) const;
    virtual Vector3d getVector3dLocalMoment(void) const;
    SlidingVectorsSystem3d getResultant(const Pos3d &centro, bool initialGeometry) const;
  };
} // end of XC namespace

#endif

