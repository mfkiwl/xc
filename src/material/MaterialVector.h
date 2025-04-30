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
//  under the terms of the GNU General Public License published by 
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
//MaterialVector.h

#ifndef MaterialVector_h
#define MaterialVector_h

#include <vector>
#include "utility/kernel/CommandEntity.h"
#include "material/ResponseId.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "utility/utils/misc_utils/colormod.h"


namespace XC {

//! @ingroup Mat
//
//! @brief Material pointer container. It's used by
//! elements to store materials for each integration point.
template <class MAT>
class MaterialVector: public std::vector<MAT *>, public CommandEntity, public MovableObject
  {
  protected:
    void clearAll(void);
    void alloc(const std::vector<MAT *> &mats);


    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public:
    typedef typename std::vector<MAT *> mat_vector;
    typedef typename mat_vector::iterator iterator;
    typedef typename mat_vector::const_iterator const_iterator;
    typedef typename mat_vector::reference reference;
    typedef typename mat_vector::const_reference const_reference;

    MaterialVector(const size_t &nMat,const MAT *matModel= nullptr);
    MaterialVector(const MaterialVector<MAT> &);
    MaterialVector<MAT> &operator=(const MaterialVector<MAT> &);
    ~MaterialVector(void)
      { clearAll(); }

    void clearMaterials(void);
    void setMaterial(const MAT *);
    void setMaterial(size_t i,MAT *);
    void setMaterial(size_t i, const MAT &);
    void setMaterial(const MAT *,const std::string &);
    void copyPropsFrom(const EntityWithProperties *);
    
    bool empty(void) const;
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void setInitialGeneralizedStrains(const std::vector<Vector> &);
    void incrementInitialGeneralizedStrains(const std::vector<Vector> &);
    void zeroInitialGeneralizedStrains(void);

    size_t getGeneralizedStressSize(void) const;
    size_t getGeneralizedStrainSize(void) const;
    Matrix getGeneralizedStresses(void) const;
    Matrix getGeneralizedStrains(void) const;
    const Vector &getMeanGeneralizedStress(void) const;
    const Vector &getMeanGeneralizedStrain(void) const;
    double getMeanGeneralizedStrain(const int &defID) const;
    double getMeanGeneralizedStress(const int &defID) const;
    double getMeanGeneralizedStrain(const std::string &cod) const;
    double getMeanGeneralizedStress(const std::string &cod) const;

    std::deque<int> getComponentIndexesFromCode(const std::string &code) const;
    
    Vector getGeneralizedStrainAtGaussPoints(const int &) const;
    Vector getGeneralizedStressAtGaussPoints(const int &) const;
    
    Matrix getGeneralizedStrain(const int &defID) const;
    Matrix getGeneralizedStress(const int &defID) const;

    Matrix getValues(const std::string &, bool silent= false) const;
    
