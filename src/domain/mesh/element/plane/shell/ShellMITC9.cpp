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
                                                                        
// Written: Leopoldo Tesser, Diego Talledo
// 9-node lagrandian shell element with membrane and drill

#include "ShellMITC9.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "domain/load/plane/ShellMecLoad.h"
#include "domain/mesh/element/utils/coordTransformation/R3vectors.h"
#include "utility/utils/misc_utils/colormod.h"

//static data
XC::Matrix  XC::ShellMITC9::stiff(54,54);
XC::Vector  XC::ShellMITC9::resid(54); 
XC::Matrix  XC::ShellMITC9::mass(54,54);

//! @brief null constructor
XC::ShellMITC9::ShellMITC9(void)
  :QuadBase9N<SectionFDPhysicalProperties>( 0, ELE_TAG_ShellMITC9, SectionFDPhysicalProperties(9)), Ktt(0.0),theCoordTransf() {}

//! @brief Return the Gauss points of the element.
const XC::GaussModel &XC::ShellMITC9::getGaussModel(void) const
  { return gauss_model_quad9; }

//! @brie Return a pointer to the i-th section of the element.
const XC::SectionForceDeformation *XC::ShellMITC9::getSectionPtr(const size_t &i) const
  {
    const SectionForceDeformation *retval= nullptr;
    const size_t sz= this->physicalProperties.size();
    if(sz>i)
      retval= this->physicalProperties[i];
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; index " << i << " out of range: (0,"
	        << sz << ")."
		<< Color::def << std::endl;
    return retval;
  }
//! @brief full constructor
XC::ShellMITC9::ShellMITC9(int tag,const SectionForceDeformation *ptr_mat)
  :QuadBase9N<SectionFDPhysicalProperties>(tag, ELE_TAG_ShellMITC9, SectionFDPhysicalProperties(9,ptr_mat)), Ktt(0.0),theCoordTransf() {}

//! @brief Virtual constructor.
XC::Element* XC::ShellMITC9::getCopy(void) const
  { return new ShellMITC9(*this); }

//! @brief set domain
void XC::ShellMITC9::setDomain(Domain *theDomain ) 
  {
    QuadBase9N<SectionFDPhysicalProperties>::setDomain(theDomain);

    static Vector eig(3);
    static Matrix ddMembrane(3,3);


    //compute drilling stiffness penalty parameter
    const Matrix &dd= physicalProperties[0]->getInitialTangent();

    //assemble ddMembrane;
    for(int i=0;i<3;i++)
      for(int j=0;j<3;j++)
        ddMembrane(i,j)= dd(i,j);

    //eigenvalues of ddMembrane
    eig= LovelyEig(ddMembrane);
  
    //set ktt 
    //Ktt= dd(2,2);  //shear modulus 
    Ktt= std::min( eig(2), std::min( eig(0), eig(1) ) );

    //basis vectors and local coordinates
    computeBasis();
  }

//! @brief return number of dofs
int XC::ShellMITC9::getNumDOF(void) const
  { return 54; }

//! @brief return stiffness matrix 
const XC::Matrix &XC::ShellMITC9::getTangentStiff(void) const
  {
    int tang_flag= 1; //get the tangent 
    //do tangent and residual here
    formResidAndTangent(tang_flag);  
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }

double XC::ShellMITC9::getArea(bool initialGeometry) const
  { return getPolygon(initialGeometry).getArea(); }

//! @brief return secant matrix 
const XC::Matrix &XC::ShellMITC9::getInitialStiff(void) const 
  {
    if(Ki.isEmpty())
      {
	static const int ndf= 6; //two membrane plus three bending plus one drill
	static const int nstress= 8; //three membrane, three moment, two shear
	static const int ngauss= 9; 
	static const int numnodes= 9;

	double volume= 0.0;

	static double xsj;  // determinant jacobian matrix 
	static double dvol[ngauss]; //volume element
	static double shp[3][numnodes];  //shape functions at a gauss point

	static Matrix stiffJK(ndf,ndf); //nodeJK stiffness 
	static Matrix dd(nstress,nstress);  //material tangent

	//static Matrix J0(2,2);  //Jacobian at center

	//static Matrix J0inv(2,2); //inverse of Jacobian at center

	//---------B-matrices------------------------------------
	static Matrix BJ(nstress,ndf);      // B matrix node J
	static Matrix BJtran(ndf,nstress);
	static Matrix BK(nstress,ndf);      // B matrix node k
	static Matrix BJtranD(ndf,nstress);
	static Matrix Bbend(3,3);  // bending B matrix
	static Matrix Bshear(2,3); // shear B matrix
	static Matrix Bmembrane(3,2); // membrane B matrix
	static double BdrillJ[ndf]; //drill B matrix
	static double BdrillK[ndf];  

	double *drillPointer;

	static double saveB[nstress][ndf][numnodes];

	//-------------------------------------------------------
	stiff.Zero();

	//compute Jacobian and inverse at center
	//double L1= 0.0;
	//double L2= 0.0;
	//computeJacobian( L1, L2, xl, J0, J0inv ); 

	//gauss loop 
	for(int i= 0;i<ngauss;i++)
	  {
	    //get shape functions
	    const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
	    shape2d(gp.r_coordinate(), gp.s_coordinate(),xl,shp,xsj);

	    //volume element to also be saved
	    dvol[i]= gp.weight()*xsj;  
	    volume+= dvol[i];

	    // j-node loop to compute strain 
	    for(int j=0;j<numnodes;j++)
	      {
		//compute B matrix 
		Bmembrane= computeBmembrane( j, shp );
		Bbend= computeBbend( j, shp );
		Bshear= computeBshear( j, shp );
		BJ= assembleB(Bmembrane,Bbend,Bshear);

		//save the B-matrix
		for(int p=0; p<nstress; p++)
		  for(int q=0;q<ndf;q++)
		    saveB[p][q][j]= BJ(p,q);

		//drilling B matrix
		drillPointer= computeBdrill( j, shp );
		for(int p=0; p<ndf; p++ )
		  {
		    //BdrillJ[p]= *drillPointer++;
		    BdrillJ[p]= *drillPointer; //set p-th component
		    drillPointer++;             //pointer arithmetic
		  }//end for p
	      } // end for j

	    dd= physicalProperties[i]->getInitialTangent();
	    dd*= dvol[i];

	    //residual and tangent calculations node loops

	    int jj= 0;
	    for(int j= 0;j<numnodes;j++)
	      {
		//extract BJ
		for(int p=0; p<nstress;p++)
		  {
		    for(int q=0;q<ndf;q++)
		      BJ(p,q)= saveB[p][q][j];
		  }//end for p

		//multiply bending terms by (-1.0) for correct statement
		// of equilibrium  
		for(int p= 3;p<6;p++)
		  {
		    for(int q= 3;q<6;q++) 
		    BJ(p,q)*= (-1.0);
		   } //end for p

		//transpose 
		//BJtran= transpose( 8, ndf, BJ );
		for(int p=0; p<ndf; p++)
		  {
		    for(int q=0; q<nstress; q++) 
		      BJtran(p,q)= BJ(q,p);
		  }//end for p

		//drilling B matrix
		drillPointer= computeBdrill( j, shp );
		for(int p=0;p<ndf;p++)
		  {
		    BdrillJ[p]= *drillPointer;
		    drillPointer++;
		  }//end for p

		//BJtranD= BJtran * dd;
		BJtranD.addMatrixProduct(0.0, BJtran,dd,1.0 );


		for(int p=0; p<ndf; p++) 
		  BdrillJ[p]*= (Ktt*dvol[i]);


		int kk= 0;
		for(int k= 0;k<numnodes;k++)
		  {
		    //extract BK
		    for(int p=0;p<nstress;p++)
		      {
			for(int q=0; q<ndf; q++ )
			  BK(p,q)= saveB[p][q][k];
		      }//end for p

		    //drilling B matrix
		    drillPointer= computeBdrill( k, shp );
		    for(int p=0; p<ndf; p++)
		      {
			BdrillK[p]= *drillPointer;
			drillPointer++;
		      }//end for p

		    //stiffJK= BJtranD * BK ;
		    // +  transpose( 1,ndf,BdrillJ ) * BdrillK; 
		    stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0 );

		    for(int p= 0;p<ndf;p++)
		      for(int q= 0;q<ndf;q++)
			stiff( jj+p, kk+q )+= stiffJK(p,q) + ( BdrillJ[p]*BdrillK[q] );
		    kk+= ndf;
		  } // end for k loop
		jj+= ndf;
	      } // end for j loop
	  } //end for i gauss loop 
	Ki= stiff;
      }
    return Ki;
  }

