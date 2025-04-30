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

// $Revision: 1.15 $
// $Date: 2005/10/07 18:09:58 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/truss/TrussSection.cpp,v $


// File: ~/element/truss/TrussSection.C
//
// Written: fmk
// Created: 07/98
// Revision: A
//
// Description: This file contains the implementation for the XC::TrussSection class.
//
// What: "@(#) TrussSection.C, revA"

#include <domain/mesh/element/truss_beam_column/truss/TrussSection.h>
#include <domain/mesh/element/utils/Information.h>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <material/section/SectionForceDeformation.h>
#include <utility/recorder/response/ElementResponse.h>
#include "material/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"
#include "domain/load/beam_loads/TrussStrainLoad.h"

#include <cmath>

//! @brief Constructor.
XC::TrussSection::TrussSection(int tag, int dim, int Nd1, int Nd2, SectionForceDeformation &theSect)
  :TrussBase(ELE_TAG_TrussSection,tag,dim,Nd1,Nd2),
   physicalProperties(1,&theSect),
   persistentInitialDeformation(0.0)
  {
    // get a copy of the material and check we obtained a valid copy
    SectionForceDeformation *theSection= physicalProperties[0];
    int order = theSection->getOrder();
    const ID &code = theSection->getResponseType();

    int i;
    for(i = 0; i < order; i++)
      if(code(i) == SECTION_RESPONSE_P)
        break;

     if(i == order)
       std::cerr << getClassName() << "::" << __FUNCTION__
		 << ";section does not provide axial response\n";

    // set node pointers to nullptr
    initialize();
  }

//! @brief Constructor.
XC::TrussSection::TrussSection(int tag,int dim,const Material *ptr_mat)
  :TrussBase(ELE_TAG_TrussSection,tag,dim,0,0), physicalProperties(1,ptr_mat),
   persistentInitialDeformation(0.0)
  {
    initialize();
  }

//! @brief Default constructor.
//! 
//! invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//! to be invoked upon
XC::TrussSection::TrussSection(void)
  :TrussBase(ELE_TAG_TrussSection),  physicalProperties(1),
   persistentInitialDeformation(0.0)
  {
    initialize();
  }

//! @brief Virtual constructor.
XC::Element* XC::TrussSection::getCopy(void) const
  { return new TrussSection(*this); }

//! @brief Returns the value of the persistent (does not get wiped out by
//! zeroLoad) initial deformation of the section.
const double &XC::TrussSection::getPersistentInitialSectionDeformation(void) const
  { return persistentInitialDeformation; }

//! @brief Increments the persistent (does not get wiped out by zeroLoad)
//! initial deformation of the section. It's used to store the deformation
//! of the material during the periods in which the element is deactivated
//! (see for alive() method).
void XC::TrussSection::incrementPersistentInitialDeformationWithCurrentDeformation(void)
  { persistentInitialDeformation+= this->computeCurrentStrain(); }

// method: setDomain()
//    to set a link to the enclosing XC::Domain and to set the node pointers.
//    also determines the number of dof associated
//    with the truss element, we set matrix and vector pointers,
//    allocate space for t matrix, determine the length
//    and set the transformation matrix.
void XC::TrussSection::setDomain(Domain *theDomain)
  {
    TrussBase::setDomain(theDomain);
    if(!theDomain)
      {
        L = 0;
        return;
      }

    // first set the node pointers
    int Nd1 = theNodes.getTagNode(0);
    int Nd2 = theNodes.getTagNode(1);
    theNodes.set_node(0,theDomain->getNode(Nd1));
    theNodes.set_node(1,theDomain->getNode(Nd2));

    // if nodes not in domain, warning message & set default numDOF as 2
    if((theNodes[0] == 0) || (theNodes[1] == 0))
      {
        if(theNodes[0] == 0)
          std::cerr << "TrussSection::setDomain() - Nd1: " << Nd1 << " does not exist in Domain\n";
        else
          std::cerr << "TrussSection::setDomain() - Nd1: " << Nd2 << " does not exist in Domain\n";

      std::cerr << " for truss with id " << this->getTag() << std::endl;

      // fill this in so don't segment fault later
      numDOF = 2;
      theMatrix = &trussM2;
      theVector = &trussV2;

      return;
    }

    // now determine the number of dof and the dimesnion
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();

    if(dofNd1 != dofNd2) {
      std::cerr << "WARNING XC::TrussSection::setDomain(): nodes " << Nd1 << " and " <<
        Nd2 << "have differing dof at ends for truss " << this->getTag() << std::endl;

      // fill this in so don't segment fault later
      numDOF = 2;
      theMatrix = &trussM2;
      theVector = &trussV2;

      return;
    }

    setup_matrix_vector_ptrs(dofNd1);

    // now determine the length, cosines and fill in the transformation
    setup_L_cos_dir();

    // create the load vector
    alloc_load(numDOF);

    update();
  }


