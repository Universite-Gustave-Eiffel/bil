#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "Context.h"
#include "CommonModel.h"
#include "FEM.h"
#include "Plasticity.h"

#define TITLE "Barcelona Expansive Model for unsaturated soils (2023)"
#define AUTHORS "Eizaguirre-Dangla"

#include "PredefinedMethods.h"


/* Nb of equations */
#define NEQ     (1+dim)

/* Equation index */
#define E_liq   (0)
#define E_mec   (1)

/* Unknown index */
#define U_p_l   (0)
#define U_u     (1)


/* We define some names for implicit terms */
#define NVI     (32)  /* Nb of implicit terms */
#define M_L           (vim   + 0)[0]
#define M_L_n         (vim_n + 0)[0]
#define W_L           (vim   + 1)

#define SIG           (vim   + 4)
#define SIG_n         (vim_n + 4)

#define F_MASS        (vim   + 13)

#define EPS_P         (vim   + 16)
#define EPS_P_n       (vim_n + 16)

#define HARDV         (vim   + 25)
#define HARDV_n       (vim_n + 25)

#define CRIT          (vim   + 28)
#define DLAMBDA       (vim   + 30)


/* We define some names for explicit terms */
#define NVE     (1)  /* Nb of explicit terms */
#define K_L           (vex + 0)[0]

/* We define some names for constant terms */
#define NV0     (0)  /* Nb of constant terms */


/* Functions */
static int    pm(const char *s) ;
static void   GetProperties(Element_t*) ;

static int    ComputeTangentCoefficients(Element_t*,double,double,double*) ;
static int    ComputeTransferCoefficients(FEM_t*,double,double*) ;

static double* ComputeVariables(Element_t*,void*,void*,void*,const double,const double,const int);
static void  ComputeSecondaryVariables(Element_t*,double,double,double*,double*) ;
static double* ComputeVariableDerivatives(Element_t*,double,double,double*,double,int) ;



#define ComputeTangentStiffnessTensor(...)  Plasticity_ComputeTangentStiffnessTensor(plasty,__VA_ARGS__)
#define ReturnMapping(...)                  Plasticity_ReturnMapping(plasty,__VA_ARGS__)
#define CopyElasticTensor(...)              Plasticity_CopyElasticTensor(plasty,__VA_ARGS__)
#define UpdateElastoplasticTensor(...)      Plasticity_UpdateElastoplasticTensor(plasty,__VA_ARGS__)
#define CopyTangentStiffnessTensor(...)     Plasticity_CopyTangentStiffnessTensor(plasty,__VA_ARGS__)


/* Material properties */
#define SATURATION_CURVE        (saturationcurve)
#define SaturationDegree(pc)    (Curve_ComputeValue(SATURATION_CURVE,pc))
#define dSaturationDegree(pc)   (Curve_ComputeDerivative(SATURATION_CURVE,pc))

#define RELATIVEPERM_CURVE                (relativepermcurve)
#define RelativePermeabilityToLiquid(pc)  (Curve_ComputeValue(RELATIVEPERM_CURVE,pc))

#define MacroElasticStiffnessSuctionChanges(p)  (Curve_ComputeValue(kappascurve,p))





/* Parameters */
static double  gravity ;
static double  rho_s ;
static double* sig0 ;
static double  rho_l0 ;
static double  p_g = 0 ;
static double  k_int ;
static double  mu_l ;
static double  kappa ;
static double  kappa_m ;
static double  poisson ;
static double  eM0 ;
static double  em0 ;
static double  e0 ;
static double  phiM0 ;
static double  phim0 ;
static double  phi0 ;
static double  alpha ;
static double  hardv0[3] ;
static Elasticity_t* elasty ;
static Plasticity_t* plasty ;
static Curve_t* saturationcurve ;
static Curve_t* relativepermcurve ;
static Curve_t* kappascurve ;
static double  p_atm = 101325. ;



/* Variable indexes */
enum {
  I_U  = 0,
  I_U2 = I_U + 2,
  I_P_L,
  I_EPS,
  I_EPS8 = I_EPS + 8,
  I_SIG,
  I_SIG8 = I_SIG + 8,
  I_EPS_P,
  I_EPS_P8 = I_EPS_P + 8,
  I_Fmass,
  I_Fmass2 = I_Fmass + 2,
  I_M_L,
  I_W_L,
  I_W_L2 = I_W_L + 2,
  I_HARDV,
  I_HARDV2 = I_HARDV + 2,
  I_CRIT,
  I_CRIT2 = I_CRIT + 1,
  I_RHO_L,
  I_PHI,
  I_K_L,
  I_GRD_P_L,
  I_GRD_P_L2 = I_GRD_P_L + 2,
  I_DLAMBDA,
  I_DLAMBDA2 = I_DLAMBDA + 1,
  I_Last
} ;

#define NbOfVariables     (I_Last)
static double  Variable[NbOfVariables] ;
static double  Variable_n[NbOfVariables] ;
static double dVariable[NbOfVariables] ;



