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
//CrossSectionProperties2d.cc

#include <material/section/repres/CrossSectionProperties2d.h>
#include "utility/utils/misc_utils/inertia.h"
#include "utility/geom/coo_sys/ref_sys/PrincipalAxes2D.h"
#include "domain/mesh/element/utils/Information.h"
#include "material/ResponseId.h"

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/matrix/ID.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/component/Parameter.h"
#include "material/section/SectionForceDeformation.h"

XC::Matrix XC::CrossSectionProperties2d::ks2(2,2);
XC::Matrix XC::CrossSectionProperties2d::ks3(3,3);

bool XC::CrossSectionProperties2d::check_values(void)
  {
    bool retval= CrossSectionProperties1d::check_values();
    if(g <= 0.0)
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; Input G <= 0.0 ... setting G to 1.0\n";
        g= 1.0;
        retval= false;
      }
    if(i <= 0.0)
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; Input I <= 0.0 ... setting I to 1.0\n";
        i= 1.0;
        retval= false;
      }
    if(alpha <= 0.0)
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; Input alpha <= 0.0 ... setting alpha to 1.0\n";
        alpha= 1.0;
        retval= false;
      }
    return retval;
  }

//! @brief Constructor.
XC::CrossSectionProperties2d::CrossSectionProperties2d(void)
  : CrossSectionProperties1d(), g(0.0) , i(0), alpha(0) {}

//! @brief Constructor.
XC::CrossSectionProperties2d::CrossSectionProperties2d(double EA_in, double EI_in)
  : CrossSectionProperties1d(EA_in), g(0.0), i(EI_in), alpha(0)
  { check_values(); }

//! @brief Constructor.
XC::CrossSectionProperties2d::CrossSectionProperties2d(const SectionForceDeformation &section)
  : CrossSectionProperties1d(section), g(1.0), i(0.0), alpha(0)
  {
    const Matrix &sectTangent= section.getInitialTangent();
    const ResponseId &sectCode= section.getResponseType();
    for(int i=0; i<sectCode.Size(); i++)
      {
	int code = sectCode(i);
	switch(code)
	  {
	  case SECTION_RESPONSE_MZ:
	    i = sectTangent(i,i);
	    break;
	  default:
	    break;
	  }
      }
  }

//! @brief Constructor (2D cross sections).
XC::CrossSectionProperties2d::CrossSectionProperties2d(double E_in, double A_in, double I_in, double G_in, double a, double r)
  : CrossSectionProperties1d(E_in,A_in,r), g(G_in), i(I_in), alpha(a)
  { check_values(); }

//! @brief Returns the angle between the principal axes and the local system.
double XC::CrossSectionProperties2d::getTheta(void) const
  { return 0.0; }

//! @brief Returns the major principal axis of inertia.
double XC::CrossSectionProperties2d::getI1(void) const
  { return I(); }

//! @brief Returns the minor principal axis of inertia.
double XC::CrossSectionProperties2d::getI2(void) const
  { return 0.0; }

//! @brief Returns the section gyration radius.
double XC::CrossSectionProperties2d::getRadiusOfGyration(void) const
  { return sqrt(this->i/this->A()); }

//! @brief Returns the principal axis of inertia.
PrincipalAxes2D XC::CrossSectionProperties2d::getInertiaAxes(void) const
  {
    const Pos2d center_of_mass(0,0);
    return PrincipalAxes2D(center_of_mass,0.0,I(),0.0);
  }

//! @brief Returns principal axis I (strong).
Vector2d XC::CrossSectionProperties2d::getAxis1VDir(void) const
  { return getInertiaAxes().getAxis1VDir(); }
//! @brief Returns principal axis I (strong).
Vector2d XC::CrossSectionProperties2d::getVDirStrongAxis(void) const
  { return getInertiaAxes().getAxis1VDir(); }
//! @brief Returns principal axis II (weak).
Vector2d XC::CrossSectionProperties2d::getAxis2VDir(void) const
  { return getInertiaAxes().getAxis2VDir(); }
