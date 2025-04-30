//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
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
//python_interface.tcc

class_<XC::TrussStrainLoad, bases<XC::ElementBodyLoad>, boost::noncopyable >("TrussStrainLoad", no_init)
  .add_property("eps1", make_function( &XC::TrussStrainLoad::E1, return_value_policy<return_by_value>() ),&XC::TrussStrainLoad::setE1, "return the strain load at back end.")
  .add_property("eps2", make_function( &XC::TrussStrainLoad::E2, return_value_policy<return_by_value>() ),&XC::TrussStrainLoad::setE2, "return the strain load at front end.")
  .def("getStrains",&XC::TrussStrainLoad::getStrains, "return the strain loads at both ends.")
  .def("getElementStrainsMatrix", &XC::TrussStrainLoad::getElementStrainsMatrix, "Return a matrix containing the axial strains at both ends as rows.") 
  ;

class_<XC::BeamLoad, bases<XC::ElementBodyLoad>, boost::noncopyable >("BeamLoad", no_init)
  .add_property("category", &XC::BeamLoad::Category)
  .def("getElementResponseId", &XC::BeamLoad::getElementResponseId, "Return the response id of the given element.") 
  ;

class_<XC::BeamStrainLoad, bases<XC::BeamLoad>, boost::noncopyable >("BeamStrainLoad", no_init)
  .add_property("backEndDeformationPlane", make_function( &XC::BeamStrainLoad::getDeformationPlane1, return_internal_reference<>() ),&XC::BeamStrainLoad::setDeformationPlane1,"Back end deformation plane.")
  .add_property("frontEndDeformationPlane", make_function( &XC::BeamStrainLoad::getDeformationPlane2, return_internal_reference<>() ),&XC::BeamStrainLoad::setDeformationPlane2,"Front end deformation plane.")
  .def("getElementStrainsMatrix", &XC::BeamStrainLoad::getElementStrainsMatrix, "Return a matrix containing the axial strains at element both ends as rows.") 
  ;

class_<XC::BeamMecLoad, bases<XC::BeamLoad>, boost::noncopyable >("BeamMecLoad", no_init)
  .add_property("axialComponent", &XC::BeamMecLoad::getAxialComponent, &XC::BeamMecLoad::setAxialComponent)
  .add_property("transComponent", &XC::BeamMecLoad::getTransComponent, &XC::BeamMecLoad::setTransComponent)
  .add_property("transYComponent", &XC::BeamMecLoad::getTransComponent, &XC::BeamMecLoad::setTransComponent)
  .def("getLocalForce",&XC::BeamMecLoad::getLocalForce,"Returns local coordinates of the force vector.")
  .def("getLocalMoment",&XC::BeamMecLoad::getLocalMoment,"Returns local coordinates of bending moment vector.")
  .def("getVector3dLocalForce",&XC::BeamMecLoad::getVector3dLocalForce,"Returns a Vector3d with the local coordinates of the force vector.")
  .def("getVector3dLocalMoment",&XC::BeamMecLoad::getVector3dLocalMoment,"Returns a Vector3d with the local coordinates of bending moment vector.")
  .def("getLocalForces",make_function(&XC::BeamMecLoad::getLocalForces, return_internal_reference<>() ),"Returns punctual/distributed force vectors (one for each element) expressed in element local coordinates.")
  .def("getLocalMoments",make_function(&XC::BeamMecLoad::getLocalMoments, return_internal_reference<>() ),"Returns punctual/distributed force moments (one for each element) expressed in element local coordinates." )
  .def("getGlobalVectors",make_function(&XC::BeamMecLoad::getGlobalVectors, return_internal_reference<>() ))
  .def("getGlobalForces",make_function(&XC::BeamMecLoad::getGlobalForces, return_internal_reference<>() ),"Returns punctual/distributed force vectors (one for each element) expressed in global coordinates.")
  .def("getGlobalMoments",make_function(&XC::BeamMecLoad::getGlobalMoments, return_internal_reference<>() ),"Returns punctual/distributed moment vectors (one for each element) expressed in global coordinates.")
  ;

class_<XC::BeamPointLoad, bases<XC::BeamMecLoad>, boost::noncopyable >("BeamPointLoad", no_init)
  .add_property("x", &XC::BeamPointLoad::X, &XC::BeamPointLoad::setX)
  ;

class_<XC::BeamUniformLoad, bases<XC::BeamMecLoad>, boost::noncopyable >("BeamUniformLoad", no_init)
  .def("getDistributedLocalForces",make_function(&XC::BeamUniformLoad::getDistributedLocalForces, return_internal_reference<>() ),"Returns distributed force vectors (one for each element) expressed in element local coordinates.")
  .def("getDistributedLocalMoments",make_function(&XC::BeamUniformLoad::getDistributedLocalMoments, return_internal_reference<>() ),"Returns distributed moment vectors (one for each element) expressed in element local coordinates.")
  .def("getDistributedGlobalForces",make_function(&XC::BeamUniformLoad::getDistributedGlobalForces, return_internal_reference<>() ),"Returns distributed force vectors (one for each element) expressed in global coordinates.")
  .def("getDistributedGlobalMoments",make_function(&XC::BeamUniformLoad::getDistributedGlobalMoments, return_internal_reference<>() ),"Returns distributed moment vectors (one for each element) expressed in global coordinates.")
  .def("getResultant",&XC::BeamUniformLoad::getResultant,"Returns the load resultant (load integrated over the elements).")
  ;

class_<XC::Beam2dPointLoad, bases<XC::BeamPointLoad>, boost::noncopyable >("Beam2dPointLoad", no_init)
  ;

class_<XC::Beam2dUniformLoad, bases<XC::BeamUniformLoad>, boost::noncopyable >("Beam2dUniformLoad", no_init)
  ;

class_<XC::Beam2dPartialUniformLoad, bases<XC::BeamUniformLoad>, boost::noncopyable >("Beam2dPartialUniformLoad", no_init)
  ;


class_<XC::Beam3dPointLoad, bases<XC::BeamPointLoad>, boost::noncopyable >("Beam3dPointLoad", no_init)
  .add_property("transYComponent", &XC::Beam3dPointLoad::getTransComponent, &XC::Beam3dPointLoad::setTransComponent)
  .add_property("transZComponent", &XC::Beam3dPointLoad::getTransZComponent, &XC::Beam3dPointLoad::setTransZComponent)
  ;

class_<XC::Beam3dUniformLoad, bases<XC::BeamUniformLoad>, boost::noncopyable >("Beam3dUniformLoad", no_init)
  .add_property("transZComponent", &XC::Beam3dUniformLoad::getTransZComponent, &XC::Beam3dUniformLoad::setTransZComponent)
  ;