//! @brief return mass matrix
const XC::Matrix &XC::ShellMITC9::getMass(void) const
  {
    int tangFlag= 1;
    formInertiaTerms( tangFlag );
    if(isDead())
      mass*=dead_srf;
    return mass;
  }

//! @brief Reactivates the element.
void XC::ShellMITC9::alive(void)
  {
    if(isDead())
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
  		  << "; not implemented yet."
                  << Color::def << std::endl;
        QuadBase9N<SectionFDPhysicalProperties>::alive();
      }
  }

//! @brief Zeroes the element load vector.
void XC::ShellMITC9::zeroLoad(void)
  {
    QuadBase9N<SectionFDPhysicalProperties>::zeroLoad();
    p0.zero();
  }

int XC::ShellMITC9::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << Color::red << getClassName() 
                << "; load over inactive element: "
                << getTag()
		<< Color::def << std::endl;
    else
      {
	computeTributaryAreas();
        const std::vector<double> areas= getTributaryAreas();

        // Accumulate elastic deformations in basic system
        if(ShellMecLoad *shellMecLoad= dynamic_cast<ShellMecLoad *>(theLoad))
          {
            shellMecLoad->addReactionsInBasicSystem(areas,loadFactor,p0); // Accumulate reactions in basic system
          }
        else
          return QuadBase9N<SectionFDPhysicalProperties>::addLoad(theLoad,loadFactor);
      }
    return -1;
  }

