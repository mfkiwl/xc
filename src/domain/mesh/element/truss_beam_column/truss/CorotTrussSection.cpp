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

// $Revision: 1.7 $
// $Date: 2003/03/12 19:20:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/truss/CorotTrussSection.cpp,v $

// Written: MHS
// Created: May 2001
//
// Description: This file contains the class implementation for XC::CorotTrussSection.

#include <domain/mesh/element/truss_beam_column/truss/CorotTrussSection.h>
#include <domain/mesh/element/utils/Information.h>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <material/section/SectionForceDeformation.h>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <utility/recorder/response/ElementResponse.h>
#include "material/ResponseId.h"
#include "domain/load/beam_loads/TrussStrainLoad.h"


//! @brief Constructor:
XC::CorotTrussSection::CorotTrussSection(int tag, int dim,int Nd1, int Nd2, SectionForceDeformation &theSec)
  :CorotTrussBase(tag,ELE_TAG_CorotTrussSection,dim,Nd1,Nd2),
   physicalProperties(1, &theSec), persistentInitialDeformation(0.0)
  {}


//! @brief Constructor.
XC::CorotTrussSection::CorotTrussSection(int tag,int dim,const Material *ptr_mat)
  : CorotTrussBase(tag,ELE_TAG_CorotTrussSection,dim,0,0),
    physicalProperties(1,ptr_mat), persistentInitialDeformation(0.0)
  {}

//! @brief Constructor:
XC::CorotTrussSection::CorotTrussSection(void)
  :CorotTrussBase(0,ELE_TAG_CorotTrussSection,0,0,0),
   physicalProperties(1), persistentInitialDeformation(0.0)
  {}

//! @brief Virtual constructor.
XC::Element* XC::CorotTrussSection::getCopy(void) const
  { return new CorotTrussSection(*this); }


//! @brief Returns the value of the persistent (does not get wiped out by
//! zeroLoad) initial deformation of the section.
const double &XC::CorotTrussSection::getPersistentInitialSectionDeformation(void) const
  { return persistentInitialDeformation; }

//! @brief Increments the persistent (does not get wiped out by zeroLoad)
//! initial deformation of the section. It's used to store the deformation
//! of the material during the periods in which the element is deactivated
//! (see for example XC::BeamColumnWithSectionFD::alive().
void XC::CorotTrussSection::incrementPersistentInitialDeformationWithCurrentDeformation(void)
  { persistentInitialDeformation+= this->computeCurrentStrain(); }

//! @brief setDomain()
//!    to set a link to the enclosing XC::Domain and to set the node pointers.
//!    also determines the number of dof associated
//!    with the XC::CorotTrussSection element, we set matrix and vector pointers,
//!    allocate space for t matrix, determine the length
//!    and set the transformation matrix.
void XC::CorotTrussSection::setDomain(Domain *theDomain)
  {
    // check XC::Domain is not null - invoked when object removed from a domain
    CorotTrussBase::setDomain(theDomain);
    if(!theDomain)
     {
       Lo = 0.0;
       Ln = 0.0;
       return;
     }


    // now determine the number of dof and the dimension
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();

    // if differing dof at the ends - print a warning message
    if(dofNd1 != dofNd2)
      {
	std::cerr << "WARNING XC::CorotTrussSection::setDomain(): nodes have differing dof at ends for XC::CorotTrussSection" <<
	this->getTag() << std::endl;
	// fill this in so don't segment fault later
	numDOF = 6;
	return;
      }

     setup_matrix_vector_ptrs(dofNd1);

     // call the base class method
     CorotTrussBase::setDomain(theDomain);

     // now determine the length, cosines and fill in the transformation
	  // NOTE t = -t(every one else uses for residual calc)
	  const Vector &end1Crd = theNodes[0]->getCrds();
	  const Vector &end2Crd = theNodes[1]->getCrds();

	  // Determine global offsets
      double cosX[3];
      cosX[0] = 0.0;  cosX[1] = 0.0;  cosX[2] = 0.0;
      int i;
      for(i = 0; i < getNumDIM(); i++) {
	  cosX[i] += end2Crd(i)-end1Crd(i);
      }

	  // Set undeformed and initial length
	  Lo = cosX[0]*cosX[0] + cosX[1]*cosX[1] + cosX[2]*cosX[2];
	  Lo = sqrt(Lo);
	  Ln = Lo;

      // Initial offsets
	     d21[0] = Lo;
	  d21[1] = 0.0;
	  d21[2] = 0.0;

	  // Set global orientation
	  cosX[0] /= Lo;
	  cosX[1] /= Lo;
	  cosX[2] /= Lo;

	  R(0,0) = cosX[0];
	  R(0,1) = cosX[1];
	  R(0,2) = cosX[2];

	  // Element lies outside the YZ plane
	  if(fabs(cosX[0]) > 0.0) {
		  R(1,0) = -cosX[1];
		  R(1,1) =  cosX[0];
		  R(1,2) =  0.0;

		  R(2,0) = -cosX[0]*cosX[2];
		  R(2,1) = -cosX[1]*cosX[2];
		  R(2,2) =  cosX[0]*cosX[0] + cosX[1]*cosX[1];
	  }
	  // Element is in the YZ plane
	  else {
		  R(1,0) =  0.0;
		  R(1,1) = -cosX[2];
		  R(1,2) =  cosX[1];

		  R(2,0) =  1.0;
		  R(2,1) =  0.0;
		  R(2,2) =  0.0;
	  }

	  // Orthonormalize last two rows of R
	  double norm;
	  for(i = 1; i < 3; i++) {
		  norm = sqrt(R(i,0)*R(i,0) + R(i,1)*R(i,1) + R(i,2)*R(i,2));
		  R(i,0) /= norm;
		  R(i,1) /= norm;
		  R(i,2) /= norm;
	  }
  }

