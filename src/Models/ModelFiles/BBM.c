#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "Context.h"
#include "CommonModel.h"
#include "FEM.h"
#include "Plasticity.h"

#define TITLE "Barcelona Basic Model for unsaturated soils (2023)"
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


/* Implicit terms */
/* Nb of implicit terms */
#define NVI     (28)
/* We define some names for implicit terms */
#define M_L           (vim   + 0)[0]
#define M_L_n         (vim_n + 0)[0]
#define W_L           (vim   + 1)

#define SIG           (vim   + 4)
#define SIG_n         (vim_n + 4)

#define F_MASS        (vim   + 13)

#define EPS_P         (vim   + 16)
#define EPS_P_n       (vim_n + 16)

#define HARDV         (vim   + 25)[0]
#define HARDV_n       (vim_n + 25)[0]

#define CRIT          (vim   + 26)[0]
#define DLAMBDA       (vim   + 27)[0]


/* Explicit terms */
/* Nb of explicit terms */
#define NVE     (1)
/* We define some names for explicit terms */
#define K_L           (vex + 0)[0]


/* Constant terms */
/* Nb of constant terms */
#define NV0     (0)
/* We define some names for constant terms */


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





/* Parameters */
static double  gravity ;
static double  rho_s ;
static double* sig0 ;
static double  rho_l0 ;
static double  p_g = 0 ;
static double  k_int ;
static double  mu_l ;
static double  kappa ;
//static double  mu ;
static double  poisson ;
static double  e0 ;
static double  phi0 ;
static double  hardv0 ;
static Elasticity_t* elasty ;
static Plasticity_t* plasty ;
static Curve_t* saturationcurve ;
static Curve_t* relativepermcurve ;
static double  kappa_s ;
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
  I_CRIT,
  I_RHO_L,
  I_PHI,
  I_K_L,
  I_GRD_P_L,
  I_GRD_P_L2 = I_GRD_P_L + 2,
  I_DLAMBDA,
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
  } else if(!strcmp(s,"shear_modulus")) { 
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
  } else if(!strcmp(s,"slope_of_swelling_line")) {
    return(16) ;
  } else if(!strcmp(s,"slope_of_virgin_consolidation_line")) {
    return(17) ;
  } else if(!strcmp(s,"slope_of_critical_state_line"))  {
    return(18) ;
  } else if(!strcmp(s,"initial_pre-consolidation_pressure")) {
    return(19) ;
  } else if(!strcmp(s,"initial_porosity")) {
    return(20) ;
  } else if(!strcmp(s,"kappa_s")) {
    return(21) ;
  } else if(!strcmp(s,"suction_cohesion_coefficient")) {
    return(22) ;
  } else if(!strcmp(s,"reference_consolidation_pressure")) {
    return(23) ;
  } else if(!strcmp(s,"poisson")) {
    return(24) ;
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
  kappa   = Element_GetPropertyValue(el,"slope_of_swelling_line") ;
  //mu      = Element_GetPropertyValue(el,"shear_modulus") ;
  poisson = Element_GetPropertyValue(el,"poisson") ;
  phi0    = Element_GetPropertyValue(el,"initial_porosity") ;
  e0      = phi0/(1 - phi0) ;
  kappa_s = Element_GetPropertyValue(el,"kappa_s") ;
  
  plasty  = (Plasticity_t*) Element_FindMaterialData(el,"Plasticity") ;
  elasty  = Plasticity_GetElasticity(plasty) ;
  
  hardv0  = Plasticity_GetHardeningVariable(plasty)[0] ;
  
  saturationcurve = Element_FindCurve(el,"sl") ;
  relativepermcurve = Element_FindCurve(el,"kl") ;
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
  int  NbOfProp = 25 ;
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
      /* Barcelona Basic model */
      {
        double lambda = Material_GetPropertyValue(mat,"slope_of_virgin_consolidation_line") ;
        double M      = Material_GetPropertyValue(mat,"slope_of_critical_state_line") ;
        double pc0    = Material_GetPropertyValue(mat,"initial_pre-consolidation_pressure") ;
        double coh    = Material_GetPropertyValue(mat,"suction_cohesion_coefficient") ;
        double p_ref  = Material_GetPropertyValue(mat,"reference_consolidation_pressure") ;
        Curve_t* lc   = Material_FindCurve(mat,"lc") ;
        
        phi0    = Material_GetPropertyValue(mat,"initial_porosity") ;
        e0      = phi0/(1 - phi0) ;

        kappa  = Material_GetPropertyValue(mat,"slope_of_swelling_line") ;
        
        Plasticity_SetTo(plasty,BBM) ;
        Plasticity_SetParameters(plasty,kappa,lambda,M,pc0,e0,coh,p_ref,lc) ;
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
        HARDV = hardv0 ;
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
    
      CRIT = x[I_CRIT] ;
      HARDV = x[I_HARDV] ;
      DLAMBDA = x[I_DLAMBDA] ;
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
    
      CRIT = x[I_CRIT] ;
      HARDV = x[I_HARDV] ;
      DLAMBDA = x[I_DLAMBDA] ;
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
    int    i ;
    
    for(i = 0 ; i < ndof*ndof ; i++) k[i] = 0 ;
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
    double c[IntFct_MaxNbOfIntPoints*9] ;
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

  /* Initialization */
  for(i = 0 ; i < ndof ; i++) r[i] = 0 ;

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
    double hardv = 0 ;
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
      
      hardv += HARDV/np ;
      
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
    Result_Store(r + i++,&hardv   ,"Hardening_variable",1) ;
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
  
  int    dec = 100 ;
  int    p ;
  
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
    double p_l = x[I_P_L] ;
    double pc = p_g - p_l ;


    /* initialization */
    {
      int i ;
      
      for(i = 0 ; i < dec ; i++) c0[i] = 0 ;
    }
    

    /* 1. Derivatives w.r.t. the strain tensor
     * --------------------------------------- */
    {
      /* 1.1 Tangent stiffness matrix */
      {
        double* c1 = c0 ;
        double crit = CRIT ;
        
        {
          double* sig_n   = SIG_n ;
          double signet_n = (sig_n[0] + sig_n[4] + sig_n[8])/3. + p_g ;
          double bulk     = - signet_n*(1 + e0)/kappa ;
          //double lame     = bulk - 2*mu/3. ;
          //double poisson  = 0.5 * lame / (lame + mu) ;
          //double young    = 2 * mu * (1 + poisson) ;
          double young    = 3 * bulk * (1 - 2*poisson) ;
          
          Elasticity_SetParameters(elasty,young,poisson) ;
          Elasticity_UpdateStiffnessTensor(elasty) ;
        }

        Elasticity_CopyStiffnessTensor(elasty,c1) ;
      
        {
          /* Criterion */
          if(crit >= 0.) {
            double logp_co  = HARDV ;
            double hardv[2] = {logp_co,pc} ;
            double sig[9] ;
            int i ;
    
            for(i = 0 ; i < 9 ; i++) sig[i] = SIG[i] ;
    
            /* Net stresses */
            sig[0] += p_g ;
            sig[4] += p_g ;
            sig[8] += p_g ;
            
          /* Continuum tangent stiffness matrix */
            //ComputeTangentStiffnessTensor(sig,hardv) ;
          /* Consistent tangent stiffness matrix */
            ComputeTangentStiffnessTensor(sig,hardv,&DLAMBDA) ;

            CopyTangentStiffnessTensor(c1) ;
          }
        }
      }
      
      /* 1.2 Coupling matrix for the total mass  */
      {
        double* c1 = c0 + 81 + 9 ;
        double rho_l = rho_l0 ;
        double sl = SaturationDegree(pc) ;
        int i ;
        
        for(i = 0 ; i < 3 ; i++) B1(i,i) = rho_l*sl ;
      }
    }
    
    
    
    /* 2. Derivatives w.r.t. the liquid pressure
     * ----------------------------------------- */
    {
      double  dp_l = dxi[U_p_l] ;
      double* dxdp_l = ComputeVariableDerivatives(el,t,dt,x,dp_l,I_P_L) ;
      
      /* 2.1 Mechanical coupling matrix */
      {
        double* dsigdp_l = dxdp_l + I_SIG ;
        double* c1 = c0 + 81 ;
            int i ;

        for(i = 0 ; i < 9 ; i++) c1[i] = dsigdp_l[i] ;
      }
      
      /* 2.2 Storage coefficient for the liquid mass */
      {
        double* c1 = c0 + 81 + 9 + 9 ;
        double phi   = x[I_PHI] ;
        double rho_l = rho_l0 ;
        
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
  

  for(p = 0 ; p < np ; p++) {
    int i ;
    double* c1 = c + p*dec ;
    
    /* initialization */
    for(i = 0 ; i < dec ; i++) c1[i] = 0 ;
    
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
      
      x_n[I_HARDV] = HARDV_n ;
      
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
        double bulk      = - signet_n*(1 + e0)/kappa ;
        double dsigm     = bulk*trde - signet_n*kappa_s/kappa*dlns ;
        //double lame      = bulk - 2*mu/3. ;
        //double poisson   = 0.5 * lame / (lame + mu) ;
        //double young     = 2 * mu * (1 + poisson) ;
        double young     = 3 * bulk * (1 - 2*poisson) ;
        double mu        = 0.5*young/(1 + poisson) ;
          
        Elasticity_SetParameters(elasty,young,poisson) ;
        Elasticity_UpdateStiffnessTensor(elasty) ;
        
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
        double logp_con = x_n[I_HARDV] ; /* log(pre-consolidation pressure) at 0 suction at the previous time step */
        double hardv[2] = {logp_con,pc} ;
        double* crit  = ReturnMapping(sig,eps_p,hardv) ;
        double* dlambda = Plasticity_GetPlasticMultiplier(plasty) ;
        
        x[I_CRIT]  = crit[0] ;
        x[I_HARDV] = hardv[0] ;
        x[I_DLAMBDA] = dlambda[0] ;
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
