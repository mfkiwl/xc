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
//DeformationPlane.cc

#include "DeformationPlane.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/d2/GeneralEquationOfPlane.h"
#include "utility/matrix/Vector.h"
#include "material/ResponseId.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/HalfPlane2d.h"
#include "utility/actor/actor/MovableVector.h"

bool XC::DeformationPlane::check_positions(const Pos3d &pA,const Pos3d &pB, const Pos3d &pC)
  {
    const Pos2d p1= Pos2d(pA.y(),pA.z());
    const Pos2d p2= Pos2d(pB.y(),pB.z());
    const Pos2d p3= Pos2d(pC.y(),pC.z());
    return check_positions(p1,p2,p3);
  }

bool XC::DeformationPlane::check_positions(const Pos2d &p1,const Pos2d &p2, const Pos2d &p3)
  {
    bool retval= true;
    const GEOM_FT tol2= 1e-3;
    const GEOM_FT d12= p1.dist2(p2);
    if(d12<tol2)
      {
        retval= false;
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; points p1= " << p1 << " and p2= " 
                  << p2 << " are too close d= " << sqrt(d12) << std::endl;
      }
    const GEOM_FT d13= p1.dist2(p3);
    if(d13<tol2)
      {
        retval= false;
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; points p1= " << p1 << " and p3= " 
                  << p3 << " are too close d= " << sqrt(d13) << std::endl;
      }
    const GEOM_FT d23= p2.dist2(p3);
    if(d23<tol2)
      {
        retval= false;
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; points p2= " << p2 << " and p3= " 
                  << p3 << " are too close d= " << sqrt(d23) << std::endl;
      }
    return retval;
  }

XC::DeformationPlane::DeformationPlane(const Pos3d &p1,const Pos3d &p2, const Pos3d &p3)
  : Plane(p1,p2,p3), MovableObject(0) { check_positions(p1,p2,p3); }

XC::DeformationPlane::DeformationPlane(const Plane &p)
  : Plane(p), MovableObject(0) {}


//! @brief Strains at three fibers.
XC::DeformationPlane::DeformationPlane( const Pos2d &yz1, const double &e_1, 
                  const Pos2d &yz2, const double &e_2,
                  const Pos2d &yz3, const double &e_3)
  : Plane(), MovableObject(0)
  {
    const Pos3d p1(e_1,yz1.x(),yz1.y());
    const Pos3d p2(e_2,yz2.x(),yz2.y());
    const Pos3d p3(e_3,yz3.x(),yz3.y());
    check_positions(p1,p2,p3);
    ThreePoints(p1,p2,p3);
  }

//! Constant strain over the section.
XC::DeformationPlane::DeformationPlane(const double &eps) 
  :Plane(Pos3d(eps,100,0),Pos3d(eps,0,100),Pos3d(eps,100,100)), MovableObject(0) {}

XC::DeformationPlane::DeformationPlane(const Vector &e)
  : Plane(), MovableObject(0)
  { setDeformation(e); }

void XC::DeformationPlane::ConstantStrain(const double &e)
  {
    const Pos3d p1(e,0.0,0.0);
    const Pos3d p2(e,100.0,0.0);
    const Pos3d p3(e,0.0,100.0);
    ThreePoints(p1,p2,p3);
  }

//! @brief Return the strain of the fiber at the position
//! being passed as parameter.
double XC::DeformationPlane::Strain(const Pos2d &p) const
  { return Plane::x(p); }

//! @brief Returns the generalized strains vector (epsilon, zCurvature, yCurvature).
const XC::Vector &XC::DeformationPlane::getDeformation(void) const
  {
    static Vector retval(3);
    retval(0)= Strain(Pos2d(0,0)); // SECTION_RESPONSE_P
    retval(1)= Strain(Pos2d(1,0))-retval(0); // SECTION_RESPONSE_MZ
    retval(2)= Strain(Pos2d(0,1))-retval(0); // SECTION_RESPONSE_MY
    return retval;
  }

//! @brief Set the generalized strains vector.
//! @param genStrains: generalized strains vector (epsilon, zCurvature, yCurvature).
void XC::DeformationPlane::setDeformation(const Vector &genStrains)
  {
    const double e_1= genStrains(0); //strain at (0,0)
    const double e_2= genStrains(0)+genStrains(1); //Strain at (1,0)
    double e_3= genStrains(0); //Strain at (0,1)
    if(genStrains.Size()>2)
      e_3+= genStrains(2); //Strain at (0,1)
    
    const Pos3d p1(e_1,0.0,0.0);
    const Pos3d p2(e_2,1.0,0.0);
    const Pos3d p3(e_3,0.0,1.0);
    ThreePoints(p1,p2,p3);
  }

//! @brief Returns the generalized strains vector.
//! @param order: dimension of the problem (1, 2 or 3).
//! @param rId: stiffness material contribution response identifiers.
const XC::Vector &XC::DeformationPlane::getDeformation(const size_t &order, const ResponseId &rId) const
  {
    static Vector retval;
    retval.resize(order);
    retval.Zero();
    const Vector &tmp= getDeformation();
    for( size_t i= 0;i<order;i++)
      {
        if(rId(i) == SECTION_RESPONSE_P)
          retval[i]+= tmp(0);
        if(rId(i) == SECTION_RESPONSE_MZ)
          retval[i]+= tmp(1);
        if(rId(i) == SECTION_RESPONSE_MY)
          retval[i]+= tmp(2);
      }
    return retval;
  }

