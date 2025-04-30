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
//PathSeriesBase.h

#ifndef PathSeriesBase_h
#define PathSeriesBase_h

#include <domain/load/pattern/time_series/CFactorSeries.h>
#include "utility/matrix/Vector.h"

namespace XC {
class Vector;

//! @ingroup TSeries
//
//! @brief Base class for time-dependent functions defined
//! by a series of points (ti,fi).
class PathSeriesBase: public CFactorSeries
  {
  protected:
    Vector thePath; //!< vector containing the data points
    bool useLast;

    static size_t load_vector_from_file(Vector &v,std::istream &is);

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    // constructors
    PathSeriesBase(int classTag,const double &cf= 1.0, bool last= false);
    PathSeriesBase(int classTag,const Vector &thePath,const double &cf= 1.0, bool last= false);

    size_t getNumDataPoints(void) const;
    size_t getNumDataPointsOnFile(const std::string &) const;
    double getPeakFactor(void) const;

    inline Vector getPath(void) const
      { return thePath; }
    inline void setPath(const Vector &d)
      { thePath= d; }
    inline void setUseLast(const bool &b)
      { this->useLast= b; }
    inline bool getUseLast(void) const
      {return this->useLast;}
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);

    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

