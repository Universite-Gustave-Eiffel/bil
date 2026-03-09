#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "Context.h"
#include "CommonModel.h"
#include "FEM.h"
#include "Plasticity.h"

#define TITLE "Plasticity with hardening (2017)"
#define AUTHORS "Dangla"

#include "PredefinedModelMethods.h"


/* Nb of equations */
#define NEQ     (dim)

/* Equation index */
#define E_MECH   (0)

/* Indices of unknowns (generic indices) */
#define U_MECH     E_MECH

/* Unknown index */
#define U_DISP     U_MECH



#include "CustomValues.h"
#include "MaterialPointMethod.h"
#include "BaseName.h"


namespace BaseName() {
template<typename T>
struct ImplicitValues_t ;

template<typename T>
struct ExplicitValues_t;

template<typename T>
struct ConstantValues_t;

template<typename T>
struct OtherValues_t;


template<typename T>
using Values_t = CustomValues_t<T,ImplicitValues_t,ExplicitValues_t,ConstantValues_t,OtherValues_t> ;


using Values_d = Values_t<double> ;

#define Values_Index(V)  CustomValues_Index(Values_t,V)



struct MPM_t: public MaterialPointMethod_t<Values_t> {
  MaterialPointMethod_SetInputs_t<Values_t> SetInputs;
  MaterialPointMethod_Integrate_t<Values_t> Integrate;
  MaterialPointMethod_Initialize_t<Values_t>  Initialize;
  MaterialPointMethod_SetTangentMatrix_t<Values_t> SetTangentMatrix;
} ;


/* We define some names for implicit terms */
template<typename T = double>
struct ImplicitValues_t {
  T Displacement[3];
  T Strain[9];
  T Stress[9];
  T BodyForce[3];
  T PlasticStrain[9];
  T HardeningVariable;
  T CriterionValue;
  T PlasticMultiplier;
} ;


template<typename T = double>
struct OtherValues_t {
};



/* We define some names for explicit terms */
template<typename T = double>
struct ExplicitValues_t {
} ;



/* We define some names for constant terms (v0 must be used as pointer below) */
template<typename T = double>
struct ConstantValues_t {
  T InitialStress[9];
};


/* The parameters below are read in the input data file */
struct Parameters_t {
  double Gravity;
  double MassDensity_solid;
  double InitialStress[9];
  double PoissonRatio;
  double YoungModulus;
  double MacroStrainGradient[9];
  double MacroFunctionIndex[9];
  union {
    struct {
      double InitialCumulativePlasticShearStrain;
      double Cohesion;
      double FrictionAngle;
      double DilatancyAngle;
    };
    struct {
      double InitialPreconsolidationPressure;
      double SlopeOfSwellingLine;
      double SlopeOfVirginConsolidationLine;
      double SlopeOfCriticalStateLine;
      double InitialVoidRatio;
    };
  };
};


MPM_t mpm;
}

using namespace BaseName();





/* Functions */
static Model_ComputePropertyIndex_t  pm ;
static void   GetProperties(Element_t*,double) ;

static double* MacroGradient(Element_t*,double) ;
static double* MacroStrain(Element_t*,double) ;


#define ComputeTangentStiffnessTensor(...)  Plasticity_ComputeTangentStiffnessTensor(plasty,__VA_ARGS__)
#define ReturnMapping(...)                  Plasticity_ReturnMapping(plasty,__VA_ARGS__)
//#define ComputeTangentStiffnessTensor(...)  Plasticity_GenericTangentStiffnessTensor(plasty,__VA_ARGS__)
//#define ReturnMapping(...)                  Plasticity_GenericReturnMapping(plasty,__VA_ARGS__)
#define CopyElasticTensor(...)              Plasticity_CopyElasticTensor(plasty,__VA_ARGS__)
#define CopyTangentStiffnessTensor(...)     Plasticity_CopyTangentStiffnessTensor(plasty,__VA_ARGS__)


/* Material parameters */
static double  macrogradient[9] ;
static double  macrostrain[9] ;
static enum {
  None,
  DruckerPrager,
  CamClay
} plasticmodel;

#define  SetPlasticModel(I) \
         do { \
           if(plasticmodel == None) { \
             plasticmodel = I ; \
           } else if(plasticmodel != I) { \
             Message_FatalError("Incompatible model") ; \
           } \
         } while(0)


#define GetProperty(a)      Element_GetPropertyValue(el,a)

