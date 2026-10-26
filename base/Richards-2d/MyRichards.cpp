#ifdef HAVE_AUTODIFF
#define USE_AUTODIFF true
#endif

#define TITLE   "Richards Equation (3D)"
#define AUTHORS "Dangla"


/* Nb of equations */
#define NEQ     (1)

/* Equation index */
#define E_LIQ   (0)

/* Unknown index (generic) */
#define U_LIQ   (0)

/* Unknown index */
#define U_P_L   U_LIQ


namespace BaseName() {

/* We define the implicit terms */
template<typename T = double>
struct ImplicitValues_t {
  T Pressure_liquid;
  T GradPressure_liquid[3];
  T Mass_liquid;
  T MassFlow_liquid[3];
} ;



/* We define the explicit terms */
template<typename T = double>
struct ExplicitValues_t {
  T Permeability_liquid;
} ;



/* We define the constant terms */
template<typename T = double>
struct ConstantValues_t {
};



/* We define other terms */
template<typename T = double>
struct OtherValues_t {
  T SaturationDegree_liquid;
};


/* Parameters */
/* The parameters below are read in the input data file */
struct Parameters_t {
  double Porosity;
  double IntrinsicPermeability;
  double ReferenceGasPressure;
  double Gravity;
  double LiquidMassDensity;
  double LiquidViscosity;
  double MacroGradient[3];
  double MacroFunctionIndex[3];
};


template<typename T>
using Values_t = CustomValues_t<T,ImplicitValues_t,ExplicitValues_t,ConstantValues_t,OtherValues_t> ;


using Values_d = Values_t<double> ;



struct MPM_t: public MaterialPointMethod_t<Values_t> {
  MaterialPointMethod_SetInputs_t<Values_t> SetInputs;
  template<typename T>
  MaterialPointMethod_Integrate_t<Values_t,T> Integrate;
  Values_t<double>* Integrate(Element_t* el,double const& t,double const& dt,Values_t<double> const& val_n,Values_t<double>& val) {return(Integrate<double>(el,t,dt,val_n,val));}
  #ifdef HAVE_AUTODIFF
  Values_t<real>* Integrate(Element_t* el,double const& t,double const& dt,Values_t<double> const& val_n,Values_t<real>& val) {return(Integrate<real>(el,t,dt,val_n,val));}
  #endif
  MaterialPointMethod_Initialize_t<Values_t>  Initialize;
  MaterialPointMethod_SetTangentMatrix_t<Values_t> SetTangentMatrix;
  MaterialPointMethod_SetTransferMatrix_t<Values_t> SetTransferMatrix;
  MaterialPointMethod_SetIndexOfPrimaryVariables_t SetIndexOfPrimaryVariables;
  void SetIncrementOfPrimaryVariables(Element_t* el,double* dui) { 
    ObVal_t* obval = Element_GetObjectiveValue(el);

    dui[0]  = 1.e-2*ObVal_GetValue(obval + U_LIQ);
  }
} ;

}

using namespace BaseName();


/* Material properties */
#define SATURATION_CURVE       (Element_GetCurve(el))
#define RELATIVEPERM_CURVE     (Element_GetCurve(el) + 1)

#define SaturationDegree(pc)                Curve_ComputeValue(SATURATION_CURVE,pc)
#define RelativePermeabilityToLiquid(pc)    Curve_ComputeValue(RELATIVEPERM_CURVE,pc)

static double macrograd[3];


/* Functions */
static int    pm(const char *s) ;
static void   GetProperties(Element_t*,double) ;
static double* MacroGrad(Element_t*,double) ;



double* MacroGrad(Element_t* el,double t)
{
  Parameters_t param = ((Parameters_t*) Element_GetProperty(el))[0] ;
  Functions_t* fcts = Material_GetFunctions(Element_GetMaterial(el)) ;
  Function_t*  fct = Functions_GetFunction(fcts) ;
  int nf = Functions_GetNbOfFunctions(fcts) ;
  double  f[3] = {0,0,0} ;
    
  for(int i = 0 ; i < 3 ; i++) {
    double& fctindex = param.MacroFunctionIndex[i] ;
    int idx = (int) floor(fctindex + 0.5) ;
    
    if(0 < idx && idx < nf + 1) {
      Function_t* macrogradfct = fct + idx - 1 ;
      
      f[i] = Function_ComputeValue(macrogradfct,t) ;
    }
        
    macrograd[i] = param.MacroGradient[i] * f[i] ;
  }
    
  return(macrograd);
}



