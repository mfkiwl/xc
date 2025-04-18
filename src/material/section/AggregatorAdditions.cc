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
//AggregatorAdditions.cc

#include "AggregatorAdditions.h"


#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::AggregatorAdditions::AggregatorAdditions(CommandEntity *owner)
  : DqUniaxialMaterial(owner), matCodes(0) {}

//! @brief Constructor.
XC::AggregatorAdditions::AggregatorAdditions(CommandEntity *owner,const UniaxialMaterial &um, int c)
  : DqUniaxialMaterial(owner,um), matCodes(1)
  {
    matCodes(0)= c;
  }

//! @brief Copy constructor.
XC::AggregatorAdditions::AggregatorAdditions(const AggregatorAdditions &other)
  : DqUniaxialMaterial(other), matCodes(other.matCodes)
  {}

//! @brief Copy constructor.
XC::AggregatorAdditions::AggregatorAdditions(const AggregatorAdditions &other,SectionForceDeformation *s)
  : DqUniaxialMaterial(other,s), matCodes(other.matCodes)
  {}

//! @brief Destructor.
XC::AggregatorAdditions::~AggregatorAdditions(void)
  {}


void XC::AggregatorAdditions::putMatCodes(const ResponseId &codes)
  { matCodes= codes; }

//! @brief Response identifiers for material stiffness contributions.
void XC::AggregatorAdditions::getResponseType(ResponseId &retval,const size_t &offset) const
  {
    const size_t n= size();
    for( size_t i=0;i<n;i++)
      retval(offset+i)= matCodes(i);
  }

//! @brief Print stuff.
void XC::AggregatorAdditions::Print(std::ostream &s, int flag) const
  {
    DqUniaxialMaterial::Print(s,flag);
    s << "\tUniaxial codes " << matCodes << std::endl;
  }

//! @brief Send object members through the communicator argument.
int XC::AggregatorAdditions::sendData(Communicator &comm)
  {
    int res= DqUniaxialMaterial::sendData(comm);
    res+= comm.sendID(matCodes,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::AggregatorAdditions::recvData(const Communicator &comm)
  {
    int res= DqUniaxialMaterial::recvData(comm);
    res+= comm.receiveID(matCodes,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::AggregatorAdditions::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::AggregatorAdditions::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.."
	        << Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.."
		    << Color::def << std::endl;
      }
    return res;
  }
