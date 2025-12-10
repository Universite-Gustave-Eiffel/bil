/* General features of the model:
 * Simple diffusion of solute (as sodium or potassium)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "CommonModel.h"

/* The Finite Volume Method */
#include "FVM.h"

#define TITLE   "Non linear Fick's second law (2015)"
#define AUTHORS ""

#include "PredefinedModelMethods.h"




/* Indices of equations/unknowns */
enum {
  E_SOLUTE   ,
  E_LAST
} ;


/* Nb of equations */
#define NbOfEquations      E_LAST
#define NEQ                NbOfEquations


/* Value of the nodal unknown (el must be used below) */
#define UNKNOWN(u,n,i)     Element_GetValueOfNodalUnknown(el,u,n,i)


/* Generic names of nodal unknowns */
#define U_SOLUTE(u,n)      UNKNOWN(u,n,E_SOLUTE)




/* Method chosen at compile time. 
 * Each equation is associated to a specific unknown.
 * Each unknown can deal with a specific model.
 * Uncomment/comment to let only one unknown per equation */

/* solute: unknown either C or logC */
#define U_C_solute
//#define U_LogC_solute
#if defined (U_LogC_solute)
  #define LogC_solute(u,n)    U_SOLUTE(u,n)
  #define C_solute(u,n)       (pow(10,LogC_solute(u,n)))
#elif defined (U_C_solute)
  #define C_solute(u,n)       U_SOLUTE(u,n)
  #define LogC_solute(u,n)    (log10(C_solute(u,n)))
#else
  #error "Ambiguous or undefined unknown"
#endif



#include "CustomValues.h"
#include "MaterialPointMethod.h"
#include "BaseName.h"



namespace BaseName() {
template<typename T>
struct ImplicitValues_t;

template<typename T>
struct ExplicitValues_t;

template<typename T>
struct ConstantValues_t;


template<typename T>
using Values_t = CustomValues_t<T,ImplicitValues_t,ExplicitValues_t,ConstantValues_t> ;

using Values_d = Values_t<double> ;

#define Values_Index(V)  CustomValues_Index(Values_d,V,double)


struct MPM_t: public MaterialPointMethod_t<Values_t> {
  MaterialPointMethod_SetInputs_t<Values_t> SetInputs;
  MaterialPointMethod_Integrate_t<Values_t> Integrate;
  MaterialPointMethod_Initialize_t<Values_t> Initialize;
  MaterialPointMethod_SetTangentMatrix_t<Values_t> SetTangentMatrix;
  MaterialPointMethod_SetFluxes_t<Values_t> SetFluxes;
  MaterialPointMethod_SetIndexOfPrimaryVariables_t SetIndexOfPrimaryVariables;
  MaterialPointMethod_SetIncrementOfPrimaryVariables_t SetIncrementOfPrimaryVariables;
} ;



/* Put here the implicit values to be saved */
template<typename T = double>
struct ImplicitValues_t {
  T U_solute;
  T Mole_solute;
  T MolarFlow_solute[Element_MaxNbOfNodes];
  T Concentration_solute;
  T Porosity;
} ;


/* Put here the explicit values to be saved */
template<typename T = double>
struct ExplicitValues_t {
  T EffectiveDiffusionCoefficient_solute;
} ;



/* Put here the constant values to be saved */
template<typename T = double>
struct ConstantValues_t {
};



/* Put here the parameters that are read from the input data file */
struct Parameters_t {
  double InitialPorosity;
  double MolecularDiffusionCoefficient_solute;
};

MPM_t mpm;
}

using namespace BaseName();




/* Math constants */
#define Ln10      Math_Ln10




/* Units
 * ----- */
#include "InternationalSystemOfUnits.h"
/* Shorthands of some units */
#define dm    (0.1*InternationalSystemOfUnits_OneMeter)
#define cm    (0.01*InternationalSystemOfUnits_OneMeter)
#define dm2   (dm*dm)
#define dm3   (dm*dm*dm)
#define cm3   (cm*cm*cm)
#define MPa   (1.e6*InternationalSystemOfUnits_OnePascal)
#define GPa   (1.e3*MPa)
#define mol   InternationalSystemOfUnits_OneMole
#define sec   InternationalSystemOfUnits_OneSecond
#define kg    InternationalSystemOfUnits_OneKilogram
#define gr    (0.001*kg)



