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
                                                                        
// $Revision: 1.9 $
// $Date: 2003/03/04 00:48:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/SectionForceDeformation.cpp,v $
                                                                        
                                                                        
// File: ~/material/SectionForceDeformation.C
//
// Written: MHS 
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class implementation for SectionForceDeformation.
//
// What: "@(#) SectionForceDeformation.C, revA"

#include <deque>
#include <regex>
#include <material/section/SectionForceDeformation.h>
#include "material/ResponseId.h"
#include <domain/mesh/element/utils/Information.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include <utility/matrix/Vector.h>
#include <utility/recorder/response/MaterialResponse.h>
#include "utility/actor/actor/MovableMatrix.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include "utility/utils/misc_utils/colormod.h"


const XC::Matrix *ptr_section_tangent= nullptr;
const XC::Matrix *ptr_initial_tangent= nullptr;

//! @brief Constructor.
//!
//! @param tag: object identifier.
//! @param classTag: object class identifier.
//! @param mat_ldr: model wide material objects manager.
XC::SectionForceDeformation::SectionForceDeformation(int tag, int classTag,MaterialHandler *mat_ldr)
  : Material(tag,classTag), material_handler(mat_ldr) {}

//! @brief Return true if both objects are equal.
bool XC::SectionForceDeformation::isEqual(const SectionForceDeformation &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
	retval= Material::isEqual(other);
	if(retval)
	  {
	    retval= (this->fDefault==other.fDefault);
	  }
      }
    return retval;
  }

//! @brief Comma separated internal forces names to with the section contributes with stiffness.
std::string XC::SectionForceDeformation::getResponseTypeString(void) const
  { return getResponseType().getString(); }

//! @brief Adds to the generalized initial strains vector inicial the vector
//! being passed as parameter.
int XC::SectionForceDeformation::incrementInitialSectionDeformation(const Vector &def)
  { return setInitialSectionDeformation(getInitialSectionDeformation()+def); }



//! @brief Returns the tangent flexibility matrix of the section.
//!
//! Obtains the section tangent stiffness matrix, \f$ksec\f$, and returns its
//! inverse, the section flexibility matrix, \f$fsec\f$, via an
//! explicit matrix inversion.  NOTE: The explicit matrix
//! inversion provides default behavior and may be overridden in
//! subclasses to suit specific SectionForceDeformation implementations.
const XC::Matrix &XC::SectionForceDeformation::getSectionFlexibility(void) const
  {
    const int order= this->getOrder();
    fDefault.resize(order,order);

    const Matrix &k= this->getSectionTangent();
    switch(order)
      {
      case 1:
        if(k(0,0) != 0.0)
          fDefault(0,0)= 1.0/k(0,0);
        break;
      case 2:
        invert2by2Matrix(k,fDefault);
        break;
      case 3:
        invert3by3Matrix(k,fDefault);
        break;
      default:
        invertMatrix(order,k,fDefault);
        break;
      }
    return fDefault;
  }

//! @brief Returns the initial flexibility matrix of the section.
const XC::Matrix &XC::SectionForceDeformation::getInitialFlexibility(void) const
  {
    const int order= this->getOrder();
    fDefault.resize(order,order);
    const Matrix &k= this->getInitialTangent();
  
    switch(order)
      {
      case 1:
        if(k(0,0) != 0.0)
          fDefault(0,0)= 1.0/k(0,0);
        break;
      case 2:
        invert2by2Matrix(k,fDefault);
        break;
      case 3:
        invert3by3Matrix(k,fDefault);
        break;
      default:
        invertMatrix(order,k,fDefault);
        break;
      }
    return fDefault;
  }

//! @brief Returns the density (mass per unit length/area/volume)
//! of the section.
double XC::SectionForceDeformation::getRho(void) const
  { return 0.0; }

//! @brief Returns the mass per unit length of the section.
double XC::SectionForceDeformation::getLinearRho(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not defined yet"
	      << Color::def << std::endl;
    return 0.0;
  }

//! @brief Returns the mass per unit area of the section.
double XC::SectionForceDeformation::getArealRho(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not defined yet"
	      << Color::def << std::endl;
    return 0.0;
  }

