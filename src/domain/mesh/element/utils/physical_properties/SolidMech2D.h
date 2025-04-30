// -*-c++-*-
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
//SolidMech2D.h
                                                                        
#include "NDMaterialPhysicalProperties.h"

#ifndef SolidMech2D_h
#define SolidMech2D_h

namespace XC {
//! @ingroup PysicalProperties
//
//! @brief Physical properties for solid mechanics.
class SolidMech2D: public NDMaterialPhysicalProperties
  {
  protected:
    double thickness; //!< Element thickness

    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    SolidMech2D(const size_t &nMat= 0, const NDMaterial *ptr_mat= nullptr, const double &t=0.0);
    SolidMech2D(const size_t &, NDMaterial &,const std::string &, const double &t=0.0);

    virtual int getComponentIndexFromCode(const std::string &) const;

    //! @brief Return the material thickness.
    inline double getThickness(void) const
      { return thickness; }
    //! @brief Set the material thickness.
    inline void setThickness(const double &t)
      { thickness= t; }
    
    double getMeanInternalForce(const std::string &) const;
    double getMeanInternalDeformation(const std::string &) const;
    
    //! @brief Return density per unit area
    double getArealRho(void) const
      { return NDMaterialPhysicalProperties::getRho()*thickness; }
    //! @brief Assign density per unit area
    void setArealRho(const double &r)
      { NDMaterialPhysicalProperties::setRho(r/thickness); }
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };

} // end of XC namespace
#endif