#define TEMPERATURE  (298)




/* To retrieve the material properties */
#define GetProperty(a)                   (Element_GetProperty(el)[pm(a)])



/* Intern Functions */
static Model_ComputePropertyIndex_t  pm ;
static void    GetProperties(Element_t*,double) ;

static void    ComputePhysicoChemicalProperties(double) ;


static double TortuosityOhJang(double) ;
static double TortuosityBazantNajjar(double) ;

#define LiquidTortuosity  TortuosityOhJang
//#define LiquidTortuosity  TortuosityBazantNajjar


/* Intern variables */
//static double phii ;

//static double d_solute ;



#include "DiffusionCoefficientOfMoleculeInWater.h"



void ComputePhysicoChemicalProperties(double TK)
{
  /* Diffusion Coefficient Of Molecules In Water (m2/s) */
  
  //d_solute  = DiffusionCoefficientOfMoleculeInWater(Na,TK) ;
  
}



int pm(const char *s)
{
#define Parameters_Index(V)  CustomValues_Index(Parameters_t,V,double)
         if(strcmp(s,"porosity") == 0) {
    return (Parameters_Index(InitialPorosity)) ;
  } else if(strcmp(s,"d_solute") == 0) {
    return (Parameters_Index(MolecularDiffusionCoefficient_solute)) ;
  } else return(-1) ;
#undef Parameters_Index
}




void GetProperties(Element_t* el,double t)
{
  /* To retrieve the material properties */
  Parameters_t& param = ((Parameters_t*) Element_GetProperty(el))[0] ;
  
  //phii     = param.InitialPorosity;
  //d_solute = param.MolecularDiffusionCoefficient_solute;
}




int SetModelProp(Model_t* model)
{
  Model_GetNbOfEquations(model) = NEQ ;
  
  Model_CopyNameOfEquation(model,E_SOLUTE  ,"solute") ;
  
  
#if defined (U_LogC_solute)
  Model_CopyNameOfUnknown(model,E_SOLUTE ,"logc_na") ;
#elif defined (U_C_solute)
  Model_CopyNameOfUnknown(model,E_SOLUTE ,"c_na") ;
#else
  #error "Ambiguous or undefined unknown"
#endif


  Model_GetComputePropertyIndex(model) = &pm ;
  Model_GetComputeMaterialProperties(model) = &GetProperties;
  
  ComputePhysicoChemicalProperties(TEMPERATURE) ;
  
  return(0) ;
}




int ReadMatProp(Material_t* mat,DataFile_t* datafile)
/* Lecture des donnees materiaux dans le fichier ficd */
{
  int  NbOfProp = ((int) sizeof(Parameters_t)/sizeof(double)) ;
  
    
  /* Default initialization */
  {
    //ComputePhysicoChemicalProperties(TEMPERATURE) ;
    
    //Material_GetPropertyValue(mat,"d_solute") = d_solute ;
  }

  Material_ScanProperties(mat,datafile,pm) ;
  
  return(NbOfProp) ;
}



int PrintModelChar(Model_t* model,FILE *ficd)
/* Saisie des donnees materiaux */
{
  
  printf(TITLE) ;
  
  if(!ficd) return(NEQ) ;
  
  printf("\n") ;
  printf("The set of equations is:\n") ;
  printf("\t- Mass balance of solute     (solute)\n") ;
  
  printf("\n") ;
  printf("The primary unknowns are:\n") ;
  printf("\t- Solute concentration       (c_na)\n") ;
  
  printf("\n") ;
  printf("Example of input data\n") ;


  fprintf(ficd,"porosity = 0.38   # Porosity\n") ;

  return(NEQ) ;
}



int DefineElementProp(Element_t* el,IntFcts_t* intfcts)
{
  unsigned int nn = Element_GetNbOfNodes(el) ;
  
  mpm.DefineNbOfInternalValues(el,nn);
  
  return(0) ;
}