int XC::TrussSection::commitState(void)
  {
    int retVal = 0;
    // call element commitState to do any base class stuff
    if((retVal = this->XC::TrussBase::commitState()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in base class";
      }
    retVal+= physicalProperties.commitState();
    return retVal;
  }

//! @brief Reverts the element to its last committed state.
int XC::TrussSection::revertToLastCommit()
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    // int retval= TrussBase::revertToLastCommit();
    int retval= physicalProperties.revertToLastCommit();
    return retval;
  }

//! @brief Reverts the element to its initial state.
int XC::TrussSection::revertToStart()
  {
    int retval= TrussBase::revertToStart();
    retval+= physicalProperties.revertToStart();
    return retval;
  }

int XC::TrussSection::update(void)
  {
    if(L == 0.0)
      { // - problem in setDomain() no further warnings
        return -1;
      }

    // determine the current strain given trial displacements at nodes
    const double strain= this->computeCurrentStrain();

    SectionForceDeformation *theSection= physicalProperties[0];
    int order = theSection->getOrder();
    const ID &code = theSection->getResponseType();

    Vector e (order);

    int i;
    for(i = 0; i < order; i++) {
      if(code(i) == SECTION_RESPONSE_P)
        e(i) = strain;
    }

    return theSection->setTrialSectionDeformation(e);
  }


const XC::Matrix &XC::TrussSection::getTangentStiff(void) const
  {
    if(L == 0.0) { // - problem in setDomain() no further warnings
        theMatrix->Zero();
        return *theMatrix;
    }

    const SectionForceDeformation *theSection= physicalProperties[0];
    int order = theSection->getOrder();
    const ID &code = theSection->getResponseType();

    const Matrix &k = theSection->getSectionTangent();
    double AE = 0.0;
    int i;
    for(i = 0; i < order; i++)
      {
        if(code(i) == SECTION_RESPONSE_P)
          AE += k(i,i);
      }

    // come back later and redo this if too slow
    Matrix &stiff = *theMatrix;

    int numDOF2 = numDOF/2;
    double temp;
    AE /= L;
    for(i = 0; i < getNumDIM(); i++) {
      for(int j = 0; j < getNumDIM(); j++) {
        temp = cosX[i]*cosX[j]*AE;
        stiff(i,j) = temp;
        stiff(i+numDOF2,j) = -temp;
        stiff(i,j+numDOF2) = -temp;
        stiff(i+numDOF2,j+numDOF2) = temp;
      }
    }

    if(isDead())
      (*theMatrix)*=dead_srf;
    return *theMatrix;
  }

const XC::Matrix &XC::TrussSection::getInitialStiff(void) const
  {
    if(L == 0.0)
      { // - problem in setDomain() no further warnings
        theMatrix->Zero();
        return *theMatrix;
      }

    const SectionForceDeformation *theSection= physicalProperties[0];
    int order = theSection->getOrder();
    const ID &code = theSection->getResponseType();

    const Matrix &k = theSection->getInitialTangent();
    double AE = 0.0;
    int i;
    for(i = 0; i < order; i++) {
      if(code(i) == SECTION_RESPONSE_P)
        AE += k(i,i);
    }

    // come back later and redo this if too slow
    Matrix &stiff = *theMatrix;

    int numDOF2 = numDOF/2;
    double temp;
    AE /= L;
    for(i = 0; i < getNumDIM(); i++) {
      for(int j = 0; j < getNumDIM(); j++) {
        temp = cosX[i]*cosX[j]*AE;
        stiff(i,j) = temp;
        stiff(i+numDOF2,j) = -temp;
        stiff(i,j+numDOF2) = -temp;
        stiff(i+numDOF2,j+numDOF2) = temp;
      }
    }

    if(isDead())
      (*theMatrix)*=dead_srf;
    return *theMatrix;
  }

