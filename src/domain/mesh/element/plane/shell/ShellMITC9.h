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
// 9-node lagrangian shell element with membrane and drill
//
#ifndef ShellMITC9_h
#define ShellMITC9_h

#include "domain/mesh/element/plane/QuadBase9N.h"
#include "domain/mesh/element/utils/physical_properties/SectionFDPhysicalProperties.h"
#include "domain/mesh/element/utils/coordTransformation/ShellLinearCrdTransf3d.h"
#include "domain/mesh/element/utils/fvectors/FVectorShell.h"

namespace XC {

//! @ingroup PlaneElements
//
//! @brief Lagrangian shell element with membrane and drill.
class ShellMITC9: public QuadBase9N<SectionFDPhysicalProperties>
  {
  private : 
    double Ktt;//!< drilling stiffness
    ShellLinearCrdTransf3d theCoordTransf; //!< Coordinate transformation.
    mutable Matrix Ki; //!< Stiffness.
    
    double xl[2][9]; //!< local nodal coordinates, two coordinates for each of nine nodes

    FVectorShell p0; //!< Reactions in the basic system due to element loads

    //static data
    static Matrix stiff;
    static Vector resid;
    static Matrix mass;
    static Matrix damping;


    void computeBasis(void);
    void formInertiaTerms(int tangFlag) const;
    void formResidAndTangent(int tang_flag) const;

    //compute Jacobian matrix and inverse at point {L1,L2}
    //void  computeJacobian( double L1, double L2,const double x[2][9], 
    //                       Matrix &JJ,Matrix &JJinv );

    double* computeBdrill(int node,const double shp[3][9]) const;
    const Matrix& assembleB(const Matrix &Bmembrane, const Matrix &Bbend, const Matrix &Bshear) const;
    const Matrix& computeBmembrane(int node, const double shp[3][9]) const;
    const Matrix& computeBbend(int node, const double shp[3][9]) const;
    const Matrix& computeBshear(int node, const double shp[3][9]) const;
    
    //Matrix transpose
    Matrix transpose( int dim1, int dim2, const Matrix &M);

    //shape function routine for four node quads
    static void shape2d( double ss, double tt, const double x[2][9], double shp[3][9], double &xsj );
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    //null constructor
    ShellMITC9(void);
    //full constructor
    ShellMITC9(int tag,const SectionForceDeformation *theMaterial);
    Element *getCopy(void) const;

    int getNumDOF(void) const;

    void setDomain(Domain *theDomain);

    //print out element data
    void Print(std::ostream &, int flag) const;
	
    //return stiffness matrix 
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff() const;
    const Matrix &getMass() const;

    const GaussModel &getGaussModel(void) const;
    virtual const SectionForceDeformation *getSectionPtr(const size_t &) const;

    void alive(void);
    void zeroLoad(void);
    // methods for applying loads
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    //get residual
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    virtual ShellCrdTransf3dBase *getCoordTransf(void);
    virtual const ShellCrdTransf3dBase *getCoordTransf(void) const;
    double getArea(bool initialGeometry= true) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

  };
} // end of XC namespace

#endif