int  ComputeLoads(Element_t* el,double t,double dt,Load_t* cg,double* r)
/* Residu du aux chargements (r) */
{
  int nn = Element_GetNbOfNodes(el) ;
  FVM_t* fvm = FVM_GetInstance(el) ;
  int    i ;

  {
    double* r1 = FVM_ComputeSurfaceLoadResidu(fvm,cg,t,dt) ;
    
    for(i = 0 ; i < NEQ*nn ; i++) r[i] = -r1[i] ;
  }
  
  return(0) ;
}



int ComputeInitialState(Element_t* el)
/* Initialise les variables du systeme (f,va) */ 
{
  int i = mpm.ComputeInitialStateByFVM(el,t);
  
  return(i) ;
}



int  ComputeExplicitTerms(Element_t* el,double t)
/* Thermes explicites (va)  */
{
  int i = mpm.ComputeExplicitTermsByFVM(el,t);

  return(i) ;
}



int  ComputeImplicitTerms(Element_t* el,double t,double dt)
/* Les variables donnees par la loi de comportement (f_1) */
{
  int i = mpm.ComputeImplicitTermsByFVM(el,t,dt);

  return(i) ;
}



int  ComputeMatrix(Element_t* el,double t,double dt,double* k)
/* Matrice (k) */
{
  int i = mpm.ComputeMassConservationMatrixByFVM(el,t,dt,k);
  
  #define K(i,j)    (k[(i)*ndof + (j)])
  #if defined (U_LogC_solute)
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(i = 0 ; i < 2*NEQ ; i++){
      K(i,E_SOLUTE)     *= Ln10*C_solute(u,0) ;
      K(i,E_SOLUTE+NEQ) *= Ln10*C_solute(u,1) ;
    }
  }
  #endif
  #undef K

  return(i) ;
}