int pm(const char *s)
{
         if(!strcmp(s,"gravity"))    { 
    return (0) ;
  } else if(!strcmp(s,"rho_s"))      { 
    return (1) ;
  } else if(!strcmp(s,"poisson_ratio")) { 
    return (2) ;
  } else if(!strcmp(s,"rho_l"))      { 
    return (3) ;
  } else if(!strcmp(s,"k_int"))      { 
    return (4) ;
  } else if(!strcmp(s,"mu_l"))       { 
    return (5) ;
  } else if(!strcmp(s,"p_l0"))       {
    return (6) ;
  } else if(!strcmp(s,"initial_stress"))       {
    return(7) ;
  } else if(!strncmp(s,"initial_stress_",15))   {
    int i = (strlen(s) > 15) ? s[15] - '1' : 0 ;
    int j = (strlen(s) > 16) ? s[16] - '1' : 0 ;
    
    return(7 + 3*i + j) ;
    
    /* BBM */
  } else if(!strcmp(s,"macro_elastic_stiffness_pressure_changes")) {
    return(16) ;
  } else if(!strcmp(s,"slope_of_virgin_consolidation_line")) {
    return(17) ;
  } else if(!strcmp(s,"slope_of_critical_state_line"))  {
    return(18) ;
  } else if(!strcmp(s,"initial_pre-consolidation_pressure")) {
    return(19) ;
  } else if(!strcmp(s,"initial_macro_void_ratio")) {
    return(20) ;
  } else if(!strcmp(s,"suction_cohesion_coefficient")) {
    return(22) ;
  } else if(!strcmp(s,"reference_consolidation_pressure")) {
    return(23) ;
  } else if(!strcmp(s,"micro_elastic_stiffness_effective_pressure_changes")) {
    return(24) ;
  } else if(!strcmp(s,"alpha_microstructure")) {
    return(25) ;
  } else if(!strcmp(s,"initial_micro_void_ratio")) {
    return(26) ;
  } else if(!strcmp(s,"initial_suction_decrease_hardening")) {
    return(27) ;
  } else if(!strcmp(s,"initial_suction_increase_hardening")) {
    return(28) ;
  } else return(-1) ;
}


void GetProperties(Element_t* el)
{
  gravity = Element_GetPropertyValue(el,"gravity") ;
  rho_s   = Element_GetPropertyValue(el,"rho_s") ;
  k_int   = Element_GetPropertyValue(el,"k_int") ;
  mu_l    = Element_GetPropertyValue(el,"mu_l") ;
  rho_l0  = Element_GetPropertyValue(el,"rho_l") ;
  sig0    = &Element_GetPropertyValue(el,"initial_stress") ;
  kappa   = Element_GetPropertyValue(el,"macro_elastic_stiffness_pressure_changes") ;
  poisson = Element_GetPropertyValue(el,"poisson_ratio") ;
  eM0     = Element_GetPropertyValue(el,"initial_macro_void_ratio") ;
  em0     = Element_GetPropertyValue(el,"initial_micro_void_ratio") ;
  e0      = eM0 + em0 ;
  phi0    = e0/(1 + e0) ;
  phiM0   = eM0/(1 + e0) ;
  phim0   = em0/(1 + e0) ;
  alpha   = Element_GetPropertyValue(el,"alpha_microstructure") ;
  kappa_m = Element_GetPropertyValue(el,"micro_elastic_stiffness_effective_pressure_changes") ;
  
  plasty  = (Plasticity_t*) Element_FindMaterialData(el,"Plasticity") ;
  elasty  = Plasticity_GetElasticity(plasty) ;
  
  hardv0[0]  = Plasticity_GetHardeningVariable(plasty)[0] ;
  hardv0[1]  = Plasticity_GetHardeningVariable(plasty)[1] ;
  hardv0[2]  = Plasticity_GetHardeningVariable(plasty)[2] ;
  
  saturationcurve = Element_FindCurve(el,"sl") ;
  relativepermcurve = Element_FindCurve(el,"kl") ;
  kappascurve = Element_FindCurve(el,"kappa_s") ;
}



int SetModelProp(Model_t* model)
/** Set the model properties */
{
  int dim = Model_GetDimension(model) ;
  char name_eqn[3][7] = {"meca_1","meca_2","meca_3"} ;
  char name_unk[3][4] = {"u_1","u_2","u_3"} ;
  int i ;
  
  /** Number of equations to be solved */
  Model_GetNbOfEquations(model) = NEQ ;
  
  /** Names of these equations */
  Model_CopyNameOfEquation(model,E_liq,"liq") ;
  for(i = 0 ; i < dim ; i++) {
    Model_CopyNameOfEquation(model,E_mec + i,name_eqn[i]) ;
  }
  
  /** Names of the main (nodal) unknowns */
  Model_CopyNameOfUnknown(model,U_p_l,"p_l") ;
  for(i = 0 ; i < dim ; i++) {
    Model_CopyNameOfUnknown(model,U_u + i,name_unk[i]) ;
  }
  
  Model_GetComputePropertyIndex(model) = pm ;
    
  return(0) ;
}



