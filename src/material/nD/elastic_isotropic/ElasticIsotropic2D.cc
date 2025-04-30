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
                                                                        
// $Revision: 1.5 $
// $Date: 2002/12/05 22:49:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/elastic_isotropic/ElasticIsotropic2D.cpp,v $
                                                                        
                                                                        
#include <material/nD/elastic_isotropic/ElasticIsotropic2D.h>                                                                        

#include <utility/matrix/Matrix.h>
#include "material/nD/NDMaterialType.h"
#include "material/ResponseId.h"

XC::Matrix XC::ElasticIsotropic2D::D(3,3);

//! @brief Constructor.
XC::ElasticIsotropic2D::ElasticIsotropic2D(int tag, int classTag, double E, double nu, double rho)
  : ElasticIsotropicMaterial(tag, classTag, 3, E, nu, rho)
  {}

//! @brief Constructor.
XC::ElasticIsotropic2D::ElasticIsotropic2D(int tag, int classTag)
  : ElasticIsotropicMaterial(tag, classTag,3, 0.0, 0.0)
  {}

int XC::ElasticIsotropic2D::getOrder(void) const
  { return 3; }

//! @brief Return the ordering and type of response quantities
//! returned by this material.
//!
//! Return the section ResponseId code that indicates
//! the ordering and type of response quantities returned
//! by the material. Lets the calling object
//! (e.g. an Element) know how to interpret the quantites returned by this
//! object.
const XC::ResponseId &XC::ElasticIsotropic2D::getResponseType(void) const
  { return RespSolidMechanics2D; }

