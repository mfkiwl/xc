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
//ThreedimMecLoad.h
                                                                        
#ifndef ThreedimMecLoad_h
#define ThreedimMecLoad_h


#include "ThreedimLoad.h"
#include "utility/geom/pos_vec/Pos3d.h"
class SlidingVectorsSystem3d;

namespace XC {
class Matrix;

//! @ingroup ElemLoads
//
//! @brief Mechanical load (forces) over three-dimensional elements.
class ThreedimMecLoad: public ThreedimLoad
  {
  public:
    ThreedimMecLoad(int tag, int classTag, const ID &theElementTags);
    ThreedimMecLoad(int tag, int classTag);

    static size_t getForceVectorDimension(void);
    
    virtual Vector getLocalForce(void) const;
    virtual Vector3d getVector3dLocalForce(void) const;
    virtual const Matrix &getLocalForces(void) const;
    virtual const Matrix &getGlobalVectors(const Matrix &) const= 0;
    virtual const Matrix &getGlobalForces(void) const;
    
    virtual SlidingVectorsSystem3d getResultant(const Pos3d &p= Pos3d(), bool initialGeometry= true) const;
  };
} // end of XC namespace

#endif