#define ItIsPeriodic  (Geometry_IsPeriodic(Element_GetGeometry(el)))



#if SharedMS_APIis(OpenMP)
  #pragma omp threadprivate(macrogradient,macrostrain,plasticmodel)
#endif
/* To treat later */
#if SharedMS_APIis(OpenMP)
  #error "Not available yet" 
#endif


int pm(const char *s)
{
#define Parameters_Index(V)  CustomValues_Index(Parameters_t,V,double)
         if(!strcmp(s,"gravity")) { 
    return (Parameters_Index(Gravity)) ;
  } else if(!strcmp(s,"rho_s")) { 
    return (Parameters_Index(MassDensity_solid)) ;
  } else if(!strcmp(s,"young")) { 
    return (Parameters_Index(YoungModulus)) ;
  } else if(!strcmp(s,"sig0")) {
    return(Parameters_Index(InitialStress[0])) ;
  } else if(!strncmp(s,"sig0_",5)) {
    int i = (strlen(s) > 5) ? s[5] - '1' : 0 ;
    int j = (strlen(s) > 6) ? s[6] - '1' : 0 ;
    return(Parameters_Index(InitialStress[3*i + j])) ;
  } else if(!strcmp(s,"poisson"))       { 
    return (Parameters_Index(PoissonRatio)) ;
  } else if(!strcmp(s,"macro-gradient")) {
    return(Parameters_Index(MacroStrainGradient[0])) ;
  } else if(!strncmp(s,"macro-gradient_",15)) {
    int i = (strlen(s) > 15) ? s[15] - '1' : 0 ;
    int j = (strlen(s) > 16) ? s[16] - '1' : 0 ;
    return(Parameters_Index(MacroStrainGradient[3*i + j])) ;
  } else if(!strcmp(s,"macro-fctindex")) {
    return(Parameters_Index(MacroFunctionIndex[0])) ;
  } else if(!strncmp(s,"macro-fctindex_",15)) {
    int i = (strlen(s) > 15) ? s[15] - '1' : 0 ;
    int j = (strlen(s) > 16) ? s[16] - '1' : 0 ;
    return(Parameters_Index(MacroFunctionIndex[3*i + j])) ;
    
    /* Model 1: Drucker-Prager */
  } else if(!strcmp(s,"initial_cumulative_plastic_shear_strain")) {
    SetPlasticModel(DruckerPrager) ;
    return (Parameters_Index(InitialCumulativePlasticShearStrain)) ;
  } else if(!strcmp(s,"cohesion")) {
    SetPlasticModel(DruckerPrager) ;
    return (Parameters_Index(Cohesion)) ;
  } else if(!strcmp(s,"friction")) {
    SetPlasticModel(DruckerPrager) ;
    return (Parameters_Index(FrictionAngle)) ;
  } else if(!strcmp(s,"dilatancy")) {
    return (Parameters_Index(DilatancyAngle)) ;
    SetPlasticModel(DruckerPrager) ;
    
    /* Model 2: Cam-clay */
  } else if(!strcmp(s,"initial_pre-consolidation_pressure")) {
    SetPlasticModel(CamClay) ;
    return (Parameters_Index(InitialPreconsolidationPressure)) ;
  } else if(!strcmp(s,"slope_of_swelling_line")) {
    SetPlasticModel(CamClay) ;
    return (Parameters_Index(SlopeOfSwellingLine)) ;
  } else if(!strcmp(s,"slope_of_virgin_consolidation_line")) {
    SetPlasticModel(CamClay) ;
    return (Parameters_Index(SlopeOfVirginConsolidationLine)) ;
  } else if(!strcmp(s,"slope_of_critical_state_line"))  {
    SetPlasticModel(CamClay) ;
    return (Parameters_Index(SlopeOfCriticalStateLine)) ;
  } else if(!strcmp(s,"initial_void_ratio")) {
    SetPlasticModel(CamClay) ;
    return (Parameters_Index(InitialVoidRatio)) ;
  } else return(-1) ;
#undef Parameters_Index
}