int  ComputeResidu(Element_t* el,double t,double dt,double* r)
/* Residu (r) */
{
  /* Initialization */
  {
    int ndof = Element_GetNbOfDOF(el) ;
    
    for(int i = 0 ; i < ndof ; i++) r[i] = 0. ;
  }
  
  /* Conservation of element Na: (N_solute - N_soluten) + dt * div(W_solute) = 0 */
  {
    int imass = Values_Index(Mole_solute);
    int iflow = Values_Index(MolarFlow_solute[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_SOLUTE,imass,iflow);
  }

  return(0) ;
}



int  ComputeOutputs(Element_t* el,double t,double* s,Result_t* r)
/* Les valeurs exploitees (s) */
{
  FVM_t* fvm = FVM_GetInstance(el) ;
  int    nso = 2 ;
  double** u = Element_ComputePointerToNodalUnknowns(el) ;

  if(Element_IsSubmanifold(el)) return(0) ;
  
  /*
    Input data
  */
  GetProperties(el,t) ;
  

  /* Initialization */
  for(int i = 0 ; i < nso ; i++) {
    Result_SetValuesToZero(r + i) ;
  }

  {
    int j = FVM_FindLocalCellIndex(fvm,s) ;
    Values_d& val = *mpm.OutputValues(el,t,j) ;
    /* Output quantities */
    int i = 0 ;
    
    Result_Store(r + i++,&val.Concentration_solute,"solute concentration",1) ;
    Result_Store(r + i++,&val.Mole_solute,"solute content",1) ;
    
    if(i != nso) arret("ComputeOutputs") ;
  }
  
  
  return(nso) ;
}



Values_d* MPM_t::SetInputs(Element_t* el,const double& t,const int& n,double const* const* u,Values_d& val)
  /** On input: (el,t,p,u,val)
   *  On output:
   *  val is initialized with the primary nodal unknowns (strain,pressure,temperature,etc...)
   * 
   *  Return a pointer to val
   */
{
  val.U_solute = C_solute(u,n) ;
  
  return(&val) ;
}


Values_d* MPM_t::Integrate(Element_t* el,const double& t,const double& dt,Values_d const& val_n,Values_d& val)
  /** On input: (el,t,dt,p,val_n,val) and val initialized with the primary unknowns
   *  On output:
   *  val is updated from the integration of the constitutive law from t-dt to t,
   *  i.e. the implicit and the explicit values.
   * 
   * 
   *  Return a pointer to val.
   **/
{
  /* Parameters */
  Parameters_t& par = ((Parameters_t*) Element_GetProperty(el))[0] ;
  double& phi0 = par.InitialPorosity;
  double& d_solute = par.MolecularDiffusionCoefficient_solute;
  
  /* Inputs */
  double c_solute  = val.U_solute ;

  /* Backup */
  
  /* Liquid components */
  val.Concentration_solute = c_solute ;
  
  /* Porosity */
  {
    double phi      = phi0 ;
    
    val.Porosity = phi;
  }
  
  /* Element contents */
  {
    double c_solute_l = c_solute ;
    double phi_l    = val.Porosity ;
    double n_solute_l = phi_l*c_solute_l ;
    
    val.Mole_solute  = n_solute_l ;
  }
  
  /*
    Transfer coefficients
  */
  {
    /* Diffusive transport in liquid phase */
    {
      /* tortuosity liquid */
      double phi    = val.Porosity;
      double tau    = LiquidTortuosity(phi) ;
 
      val.EffectiveDiffusionCoefficient_solute = d_solute*tau ;
    }
  }
  
  return(&val) ;
}



void  MPM_t::SetIndexOfPrimaryVariables(Element_t* el,int* ind)
  /** On input: (el,ind)
   *  ind = a pointer to an array of ncol integers
   *  ind[k] = index of the k^th unknown in the custom values struct
   *  ncol = nb of the tangent matrix columns
   * 
   *  On ouput:
   *  ind[k] are updated
   */
{
  ind[0] = Values_Index(U_solute);
}


void MPM_t::SetIncrementOfPrimaryVariables(Element_t* el,double* dui)
  /** On input: (el,dui)
   *  dui = a pointer to an array of ncol doubles
   *  ncol = nb of the tangent matrix columns
   *  dui[k] = arbitrary small increment of the k^th unknown used for 
   *           the numerical derivatives (see operator Differentiate)
   * 
   *  On ouput:
   *  dui[k] are updated
   */
{    
  ObVal_t* obval = Element_GetObjectiveValue(el) ;
    
  dui[0] =  1.e-2 * ObVal_GetValue(obval + 0) ;
}



int MPM_t::SetTangentMatrix(Element_t* el,double const& t,double const& dt,int const& i,Values_d const& val,Values_d const& dval,int const& k,double* c)
  /** On input: (el,t,dt,p,val,dval,k,c)
   *  k = the k^th column of the tangent matrix to be filled.
   *  dval = the derivatives of val wrt the k^th primary unknown.
   *  c = pointer to the matrix to be partially filled for the components of the column k and
   *  possibly for any other column if their components don't depend on the derivatives of val.
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
{
  int nn = Element_GetNbOfNodes(el) ;
  FVM_t* fvm   = FVM_GetInstance(el) ;
  double* dist = FVM_ComputeIntercellDistances(fvm) ;
  int    dec = NEQ*NEQ ;


  {        
    for(int j = 0 ; j < nn ; j++) {
      double* cij = c + (i*nn + j)*NEQ*NEQ ;
      double dij  = dist[nn*i + j] ;
      double dtdij = dt/dij ;
        
      /* Content terms at node i */
      if(j == i) {
        cij[E_SOLUTE*NEQ   + k] = dval.Mole_solute ;
      }

      /* Transfer terms from node i to node j: d(wij)/d(ui_k) */
      if(j != i) {  
        cij[E_SOLUTE*NEQ   + k] = - dtdij*dval.MolarFlow_solute[j] ;
      }
    }
  }

  return(dec) ;
}



