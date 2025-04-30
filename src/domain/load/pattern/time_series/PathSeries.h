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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/25 23:32:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/PathSeries.h,v $
                                                                        
                                                                        
#ifndef PathSeries_h
#define PathSeries_h

// File: ~/analysis/integrator/PathSeries.h
// 
// Written: fmk 
// Created: 07/99
// Revision: A
//
//
// What: "@(#) PathSeries.h, revA"

#include "PathSeriesBase.h"
#include <string>

namespace XC {
class Vector;

//! @ingroup TSeries
//
//! @brief Time-dependent function that linear interpolates the
//! load factor using user specified control points provided in a vector object.
//! the points in the vector are given at regular time increments pathTimeIncr
//! apart.
//!
//! The relationship between the pseudo time and the load factor follows
//! a user specified path. The path points are specified at constant time
//! intervals. For a pseudo time not at a path point, linear interpolation
//! is performed to determine the load factor. If the time specified is
//! beyond the last path point a load factor of \f$0.0\f$ will be returned.
//! Specify useLast= true to use the last data point instead of 0.0.
class PathSeries: public PathSeriesBase
  {
  private:
    double pathTimeIncr; //!< Time step.
    double startTime; //!< provide a start time for provided load factors.
    bool prependZero; //!< if true prepend a zero value to the series of load factors.
    void prepend_zero_if_appropriate(void);
  protected:
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

  public:
    // constructors
    PathSeries(void);
    PathSeries(const Vector &thePath,double pathTimeIncr = 1.0, double cf= 1.0, bool useLast = false, bool prependZero = false, double startTime = 0.0);
    PathSeries(const std::string &fileName, double pathTimeIncr = 1.0, double cf= 1.0, bool useLast = false, bool prependZero = false, double startTime = 0.0);
    
    //! @brief Virtual constructor.
    TimeSeries *getCopy(void) const
      { return new PathSeries(*this); }

    // method to get factor
    double getFactor(double pseudoTime) const;
    double getDuration(void) const;
    inline void setTimeIncr(const double &d)
      { pathTimeIncr= d; }
    inline double getTimeIncr(double) const
      {return pathTimeIncr;}
    inline double getTimeIncr(void) const
      {return pathTimeIncr;}
    inline void setStartTime(const double &d)
      { this->startTime= d; }
    inline double getStartTime(void) const
      {return this->startTime;}
    inline void setPrependZero(const bool &b)
      { this->prependZero= b; }
    inline bool getPrependZero(void) const
      {return this->prependZero;}

    void readFromFile(const std::string &fileName);
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);


    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