int ReadMatProp(Material_t* mat,DataFile_t* datafile)
/** Read the material properties in the stream file ficd 
 *  Return the nb of (scalar) properties of the model */
{
  int  NbOfProp = 29 ;
  int i ;

  /* Par defaut tout a 0 */
  for(i = 0 ; i < NbOfProp ; i++) Material_GetProperty(mat)[i] = 0. ;
  
  Material_ScanProperties(mat,datafile,pm) ;
  
  
  /* Plasticity */
  {
    plasty = Plasticity_Create() ;
      
    Material_AppendData(mat,1,plasty,"Plasticity") ;
  }
  
  /* Elastic and plastic properties */
  {
    elasty = Plasticity_GetElasticity(plasty) ;
    
    {
      /* Elasticity */
      {
        //double young    = Material_GetPropertyValue(mat,"young") ;
        //double poisson  = Material_GetPropertyValue(mat,"poisson") ;
        
        Elasticity_SetToIsotropy(elasty) ;
        //Elasticity_SetParameters(elasty,young,poisson) ;
        //Elasticity_ComputeStiffnessTensor(elasty) ;
      }
    }
    {
      /* Barcelona Expansive model */
      {
        double lambda = Material_GetPropertyValue(mat,"slope_of_virgin_consolidation_line") ;
        double M      = Material_GetPropertyValue(mat,"slope_of_critical_state_line") ;
        double pc0    = Material_GetPropertyValue(mat,"initial_pre-consolidation_pressure") ;
        double coh    = Material_GetPropertyValue(mat,"suction_cohesion_coefficient") ;
        double p_ref  = Material_GetPropertyValue(mat,"reference_consolidation_pressure") ;
        double si  = Material_GetPropertyValue(mat,"initial_suction_increase_hardening") ;
        double sd  = Material_GetPropertyValue(mat,"initial_suction_decrease_hardening") ;
        Curve_t* lc   = Material_FindCurve(mat,"lc") ;
        Curve_t* fi   = Material_FindCurve(mat,"f_I") ;
        Curve_t* fd   = Material_FindCurve(mat,"f_D") ;
        Curve_t* sl   = Material_FindCurve(mat,"sl") ;
        
        eM0     = Material_GetPropertyValue(mat,"initial_macro_void_ratio") ;
        em0     = Material_GetPropertyValue(mat,"initial_micro_void_ratio") ;
        kappa  = Material_GetPropertyValue(mat,"macro_elastic_stiffness_pressure_changes") ;
        alpha  = Material_GetPropertyValue(mat,"alpha_microstructure") ;
        kappa_m = Material_GetPropertyValue(mat,"micro_elastic_stiffness_effective_pressure_changes") ;
        
        Plasticity_SetTo(plasty,BExM) ;
        Plasticity_SetParameters(plasty,kappa,lambda,M,pc0,eM0,em0,coh,p_ref,alpha,si,sd,kappa_m,lc,fi,fd,sl) ;
      }
    }

#if 0
    {      
      Elasticity_PrintStiffnessTensor(elasty) ;
    }
#endif
  }
  
  return(NbOfProp) ;
}


int PrintModelProp(Model_t* model,FILE *ficd)
/** Print the model properties 
 *  Return the nb of equations */
{
  printf(TITLE) ;
  printf("\n") ;
  
  if(!ficd) return(0) ;

  printf("\n\
The system consists in (1 + dim) equations\n\
\t 1. The mass balance equation for water (liq)\n\
\t 2. The equilibrium equation (meca_1,meca_2,meca_3)\n") ;

  printf("\n\
The primary unknowns are\n\
\t 1. The liquid pressure (p_l)\n\
\t 2. The displacement vector (u_1,u_2,u_3)\n") ;

  printf("\n\
Example of input data\n\n") ;
  

  fprintf(ficd,"gravity = 0       # gravity\n") ;
  fprintf(ficd,"rho_s = 2350      # mass density of solid skeleton\n") ;
  fprintf(ficd,"young = 5.8e+09   # Young's modulus\n") ;
  fprintf(ficd,"poisson = 0.3     # Poisson's ratio\n") ;
  fprintf(ficd,"porosity = 0.15   # porosity\n") ;
  fprintf(ficd,"rho_l = 1000      # mass density of fluid\n") ;
  fprintf(ficd,"p_g = 0           # gas pressure\n") ;
  fprintf(ficd,"k_int = 1e-19     # intrinsic permeability\n") ;
  fprintf(ficd,"mu_l = 0.001      # viscosity of liquid\n") ;
  fprintf(ficd,"sig0_ij = -11.5e6 # initial stress sig0_ij\n") ;
  fprintf(ficd,"Curves = my_file  # file name: p_c S_l k_rl\n") ;
  
  return(0) ;
}


int DefineElementProp(Element_t* el,IntFcts_t* intfcts)
/** Define some properties attached to each element 
 *  Return 0 */
{
  IntFct_t* intfct = Element_GetIntFct(el) ;
  int NbOfIntPoints = IntFct_GetNbOfPoints(intfct) + 1 ;

  /** Define the length of tables */
  Element_SetNbOfImplicitTerms(el,NVI*NbOfIntPoints) ;
  Element_SetNbOfExplicitTerms(el,NVE*NbOfIntPoints) ;
  Element_SetNbOfConstantTerms(el,NV0*NbOfIntPoints) ;
  return(0) ;
}



