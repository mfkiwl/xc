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
//elem_load.cc

#include "elem_load.h"
#include "utility/matrix/Vector.h"
#include "domain/load/beam_loads/Beam2dPointLoad.h"
#include "domain/load/beam_loads/Beam2dUniformLoad.h"
#include "domain/load/beam_loads/Beam2dPartialUniformLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include "domain/load/beam_loads/Beam3dPointLoad.h"
#include "domain/load/beam_loads/Beam3dUniformLoad.h"
#include "domain/load/beam_loads/TrussStrainLoad.h"
#include "domain/load/volumetric/BrickRawLoad.h"
#include "domain/load/volumetric/BrickSelfWeight.h"
#include "domain/load/volumetric/BrickStrainLoad.h"
#include "domain/load/plane/ShellRawLoad.h"
#include "domain/load/plane/ShellUniformLoad.h"
#include "domain/load/plane/BidimStrainLoad.h"
#include "domain/load/plane/QuadStrainLoad.h"
#include "domain/load/plane/QuadRawLoad.h"
#include "domain/load/plane/ShellStrainLoad.h"
#include "domain/load/SurfaceLoad.h"

//! @brief Creates a new load over elements.
//! 
//! @param lp: load patter to append the load into.
//! @param tag_el: identifier of the element load.
//! @param loadType: load type
XC::ElementalLoad *XC::process_element_load(XC::LoadPattern *lp,int &tag_el,const std::string &loadType)
  {
    ElementalLoad *retval= nullptr;
    if(loadType == "beam2d_uniform_load")
      retval= new_elem_load<Beam2dUniformLoad>(lp,tag_el);
    else if(loadType == "beam2d_partial_uniform_load")
      retval= new_elem_load<Beam2dPartialUniformLoad>(lp,tag_el);
    else if(loadType == "beam2d_point_load")
      retval= new_elem_load<Beam2dPointLoad>(lp,tag_el);
    else if(loadType == "beam_strain_load")
      retval= new_elem_load<BeamStrainLoad>(lp,tag_el);
    else if(loadType == "beam3d_point_load")
      retval= new_elem_load<Beam3dPointLoad>(lp,tag_el);
    else if(loadType == "beam3d_uniform_load")
      retval= new_elem_load<Beam3dUniformLoad>(lp,tag_el);
    else if(loadType == "brick_self_weight")
      retval= new_elem_load<BrickSelfWeight>(lp,tag_el);
    else if(loadType == "shell_raw_load")
      retval= new_elem_load<ShellRawLoad>(lp,tag_el);
    else if(loadType == "quad_raw_load")
      retval= new_elem_load<QuadRawLoad>(lp,tag_el);
    else if(loadType == "brick_raw_load")
      retval= new_elem_load<BrickRawLoad>(lp,tag_el);
    else if(loadType == "shell_uniform_load")
      retval= new_elem_load<ShellUniformLoad>(lp,tag_el);
    else if(loadType == "bidim_strain_load")
      retval= new_elem_load<BidimStrainLoad>(lp,tag_el);
    else if(loadType == "quad_strain_load")
      retval= new_elem_load<QuadStrainLoad>(lp,tag_el);
    else if(loadType == "shell_strain_load")
      retval= new_elem_load<ShellStrainLoad>(lp,tag_el);
    else if(loadType == "brick_strain_load")
      retval= new_elem_load<BrickStrainLoad>(lp,tag_el);
    else if(loadType == "truss_temp_load")
      {
	std::cerr << __FUNCTION__ << "; load type: '"
	          << loadType << "' deprecated."
		  << " Use truss_strain_load." << std::endl;
        retval= new_elem_load<TrussStrainLoad>(lp,tag_el);
      }
    else if(loadType == "truss_strain_load")
      retval= new_elem_load<TrussStrainLoad>(lp, tag_el);
    else if(loadType == "surface_load")
      retval= new_elem_load<SurfaceLoad>(lp, tag_el);
    else
      std::cerr << __FUNCTION__ << "; load type: '"
	        << loadType << "' unknown." << std::endl;
    return retval;
  }
