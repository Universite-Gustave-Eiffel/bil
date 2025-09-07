#ifndef MATERIALPOINTMETHOD_H
#define MATERIALPOINTMETHOD_H

#include "Element.h"
#include "CustomValues.h"
#include "ConstitutiveIntegrator.h"

template<template<typename> typename V>
using MaterialPointMethod_SetInputs_t = V<double>* (Element_t*,double const&,int const&,double const* const*,V<double>&);

template<template<typename> typename V,typename T = double>
using MaterialPointMethod_Integrate_t = V<T>* (Element_t*,double const&,double const&,V<double> const&,V<T>&) ;

template<template<typename> typename V>
using MaterialPointMethod_Initialize_t = V<double>* (Element_t*,double const&,V<double>&);

template<template<typename> typename V>
using MaterialPointMethod_SetTangentMatrix_t = int (Element_t*,double const&,double const&,int const&,V<double> const&,V<double> const&,int const&,double*);

template<template<typename> typename V>
using MaterialPointMethod_SetTransferMatrix_t = int (Element_t*,double const&,int const&,V<double> const&,double*);

template<template<typename> typename V>
using MaterialPointMethod_SetFluxes_t = V<double>* (Element_t*,double const&,int const&,int const&,V<double> const&,V<double>*);

using MaterialPointMethod_SetIndexOfPrimaryVariables_t = void (Element_t*,int*);

using MaterialPointMethod_SetIncrementOfPrimaryVariables_t = void (Element_t*,double*);





template<template<typename> typename V,typename MPM>
struct MaterialPointMethod_t {
  private:
  //static constexpr std::size_t n_mpm = sizeof...(MPM);
  //using D = std::tuple_element_t<0, std::tuple<MPM...>>;
  //using D = std::conditional_t<sizeof...(MPM) == 1,std::tuple_element_t<0, std::tuple<MPM...>>,MaterialPointMethod_t<V>>;
  using D = MPM;
  
  public:
  template<typename T>
  using Value_type = V<T>;
  
  //MaterialPointMethod_t() {}
  virtual ~MaterialPointMethod_t() {}
  
  /** Explanation of the parameters common to all virtual functions.
   *  Element_t* el          = pointer to element object
   *  double const t         = current time
   *  double const dt        = time increment (i.e. the previous time is t-dt)
   *  double const* const* u = pointer to pointer to primary nodal unknowns
   *  int const p            = p^th interpolation Gauss point of the FE
   *  V<T> val               = custom values at the current time
   *  V<double> val_n        = custom values at the previous time (always an input)
   */
  //template<std::enable_if_t<(sizeof...(MPM) == 1),bool>>
  V<double>* SetInputs(Element_t* el,double const& t,int const& p,double const* const* u,V<double>& val){
    return(static_cast<D*>(this)->SetInputs(el,t,p,u,val));}
  //template<std::enable_if_t<(sizeof...(MPM) == 0),bool>>
  //V<double>* SetInputs(Element_t* el,double const& t,int const& p,double const* const* u,V<double>& val){return(NULL);}
  /** On input: (el,t,p,u,val)
   *  On output:
   *  val is initialized with the primary nodal unknowns (strain,pressure,temperature,etc...)
   * 
   *  Return a pointer to val
   */

  template<typename T>
  V<T>* Integrate(Element_t* el,double const& t,double const& dt,V<double> const& val_n,V<T>& val){
    return(static_cast<D*>(this)->Integrate(el,t,dt,val_n,val));}
  /** On input: (el,t,dt,p,val_n,val)
   *  On output:
   *  val is updated from the integration of the constitutive law from t-dt to t.
   * 
   *  Return a pointer to val.
   **/

  virtual V<double>* Initialize(Element_t*,double const&,V<double>&){return(NULL);}
  /** On input: (el,t,val)
   *  On output:
   *  val is initialized.
   * 
   *  Return a pointer to val.
   */

  int SetTangentMatrix(Element_t* el,double const& t,double const& dt,int const& p,V<double> const& val,V<double> const& dval,int const& k,double* c){
    return(static_cast<D*>(this)->SetTangentMatrix(el,t,dt,p,val,dval,k,c));}
  /** On input: (el,t,dt,p,val,dval,k,c)
   *  k = the k^th column of the tangent matrix to be filled.
   *  dval = the derivatives of val wrt the k^th primary unknown.
   *  c = pointer to the matrix to be partially filled (only the column k).
   * 
   *  On output:
   *  c is updated.
   * 
   *  Return the shift (the size of the matrix: ncols*nrows) if succeeds 
   *  or < 0 if fails.
   *  Exemples:
   *    - for an elastic matrix, shift = 81
   *    - for a poroelastic matrix, shift = 100
   */