int  ComputeLoads(Element_t* el,double t,double dt,Load_t* cg,double* r)
/** Compute the residu (r) due to loads 
 *  Return 0 if succeeded and -1 if failed */
{
  int dim = Element_GetDimensionOfSpace(el) ;
  IntFct_t* fi = Element_GetIntFct(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  int ndof = nn*NEQ ;
  FEM_t* fem = FEM_GetInstance(el) ;
  int    i ;

  {
    double* r1 = FEM_ComputeSurfaceLoadResidu(fem,fi,cg,t,dt) ;
  
    /* hydraulic */
    if(Element_FindEquationPositionIndex(el,Load_GetNameOfEquation(cg)) == E_liq) {
      for(i = 0 ; i < ndof ; i++) r[i] = -r1[i] ;
      
    /* other */
    } else {
      for(i = 0 ; i < ndof ; i++) r[i] = r1[i] ;
    }
  }
  
  return(0) ;
}


int ComputeInitialState(Element_t* el)
{
  double* vim0  = Element_GetImplicitTerm(el) ;
  double* vex0  = Element_GetExplicitTerm(el) ;
  double** u   = Element_ComputePointerToNodalUnknowns(el) ;
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int NbOfIntPoints = IntFct_GetNbOfPoints(intfct) ;
  DataFile_t* datafile = Element_GetDataFile(el) ;
  int    p ;
  
  /* We skip if the element is a submanifold */
  if(Element_IsSubmanifold(el)) return(0) ;

  /*
    Input data
  */
  GetProperties(el) ;
  
  /* Pre-initialization */
  for(p = 0 ; p < NbOfIntPoints ; p++) {
    
    /* storage in vim */
    {
      double* vim  = vim0 + p*NVI ;
      int    i ;

      
      /* Initial stresses, hardening variables */
      if(DataFile_ContextIsPartialInitialization(datafile)) {
      } else {
        for(i = 0 ; i < 9 ; i++) SIG[i] = sig0[i] ;
        for(i = 0 ; i < 3 ; i++) HARDV[i] = hardv0[i] ;
      }
      
      for(i = 0 ; i < 9 ; i++) EPS_P[i]  = 0 ;
    }
  }
  
    
  /* Loop on integration points */
  for(p = 0 ; p < NbOfIntPoints ; p++) {
    /* Variables */
    double* x = ComputeVariables(el,u,u,vim0,0,0,p) ;
    
    /* storage in vim */
    {
      double* vim  = vim0 + p*NVI ;
      int    i ;
      
      M_L = x[I_M_L] ;
    
      for(i = 0 ; i < 3 ; i++) W_L[i] = x[I_W_L + i] ;
    
      for(i = 0 ; i < 9 ; i++) SIG[i] = x[I_SIG + i] ;
      
      for(i = 0 ; i < 3 ; i++) F_MASS[i] = x[I_Fmass + i] ;
      
      for(i = 0 ; i < 9 ; i++) EPS_P[i]  = x[I_EPS_P + i] ;
    
      for(i = 0 ; i < 2 ; i++) CRIT[i] = x[I_CRIT + i] ;
      
      for(i = 0 ; i < 3 ; i++) HARDV[i] = x[I_HARDV + i] ;
      
      for(i = 0 ; i < 2 ; i++) DLAMBDA[i] = x[I_DLAMBDA + i] ;
    }
    
    
    /* storage in vex */
    {
      double* vex  = vex0 + p*NVE ;
      double rho_l = x[I_RHO_L] ;
      double p_l = x[I_P_L] ;
      double pc = p_g - p_l ;
      double k_h = rho_l*k_int/mu_l*RelativePermeabilityToLiquid(pc) ;
    
      K_L = k_h ;
    }
  }
  
  return(0) ;
}


int  ComputeExplicitTerms(Element_t* el,double t)
/** Compute the explicit terms */
{
  double* vim0 = Element_GetPreviousImplicitTerm(el) ;
  double* vex0 = Element_GetExplicitTerm(el) ;
  double** u = Element_ComputePointerToPreviousNodalUnknowns(el) ;
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int NbOfIntPoints = IntFct_GetNbOfPoints(intfct) ;
  int    p ;
  
  /* We skip if the element is a submanifold */
  if(Element_IsSubmanifold(el)) return(0) ;

  /*
    Input data
  */
  GetProperties(el) ;

  /* Loop on integration points */
  for(p = 0 ; p < NbOfIntPoints ; p++) {
    /* Variables */
    double* x = ComputeVariables(el,u,u,vim0,t,0,p) ;
    
    /* fluid mass density */
    double rho_l = x[I_RHO_L] ;
    
    /* pressures */
    double p_l = x[I_P_L] ;
    double pc = p_g - p_l ;
    
    /* permeability */
    double k_h = rho_l*k_int/mu_l*RelativePermeabilityToLiquid(pc) ;
    
    /* storage in vex */
    {
      double* vex  = vex0 + p*NVE ;
      
      K_L = k_h ;
    }
  }
  
  return(0) ;
}



int  ComputeImplicitTerms(Element_t* el,double t,double dt)
{
  double* vim0  = Element_GetCurrentImplicitTerm(el) ;
  double* vim_n  = Element_GetPreviousImplicitTerm(el) ;
  double** u   = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el) ;
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int NbOfIntPoints = IntFct_GetNbOfPoints(intfct) ;
  int    p ;
  
  /* We skip if the element is a submanifold */
  if(Element_IsSubmanifold(el)) return(0) ;

  /*
    Input data
  */
  GetProperties(el) ;
  
    
  /* Loop on integration points */
  for(p = 0 ; p < NbOfIntPoints ; p++) {
    /* Variables */
    double* x = ComputeVariables(el,u,u_n,vim_n,t,dt,p) ;
    
    /* storage in vim */
    {
      double* vim  = vim0 + p*NVI ;
      int    i ;
      
      M_L = x[I_M_L] ;
    
      for(i = 0 ; i < 3 ; i++) W_L[i] = x[I_W_L + i] ;
    
      for(i = 0 ; i < 9 ; i++) SIG[i] = x[I_SIG + i] ;
      
      for(i = 0 ; i < 3 ; i++) F_MASS[i] = x[I_Fmass + i] ;
      
      for(i = 0 ; i < 9 ; i++) EPS_P[i]  = x[I_EPS_P + i] ;
    
      for(i = 0 ; i < 2 ; i++) CRIT[i] = x[I_CRIT + i] ;
      
      for(i = 0 ; i < 3 ; i++) HARDV[i] = x[I_HARDV + i] ;
      
      for(i = 0 ; i < 2 ; i++) DLAMBDA[i] = x[I_DLAMBDA + i] ;
    }
  }
  
  return(0) ;
}



