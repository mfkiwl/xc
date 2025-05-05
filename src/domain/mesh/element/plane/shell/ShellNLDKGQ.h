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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.10 $
// $Date: 2014/09/30 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/ShellNLDKGQ/ShellNLDKGQ.h,v $

//Written: Lisha Wang, Xinzheng Lu, Linlin Xie, Song Cen & Quan Gu
//
//  four node flat shell element with membrane and drill DOF
//  considering geometric nonlinear, form nonlinear shell element
//  using updated Lagrangian formula
// Ref: Plate Bending Part - DKQ, thin plate element
//      Membrane Part - GQ12, a membrane element with drilling DOF
//

#ifndef ShellNLDKGQ_h
#define ShellNLDKGQ_h

#include "Shell4NBase.h"
#include "domain/mesh/element/utils/coordTransformation/ShellNLCrdTransf3d.h"

namespace XC {

//! @brief four node flat shell element with membrane and drill DOF
//!  considering geometric nonlinear, form nonlinear shell element
//!  using updated Lagrangian formula.
//!
//! <a href="http://www.luxinzheng.net/publication7/2017-JEE-DKGQ_OpenSees.htm">A high-performance quadrilateral flat shell element for seismic collapse simulation of tall buildings and its implementation in OpenSees</a>
class ShellNLDKGQ: public Shell4NBase
  {
  private: 
    static ShellNLCrdTransf3d non_linear_trf;    

    //last resid
    // Vector CstrainGauss,TstrainGauss;
    Vector CstrainGauss;
    mutable Vector TstrainGauss;  //modify for geometric nonlinearity
    
    //start Yuli Huang (yulihuang@gmail.com) & Xinzheng Lu (luxz@tsinghua.edu.cn)
    void updateBasis(void) const;
    //end Yuli Huang (yulihuang@gmail.com) & Xinzheng Lu (luxz@tsinghua.edu.cn)

    //inertia terms
    void formInertiaTerms(int tangFlag) const;

    //form residual and tangent					  
    void formResidAndTangent(int tangFlag) const;

    //compute Bdrill matrix
    //double* computeBdrill( int node, const double shp[3][4]);

    //assemble a B matrix 
    const Matrix& assembleB( const Matrix &Bmembrane,
					   const Matrix &Bbend, 
					   const Matrix &Bshear ) const;

    //compute Bmembrane matrix
    const Matrix& computeBmembrane( int node, const double shp[3][4],
					   const double shpDrill[4][4]) const;

    //compute Bbend matrix
    const Matrix& computeBbend( int node, const double shpBend[6][12] ) const;

    //add for geometric nonlinearity
    const Matrix& computeBG(int node, const double shpBend[6][12]) const;
    const Vector& computeNLdstrain(const Matrix &BG,const Vector &dispIncLocalBend) const;

    //shape function routine for four node quads
    static void shape2d( double ss, double tt, 
			    const double x[2][4], 
			    double shp[3][4], 
			    double &xsj ,double sx[2][2]);

    //shape function routine for membrane
    static void shapeDrill(double ss, double tt, const double x[2][4],
		    double sx[2][2], double shpDrill[4][4]);
    //shape function routine for bending
    static void shapeBend(double ss, double tt, const double x[2][4], double sx[2][2], double shpBend[6][12]);
    
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    ShellNLDKGQ(void);
    ShellNLDKGQ(int tag, const SectionForceDeformation *);
    ShellNLDKGQ(int tag, 
	 int node1, int node2, int node3, int node4,
	 const SectionForceDeformation &theMaterial);
    Element *getCopy(void) const;

    int resetNodalCoordinates(void);
    void setDomain(Domain *theDomain);

      
    int commitState(void); //commit state
    int revertToLastCommit(void); //revert to last commit 
    int revertToStart(void); //revert to start 

    //print out element data
    void Print(std::ostream &os, int flag) const;

    //return stiffness matrix 
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  }; 

} // end of XC namespace
#endif