  virtual int SetTransferMatrix(Element_t*,double const&,int const&,V<double> const&,double*){return(-1);}
  /** On input: (el,dt,p,val,c)
   *  c = pointer to the transfer matrix to be filled
   * 
   *  On output:
   *  c is updated.
   * 
   *  Return the shift (the size of the matrix: ncols*nrows)
   *  Example: if there are ndif diffusion process, shift = 9*ndif*ndif
   */

  virtual V<double>* SetFluxes(Element_t*,double const&,int const&,int const&,V<double> const&,V<double>*){return(NULL);}
  /** On input: (el,t,i,j,grdval,val)
   *  i,j = node numbers from which and to which flux is computed.
   *  grdval = value gradient
   * 
   *  On output:
   *  val[i] and val[j] are updated.
   * 
   *  Return a pointer to val.
   */

  virtual void SetIndexOfPrimaryVariables(Element_t*,int*){}
  /** On input: (el,ind)
   *  ind = a pointer to an array of ncol integers
   *  ind[k] = index of the k^th unknown in the custom values struct
   *  ncol = nb of the tangent matrix columns
   * 
   *  On ouput:
   *  ind[k] are updated
   */

  virtual void SetIncrementOfPrimaryVariables(Element_t*,double*){}
  /** On input: (el,dui)
   *  dui = a pointer to an array of ncol doubles
   *  ncol = nb of the tangent matrix columns
   *  dui[k] = arbitrary small increment of the k^th unknown used for 
   *           the numerical derivatives (see operator Differentiate)
   * 
   *  On ouput:
   *  dui[k] are updated
   */

  void DefineNbOfInternalValues(Element_t* el,int const N) {
    int const nvi = CustomValues_NbOfImplicitValues(V<double>);
    int const nve = CustomValues_NbOfExplicitValues(V<double>);
    int const nv0 = CustomValues_NbOfConstantValues(V<double>);
          
    Element_SetNbOfImplicitTerms(el,N*nvi) ;
    Element_SetNbOfExplicitTerms(el,N*nve) ;
    Element_SetNbOfConstantTerms(el,N*nv0) ;
  }


  V<double>* InitializeValues(Element_t* el,double const t,int const i) {
    double* vi = Element_GetImplicitTerm(el) ;
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    ci.Set(el,t,0,u,vi,u,vi) ;
    Element_ComputeMaterialProperties(el,t) ;
    
    return(ci.InitializeValues(i));
  }


  V<double>* OutputValues(Element_t* el,double const t,int const i) {
    double* vi = Element_GetImplicitTerm(el) ;
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    ci.Set(el,t,0,u,vi,u,vi) ;
    Element_ComputeMaterialProperties(el,t) ;
    
    return(ci.IntegrateValues(i));
  }

        
  int ComputeInitialStateByFEM(Element_t* el,double const t) {
    int i;
    double* vi = Element_GetImplicitTerm(el) ;
    double** u = Element_ComputePointerToCurrentNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0) ;
    ci.Set(el,t,0,u,vi,u,vi) ;
    Element_ComputeMaterialProperties(el,t) ;
    i = ci.ComputeInitialStateByFEM();
          
