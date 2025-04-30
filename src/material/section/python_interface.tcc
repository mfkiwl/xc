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

const XC::Vector &(XC::SectionForceDeformation::*getStressResultantVector)(void) const= &XC::SectionForceDeformation::getStressResultant;
const XC::Vector &(XC::SectionForceDeformation::*getSectionDeformationConstVector)(void) const= &XC::SectionForceDeformation::getSectionDeformation;
class_<XC::SectionForceDeformation, XC::SectionForceDeformation *, bases<XC::Material>, boost::noncopyable >("SectionForceDeformation", no_init)
  .add_property("rho",&XC::SectionForceDeformation::getRho,"Return the material density.")
  .add_property("linearRho",&XC::SectionForceDeformation::getLinearRho,"Return the material linear density (mass/length).")
  .add_property("arealRho",&XC::SectionForceDeformation::getArealRho,"Return the material areal density (mass/surf. area).")
  .def("getStressResultantComponent",&XC::SectionForceDeformation::getStressResultantByName)
  .def("getStressResultant",make_function(getStressResultantVector, return_internal_reference<>()))
  .def("getSectionDeformationByName",&XC::SectionForceDeformation::getSectionDeformationByName)
  .def("getSectionDeformation",make_function( getSectionDeformationConstVector, return_internal_reference<>() ),"returns section deformation vector (generalized strains).")
  .add_property("sectionDeformation", make_function( getSectionDeformationConstVector, return_internal_reference<>() ), &XC::SectionForceDeformation::setTrialSectionDeformation,"section deformation vector (generalized strains).")
  .add_property("initialSectionDeformation", make_function( &XC::SectionForceDeformation::getInitialSectionDeformation, return_internal_reference<>() ), &XC::SectionForceDeformation::setInitialSectionDeformation,"section initial deformation vector (generalized strains).")
  .def("getTangentStiffness", make_function(&XC::SectionForceDeformation::getSectionTangent, return_internal_reference<>()))
  .def("getInitialTangentStiffness", make_function(&XC::SectionForceDeformation::getInitialTangent, return_internal_reference<>()))
  .def("getFlexibility", make_function(&XC::SectionForceDeformation::getSectionFlexibility, return_internal_reference<>()))
  .def("getInitialFlexibility", make_function(&XC::SectionForceDeformation::getInitialFlexibility, return_internal_reference<>()))
  .def("getStrain",&XC::SectionForceDeformation::getStrain,"Returns strain at position being passed as parameter.")
   ;

class_<XC::Bidirectional, bases<XC::SectionForceDeformation>, boost::noncopyable >("Bidirectional", no_init);

//class_<XC::GenericSectionNd, bases<XC::SectionForceDeformation>, boost::noncopyable >("GenericSectionNd", no_init);

//class_<XC::Isolator2spring , bases<XC::SectionForceDeformation>, boost::noncopyable >("Isolator2spring", no_init);