//! @brief Returns principal axis II (weak).
Vector2d XC::CrossSectionProperties2d::getVDirWeakAxis(void) const
  { return getInertiaAxes().getAxis2VDir(); }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::CrossSectionProperties2d::getSectionTangent2x2(void) const
  {
    ks2(0,0)= EA(); //Axial stiffness.
    ks2(1,1)= EI(); //z bending stiffness.
    return ks2;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::CrossSectionProperties2d::getInitialTangent2x2(void) const
  { return getSectionTangent2x2(); }

//! @brief Returns the flexibility matrix.
const XC::Matrix &XC::CrossSectionProperties2d::getSectionFlexibility2x2(void) const
  {
    ks2(0,0)= 1.0/(EA());
    ks2(1,1)= 1.0/(EI());
    return ks2;
  }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::CrossSectionProperties2d::getInitialFlexibility2x2(void) const
  { return getSectionFlexibility2x2(); }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::CrossSectionProperties2d::getSectionTangent3x3(void) const
  {
    ks3(0,0)= EA(); //Axial stiffness.
    ks3(1,1)= EI(); //z bending stiffness.
    ks3(2,2)= GAAlpha(); //Shear stiffness.
    return ks3;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::CrossSectionProperties2d::getInitialTangent3x3(void) const
  { return getSectionTangent3x3(); }

//! @brief Returns the flexibility matrix.
const XC::Matrix &XC::CrossSectionProperties2d::getSectionFlexibility3x3(void) const
  {
    ks3(0,0)= 1.0/(EA());
    ks3(1,1)= 1.0/(EI());
    ks3(2,2)= 1.0/GAAlpha(); //Shear stiffness.
    return ks3;
  }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::CrossSectionProperties2d::getInitialFlexibility3x3(void) const
  { return getSectionFlexibility3x3(); }


int XC::CrossSectionProperties2d::setParameter(const std::vector<std::string> &argv,Parameter &param,SectionForceDeformation *scc)
  {
    if(argv.size() < 1)
      return -1;

    if(argv[0] == "I")
      {
        param.setValue(I());
        return param.addObject(3,scc);
      }
    else if(argv[0] == "G")
      {
        param.setValue(G());
        return param.addObject(4,scc);
      }
    else if((argv[0] == "alpha") || (argv[0] == "alpha_y"))
      {
        param.setValue(Alpha());
        return param.addObject(5,scc);
      }
    else 
      return CrossSectionProperties1d::setParameter(argv,param,scc);
    return -1;
  }

int XC::CrossSectionProperties2d::updateParameter(int parameterID, Information &info)
  {
     switch(parameterID)
      {
      case 3:
        i= info.theDouble;
        return 0;
      case 4:
        g= info.theDouble;
        return 0;
      case 5:
        alpha= info.theDouble;
        return 0;
      default:
        return CrossSectionProperties1d::updateParameter(parameterID,info);
      }
    return 0;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::CrossSectionProperties2d::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::CrossSectionProperties2d::sendData(Communicator &comm)
  {
    int res= CrossSectionProperties1d::sendData(comm);
    res+= comm.sendDoubles(g,i,alpha,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::CrossSectionProperties2d::recvData(const Communicator &comm)
  {
    int res= CrossSectionProperties1d::recvData(comm); 
    res+= comm.receiveDoubles(g,i,alpha,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::CrossSectionProperties2d::getPyDict(void) const
  {
    boost::python::dict retval= CrossSectionProperties1d::getPyDict();
    retval["g"]= this->g;
    retval["i"]= this->i;
    retval["alpha"]= this->alpha;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::CrossSectionProperties2d::setPyDict(const boost::python::dict &d)
  {
    CrossSectionProperties1d::setPyDict(d);
    this->g= boost::python::extract<double>(d["g"]);
    this->i= boost::python::extract<double>(d["i"]);
    this->alpha= boost::python::extract<double>(d["alpha"]);
  }

//! @brief Sends object through the communicator argument.
int XC::CrossSectionProperties2d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::CrossSectionProperties2d::recvSelf(const Communicator &comm)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }


const XC::Matrix& XC::CrossSectionProperties2d::getInitialTangentSensitivity3x3(int gradIndex)
  {
    ks3.Zero();
    return ks3;
  }

//! @brief Print stuff.
void XC::CrossSectionProperties2d::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      {}
    else
      {
        s << "CrossSectionProperties2d, E: " << E() << std::endl;
        s << "\t G: " << G() << std::endl;
        s << "\t A: " << A() << std::endl;
        s << "\tI: " << I();
      }
  }

//! @brief Insertion on an output stream.
std::ostream &XC::operator<<(std::ostream &os,const XC::CrossSectionProperties2d &c)
  {
    c.Print(os);
    return os;
  }