Values_d*  MPM_t::SetFluxes(Element_t* el,double const& t,int const& i,int const& j,Values_d const& grdval,Values_d* val)
  /** On input: (el,t,i,j,grdval,val)
   *  i,j = node numbers from which and to which fluxes are computed.
   *  grdval = value gradients
   * 
   *  On output:
   *  val[i] and val[j] are updated for the fluxes between i and j.
   * 
   *  Return a pointer to val.
   */
{
  Values_d& vali = val[i];
  Values_d& valj = val[j];
  CustomValues_t<double,ExplicitValues_t> valij ;
  
  {
    double* va = Element_GetExplicitTerm(el) ;
    CustomValues_t<double,ExplicitValues_t>* vale = (CustomValues_t<double,ExplicitValues_t>*) va ;
    
    valij = 0.5 * (vale[i] + vale[j]);
  }
  
  /* Transport in liquid phase */
  {
    /* Diffusion */
    {
      /* Gradients */
      double grd_solute = grdval.Concentration_solute;
      
      /* Transfer terms */
      double kf = valij.EffectiveDiffusionCoefficient_solute ;
      
      /* Flux */
      double w_solute = - kf * grd_solute;
    
      /* Backup */
      vali.MolarFlow_solute[j]    = w_solute;
    }
  }
  
  {
    valj.MolarFlow_solute[i]    = - vali.MolarFlow_solute[j] ;
  }
    
  return(val + i) ;
}


Values_d*  MPM_t::Initialize(Element_t* el,double const& t,Values_d& val)
  /** On input: (el,t,val) with val initialized with the primary nodal unknowns.
   *  On output:
   *  val is fully initialized, including the implicit, the explicit and the constant values.
   * 
   *  Return a pointer to val.
   */
{
  return(&val);
}






double TortuosityOhJang(double phi)
/* Ref:
 * Byung Hwan Oh, Seung Yup Jang, 
 * Prediction of diffusivity of concrete based on simple analytic equations, 
 * Cement and Concrete Research 34 (2004) 463 - 480.
 * 
 * tau = tau_paste * tau_agg
 * 
 * tau_agg   = ratio of diffusivity in concrete and diffusivity in matrix (cement paste)
 * tau_paste = ratio of diffusivity in cement paste and diffusivity in liquid bulk
 * 
 * tau_paste = (m_p + sqrt(m_p**2 + phi_c/(1 - phi_c) * (Ds/D0)**(1/n)))**n
 * m_p = 0.5 * ((phi_cap - phi_c) + (Ds/D0)**(1/n) * (1 - phi_c - phi_cap)) / (1 - phi_c)
 * phi_cap = capillary porosity of the cement paste 
 * 
 * tau_agg = 1 + V_a/(1/(2*D_i*eps - 1) + (1 - V_a)/3)
 * V_a = Aggregate volume fraction
 * D_i = Diffusivity of the ITZ relative to that of cement paste 
 * eps = thickness ratio of ITZ: t/r_a 
 * t   = thickness of the ITZ
 * r_a = radius of the aggregate 
 * D_i = 7 
 * eps = 0.002 (Concrete)  ; 0.02 (Mortar) 
 * V_a = 0.67  (Concrete)  ; 0.45 (Mortar) 
 * tau_agg = 0.27 (Concrete) ; 0.63 (Mortar)
 */
{
  double v_a     = 0.5 ;
  double phi_cap = (phi > 0) ? (1 - v_a) * phi : 0  ;
  double phi_c = 0.18 ;          /* Critical porosity */
  double n     = 2.7 ;           /* n  = 2.7   (OPC) ; 4.5  (OPC + 10% Silica fume) */
  double ds    = 2.e-4 ;         /* ds = 2.e-4 (OPC) ; 5e-5 (OPC + 10% Silica fume) */
  double dsn   = pow(ds,1/n) ;
  double m_phi = 0.5 * ((phi_cap - phi_c) + dsn * (1 - phi_c - phi_cap)) / (1 - phi_c) ;
  double tau_paste = pow(m_phi + sqrt(m_phi*m_phi + dsn * phi_c/(1 - phi_c)),n) ;
  double tau_agg = 0.27 ;
  double tausat =  tau_paste * tau_agg ;
  
  double tau =  tausat ;
    
  return(tau) ;
}



double TortuosityBazantNajjar(double phi)
/** Liquid totuosity according to Bazant and Najjar */
{
  double iff = (phi < 0.8) ? 2.9e-4*exp(9.95*phi) : phi ;
    
  return(iff) ;
}