int XC::ShellMITC9::addInertiaLoadToUnbalance(const Vector &accel)
  {
    int tangFlag= 1;

    const bool haveRho= physicalProperties.haveRho();
    if(!haveRho) 
      return 0;

    int count= 0;
    for(int i=0; i<9; i++)
      {
        const Vector &Raccel= theNodes[i]->getRV(accel);
        for(int j=0; j<6; j++)
          resid(count++)= Raccel(i);
      }

    formInertiaTerms( tangFlag );
    if(load.isEmpty()) 
      load.reset(54);
    load.addMatrixVector(1.0, mass, resid, -1.0);
    return 0;
  }

//! @brief get residual
const XC::Vector &XC::ShellMITC9::getResistingForce(void) const
  {
    int tang_flag= 0; //don't get the tangent
    formResidAndTangent( tang_flag );

    if(!load.isEmpty())
      resid-= load;
    resid+= theCoordTransf.getGlobalResistingForce(p0.getVector());
    if(isDead())
      resid*=dead_srf;
    return resid;   
  }

//! @brief get residual with inertia terms
const XC::Vector &XC::ShellMITC9::getResistingForceIncInertia(void) const
  {
    static Vector res(54);
    res= getResistingForce();

    formInertiaTerms(0);

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      res+= this->getRayleighDampingForces();
    return res;
  }
//! @brief form inertia terms
void XC::ShellMITC9::formInertiaTerms(int tangFlag) const
  {
    //translational mass only
    //rotational inertia terms are neglected

    static const int ndf= 6; 
    static const int numberNodes= 9;
    static const int numberGauss= 9;
    static const int nShape= 3;
    static const int massIndex= nShape - 1;

    double xsj;  // determinant jacobian matrix 
    double dvol; //volume element
    static double shp[nShape][numberNodes];  //shape functions at a gauss point
    static Vector momentum(ndf);
    
    double temp, massJK;

    //zero mass 
    mass.Zero();


    //gauss loop
    for(int i= 0; i < numberGauss; i++ )
      {
        //get shape functions
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        shape2d(gp.r_coordinate(),gp.s_coordinate(), xl, shp, xsj );
        //volume element to also be saved
        dvol= gp.weight() * xsj;

        //node loop to compute accelerations
        momentum.Zero();
        for(int j= 0;j<numberNodes;j++) 
          //momentum += ( shp[massIndex][j] * theNodes[j]->getTrialAccel() );
          momentum.addVector(1.0, theNodes[j]->getTrialAccel(), shp[massIndex][j]);
      
        //density
        const double rhoH= physicalProperties[i]->getArealRho(); // mass per unit area

        //multiply acceleration by density to form momentum
        momentum*= rhoH;

        //residual and tangent calculations node loops
        //jj= 0;
        for(int j=0, jj=0;j<numberNodes;j++,jj+=ndf)
          {
            temp= shp[massIndex][j] * dvol;
 
            for(int p=0;p<3;p++)
              resid( jj+p ) += ( temp * momentum(p) );
     
            if(tangFlag == 1 && rhoH != 0.0)
              {
                //multiply by density
                temp*= rhoH;

                //node-node translational mass
                //kk= 0;
               for(int k=0, kk=0; k<numberNodes; k++, kk+=ndf )
                 {
                   massJK= temp*shp[massIndex][k];
                   for(int p= 0;p<3;p++) 
                     mass(jj+p,kk+p)+= massJK;
                 } // end for k loop
              } // end if tang_flag 
          } // end for j loop
      } //end for i gauss loop 
  }