int pm(const char *s) {
#define Parameters_Index(V)  static_cast<int>(CustomValues_Index(Parameters_t,V,double))
  if(!strcmp(s,"Gravity")) {
    return (Parameters_Index(Gravity)) ;
  } else if(!strcmp(s,"Porosity")) {
    return (Parameters_Index(Porosity)) ;
  } else if(!strcmp(s,"LiquidMassDensity")) {
    return (Parameters_Index(LiquidMassDensity)) ;
  } else if(!strcmp(s,"IntrinsicPermeability")) {
    return (Parameters_Index(IntrinsicPermeability)) ;
  } else if(!strcmp(s,"LiquidViscosity")) {
    return (Parameters_Index(LiquidViscosity)) ;
  } else if(!strcmp(s,"ReferenceGasPressure")) {
    return (Parameters_Index(ReferenceGasPressure)) ;
  } else if(!strcmp(s,"MacroGradient")) {
    return(Parameters_Index(MacroGradient[0])) ;
  } else if(!strncmp(s,"MacroGradient_",14)) {
    int i = (strlen(s) > 14) ? s[14] - '1' : 0 ;
    
    if(i < 0 || i >= 3) i = 0 ;
    
    return(Parameters_Index(MacroGradient[0]) + i);
  } else if(!strcmp(s,"MacroFunctionIndex")) {
    return(Parameters_Index(MacroFunctionIndex[0])) ;
  } else if(!strncmp(s,"MacroFunctionIndex_",19)) {
    int i = (strlen(s) > 19) ? s[19] - '1' : 0 ;
    
    if(i < 0 || i >= 3) i = 0 ;
    
    return(Parameters_Index(MacroFunctionIndex[0]) + i);
  } else return(-1) ;
#undef Parameters_Index
}



void GetProperties(Element_t* el,double t)
{
}



int MPM_t::SetTangentMatrix(Element_t* el,double const& t,double const& dt,int const& p,Values_d const& val,Values_d const& dval,int const& k,double* c)
{
  int   ncols = 1;
  int   dec = ncols*ncols ;
  double* c0 = c + p*dec ;
      
  /* Derivatives w.r.t U_LIQ */
  if(k == 0) {
    /* General storage matrix */
    double* c1 = c0 ;

    c1[0] = dval.Mass_liquid ;
  }

  return(dec) ;
}




int MPM_t::SetTransferMatrix(Element_t* el,double const& dt,int const& p,Values_d const& val,double* c)
{
  int    dec = 9 ;

  {
    double* c0 = c + p*dec ;
    
    {
      /* Transfer coefficients for the liquid mass flux */
      {
        double* c1 = c0 ;
        
        c1[0] = dt*val.Permeability_liquid ;
        c1[4] = c1[0] ;
        c1[8] = c1[0] ;
      }
    }
  }

  return(dec) ;
}



void MPM_t::SetIndexOfPrimaryVariables(Element_t* el,int* ind) {
  ind[0]  = static_cast<int>(CustomValues_Index(Value_type,Pressure_liquid));
}




Values_d* MPM_t::SetInputs(Element_t* el,const double& t,const int& p,double const* const* u,Values_d& val)
{
  LocalVariables_t<Values_d> var(u,NULL);
    
  /* Pressure and pressure gradient */
  var.ValueAndGradientFEM(el,p,U_LIQ,&val.Pressure_liquid) ;
  
  if(Geometry_IsPeriodic(Element_GetGeometry(el))) {
    double* grad = MacroGrad(el,t);
    IntFct_t* intfct = Element_GetIntFct(el) ;
    double* h = IntFct_GetFunctionAtPoint(intfct,p) ;
    double* coor = Element_ComputeCoordinateVector(el,h) ;
      
    for(int i = 0 ; i < 3 ; i++) {
      val.GradPressure_liquid[i] += grad[i] ;
      val.Pressure_liquid        += grad[i]*coor[i] ;
    }
  }
  
  return(&val) ;
}



template <typename T>
Values_t<T>* MPM_t::Integrate(Element_t* el,const double& t,const double& dt,Values_d const& val_n,Values_t<T>& val)
{
  Parameters_t param = ((Parameters_t*) Element_GetProperty(el))[0];
  int dim = Element_GetDimensionOfSpace(el);
  /* Parameters */
  double& rho_l   = param.LiquidMassDensity;
  double& phi     = param.Porosity;
  double& gravity = param.Gravity;
  double& pg      = param.ReferenceGasPressure;
  double& mu_l    = param.LiquidViscosity;
  double& k_int   = param.IntrinsicPermeability;
  /* Pressures */
  T pl  = val.Pressure_liquid;
  T pc  = pg - pl;
    
  /* saturation */
  T sl  = SaturationDegree(pc);
    
  /* liquid mass content */
  T m_l = rho_l*phi*sl;
  
  
  /* Backup */

  val.SaturationDegree_liquid = sl;
  val.Mass_liquid = m_l;
  
  /* Fluxes */
  {
    /* Transfer coefficients at the previous time */
    double k_l = val_n.Permeability_liquid;
    
    /* Fluxes */
    T* w_l = val.MassFlow_liquid;
    
    /* Gradients */
    T* gpl = val.GradPressure_liquid;
    
    for(int i = 0 ; i < 3 ; i++) w_l[i] = - k_l*gpl[i];
    w_l[dim-1] += k_l*rho_l*gravity;
  }
  
  /* Transfer coefficients at the current time */
  {
    T kr_l    = RelativePermeabilityToLiquid(pc);
    T k_l     = rho_l*k_int/mu_l*kr_l;
      
    val.Permeability_liquid  = k_l;
  }
  
  return(&val) ;
}