    std::set<std::string> getNames(void) const;
    boost::python::list getNamesPy(void) const;
    std::set<int> getTags(void) const;
    boost::python::list getTagsPy(void) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };

//! @brief Default constructor.
template <class MAT>
MaterialVector<MAT>::MaterialVector(const size_t &nMat,const MAT *matModel)
  : std::vector<MAT *>(nMat,nullptr), MovableObject(MAT_VECTOR_TAG)
  {
    if(matModel)
      {
        for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
          {
            (*i)= dynamic_cast<MAT *>(matModel->getCopy());
            if(!(*i))
              std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		        << "; failed allocate material model pointer"
			<< Color::def << std::endl;
          }
      }
  }

//! @brief Copy materials from another vector.
template <class MAT>
void MaterialVector<MAT>::alloc(const std::vector<MAT *> &mats)
  {
    clearAll();
    const size_t nMat= mats.size();
    this->resize(nMat);
    for(size_t i= 0;i<nMat;i++)
      {
        if(mats[i])
          {
            (*this)[i]= dynamic_cast<MAT *>(mats[i]->getCopy());
            if(!(*this)[i])
              std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		        << "; failed allocate material model pointer"
			<< Color::def << std::endl;
          }
      }
  }

//! @brief Copy constructor.
template <class MAT>
MaterialVector<MAT>::MaterialVector(const MaterialVector<MAT> &other)
  : std::vector<MAT *>(other.size(),nullptr), MovableObject(MAT_VECTOR_TAG)
  { alloc(other); }

//! @brief Assignment operator.
template <class MAT>
MaterialVector<MAT> &MaterialVector<MAT>::operator=(const MaterialVector<MAT> &other)
  { 
    alloc(other);
    return *this;
  }

template <class MAT>
void MaterialVector<MAT>::setMaterial(const MAT *new_mat)
  {
    clearMaterials();
    if(new_mat)
      {
        for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
          {
            (*i)= new_mat->getCopy();
            if(!(*i))
              std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		        << "; failed allocate material model pointer"
			<< Color::def << std::endl;
          }
      }
  }

template <class MAT>
void MaterialVector<MAT>::setMaterial(const MAT *new_mat, const std::string &type)
  {
    clearMaterials();
    if(new_mat)
      {
        for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
          {
            (*i)= new_mat->getCopy(type.c_str());
            if(!(*i))
              std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		        << "; failed allocate material model pointer"
			<< Color::def << std::endl;
          }
      }
  }

template <class MAT>
void MaterialVector<MAT>::setMaterial(size_t i,MAT *new_mat)
  {
    if((*this)[i])
      delete (*this)[i];
    (*this)[i]= new_mat;
  }

template <class MAT>
void MaterialVector<MAT>::setMaterial(size_t i, const MAT &new_mat)
  { setMaterial(i, new_mat.getCopy()); }

//! @brief copy the user defined properties of the given object on each of
//! the materials.  
template <class MAT>
void MaterialVector<MAT>::copyPropsFrom(const EntityWithProperties *other_mat)
  {
    if(other_mat)
      {
	const EntityWithProperties &tmp= *other_mat;
        for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
          {
            (*i)->copyPropsFrom(tmp);
          }
      }
  }

template <class MAT>
void MaterialVector<MAT>::clearMaterials(void)
  {
    for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
      {
        if(*i)
	  {
	    delete (*i);
            (*i)= nullptr;
	  }
      }
  }

//! @brief Returns true ifno se ha asignado material.
template <class MAT>
bool MaterialVector<MAT>::empty(void) const
  {
    if(mat_vector::empty())
      return true;
    else
      return ((*this)[0]==nullptr);
  }

template <class MAT>
void MaterialVector<MAT>::clearAll(void)
  {
    clearMaterials();
    std::vector<MAT *>::clear();
  }


//! @brief Commits materials state (normally after convergence).
template <class MAT>
int MaterialVector<MAT>::commitState(void)
  {
    int retVal= 0;

    for(iterator i=mat_vector::begin();i!=mat_vector::end();i++)
      retVal+= (*i)->commitState();
    return retVal;
  }

//! @brief Returns materials to its last committed state.
template <class MAT>
int MaterialVector<MAT>::revertToLastCommit(void)
  {
    int retVal= 0;

    for(iterator i=mat_vector::begin();i!=mat_vector::end();i++)
      retVal+= (*i)->revertToLastCommit() ;
    return retVal;
  }


//! @brief Return materials to its initial state.
template <class MAT>
int MaterialVector<MAT>::revertToStart(void)
  {
    int retVal = 0;

    for(iterator i=mat_vector::begin();i!=mat_vector::end();i++)
      retVal+= (*i)->revertToStart();
    return retVal;
  }

//! @brief Ask the materials about the values that correspond to the code.
//! @tparam MAT: material type.
//! @param cod: identifier of the requested value.
//! @param silent: if true don't complaint about non-existent property.
template <class MAT>
XC::Matrix XC::MaterialVector<MAT>::getValues(const std::string &code, bool silent) const
  {
    Matrix retval;
    const int nMat= this->size();
    std::vector<Matrix> tmp(nMat);
    int count= 0;
    int nRows= 0;
    int nCols= 0;
    for(const_iterator i=mat_vector::begin();i!=mat_vector::end();i++, count++)
      {
        const Matrix v= (*i)->getValues(code, silent);
	nRows+= v.noRows();
	nCols= std::max(nCols, v.noCols());
	tmp[count]= v;
      }
    retval.resize(nRows,nCols);
    int iRow= 0;
    int iCol= 0;
    for(int i= 0;i<nMat;i++)
      {
	const Matrix v= tmp[i];
        for(int j= 0;j<v.noRows();j++)
	  {
	    for(int k= 0;k<v.noCols();k++)
	      {
		iCol= k;
	        retval(iRow,iCol)= v(j,k);
	      }
	    iRow+= 1;
	  }
      }
    return retval;
  }
  
//! @brief Returns the size of stress vector.
template <class MAT>
size_t MaterialVector<MAT>::getGeneralizedStressSize(void) const
  { return (*this)[0]->getGeneralizedStress().Size(); }

//! @brief Returns the size of generalized strains vector.
template <class MAT>
size_t MaterialVector<MAT>::getGeneralizedStrainSize(void) const
  { return (*this)[0]->getGeneralizedStrain().Size(); }

//! @brief Returns generalized stress values on each integration point.
template <class MAT>
Matrix MaterialVector<MAT>::getGeneralizedStresses(void) const
  {
    const size_t ncol= getGeneralizedStressSize();
    const size_t nMat= this->size();
    Matrix retval(nMat,ncol);
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStress();
        retval.putRow(i,s);
      }
    return retval;
  }

