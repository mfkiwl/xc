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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/GenericSection1d.cpp,v $
                                                                        
                                                                        
// File: ~/material/GenericSection1d.C
//
// Written: MHS 
// Created: Apr 2000
// Revision: A
//
// Description: This file contains the class implementation for XC::GenericSection1d.
//
// What: "@(#) GenericSection1d.C, revA"

#include "GenericSection1d.h"
#include <material/uniaxial/UniaxialMaterial.h>
#include <domain/mesh/element/utils/Information.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <classTags.h>
#include <cstring>
#include "material/ResponseId.h"

XC::Vector XC::GenericSection1d::s(1);
XC::Matrix XC::GenericSection1d::ks(1,1);
XC::ResponseId XC::GenericSection1d::c(1);

//! @brief Allocates material.
void XC::GenericSection1d::alloc(const UniaxialMaterial &m)
  {
    free();
    theModel= m.getCopy();
    if(!theModel)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to get copy of material model.\n";
        exit(-1);
      }
  }

//! @brief Frees memory.
void XC::GenericSection1d::free(void)
  {
    if(theModel)
      delete theModel;
    theModel= nullptr;
  }
//! @brief Constructor.
//!
//! Constructs a GenericSection1D whose unique integer tag among
//! SectionForceDeformation objects in the domain is given by \p tag. Obtains
//! a copy of the UniaxialMaterial \p m via a call to getCopy().
//! The section code is set to be \p code.
XC::GenericSection1d::GenericSection1d(int tag, UniaxialMaterial &m, int type)
  :PrismaticBarCrossSection(tag,SEC_TAG_Generic1d), theModel(nullptr), code(type)
  { alloc(m); }

//! @brief Constructor.
XC::GenericSection1d::GenericSection1d(int tag)
  :PrismaticBarCrossSection(tag,SEC_TAG_Generic1d), theModel(nullptr), code(0)
  {}

//! @brief Constructor.
XC::GenericSection1d::GenericSection1d(const GenericSection1d &other)
  :PrismaticBarCrossSection(other), theModel(nullptr), code(other.code)
  {
    if(other.theModel)
      alloc(*other.theModel);
  }

XC::GenericSection1d &XC::GenericSection1d::operator=(const GenericSection1d &other)
  {
    PrismaticBarCrossSection::operator=(other);
    if(other.theModel)
      alloc(*other.theModel);
    else
      free();
    return *this;
  }


//! @brief Destructor.
XC::GenericSection1d::~GenericSection1d(void)
  { free(); }

//! @brief Sets the cross-section initial strain
//! (generalized: axial and bending).
int XC::GenericSection1d::setInitialSectionDeformation(const Vector &def)
  { return theModel->setInitialStrain(def(0)); }

//! @brief Sets the cross-section trial strain
//! (generalized: axial and bending).
//!
//! Sets the trial section deformation vector, \f$esec\f$, to be \p def,
//! then invokes setTrialStrain() on the UniaxialMaterial.
int XC::GenericSection1d::setTrialSectionDeformation(const Vector &def)
  { return theModel->setTrialStrain(def(0)); }

//! @brief Zeroes initial strain.
void XC::GenericSection1d::zeroInitialSectionDeformation(void)
  { theModel->setInitialStrain(0.0); }

//! @brief Returns the cross-section initial strain
//! (generalized: axial and bending).
const XC::Vector &XC::GenericSection1d::getInitialSectionDeformation(void) const
  {
    static Vector e(1); // static for class-wide returns
    e(0) = theModel->getInitialStrain();
    return e;
  }

//! @brief Returns material's trial generalized deformation.
const XC::Vector &XC::GenericSection1d::getSectionDeformation(void) const
  {
    static Vector e(1); // static for class-wide returns
    e(0) = theModel->getStrain();
    return e;
  }

