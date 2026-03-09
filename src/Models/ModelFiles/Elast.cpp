#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "CommonModel.h"
#include "FEM.h"
#include "DataSet.h"
#include "Modules.h"
#include "Elasticity.h"

#define TITLE   "Elasticity"
#define AUTHORS " "

#include "PredefinedModelMethods.h"


/* Nb of equations */
#define NEQ     (dim)

/* Equation index */
#define E_MECH   (0)
/* Unknown index */
#define U_DISP   (0)


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
  template<typename T>
  MaterialPointMethod_Integrate_t<Values_t,T> Integrate;
  Values_t<double>* Integrate(Element_t* el,double const& t,double const& dt,Values_t<double> const& val_n,Values_t<double>& val) {return(Integrate<double>(el,t,dt,val_n,val));}
  MaterialPointMethod_Initialize_t<Values_t>  Initialize;
  MaterialPointMethod_SetTangentMatrix_t<Values_t> SetTangentMatrix;
  MaterialPointMethod_SetIndexOfPrimaryVariables_t SetIndexOfPrimaryVariables;
  MaterialPointMethod_SetIncrementOfPrimaryVariables_t SetIncrementOfPrimaryVariables;
} ;



template<typename T = double>
struct ImplicitValues_t {
  T Displacement[3];
  T Strain[9];
  T Stress[9];
  T BodyForce[3];
};



/* We define some names for explicit terms */
template<typename T = double>
struct ExplicitValues_t {
};



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
};


MPM_t mpm;
}

using namespace BaseName();


/* Functions */
static Model_ComputePropertyIndex_t  pm ;
static double* MicrostructureElasticTensor(DataSet_t*,double*) ;

static void    ComputeMicrostructure(DataSet_t*,double*,double*) ;
static void    CheckMicrostructureDataSet(DataSet_t*) ;


static double* MacroGradient(Element_t*,double) ;
static double* MacroStrain(Element_t*,double) ;


/* Material parameters */
static double  macrogradient[9] ;
static double  macrostrain[9] ;
#if SharedMS_APIis(OpenMP)
  #pragma omp threadprivate(macrogradient,macrostrain)
#endif