//! @brief Returns generalized strain values on each integration point.
template <class MAT>
Matrix MaterialVector<MAT>::getGeneralizedStrains(void) const
  {
    const size_t ncol= getGeneralizedStrainSize();
    const size_t nMat= this->size();
    Matrix retval(nMat,ncol);
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStrain();
        for(size_t j= 0;j<ncol;j++)
          retval(i,j)= s(j);
      }
    return retval;
  }
  
//! @brief Returns average generalized stress values on element. In a future we can enhance this by providing an extrapolation of results in gauss points as described in 28.4 <a href="http://www.colorado.edu/engineering/cas/courses.d/IFEM.d/IFEM.Ch28.d/IFEM.Ch28.pdf">IFEM chapter 28.</a> 
template <class MAT>
const Vector &MaterialVector<MAT>::getMeanGeneralizedStress(void) const
  {
    static Vector retval;
    retval= (*this)[0]->getGeneralizedStress();
    const size_t nMat= this->size();
    for(size_t i= 1;i<nMat;i++)
      retval+= (*this)[i]->getGeneralizedStress();
    retval/=nMat;
    return retval;
  }

//! @brief Returns average generalized strain values on element.In a future we can enhance this by providing an extrapolation of results in gauss points as described in 28.4 <a href="http://www.colorado.edu/engineering/cas/courses.d/IFEM.d/IFEM.Ch28.d/IFEM.Ch28.pdf">IFEM chapter 28.</a> 
template <class MAT>
const Vector &MaterialVector<MAT>::getMeanGeneralizedStrain(void) const
  {
    static Vector retval;
    retval= (*this)[0]->getGeneralizedStrain();
    const size_t nMat= this->size();
    for(size_t i= 0;i<nMat;i++)
      retval+= (*this)[i]->getGeneralizedStrain();
    retval/=nMat;
    return retval;
  }
  
//! @brief Return the strain or stress vector index corresponding to the given
//! code.
template <class MAT>
std::deque<int> MaterialVector<MAT>::getComponentIndexesFromCode(const std::string &code) const
  {
    const ResponseId &rId= (*this)[0]->getResponseType();
    return rId.getComponentIndexesFromCode(code);
  }

//! @brief Returns the defID component of the average strain vector.
//! @param defID component index.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStrain(const int &defID) const
  {
    double retval= 0.0;
    const Vector &e= getMeanGeneralizedStrain(); //generalized strains vector.
    const ResponseId &code= (*this)[0]->getResponseType();
    const int order= code.Size();
    for(int i= 0;i<order;i++)
      if(code(i) == defID)
        retval+= e(i);
    return retval;
  }
  