//! @brief Returns the mass per unit volume of the section.
double XC::SectionForceDeformation::getVolumetricRho(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not defined yet"
	      << Color::def << std::endl;
    return 0.0;
  }

/*
int 
XC::SectionForceDeformation::setResponse(const std::vector<std::string> &argv, Information &sectInfo)
{
    // deformations
    if ((strcmp(argv[0],"deformations") ==0) || 
        (strcmp(argv[0],"deformation") ==0)) {

        XC::Vector *theVector= new Vector(this->getOrder());
        if (theVector == 0) {
            std::cerr << "WARNING XC::SectionForceDeformation::setResponse() - out of memory."
                      << Color::def << std::endl;
            return -1;
        } 
        sectInfo.theVector= theVector;
        sectInfo.theType= VectorType;        
        return 1;
    } 

    // stress resultants
    else if ((strcmp(argv[0],"forces") ==0) ||
             (strcmp(argv[0],"force") ==0)) {

        XC::Vector *theVector= new Vector(this->getOrder());
        if (theVector == 0) {
            std::cerr << "WARNING SectionForceDeformation::setResponse() - out of memory."
                      << Color::def << std::endl;
            return -1;
        } 
        sectInfo.theVector= theVector;
        sectInfo.theType= VectorType;        
        return 2;
    } 

        // tangent stiffness
        else if (argv[0] == "stiff" ||
                argv[0] == "stiffness") {
                const int order= this->getOrder();
                XC::Matrix *newMatrix= new Matrix(order,order);
                if (newMatrix == 0) {
                        std::cerr << "WARNING XC::SectionForceDeformation::setResponse() - out of memory."
                                  << Color::def << std::endl;
                        return -1;
                } 
                sectInfo.theMatrix= newMatrix;
                sectInfo.theType= MatrixType;        
                return 3;
        }

    // otherwise response quantity is unknown for the Section class
    else
        return -1;    
}
*/

//! @brief Returns the respuesta of the section
XC::Response *XC::SectionForceDeformation::setResponse(const std::vector<std::string> &argv, Information &sectInfo)
  {
    // deformations
    if(argv[0] == "deformations" || argv[0] == "deformation")
      return new MaterialResponse(this, 1, this->getSectionDeformation());
    // forces
    else if(argv[0] == "forces" || argv[0] == "force")
      return new MaterialResponse(this, 2, this->getStressResultant());
    // tangent
    else if(argv[0] == "stiff" || argv[0] == "stiffness")
      return new MaterialResponse(this, 3, this->getSectionTangent());
    // force and deformation
    else if (argv[0] == "forceAndDeformation")
      return new MaterialResponse(this, 4, XC::Vector(2*this->getOrder()));
    else
      return 0;
  }

//! @brief Returns section response.
int XC::SectionForceDeformation::getResponse(int responseID, Information &secInfo)
  {
    switch (responseID)
      {
      case 1:
        return secInfo.setVector(this->getSectionDeformation());
      case 2:
        return secInfo.setVector(this->getStressResultant());
      case 3:
        return secInfo.setMatrix(this->getSectionTangent());
      case 4:
        {
          Vector &theVec= *(secInfo.theVector);
          const Vector &e= this->getSectionDeformation();
          const Vector &s= this->getStressResultant();
          const int order= this->getOrder();
          for(int i= 0; i < order; i++)
            {
              theVec(i)= e(i);
              theVec(i+order)= s(i);
            }
          return secInfo.setVector(theVec);
        }
      default:
        return -1;
      }
  }

//! @brief Return values of internal forces, deformations...
//! @param cod: name of the requested value.
//! @param silent: if true don't complain about non-existen property.
XC::Matrix XC::SectionForceDeformation::getValues(const std::string &cod, bool silent) const
  {
    Matrix retval;
    const std::regex internal_forces_regexp("[nNPmMqQvV][12][12]*");
    const std::regex deformations_regexp("(def)([nNPmMqQvV][12][12]*)");
    if((cod == "forces") || (cod == "force") || (cod == "generalized_stress"))
      { retval= Matrix(getStressResultant()); }
    else if((cod == "deformations") || (cod == "deformation") || (cod == "generalized_strain"))
      { retval= Matrix(getStressResultant()); }
    else if((cod == "stress") || (cod == "stresses"))
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; stresses are not constant in the section."
	          << " Returning internal forces."
	          << Color::def << std::endl;
	retval= Matrix(this->getStressResultant());
      }
    else if(std::regex_match(cod, internal_forces_regexp)) // internal force.
      {
	retval.resize(1,1);
	retval(0,0)= getStressResultantByName(cod);
      }
    else if(std::regex_match(cod, deformations_regexp)) // deformations
      {
	retval.resize(1,1);
	retval(0,0)= getSectionDeformationByName(cod);
      }
    else
      retval= Material::getValues(cod, silent);
    return retval;
  }