int  ComputeMatrix(Element_t* el,double t,double dt,double* k)
/** Compute the matrix (k) */
{
#define K(i,j)    (k[(i)*ndof + (j)])
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int ndof = nn*NEQ ;
  FEM_t* fem = FEM_GetInstance(el) ;


  /* Initialization */
  {
    double zero = 0. ;
    int    i ;
    
    for(i = 0 ; i < ndof*ndof ; i++) k[i] = zero ;
  }


  /* We skip if the element is a submanifold */
  if(Element_IsSubmanifold(el)) return(0) ;
  
  
  /*
    Input data
  */
  GetProperties(el) ;


  /*
  ** Poromechanic matrix
  */
  {
    double c[IntFct_MaxNbOfIntPoints*100] ;
    int dec = ComputeTangentCoefficients(el,t,dt,c) ;
    double* kp = FEM_ComputePoroelasticMatrix(fem,intfct,c,dec,1,U_u) ;
    
    {
      int i ;
      
      for(i = 0 ; i < ndof*ndof ; i++) {
        k[i] = kp[i] ;
      }
    }
  }
  
  /*
  ** Conduction matrix
  */
  {
    double c[IntFct_MaxNbOfIntPoints*100] ;
    int dec = ComputeTransferCoefficients(fem,dt,c) ;
    double* kc = FEM_ComputeConductionMatrix(fem,intfct,c,dec) ;
    int    i ;
  
    for(i = 0 ; i < nn ; i++) {
      int    j ;
      
      for(j = 0 ; j < nn ; j++) {
        K(E_liq + i*NEQ,U_p_l + j*NEQ) += dt*kc[i*nn + j] ;
      }
    }
  }
  
  return(0) ;
#undef K
}




int  ComputeResidu(Element_t* el,double t,double dt,double* r)
/** Comput the residu (r) */
{
#define R(n,i)    (r[(n)*NEQ+(i)])
  double* vim_1 = Element_GetCurrentImplicitTerm(el) ;
  double* vim_n = Element_GetPreviousImplicitTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int np = IntFct_GetNbOfPoints(intfct) ;
  int ndof = nn*NEQ ;
  FEM_t* fem = FEM_GetInstance(el) ;
  int    i ;
  double zero = 0. ;

  /* Initialization */
  for(i = 0 ; i < ndof ; i++) r[i] = zero ;

  if(Element_IsSubmanifold(el)) return(0) ;


  /* 1. Mechanics */
  
  /* 1.1 Stresses */
  {
    double* vim = vim_1 ;
    double* rw = FEM_ComputeStrainWorkResidu(fem,intfct,SIG,NVI) ;
    
    for(i = 0 ; i < nn ; i++) {
      int j ;
      
      for(j = 0 ; j < dim ; j++) R(i,E_mec + j) -= rw[i*dim + j] ;
    }
    
  }
  
  /* 1.2 Body forces */
  {
    double* vim = vim_1 ;
    double* rbf = FEM_ComputeBodyForceResidu(fem,intfct,F_MASS + dim - 1,NVI) ;
    
    for(i = 0 ; i < nn ; i++) {
      R(i,E_mec + dim - 1) -= -rbf[i] ;
    }
    
  }
  
  
  /* 2. Hydraulics */
  
  /* 2.1 Accumulation Terms */
  {
    double* vim = vim_1 ;
    double g1[IntFct_MaxNbOfIntPoints] ;
    
    for(i = 0 ; i < np ; i++ , vim += NVI , vim_n += NVI) g1[i] = M_L - M_L_n ;
    
    {
      double* ra = FEM_ComputeBodyForceResidu(fem,intfct,g1,1) ;
    
      for(i = 0 ; i < nn ; i++) R(i,E_liq) -= ra[i] ;
    }
  }
  
  /* 2.2 Transport Terms */
  {
    double* vim = vim_1 ;
    double* rf = FEM_ComputeFluxResidu(fem,intfct,W_L,NVI) ;
    
    for(i = 0 ; i < nn ; i++) R(i,E_liq) -= -dt*rf[i] ;
  }
  
  return(0) ;
#undef R
}



