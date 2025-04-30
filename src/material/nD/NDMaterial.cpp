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
**                                                                    **
** Additions and changes by:                                          **
**   Boris Jeremic (@ucdavis.edu)                                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.17 $                                                              
// $Date: 2004/07/20 22:39:02 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/NDMaterial.cpp,v $                                                                
                                                                        
// File: ~/material/XC::NDMaterial.C
//
// Written: MHS 
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class implementation for NDMaterial.
//
// What: "@(#) XC::NDMaterial.C, revA"

#include "material/nD/NDMaterial.h"
#include "domain/mesh/element/utils/Information.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/nDarray/stresst.h"
#include "utility/matrix/nDarray/straint.h"
#include "utility/matrix/nDarray/Tensor.h"
#include "utility/recorder/response/MaterialResponse.h"
#include "material/ResponseId.h"
#include "utility/utils/misc_utils/colormod.h"

XC::Matrix XC::NDMaterial::errMatrix(1,1);
XC::Vector XC::NDMaterial::errVector(1);

//! @brief Constructor.
//!
//! To construct a NDMaterial whose unique integer among
//! NDMaterials in the domain is given by \p tag, and whose class
//! identifier is given by \p classTag. These integers are passed to
//! the Material class constructor.
//!
//! @param tag: material identifier.
//! @param classTag: identifier of the material class.
XC::NDMaterial::NDMaterial(int tag, int classTag)
  : Material(tag,classTag) {}

//! @brief Constructor.
XC::NDMaterial::NDMaterial(void)
  : Material(0, 0) {}


//! @brief Get material density.
double XC::NDMaterial::getRho(void) const
  { return 0.0; }

//! @brief Set material density.
void XC::NDMaterial::setRho(const double &)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << " not implemented yet."
	      << Color::def << std::endl;
  }

// BJ added 19June2002
//! @brief Return the material elastic modulus.
double XC::NDMaterial::getE(void) const 
  { return 0.0; }

// BJ added 19June2002
//! @brief Return the material Poisson's ratio.
double XC::NDMaterial::getnu(void) const
  { return 0.0; }

// BJ added 19June2002
//! @brief Return the material psi??.
double XC::NDMaterial::getpsi(void) const
  { return 0.0; }

//! @brief Returns committed stresses
const XC::Vector &XC::NDMaterial::getCommittedStress(void) 
  { return this->getStress(); }

//! @brief Returns committed strains
const XC::Vector &XC::NDMaterial::getCommittedStrain(void) 
  { return this->getStrain(); }

// methods to set and retrieve state.

//! @brief Sets the value of the trial strain vector, that value used by {\em
//! getStress()} and getTangent(), to be \p strain. To return \f$0\f$
//! if successful and a negative number if not.
int XC::NDMaterial::setTrialStrain(const Vector &v)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return -1;    
  }

//! @brief Sets trial strain value.
int XC::NDMaterial::setTrialStrain(const Vector &v, const Vector &r)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return -1;    
  }

//! @brief Set trial strain increment.
int XC::NDMaterial::setTrialStrainIncr(const Vector &v)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return -1;    
  }

//! @brief Set trial strain increment.
int XC::NDMaterial::setTrialStrainIncr(const Vector &v, const Vector &r)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return -1;    
  }

//! @brief Sets the initial strain value.
void XC::NDMaterial::setInitialGeneralizedStrain(const Vector &)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
  }

//! @brief Return the tangent stiffness matrix at the current trial
//! strain.
const XC::Matrix &XC::NDMaterial::getTangent(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return errMatrix;
  }

//! @brief Returns the material stress vector at the current trial strain.
const XC::Vector &XC::NDMaterial::getStress(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return errVector;    
  }

//! @brief Returns strain.
const XC::Vector &XC::NDMaterial::getStrain(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return errVector;    
  }

//! @brief return the Von Mises equivalent stress.
//!
//! <a href="https://en.wikipedia.org/wiki/Von_Mises_yield_criterion"> Von Mises yield criterion.</a>
double XC::NDMaterial::getVonMisesStress(void) const
  {
    double retval= 0.0;
    const Vector sg= getStress();
    const size_t sz= sg.Size();
    //NDmaterial stress order = 11, 22, 33, 12, 23, 31 
    if(sz==6) // 3D material
      {
	const double &sg11= sg[0]; const double &sg22= sg[1]; const double &sg33= sg[2];
	const double &sg12= sg[3]; const double &sg23= sg[4]; const double &sg31= sg[5];
	retval= sqrt(0.5*(pow(sg11-sg22,2)+pow(sg22-sg33,2)+pow(sg33-sg11,2)+6*(sg12*sg12+sg23+sg23+sg31+sg31)));
      }
    else if(sz==3) // 2D material.
      {
	const double &sg11= sg[0]; const double &sg22= sg[1]; 
	const double &sg12= sg[2]; 
	retval= sqrt(sg11*sg11+sg11*sg22+sg22*sg22+3.0*sg12*sg12);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ", wrong stress vector size (" << sz
	        << ")." << std::endl;
    return retval;
  }

//! @brief Return the initial strain.
const XC::Vector &XC::NDMaterial::getInitialGeneralizedStrain(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return errVector;    
  }

//! @brief Set initial strain value.
//! @param strain: strain value.
int XC::NDMaterial::setInitialStrain(const Vector &v)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return -1;    
  }

//! @brief Increments initial strain.
//! @param strainIncrement: value of the strain increment.
int XC::NDMaterial::incrementInitialStrain(const Vector &)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return -1;
  }

