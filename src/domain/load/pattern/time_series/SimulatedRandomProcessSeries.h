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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1 $
// $Date: 2003/03/04 00:49:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/SimulatedRandomProcessSeries.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu), February 2002
// Revised: 
//

#ifndef SimulatedRandomProcessSeries_h
#define SimulatedRandomProcessSeries_h

#include <domain/load/pattern/TimeSeries.h>
#include "utility/matrix/Vector.h"

namespace XC {
class Spectrum;
class RandomNumberGenerator;
class Vector;

//! @ingroup TSeries
//
//! @brief Time series obtained from a simulated random process.
//!
//! In probability theory and related fields, a stochastic or random
//! process is a mathematical object usually defined as a family of random
//! variables. Historically, the random variables were associated with or
//! indexed by a set of numbers, usually viewed as points in time, giving the
//! interpretation of a stochastic process representing numerical values of
//! some system randomly changing over time, such as the growth of a bacterial
//! population, an electrical current fluctuating due to thermal noise, or
//! the movement of a gas molecule.
class SimulatedRandomProcessSeries: public TimeSeries
  {
  private:
    RandomNumberGenerator *theRandomNumberGenerator;
    Spectrum *theSpectrum;
    int numFreqIntervals;
    double mean;
    double deltaW;
    Vector theta;
    Vector A;
  protected:

  public:
    SimulatedRandomProcessSeries(RandomNumberGenerator *theRandNumGenerator, Spectrum *theSpectrum, int numFreqIntervals, double mean);

    // method to get load factor
    double getFactor(double pseudoTime) const;

    // None of the following functions should be invoked on this type
    // of object
    double getDuration(void) const {return 0.0;} // dummy function
    double getPeakFactor(void) const {return 0.0;} // dummy function
    double getTimeIncr(double pseudoTime) const {return 1.0;} // dummy function
    
    // methods for output    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
