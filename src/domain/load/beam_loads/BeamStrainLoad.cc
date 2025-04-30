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
//BeamStrainLoad.cpp

#include <domain/load/beam_loads/BeamStrainLoad.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "material/ResponseId.h"
#include "domain/mesh/element/truss_beam_column/BeamColumn.h"
#include "material/section/PrismaticBarCrossSection.h"

XC::BeamStrainLoad::BeamStrainLoad(int tag, const ID &theElementTags)
  : BeamLoad(tag, LOAD_TAG_BeamStrainLoad, theElementTags) {}

XC::BeamStrainLoad::BeamStrainLoad(int tag)
  : BeamLoad(tag, LOAD_TAG_BeamStrainLoad) {}

//! @brief Return the category of this kind of loads.
std::string XC::BeamStrainLoad::Category(void) const
  { return "beam_strain"; }

const XC::Vector &XC::BeamStrainLoad::getData(int &type, const double &loadFactor) const
  {
    type = getClassTag();
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    static const Vector trash;
    return trash;
  }

//! @brief Send data through the communicator argument.
int XC::BeamStrainLoad::sendData(Communicator &comm)
  {
    int res= BeamLoad::sendData(comm);
    res+= comm.sendMovable(backEndDeformationsPlane,getDbTagData(),CommMetaData(5));
    res+= comm.sendMovable(frontEndDeformationPlane,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::BeamStrainLoad::recvData(const Communicator &comm)
  {
    int res= BeamLoad::recvData(comm);
    res+= comm.receiveMovable(backEndDeformationsPlane,getDbTagData(),CommMetaData(5));;
    res+= comm.receiveMovable(frontEndDeformationPlane,getDbTagData(),CommMetaData(6));;
    return res;
  }

int XC::BeamStrainLoad::sendSelf(Communicator &comm)
  {
    static ID data(3);
    int res= sendData(comm);
    const int dataTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BeamStrainLoad::sendSelf() - failed to send extra data\n";    
    return res;
  }

int XC::BeamStrainLoad::recvSelf(const Communicator &comm)
  {
    static ID data(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BeamStrainLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(comm);
    return res;
  }

const XC::Vector &XC::BeamStrainLoad::getSection1Deformation(const size_t &order,const ResponseId &code) const
  { return backEndDeformationsPlane.getDeformation(order,code); }

const XC::Vector &XC::BeamStrainLoad::getSection2Deformation(const size_t &order,const ResponseId &code) const
  { return frontEndDeformationPlane.getDeformation(order,code); }

//! @brief Return the strains at both ends of the beam element.
XC::Matrix XC::BeamStrainLoad::getElementStrainsMatrix(const Element &e) const
  {
    Matrix retval;
    const int elemTag= e.getTag();
    if(this->actsOnElement(elemTag))
      {
	const BeamColumn *ptrBeam= dynamic_cast<const BeamColumn *>(&e);
	if(ptrBeam)
	  {
	    const PrismaticBarCrossSection *section= ptrBeam->getSectionPtr(0);
	    const size_t order= section->getOrder();
	    const ResponseId &code= section->getResponseType();
	    const Vector &e1= this->getSection1Deformation(order,code);
	    const Vector &e2= this->getSection2Deformation(order,code);
	    const int nCols= e1.Size();
	    retval.resize(2, nCols);
	    for(int j= 0; j<nCols;j++)
	      {
		retval(0,j)= e1(j);
		retval(1,j)= e2(j);
	      }
	  }
	else
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << ": element: " << elemTag
		      << " with type: "
		      << e.getClassName()
		      << " is incompatible with this type of load."
		      << std::endl;
	  }
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << ": element with tag: " << elemTag
		  << " not loaded."
		  << std::endl;
      }
    return retval;
  }
    