//! @brief Zeroes the initial strain.
void XC::NDMaterial::zeroInitialStrain(void)
  {}

//! @brief Return the value of the initial strain.
const XC::Vector &XC::NDMaterial::getInitialStrain(void) const
  {
    static Vector retval;
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return retval;
  }

//! @brief Set trial strain value.
int XC::NDMaterial::setTrialStrain(const Tensor &v)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return -1;    
  }

//! @brief Set trial strain value.
int XC::NDMaterial::setTrialStrain(const Tensor &v, const Tensor &r)    
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return -1;    
  }

//! @brief Set trial strain increment.
int XC::NDMaterial::setTrialStrainIncr(const Tensor &v)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return -1;    
  }

//! @brief Set trial strain increment.
int XC::NDMaterial::setTrialStrainIncr(const Tensor &v, const Tensor &r)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility."
              << Color::def << std::endl;
    return -1;    
  }
const XC::straintensor &XC::NDMaterial::getPlasticStrainTensor(void) const
  {
     std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; subclass responsibility.\n";
     static straintensor errstraintensor;
     return errstraintensor;
  }

const XC::stresstensor &XC::NDMaterial::getStressTensor(void) const
  {
    static stresstensor st;
    st= stresstensor(getStress());
    return st;
  }
  
const XC::straintensor &XC::NDMaterial::getStrainTensor(void) const
  {
    static straintensor st;
    st= straintensor(getStrain());
    return st;
  }

const XC::Tensor &XC::NDMaterial::getTangentTensor(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented."
              << Color::def << std::endl;
    static Tensor errTensor;    
    return errTensor;    
  }

XC::Response* XC::NDMaterial::setResponse(const std::vector<std::string> &argv, Information &matInfo)
  {
    if(argv[0] == "stress" || argv[0] == "stresses")
      return new MaterialResponse(this, 1, this->getStress());
    else if(argv[0] == "strain" || argv[0] == "strains")
      return new MaterialResponse(this, 2, this->getStrain());
    else if((argv[0] == "tangent") || (argv[0] == "tangent"))
      return new MaterialResponse(this, 4, this->getTangent());
    else if((argv[0] == "damage") || (argv[0] == "Damage"))
      {
	static Vector vec(3);
	vec.Zero();
        return new MaterialResponse(this, 5, vec); // zero vector
      }

      return 0;
  }

int XC::NDMaterial::getResponse(int responseID, Information &matInfo)
  {
    switch(responseID)
      {
      case 1:
        return matInfo.setVector(this->getStress());
      case 2:
        return matInfo.setVector(this->getStrain());
      case 3:
        return matInfo.setMatrix(this->getTangent());
      default:
        return -1;
      }
  }

//! @brief Returns material response.
//! @param cod: name of the requested value.
//! @param silent: if true don't complain about non-existen property.
XC::Matrix XC::NDMaterial::getValues(const std::string &cod, bool silent) const
  {
    Matrix retval;
    if((cod == "tangent") || (cod == "Tangent"))
      retval= this->getTangent();
    else if((cod == "von_mises_stress") || (cod == "Von_Mises_stress"))
      {
	retval.resize(1,1);
	retval(0,0)= getVonMisesStress();
      }
    else
      retval= Material::getValues(cod, silent);
    return retval;
  }

//! @brief Revert the material to its initial state.
int XC::NDMaterial::revertToStart(void)
  {
    zeroInitialStrain();
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::NDMaterial::sendData(Communicator &comm)
  {
    setDbTagDataPos(0,getTag());
    return 0;
  }

//! @brief Receives object members through the communicator argument.
int XC::NDMaterial::recvData(const Communicator &comm)
  {
    setTag(getDbTagDataPos(0));
    return 0;
  }

// AddingSensitivity:BEGIN ////////////////////////////////////////
int XC::NDMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }

int XC::NDMaterial::updateParameter(int parameterID, Information &info)
  { return -1; }

int XC::NDMaterial::activateParameter(int parameterID)
  { return -1; }

const XC::Vector &XC::NDMaterial::getStressSensitivity(int gradNumber, bool conditional)
  {
    static Vector dummy(1);
    return dummy;
  }

const XC::Vector &XC::NDMaterial::getStrainSensitivity(int gradNumber)
  {
    static Vector dummy(1);
    return dummy;
  }

double XC::NDMaterial::getRhoSensitivity(int gradNumber)
  { return 0.0; }

const XC::Matrix &XC::NDMaterial::getDampTangentSensitivity(int gradNumber)
  {
    static Matrix dummy(1,1);
    return dummy;
  }

const XC::Matrix &XC::NDMaterial::getTangentSensitivity(int gradNumber)
  {
    static Matrix dummy(1,1);
    return dummy;
  }

int XC::NDMaterial::commitSensitivity(XC::Vector & strainSensitivity, int gradNumber, int numGrads)
  { return 0; }
// AddingSensitivity:END //////////////////////////////////////////

//! @brief Receive a pointer to material through the communicator argument.
XC::NDMaterial *XC::receiveNDMaterialPtr(NDMaterial *ptr,DbTagData &dt,const Communicator &comm,const BrokedPtrCommMetaData &md)
  {
    NDMaterial *retval= nullptr;
    Material *tmp= nullptr;
    tmp= receiveMaterialPtr(ptr,dt,comm,md);
    if(tmp)
      {
        retval= dynamic_cast<NDMaterial *>(tmp);
        if(!retval)
          {
            std::cerr << Color::red << __FUNCTION__
		      <<"; WARNING - failed to get material."
		      << Color::def << std::endl;
            delete tmp;
          }
      }
    return retval;
  }
