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
                                                                        
// $Revision: 1.3 $
// $Date: 2008/08/26 16:49:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/ElasticShearSection2d.cpp,v $

#include "ElasticShearSection2d.h"
#include "material/section/repres/section_geometry/SectionGeometry.h"
#include "material/ResponseId.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include <utility/matrix/Matrix.h>



#include <cstdlib>

XC::Vector XC::ElasticShearSection2d::s(3);

//! @brief Default constructor.
XC::ElasticShearSection2d::ElasticShearSection2d(void)
  :BaseElasticSection2d(0, SEC_TAG_ElasticShear2d,3), parameterID(0) {}

//! @brief Constructor.
XC::ElasticShearSection2d::ElasticShearSection2d(int tag, MaterialHandler *mat_ldr)
  : BaseElasticSection2d(tag,SEC_TAG_ElasticShear2d,3,mat_ldr), parameterID(0)
  {}

const XC::Vector &XC::ElasticShearSection2d::getStressResultant(void) const
  {
    const Vector e= getSectionDeformation();
    s(0) = ctes_scc.EA()*e(0);
    s(1) = ctes_scc.EI()*e(1);    
    s(2) = ctes_scc.GAAlpha()*e(2);
    return s;
  }

const XC::Matrix &XC::ElasticShearSection2d::getSectionTangent(void) const
  { return ctes_scc.getSectionTangent3x3(); }

const XC::Matrix &XC::ElasticShearSection2d::getInitialTangent(void) const
  { return getSectionTangent(); }

const XC::Matrix &XC::ElasticShearSection2d::getSectionFlexibility(void) const
  { return ctes_scc.getSectionFlexibility3x3(); }

const XC::Matrix &XC::ElasticShearSection2d::getInitialFlexibility(void) const
  { return getSectionFlexibility(); }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::ElasticShearSection2d::getCopy(void) const
  { return new ElasticShearSection2d(*this); }

const XC::ResponseId &XC::ElasticShearSection2d::getResponseType(void) const
  { return RespElasticShSection2d; }

int XC::ElasticShearSection2d::getOrder(void) const
  { return 3; }

//! @brief Returns the shear stiffness along y axis.
const double &XC::ElasticShearSection2d::GAy(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(2,2);
  }

//! @brief Send object members through the communicator argument.
int XC::ElasticShearSection2d::sendData(Communicator &comm)
  {
    int res= BaseElasticSection2d::sendData(comm);
    setDbTagDataPos(9,parameterID);
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ElasticShearSection2d::recvData(const Communicator &comm)
  {
    int res= BaseElasticSection2d::recvData(comm);
    parameterID= getDbTagDataPos(9);
    return res;
  }

int XC::ElasticShearSection2d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::ElasticShearSection2d::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
 
int XC::ElasticShearSection2d::setParameter(const std::vector<std::string> &argv,Parameter &param)
  { return ctes_scc.setParameter(argv,param,this); }

int XC::ElasticShearSection2d::updateParameter(int paramID, Information &info)
  { return ctes_scc.updateParameter(paramID,info); }

int XC::ElasticShearSection2d::activateParameter(int paramID)
  {
    parameterID = paramID;
    return 0;
  }

const XC::Vector &XC::ElasticShearSection2d::getStressResultantSensitivity(int gradIndex,bool conditional)
  {
    s.Zero();

    const Vector &e= getSectionDeformation();
    if(parameterID == 1)
      { // E
        s(0) = ctes_scc.A()*e(0);
        s(1) = ctes_scc.I()*e(1);
      }
    if(parameterID == 2)
      { // A
        s(0)= ctes_scc.E()*e(0);
        s(2)= ctes_scc.G()*ctes_scc.Alpha()*e(2);
      }
    if(parameterID == 3) // I
      s(1)= ctes_scc.E()*e(1);
    if(parameterID == 4) // G
      s(2)= ctes_scc.A()*ctes_scc.Alpha()*e(2);
    if(parameterID == 5) // alpha
      s(2)= ctes_scc.G()*ctes_scc.A()*e(2);
    return s;
  }

const XC::Matrix& XC::ElasticShearSection2d::getInitialTangentSensitivity(int gradIndex)
  { return ctes_scc.getInitialTangentSensitivity3x3(gradIndex); }