//! @brief form residual and tangent
void XC::ShellMITC9::formResidAndTangent(int tang_flag) const
  {
    //
    //  six(6) nodal dof's ordered :
    //
    //    -        - 
    //   |    u1    |   <---plate membrane
    //   |    u2    |
    //   |----------|
    //   |  w= u3  |   <---plate bending
    //   |  theta1  | 
    //   |  theta2  | 
    //   |----------|
    //   |  theta3  |   <---drill 
    //    -        -  
    //
    // membrane strains ordered :
    //
    //            strain(0)=   eps00     i.e.   (11)-strain
    //            strain(1)=   eps11     i.e.   (22)-strain
    //            strain(2)=   gamma01   i.e.   (12)-shear
    //
    // curvatures and shear strains ordered  :
    //
    //            strain(3)=     kappa00  i.e.   (11)-curvature
    //            strain(4)=     kappa11  i.e.   (22)-curvature
    //            strain(5)=   2*kappa01  i.e. 2*(12)-curvature 
    //
    //            strain(6)=     gamma02  i.e.   (13)-shear
    //            strain(7)=     gamma12  i.e.   (23)-shear
    //
    //  same ordering for moments/shears but no 2 
    //  
    //  Then, 
    //              epsilon00= -z * kappa00      +    eps00_membrane
    //              epsilon11= -z * kappa11      +    eps11_membrane
    //  gamma01= 2*epsilon01= -z * (2*kappa01)  +  gamma01_membrane 
    //
    //  Shear strains gamma02, gamma12 constant through cross section
    //

    static const int ndf= 6; //two membrane plus three bending plus one drill
    static const int nstress= 8; //three membrane, three moment, two shear
    static const int ngauss= 9; 
    static const int numnodes= 9;


    int success;
  
    double volume= 0.0;

    static double xsj;  // determinant jacobian matrix 
    static double dvol[ngauss]; //volume element
    static Vector strain(nstress);  //strain
    static double shp[3][numnodes];  //shape functions at a gauss point
    static Vector residJ(ndf); //nodeJ residual 
    static Matrix stiffJK(ndf,ndf); //nodeJK stiffness 
    static Vector stress(nstress);  //stress resultants
    static Matrix dd(nstress,nstress);  //material tangent

    double epsDrill= 0.0;  //drilling "strain"
    double tauDrill= 0.0; //drilling "stress"

    //---------B-matrices------------------------------------

    static Matrix BJ(nstress,ndf);      // B matrix node J
    static Matrix BJtran(ndf,nstress);
    static Matrix BK(nstress,ndf);      // B matrix node k
    static Matrix BJtranD(ndf,nstress);
    static Matrix Bbend(3,3);  // bending B matrix
    static Matrix Bshear(2,3); // shear B matrix
    static Matrix Bmembrane(3,2); // membrane B matrix
    static double BdrillJ[ndf]; //drill B matrix
    static double BdrillK[ndf];  

    double *drillPointer;

    static double saveB[nstress][ndf][numnodes];

    //-------------------------------------------------------
   
    //zero stiffness and residual 
    stiff.Zero();
    resid.Zero();

    //gauss loop 
    for(int i= 0; i < ngauss; i++ )
      {
        //get shape functions
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        shape2d( gp.r_coordinate(), gp.s_coordinate(), xl, shp, xsj );

        //volume element to also be saved
        dvol[i]= gp.weight() * xsj;
	volume += dvol[i];

        //zero the strains
        strain.Zero();
        epsDrill= 0.0;

        // j-node loop to compute strain 
        for(int j= 0; j < numnodes; j++ )
          {
            //compute B matrix 
            Bmembrane= computeBmembrane( j, shp );
            Bbend= computeBbend( j, shp );
            Bshear= computeBshear( j, shp );
	  
            BJ= assembleB( Bmembrane, Bbend, Bshear );

 	    //save the B-matrix
	    for(int p=0; p<nstress; p++)
              for(int q=0; q<ndf; q++ )
                saveB[p][q][j]= BJ(p,q);

            //nodal "displacements" 
            const Vector &ul= theNodes[j]->getTrialDisp();

            //compute the strain
            //strain += (BJ*ul); 
            strain.addMatrixVector(1.0, BJ,ul,1.0 );

            //drilling B matrix
            drillPointer= computeBdrill( j, shp );
            for(int p=0; p<ndf; p++)
              {
	        //BdrillJ[p]= *drillPointer++;
	        BdrillJ[p]= *drillPointer; //set p-th component
	        drillPointer++;             //pointer arithmetic
              }//end for p

            //drilling "strain" 
            for(int p= 0; p < ndf; p++ )
	      epsDrill +=  BdrillJ[p]*ul(p);
	  } // end for j
      //send the strain to the material 
      success= const_cast<SectionForceDeformation *>(physicalProperties[i])->setTrialSectionDeformation(strain);
      //compute the stress
      stress= physicalProperties[i]->getStressResultant();
      //drilling "stress" 
      tauDrill= Ktt * epsDrill;

      //multiply by volume element
      stress*= dvol[i];
      tauDrill *= dvol[i];

      if(tang_flag == 1)
        {
          dd= physicalProperties[i]->getSectionTangent();
          dd*= dvol[i];
        } //end if tang_flag


      //residual and tangent calculations node loops
      int jj= 0;
      for(int j= 0; j < numnodes; j++ )
        {
          //extract BJ
          for(int p=0; p<nstress; p++)
            {
	      for(int q=0; q<ndf; q++ )
	        BJ(p,q)= saveB[p][q][j];
            }//end for p

          //multiply bending terms by (-1.0) for correct statement
          // of equilibrium  
          for(int p= 3; p < 6; p++)
            for(int q= 3; q < 6; q++ ) 
              BJ(p,q) *= (-1.0);

          //transpose 
          //BJtran= transpose( 8, ndf, BJ );
          for(int p=0; p<ndf; p++)
	    for(int q=0; q<nstress; q++) 
	      BJtran(p,q)= BJ(q,p);

          //residJ= BJtran * stress;
          residJ.addMatrixVector(0.0, BJtran,stress,1.0 );

          //drilling B matrix
          drillPointer= computeBdrill( j, shp );
          for(int p=0; p<ndf; p++ )
            {
	      BdrillJ[p]= *drillPointer;
	      drillPointer++;
            }//end for p

          //residual including drill
          for(int p= 0; p < ndf; p++ )
            resid( jj + p ) += ( residJ(p) + BdrillJ[p]*tauDrill );

          if(tang_flag == 1 )
            {
              //BJtranD= BJtran * dd;
              BJtranD.addMatrixProduct(0.0, BJtran,dd,1.0 );
        
	      for(int p=0; p<ndf; p++)
                { BdrillJ[p]*= ( Ktt*dvol[i] ); }

          int kk= 0;
          for(int k= 0; k < numnodes; k++ )
            {
              //extract BK
	      for(int p=0; p<nstress; p++)
                for(int q=0; q<ndf; q++ )
                  BK(p,q)= saveB[p][q][k];
	  
              //drilling B matrix
	      drillPointer= computeBdrill( k, shp );
	      for(int p=0; p<ndf; p++ )
                {
	          BdrillK[p]= *drillPointer;
	          drillPointer++;
		}//end for p
  
              //stiffJK= BJtranD * BK ;
	      // +  transpose( 1,ndf,BdrillJ ) * BdrillK; 
	      stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0 );

              for(int p= 0;p<ndf;p++)
	        for(int q= 0; q < ndf; q++ )
                  stiff( jj+p, kk+q ) += stiffJK(p,q) + ( BdrillJ[p]*BdrillK[q] );

              kk += ndf;
	    } // end for k loop
	  } // end if tang_flag 
        jj += ndf;
      } // end for j loop
    } //end for i gauss loop
  }

