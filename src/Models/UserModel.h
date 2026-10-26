#ifndef USERMODEL_H
#define USERMODEL_H


#include <cstddef>

#include "Model.h"

/* MPM is a derived class that inherits from MaterialPointMethod_t */
template<typename MPM>
struct UserModel_t : public Model_t {
  protected:
  MPM _mpm;
  bool _is_fem_used = false;
  bool _is_fvm_used = false;
  
  MPM& GetInstanceOfMaterialPointMethod(){return _mpm;}
  bool& IsFEMUsed(){return _is_fem_used;}
  bool& IsFVMUsed(){return _is_fvm_used;}
  void UseFEM(){_is_fem_used = true;_is_fvm_used = false;}
  void UseFVM(){_is_fvm_used = true;_is_fem_used = false;}

  template<typename T>
  using Value_type = typename MPM::template Value_type<T>;
  
  template<typename T>
  using ImplicitValues_t = CustomValues_TypeOfImplicitValues(Value_type<T>);
  
  template<typename T>
  using ExplicitValues_t = CustomValues_TypeOfExplicitValues(Value_type<T>);
  
  template<typename T>
  using ConstantValues_t = CustomValues_TypeOfConstantValues(Value_type<T>);

  template <typename MemberType, typename T = double>
  std::size_t Values_Index(const MemberType Value_type<T>::* m) {
    Value_type<T>* cv;

    return(CustomValues_SafeIndex<Value_type<T>,MemberType,T>(cv,m));
  }
  
  public:
  using Model_t::Model_t;

  
  virtual int DefineElementProperties(Element_t* el,IntFcts_t* intfcts,ShapeFcts_t* shapefcts){
    if(IsFEMUsed()) {
      IntFct_t* intfct = Element_GetIntFct(el);
      int NbOfIntPoints = IntFct_GetNbOfPoints(intfct) + 1;
    
      _mpm.DefineNbOfInternalValues(el,NbOfIntPoints);
    }
    if(IsFVMUsed()) {
      unsigned int nn = Element_GetNbOfNodes(el);

      _mpm.DefineNbOfInternalValues(el,nn);
    }

    return(0);
  }
  
  virtual int ComputeInitialState(Element_t* el,double t){
    if(IsFEMUsed()) {
      return(this->_mpm.ComputeInitialStateByFEM(el,t));
    }
    if(IsFVMUsed()) {
      return(this->_mpm.ComputeInitialStateByFVM(el,t));
    }
    return(-1);
  }
  
  virtual int ComputeExplicitTerms(Element_t* el,double t){
    if(IsFEMUsed()) {
      return(this->_mpm.ComputeExplicitTermsByFEM(el,t));
    }
    if(IsFVMUsed()) {
      return(this->_mpm.ComputeExplicitTermsByFVM(el,t));
    }
    return(-1);
  }
  
  virtual int ComputeImplicitTerms(Element_t* el,double t,double dt){
    if(IsFEMUsed()) {
      return(this->_mpm.ComputeImplicitTermsByFEM(el,t,dt));
    }
    if(IsFVMUsed()) {
      return(this->_mpm.ComputeImplicitTermsByFVM(el,t,dt));
    }
    return(-1);
  }
  
  virtual int ComputeLoads(Element_t* el,double t,double dt,Load_t* cg,double* r){
    int ndof = Element_GetNbOfDOF(el);

    for(int i = 0 ; i < ndof ; i++) r[i] = 0.;

    if(IsFEMUsed()) {
      IntFct_t* fi = Element_GetIntFct(el) ;
      FEM_t *fem = FEM_GetInstance(el) ;
      double* r1 = FEM_ComputeSurfaceLoadResidu(fem,fi,cg,t,dt) ;
    
      for(int i = 0 ; i < ndof ; i++) r[i] = -r1[i] ;
    }
    if(IsFVMUsed()) {
      FVM_t* fvm = FVM_GetInstance(el) ;
      double* r1 = FVM_ComputeSurfaceLoadResidu(fvm,cg,t,dt) ;

      for(int i = 0 ; i < ndof ; i++) r[i] = -r1[i] ;
    }
  
    return(0) ;
  }
};

#endif