#define ItIsPeriodic  (Geometry_IsPeriodic(Element_GetGeometry(el)))


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
    
    for(int i = 0 ; i < 9 ; i++) {
      int idx = floor(fctindex[i] + 0.5) ;
      
      if(0 < idx && idx < nf + 1) {
        Function_t* macrogradfct = fct + idx - 1 ;
      
        f[i] = Function_ComputeValue(macrogradfct,t) ;
      }
    }
  }
  
  {
    double* g = &Element_GetPropertyValue(el,"macro-gradient") ;
    
    for(int i = 0 ; i < 9 ; i++) {
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



int SetModelProp(Model_t* model)
/** Set the model properties */
{
  int dim = Model_GetDimension(model) ;
  
  /** Number of equations to be solved */
  Model_GetNbOfEquations(model) = NEQ ;
  
  /** Names of these equations */
  for(int i = 0 ; i < dim ; i++) {
    char name_eqn[7] ;
    sprintf(name_eqn,"meca_%d",i + 1) ;
    Model_CopyNameOfEquation(model,E_MECH + i,name_eqn) ;
  }
  
  /** Names of the main unknowns */
  for(int i = 0 ; i < dim ; i++) {
    char name_unk[4] ;
    sprintf(name_unk,"u_%d",i + 1) ;
    Model_CopyNameOfUnknown(model,U_DISP + i,name_unk) ;
  }
  
  Model_GetComputePropertyIndex(model) = &pm ;
    
  return(0) ;
}



int ReadMatProp(Material_t* mat,DataFile_t* datafile)
/** Read the material properties in the stream file ficd */
{
  int NbOfProp = ((int) sizeof(Parameters_t)/sizeof(double)) ;

  /* Par defaut tout a 0 */
  Material_SetPropertiesToZero(mat,NbOfProp);
  Material_ScanProperties(mat,datafile,pm) ;
  
  
  /* Elasticity */
  {
    Elasticity_t* elasty = Elasticity_Create() ;
      
    Material_AppendData(mat,1,elasty,"Elasticity") ;
  }
  
  /* The 4th rank elastic tensor */
  {
    char* method = Material_GetMethod(mat) ;
    Elasticity_t* elasty = Material_FindData(mat,"Elasticity") ;

    /* obtained from a microstructure */
    if(!strncmp(method,"Microstructure",14)) {
      char* p = strstr(method," ") ;
      char* cellname = p + strspn(p," ") ;
      Options_t* options = Options_Create(NULL) ;
      DataSet_t* dataset = DataSet_Create(cellname,options) ;
      double* c = Elasticity_GetStiffnessTensor(elasty) ;
      
      CheckMicrostructureDataSet(dataset) ;
      
      MicrostructureElasticTensor(dataset,c) ;
      
    /* isotropic Hooke's law */
    } else {
      /* Parameters */
      Parameters_t& par = ((Parameters_t*) Material_GetProperty(mat))[0] ;
      double young = par.YoungModulus;
      double poisson = par.PoissonRatio;
    
      Elasticity_SetToIsotropy(elasty) ;
      Elasticity_SetParameters(elasty,young,poisson) ;
      
      {
        double* c = Elasticity_GetStiffnessTensor(elasty) ;
    
        Elasticity_ComputeStiffnessTensor(elasty,c) ;
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



int PrintModelChar(Model_t* model,FILE* ficd)
/** Print the model characteristics */
{
  printf(TITLE) ;
  
  if(!ficd) return(0) ;
  
  printf("\n") ;
  printf("The set of equations is:\n") ;
  printf("\t- Mechanical Equilibrium     (meca_[1,2,3])\n") ;
  
  printf("\n") ;
  printf("The primary unknowns are:\n") ;
  printf("\t- Displacements              (u_[1,2,3]) \n") ;
  
  printf("\n") ;
  printf("Example of input data\n") ;

  printf("gravity = 0          # gravity\n") ;
  printf("rho_s = 0            # mass density of the dry material\n") ;
  printf("young = 2713e6       # Young's modulus\n") ;
  printf("poisson = 0.339      # Poisson's ratio\n") ;
  printf("sig0_11 = 0          # initial stress 11 (any ij are allowed)\n") ;
  printf("macro-strain_23 = 1  # For periodic BC (any ij)\n") ;
  
  return(0) ;
}



int DefineElementProp(Element_t* el,IntFcts_t* intfcts,ShapeFcts_t* shapefcts)
/** Define some properties attached to each element */
{
  IntFct_t* intfct = Element_GetIntFct(el) ;
  int NbOfIntPoints = IntFct_GetNbOfPoints(intfct) ;
  
  mpm.DefineNbOfInternalValues(el,NbOfIntPoints);
  
  return(0) ;
}



int  ComputeLoads(Element_t* el,double t,double dt,Load_t* cg,double* r)
/** Compute the residu (r) due to loads */
{
  IntFct_t* intfct = Element_GetIntFct(el) ;
  int ndof = Element_GetNbOfDOF(el) ;
  FEM_t* fem = FEM_GetInstance(el) ;
  
  {
    double* r1 = FEM_ComputeSurfaceLoadResidu(fem,intfct,cg,t,dt) ;
    
    for(int i = 0 ; i < ndof ; i++) r[i] = r1[i] ;
  }
  
  return(0) ;
}



int ComputeInitialState(Element_t* el,double t)
/** Compute the initial state i.e. 
 *  the constant terms,
 *  the explicit terms,
 *  the implicit terms.
 */ 
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
/** Compute the implicit terms */
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
  /* 1. Mechanics */
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
  int NbOfOutputs = 3 ;
  double* vim0 = Element_GetCurrentImplicitTerm(el) ;
  double** u  = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int np = IntFct_GetNbOfPoints(intfct) ;
  int dim = Geometry_GetDimension(Element_GetGeometry(el)) ;

  if(Element_IsSubmanifold(el)) return(0) ;

  {
    /* Interpolation functions at s */
    double* a = Element_ComputeCoordinateInReferenceFrame(el,s) ;
    int p = IntFct_ComputeFunctionIndexAtPointOfReferenceFrame(intfct,a) ; 
    double* dis  = Element_ComputeDisplacementVector(el,u,intfct,p,U_DISP) ;
    double dis_tot[3] = {0,0,0} ;
    double sig[9] = {0,0,0,0,0,0,0,0,0} ;
    
    /* Displacement */
    for(int i = 0 ; i < dim ; i++) {
      dis_tot[i] = dis[i] ;
    }

    if(ItIsPeriodic) {
      for(int i = 0 ; i < dim ; i++) {
        int j ;
        
        for(j = 0 ; j < dim ; j++) {
          dis_tot[i] += MacroGradient(el,t)[3*i + j] * s[j] ;
        }
      }
    }
      
    /* Averaging */
    for(p = 0 ; p < np ; p++) {
      CustomValues_t<double,ImplicitValues_t>* val1 = (CustomValues_t<double,ImplicitValues_t>*) vim0 ;
      
      for(int i = 0 ; i < 9 ; i++) sig[i] += val1[p].Stress[i]/np ;
    }
      
    {
      int i = 0 ;
      
      Result_Store(r + i++,dis_tot ,"Displacements",3) ;
      Result_Store(r + i++,sig ,"Stresses",9) ;
      Result_Store(r + i++,dis,"Perturbated-displacements",3) ;
      
      if(i != NbOfOutputs) arret("ComputeOutputs") ;
    }
  }

  return(NbOfOutputs) ;
}




Values_d* MPM_t::SetInputs(Element_t* el,const double& t,const int& p,double const* const* u,Values_d& val)
{
  LocalVariables_t<Values_d> var(u,NULL);
  
  /* Displacements and strains */
  var.DisplacementVectorAndStrainFEM(el,p,U_DISP,val.Displacement) ;
  
  if(ItIsPeriodic) {
    double* strain = MacroStrain(el,t);
    
    for(int i = 0 ; i < 9 ; i++) {
      val.Strain[i]   += strain[i] ;
    }
  }
  
  return(&val) ;
}



template <typename T>
Values_t<T>* MPM_t::Integrate(Element_t* el,const double& t,const double& dt,Values_d const& val_n,Values_t<T>& val)
/** Compute the secondary variables from the primary ones. */
{
  /* Parameters */
  Parameters_t& par = ((Parameters_t*) Element_GetProperty(el))[0] ;
  double& gravity = par.Gravity;
  double& rho_s   = par.MassDensity_solid;
  double* sig0    = par.InitialStress;
  /* Inputs 
   * ------*/
  /* Strains */
  T* eps   =  val.Strain ;
    

  /* Outputs 
   * ------*/

  /* Backup stresses */
  {
    T* sig   = val.Stress ;
    Elasticity_t* elasty  = Element_FindMaterialData(el,"Elasticity") ;
    double* cijkl = Elasticity_GetStiffnessTensor(elasty) ;
    
    #define C(i,j)  (cijkl[(i)*9+(j)])
    for(int i = 0 ; i < 9 ; i++) {      
      sig[i] = sig0[i];
      
      for(int j = 0 ; j < 9 ; j++) {
        sig[i] += C(i,j)*eps[j] ;
      }
    }
    #undef C
  }
  
  
  /* Backup body force */
  {
    int dim = Element_GetDimensionOfSpace(el) ;
    T* f_mass = val.BodyForce ;
      
    for(int i = 0 ; i < 3 ; i++) f_mass[i] = 0 ;
    f_mass[dim - 1] = (rho_s)*gravity ;
  }
  
  return(&val) ;
}




Values_d* MPM_t::Initialize(Element_t* el,double const& t,Values_d& val)
{
  /* Parameters */
  Parameters_t& par = ((Parameters_t*) Element_GetProperty(el))[0] ;
  double* sig0   = par.InitialStress;
  DataFile_t* datafile = Element_GetDataFile(el) ;
  
  /* Initial stresses */
  if(DataFile_ContextIsPartialInitialization(datafile)) {
    for(int i = 0 ; i < 9 ; i++) val.InitialStress[i] = val.Stress[i] ;
  } else {
    for(int i = 0 ; i < 9 ; i++) val.InitialStress[i] = sig0[i] ;
    for(int i = 0 ; i < 9 ; i++) val.Stress[i] = sig0[i] ;
  }
    
  return(&val);
}

  


void MPM_t::SetIndexOfPrimaryVariables(Element_t* el,int* ind) {
  //ind[0] = Values_Index(Strain[0]);
    
  for(int i = 0 ; i < 9 ; i++) {
    ind[i] = -1;
  }
}




void MPM_t::SetIncrementOfPrimaryVariables(Element_t* el,double* dui) {  
  ObVal_t* obval = Element_GetObjectiveValue(el) ;
        
  for(int i = 0 ; i < 9 ; i++) {
    dui[i] =  1.e-6 ;
  }
}





int MPM_t::SetTangentMatrix(Element_t* el,double const& t,double const& dt,int const& p,Values_d const& val,Values_d const& dval,int const& k,double* c)
{
  int ncols = 9;
  int dec = ncols*ncols;
  double* c0 = c + p*dec;

    /* 1. Derivatives w.r.t. the strain tensor
     * --------------------------------------- */
    if(k == 0) {
      /* 1.1 Tangent stiffness matrix */
      {
        Elasticity_t* elasty  = Element_FindMaterialData(el,"Elasticity") ;
        
        Elasticity_CopyStiffnessTensor(elasty,c0) ;
      }
    }

  return(dec) ;
}



  
double* MicrostructureElasticTensor(DataSet_t* dataset,double* c)
{
#define C(i,j,k,l)  (c[(((i)*3+(j))*3+(k))*3+(l)])

  {
    double grd[9] = {0,0,0,0,0,0,0,0,0} ;
    int k ;
    
    for(k = 0 ; k < 3 ; k++) {
      int l ;
      
      for(l = 0 ; l < 3 ; l++) {
        double sig[9] ;
        int i ;
          
        /* Set the macro gradient (k,l) as 1 */
        grd[k*3 + l] = 1 ;
          
        /* For eack (k,l) compute the stresses Sij = Cijkl */
        {
          Session_Open() ;
          Message_SetNewVerbosity(0) ;
        
          Message_Direct("\n") ;
          Message_Direct("Start a microstructure calculation") ;
          Message_Direct("\n") ;
          
          ComputeMicrostructure(dataset,grd,sig) ;
          
          Session_Close() ;
        }

        for(i = 0 ; i < 3 ; i++) {
          int j ;
                
          for(j = 0 ; j < 3 ; j++) {
            C(i,j,k,l) = sig[i*3 + j] ;
          }
        }
        
        grd[k*3 + l] = 0 ;
      }
    }
  }
    
  return(c) ;
#undef C
}



void ComputeMicrostructure(DataSet_t* dataset,double* macrograd,double* sig)
{
  /* Set input data of the microstructure */
  {
    /* Update the macro-gradient */
    {
      Materials_t* mats = DataSet_GetMaterials(dataset) ;
      int nmats = Materials_GetNbOfMaterials(mats) ;
      int j ;
    
      for(j = 0 ; j < nmats ; j++) {
        Material_t* mat = Materials_GetMaterial(mats) + j ;
        Model_t* model = Material_GetModel(mat) ;
        Model_ComputePropertyIndex_t* pidx = Model_GetComputePropertyIndex(model) ;
        
        {
          double* grd = Material_GetProperty(mat) + pidx("macro-gradient") ;
          int i ;
    
          for(i = 0 ; i < 9 ; i++) {
            grd[i] = macrograd[i] ;
          }
        }
      }
    }
  }
    
  /* Compute the microstructure */
  {
    Module_t* module = DataSet_GetModule(dataset) ;
    
    Module_ComputeProblem(module,dataset) ;
  }

  /* Backup stresses as averaged stresses */
  {
    Mesh_t* mesh = DataSet_GetMesh(dataset) ;
    
    FEM_AverageStresses(mesh,sig) ;
  }
}




void CheckMicrostructureDataSet(DataSet_t* dataset)
{
  /* Set input data of the microstructure */
  {
    /* Update the macro-fctindex */
    {
      Materials_t* mats = DataSet_GetMaterials(dataset) ;
      int nmats = Materials_GetNbOfMaterials(mats) ;
      int j ;
    
      for(j = 0 ; j < nmats ; j++) {
        Material_t* mat = Materials_GetMaterial(mats) + j ;
        Model_t* model = Material_GetModel(mat) ;
        Model_ComputePropertyIndex_t* pidx = Model_GetComputePropertyIndex(model) ;
        
        if(!pidx) {
          arret("ComputeMicrostructure(1): Model_GetComputePropertyIndex(model) undefined") ;
        }
          
        {
          int k = pidx("macro-fctindex") ;
          int i ;
            
          for(i = 0 ; i < 9 ; i++) {
            Material_GetProperty(mat)[k + i] = 1 ;
          }
        }
      }
    }

    /* Check and update the function of time */
    {
      Functions_t* fcts = DataSet_GetFunctions(dataset) ;
      int nfcts = Functions_GetNbOfFunctions(fcts) ;
      
      if(nfcts < 1) {
        arret("ComputeMicrostructure(1): the min nb of functions should be 1") ;
      }
        
      {
        Function_t* func = Functions_GetFunction(fcts) ;
        int npts = Function_GetNbOfPoints(func) ;
          
        if(npts < 2) {
          arret("ComputeMicrostructure(2): the min nb of points should be 2") ;
        }
          
        {
          double* t = Function_GetXValue(func) ;
          double* f = Function_GetFValue(func) ;
            
          Function_GetNbOfPoints(func) = 2 ;
          t[0] = 0 ;
          t[1] = 1 ;
          f[0] = 0 ;
          f[1] = 1 ;
        }
      }
    }
    
    /* The dates */
    {
      {
        Dates_t* dates = DataSet_GetDates(dataset) ;
        int     nbofdates  = Dates_GetNbOfDates(dates) ;
          
        if(nbofdates < 2) {
          arret("ComputeMicrostructure(3): the min nb of dates should be 2") ;
        }
      
        Dates_GetNbOfDates(dates) = 2 ;
      }
    }
  }
}