//! @brief compute local coordinates and basis
void XC::ShellMITC9::computeBasis(void) 
  {
    theCoordTransf= ShellLinearCrdTransf3d(theNodes);
    for(int i= 0;i<9;i++)
      {
        const Vector &coorI= theNodes[i]->getCrds();

        xl[0][i]= coorI^theCoordTransf.G1();
        xl[1][i]= coorI^theCoordTransf.G2();
      }  //end for i
  }

//! @brief compute Bdrill
double *XC::ShellMITC9::computeBdrill( int node, const double shp[3][9] ) const
  {
    static double Bdrill[6];
    static double B1;
    static double B2;
    static double B6;

    //---Bdrill Matrix in standard {1,2,3} mechanics notation---------
    //             -                                       -
    //   Bdrill= | -0.5*N,2   +0.5*N,1    0    0    0   -N |   (1x6) 
    //             -                                       -  
    //----------------------------------------------------------------

    B1=-0.5*shp[1][node]; 
    B2=+0.5*shp[0][node];
    B6=-shp[2][node];

    const Vector &g1= theCoordTransf.G1();
    const Vector &g2= theCoordTransf.G2();

    Bdrill[0]= B1*g1[0] + B2*g2[0];
    Bdrill[1]= B1*g1[1] + B2*g2[1]; 
    Bdrill[2]= B1*g1[2] + B2*g2[2];

    const Vector &g3= theCoordTransf.G3();
    Bdrill[3]= B6*g3[0];
    Bdrill[4]= B6*g3[1]; 
    Bdrill[5]= B6*g3[2];
    return Bdrill;
  }

