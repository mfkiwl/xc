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
                                                                        
// $Revision: 1.8 $
// $Date: 2003/02/14 23:01:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/ElasticSection2d.cpp,v $
                                                                        
                                                                        
///////////////////////////////////////////////////////
// File:  ~/Src/element/hinge/fiber_section/ElasticSection2d.cpp
//
// Written by Matthew Peavy
//
// Written:  Feb 13, 2000
// Debugged: Feb 14, 2000
// Revised:  May 2000 -- MHS (to include elastic shear)
//
//
// Purpose:  This file contains the function definitions
// for the XC::ElasticSection2d class.

#include <material/section/elastic_section/ElasticSection2d.h>
#include "material/section/repres/section_geometry/SectionGeometry.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>


#include "material/ResponseId.h"

XC::Vector XC::ElasticSection2d::s(2);

//! @brief Constructor.
//!
//! @param tag: identifier for the object.
//! @param mat_ldr: manager of the material objects.
XC::ElasticSection2d::ElasticSection2d(int tag, MaterialHandler *mat_ldr)
  : BaseElasticSection2d(tag,SEC_TAG_Elastic2d,2,mat_ldr) {}

//! @brief Default constructor.
XC::ElasticSection2d::ElasticSection2d(void)
  : BaseElasticSection2d(0,SEC_TAG_Elastic2d,2) {}


//! To construct an ElasticSection2D with an integer identifier {\em
//! tag}, an elastic modulus of \f$E\f$, a second moment of area \f$I\f$,
//! a section area of \f$A\f$.
//!
//! @param tag: identifier for the object.
//! @param E: elastic modulus
//! @param A: area.
//! @param I: moment of inertia.
XC::ElasticSection2d::ElasticSection2d(int tag, double E, double A, double I)
  : BaseElasticSection2d(tag,SEC_TAG_Elastic2d,2,E,A,I,0.0,0.0)
  {}

XC::ElasticSection2d::ElasticSection2d(int tag, double EA, double EI)
  :BaseElasticSection2d(tag, SEC_TAG_Elastic2d,2,1,EA,EI,0.0,0.0)
  {}

/**
 @brief Returns the cross-section stress resultant.

 Returns the section stress resultants, \f$ssec\f$, the product of the 
 section stiffness matrix, \f$ksec\f$, and the section deformation 
 vector, \f$esec\f$,
 \f[
 ssec = ksec esec = \left[
    \begin{array}{c}
        P\\
        M_z\\
        V_y
    \end{array} 
  \right]
 \f]
 where \f$P\f$ is the axial force, \f$M_z\f$ is the bending moment about the
 local z-axis, and \f$V_y\f$ is the shear force along the local y-axis.
*/
const XC::Vector &XC::ElasticSection2d::getStressResultant(void) const
  {
    const Vector &e= getSectionDeformation();
    s(0)= ctes_scc.EA()*e(0);
    s(1)= ctes_scc.EI()*e(1);
    return s;
  }

/**
 @brief Returns the tangent stiffness matrix.

 \f[
 fsec = \left[
    \begin{array}{ccc}
        EA &  0 &  0\\
         0 & EI &  0\\
         0 &  0 & \alpha GA
    \end{array} 
  \right]
 \f]
*/
const XC::Matrix &XC::ElasticSection2d::getSectionTangent(void) const
  { return ctes_scc.getSectionTangent2x2(); }


//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::ElasticSection2d::getInitialTangent(void) const
  { return getSectionTangent(); }

/**
 @brief Returns the flexibility matrix.

 \f[
 fsec = \left[
    \begin{array}{ccc}
        \frac{1}{EA} &            0 &                  0\\
                   0 & \frac{1}{EI} &                  0\\
                   0 &            0 & \frac{1}{\alpha GA}
    \end{array} 
  \right]
 \f]
*/
const XC::Matrix &XC::ElasticSection2d::getSectionFlexibility(void) const
  { return ctes_scc.getSectionFlexibility2x2(); }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::ElasticSection2d::getInitialFlexibility(void) const
  { return getSectionFlexibility(); }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::ElasticSection2d::getCopy(void) const
  { return new ElasticSection2d(*this); }

/**
 @brief Section stiffness contribution response identifiers.

 Returns the section ID code that indicates the ordering of
 section response quantities. For this section, axial response is the
 first quantity, bending about the local z-axis is the second, and
 shear along the local y-axis is third.
 \f[
 code := \left[
   \begin{array}{c}
       2\\
       1\\
       3
   \end{array} 
 \right]
 \f]
*/
const XC::ResponseId &XC::ElasticSection2d::getResponseType(void) const
  { return RespElasticSection2d; }

//! @brief Return 3.
int XC::ElasticSection2d::getOrder(void) const
  { return 2; }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ElasticSection2d::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Sends object through the communicator argument.
int XC::ElasticSection2d::sendSelf(Communicator &comm)
  {
    const DbTagData &dbTagData= getDbTagData();
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(dbTagData.Size());
    int res= sendData(comm);

    res+= comm.sendIdData(dbTagData,dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ElasticSection2d::recvSelf(const Communicator &comm)
  {
    DbTagData &dbTagData= getDbTagData();
    inicComm(dbTagData.Size());
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(dbTagData,dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Print stuff.
void XC::ElasticSection2d::Print(std::ostream &s, int flag) const
  {
    s << "ElasticSection2d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

