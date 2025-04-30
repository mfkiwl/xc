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
//PathSeriesBase.cpp


#include "PathSeriesBase.h"
#include <utility/matrix/Vector.h>

#include <fstream>
#include "utility/actor/actor/CommMetaData.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::PathSeriesBase::PathSeriesBase(int classTag, const double &theFactor, bool last)	
  : CFactorSeries(classTag,theFactor), useLast(last) {}

		   
//! @brief Constructor.
XC::PathSeriesBase::PathSeriesBase(int classTag,const Vector &theLoadPath,const double &theFactor, bool last)
  :CFactorSeries(classTag,theFactor),
   thePath(theLoadPath), useLast(last)
  {}

//! @brief Return the number of points that define the path.
size_t XC::PathSeriesBase::getNumDataPoints(void) const
  { return thePath.Size(); }

//! @brief Return the number of data en on the file with the name being passed as parameter.
size_t XC::PathSeriesBase::getNumDataPointsOnFile(const std::string &fName) const
  {
    double dataPoint;
    std::ifstream theFile;
    size_t retval= 0;
    // first open and go through file containing path
    theFile.open(fName.c_str(), std::ios::in);
    if(theFile.bad() || !theFile.is_open())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - could not open file "
		  << fName << std::endl;
      }
    else
      {
        while(theFile >> dataPoint)
          retval++;
      }
    theFile.close();
    return retval;
  }

//! @brief Read load vector from file
size_t XC::PathSeriesBase::load_vector_from_file(Vector &v,std::istream &is)
  {
    size_t count = 0;
    double dataPoint;
    while(is >> dataPoint)
      {
	v(count) = dataPoint;
	count++;
      }
    return count;
  }

//! @brief Return the peak value of the factor.
double XC::PathSeriesBase::getPeakFactor(void) const
  {
    double retval= 0.0;
    const int num = thePath.Size();
    if(num>0)
      {
	double peak= fabs(thePath(0));
	double temp= 0.0;
	for(int i=1;i<num;i++)
	  {
	    temp= fabs(thePath(i));
	    if(temp > peak)
	      peak= temp;
	  }
	retval= peak*cFactor;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; WARNING: empty data path." << std::endl;
    return retval;
  }

//! @brief Printing stuff.
void XC::PathSeriesBase::Print(std::ostream &s, int flag) const
  {
    if(flag == 1)
      s << " specified path: " << thePath;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::PathSeriesBase::getPyDict(void) const
  {
    boost::python::dict retval= CFactorSeries::getPyDict();
    retval["path"]= this->thePath.getPyList();
    retval["use_last"]= this->useLast;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::PathSeriesBase::setPyDict(const boost::python::dict &d)
  {
    CFactorSeries::setPyDict(d);
    this->thePath= Vector(boost::python::extract<boost::python::list>(d["path"]));
    this->useLast= boost::python::extract<bool>(d["use_last"]);
  }

//! @brief Send members through the communicator argument.
int XC::PathSeriesBase::sendData(Communicator &comm)
  {
    int res= sendData(comm);
    res+= comm.sendVector(thePath,getDbTagData(),CommMetaData(1));
    res+= comm.sendBool(useLast,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::PathSeriesBase::recvData(const Communicator &comm)
  {
    int res= recvData(comm);
    res+= comm.receiveVector(thePath,getDbTagData(),CommMetaData(1));
    res+= comm.receiveBool(useLast,getDbTagData(),CommMetaData(2));
    return res;
  }