//HalfPlane2d (XC::PrismaticBarCrossSection::*getTensionedHalfPlaneRecta)(const Line2d &,bool &)= &XC::PrismaticBarCrossSection::getTensionedHalfPlane;
//HalfPlane2d (XC::PrismaticBarCrossSection::*getTensionedHalfPlane)(bool &)= &XC::PrismaticBarCrossSection::getTensionedHalfPlane;
class_<XC::PrismaticBarCrossSection, XC::PrismaticBarCrossSection *, bases<XC::SectionForceDeformation>, boost::noncopyable >("PrismaticBarCrossSection", no_init)
  .def("getCenterOfMassY",&XC::PrismaticBarCrossSection::getCenterOfMassY)
  .def("getCenterOfMassZ",&XC::PrismaticBarCrossSection::getCenterOfMassZ)
  .def("isSubjectedToAxialLoad",&XC::PrismaticBarCrossSection::hayAxil,"True if section is in compression (or in tension).")
  .def("isSubjectedToBending",&XC::PrismaticBarCrossSection::isSubjectedToBending,"True if section is bended.")
  .def("isSubjectedToShear",&XC::PrismaticBarCrossSection::isSubjectedToShear,"True if section is subjected to shear.")
  .def("isSubjectedToTorsion",&XC::PrismaticBarCrossSection::hayTorsor,"True if section is subjected to torsion.")
  .def("getN",&XC::PrismaticBarCrossSection::getN,"Returns internal axial force.")
  .def("getMy",&XC::PrismaticBarCrossSection::getMy,"Returns internal bending moment around 'y' axis.")
  .def("getMz",&XC::PrismaticBarCrossSection::getMz,"Returns internal bending moment around 'z' axis.")
  .def("getCenterOfMass",&XC::PrismaticBarCrossSection::getCenterOfMass)
  .def("getInternalForcesAxes",&XC::PrismaticBarCrossSection::getInternalForcesAxis)
  .def("getNeutralAxis",&XC::PrismaticBarCrossSection::getNeutralAxis,"Returns neutral axis.")
  .def("getPointOnTensionedHalfPlane",&XC::PrismaticBarCrossSection::getPointOnTensionedHalfPlane)
  .def("getPointOnCompressedHalfPlane",&XC::PrismaticBarCrossSection::getPointOnCompressedHalfPlane)
  .def("setTrialDeformationPlane",&XC::PrismaticBarCrossSection::setTrialDeformationPlane,"trial deformations are calculated with the deformation plane passed as argument.")
  .def("getDeformationPlane",&XC::PrismaticBarCrossSection::getDeformationPlane,"returns section deformation plane.")
  .def("setInitialDeformationPlane",&XC::PrismaticBarCrossSection::setInitialDeformationPlane,"initial deformations are calculated with the deformation plane passed as argument.")
  .def("getInitialDeformationPlane",&XC::PrismaticBarCrossSection::getInitialDeformationPlane,"returns section initial deformation plane.")
  .add_property("deformationPlane", &XC::PrismaticBarCrossSection::getDeformationPlane, &XC::PrismaticBarCrossSection::setTrialDeformationPlane,"section deformation plane.")
  .add_property("initialDeformationPlane", &XC::PrismaticBarCrossSection::getInitialDeformationPlane, &XC::PrismaticBarCrossSection::setInitialDeformationPlane,"initial section deformation plane.")
  .def("EA", make_function(&XC::PrismaticBarCrossSection::EA, return_value_policy<copy_const_reference>() ), "return axial stiffness.")
  .def("GAy", make_function(&XC::PrismaticBarCrossSection::GAy, return_value_policy<copy_const_reference>() ), " return shear stiffness on y axis.")
  .def("GAz", make_function(&XC::PrismaticBarCrossSection::GAz, return_value_policy<copy_const_reference>() ), " return shear stiffness on z axis.")
  .def("EIz", make_function(&XC::PrismaticBarCrossSection::EIz, return_value_policy<copy_const_reference>() ), " return bending stiffness around z axis.")
  .def("EIy", make_function(&XC::PrismaticBarCrossSection::EIy, return_value_policy<copy_const_reference>() ), " return bending stiffness around y axis.")
  .def("EIyz", make_function(&XC::PrismaticBarCrossSection::EIyz, return_value_policy<copy_const_reference>() ), " return the product of inertia multiplied by the Young modulus.")
  .def("GJ", make_function(&XC::PrismaticBarCrossSection::GJ, return_value_policy<copy_const_reference>() ), " return the torsional stiffness.")
  .def("getTheta", &XC::PrismaticBarCrossSection::getTheta, "return the angle that defines the principal axis of inertia.")
  .def("getEI1", &XC::PrismaticBarCrossSection::getEI1, " return the bending stiffness around the major principal axis of inertia.")
  .def("getEI2", &XC::PrismaticBarCrossSection::getEI2, " return the bending stiffness around the minor principal axis of inertia.")
  .def("getInertiaAxes", &XC::PrismaticBarCrossSection::getInertiaAxes, " return the principal axes of inertia of the cross-section.")
  .def("getAxis1VDir", &XC::PrismaticBarCrossSection::getAxis1VDir, " return the direction vector of the major principal axis of inertia.")
  .def("getVDirStrongAxis", &XC::PrismaticBarCrossSection::getVDirStrongAxis, " return the direction vector of the strong axis.")
  .def("getAxis2VDir", &XC::PrismaticBarCrossSection::getAxis2VDir, "return the direction vector of the major principal axis of inertia.")
  .def("getVDirWeakAxis", &XC::PrismaticBarCrossSection::getVDirWeakAxis, " return the direction vector of the weak axis.")
//.def("getTractionHalfPlane",getTensionedHalfPlane)
//  .def("getTractionHalfPlaneFromLine",getTensionedHalfPlaneRecta)
//  .def("getCompressedHalfPlane",&XC::PrismaticBarCrossSection::getCompressedHalfPlane)
//  .def("getCompressedHalfPlane",&XC::PrismaticBarCrossSection::getCompressedHalfPlane)
  ;

class_<XC::SectionAggregator , bases<XC::PrismaticBarCrossSection>, boost::noncopyable >("SectionAggregator", no_init)
  .def("getSection", make_function(&XC::SectionAggregator::getSection, return_internal_reference<>()))
  .def("setSection",&XC::SectionAggregator::setSection)
  .def("setAdditions",&XC::SectionAggregator::setAdditionsPyList)
  .def("getAdditions", make_function(&XC::SectionAggregator::getAdditions, return_internal_reference<>()))
  ;

class_<XC::AggregatorAdditions, bases<XC::DqUniaxialMaterial>, boost::noncopyable >("AggregatorAdditions", no_init);

class_<XC::GenericSection1d , bases<XC::PrismaticBarCrossSection>, boost::noncopyable >("GenericSection1d", no_init);

typedef std::vector<XC::PrismaticBarCrossSection *> v_sections;
class_<v_sections, boost::noncopyable>("v_sections")
  .def(vector_indexing_suite<v_sections>() )
  ;

class_<XC::PrismaticBarCrossSectionsVector, bases<CommandEntity,v_sections>, boost::noncopyable >("PrismaticBarCrossSectionsVector", no_init);

class_<XC::CrossSectionKR, bases<CommandEntity>, boost::noncopyable >("CrossSectionKR", no_init);



#include "repres/python_interface.tcc"
#include "elastic_section/python_interface.tcc"
#include "plate_section/python_interface.tcc"