//! @brief Return the section deformation vector, \f$esec\f$.
XC::Vector XC::SectionForceDeformation::getTrialSectionDeformation(void) const
  { return getSectionDeformation()+getInitialSectionDeformation(); }

//! @brief Returns 'defID' component of the generalized strain vector.
double XC::SectionForceDeformation::getSectionDeformation(const int &defID) const
  {
    double retval= 0.0;
    const int order= getOrder();
    const Vector &e= getSectionDeformation(); //Generalized strain vector.
    const ResponseId &code= this->getResponseType();
    for(int i= 0;i<order;i++)
      if(code(i) == defID)
        retval+= e(i);
    return retval;
  }

//! @brief Returns 'defID' component of the generalized stress vector.
double XC::SectionForceDeformation::getStressResultant(const int &defID) const
  {
    double retval= 0.0;
    const int order= getOrder();
    const Vector &f= getStressResultant(); //Vector de esfuerzos.
    const ResponseId &code= getResponseType();
    for(int i= 0;i<order;i++)
      if(code(i) == defID)
        retval+= f(i);
    return retval;
  }

//! Returns the esfuerzos de la placa.
double XC::SectionForceDeformation::getStressResultantByName(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1") //Axial force axil per unit length, parallel to the axis 1.
      retval= getStressResultant(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2") //Axial force per unit length, parallel to the axis 2.
      retval= getStressResultant(MEMBRANE_RESPONSE_n2);
    else if(cod == "n12")
      retval= getStressResultant(MEMBRANE_RESPONSE_n12);
    else if(cod == "m1") //Flector per unit length, around the axis 1.
      retval= getStressResultant(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Flector per unit length, around the axis 2.
      retval= getStressResultant(PLATE_RESPONSE_m2);
    else if(cod == "m12")
      retval= getStressResultant(PLATE_RESPONSE_m12);
    else if(cod == "q13")
      retval= getStressResultant(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= getStressResultant(PLATE_RESPONSE_q23);
    else if(cod == "P" || cod == "N") //Esfuerzo axil (barra prinsmatica).
      retval= getStressResultant(SECTION_RESPONSE_P);
    else if(cod == "Mz") //Bending around the z axis.
      retval= getStressResultant(SECTION_RESPONSE_MZ);
    else if(cod == "My") //Bending around the y axis.
      retval= getStressResultant(SECTION_RESPONSE_MY);
    else if(cod == "Vz")
      retval= getStressResultant(SECTION_RESPONSE_VZ);
    else if(cod == "Vy")
      retval= getStressResultant(SECTION_RESPONSE_VY);
    else if(cod == "T" || cod == "Mx")
      retval= getStressResultant(SECTION_RESPONSE_T);

    return retval;
  }

//! @brief Returns section's trial deformation component by its name.
double XC::SectionForceDeformation::getSectionDeformationByName(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "defn1")
      retval= getSectionDeformation(MEMBRANE_RESPONSE_n1);
    else if(cod == "defn2")
      retval= getSectionDeformation(MEMBRANE_RESPONSE_n2);
    else if(cod == "defm1") // Bending around the 1 axis.
      retval= getSectionDeformation(PLATE_RESPONSE_m1);
    else if(cod == "defm2") // Bending around the 2 axis.
      retval= getSectionDeformation(PLATE_RESPONSE_m2);
    else if(cod == "defq13")
      retval= getSectionDeformation(PLATE_RESPONSE_q13);
    else if(cod == "defq23")
      retval= getSectionDeformation(PLATE_RESPONSE_q23);
    else if(cod == "defm12")
      retval= getSectionDeformation(PLATE_RESPONSE_m12);
    else if(cod == "defn12")
      retval= getSectionDeformation(MEMBRANE_RESPONSE_n12);
    else if(cod == "defP" || cod == "defN") // Axial force (prismatic bar).
      retval= getSectionDeformation(SECTION_RESPONSE_P);
    else if(cod == "defMz") // Rotation around the axis z.
      retval= getSectionDeformation(SECTION_RESPONSE_MZ);
    else if(cod == "defMy") // Rotation around the axis y.
      retval= getSectionDeformation(SECTION_RESPONSE_MY);
    else if(cod == "defVz")
      retval= getSectionDeformation(SECTION_RESPONSE_VZ);
    else if(cod == "defVy")
      retval= getSectionDeformation(SECTION_RESPONSE_VY);
    else if(cod == "defT")
      retval= getSectionDeformation(SECTION_RESPONSE_T);
    else
      std::cerr << "Deformation code: '" << cod 
                << "' unknown."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::SectionForceDeformation::sendData(Communicator &comm)
  {
    setDbTagDataPos(1,getTag());
    int res= comm.sendMatrix(fDefault,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::SectionForceDeformation::recvData(const Communicator &comm)
  {
    setTag(getDbTagDataPos(1));
    int res= comm.receiveMatrix(fDefault,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::SectionForceDeformation::getPyDict(void) const
  {
    boost::python::dict retval= Material::getPyDict();
    retval["fDefault"]= this->fDefault.getPyList();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::SectionForceDeformation::setPyDict(const boost::python::dict &d)
  {
    Material::setPyDict(d);
    const boost::python::list &fDefault_lst= boost::python::extract<boost::python::list>(d["fDefault"]);
    this->fDefault= Matrix(fDefault_lst);
  }

// AddingSensitivity:BEGIN ////////////////////////////////////////
int XC::SectionForceDeformation::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }

int XC::SectionForceDeformation::updateParameter(int responseID, Information &eleInformation)
  { return -1; }

int XC::SectionForceDeformation::activateParameter(int parameterID)
  { return -1; }

const XC::Vector &XC::SectionForceDeformation::getStressResultantSensitivity(int gradNumber, bool conditional)
  {
    static Vector dummy(1);
    return dummy;
  }

const XC::Vector &XC::SectionForceDeformation::getSectionDeformationSensitivity(int gradNumber)
  {
    static Vector dummy(1);
    return dummy;
  }

const XC::Matrix &XC::SectionForceDeformation::getSectionTangentSensitivity(int gradNumber)
  {
    static XC::Matrix dummy(1,1);
    return dummy;
  }

const XC::Matrix &XC::SectionForceDeformation::getSectionFlexibilitySensitivity(int gradIndex)
  {
    const int order= this->getOrder();
    fDefault.resize(order,order);
 
    const Matrix &dksdh= this->getSectionTangentSensitivity(gradIndex);
    const Matrix &fs= this->getSectionFlexibility();
    fDefault= (fs * dksdh * fs) * -1;

    return fDefault;
  }

const XC::Matrix &XC::SectionForceDeformation::getInitialTangentSensitivity(int gradIndex)
  {
    const int order= this->getOrder();
    fDefault.resize(order,order);
    fDefault.Zero();
    return fDefault;
  }

const XC::Matrix &XC::SectionForceDeformation::getInitialFlexibilitySensitivity(int gradIndex)
  {
    const int order= this->getOrder();
    fDefault.resize(order,order);
    const Matrix &dksdh= this->getInitialTangentSensitivity(gradIndex);
    const Matrix &fs= this->getInitialFlexibility();
    fDefault= (fs * dksdh * fs) * -1;
    return fDefault;
  }

double XC::SectionForceDeformation::getRhoSensitivity(int gradNumber)
  { return 0.0; }

//! @brief Return the energy stored in the material.
double XC::SectionForceDeformation::getEnergy(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << Color::def << std::endl;
    return 0;
  }  

int XC::SectionForceDeformation::commitSensitivity(const XC::Vector& defSens, int gradNumber, int numGrads)
  { return -1; }
// AddingSensitivity:END ///////////////////////////////////////////