Values_d* MPM_t::Initialize(Element_t* el,double const& t,Values_d& val)
{
  Parameters_t param = ((Parameters_t*) Element_GetProperty(el))[0];
  /* Parameters */
  double& rho_l   = param.LiquidMassDensity;
  double& pg      = param.ReferenceGasPressure;
  double& mu_l    = param.LiquidViscosity;
  double& k_int   = param.IntrinsicPermeability;
  /* Pressures */
  double pl   = val.Pressure_liquid;
  double pc   = pg - pl;
  double kr_l = RelativePermeabilityToLiquid(pc);
  double k_l  = rho_l*k_int/mu_l*kr_l;

  val.Permeability_liquid  = k_l;
  
  return(&val);
}




namespace BaseName() {
struct MyRichards : public UserModel_t<MPM_t> {
  private:
  MPM_t& mpm = GetInstanceOfMaterialPointMethod();
  
  public:
  int SetModelProperties(){
    strcpy(GetCodeNameOfModel(),"MyRichards");
    Model_CopyShortTitle(this,TITLE) ;
    Model_CopyNameOfAuthors(this,AUTHORS) ;
    
    SetNbOfEquations(NEQ) ;
  
    Model_CopyNameOfEquation(this,E_LIQ, "liq") ;
    Model_CopyNameOfUnknown(this,U_P_L,"p_l") ;
    
    SetComputePropertyIndex(&pm);
    SetComputeMaterialProperties(&GetProperties);
    UseFEM();
    return(0) ;
  }


  int ReadMatProp(Material_t *mat,DataFile_t *datafile) {
    int  NbOfProp = ((int) sizeof(Parameters_t)/sizeof(double)) ;

    if(datafile) {
      /* Par defaut tout a 0 */
      Material_SetPropertiesToZero(mat,NbOfProp);

      Material_ScanProperties(mat,datafile,pm) ;
    }
  
    return(NbOfProp) ;
  }
  
  int ComputeMatrix(Element_t* el,double t,double dt,double* k){
    int i = mpm.ComputeMassConservationMatrixByFEM(el,t,dt,k);
  
    return(i);
  }
  
  int ComputeResidu(Element_t* el,double t,double dt,double* r){
    /* Initialization */
    {
      int ndof = Element_GetNbOfDOF(el) ;
    
      for(int i = 0 ; i < ndof ; i++) r[i] = 0. ;
    }
  
    /* Conservation of liquid mass */
    {
      double Value_type<double>::* mass = &Value_type<double>::Mass_liquid;
      double (Value_type<double>::* flow)[3] = &Value_type<double>::MassFlow_liquid;
      int imass = static_cast<int>(Values_Index(mass));
      int iflow = static_cast<int>(Values_Index(flow));
    
      mpm.ComputeMassConservationResiduByFEM(el,t,dt,r,E_LIQ,imass,iflow);
    }
  
    return(0);
  }
  
  int ComputeOutputs(Element_t* el,double t,double* s,Result_t* r){
    int NbOfOutputs = 3 ;
    double* vim = Element_GetImplicitTerm(el) ;
    double* vex = Element_GetExplicitTerm(el) ;
    double** u  = Element_ComputePointerToCurrentNodalUnknowns(el) ;
    IntFct_t* intfct = Element_GetIntFct(el) ;
    int np = IntFct_GetNbOfPoints(intfct) ;

    //if(Element_IsSubmanifold(el)) return(0) ;

    /* Initialization */
    {    
      for(int i = 0 ; i < NbOfOutputs ; i++) {
        Result_SetValuesToZero(r + i) ;
      }
    }
  
    /*
      Input data
    */
    ComputeMaterialProperties(el,t) ;

    {
      /* Interpolation functions at s */
      double* a = Element_ComputeCoordinateInReferenceFrame(el,s) ;
      int p = IntFct_ComputeFunctionIndexAtPointOfReferenceFrame(intfct,a) ;
      Parameters_t param = ((Parameters_t*) Element_GetProperty(el))[0];
      double pg  = param.ReferenceGasPressure;
      /* pressures */
      double pl  = Element_ComputeUnknown(el,u,intfct,p,U_P_L) ;
      double pc  = pg - pl ;
    
      /* saturation */
      double sl  = SaturationDegree(pc) ;
    
      double w_l[3] = {0,0,0} ;
    
      /* Averaging */
      for(int i = 0 ; i < np ; i++) {
        CustomValues_t<double,ImplicitValues_t>* val1 = (CustomValues_t<double,ImplicitValues_t>*) vim ;
      
        for(int j = 0 ; j < 3 ; j++) w_l[j]  += val1[i].MassFlow_liquid[j]/np ;
      }
    
      {
        int i = 0 ;
      
        Result_Store(r + i++,&pl,"pressure",1) ;
        Result_Store(r + i++,w_l,"flow",3) ;
        Result_Store(r + i++,&sl,"saturation",1) ;
      }
    }
  
    return(NbOfOutputs) ;
  }
};
}