//! @brief Return the element material.
const XC::Material *XC::TrussSection::getMaterial(void) const
  { return physicalProperties[0]; }

//! @brief Return the element material.
XC::Material *XC::TrussSection::getMaterial(void)
  { return physicalProperties[0]; }

//! @brief Return the density of the section.
double XC::TrussSection::getRho(void) const
  { return physicalProperties[0]->getLinearRho(); }

//! @brief Returns the element mass per unit length.
double XC::TrussSection::getLinearRho(void) const
  { return getRho(); }

//! @brief Return the element mass matrix.
const XC::Matrix &XC::TrussSection::getMass(void) const
  {
    // zero the matrix
    Matrix &mass = *theMatrix;
    mass.Zero();

    const double rho= getLinearRho(); // mass per unit length.
    // check for quick return
    if(L == 0.0 || rho == 0.0)
      { // - problem in setDomain() no further warnings
        return mass;
      }

    const double M= 0.5*rho*L;

    int numDOF2 = numDOF/2;
    for(int i = 0; i < getNumDIM(); i++) {
      mass(i,i) = M;
      mass(i+numDOF2,i+numDOF2) = M;
    }

    if(isDead())
      mass*=dead_srf;
    return mass;
  }

//! @brief Reactivates the element.
void XC::TrussSection::alive(void)
  {
    if(isDead())
      {
	// Store the current deformation.
        this->incrementPersistentInitialDeformationWithCurrentDeformation();
	TrussBase::alive(); // Not dead anymore.
      }
  }

//! @brief Make loads zero.
void XC::TrussSection::zeroLoad(void)
  {
    TrussBase::zeroLoad();
    (*physicalProperties[0]).zeroInitialSectionDeformation(); //Removes also initial strains.
  }

int XC::TrussSection::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::clog << getClassName() << "::" << __FUNCTION__
                << "; Warning, load over inactive element: "
                << getTag() << std::endl;
    else
      {
        if(TrussStrainLoad *trsLoad= dynamic_cast<TrussStrainLoad *>(theLoad))
          {
            const double e1= trsLoad->E1()*loadFactor;
            const double e2= trsLoad->E2()*loadFactor;
            Vector ezero(1);
	    ezero[0]= (e2+e1)/2;
            physicalProperties[0]->incrementInitialSectionDeformation(ezero);
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; load type unknown for truss with tag: "
		      << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }


int XC::TrussSection::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    const double rho= getLinearRho(); // mass per unit length.
    // check for a quick return
    if(L == 0.0 || rho == 0.0)
        return 0;

    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);

    int nodalDOF = numDOF/2;

#ifdef _G3DEBUG
    if(nodalDOF != Raccel1.Size() || nodalDOF != Raccel2.Size()) {
      std::cerr << "XC::TrussSection::addInertiaLoadToUnbalance " <<
        "matrix and vector sizes are incompatible\n";
      return -1;
    }
#endif

    double M = 0.5*rho*L;
    // want to add ( - fact * M R * accel ) to unbalance
    for(int i=0; i<getNumDIM(); i++)
      {
        double val1 = Raccel1(i);
        double val2 = Raccel2(i);

        // perform - fact * M*(R * accel) // remember M a diagonal matrix
        val1 *= -M;
        val2 *= -M;

        (*getLoad())(i) += val1;
        (*getLoad())(i+nodalDOF) += val2;
      }

    return 0;
  }