//! @brief assemble a B matrix
const XC::Matrix &XC::ShellMITC9::assembleB( const Matrix &Bmembrane, const Matrix &Bbend, const Matrix &Bshear) const
  {
    //Matrix Bbend(3,3);  // plate bending B matrix
    //Matrix Bshear(2,3); // plate shear B matrix
    //Matrix Bmembrane(3,2); // plate membrane B matrix

    static Matrix B(8,6);
    static Matrix BmembraneShell(3,3); 
    static Matrix BbendShell(3,3); 
    static Matrix BshearShell(2,6);
    static Matrix Gmem(2,3);
    static Matrix Gshear(3,6);
    int pp;

    // For Shell : 
    //
    //---B Matrices in standard {1,2,3} mechanics notation---------
    //            -                     _          
    //           | Bmembrane  |     0    |
    //           | --------------------- |     
    //    B=     |     0      |  Bbend   |   (8x6) 
    //           | --------------------- |
    //           |         Bshear        |
    //            -           -         -
    //-------------------------------------------------------------

    const Vector &g1= theCoordTransf.G1();
    const Vector &g2= theCoordTransf.G2();
    const Vector &g3= theCoordTransf.G3();
    //shell modified membrane terms
    Gmem(0,0)= g1[0];
    Gmem(0,1)= g1[1];
    Gmem(0,2)= g1[2];

    Gmem(1,0)= g2[0];
    Gmem(1,1)= g2[1];
    Gmem(1,2)= g2[2];

    //BmembraneShell= Bmembrane * Gmem;
    BmembraneShell.addMatrixProduct(0.0, Bmembrane,Gmem,1.0 );

    //shell modified bending terms 
    Matrix &Gbend= Gmem;

    //BbendShell= Bbend * Gbend;
    BbendShell.addMatrixProduct(0.0, Bbend,Gbend,1.0 ); 

    //shell modified shear terms 
    Gshear.Zero();

    Gshear(0,0)= g3[0];
    Gshear(0,1)= g3[1];
    Gshear(0,2)= g3[2];

    Gshear(1,3)= g1[0];
    Gshear(1,4)= g1[1];
    Gshear(1,5)= g1[2];

    Gshear(2,3)= g2[0];
    Gshear(2,4)= g2[1];
    Gshear(2,5)= g2[2];

    //BshearShell= Bshear * Gshear;
    BshearShell.addMatrixProduct(0.0, Bshear,Gshear,1.0 );
  
    B.Zero();

    //assemble B from sub-matrices 

    //membrane terms 
    for(int p= 0; p < 3; p++ )
      for(int q= 0; q < 3; q++ ) 
        B(p,q)= BmembraneShell(p,q);

    //bending terms
    for(int p= 3; p < 6; p++ )
      {
        pp= p - 3;
        for(int q= 3; q < 6; q++ ) 
          B(p,q)= BbendShell(pp,q-3); 
      } // end for p

    //shear terms 
    for(int p= 0; p < 2; p++ )
      {
        pp= p + 6;
        for(int q= 0; q < 6; q++ )
          B(pp,q)= BshearShell(p,q); 
      } //end for p
    return B;
  }

//! @brief compute Bmembrane matrix
const XC::Matrix &XC::ShellMITC9::computeBmembrane( int node, const double shp[3][9] ) const
  {
    static Matrix Bmembrane(3,2);

    //---Bmembrane Matrix in standard {1,2,3} mechanics notation---------
    //                -             -
    //               | +N,1      0   | 
    // Bmembrane=   |   0     +N,2  |    (3x2)
    //               | +N,2    +N,1  |
    //                -             -  
    //  three(3) strains and two(2) displacements (for plate)
    //-------------------------------------------------------------------

    Bmembrane.Zero();

    Bmembrane(0,0)= shp[0][node];
    Bmembrane(1,1)= shp[1][node];
    Bmembrane(2,0)= shp[1][node];
    Bmembrane(2,1)= shp[0][node];
    return Bmembrane;
  }


//! @brief compute Bbend matrix
const XC::Matrix &XC::ShellMITC9::computeBbend( int node, const double shp[3][9] ) const
  {
    static Matrix Bbend(3,2);

    //---Bbend Matrix in standard {1,2,3} mechanics notation---------
    //            -             -
    //   Bbend= |    0    -N,1  | 
    //           |  +N,2     0   |    (3x2)
    //           |  +N,1   -N,2  |
    //            -             -  
    //  three(3) curvatures and two(2) rotations (for plate)
    //----------------------------------------------------------------

    Bbend.Zero();

    Bbend(0,1)= -shp[0][node];
    Bbend(1,0)=  shp[1][node];
    Bbend(2,0)=  shp[0][node];
    Bbend(2,1)= -shp[1][node]; 
    return Bbend;
  }

//! @brief compute standard Bshear matrix
const XC::Matrix &XC::ShellMITC9::computeBshear( int node, const double shp[3][9] ) const
  {
    static Matrix Bshear(2,3);

    //---Bshear Matrix in standard {1,2,3} mechanics notation------
    //             -                -
    //   Bshear= | +N,1      0    +N |  (2x3)
    //            | +N,2     -N     0 |
    //             -                -  
    //  two(2) shear strains, one(1) displacement and two(2) rotations for plate 
    //-----------------------------------------------------------------------

    Bshear.Zero();

    Bshear(0,0)=  shp[0][node];
    Bshear(0,2)=  shp[2][node];
    Bshear(1,0)=  shp[1][node];
    Bshear(1,1)= -shp[2][node];
    return Bshear;
  }