    return(i);
  }
  
        
  int ComputeInitialStateByFVM(Element_t* el,double const t) {
    int i;
    double* vi = Element_GetImplicitTerm(el) ;
    double** u = Element_ComputePointerToCurrentNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0) ;
    ci.Set(el,t,0,u,vi,u,vi) ;
    Element_ComputeMaterialProperties(el,t) ;
    i = ci.ComputeInitialStateByFVM();
    
    return(i);
  }


  int ComputeExplicitTermsByFEM(Element_t* el,double const t) {
    int i;
    double* vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0) ;
    ci.Set(el,t,0,u_n,vi_n,u_n,vi_n) ;
    Element_ComputeMaterialProperties(el,t) ;
    i = ci.ComputeExplicitTermsByFEM();
    
    return(i);
  }


  int ComputeExplicitTermsByFVM(Element_t* el,double const t) {
    int i;
    double* vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0) ;
    ci.Set(el,t,0,u_n,vi_n,u_n,vi_n) ;
    Element_ComputeMaterialProperties(el,t) ;
    i = ci.ComputeExplicitTermsByFVM();
    
    return(i);
  }


  int ComputeImplicitTermsByFEM(Element_t* el,double const t,double const dt) {
    int i;
    double* vi    = Element_GetCurrentImplicitTerm(el);
    double* vi_n  = Element_GetPreviousImplicitTerm(el);
    double** u   = Element_ComputePointerToCurrentNodalUnknowns(el);
    double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el);
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0);
    ci.Set(el,t,dt,u_n,vi_n,u,vi);
    Element_ComputeMaterialProperties(el,t);
    i = ci.ComputeImplicitTermsByFEM();
    
    return(i);
  }


  int ComputeImplicitTermsByFVM(Element_t* el,double const t,double const dt) {
    int k;
    double* vi    = Element_GetCurrentImplicitTerm(el);
    double* vi_n  = Element_GetPreviousImplicitTerm(el);
    double** u   = Element_ComputePointerToCurrentNodalUnknowns(el);
    double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el);
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0);
    ci.Set(el,t,dt,u_n,vi_n,u,vi);
    Element_ComputeMaterialProperties(el,t);
    k = ci.ComputeImplicitTermsByFVM();
    
    return(k);
  }


  int ComputeTangentStiffnessMatrixByFEM(Element_t* el,double const t,double const dt,double* k) {
    double*  vi   = Element_GetCurrentImplicitTerm(el) ;
    double*  vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u    = Element_ComputePointerToCurrentNodalUnknowns(el) ;
    double** u_n  = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    int ndof = Element_GetNbOfDOF(el);
    ConstitutiveIntegrator_t<D> ci(this);
    
    for(int i = 0 ; i < ndof*ndof ; i++) k[i] = 0;
    
    if(Element_IsSubmanifold(el)) return(0) ;
    Element_ComputeMaterialProperties(el,t) ;
    ci.Set(el,t,dt,u_n,vi_n,u,vi) ;
    
    {
      double* kp = ci.ComputeTangentStiffnessMatrixByFEM();
      
      if(!kp) return(1);
      
      for(int i = 0 ; i < ndof*ndof ; i++) k[i] = kp[i] ;
    }
    
    return(0);
  }


  int ComputePoromechanicalMatrixByFEM(Element_t* el,double const t,double const dt,double* k,int const e_mech) {
    double*  vi   = Element_GetCurrentImplicitTerm(el) ;
    double*  vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u    = Element_ComputePointerToCurrentNodalUnknowns(el) ;
    double** u_n  = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    int ndof = Element_GetNbOfDOF(el);
    ConstitutiveIntegrator_t<D> ci(this);
    
    for(int i = 0 ; i < ndof*ndof ; i++) k[i] = 0;
    
    if(Element_IsSubmanifold(el)) return(0) ;
    Element_ComputeMaterialProperties(el,t) ;
    ci.Set(el,t,dt,u_n,vi_n,u,vi) ;
    
    {
      #ifdef USE_AUTODIFF
      double* kp = ci.ComputeAutodiffPoromechanicalMatrixByFEM(e_mech);
      #else
      double* kp = ci.ComputePoromechanicalMatrixByFEM(e_mech);
      #endif
      
      if(!kp) return(1);
      
      for(int i = 0 ; i < ndof*ndof ; i++) k[i] = kp[i] ;
    }
    
    return(0);
  }

        
  int ComputeMassConservationMatrixByFEM(Element_t* el,double const t,double const dt,double* k) {
    double* vi   = Element_GetCurrentImplicitTerm(el) ;
    double* vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u   = Element_ComputePointerToNodalUnknowns(el) ;
    double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    int ndof = Element_GetNbOfDOF(el);
    ConstitutiveIntegrator_t<D> ci(this);
    
    for(int i = 0 ; i < ndof*ndof ; i++) k[i] = 0;
    
    if(Element_IsSubmanifold(el)) return(0) ;
    Element_ComputeMaterialProperties(el,t) ;
    ci.Set(el,t,dt,u_n,vi_n,u,vi) ;
    
    {
      #ifdef USE_AUTODIFF
      double* km = ci.ComputeAutodiffMassConservationMatrixByFEM();
      #else
      double* km = ci.ComputeMassConservationMatrixByFEM();
      #endif
      
      if(!km) return(1);
      
      for(int i = 0 ; i < ndof*ndof ; i++) k[i] = km[i] ;
    }
    
    return(0);
  }


  int ComputeMassConservationMatrixByFVM(Element_t* el,double const t,double const dt,double* k) {
    double* vi   = Element_GetCurrentImplicitTerm(el) ;
    double* vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u   = Element_ComputePointerToNodalUnknowns(el) ;
    double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    int ndof = Element_GetNbOfDOF(el);
    ConstitutiveIntegrator_t<D> ci(this);
    
    for(int i = 0 ; i < ndof*ndof ; i++) k[i] = 0;
    
    if(Element_IsSubmanifold(el)) return(0) ;
    Element_ComputeMaterialProperties(el,t) ;
    ci.Set(el,t,dt,u_n,vi_n,u,vi) ;
    
    {
      #ifdef USE_AUTODIFF
      double* km = ci.ComputeAutodiffMassConservationMatrixByFVM();
      #else
      double* km = ci.ComputeMassConservationMatrixByFVM();
      #endif
      
      if(!km) return(1);
      
      for(int i = 0 ; i < ndof*ndof ; i++) k[i] = km[i] ;
    }
    
    return(0);
  }


  int ComputeMechanicalEquilibriumResiduByFEM(Element_t* el,double const t,double const dt,double* r,int const e_mech,int const istress,int const ibforce) {
    double* vi   = Element_GetCurrentImplicitTerm(el) ;
    double* vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u   = Element_ComputePointerToCurrentNodalUnknowns(el) ;
    double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0) ;
    ci.Set(el,t,dt,u_n,vi_n,u,vi) ;
    
    {
      double* rw = ci.ComputeMechanicalEquilibiumResiduByFEM(istress,ibforce);
      int nn = Element_GetNbOfNodes(el) ;
      int dim = Element_GetDimensionOfSpace(el) ;
      int neq = Element_GetNbOfEquations(el);
      
      if(!rw) return(1);
      
      for(int i = 0 ; i < nn ; i++) {
        for(int j = 0 ; j < dim ; j++) {
          r[(i)*neq + (e_mech + j)] -= rw[i*dim + j] ;
        }
      }
    }
    
    return(0);
  }


  int ComputeMassConservationResiduByFEM(Element_t* el,double const t,double const dt,double* r,int const e_mass,int const imass,int const iflow) {
    double* vi   = Element_GetCurrentImplicitTerm(el) ;
    double* vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u     = Element_ComputePointerToCurrentNodalUnknowns(el) ;
    double** u_n   = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0) ;
    ci.Set(el,t,dt,u_n,vi_n,u,vi) ;
    
    {
      double* ra =  ci.ComputeMassConservationResiduByFEM(imass,iflow);
      int nn = Element_GetNbOfNodes(el) ;
      int neq = Element_GetNbOfEquations(el);
      
      if(!ra) return(1);
      
      for(int i = 0 ; i < nn ; i++) {
        r[(i)*neq + (e_mass)] -= ra[i] ;
      }
    }
    
    return(0);
  }


  int ComputeMassConservationResiduByFVM(Element_t* el,double const t,double const dt,double* r,int const e_mass,int const imass,int const iflow) {
    double* vi   = Element_GetCurrentImplicitTerm(el) ;
    double* vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u     = Element_ComputePointerToCurrentNodalUnknowns(el) ;
    double** u_n   = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0) ;
    ci.Set(el,t,dt,u_n,vi_n,u,vi) ;
    
    {
      double* ra =  ci.ComputeMassConservationResiduByFVM(imass,iflow);
      int nn = Element_GetNbOfNodes(el) ;
      int neq = Element_GetNbOfEquations(el);
      
      if(!ra) return(1);
      
      for(int i = 0 ; i < nn ; i++) {
        r[(i)*neq + (e_mass)] -= ra[i] ;
      }
    }
    
    return(0);
  }


  int ComputeFluxResiduByFVM(Element_t* el,double const t,double const dt,double* r,int const e_mass,int const iflow) {
    double* vi   = Element_GetCurrentImplicitTerm(el) ;
    double* vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u     = Element_ComputePointerToCurrentNodalUnknowns(el) ;
    double** u_n   = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0) ;
    ci.Set(el,t,dt,u_n,vi_n,u,vi) ;
    
    {
      double* ra =  ci.ComputeFluxResiduByFVM(iflow);
      int nn = Element_GetNbOfNodes(el) ;
      int neq = Element_GetNbOfEquations(el);
      
      if(!ra) return(1);
      
      for(int i = 0 ; i < nn ; i++) {
        r[(i)*neq + (e_mass)] -= ra[i] ;
      }
    }
    
    return(0);
  }


  int ComputeBodyForceResiduByFVM(Element_t* el,double const t,double const dt,double* r,int const e_mass,int const imass) {
    double* vi   = Element_GetCurrentImplicitTerm(el) ;
    double* vi_n = Element_GetPreviousImplicitTerm(el) ;
    double** u     = Element_ComputePointerToCurrentNodalUnknowns(el) ;
    double** u_n   = Element_ComputePointerToPreviousNodalUnknowns(el) ;
    ConstitutiveIntegrator_t<D> ci(this);
    
    if(Element_IsSubmanifold(el)) return(0) ;
    ci.Set(el,t,dt,u_n,vi_n,u,vi) ;
    
    {
      double* ra =  ci.ComputeBodyForceResiduByFVM(imass);
      int nn = Element_GetNbOfNodes(el) ;
      int neq = Element_GetNbOfEquations(el);
      
      if(!ra) return(1);
      
      for(int i = 0 ; i < nn ; i++) {
        r[(i)*neq + (e_mass)] -= ra[i] ;
      }
    }
    
    return(0);
  }
};

#endif