//! @brief Commit element state.
int XC::CorotTrussSection::commitState(void)
  {
    int retVal = 0;
    // call element commitState to do any base class stuff
    if((retVal = this->CorotTrussBase::commitState()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed in base class." << std::endl;
      }
    retVal+= physicalProperties.commitState();
    return retVal;
  }

//! @brief Revert the element to its last committed state.
int XC::CorotTrussSection::revertToLastCommit()
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    //  int retval= CorotTrussBase::revertToLastCommit();
    // Revert the material
    int retval= physicalProperties.revertToLastCommit();
    return retval;
  }

//! @brief Revert the element to its initial state.
int XC::CorotTrussSection::revertToStart()
  {
    int retval= CorotTrussBase::revertToStart();
    // Revert the material to start
    retval+= physicalProperties.revertToStart();
    return retval;
  }

//! @brief Update element state.
double XC::CorotTrussSection::computeCurrentStrain(void) const
  {
    // Nodal displacements
    const Vector &end1Disp = theNodes[0]->getTrialDisp();
    const Vector &end2Disp = theNodes[1]->getTrialDisp();

    // Initial offsets
    d21[0] = Lo;
    d21[1] = 0.0;
    d21[2] = 0.0;

    // Update offsets in basic system due to nodal displacements
    for(int i = 0; i < getNumDIM(); i++)
      {
        d21[0] += R(0,i)*(end2Disp(i)-end1Disp(i));
        d21[1] += R(1,i)*(end2Disp(i)-end1Disp(i));
        d21[2] += R(2,i)*(end2Disp(i)-end1Disp(i));
      }

    // Compute new length
    Ln = d21[0]*d21[0] + d21[1]*d21[1] + d21[2]*d21[2];
    Ln = sqrt(Ln);

    // Compute engineering strain
    return (Ln-Lo)/Lo - persistentInitialDeformation;
  }

//! @brief Update element state.
int XC::CorotTrussSection::update(void)
  {
    // Compute engineering strain
    const double strain= computeCurrentStrain();

    SectionForceDeformation *theSection= physicalProperties[0];
    int order= theSection->getOrder();
    const ID &code= theSection->getResponseType();

    static double data[10];
    Vector e(data, order);
    for(int i = 0; i < order; i++)
      {
	if(code(i) == SECTION_RESPONSE_P)
	  e(i)= strain;
	else
	  e(i)= 0.0;
      }

    // Set material trial strain
    return theSection->setTrialSectionDeformation(e);
  }

