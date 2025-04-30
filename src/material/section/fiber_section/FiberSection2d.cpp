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
*                                                                    **
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

// $Revision: 1.23 $
// $Date: 2005/03/31 19:04:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSection2d.cpp,v $

// Written: fmk
// Created: 04/04
//
// Description: This file contains the class implementation of XC::FiberSection2d.

#include "FiberSection2d.h"
#include <cstdlib>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>
#include "material/section/fiber_section/fiber/UniaxialFiber2d.h"
#include <classTags.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/repres/section/FiberSectionRepr.h>

#include "material/ResponseId.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"

//! @brief Constructor (it's used in FiberSectionShear2d).
XC::FiberSection2d::FiberSection2d(int tag,int classTag,MaterialHandler *mat_ldr)
  : FiberSectionBase(tag, classTag, 2, mat_ldr), parameterID(0) {}

// constructors:
XC::FiberSection2d::FiberSection2d(int tag,const fiber_list &fiberList,MaterialHandler *mat_ldr)
  : FiberSectionBase(tag, SEC_TAG_FiberSection2d,2,mat_ldr), parameterID(0)
  {
    fibers.setup(*this,fiberList,kr);
  }

//! @brief Constructor.
XC::FiberSection2d::FiberSection2d(int tag, MaterialHandler *mat_ldr)
  : FiberSectionBase(tag, SEC_TAG_FiberSection2d,2,mat_ldr), parameterID(0)
  {}

//! @brief Adds a fiber to the section.
XC::Fiber *XC::FiberSection2d::addFiber(Fiber &newFiber)
  { return fibers.addFiber(*this,newFiber,kr); }

//! @brief Adds a fiber to the section.
XC::Fiber *XC::FiberSection2d::addFiber(int tag,const MaterialHandler &ldr,const std::string &nmbMat,const double &Area, const Vector &position)
  {
    Fiber *retval= fibers.findFiber(tag);
    if(retval)
      std::cerr <<  getClassName() << "::" << __FUNCTION__
		<< "; fiber with tag: " << tag
		<< " already exists." << std::endl;
    else
      {
        if(position.Size()<1)
          std::cerr <<  getClassName() << "::" << __FUNCTION__
		    << "; position vector must be of dimension 2."
		    << std::endl;
        UniaxialFiber2d f(tag,ldr,nmbMat,Area,position(0));
        retval= addFiber(f);
      }
    return retval;
  }

void XC::FiberSection2d::setupFibers(void)
  {
    if(section_repres)
      fibers.setup(*this,section_repres->get2DFibers(),kr);
    else
      fibers.updateKRCenterOfMass(*this,kr);
  }

double XC::FiberSection2d::get_strain(const double &y) const
  {
    const Vector &def= getSectionDeformation();
    return (def(0) + y*def(1));
  }

//! @brief Returns the strains in the position being passed as parameter.
double XC::FiberSection2d::getStrain(const double &y,const double &) const
  { return get_strain(-y); } //XXX We must change sign of the y coordinate
                               //because of the mess due to the change of
                               //the coordinate sign in the private member
                               //of the object. See, for example,
                               //UniaxialFiber2d constructor

//! @brief Sets values for initial strains.
int XC::FiberSection2d::setInitialSectionDeformation(const Vector &deforms)
  {
    FiberSectionBase::setInitialSectionDeformation(deforms);
    return fibers.setInitialSectionDeformation(*this);
  }

//! @brief Sets values for trial strains.
int XC::FiberSection2d::setTrialSectionDeformation(const Vector &deforms)
  {
    FiberSectionBase::setTrialSectionDeformation(deforms);
    return fibers.setTrialSectionDeformation(*this,kr);
  }

//! @brief Return the initial tangent stiffness matrix.
const XC::Matrix &XC::FiberSection2d::getInitialTangent(void) const
  { return fibers.getInitialTangent(*this); }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::FiberSection2d::getCopy(void) const
  { return new FiberSection2d(*this); }

const XC::ResponseId &XC::FiberSection2d::getResponseType(void) const
  { return RespElasticSection2d; }

int XC::FiberSection2d::getOrder(void) const
  { return getResponseType().Size(); }

//! @brief Returns the bending stiffness of the cross-section around the z axis.
const double &XC::FiberSection2d::EIz(void) const
  {
    const Matrix &tang= this->getSectionTangent();
    return tang(1,1);
  }

//! @brief Returns the section to its last committed state.
int XC::FiberSection2d::revertToLastCommit(void)
  {
    // Last committed section deformations
    FiberSectionBase::revertToLastCommit();
    return fibers.revertToLastCommit(*this,kr);
  }

//! @brief Returns the section to its initial state.
int XC::FiberSection2d::revertToStart(void)
  {
    FiberSectionBase::revertToStart();
    return fibers.revertToStart(*this,kr);
  }