//! @brief Return the integration of stresses over the section.
//!
//! Gets the section resisting force, \f$ssec\f$, to be the result of invoking 
//! getStress() on the UniaxialMaterial, then returns \f$ssec\f$.
const XC::Vector &XC::GenericSection1d::getStressResultant(void) const
  {
    s(0) = theModel->getStress();
    return s;
  }

//! @brief Return the tangent stiffness matrix.
//!
//! Sets the section tangent stiffness matrix, \f$ksec\f$, to be the result of
//! invoking getTangent() on the UniaxialMaterial, then returns \f$ksec\f$.
const XC::Matrix &XC::GenericSection1d::getSectionTangent(void) const
  {
    ks(0,0) = theModel->getTangent();
    return ks;
  }

//! @brief Returns the initial value of the tangent stiffness matrix.
const XC::Matrix &XC::GenericSection1d::getInitialTangent(void) const
  {
    ks(0,0) = theModel->getInitialTangent();
    return ks;
  }

//! @brief Return the flexibility matrix.
//!
//! Gets the section flexibility matrix, \f$fsec\f$, to be the inverse
//! of the result of invoking getTangent() on the UniaxialMaterial, then
//! returns \f$fsec\f$.
const XC::Matrix &XC::GenericSection1d::getSectionFlexibility(void) const
  {
    double tangent = theModel->getTangent();
    if(tangent != 0.0)
      ks(0,0) = 1.0/tangent;
    else
      ks(0,0) = 1.0e12;
    return ks;
  }

//! @brief Returns the initial value of the flexibility matrix.
const XC::Matrix &XC::GenericSection1d::getInitialFlexibility(void) const
  {
    double tangent = theModel->getInitialTangent();
    ks(0,0) = 1.0/tangent;
    return ks;
  }

//! @brief Invokes commitState() on the UniaxialMaterial and returns the
//! result of that invocation.
int XC::GenericSection1d::commitState(void)
  { return theModel->commitState(); }

//! @brief Invokes revertToLastCommit() on the UniaxialMaterial and returns the
//! result of that invocation.
int XC::GenericSection1d::revertToLastCommit ()
  { return theModel->revertToLastCommit(); }

//! @brief Invokes revertToStart() on the UniaxialMaterial and returns the
//! result of that invocation.
int XC::GenericSection1d::revertToStart ()
  { return theModel->revertToStart(); }

//! @brief Returns the response type.
//!
//! Returns the section ID code that indicates the type of response quantity 
//! returned by this instance of GenericSection1D.
const XC::ResponseId &XC::GenericSection1d::getResponseType(void) const
  {
    c(0)= code;
    return c;
  }

//! @brief Return order.
int XC::GenericSection1d::getOrder(void) const
  { return 1; }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::GenericSection1d::getCopy(void) const
  { return new GenericSection1d(*this); }

//! @brief Send object members through the communicator argument.
int XC::GenericSection1d::sendData(Communicator &comm)
  {
    int res= PrismaticBarCrossSection::sendData(comm);
    setDbTagDataPos(5,code);
    res+= comm.sendBrokedPtr(theModel,getDbTagData(),BrokedPtrCommMetaData(6,7,8));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::GenericSection1d::recvData(const Communicator &comm)
  {
    int res= PrismaticBarCrossSection::recvData(comm);
    code= getDbTagDataPos(5);
    theModel= comm.getBrokedMaterial(theModel,getDbTagData(),BrokedPtrCommMetaData(6,7,8));
    return res;
  }

//! @brief Sends object members through the communicator argument.
int XC::GenericSection1d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(9);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::GenericSection1d::recvSelf(const Communicator &comm)
  {
    inicComm(9);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

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

//! @brief Prints to the stream \p s the object's \p tag, then invokes
//! Print() on the UniaxialMaterial.
void XC::GenericSection1d::Print(std::ostream &s, int flag) const
  {
    s << "GenericSection1d (Uniaxial), tag: " << this->getTag() << std::endl
      << "\tResponse code: " << code << std::endl
      << "\tUniaxialMaterial: " << theModel->getTag() << std::endl;
  }