const XC::Matrix &XC::CorotTrussSection::getTangentStiff(void) const
  {
    static Matrix kl(3,3);

    // Material stiffness
    //
    // Get material tangent
    const SectionForceDeformation *theSection= physicalProperties[0];
    int order= theSection->getOrder();
    const ID &code= theSection->getResponseType();

    const Matrix &ks = theSection->getSectionTangent();
    const Vector &s  = theSection->getStressResultant();

    double EA = 0.0;
    double q = 0.0;

        int i,j;
        for(i = 0; i < order; i++) {
                if(code(i) == SECTION_RESPONSE_P) {
                        EA += ks(i,i);
                        q  += s(i);
                }
        }

        EA /= (Ln*Ln*Lo);

    for(i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            kl(i,j) = EA*d21[i]*d21[j];

        // Geometric stiffness
        //
        // Get material stress
        double SA = q/(Ln*Ln*Ln);
        double SL = q/Ln;

    for(i = 0; i < 3; i++) {
        kl(i,i) += SL;
        for(j = 0; j < 3; j++)
            kl(i,j) -= SA*d21[i]*d21[j];
    }

    // Compute R'*kl*R
    static XC::Matrix kg(3,3);
    kg.addMatrixTripleProduct(0.0, R, kl, 1.0);

    Matrix &K = *theMatrix;
    K.Zero();

    // Copy stiffness into appropriate blocks in element stiffness
    int numDOF2 = numDOF/2;
    for(i = 0; i < getNumDIM(); i++) {
        for(j = 0; j < getNumDIM(); j++) {
            K(i,j)                 =  kg(i,j);
            K(i,j+numDOF2)         = -kg(i,j);
            K(i+numDOF2,j)         = -kg(i,j);
            K(i+numDOF2,j+numDOF2) =  kg(i,j);
        }
    }
    if(isDead())
      (*theMatrix)*=dead_srf;
    return *theMatrix;
  }

const XC::Matrix &XC::CorotTrussSection::getInitialStiff(void) const
  {
    static Matrix kl(3,3);

    // Material stiffness
    //
    // Get material tangent
    const SectionForceDeformation *theSection= physicalProperties[0];
    int order = theSection->getOrder();
    const ID &code= theSection->getResponseType();

    const Matrix &ks = theSection->getInitialTangent();

    double EA = 0.0;

    int i,j;
    for(i = 0; i < order; i++) {
      if(code(i) == SECTION_RESPONSE_P) {
        EA += ks(i,i);
      }
    }

    kl(0,0) = EA / Lo;

    // Compute R'*kl*R
    static Matrix kg(3,3);
    kg.addMatrixTripleProduct(0.0, R, kl, 1.0);

    Matrix &K = *theMatrix;
    K.Zero();

    // Copy stiffness into appropriate blocks in element stiffness
    int numDOF2 = numDOF/2;
    for(i = 0; i < getNumDIM(); i++) {
      for(j = 0; j < getNumDIM(); j++) {
        K(i,j)                 =  kg(i,j);
        K(i,j+numDOF2)         = -kg(i,j);
        K(i+numDOF2,j)         = -kg(i,j);
        K(i+numDOF2,j+numDOF2) =  kg(i,j);
      }
    }
    if(isDead())
      (*theMatrix)*=dead_srf;
    return *theMatrix;
}

const XC::Material *XC::CorotTrussSection::getMaterial(void) const
  { return physicalProperties[0]; }
XC::Material *XC::CorotTrussSection::getMaterial(void)
  { return physicalProperties[0]; }

//! @brief Return the linear density of the section.
double XC::CorotTrussSection::getRho(void) const
  { return physicalProperties[0]->getLinearRho(); }

//! @brief Returns the mass per unit length.
double XC::CorotTrussSection::getLinearRho(void) const
  { return getRho(); }

//! @brief Return the mass matrix.
const XC::Matrix &XC::CorotTrussSection::getMass(void) const
  {
    Matrix &Mass = *theMatrix;
    Mass.Zero();

    const double rho= getLinearRho(); // mass per unit length.
    // check for quick return
    if(Lo == 0.0 || rho == 0.0)
        return Mass;

    const double M= 0.5*rho*Lo;
    int numDOF2 = numDOF/2;
    for(int i = 0; i < getNumDIM(); i++)
      {
        Mass(i,i)= M;
        Mass(i+numDOF2,i+numDOF2)= M;
      }

    if(isDead())
      (*theMatrix)*=dead_srf;
    return *theMatrix;
  }

//! @brief Reactivates the element.
void XC::CorotTrussSection::alive(void)
  {
    if(isDead())
      {
	// Store the current deformation.
        this->incrementPersistentInitialDeformationWithCurrentDeformation();
	CorotTrussBase::alive(); // Not dead anymore.
      }
  } 

//! @brief Make loads zero.
void XC::CorotTrussSection::zeroLoad(void)
  {
    CorotTrussBase::zeroLoad();
    (*physicalProperties[0]).zeroInitialSectionDeformation(); //Removes also initial strains.
  }

int XC::CorotTrussSection::addLoad(ElementalLoad *theLoad, double loadFactor)
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

//! @brief Add inertia load to the out of balance vector.
int XC::CorotTrussSection::addInertiaLoadToUnbalance(const Vector &accel)
  {
    return 0;
  }

//! @brief Return the axial internal force.
double XC::CorotTrussSection::getAxialForce(void) const
  {
    double retval= 0.0;
    const SectionForceDeformation *theSection= physicalProperties[0];
    const int order= theSection->getOrder();
    const ID &code= theSection->getResponseType();
    const Vector &s= theSection->getStressResultant();
    for(int i = 0;i<order;i++)
      {
	if(code(i) == SECTION_RESPONSE_P)
	  retval+= s(i);
      }
    if(isDead())
      retval*=dead_srf;
    return retval;
  }

//! @brief Return the element initial strain.
double XC::CorotTrussSection::getInitialStrain(void) const
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
const XC::Vector &XC::CorotTrussSection::getResistingForce(void) const
  {
    const double SA= getAxialForce()/Ln;

    static Vector ql(3);

    ql(0) = d21[0]*SA;
    ql(1) = d21[1]*SA;
    ql(2) = d21[2]*SA;

    static Vector qg(3);
    qg.addMatrixTransposeVector(0.0, R, ql, 1.0);

    Vector &P= *theVector;
    P.Zero();

    // Copy forces into appropriate places
    int numDOF2 = numDOF/2;
    for(int i = 0; i < getNumDIM(); i++)
      {
        P(i)         = -qg(i);
        P(i+numDOF2) =  qg(i);
      }

    if(isDead())
      (*theVector)*=dead_srf;
    return *theVector;
  }

const XC::Vector &XC::CorotTrussSection::getResistingForceIncInertia(void) const
  {
    Vector &P = *theVector;
    P = this->getResistingForce();

    const double rho= getLinearRho(); // mass per unit length.
    if(rho != 0.0) {

      const Vector &accel1 = theNodes[0]->getTrialAccel();
      const Vector &accel2 = theNodes[1]->getTrialAccel();

      double M = 0.5*rho*Lo;
      int numDOF2 = numDOF/2;
      for(int i = 0; i < getNumDIM(); i++) {
        P(i)         += M*accel1(i);
        P(i+numDOF2) += M*accel2(i);
      }
    }

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      *theVector+= this->getRayleighDampingForces();

    if(isDead())
      (*theVector)*=dead_srf; //XXX Se aplica 2 veces sobre getResistingForce: arreglar.
    return *theVector;
  }

int XC::CorotTrussSection::sendSelf(Communicator &comm)
  { return -1; }

int XC::CorotTrussSection::recvSelf(const Communicator &comm)
  { return -1; }

void XC::CorotTrussSection::Print(std::ostream &s, int flag) const
  {
    s << "\nCorotTrussSection, tag: " << this->getTag() << std::endl;
    s << "\tConnected Nodes: " << theNodes;
    s << "\tUndeformed Length: " << Lo << std::endl;
    s << "\tCurrent Length: " << Ln << std::endl;
    s << "\tRotation matrix: " << std::endl;

    const SectionForceDeformation *theSection= physicalProperties[0];
    if(theSection)
      {
	s << "\tSection, tag: " << theSection->getTag() << std::endl;
	theSection->Print(s,flag);
      }
  }

XC::Response *XC::CorotTrussSection::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    // a material quantity
    if(argv[0] == "section")
      return physicalProperties.setResponse(argv,eleInfo);
    else
      return 0;
  }

int XC::CorotTrussSection::getResponse(int responseID, Information &eleInfo)
  { return 0; }