//! @brief Send data through the communicator argument.
int XC::FiberSection2d::sendData(Communicator &comm)
  {
    int res= FiberSectionBase::sendData(comm);
    res+= comm.sendInt(parameterID,getDbTagData(),CommMetaData(12));
    return res;
  }


//! @brief Receive data through the communicator argument.
int XC::FiberSection2d::recvData(const Communicator &comm)
  {    
    int res= FiberSectionBase::recvData(comm);
    res+= comm.receiveInt(parameterID,getDbTagData(),CommMetaData(12));
    return res;
  }

int XC::FiberSection2d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(13);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data."
	        << std::endl;
    return res;
  }

int XC::FiberSection2d::recvSelf(const Communicator &comm)
  {
    inicComm(13);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids."
	        << std::endl;
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive data."
	        << std::endl;
      }
    return res;
  }

void XC::FiberSection2d::Print(std::ostream &s, int flag) const
  {
    s << "\nFiberSection2d, tag: " << this->getTag() << std::endl;
    s << "\tSection code: " << getResponseType();
    if(flag == 1)
      fibers.Print(s,flag);
  }

int XC::FiberSection2d::getResponse(int responseID, Information &sectInfo)
  {
    // Just call the base class method ... don't need to define
    // this function, but keeping it here just for clarity
    return FiberSectionBase::getResponse(responseID, sectInfo);
  }



// AddingSensitivity:BEGIN ////////////////////////////////////
int XC::FiberSection2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= 0;
    const int argc= argv.size();
    if(argc>0)
      {
	// Check if the parameter belongs to a fiber
	// unlike setResponse, only allowing 'fiber y z matTag ...' because
	// the setResponse logic breaks down with the trailing arguments
	if(argv[0]=="fiber")
	  {
	    if(argc < 5)
	      retval= 0;
	    else
	      {
	        const int matTag = atoi(argv[3]);
	        const double yCoord = atof(argv[1]);
	        Fiber *fiber= fibers.getClosestFiber(matTag,yCoord);
		if(fiber)
		  {
	            std::vector<std::string> argv2(argv);
	            argv2.erase(argv2.begin(),argv2.begin()+4);
		    fiber->getMaterial()->setParameter(argv2, param);
		  }
	      }
 	    retval= fibers.setParameter(argv,param);
	  }
	else
	  retval= FiberSectionBase::setParameter(argv,param);
	if(retval<0)
	  {
	    std::cerr <<  getClassName() << "::" << __FUNCTION__
		      << "; could not set parameter. " << std::endl;
	    retval= -1;
	  }
      }
    return retval;
  }

int XC::FiberSection2d::updateParameter (int parameterID, Information &info)
  {
    // Check if it is a material parameter (only option for now)
    if(parameterID > 1000)
      {
        // Get section number and material number
        int sectionTag= this->getTag();
        parameterID= parameterID - sectionTag*100000;
        int materialTag= (int)( floor((double)parameterID) / (1000) );
        parameterID= parameterID - materialTag*1000;

        int ok= fibers.updateParameter(materialTag,parameterID,info);
        if(ok < 0)
          {
            std::cerr <<  getClassName() << "::" << __FUNCTION__
		      << "; could not update parameter. " << std::endl;
            return ok;
          }
        else
          { return ok; }
      }
    else
      {
        std::cerr <<  getClassName() << "::" << __FUNCTION__
		  << "; could not update parameter. " << std::endl;
        return -1;
      }
  }

int XC::FiberSection2d::activateParameter(int passedParameterID)
  {
    // Note that the parameteID that is stored here at the
    // section level contains all information about section
    // and material tag number:
    parameterID= passedParameterID;
    return fibers.activateParameter(passedParameterID);
  }

const XC::Vector &XC::FiberSection2d::getSectionDeformationSensitivity(int gradNumber)
  {
    static XC::Vector dummy(2);
    return dummy;
  }

const XC::Vector &XC::FiberSection2d::getStressResultantSensitivity(int gradNumber, bool conditional)
  { return fibers.getStressResultantSensitivity(gradNumber,conditional); }

const XC::Matrix &XC::FiberSection2d::getSectionTangentSensitivity(int gradNumber)
  {
    static XC::Matrix something(2,2);
    something.Zero();
    return something;
  }

int XC::FiberSection2d::commitSensitivity(const XC::Vector& defSens, int gradNumber, int numGrads)
  {
    fibers.commitSensitivity(defSens,gradNumber,numGrads);
    return 0;
  }

// AddingSensitivity:END ///////////////////////////////////

XC::FiberSection2d XC::FiberSectionReprToFiberSection2d(const int &tag,const XC::FiberSectionRepr &fiberSectionRepr)
  { return fiberSectionRepr.getFiberSection2d(tag); }