//! @brief Return the axial internal force.
double XC::TrussSection::getAxialForce(void) const
  {
    double retval= 0.0;
    if(L!=0.0)
      {
        const SectionForceDeformation *theSection= physicalProperties[0];
	const int order= theSection->getOrder();
	const ID &code= theSection->getResponseType();

	const Vector &s = theSection->getStressResultant();
	for(int i = 0; i < order; i++)
	  {
	    if(code(i) == SECTION_RESPONSE_P)
	      retval+= s(i);
	  }
      }
    if(isDead())
      retval*=dead_srf;
    return retval;
  }

//! @brief Return the element initial strain.
double XC::TrussSection::getInitialStrain(void) const
  {
    double retval= 0.0;
    const SectionForceDeformation *theSection= physicalProperties[0];
    const int order= theSection->getOrder();
    const ID &code= theSection->getResponseType();
    const Vector &s= theSection->getInitialSectionDeformation();
    for(int i = 0;i<order;i++)
      {
	if(code(i) == SECTION_RESPONSE_P)
	  retval+= s(i);
      }
    return retval;
  }

//! @brief Return the element resisting force.
const XC::Vector &XC::TrussSection::getResistingForce(void) const
  {
    const double force= getAxialForce();
    const int numDOF2 = numDOF/2;
    double temp;
    for(int i = 0; i < getNumDIM(); i++)
      {
        temp = cosX[i]*force;
        (*theVector)(i) = -temp;
        (*theVector)(i+numDOF2) = temp;
      }

    // add P
    (*theVector)-= *getLoad();

    if(isDead())
      (*theVector)*=dead_srf;
    return *theVector;
  }