boost::python::list XC::DeformationPlane::getGeneralizedStrainsPy(const boost::python::list &l) const
  {
    const size_t order= boost::python::len(l);
    const ResponseId rId(l);
    const Vector &def= this->getDeformation(order, rId);
    const size_t sz= def.Size();
    boost::python::list retval;
    for( size_t i= 0;i<sz;i++)
      { retval.append(def[i]); }
    return retval;
  }

//! @brief Return the neutral axis.
Line2d XC::DeformationPlane::getNeutralAxis(void)const
  {
    const double a= angle(*this,YZPlane3d);
    Line2d retval;
    if(a>1e-4)
      {
        Line3d trace= YZTrace();
        if(trace.exists())
          retval= trace.YZ2DProjection();
      }
    else //Almost parallel: can't find intersection.
      retval.setExists(false);
    return retval;
  }


//! @brief Returns (if possible) a point in the tensioned side of the
//! cross-section.
Pos2d XC::DeformationPlane::getPointOnTensionedHalfPlane(void) const
  {
    Pos2d retval(0,0);
    const Line2d fn(getNeutralAxis());
    bool exists= fn.exists();
    const double a= angle(*this,YZPlane3d);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Point());
        //const Vector2d v(getMaximumSlopeLineYZ().YZ2DProjection().VDir());
        const Vector2d v= fn.VDir().Normal();
        retval= p0+1000*v;
        if(Strain(retval)<0) //Compressed zone.
          retval= p0-1000*v;
      }
    else //Degenerated neutral axis.
      retval.setExists(Strain(retval)>0.0);
    return retval;
  }

//! @brief Returns (if possible) a point in the compressed zone.
Pos2d XC::DeformationPlane::getPointOnCompressedHalfPlane(void) const
  {
    Pos2d retval(0,0);
    const Line2d fn(getNeutralAxis());
    bool exists= fn.exists();
    const double a= angle(*this,YZPlane3d);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Point());
        //const Vector2d v(getMaximumSlopeLineYZ().YZ2DProjection().VDir());
        const Vector2d v= fn.VDir().Normal();
        retval= p0+1000*v;
        if(Strain(retval)>0) //Tensioned side.
          retval= p0-1000*v;
      }
    else //Degenerated neutral axis.
      retval.setExists(Strain(retval)<0.0);
    return retval;
  }

//! @brief Returns the half plane which border is the line being passed
//! as parameter and is contained in the half plane in traction.
HalfPlane2d XC::DeformationPlane::getTensionedHalfPlane(const Line2d &r) const
  {
    const HalfPlane2d spt= getTensionedHalfPlane();
    assert(spt.exists());
    const Pos2d p0(r.Point());
    const Vector2d v= r.VDir().Normal();
  
    Pos2d p(p0+1000*v);
    HalfPlane2d retval;
    if(spt.In(p))
      retval= HalfPlane2d(r,p);
    else
      {
        p= p0-1000*v;
        if(spt.In(p))
          retval= HalfPlane2d(r,p);
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << " couldn't find the tensioned half-plane"
                    << " with the border r= " << r << std::endl;
      }
    return retval;
  }

//! @brief Returns the tensioned half-plane.
HalfPlane2d XC::DeformationPlane::getTensionedHalfPlane(void) const
  {
    const Line2d fn= getNeutralAxis();
    bool exists= fn.exists();
    const double a= angle(*this,YZPlane3d);
    Pos2d tmp(0,0);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Point());
        const Vector2d v= fn.VDir().Normal();
        tmp= p0+1000*v;
        if(Strain(tmp)<0) //Compressed zone.
          tmp= p0-1000*v;
      }
    else //Degenerated neutral axis.
      exists= (Strain(tmp)>0.0);

    HalfPlane2d retval;
    if(exists)
      retval= HalfPlane2d(fn,tmp);
    return retval;
  }

//! @brief Returns the compressed half-plane.
HalfPlane2d XC::DeformationPlane::getCompressedHalfPlane(const Line2d &r) const
  {
    const Line2d fn= getNeutralAxis();
    bool exists= fn.exists();
    const double a= angle(*this,YZPlane3d);
    Pos2d tmp(0,0);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Point());
        const Vector2d v= fn.VDir().Normal();
        tmp= p0+1000*v;
        if(Strain(tmp)>0) //Tensioned side.
          tmp= p0-1000*v;
      }
    else //Degenerated neutral axis.
      exists= (Strain(tmp)<0.0);

    HalfPlane2d retval;
    if(exists)
      retval= HalfPlane2d(fn,tmp);
    return retval;
  }

//! @brief Returns the compressed half plane.
HalfPlane2d XC::DeformationPlane::getCompressedHalfPlane(void) const
  {
    const Line2d fn= getNeutralAxis();
    bool exists= fn.exists();
    HalfPlane2d retval;
    if(exists)
      retval= getCompressedHalfPlane(fn);
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::DeformationPlane::sendData(Communicator &comm)
  {
    //setDbTagDataPos(0,getTag());
    Vector ec_gen(4);
    GeneralEquationOfPlane ec= getGeneralEquation();
    ec_gen(0)= ec.a(); ec_gen(1)= ec.b(); ec_gen(2)= ec.c(); ec_gen(3)= ec.d(); 
    int res= comm.sendVector(ec_gen,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::DeformationPlane::recvData(const Communicator &comm)
  {
    //setTag(getDbTagDataPos(0));
    Vector ec_gen(4);
    int res= comm.receiveVector(ec_gen,getDbTagData(),CommMetaData(1));
    GeneralEquationOfPlane ec(ec_gen(0),ec_gen(1),ec_gen(2),ec_gen(3));
    GeneralEquation(ec);
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::DeformationPlane::sendSelf(Communicator &comm)
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
int XC::DeformationPlane::recvSelf(const Communicator &comm)
  {
    inicComm(3);
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