//! @brief Returns the defID component of the average strain vector.
//! @param cod string label identifying the component index.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStrain(const std::string &cod) const
  {
    const ResponseId &rId= (*this)[0]->getResponseType();
    const int defId= rId.getComponentIdFromString(cod);
    return this->getMeanGeneralizedStrain(defId);
  }

//! @brief Returns the defID component of the strain vector at Gauss points.
//! @param defID component index.
template <class MAT>
XC::Vector MaterialVector<MAT>::getGeneralizedStrainAtGaussPoints(const int &defID) const
  {
    const size_t nMat= this->size();
    const ResponseId &code= (*this)[0]->getResponseType();
    const int order= code.Size();
    Vector retval(nMat);
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &e= (*this)[i]->getGeneralizedStrain(); //Strain at i-th Gauss point.
        for(int j= 0;j<order;j++) // Searching for the component.
	  if(code(j) == defID)
	    retval(i)= e(j);
      }
    return retval;
  }


//! @brief Returns the defID component of the average generalized stress vector.
//! @param defID component index.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStress(const int &defID) const
  {
    double retval= 0.0;
    const Vector &f= getMeanGeneralizedStress(); //Vector de esfuerzos.
    const ResponseId &code= (*this)[0]->getResponseType();
    const int order= code.Size();
    for(int i= 0;i<order;i++)
      if(code(i) == defID)
        retval+= f(i);
    return retval;
  }
  
//! @brief Returns the defID component of the average stress vector.
//! @param cod string label identifying the component index.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStress(const std::string &cod) const
  {
    const ResponseId &rId= (*this)[0]->getResponseType();
    const int defId= rId.getComponentIdFromString(cod);
    return this->getMeanGeneralizedStress(defId);
  }
   
//! @brief Returns the defID component of the stress vector at Gauss points.
//! @param defID component index.
template <class MAT>
XC::Vector MaterialVector<MAT>::getGeneralizedStressAtGaussPoints(const int &defID) const
  {
    const size_t nMat= this->size();
    const ResponseId &code= (*this)[0]->getResponseType();
    const int order= code.Size();
    Vector retval(nMat);
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &e= (*this)[i]->getGeneralizedStress(); //Stress at i-th Gauss point.
        for(int j= 0;j<order;j++) // Searching for the component.
	  if(code(j) == defID)
	    retval(i)= e(j);
      }
    return retval;
  }

//! @brief Returns the defID component of generalized stress vector on each integration point.
//! @param defID component index.
template <class MAT>
Matrix MaterialVector<MAT>::getGeneralizedStress(const int &defID) const
  {
    const size_t nMat= this->size();
    Matrix retval(nMat,1);
    const ResponseId &code= (*this)[0]->getResponseType();
    const int order= code.Size();
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStress();
        for(int j= 0;j<order;j++)
          if(code(j) == defID)
            retval(i,1)+= s(i);
      }
    return retval;
  }

//! @brief Returns the defID component of generalized strain vector on each integration point.
//! @param defID component index.
template <class MAT>
Matrix MaterialVector<MAT>::getGeneralizedStrain(const int &defID) const
  {
    const size_t nMat= this->size();
    Matrix retval(nMat,1);
    const ResponseId &code= (*this)[0]->getResponseType();
    const int order= code.Size();
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStrain();
        for(int j= 0;j<order;j++)
          if(code(j) == defID)
            retval(i,1)+= s(i);
      }
    return retval;
  }

//! @brief Assigns initial values to materials initial strains.
template <class MAT>
void MaterialVector<MAT>::setInitialGeneralizedStrains(const std::vector<Vector> &iS)
  {
    const size_t nMat= this->size();
    const size_t sz= std::min(nMat,iS.size());
    if(iS.size()<nMat)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; received: "
                << iS.size() << " generalized strain vectors, expected: "
                << nMat << "."
	        << Color::def << std::endl;
    for(size_t i= 0;i<sz;i++)
      (*this)[i]->setInitialGeneralizedStrain(iS[i]);
  }

