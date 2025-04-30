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
//BeamColumnWithSectionFD.cc

#include "BeamColumnWithSectionFD.h"


#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include <material/section/PrismaticBarCrossSection.h>
#include <utility/recorder/response/ElementResponse.h>


XC::BeamColumnWithSectionFD::BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc)
  : BeamColumn(tag,classTag), theSections(numSecc), rho(0.0)
  {}

XC::BeamColumnWithSectionFD::BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc= 1,const Material *m= nullptr)
  : BeamColumn(tag,classTag), theSections(numSecc,m), rho(0.0)
  {}

XC::BeamColumnWithSectionFD::BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const PrismaticBarCrossSection *matModel)
  : BeamColumn(tag,classTag), theSections(numSecc,matModel), rho(0.0)
  {}

XC::BeamColumnWithSectionFD::BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const PrismaticBarCrossSection *sccModel,int Nd1,int Nd2)
  : BeamColumn(tag,classTag,Nd1,Nd2), theSections(numSecc,sccModel), rho(0.0)
  {}

//! @brief Zeroes loads on element.
void XC::BeamColumnWithSectionFD::zeroLoad(void)
  {
    BeamColumn::zeroLoad();
    theSections.zeroInitialSectionDeformations(); //Removes initial strains.
  }

int XC::BeamColumnWithSectionFD::commitState(void)
  {
    int retVal = 0;

    if((retVal= BeamColumn::commitState()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in base class." << std::endl;
        return (-1);
      }
    retVal+= theSections.commitState();
    return retVal;
  }

//! @brief Revert the state of element to its last commit.
int XC::BeamColumnWithSectionFD::revertToLastCommit(void)
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    // int retval= BeamColumn::revertToLastCommit();
    int retval= theSections.revertToLastCommit();
    return retval;
  }

//! @brief Revert the state of element to initial.
int XC::BeamColumnWithSectionFD::revertToStart(void)
  {
    int retval= BeamColumn::revertToStart();
    retval+= theSections.revertToStart();
    return retval;
  }

//! @brief Try to set the element material by downcasting the
//! pointer argument.
void XC::BeamColumnWithSectionFD::set_material(const Material *m)
  {
    if(m)
      {
        const PrismaticBarCrossSection *scc= dynamic_cast<const PrismaticBarCrossSection *>(m);
        if(scc)
          setSection(scc);
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; material type: '" << m->getClassName()
	              << "' is not valid.\n";
          }
      }
    else
      if(verbosity>0)
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; pointer to material is null." << std::endl;
  }
//! @brief Set the element material.
void XC::BeamColumnWithSectionFD::setMaterial(const std::string &matName)
  {
    const Material *ptr_mat= get_material_ptr(matName);
    if(ptr_mat)
      { set_material(ptr_mat); }
  }

void XC::BeamColumnWithSectionFD::setSection(const PrismaticBarCrossSection *matModel)
  { theSections.setSection(matModel); }

bool XC::BeamColumnWithSectionFD::setSections(const std::vector<PrismaticBarCrossSection *> &sectionPtrs)
  { return theSections.setSections(sectionPtrs); }
void XC::BeamColumnWithSectionFD::setTrialSectionDeformations(const std::vector<Vector> &vs)
  { return theSections.setTrialSectionDeformations(vs); }

//! @brief Returns a pointer to the i-th section of the element.
const XC::PrismaticBarCrossSection *XC::BeamColumnWithSectionFD::getSectionPtr(const size_t &i) const
  { return theSections[i]; }

XC::Response* XC::BeamColumnWithSectionFD::setSectionResponse(PrismaticBarCrossSection *theSection,const std::vector<std::string> &argv,const size_t &offset, Information &info)
  {
    Response *retval= nullptr;
    if(theSection)
      {
        std::vector<std::string> argvOffset(argv);
        argvOffset.erase(argvOffset.begin(),argvOffset.begin()+offset);
        retval= theSection->setResponse(argvOffset,info);
      }
    return retval;
  }

int XC::BeamColumnWithSectionFD::setSectionParameter(PrismaticBarCrossSection *theSection,const std::vector<std::string> &argv,const size_t &offset, Parameter &param)
  {
    int retval= -1;
    if(theSection)
      {
        std::vector<std::string> argvOffset(argv);
        argvOffset.erase(argvOffset.begin(),argvOffset.begin()+offset);
        retval= theSection->setParameter(argvOffset,param);
      }
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::BeamColumnWithSectionFD::sendData(Communicator &comm)
  {
    int res= BeamColumn::sendData(comm);
    res+= comm.sendMovable(theSections,getDbTagData(),CommMetaData(7));
    res+= comm.sendMovable(section_matrices,getDbTagData(),CommMetaData(8));
    res+= comm.sendDouble(rho,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::BeamColumnWithSectionFD::recvData(const Communicator &comm)
  {
    int res= BeamColumn::recvData(comm);
    res+= comm.receiveMovable(theSections,getDbTagData(),CommMetaData(7));
    res+= comm.receiveMovable(section_matrices,getDbTagData(),CommMetaData(8));
    res+= comm.receiveDouble(rho,getDbTagData(),CommMetaData(9));
    return res;
  }


