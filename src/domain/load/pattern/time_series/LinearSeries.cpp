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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/LinearSeries.cpp,v $
                                                                        
                                                                        
// File: ~/domain/load/pattern/LinearSeries.C
//
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class definition for XC::LinearSeries.
// LinearSeries is a concrete class. A XC::LinearSeries object provides
// a linear time series. the factor is given by the pseudoTime and 
// a constant factor provided in the constructor.
//
// What: "@(#) LinearSeries.C, revA"


#include <domain/load/pattern/time_series/LinearSeries.h>
#include <utility/matrix/Vector.h>


//! @brief Constructor.
//!
//! @param theFactor: factor used in the relation.
XC::LinearSeries::LinearSeries(const double &theFactor, const double &startTime)
  :CFactorSeries(TSERIES_TAG_LinearSeries,theFactor), tStart(startTime)
  {}

//! @brief Returns the load factor at the specified time
//! (product of \p cFactor and \p pseudoTime).
double XC::LinearSeries::getFactor(double pseudoTime) const
  {
    double retval= 0.0;
    if(pseudoTime >= tStart)
      retval= cFactor*(pseudoTime-tStart);
    return retval;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::LinearSeries::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::LinearSeries::getPyDict(void) const
  {
    boost::python::dict retval= CFactorSeries::getPyDict();
    retval["t_start"]= this->tStart;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::LinearSeries::setPyDict(const boost::python::dict &d)
  {
    CFactorSeries::setPyDict(d);
    this->tStart= boost::python::extract<double>(d["t_start"]);
  }

//! @brief Send object members through the communicator argument.
int XC::LinearSeries::sendData(Communicator &comm)
  {
    int retval= CFactorSeries::sendData(comm);
    retval+= comm.sendDouble(tStart,getDbTagData(),CommMetaData(1));
    return retval;
  }

//! @brief Receives object members through the communicator argument.
int XC::LinearSeries::recvData(const Communicator &comm)
  {
    int retval= CFactorSeries::recvData(comm);
    retval+= comm.receiveDouble(tStart,getDbTagData(),CommMetaData(1));
    return retval;
  }

//! @brief Sends object through the communicator argument.
int XC::LinearSeries::sendSelf(Communicator &comm)
  {
    inicComm(2);
    int result= sendData(comm);

    const int dataTag= getDbTag(comm);
    result+= comm.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ch failed to send data.\n";
    return result;
  }

//! @brief Receives object through the communicator argument.
int XC::LinearSeries::recvSelf(const Communicator &comm)
  {
    inicComm(2);

    const int dataTag = this->getDbTag();  
    int result = comm.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ch failed to receive data.\n";
    else
      result+= recvData(comm);
    return result;    
  }

//! @brief Print stuff.
void XC::LinearSeries::Print(std::ostream &s, int flag) const
  {
    s << getClassName() << ": linear factor: "
      << cFactor << " start time: " << tStart << "\n";
  }
