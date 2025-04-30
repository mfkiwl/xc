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
                                                                        
// Written: Chris McGann
//          February 2011

#include "DruckerPragerPlaneStrain.h"
#include "material/ResponseId.h"

//static vectors and matrices
XC::Vector XC::DruckerPragerPlaneStrain::strain(3);
XC::Vector XC::DruckerPragerPlaneStrain::stress(3);
XC::Matrix XC::DruckerPragerPlaneStrain::tangent(3,3);

//null constructor
XC::DruckerPragerPlaneStrain::DruckerPragerPlaneStrain(int tag)
  : DruckerPrager(tag, ND_TAG_DruckerPragerPlaneStrain) {}

//full constructor
XC::DruckerPragerPlaneStrain::DruckerPragerPlaneStrain(int tag, double bulk, double shear,
		  double s_y, double r, double r_bar, double Kinfinity, double Kinit, 
		  double d1, double d2, double H, double t, double mDen, double atm)
  : DruckerPrager(tag, ND_TAG_DruckerPragerPlaneStrain, bulk, shear, s_y, r, r_bar, Kinfinity, Kinit, d1, d2, H, t, mDen, atm) {}

//destructor
XC::DruckerPragerPlaneStrain::~DruckerPragerPlaneStrain(void) 
  {} 

//make a clone of this material
XC::NDMaterial *XC::DruckerPragerPlaneStrain::getCopy(void) const
  { return new DruckerPragerPlaneStrain(*this); }

//send back type of material
const std::string &XC::DruckerPragerPlaneStrain::getType(void) const 
  {
    static const std::string retval= "PlaneStrain";
    return retval;
  }

//send back order of strain in vector form
int XC::DruckerPragerPlaneStrain::getOrder() const 
  { 
    return 3; 
  }

//! @brief Return the ordering and type of response quantities
//! returned by this material.
//!
//! Return the section ResponseId code that indicates
//! the ordering and type of response quantities returned
//! by the material. Lets the calling object
//! (e.g. an Element) know how to interpret the quantites returned by this
//! object.
const XC::ResponseId &XC::DruckerPragerPlaneStrain::getResponseType(void) const
  { return RespSolidMechanics2D; }

//get the strain and integrate plasticity equations
int XC::DruckerPragerPlaneStrain::setTrialStrain(const Vector &strain_from_element) 
  {
    mEpsilon.Zero();
    mEpsilon(0) = strain_from_element(0);
    mEpsilon(1) = strain_from_element(1);
    mEpsilon(3) = strain_from_element(2);

    this->plastic_integrator();
    return 0;
  }

//unused trial strain function
int XC::DruckerPragerPlaneStrain::setTrialStrain(const Vector &v, const Vector &r)
  {
    std::cerr << "YOU SHOULD NOT SEE THIS: DruckerPrager::setTrialStrain (const Vector &v, const Vector &r)" << std::endl;
    return this->setTrialStrain (v);
  }

//! @brief Return the strain.
const XC::Vector &XC::DruckerPragerPlaneStrain::getStrain(void) const 
  {
    strain(0) = mEpsilon(0);
    strain(1) = mEpsilon(1);
    strain(2) = mEpsilon(3);
    return strain;
  } 

//! @brief Return the stress.
const XC::Vector &XC::DruckerPragerPlaneStrain::getStress(void) const 
  {
     stress(0) = mSigma(0);
     stress(1) = mSigma(1);
     stress(2) = mSigma(3);	
     return stress;
  }

//! @brief Return the material tangent stiffness.
const XC::Matrix &XC::DruckerPragerPlaneStrain::getTangent(void) const 
  {
    tangent(0,0) = mCep(0,0);
    tangent(0,1) = mCep(0,1);
    tangent(0,2) = mCep(0,3);
    tangent(1,0) = mCep(1,0);
    tangent(1,1) = mCep(1,1);
    tangent(1,2) = mCep(1,3);
    tangent(2,0) = mCep(3,0);
    tangent(2,1) = mCep(3,1);
    tangent(2,2) = mCep(3,3);	
    return tangent;
  } 

//! @brief Return the material initial stiffness.
const XC::Matrix& XC::DruckerPragerPlaneStrain::getInitialTangent(void) const 
  {
    tangent(0,0)= mCep(0,0);
    tangent(0,1)= mCep(0,1);
    tangent(0,2)= mCep(0,3);
    tangent(1,0)= mCep(1,0);
    tangent(1,1)= mCep(1,1);
    tangent(1,2)= mCep(1,3);
    tangent(2,0)= mCep(3,0);
    tangent(2,1)= mCep(3,1);
    tangent(2,2)= mCep(3,3);	
    return tangent;
  } 

