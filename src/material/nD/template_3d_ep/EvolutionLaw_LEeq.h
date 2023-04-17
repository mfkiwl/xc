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

//============================================================================
// COPYRIGHT (C):     :-))                                                   |
// PROJECT:           Object Oriented Finite Element Program                     |
// PURPOSE:           General platform for elaso-plastic constitutive model      |
//                    implementation                                             |
//                                                                               |
// CLASS:             EvolutionLaw_L_Eeq (linear Evolution law)                  |
//                                                                               |
//                                                                               |
// VERSION:                                                                      |
// LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )   |
// TARGET OS:         DOS || UNIX || . . .                                       |
// DESIGNER(S):       Boris Jeremic, Zhaohui Yang                                |
// PROGRAMMER(S):     Boris Jeremic, Zhaohui Yang                                |
//                                                                               |
//                                                                               |
// DATE:              09-02-2000                                                 |
// UPDATE HISTORY:                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
//                                                                               |
// SHORT EXPLANATION: This is a linear evolution law for the evoltion of a       |
//                    scalar variable k which depends on plastic equi. strain    |
//                    i.e. dk = a*de_eq_p                                        |
//                                                                               |
//================================================================================

#ifndef EvolutionLaw_LEeq_H
#define EvolutionLaw_LEeq_H

#include <cmath>
#include "material/nD/template_3d_ep/EvolutionLaw_S.h"

namespace XC {
//! @ingroup MatNDEL
//!
//! @brief Linear evolution law.
//! 
//! This is a linear evolution law for the evoltion of a scalar variable
//! \f$k\f$ which depends on plastic equi. strain i.e. \f$dk = a*de_{eq_p}\f$.
class EvolutionLaw_L_Eeq: public EvolutionLaw_S
  {
  private:
    double  a; //!< coefficient to define the linear hardening rule of a scalar hardening var

  public:
    EvolutionLaw_L_Eeq(const double &ad= 10.0);
    
    EvolutionLaw_S *getCopy(void) const; //create a clone of itself
    
    // some accessor functions
    double geta(void) const; // Linear coefficient used to evolve internal var
    void seta(const double &);

    double h_s( EPState *EPS, PotentialSurface *PS);    // Evaluating hardening function h
    
    void print();

    friend std::ostream& operator<<(std::ostream &,const EvolutionLaw_L_Eeq &);
  };
std::ostream& operator<<(std::ostream &,const EvolutionLaw_L_Eeq &);
} // end of XC namespace


#endif