//! @brief shape function routine for nine node quads
void XC::ShellMITC9::shape2d( double ss, double tt,const double x[2][9], double shp[3][9],double &xsj)
  {
    static const double s[]= { -0.5,  0.5, 0.5, -0.5 };
    static const double t[]= { -0.5, -0.5, 0.5,  0.5 };
    static double xs[2][2];
    static double sx[2][2];

    for(int i= 0; i < 4; i++ )
      {
        shp[2][i]= ( 0.5 + s[i]*ss )*( 0.5 + t[i]*tt );
        shp[0][i]= s[i] * ( 0.5 + t[i]*tt );
        shp[1][i]= t[i] * ( 0.5 + s[i]*ss );
      } // end for i

    shp[2][8]= (1-ss*ss)*(1-tt*tt);

    shp[2][4]= (0.5+t[1]*tt)*(1-ss*ss)-shp[2][8]/2;
    shp[2][5]= (0.5+s[1]*ss)*(1-tt*tt)-shp[2][8]/2;
    shp[2][6]= (0.5+t[3]*tt)*(1-ss*ss)-shp[2][8]/2;
    shp[2][7]= (0.5+s[3]*ss)*(1-tt*tt)-shp[2][8]/2;

    shp[2][0]= shp[2][0]-(shp[2][4]+shp[2][7])/2-shp[2][8]/4;
    shp[2][1]= shp[2][1]-(shp[2][5]+shp[2][4])/2-shp[2][8]/4;
    shp[2][2]= shp[2][2]-(shp[2][6]+shp[2][5])/2-shp[2][8]/4;
    shp[2][3]= shp[2][3]-(shp[2][7]+shp[2][6])/2-shp[2][8]/4;

    shp[0][8]= -2*ss*(1-tt*tt);
    shp[1][8]= -2*tt*(1-ss*ss);

    shp[0][4]= -2*ss*(0.5+t[1]*tt)-shp[0][8]/2;
    shp[0][5]= (1-tt*tt)*s[1]     -shp[0][8]/2;
    shp[0][6]= -2*ss*(0.5+t[3]*tt)-shp[0][8]/2;
    shp[0][7]= (1-tt*tt)*s[3]     -shp[0][8]/2;

    shp[1][4]= (1-ss*ss)*t[1]     -shp[1][8]/2;
    shp[1][5]= -2*tt*(0.5+s[1]*ss)-shp[1][8]/2;
    shp[1][6]= (1-ss*ss)*t[3]     -shp[1][8]/2;
    shp[1][7]= -2*tt*(0.5+s[3]*ss)-shp[1][8]/2;

    shp[0][0]= shp[0][0]-(shp[0][4]+shp[0][7])/2-shp[0][8]/4;
    shp[0][1]= shp[0][1]-(shp[0][5]+shp[0][4])/2-shp[0][8]/4;
    shp[0][2]= shp[0][2]-(shp[0][6]+shp[0][5])/2-shp[0][8]/4;
    shp[0][3]= shp[0][3]-(shp[0][7]+shp[0][6])/2-shp[0][8]/4;

    shp[1][0]= shp[1][0]-(shp[1][4]+shp[1][7])/2-shp[1][8]/4;
    shp[1][1]= shp[1][1]-(shp[1][5]+shp[1][4])/2-shp[1][8]/4;
    shp[1][2]= shp[1][2]-(shp[1][6]+shp[1][5])/2-shp[1][8]/4;
    shp[1][3]= shp[1][3]-(shp[1][7]+shp[1][6])/2-shp[1][8]/4;
  
    // Construct jacobian and its inverse
    for(int i= 0; i < 2; i++ )
      {
        for(int j= 0; j < 2; j++ )
          {
            xs[i][j]= 0.0;
            for(int k= 0; k < 9; k++ )
              { xs[i][j] +=  x[i][k] * shp[j][k]; } //end for k
          } //end for j
       }  // end for i 

    xsj= xs[0][0]*xs[1][1] - xs[0][1]*xs[1][0];
    //inverse jacobian
    double jinv= 1.0 / xsj;
    sx[0][0]=  xs[1][1] * jinv;
    sx[1][1]=  xs[0][0] * jinv;
    sx[0][1]= -xs[0][1] * jinv; 
    sx[1][0]= -xs[1][0] * jinv;

    double temp= 0;
    //form global derivatives 
    for(int i= 0; i < 9; i++ )
      {
        temp= shp[0][i]*sx[0][0] + shp[1][i]*sx[1][0];
        shp[1][i]= shp[0][i]*sx[0][1] + shp[1][i]*sx[1][1];
        shp[0][i]= temp;
      } // end for i
    return;
  }

