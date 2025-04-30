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
//NDMaterialPhysicalProperties.cc

#include "SolidMech2D.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::SolidMech2D::SolidMech2D(const size_t &nMat,const NDMaterial *ptr_mat, const double &t)
  :NDMaterialPhysicalProperties(nMat,ptr_mat), thickness(t)
  {}

//! @brief Constructor.
XC::SolidMech2D::SolidMech2D(const size_t &nMat, NDMaterial &theMat,const std::string &type, const double &t)
  : NDMaterialPhysicalProperties(nMat,nullptr), thickness(t)
  {
    if(check_material_type(type))
      theMaterial.setMaterial(&theMat,type);
  }

//! @brief Return the strain or stress vector index corresponding to the given
//! code.
int XC::SolidMech2D::getComponentIndexFromCode(const std::string &code) const
  {
    int retval= -1;
    const std::deque<int> tmp= theMaterial.getComponentIndexesFromCode(code);
    const size_t sz= tmp.size();
    if(sz>0)
      {
        retval= tmp[0];
	if(sz>1)
	  std::cerr << Color::yellow << getClassName() << "::" << __FUNCTION__
		    << "; code: '" << code << "' returns "
		    << sz << " indexes, instead of one."
		    << Color::def << std::endl;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; code: '" << code << "' unknown."
	        << Color::def << std::endl;
    return retval;
  }

//! Returns mean value of internal force.
double XC::SolidMech2D::getMeanInternalForce(const std::string &cod) const
  {
    double retval= theMaterial.getMeanGeneralizedStress(cod);
    retval*= this->thickness;
    return retval;
  }

//! Returns mean value of internal force.
double XC::SolidMech2D::getMeanInternalDeformation(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1") //Average axial force per unit length, parallel to the axis 1.
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2") //Average axial force per unit length, parallel to the axis 2.
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n2);
    else if(cod == "n12")
      retval= theMaterial.getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n12);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< " unknown internal deformation: '"
                << cod << "'\n";
    return retval;
  }

#define DBTAGS_SIZE 3
//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::SolidMech2D::getDbTagData(void) const
  {
    static DbTagData retval(DBTAGS_SIZE); 
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::SolidMech2D::sendData(Communicator &comm)
  {
    int res= NDMaterialPhysicalProperties::sendData(comm);
    res+= comm.sendDouble(thickness,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::SolidMech2D::recvData(const Communicator &comm)
  {
    int res= NDMaterialPhysicalProperties::recvData(comm);
    res+= comm.receiveDouble(thickness,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object.
int XC::SolidMech2D::sendSelf(Communicator &comm)
  {
    inicComm(DBTAGS_SIZE);

    int res= this->sendData(comm);

    const int dataTag= getDbTag();
    res += comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< ";failed to send ID data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object
int XC::SolidMech2D::recvSelf(const Communicator &comm)
  {
    inicComm(DBTAGS_SIZE);

    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< ";failed to send ID data."
	        << Color::def << std::endl;
    else
      res+= this->recvData(comm);
    return res;
  }
