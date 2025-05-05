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
//ShellMITC4Base.h

#ifndef ShellMITC4Base_h
#define ShellMITC4Base_h

#include "Shell4NBase.h"

namespace XC {

//! @ingroup PlaneElements
//
//! @brief Base class for MIT C4 shell elements.
class ShellMITC4Base: public Shell4NBase
  {
  protected:
    double Ktt; //!<drilling stiffness

    static const int ngauss= 4; //!< Number of gauss points.
    static const int nstress; //!< (8): three membrane, three moment, two shear
    mutable std::vector<Vector> strains; //!< strains at gauss points.
    std::vector<Vector> persistentInitialDeformation; //!< Persistent initial strain at element level. Used to store the deformation during the inactive phase of the element (if any).
    static ShellBData BData; //!< B-bar data


    void formResidAndTangent(int tang_flag) const;
    const Matrix calculateG(void) const;
    double *computeBdrill(int node, const double shp[3][4]) const;
    const Matrix& assembleB(const Matrix &Bmembrane, const Matrix &Bbend, const Matrix &Bshear) const;
    const Matrix& computeBmembrane(int node, const double shp[3][4] ) const;
    const Matrix& computeBbend(int node, const double shp[3][4] ) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    //null constructor
    ShellMITC4Base(int classTag,const ShellCrdTransf3dBase *);
    ShellMITC4Base(int tag,int classTag,const SectionForceDeformation *ptr_mat,const ShellCrdTransf3dBase *); 
    //full constructor
    ShellMITC4Base(int tag,int classTag, int node1, int node2, int node3, int node4, const SectionFDPhysicalProperties &,const ShellCrdTransf3dBase *);
    
    // Element birth and death stuff.
    const std::vector<Vector> &getPersistentInitialDeformation(void) const;
    void incrementPersistentInitialDeformationWithCurrentDeformation(void);

    int initializeCoordTransf(void);
    void setDomain(Domain *theDomain);
  
    //return stiffness matrix 
    const Matrix &getInitialStiff(void) const;

    void alive(void);

  }; 

} // end of XC namespace
#endif