double* MacroGradient(Element_t* el,double t)
{
  double* gradient = macrogradient ;
  double  f[9] = {0,0,0,0,0,0,0,0,0} ;
  
  {
    Functions_t* fcts = Material_GetFunctions(Element_GetMaterial(el)) ;
    Function_t*  fct = Functions_GetFunction(fcts) ;
    int nf = Functions_GetNbOfFunctions(fcts) ;
    double* fctindex = &Element_GetPropertyValue(el,"macro-fctindex") ;
    int i ;
    
    for(i = 0 ; i < 9 ; i++) {
      int idx = (int) floor(fctindex[i] + 0.5) ;
      
      if(0 < idx && idx < nf + 1) {
        Function_t* macrogradfct = fct + idx - 1 ;
      
        f[i] = Function_ComputeValue(macrogradfct,t) ;
      }
    }
  }
  
  {
    double* g = &Element_GetPropertyValue(el,"macro-gradient") ;
    int i ;
    
    for(i = 0 ; i < 9 ; i++) {
      gradient[i] = g[i] * f[i] ;
    }
  }
  
  return(gradient) ;
}



double* MacroStrain(Element_t* el,double t)
{
  double* strain = macrostrain ;
  double* grd = MacroGradient(el,t) ;
  int i ;
    
  for(i = 0 ; i < 3 ; i++) {
    int j ;
      
    for(j = 0 ; j < 3 ; j++) {
      strain[3*i + j] = 0.5*(grd[3*i + j] + grd[3*j + i]) ;
    }
  }
  
  return(strain) ;
}



void GetProperties(Element_t* el,double t)
{
  //gravity = Element_GetPropertyValue(el,"gravity") ;
  //rho_s   = Element_GetPropertyValue(el,"rho_s") ;
  //sig0    = &Element_GetPropertyValue(el,"sig0") ;
  //hardv0  = Element_GetPropertyValue(el,"hardv0") ;
  
  #if 0
  {
    int id = SharedMS_CurrentThreadId ;
    GenericData_t* gdat = Element_FindMaterialGenericData(el,"Plasticity") ;
    int n = (gdat) ? GenericData_GetNbOfData(gdat) : 0 ;
    
    if(id < n) {
      plasty = ((Plasticity_t*) GenericData_GetData(gdat)) + id ;
      //plasty = Element_FindMaterialData(el,"Plasticity") + id ;
    } else {
      arret("GetProperties") ;
    }
  
    {
      Elasticity_t* elasty = Plasticity_GetElasticity(plasty) ;
    
      cijkl   = Elasticity_GetStiffnessTensor(elasty) ;
      hardv0  = Plasticity_GetHardeningVariable(plasty)[0] ;
    }
  }
  #endif
}



int SetModelProp(Model_t* model)
/** Set the model properties */
{
  int dim = Model_GetDimension(model) ;
  int i ;
  
  /** Number of equations to be solved */
  Model_GetNbOfEquations(model) = NEQ ;
  
  /** Names of these equations */
  for(i = 0 ; i < dim ; i++) {
    char name_eqn[7] ;
    sprintf(name_eqn,"meca_%d",i + 1) ;
    Model_CopyNameOfEquation(model,E_MECH + i,name_eqn) ;
  }
  
  /** Names of the main unknowns */
  for(i = 0 ; i < dim ; i++) {
    char name_unk[4] ;
    sprintf(name_unk,"u_%d",i + 1) ;
    Model_CopyNameOfUnknown(model,U_DISP + i,name_unk) ;
  }
  
  Model_GetComputePropertyIndex(model) = &pm ;
  Model_GetComputeMaterialProperties(model) = &GetProperties;
  
  return(0) ;
}