XC::Matrix XC::ShellMITC9::transpose( int dim1,int dim2,const Matrix &M ) 
  {
    Matrix Mtran( dim2, dim1 );

    for(int i= 0; i < dim1; i++ )
      {
        for(int j= 0; j < dim2; j++ ) 
           Mtran(j,i)= M(i,j);
      } // end for i
    return Mtran;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ShellMITC9::getDbTagData(void) const
  {
    static DbTagData retval(19);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::ShellMITC9::sendData(Communicator &comm)
  {
    int res= QuadBase9N<SectionFDPhysicalProperties>::sendData(comm);
    res+=comm.sendDoubles(Ktt,xl[0][0],xl[0][1],xl[0][2],xl[0][3],getDbTagData(),CommMetaData(8));
    res+=comm.sendDoubles(xl[1][0],xl[1][1],xl[1][2],xl[1][3],getDbTagData(),CommMetaData(9));
    res+=comm.sendMovable(theCoordTransf,getDbTagData(),CommMetaData(10));
    res+= p0.sendData(comm,getDbTagData(),CommMetaData(11));
    res+= comm.sendMatrix(Ki,getDbTagData(),CommMetaData(14));
    //res+= comm.sendVectors(initDisp,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ShellMITC9::recvData(const Communicator &comm)
  {
    int res= QuadBase9N<SectionFDPhysicalProperties>::recvData(comm);
    res+=comm.receiveDoubles(Ktt,xl[0][0],xl[0][1],xl[0][2],xl[0][3],getDbTagData(),CommMetaData(8));
    res+=comm.receiveDoubles(xl[1][0],xl[1][1],xl[1][2],xl[1][3],getDbTagData(),CommMetaData(9));
    res+= comm.receiveMovable(theCoordTransf,getDbTagData(),CommMetaData(10));
    res+= p0.receiveData(comm,getDbTagData(),CommMetaData(11));
    res+= comm.receiveMatrix(Ki,getDbTagData(),CommMetaData(14));
    //res+= comm.receiveVectors(initDisp,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Returns a pointer a la coordinate transformation.
XC::ShellCrdTransf3dBase *XC::ShellMITC9::getCoordTransf(void)
  { return &theCoordTransf; }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
const XC::ShellCrdTransf3dBase *XC::ShellMITC9::getCoordTransf(void) const
  { return &theCoordTransf; }

//! @brief print out element data
void XC::ShellMITC9::Print(std::ostream &s, int flag ) const
  {
    if(flag == -1)
      {
        int eleTag= this->getTag();
        s << "EL_ShellMITC9\t" << eleTag << "\t";
        s << eleTag << "\t" << 1; 
        s  << "\t" << theNodes.getTagNode(0) << "\t" << theNodes.getTagNode(1);
        s  << "\t" << theNodes.getTagNode(2) << "\t" << theNodes.getTagNode(3);
	s  << "\t" << theNodes.getTagNode(4) << "\t" << theNodes.getTagNode(5);
        s  << "\t" << theNodes.getTagNode(6) << "\t" << theNodes.getTagNode(7);
        s  << "\t" << theNodes.getTagNode(8) << "\t0.00";
        s << std::endl;
        s << "PROP_3D\t" << eleTag << "\t";
        s << eleTag << "\t" << 1; 
        s  << "\t" << -1 << "\tSHELL\t1.0\0.0";
        s << std::endl;
      }
    else if(flag < -1)
      {
         int counter= (flag + 1) * -1;
         int eleTag= this->getTag();
         int i,j;
         for( i= 0; i < 9; i++ )
           {
             const Vector &stress= physicalProperties[i]->getStressResultant();
       
              s << "STRESS\t" << eleTag << "\t" << counter << "\t" << i << "\tTOP";
              for(j=0; j<6; j++)
                s << "\t" << stress(j);
              s << std::endl;
           }
      }
    else
      {
        s << std::endl;
        s << "NL Nine Node Shell \n";
        s << "Element Number: " << this->getTag() << std::endl;
        s << "Node 1 : " << theNodes.getTagNode(0) << std::endl;
        s << "Node 2 : " << theNodes.getTagNode(1) << std::endl;
        s << "Node 3 : " << theNodes.getTagNode(2) << std::endl;
        s << "Node 4 : " << theNodes.getTagNode(3) << std::endl;
        s << "Node 5 : " << theNodes.getTagNode(4) << std::endl;
        s << "Node 6 : " << theNodes.getTagNode(5) << std::endl;
        s << "Node 7 : " << theNodes.getTagNode(6) << std::endl;
        s << "Node 8 : " << theNodes.getTagNode(7) << std::endl;
        s << "Node 9 : " << theNodes.getTagNode(8) << std::endl;
        s << "Material Information : \n ";
        physicalProperties.Print( s, flag );
        s << std::endl;
      }
  }

int XC::ShellMITC9::sendSelf(Communicator &comm)
  {
    inicComm(getDbTagData().Size());
    int res= sendData(comm);

    const int dataTag= getDbTag();
    res= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to send ID data."
		<< Color::def << std::endl;
    return res;
  }
    
int  XC::ShellMITC9::recvSelf(const Communicator &comm)
  {
    inicComm(getDbTagData().Size());

    const int dataTag= getDbTag();
    int res = comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data."
		<< Color::def << std::endl;
    else
      res+= recvData(comm);
    return res;
  }