int  ComputeOutputs(Element_t* el,double t,double* s,Result_t* r)
/** Compute the outputs (r) */
{
  int NbOfOutputs = 8 ;
  double* vex0  = Element_GetExplicitTerm(el) ;
  double* vim0  = Element_GetCurrentImplicitTerm(el) ;
  double** u   = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int np = IntFct_GetNbOfPoints(intfct) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  FEM_t* fem = FEM_GetInstance(el) ;

  if(Element_IsSubmanifold(el)) return(0) ;

  /* Initialization */
  {
    int    i ;
    
    for(i = 0 ; i < NbOfOutputs ; i++) {
      Result_SetValuesToZero(r + i) ;
    }
  }
  
  /*
    Input data
  */
  GetProperties(el) ;

  {
    /* Interpolation functions at s */
    double* a = Element_ComputeCoordinateInReferenceFrame(el,s) ;
    int p = IntFct_ComputeFunctionIndexAtPointOfReferenceFrame(intfct,a) ;
    /* Pressure */
    double p_l = FEM_ComputeUnknown(fem,u,intfct,p,U_p_l) ;
    double pc  = p_g - p_l ;
    /* saturation */
    double sl = SaturationDegree(pc) ;
    /* Displacement */
    double dis[3] = {0,0,0} ;
    /* strains */
    double eps[9] = {0,0,0,0,0,0,0,0,0} ;
    double eps_p[9] = {0,0,0,0,0,0,0,0,0} ;
    double tre,e ;
    double w_l[3] = {0,0,0} ;
    double sig[9] = {0,0,0,0,0,0,0,0,0} ;
    double hardv[3] = {0,0,0} ;
    double k_h = 0 ;
    int    i ;
    
    for(i = 0 ; i < dim ; i++) {
      dis[i] = FEM_ComputeUnknown(fem,u,intfct,p,U_u + i) ;
    }
    
    /* Averaging */
    for(i = 0 ; i < np ; i++) {
      double* vim  = vim0 + i*NVI ;
      double* vex  = vex0 + i*NVE ;
      double* def =  FEM_ComputeLinearStrainTensor(fem,u,intfct,i,U_u) ;
      int j ;
      
      for(j = 0 ; j < 3 ; j++) w_l[j] += W_L[j]/np ;

      for(j = 0 ; j < 9 ; j++) sig[j] += SIG[j]/np ;
      
      for(j = 0 ; j < 9 ; j++) eps_p[j] += EPS_P[j]/np ;
      
      for(j = 0 ; j < 9 ; j++) eps[j] += def[j]/np ;
      
      for(j = 0 ; j < 3 ; j++) hardv[j] += HARDV[j]/np ;
      
      k_h += K_L/np ;
    }
    
    tre = eps[0] + eps[4] + eps[8] ;
    e   = (1 + e0) * tre ;
      
    i = 0 ;
    Result_Store(r + i++,&p_l     ,"Liquid_pore_pressure",1) ;
    Result_Store(r + i++,dis      ,"Displacements",3) ;
    Result_Store(r + i++,w_l      ,"Fluid_mass_flow",3) ;
    Result_Store(r + i++,sig      ,"Stresses",9) ;
    Result_Store(r + i++,&sl      ,"Saturation_degree",1) ;
    Result_Store(r + i++,&e       ,"Void_ratio_variation",1) ;
    Result_Store(r + i++,eps_p    ,"Plastic_strains",9) ;
    Result_Store(r + i++,hardv    ,"Hardening_variable",1) ;
  }
  
  return(NbOfOutputs) ;
}



int ComputeTangentCoefficients(Element_t* el,double t,double dt,double* c)
/*
**  Tangent matrix (c), return the shift (dec).
*/
{
#define T4(a,i,j,k,l)  ((a)[(((i)*3+(j))*3+(k))*3+(l)])
#define T2(a,i,j)      ((a)[(i)*3+(j)])
#define C1(i,j,k,l)    T4(c1,i,j,k,l)
#define B1(i,j)        T2(c1,i,j)
  int dim = Element_GetDimensionOfSpace(el) ;
  double*  vim0  = Element_GetCurrentImplicitTerm(el) ;
  double*  vim0_n = Element_GetPreviousImplicitTerm(el) ;
//  double*  vex0  = Element_GetExplicitTerm(el) ;
  double** u     = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  double** u_n   = Element_ComputePointerToPreviousNodalUnknowns(el) ;
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int np = IntFct_GetNbOfPoints(intfct) ;
  FEM_t* fem = FEM_GetInstance(el) ;
  
  int    dec = 100 ;
  int    p ;
  double zero = 0. ;
  
  double dxi[Model_MaxNbOfEquations] ;
  
  {
    ObVal_t* obval = Element_GetObjectiveValue(el) ;
    int i ;
    
    for(i = 0 ; i < NEQ ; i++) {
      dxi[i] =  1.e-2*ObVal_GetValue(obval + i) ;
    }
  }


  
  for(p = 0 ; p < np ; p++) {
    double* vim  = vim0 + p*NVI ;
    double* vim_n  = vim0_n + p*NVI ;
    double* c0 = c + p*dec ;
    /* Variables */
    double* x = ComputeVariables(el,u,u_n,vim0_n,t,dt,p) ;
    
    /* Pressure */
    double p_l = FEM_ComputeUnknown(fem,u,intfct,p,U_p_l) ;
    //double p_l = x[I_P_L] ;
    double pc = p_g - p_l ;


    /* initialization */
    {
      int i ;
      
      for(i = 0 ; i < dec ; i++) c0[i] = zero ;
    }
    

    /* Mechanics */
    {
      double sig[9] ;
      int i ;
    
      for(i = 0 ; i < 9 ; i++) sig[i] = SIG[i] ;
    
      /* Net stresses */
      sig[0] += p_g ;
      sig[4] += p_g ;
      sig[8] += p_g ;
      
      /* Tangent stiffness matrix */
      {
        double* c1 = c0 ;
        double* crit = CRIT ;
        
        {
          double* sig_n   = SIG_n ;
          double signet_n = (sig_n[0] + sig_n[4] + sig_n[8])/3. + p_g ;
          double bulk     = - signet_n*(1 + eM0)/kappa ;
          double young    = 3 * bulk * (1 - 2*poisson) ;
          
          Elasticity_SetParameters(elasty,young,poisson) ;
          Elasticity_UpdateStiffnessTensor(elasty) ;
        }

        Elasticity_CopyStiffnessTensor(elasty,c1) ;
      
        {
          /* Criterion */
          if(crit[0] >= 0. || crit[1] >= 0.) {
            double hardv[4] = {HARDV[0],HARDV[1],HARDV[2],pc} ;
            
          /* Continuum tangent stiffness matrix */
            //ComputeTangentStiffnessTensor(sig,hardv) ;
          /* Consistent tangent stiffness matrix */
            ComputeTangentStiffnessTensor(sig,hardv,DLAMBDA) ;

            CopyTangentStiffnessTensor(c1) ;
          }
        }
      }
      
      
      /* Coupling matrix */
      {
        double  dp_l = dxi[U_p_l] ;
        double* dxdp_l = ComputeVariableDerivatives(el,t,dt,x,dp_l,I_P_L) ;
        double* dsigdp_l = dxdp_l + I_SIG ;
        double* c1 = c0 + 81 ;

        for(i = 0 ; i < 9 ; i++) c1[i] = dsigdp_l[i] ;
      }
    }
    
    
    /* Hydraulics */
    {
      /* Fluid mass density */
      double rho_l = rho_l0 ;
      double sl = SaturationDegree(pc) ;
    
    
      /* Coupling matrix */
      {
        double* c1 = c0 + 81 + 9 ;
        int i ;
        
        for(i = 0 ; i < 3 ; i++) B1(i,i) = rho_l*sl ;
      }
      
      
      /* Storage matrix */
      {
        double* c1 = c0 + 81 + 9 + 9 ;
        //double dxk   = dxi[U_p_l] ;
        //int    k     = I_P_L ;
        //double* dx   = ComputeVariableDerivatives(el,t,dt,x,dxk,k) ;
        /* Porosity */
        double* eps  = FEM_ComputeLinearStrainTensor(fem,u,intfct,p,U_u) ;
        double tre   = eps[0] + eps[4] + eps[8] ;
        double phi   = phi0 + tre ;
        
        c1[0] = - rho_l*phi*dSaturationDegree(pc) ;
      }
    }
  }
  
  return(dec) ;
#undef C1
#undef B1
#undef T2
#undef T4
}