const XC::Vector &XC::TrussSection::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();
    const double rho= getLinearRho(); // mass per unit length. 
    // now include the mass portion
    if(L != 0.0 && rho != 0.0)
      {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();

        const double M = 0.5*rho*L;
        const int dof = getNumDIM();
        const int start = numDOF/2;
        for(int i=0; i<dof; i++)
          {
            (*theVector)(i)+= M*accel1(i);
            (*theVector)(i+start)+= M*accel2(i);
          }
      }

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      *theVector+= this->getRayleighDampingForces();

    if(isDead())
      (*theVector)*=dead_srf; //XXX Se aplica 2 veces sobre getResistingForce: arreglar.
    return *theVector;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::TrussSection::getDbTagData(void) const
  {
    static DbTagData retval(23);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::TrussSection::sendData(Communicator &comm)
  {
    int res= TrussBase::sendData(comm);
    res+= comm.sendMovable(physicalProperties,getDbTagData(),CommMetaData(20));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::TrussSection::recvData(const Communicator &comm)
  {
    int res= TrussBase::recvData(comm);
    res+= comm.receiveMovable(physicalProperties,getDbTagData(),CommMetaData(20));
    return res;
  }

int XC::TrussSection::sendSelf(Communicator &comm)
  {
    inicComm(21);
    int res= sendData(comm);

    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data\n";
    return res;
  }

int XC::TrussSection::recvSelf(const Communicator &comm)
  {
    inicComm(21);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to recv ID data";
    else
      res+= recvData(comm);
    return res;
  }

void XC::TrussSection::Print(std::ostream &s, int flag) const
  {
    // compute the strain and axial force in the member
    double strain, force= 0.0;
    if(L == 0.0)
      {
        strain = 0;
        force = 0.0;
      }
    else
      {
	 strain= this->computeCurrentStrain();

         const SectionForceDeformation *theSection= physicalProperties[0];
	 int order = theSection->getOrder();
	 const ID &code = theSection->getResponseType();

	 Vector e(order);

	 for(int i = 0; i < order; i++)
	   {
	     if(code(i) == SECTION_RESPONSE_P)
		     e(i) = strain;
	   }
	 Vector backup= theSection->getTrialSectionDeformation();
	 SectionForceDeformation *sectNoConst= const_cast<SectionForceDeformation *>(theSection);
	 sectNoConst->setTrialSectionDeformation(e);

	 const Vector &s= theSection->getStressResultant();
	 for(int i = 0; i < order; i++)
	   {
	     if(code(i) == SECTION_RESPONSE_P)
		     force += s(i);
	   }
	 sectNoConst->setTrialSectionDeformation(backup);
      }

    double temp;
    int numDOF2 = numDOF/2;
    for(int i=0; i<getNumDIM(); i++)
      {
        temp = force*cosX[i];
        (*theVector)(i) = -force;
        (*theVector)(i+numDOF2) = force;
      }

    if(flag == 0)
      { // print everything
        s << "Element: " << this->getTag();
        s << " type: TrussSection  iNode: " << theNodes.getTagNode(0);
        s << " jNode: " << theNodes.getTagNode(1);

        s << " \n\t strain: " << strain;
        s << " axial load: " << force;
        if(theVector != 0)
            s << " \n\t unbalanced load: " << *theVector;
        s << " \t Section: " << *physicalProperties[0];
        s << std::endl;
      }
    else if(flag == 1)
      {
        s << this->getTag() << "  " << strain << "  ";
        s << force << std::endl;
      }
  }

//! @brief Compute element current strain.
double XC::TrussSection::computeCurrentStrain(void) const
  {
    // NOTE method will not be called if L == 0

    // determine the strain
    const Vector &disp1= theNodes[0]->getTrialDisp();
    const Vector &disp2= theNodes[1]->getTrialDisp();

    // Compute length increment.
    double retval= 0.0; // Length increment.
    for(int i=0; i<getNumDIM(); i++)
      {
        retval += (disp2(i)-disp1(i))*cosX[i];
      }

    // this method should never be called with L == 0
    retval/=L; // Compute strain.
    if(persistentInitialDeformation!=0.0)
      retval-= persistentInitialDeformation; // substracts persistent deformation 
    return retval;
  }

XC::Response *XC::TrussSection::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    //
    // we compare argv[0] for known response types for the XC::Truss
    //

    // axial force
    if(argv[0] == "force" || argv[0] == "forces" ||
	argv[0] == "axialForce")
      return new ElementResponse(this, 1, 0);

    else if(argv[0] == "defo" || argv[0] == "deformations" ||
	     argv[0] == "deformation")
      return new ElementResponse(this, 2, 0);

    // a section quantity
    else if(argv[0] == "section")
      return physicalProperties.setResponse(argv,eleInformation);

    // otherwise response quantity is unknown for the XC::Truss class
    else
      return 0;
  }

int XC::TrussSection::getResponse(int responseID, Information &eleInformation)
  {
    double strain, force= 0.0;

    SectionForceDeformation *theSection= physicalProperties[0];
    switch(responseID)
      {
      case 1:
	if(L == 0.0)
	  {
	    strain = 0;
	    force = 0.0;
	  }
	else
	  {
	    strain = this->computeCurrentStrain();
	    int order = theSection->getOrder();
	    const ID &code = theSection->getResponseType();
   	    Vector e(order);

	    for(int i = 0; i < order; i++)
	      {
		if(code(i) == SECTION_RESPONSE_P)
		  e(i) = strain;
	      }

	    theSection->setTrialSectionDeformation(e);

	    const Vector &s = theSection->getStressResultant();
	    for(int i = 0; i < order; i++)
	      {
		if(code(i) == SECTION_RESPONSE_P)
			force += s(i);
	      }

	  }
	eleInformation.theDouble = force;
	return 0;
      case 2:
	if(L == 0.0)
	  { strain = 0; }
	else
	  { strain = this->computeCurrentStrain(); }
	eleInformation.theDouble = strain*L;
	return 0;
      default:
	if(responseID >= 100)
	  return theSection->getResponse(responseID-100, eleInformation);
	else
	  return -1;
      }
  }

int XC::TrussSection::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // a material parameter
    if(argv[0] == "section" || argv[0] == "-section")
      {
        int ok = setMaterialParameter(physicalProperties[0],argv,1,param);
        if(ok < 0)
          return -1;
        else
          return ok + 100;
      }
    // otherwise parameter is unknown for the TrussSection class
    else
      return -1;
  }

int XC::TrussSection::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
      case -1:
        return -1;
      default:
        if(parameterID >= 100)
          return physicalProperties[0]->updateParameter(parameterID-100, info);
        else
          return -1;
      }
  }