//! \brief Adds to the materials initial strains the values being passed as parameters.
template <class MAT>
void MaterialVector<MAT>::incrementInitialGeneralizedStrains(const std::vector<Vector> &iS)
  {
    const size_t nMat= this->size();
    const size_t sz= std::min(nMat,iS.size());
    if(iS.size()<nMat)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; received: "
                << iS.size() << " generalized strain vectors, expected: "
                << nMat << "."
	        << Color::def << std::endl;
    for(size_t i= 0;i<sz;i++)
      (*this)[i]->incrementInitialGeneralizedStrain(iS[i]);
  }

//! \brief Initialize initial strains.
template <class MAT>
void MaterialVector<MAT>::zeroInitialGeneralizedStrains(void)
  {
    const size_t nMat= this->size();
    for(size_t i= 0;i<nMat;i++)
      (*this)[i]->zeroInitialGeneralizedStrain();
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
template <class MAT>
XC::DbTagData &MaterialVector<MAT>::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Send object members through the communicator argument.
template <class MAT>
int MaterialVector<MAT>::sendData(Communicator &comm)
  {
    int res= 0;
    if(this->empty())
      setDbTagDataPos(0,0);
    else
      {
        setDbTagDataPos(0,1);
        const size_t nMat= this->size();
        DbTagData cpMat(nMat*3);

        for(size_t i= 0;i<nMat;i++)
          res+= comm.sendBrokedPtr((*this)[i],cpMat,BrokedPtrCommMetaData(i,i+nMat,i+2*nMat));
        res+= cpMat.send(getDbTagData(),comm,CommMetaData(1));
      }
    return res;
  }

//! @brief Receives object through the communicator argument.
template <class MAT>
int MaterialVector<MAT>::recvData(const Communicator &comm)
  {
    const int flag= getDbTagDataPos(0);
    int res= 0;
    if(flag!=0)
      {
        const size_t nMat= this->size();
        DbTagData cpMat(nMat*3);
        res+= cpMat.receive(getDbTagData(),comm,CommMetaData(1));
	clearMaterials(); // erase existing materials if any.
        for(size_t i= 0;i<nMat;i++)
          {
            const BrokedPtrCommMetaData meta(i,i+nMat,i+2*nMat);
            // Receive the material
            (*this)[i]= comm.getBrokedMaterial((*this)[i],cpMat,meta);
          }
      }
    return res;
  }

//! @brief Return the names of the materials.
template <class MAT>
std::set<std::string> MaterialVector<MAT>::getNames(void) const
  {
    std::set<std::string> retval;
    for(const_iterator i= mat_vector::begin();i!=mat_vector::end();i++)
      retval.insert((*i)->getName());
    return retval;
  }

//! @brief Return the names of the materials in a python list.
template <class MAT>
boost::python::list MaterialVector<MAT>::getNamesPy(void) const
  {
    boost::python::list retval;
    std::set<std::string> tmp= getNames();
    for(std::set<std::string>::const_iterator i= tmp.begin();i!=tmp.end();i++)
        retval.append(*i);
    return retval;
  }  

//! @brief Return the identifiers of the materials.
template <class MAT>
std::set<int> MaterialVector<MAT>::getTags(void) const
  {
    std::set<int> retval;
    for(const_iterator i= mat_vector::begin();i!=mat_vector::end();i++)
      retval.insert((*i)->getTag());
    return retval;
  }

//! @brief Return the identifiers of the materials in a python list.
template <class MAT>
boost::python::list MaterialVector<MAT>::getTagsPy(void) const
  {
    boost::python::list retval;
    std::set<int> tmp= getTags();
    for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
        retval.append(*i);
    return retval;
  }  

//! @brief Sends object through the communicator argument.
template <class MAT>
int MaterialVector<MAT>::sendSelf(Communicator &comm)
  {
    inicComm(2);
    int res= sendData(comm);
    const int dataTag=getDbTag();
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << dataTag << " failed to send ID"
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
template <class MAT>
int MaterialVector<MAT>::recvSelf(const Communicator &comm)
  {
    const int dataTag= this->getDbTag();
    inicComm(2);
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << dataTag << " failed to receive ID"
	        << Color::def << std::endl;
    else
      res+= recvData(comm);
    return res;
  }

} // end of XC namespace

#endif