int ComputeTransferCoefficients(FEM_t* fem,double dt,double* c)
/*
**  Conduction matrix (c) and shift (dec)
*/
{
  Element_t* el = FEM_GetElement(fem) ;
  double* vex0 = Element_GetExplicitTerm(el) ;
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int np = IntFct_GetNbOfPoints(intfct) ;
  int    dec = 9 ;
  int    p ;
  double zero = 0. ;
  

  for(p = 0 ; p < np ; p++) {
    int i ;
    double* c1 = c + p*dec ;
    
    /* initialization */
    for(i = 0 ; i < dec ; i++) c1[i] = zero ;
    
    {
      double* vex  = vex0 + p*NVE ;
      
      /* Permeability tensor */
      c1[0] = K_L ;
      c1[4] = K_L ;
      c1[8] = K_L ;
    }
  }
  
  return(dec) ;
}





double* ComputeVariables(Element_t* el,void* vu,void* vu_n,void* vf_n,const double t,const double dt,const int p)
{
  IntFct_t* intfct = Element_GetIntFct(el) ;
  FEM_t*    fem    = FEM_GetInstance(el) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  double** u   = (double**) vu ;
  double** u_n = (double**) vu_n ;
  double*  f_n = (double*)  vf_n ;
  double*  x   = Variable ;
  double*  x_n = Variable_n ;
  
    
  /* Primary Variables */
  {
    int    i ;
    
    /* Displacements */
    for(i = 0 ; i < dim ; i++) {
      x[I_U + i] = FEM_ComputeUnknown(fem,u,intfct,p,U_u + i) ;
    }
    
    for(i = dim ; i < 3 ; i++) {
      x[I_U + i] = 0 ;
    }
    
    /* Strains */
    {
      double* eps =  FEM_ComputeLinearStrainTensor(fem,u,intfct,p,U_u) ;
    
      for(i = 0 ; i < 9 ; i++) {
        x[I_EPS + i] = eps[i] ;
      }
      
      FEM_FreeBufferFrom(fem,eps) ;
    }
    
    /* Pressure */
    x[I_P_L] = FEM_ComputeUnknown(fem,u,intfct,p,U_p_l) ;
    
    /* Pressure gradient */
    {
      double* grd = FEM_ComputeUnknownGradient(fem,u,intfct,p,U_p_l) ;
    
      for(i = 0 ; i < 3 ; i++) {
        x[I_GRD_P_L + i] = grd[i] ;
      }
      
      FEM_FreeBufferFrom(fem,grd) ;
    }
  }
  
  
  /* Needed variables to compute secondary variables */
  {
    int    i ;
    
    /* Stresses, strains at previous time step */
    {
      double* eps_n =  FEM_ComputeLinearStrainTensor(fem,u_n,intfct,p,U_u) ;
      double* vim_n = f_n + p*NVI ;
    
      for(i = 0 ; i < 9 ; i++) {
        x_n[I_EPS   + i] = eps_n[i] ;
        x_n[I_SIG   + i] = SIG_n[i] ;
        x_n[I_EPS_P + i] = EPS_P_n[i] ;
      }
      
      for(i = 0 ; i < 3 ; i++) {
        x_n[I_HARDV + i] = HARDV_n[i] ;
      }
      
      FEM_FreeBufferFrom(fem,eps_n) ;
    }
    
    /* Pressure at previous time step */
    x_n[I_P_L] = FEM_ComputeUnknown(fem,u_n,intfct,p,U_p_l) ;
    
    /* Transfer coefficient */
    {
      double* vex0 = Element_GetExplicitTerm(el) ;
      double* vex  = vex0 + p*NVE ;
      
      x_n[I_K_L]  = K_L ;
    }
  }
    
  ComputeSecondaryVariables(el,t,dt,x_n,x) ;
  
  return(x) ;
}