int ReadMatProp(Material_t* mat,DataFile_t* datafile)
/** Read the material properties in the stream file ficd 
 *  Return the nb of (scalar) properties of the model */
{
  int NbOfProp = ((int) sizeof(Parameters_t)/sizeof(double)) ;
  #if SharedMS_APIis(None)
    int nthreads = 1 ;
  #else
    int nthreads = SharedMS_MaxNbOfThreads ;
  #endif

  /* Par defaut tout a 0 */
  Material_SetPropertiesToZero(mat,NbOfProp);
  plasticmodel = None ;
  Material_ScanProperties(mat,datafile,pm) ;

  /* Plasticity */
  {
    Plasticity_t* plasty = Mry_Create(Plasticity_t,nthreads,Plasticity_Create()) ;

    Material_AppendData(mat,nthreads,plasty,"Plasticity") ;
  }
  
  /* Elastic and plastic properties */
  {
    Plasticity_t* plasty = Material_FindData(mat,"Plasticity") ;
    
    for(int i = 0 ; i < nthreads ; i++) {
      Plasticity_t* plastyi = plasty + i ;
      Elasticity_t* elasty = Plasticity_GetElasticity(plastyi) ;
    
      {
        /* Parameters */
        Parameters_t& par = ((Parameters_t*) Material_GetProperty(mat))[0] ;
        
        /* Elasticity */
        {
          double young = par.YoungModulus;
          double poisson = par.PoissonRatio;
        
          Elasticity_SetToIsotropy(elasty) ;
          Elasticity_SetParameters(elasty,young,poisson) ;
        
          Elasticity_UpdateElasticTensors(elasty) ;
        }
      
        /* Drucker-Prager */
        if(plasticmodel == DruckerPrager) {
          double cohesion = par.Cohesion;
          double af       = par.FrictionAngle*M_PI/180. ;
          double ad       = par.DilatancyAngle*M_PI/180. ;
        
          Plasticity_SetTo(plastyi,DruckerPrager) ;
          Plasticity_SetParameters(plastyi,af,ad,cohesion,NULL) ;
      
        /* Cam-Clay with linear elasticity */
        } else if(plasticmodel == CamClay) {
          double kappa  = par.SlopeOfSwellingLine;
          double lambda = par.SlopeOfVirginConsolidationLine;
          double M      = par.SlopeOfCriticalStateLine;
          double pc0    = par.InitialPreconsolidationPressure;
          double e0     = par.InitialVoidRatio ;
        
          Plasticity_SetTo(plastyi,CamClay) ;
          Plasticity_SetParameters(plastyi,kappa,lambda,M,pc0,e0) ;
        
        } else {
          Message_FatalError("Unknown model") ;
        }
      }

#if 0
      {      
        Elasticity_PrintStiffnessTensor(elasty) ;
      }
#endif
    }
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
The system consists in (dim) equations\n\
\t 1. The equilibrium equation (meca_1,meca_2,meca_3)\n") ;

  printf("\n\
The primary unknowns are\n\
\t 1. The displacement vector (u_1,u_2,u_3)\n") ;

  printf("\n\
Example of input data\n\n") ;
  

  fprintf(ficd,"gravity = 0       # gravity\n") ;
  fprintf(ficd,"rho_s = 2350      # masse volumique du squelette sec\n") ;
  fprintf(ficd,"young = 5.8e+09   # module d\'Young\n") ;
  fprintf(ficd,"poisson = 0.3     # coefficient de Poisson\n") ;
  fprintf(ficd,"cohesion = 1e+06  # cohesion (Drucker-Prager model) \n") ;
  fprintf(ficd,"friction = 25     # friction angle (Drucker-Prager model) \n") ;
  fprintf(ficd,"dilatancy = 25    # dilatancy angle (Drucker-Prager model) \n") ;
  fprintf(ficd,"sig0_ij = -11.5e6 # contrainte initiale sig0_ij\n") ;
  
  return(0) ;
}


int DefineElementProp(Element_t* el,IntFcts_t* intfcts)
/** Define some properties attached to each element 
 *  Return 0 */
{
  IntFct_t* intfct = Element_GetIntFct(el) ;
  int NbOfIntPoints = IntFct_GetNbOfPoints(intfct) ;
  
  mpm.DefineNbOfInternalValues(el,NbOfIntPoints);
  
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

  {
    double* r1 = FEM_ComputeSurfaceLoadResidu(fem,fi,cg,t,dt) ;
  
    {
      int i ;
      
      for(i = 0 ; i < ndof ; i++) r[i] = r1[i] ;
    }
  }
  
  return(0) ;
}


int ComputeInitialState(Element_t* el,double t)
{
  int i = mpm.ComputeInitialStateByFEM(el,t);
  
  return(i);
}


int  ComputeExplicitTerms(Element_t* el,double t)
/** Compute the explicit terms */
{
  return(0) ;
}



int  ComputeImplicitTerms(Element_t* el,double t,double dt)
{
  int i = mpm.ComputeImplicitTermsByFEM(el,t,dt);
  
  return(i);
}



int  ComputeMatrix(Element_t* el,double t,double dt,double* k)
/** Compute the matrix (k) */
{
  int i = mpm.ComputeTangentStiffnessMatrixByFEM(el,t,dt,k);
  
  return(i);
}




int  ComputeResidu(Element_t* el,double t,double dt,double* r)
/** Comput the residu (r) */
{
  /* Initialization */
  {
    int ndof = Element_GetNbOfDOF(el) ;
    
    for(int i = 0 ; i < ndof ; i++) r[i] = 0. ;
  }
  
  {
    int istress = Values_Index(Stress[0]);
    int ibforce = Values_Index(BodyForce[0]);
    mpm.ComputeMechanicalEquilibriumResiduByFEM(el,t,dt,r,E_MECH,istress,ibforce);
  }

  return(0);
}



int  ComputeOutputs(Element_t* el,double t,double* s,Result_t* r)
/** Compute the outputs (r) */
{
  int NbOfOutputs = 6 ;
  double* vim0  = Element_GetCurrentImplicitTerm(el) ;
  double** u   = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int np = IntFct_GetNbOfPoints(intfct) ;
  int dim = Geometry_GetDimension(Element_GetGeometry(el)) ;

  //if(Element_IsSubmanifold(el)) return(0) ;

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
  Element_ComputeMaterialProperties(el,t) ;

  {
    /* Interpolation functions at s */
    double* a = Element_ComputeCoordinateInReferenceFrame(el,s) ;
    int p = IntFct_ComputeFunctionIndexAtPointOfReferenceFrame(intfct,a) ;
    /* Displacement */
    double* pdis = Element_ComputeDisplacementVector(el,u,intfct,p,U_DISP) ;
    double dis[3] = {0,0,0} ;
    /* strains */
    double eps_p[9] = {0,0,0,0,0,0,0,0,0} ;
    double sig[9] = {0,0,0,0,0,0,0,0,0} ;
    double hardv = 0 ;
    double crit = 0 ;
    
    for(int i = 0 ; i < dim ; i++) {
      dis[i] = pdis[i] ;
    }

    if(ItIsPeriodic) {
      for(int i = 0 ; i < dim ; i++) {
        int j ;
        
        for(j = 0 ; j < dim ; j++) {
          dis[i] += MacroGradient(el,t)[3*i + j] * s[j] ;
        }
      }
    }
    
    /* Averaging */
    for(p = 0 ; p < np ; p++) {
      CustomValues_t<double,ImplicitValues_t>* val1 = (CustomValues_t<double,ImplicitValues_t>*) vim0 ;

      for(int j = 0 ; j < 9 ; j++) sig[j] += val1[p].Stress[j]/np ;
      
      for(int j = 0 ; j < 9 ; j++) eps_p[j] += val1[p].PlasticStrain[j]/np ;
      
      hardv += val1[p].HardeningVariable/np ;
      
      crit += val1[p].CriterionValue/np ;
    }
    
    {
      int i = 0 ;
      
      Result_Store(r + i++,dis   ,"Displacements",3) ;
      Result_Store(r + i++,sig   ,"Stresses",9) ;
      Result_Store(r + i++,pdis  ,"Perturbated-displacements",3) ;
      Result_Store(r + i++,eps_p ,"Plastic-strains",9) ;
      Result_Store(r + i++,&hardv,"Hardening variable",1) ;
      Result_Store(r + i++,&crit ,"Yield function",1) ;
    
      if(i != NbOfOutputs) {
        Message_RuntimeError("ComputeOutputs: wrong number of outputs") ;
      }
    }
  }
  
  return(NbOfOutputs) ;
}


int MPM_t::SetTangentMatrix(Element_t* el,double const& t,double const& dt,int const& p,Values_d const& val,Values_d const& dval,int const& k,double* c)
{
  int    dec = 81 ;
  double* c0 = c + p*dec ;
  
    /* Mechanics */
    if(k == 0) {
      double* c1 = c0 ;
      
      
      /* Tangent stiffness matrix */
      {
        /* Criterion */
        int id = SharedMS_CurrentThreadId ;
        Plasticity_t* plasty = Element_FindMaterialData(el,"Plasticity") + id ;
        double crit = val.CriterionValue ;
        
        if(crit >= 0.) {
          double* sig = val.Stress;
          double hardv = val.HardeningVariable;
          double dlambda = val.PlasticMultiplier;
          //Plasticity_FreeBuffer(plasty) ;
          
          /* Continuum tangent stiffness matrix */
          //ComputeTangentStiffnessTensor(SIG,&HARDV) ;
          /* Consistent tangent stiffness matrix */
          ComputeTangentStiffnessTensor(sig,&hardv,&dlambda) ;
      
          CopyTangentStiffnessTensor(c1) ;
          
        } else {
      
          CopyElasticTensor(c1) ;
        }
      }
    }
  
  return(dec) ;
}



Values_d* MPM_t::SetInputs(Element_t* el,const double& t,const int& p,double const* const* u,Values_d& val)
{
  LocalVariables_t<Values_d> var(u,NULL);
  
  /* Displacements and strains */
  var.DisplacementVectorAndStrainFEM(el,p,U_MECH,val.Displacement) ;
  
  if(ItIsPeriodic) {
    double* strain = MacroStrain(el,t);
    
    for(int i = 0 ; i < 9 ; i++) {
      val.Strain[i]   += strain[i] ;
    }
  }
  
  return(&val) ;
}



Values_d*  MPM_t::Integrate(Element_t* el,const double& t,const double& dt,Values_d const& val_n,Values_d& val)
/** Compute the secondary variables from the primary ones. */
{
  /* Parameters */
  Parameters_t& par = ((Parameters_t*) Element_GetProperty(el))[0] ;
  double& gravity = par.Gravity;
  double& rho_s   = par.MassDensity_solid;
  double* sig0    = par.InitialStress;
  /* Strains */
  double* eps = val.Strain ;
  double const* eps_n = val_n.Strain ;
  /* Plastic strains */
  double* eps_p  = val.PlasticStrain ;
  double const* eps_pn = val_n.PlasticStrain ;

  /* Backup stresses, plastic strains */
  {
    double* sig   = val.Stress ;
    double const* sig_n = val_n.Stress ;
    double const hardv_n = val_n.HardeningVariable ;
    
    
    {
      int id = SharedMS_CurrentThreadId ;
      Plasticity_t* plasty = Element_FindMaterialData(el,"Plasticity") + id ;
      Elasticity_t* elasty = Plasticity_GetElasticity(plasty) ;
      double* cijkl = Elasticity_GetStiffnessTensor(elasty) ;
      double  deps[9] ;
      
      /* Incremental deformations */
      for(int i = 0 ; i < 9 ; i++) deps[i] =  eps[i] - eps_n[i] ;
    
      /* Elastic trial stresses */
      for(int i = 0 ; i < 9 ; i++) sig[i] = sig_n[i] ;
    
      #define C(i,j)  (cijkl[(i)*9+(j)])
      for(int i = 0 ; i < 9 ; i++) {      
        for(int j = 0 ; j < 9 ; j++) {
          sig[i] += C(i,j)*deps[j] ;
        }
      }
      #undef C
    
      /* Plastic strains */
      for(int i = 0 ; i < 9 ; i++) eps_p[i] = eps_pn[i] ;
    
      //Plasticity_FreeBuffer(plasty) ;
    
      /* Projection */
      {
        double hardv = hardv_n ;
        double* crit = ReturnMapping(sig,eps_p,&hardv) ;
        double* dlambda = Plasticity_GetPlasticMultiplier(plasty) ;
        
        val.CriterionValue  = crit[0] ;
        val.HardeningVariable = hardv ;
        val.PlasticMultiplier = dlambda[0] ;
      }
    }
  }
  
  
  /* Backup body force */
  {
    {
      int dim = Element_GetDimensionOfSpace(el) ;
      double* fmass = val.BodyForce ;
      
      for(int i = 0 ; i < 3 ; i++) fmass[i] = 0 ;
      fmass[dim - 1] = (rho_s)*gravity ;
    }
  }
  
  return(&val) ;
}




Values_d* MPM_t::Initialize(Element_t* el,double const& t,Values_d& val)
{
  /* Parameters */
  Parameters_t& par = ((Parameters_t*) Element_GetProperty(el))[0] ;
  double* sig0   = par.InitialStress;
  DataFile_t* datafile = Element_GetDataFile(el) ;
  int id = SharedMS_CurrentThreadId ;
  Plasticity_t* plasty = Element_FindMaterialData(el,"Plasticity") + id ;
  double hardv0  = Plasticity_GetHardeningVariable(plasty)[0] ;
  
    {
      /* Initial stresses, hardening variable */
      if(DataFile_ContextIsPartialInitialization(datafile)) {
        for(int i = 0 ; i < 9 ; i++) val.InitialStress[i] = val.Stress[i] ;
      } else {
        for(int i = 0 ; i < 9 ; i++) val.InitialStress[i] = sig0[i] ;
        for(int i = 0 ; i < 9 ; i++) val.Stress[i]  = sig0[i] ;
        val.HardeningVariable = hardv0 ;
      }
      
      /* Initial plastic strains */
      for(int i = 0 ; i < 9 ; i++) val.PlasticStrain[i]  = 0 ;
    }
  
  return(&val) ;
}