void  ComputeSecondaryVariables(Element_t* el,double t,double dt,double* x_n,double* x)
{
  /* Inputs 
   * ------*/
  int dim = Element_GetDimensionOfSpace(el) ;
  /* Strains */
  double* eps   =  x   + I_EPS ;
  double* eps_n =  x_n + I_EPS ;
  /* Stresses */
  double* sig_n =  x_n + I_SIG ;
  /* Plastic strains */
  double* eps_pn = x_n + I_EPS_P ;
  /* Pressure */
  double  p_l   = x[I_P_L] ;
  double  p_ln  = x_n[I_P_L] ;
  double  pc   = p_g - p_l ;
  double  pc_n = p_g - p_ln ;
    

  /* Outputs 
   * ------*/

  /* Backup stresses, plastic strains */
  {
    double* sig   = x + I_SIG ;
    double* eps_p = x + I_EPS_P ;
    
    {
      double deps[9] ;
      int    i ;
      
      /* Incremental deformations */
      for(i = 0 ; i < 9 ; i++) deps[i] =  eps[i] - eps_n[i] ;
    
      /* Elastic trial stresses at t */
      {
        double trde      = deps[0] + deps[4] + deps[8] ;
        double dlns      = log((pc + p_atm)/(pc_n + p_atm)) ;
        double signet_n  = (sig_n[0] + sig_n[4] + sig_n[8])/3. + p_g ;
        double bulk      = - signet_n*(1 + eM0)/kappa ;
        double kappa_s   = MacroElasticStiffnessSuctionChanges(-signet_n) ;
        double dsigm     = bulk*trde - signet_n*kappa_s/kappa*dlns ;
        double young     = 3 * bulk * (1 - 2*poisson) ;
        double mu        = 0.5 * young/(1 + poisson) ;
          
        Elasticity_SetParameters(elasty,young,poisson) ;
        Elasticity_UpdateElasticTensors(elasty) ;
        
        for(i = 0 ; i < 9 ; i++) sig[i] = sig_n[i] + 2*mu*deps[i] ;
      
        sig[0] += dsigm - 2*mu*trde/3. ;
        sig[4] += dsigm - 2*mu*trde/3. ;
        sig[8] += dsigm - 2*mu*trde/3. ;
      }
      
      /* Elastic trial net stresses at t  */
      {
        sig[0] += p_g ;
        sig[4] += p_g ;
        sig[8] += p_g ;
      }
    
      /* Plastic strains */
      for(i = 0 ; i < 9 ; i++) eps_p[i] = eps_pn[i] ;
    
      /* Return mapping */
      {
        double hardv[4] = {x_n[I_HARDV],x_n[I_HARDV + 1],x_n[I_HARDV + 2],pc} ;
        double* crit  = ReturnMapping(sig,eps_p,hardv) ;
        double* dlambda = Plasticity_GetPlasticMultiplier(plasty) ;
        
        for(i = 0 ; i < 2 ; i++) x[I_CRIT + i]  = crit[i] ;
        for(i = 0 ; i < 3 ; i++) x[I_HARDV + i] = hardv[i] ;
        for(i = 0 ; i < 2 ; i++) x[I_DLAMBDA + i] = dlambda[i] ;
      }
    
      /* Total stresses */
      sig[0] -= p_g ;
      sig[4] -= p_g ;
      sig[8] -= p_g ;
    }
  }
  
  
  /* Backup mass flow */
  {
    /* Porosity */
    double tre   = eps[0] + eps[4] + eps[8] ;
    double phi   = phi0 + tre ;
    
    /* Fluid mass density */
    double rho_l = rho_l0 ;
    
    /* Fluid mass flow */
    {
      /* Transfer coefficient */
      double k_h = x_n[I_K_L] ;
    
      /* Pressure gradient */
      double* gpl = x + I_GRD_P_L ;
    
      /* Mass flow */
      double* w_l = x + I_W_L ;
      int i ;
    
      for(i = 0 ; i < 3 ; i++) w_l[i] = - k_h*gpl[i] ;
      w_l[dim - 1] += k_h*rho_l*gravity ;
    
      /* permeability */
      x[I_K_L] = rho_l*k_int/mu_l*RelativePermeabilityToLiquid(pc) ;
    }
    
    /* Liquid mass content, body force */
    {
    /* saturation */
      double  sl  = SaturationDegree(pc) ;
      double  m_l = rho_l*phi*sl ;
      double* f_mass = x + I_Fmass ;
      int i ;
    
      x[I_M_L]   = m_l ;
      x[I_RHO_L] = rho_l ;
      x[I_PHI]   = phi ;
      
      for(i = 0 ; i < 3 ; i++) f_mass[i] = 0 ;
      f_mass[dim - 1] = (rho_s + m_l)*gravity ;
    }
  }
}






#if 1
double* ComputeVariableDerivatives(Element_t* el,double t,double dt,double* x,double dxi,int i)
{
  double* dx = dVariable ;
  double* x_n = Variable_n ;
  int j ;
  
  /* Primary Variables */
  for(j = 0 ; j < NbOfVariables ; j++) {
    dx[j] = x[j] ;
  }
  
  /* We increment the variable as (x + dx) */
  dx[i] += dxi ;
  
  ComputeSecondaryVariables(el,t,dt,x_n,dx) ;
  
  /* The numerical derivative as (f(x + dx) - f(x))/dx */
  for(j = 0 ; j < NbOfVariables ; j++) {
    dx[j] -= x[j] ;
    dx[j] /= dxi ;
  }

  return(dx) ;
}
#endif
