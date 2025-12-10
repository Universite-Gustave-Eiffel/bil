/* General features of the model:
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "CommonModel.h"

/* The Finite Volume Method */
#include "FVM.h"

/* Cement chemistry */
#include "HardenedCementChemistry.h"
#include "CementSolutionDiffusion.h"


#define TEMPERATURE   (293)

#define TITLE   "Internal/External sulfate attack of concrete (2017)" 
#define AUTHORS "Gu-Dangla"

#include "PredefinedModelMethods.h"



/* Indices of equations/unknowns */
enum {
  E_SULFUR,
  E_CHARGE,
  E_CALCIUM,
  E_POTASSIUM,
  E_ALUMINIUM,
  /* Uncomment/comment the two next lines to consider/suppress electroneutrality */
  E_ENEUTRAL,
  #define E_ENEUTRAL E_ENEUTRAL 
  E_LAST
} ;

#define E_MECH



/* Nb of equations */
#define NEQ                E_LAST





/* Value of the nodal unknown (u and el must be used below) */
#define UNKNOWN(n,i)     Element_GetValueOfNodalUnknown(el,u,n,i)
#define UNKNOWNn(n,i)    Element_GetValueOfNodalUnknown(el,u_n,n,i)


/* Generic names of nodal unknowns */
#define U_Sulfur(n)    (UNKNOWN(n,E_SULFUR))
#define Un_Sulfur(n)   (UNKNOWNn(n,E_SULFUR))

#define U_charge(n)     (UNKNOWN(n,E_CHARGE))
#define Un_charge(n)    (UNKNOWNn(n,E_CHARGE))

#define U_Calcium(n)    (UNKNOWN(n,E_CALCIUM))
#define Un_Calcium(n)   (UNKNOWNn(n,E_CALCIUM))

#define U_Potassium(n)  (UNKNOWN(n,E_POTASSIUM))
#define Un_Potassium(n) (UNKNOWNn(n,E_POTASSIUM))

#define U_Aluminium(n)  (UNKNOWN(n,E_ALUMINIUM))
#define Un_Aluminium(n) (UNKNOWNn(n,E_ALUMINIUM))

#define U_eneutral(n)   (UNKNOWN(n,E_ENEUTRAL))
#define Un_eneutral(n)  (UNKNOWNn(n,E_ENEUTRAL))




/* Method chosen at compile time. 
 * Each equation is associated to a specific unknown.
 * Each unknown can deal with a specific model.
 * Uncomment/comment to let only one unknown per equation */

/* Sulfur: unknown either C_SO4 or C_H2SO4 or LogC_SO4 or LogC_H2SO4 */
//#define U_C_SO4         U_Sulfur
#define U_LogC_SO4      U_Sulfur
//#define U_C_H2SO4       U_Sulfur
//#define U_LogC_H2SO4    U_Sulfur

/* charge: */
#define U_PSI       U_charge

/* Calcium:
 * - U_ZN_Ca_S: dissolution kinetics of CH; Cc at equilibrium 
 * - U_LogS_CH: dissolution kinetics of CH; precipitation kinetics of Cc */
#define U_ZN_Ca_S   U_Calcium
//#define U_LogS_CH     U_Calcium

/* Potassium: unknown either C or logC */
//#define U_C_K       U_Potassium
#define U_LogC_K    U_Potassium

/* Aluminium:
 * - U_ZN_Al_S: dissolution kinetics of AH3; Cc at equilibrium
 * - U_LogS_CH: dissolution kinetics of CH; precipitation kinetics of Cc */
//#define U_C_Al       U_Aluminium
//#define U_LogC_Al    U_Aluminium
#define U_ZN_Al_S    U_Aluminium

/* Electroneutrality: unknown either C_OH, logC_OH or Z_OH = C_H - C_OH */
#ifdef E_ENEUTRAL
//#define U_C_OH      U_eneutral
#define U_LogC_OH   U_eneutral
//#define U_Z_OH      U_eneutral
#endif







/* Compiling options */
#define EXPLICIT  1
#define IMPLICIT  2
#define ELECTRONEUTRALITY   IMPLICIT



/* Names of nodal unknowns */
#if defined (U_LogC_H2SO4) && !defined (U_C_H2SO4) && !defined (U_LogC_SO4) && !defined (U_C_SO4)
  #define LogC_H2SO4(n)   U_Sulfur(n)
  #define C_H2SO4(n)      (pow(10,LogC_H2SO4(n)))
#elif defined (U_C_H2SO4) && !defined (U_LogC_H2SO4) && !defined (U_LogC_SO4) && !defined (U_C_SO4)
  #define C_H2SO4(n)      U_Sulfur(n)
  #define LogC_H2SO4(n)   (log10(C_H2SO4(n)))
#elif defined (U_LogC_SO4) && !defined (U_C_SO4) && !defined (U_LogC_H2SO4) && !defined (U_C_H2SO4)
  #define LogC_SO4(n)     U_Sulfur(n)
  #define C_SO4(n)        (pow(10,LogC_SO4(n)))
#elif defined (U_C_SO4) && !defined (U_LogC_SO4) && !defined (U_LogC_H2SO4) && !defined (U_C_H2SO4)
  #define C_SO4(n)        U_Sulfur(n)
  #define LogC_SO4(n)     (log10(C_SO4(n)))
#else
  #error "Ambiguous or undefined unknown"
#endif



#define ZN_Ca_S(n)   U_Calcium(n)

#define PSI(n)       U_charge(n)

#if defined (U_LogC_K) && !defined (U_C_K)
  #define LogC_K(n)     U_Potassium(n)
  #define C_K(n)        (pow(10,LogC_K(n)))
#elif defined (U_C_K) && !defined (U_LogC_K)
  #define C_K(n)        U_Potassium(n)
  #define LogC_K(n)     (log10(C_K(n)))
#else
  #error "Ambiguous or undefined unknown"
#endif

#define ZN_Al_S(n)   U_Aluminium(n)

#ifdef E_ENEUTRAL
  #if defined (U_LogC_OH) && !defined (U_C_OH)
    #define LogC_OH(n)   U_eneutral(n)
    #define C_OH(n)      (pow(10,LogC_OH(n)))
  #elif defined (U_C_OH) && !defined (U_LogC_OH)
    #define C_OH(n)      U_eneutral(n)
    #define LogC_OH(n)   (log10(C_OH(n)))
  #else
    #error "Ambiguous or undefined unknown"
  #endif
#endif



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


template<typename T = double>
struct ImplicitValues_t {
  T U_sulfur;
  T U_charge;
  T U_calcium;
  T U_potassium;
  T U_aluminium;
  #ifdef E_ENEUTRAL
  T U_eneutral;
  #endif
  T Mole_sulfur;
  T MolarFlow_sulfur[Element_MaxNbOfNodes];
  T Mole_charge;
  T MolarFlow_charge[Element_MaxNbOfNodes];
  T Mole_calcium;
  T MolarFlow_calcium[Element_MaxNbOfNodes];
  T Mole_potassium;
  T MolarFlow_potassium[Element_MaxNbOfNodes];
  T Mole_aluminium;
  T MolarFlow_aluminium[Element_MaxNbOfNodes];
  T Mole_silicon;
  T MolarFlow_silicon[Element_MaxNbOfNodes];
  T Mole_chlorine;
  T MolarFlow_chlorine[Element_MaxNbOfNodes];
  T Mole_solidportlandite;
  T Mole_solidgypsum;
  T Mole_solidAH3;
  T Mole_solidAFm;
  T Mole_solidAFt;
  T Mole_solidC3AH6;
  T Mole_solidCSH;
  T Porosity;
  T Volume_solidtotal;
  T Volume_solidCSH;
  T Concentration_oh;
  T ChemicalPotential[CementSolutionDiffusion_NbOfConcentrations];
  T Strain;
  T CrystalPoreDeformation;
  T SaturationIndexOfAFtAtPoreWall;
  T CrystalPressure;
  T DamageStrain;
  T PoreRadius;
  T SaturationDegree_crystal;
  T Mole_solidCa;
  T Mole_liquidCa;
} ;



template<typename T = double>
struct OtherValues_t {
};





template<typename T = double>
struct ExplicitValues_t {
  T Tortuosity_liquid;
  T AqueousConcentration[CementSolutionDiffusion_NbOfConcentrations];
};



template<typename T = double>
struct ConstantValues_t {
  T InitialVolume_solidtotal;
};


struct MPM_t: public MaterialPointMethod_t<Values_t> {
  MaterialPointMethod_SetInputs_t<Values_t> SetInputs;
  MaterialPointMethod_Integrate_t<Values_t> Integrate;
  MaterialPointMethod_Initialize_t<Values_t> Initialize;
  MaterialPointMethod_SetTangentMatrix_t<Values_t> SetTangentMatrix;
  MaterialPointMethod_SetFluxes_t<Values_t> SetFluxes;
  MaterialPointMethod_SetIndexOfPrimaryVariables_t SetIndexOfPrimaryVariables;
  MaterialPointMethod_SetIncrementOfPrimaryVariables_t SetIncrementOfPrimaryVariables;
} ;


/* The parameters below are read in the input data file */
struct Parameters_t {
  double phi0;
  double n_ca_ref;
  double n_si_ref;
  double n_al_ref;
  double n_csh2_0;
  double n_afm_0;
  double n_aft_0;
  double n_c3ah6_0;
  double r_afm;
  double r_aft;
  double r_c3ah6;
  double r_csh2;
  double K_bulk;
  double strain0;
  double strainf;
  double alphacoef;
  double betacoef;
  double Biot;
  double ar_AFt;
  double ap_AFt;
  double dr_AFt;
  double dp_AFt;
};

static MPM_t mpm;
}

using namespace BaseName();

static double phi0;
static double n_ca_ref;
static double n_si_ref;
static double n_al_ref;
static double n_csh2_0;
static double n_afm_0;
static double n_aft_0;
static double n_c3ah6_0;
static double r_afm;
static double r_aft;
static double r_c3ah6;
static double r_csh2;
static double K_bulk;
static double strain0;
static double strainf;
static double alphacoef;
static double betacoef;
static double Biot;
static double ar_AFt;
static double ap_AFt;
static double dr_AFt;
static double dp_AFt;


/* Math constants */
#define Ln10      Math_Ln10




#include "InternationalSystemOfUnits.h"
/* Shorthands of some units */
#define m     (InternationalSystemOfUnits_OneMeter)
#define m3    (m*m*m)
#define dm    (0.1*m)
#define cm    (0.01*m)
#define nm    (1.e-9*m)
#define dm3   (dm*dm*dm)
#define cm3   (cm*cm*cm)
#define Pa    (InternationalSystemOfUnits_OnePascal)
#define MPa   (1.e6*Pa)
#define GPa   (1.e9*Pa)
#define J     (Pa*m3)
#define mol   (InternationalSystemOfUnits_OneMole)




/* Material properties
 * ------------------- */
#define SATURATION_CURVE           (satcurve)
#define LiquidSaturationDegree(r)  (Curve_ComputeValue(SATURATION_CURVE,r))
#define dLiquidSaturationDegree(r) (Curve_ComputeDerivative(SATURATION_CURVE,r))
#define PoreEntryRadiusMax         (Curve_GetXRange(SATURATION_CURVE)[1])
#define Damage(straind) \
        (1 - strain0/straind*exp(-(straind - strain0)/strainf))



/*
  Solids
  CH   = Calcium Hydroxide (Portlandite)
  CSH2 = Calcium Sulfate Dihydrate (Gypsum)
  CSH  = Calcium Silicates Hydrate
  SH   = Amorphous Silica Gel
*/


/* Calcium Silicate Hydrate Properties (C-S-H)
 * ------------------------------------------- */
//#define MOLARVOLUMEOFCSH_CURVE           (Element_GetCurve(el) + 2)
//#define MolarVolumeOfCSH(q)    (Curve_ComputeValue(MOLARVOLUMEOFCSH_CURVE,q))
#define V_CSH                  (78 * cm3)
#define V_SH                   (43 * cm3)
#define MolarVolumeOfCSH(x)    ((x)/1.7*V_CSH + (1 - (x)/1.7)*V_SH)
#define CSHSolidContent(zn_si_s)       SiliconContentInCSH(zn_si_s)



/* Calcium Hydroxide (Portlandite) Properties (CH)
 * ----------------------------------------------- */
/* Molar volume of CH solid */
#define V_CH       (33 * cm3)
#define CHSolidContent(zn_ca_s)        CalciumContentInCH(zn_ca_s)



/* Gypsum (CSH2) Properties
 * ------------------------ */
/* Molar volume of CSH2 crystal */
#define V_CSH2     (75 * cm3)
#define CSH2SolidContent_kin(n,s,dt)     MAX((n + dt*r_csh2*(s - 1)),0.)
#define CSH2SolidContent(n,s,dt)         CSH2SolidContent_kin(n,s,dt)



/* Gibbsite Properties (AH3)
 * ------------------------- */
/* Molar volume of AH3 solid */
#define V_AH3      (64.44 * cm3)
#define AH3SolidContent(zn_al_s)    (0.5*AluminiumContentInAH3(zn_al_s))



/* Monosulfoaluminate Properties (AFm = C4ASH12)
 * --------------------------------------------- */
/* Molar volume of AFm solid */
#define V_AFm      (311.26 * cm3)      /* Thermochimie (ANDRA) */
//#define AFmSolidContent(n,s,dt)     (n*pow(s,dt/t_afm))
#define AFmSolidContent(n,s,dt)     MAX((n + dt*r_afm*(s - 1)),0.)



/* Ettringite Properties (AFt = C6AS3H32)
 * -------------------------------------- */
/* Molar volume of AFt solid */
#define V_AFt      (710.32 * cm3)      /* Thermochimie (ANDRA) */
//#define AFtSolidContent(n,s,dt)     (n*pow(s,dt/t_aft))
#define AFtSolidContent(n,s,dt)     MAX((n + dt*r_aft*(s - 1)),0.)
/* Surface tension (N/m) */
#define Gamma_AFt   (0.1*Pa*m)



/* Sulfate adsorption curve 
 * ------------------------ */
#define AdsorbedSulfatePerUnitMoleOfCSH(c_so4,c_oh) \
        (alphacoef * (c_so4) / ((c_so4) + betacoef * (1.)))
//        (alphacoef * (c_so4) / ((c_so4) + betacoef * (c_oh)))



/* Crystal properties 
 * ------------------ */
#define V_Crystal       V_AFt
#define Gamma_Crystal   Gamma_AFt
/* Crystallization pressure */
#define CrystallizationPressure(beta) \
        RT/V_Crystal*log(beta)
#define dCrystallizationPressure(beta) \
        RT/V_Crystal/(beta)


/* Crystal growth kinetics */
#define PoreCrystalGrowthRate(s_c,beta,beta_p) \
        ((s_c) * CrystalGrowthRate(ap_AFt,dp_AFt,(beta_p)/(beta)))

#define dPoreCrystalGrowthRate(s_c,beta,beta_p) \
        ((s_c) * dCrystalGrowthRate(ap_AFt,dp_AFt,(beta_p)/(beta)) / (beta))

#define InterfaceCrystalGrowthRate(beta,beta_r) \
        (CrystalGrowthRate(ar_AFt,dr_AFt,(beta_r)/(beta)))
        
#define dInterfaceCrystalGrowthRate(beta,beta_r) \
        (dCrystalGrowthRate(ar_AFt,dr_AFt,(beta_r)/(beta)) / (beta))

//#define CrystalGrowthRate(crys,diss,ateb) \
//        ((((ateb) < 1) ? (crys) : (diss)) * (1 - (ateb)))
//#define dCrystalGrowthRate(crys,diss,ateb) \
//        ((((ateb) < 1) ? (crys) : (diss)) * (-1))

#define NA    (1)    //(-0.066666667)
#define NB    (1)    //(1.55)
#define CrystalGrowthRate(C,D,A) \
        ((((A) < 1) ? (C) : (-D)) * pow(fabs(1 - pow(A,NA)),NB))
#define dCrystalGrowthRate(C,D,A) \
        ((((A) < 1) ? (C) : (D)) * (-fabs(NA)*NB)*pow(A,NA-1)*pow(fabs(1 - pow(A,NA)),NB-1))


/* Crystal - liquid interface
 * -------------------------- */
/* Interface equilibrium saturation index */
#define InterfaceEquilibriumSaturationIndex(r) \
        (exp(2*Gamma_Crystal*V_Crystal/(RT*r)))

#define dInterfaceEquilibriumSaturationIndex(r) \
        (-2*Gamma_Crystal*V_Crystal/(RT*r*r)*InterfaceEquilibriumSaturationIndex(r))

#define InverseOfInterfaceEquilibriumSaturationIndex(b) \
        (2*Gamma_Crystal*V_Crystal/(RT*log(b)))



/* Hydrogarnet Properties (C3AH6)
 * ------------------------------ */
/* Molar volume of C3AH6 solid */
#define V_C3AH6      (149.52 * cm3)
#define C3AH6SolidContent(n,s,dt)     MAX((n + dt*r_c3ah6*(s - 1)),0.)



/* Element contents in solid phases  */
//#define CalciumContentInCHAndCSH2(zn_ca_s) (n_ca_ref*MAX(zn_ca_s,0.))
#define CalciumContentInCH(zn_ca_s)        (n_ca_ref*MAX(zn_ca_s,0.))
#define SiliconContentInCSH(zn_si_s)       (n_si_ref*MAX(zn_si_s,0.))
#define AluminiumContentInAH3(zn_al_s)     (n_al_ref*MAX(zn_al_s,0.))


/* Gypsum-based porous material properties */
/* Porosity of gypsum-based porous material (-) */
#define PHI_Gyp    (0.85)
/* Molar volume of gypsum-based porous material */
#define V_Gyp      (V_CSH2/(1 - PHI_Gyp))



/* To retrieve the material properties */
#define GetProperty(a) \
        (pm(a) < 0) ? 0 : Element_GetProperty(el)[pm(a)]



/* Fonctions */
static int     pm(const char *s) ;
static void    GetProperties(Element_t*,double) ;

static double  Radius(double,double,double,Element_t*) ;


static double TortuosityToLiquid_OhJang(double) ;
static double TortuosityToLiquid_BazantNajjar(double) ;

static void   ComputePhysicoChemicalProperties(void) ;

static double PoreWallEquilibriumSaturationIndex(double,double,double,double,double,double,double) ;
static double ElasticDamageStress(double,double) ;
static double dElasticDamageStress(double,double) ;
static double DamageStrain(double,double) ;

//#define LiquidTortuosity  TortuosityToLiquid_OhJang
#define LiquidTortuosity  TortuosityToLiquid_BazantNajjar




/* Parameters */
static double phimin = 0.01 ;
static double RT ;
static Curve_t* satcurve ;


#include "PhysicalConstant.h"
#include "Temperature.h"

void ComputePhysicoChemicalProperties(void)
{
  RT = PhysicalConstant_PerfectGasConstant * TEMPERATURE ;
}


static CementSolutionDiffusion_t* csd = NULL ;
static HardenedCementChemistry_t<double>* hcc = NULL ;


int pm(const char* s)
{
#define Parameters_Index(V)  CustomValues_Index(Parameters_t,V,double)
  if(!strcmp(s,"InitialPorosity")) {
    return (Parameters_Index(phi0)) ;
  } else if(!strcmp(s,"InitialContent_CH")) {
    return (Parameters_Index(n_ca_ref)) ;
  } else if(!strcmp(s,"InitialContent_CSH")) {
    return (Parameters_Index(n_si_ref)) ;
  } else if(!strcmp(s,"InitialContent_AH3")) {
    return (Parameters_Index(n_al_ref)) ;
  } else if(!strcmp(s,"InitialContent_CSH2")) {
    return (Parameters_Index(n_csh2_0)) ;
  } else if(!strcmp(s,"InitialContent_AFm")) {
    return (Parameters_Index(n_afm_0)) ;
  } else if(!strcmp(s,"InitialContent_AFt")) {
    return (Parameters_Index(n_aft_0)) ;
  } else if(!strcmp(s,"InitialContent_C3AH6")) {
    return (Parameters_Index(n_c3ah6_0)) ;
  } else if(!strcmp(s,"PrecipitationRate_AFm")) {
    return (Parameters_Index(r_afm)) ;
  } else if(!strcmp(s,"PrecipitationRate_AFt")) {
    return (Parameters_Index(r_aft)) ;
  } else if(!strcmp(s,"PrecipitationRate_CSH2")) {
    return (Parameters_Index(r_csh2)) ;
  } else if(!strcmp(s,"PrecipitationRate_C3AH6")) {
    return (Parameters_Index(r_c3ah6)) ;
  } else if(!strcmp(s,"PrecipitationRateAtInterface_AFt")) {
    return (Parameters_Index(ar_AFt)) ;
  } else if(!strcmp(s,"PrecipitationRateAtPoreWall_AFt")) {
    return (Parameters_Index(ap_AFt)) ;
  } else if(!strcmp(s,"DissolutionRateAtInterface_AFt")) {
    return (Parameters_Index(dr_AFt)) ;
  } else if(!strcmp(s,"DissolutionRateAtPoreWall_AFt")) {
    return (Parameters_Index(dp_AFt)) ;
  } else if(!strcmp(s,"ElasticBulkModulus")) {
    return (Parameters_Index(K_bulk)) ;
  } else if(!strcmp(s,"Strain0")) {
    return (Parameters_Index(strain0)) ;
  } else if(!strcmp(s,"Strainf")) {
    return (Parameters_Index(strainf)) ;
  } else if(!strcmp(s,"AdsorbedSulfatePerUnitMoleOfCSH_coefA")) {
    return (Parameters_Index(alphacoef)) ;
  } else if(!strcmp(s,"AdsorbedSulfatePerUnitMoleOfCSH_coefB")) {
    return (Parameters_Index(betacoef)) ;
  } else if(!strcmp(s,"BiotCoef")) {
    return (Parameters_Index(Biot)) ;
  } else return(-1) ;
#undef Parameters_Index
}


void GetProperties(Element_t* el,double t)
{
  /* To retrieve the material properties */
  Parameters_t& param = ((Parameters_t*) Element_GetProperty(el))[0] ;
  
  phi0  = param.phi0;
  n_ca_ref  = param.n_ca_ref;
  n_si_ref  = param.n_si_ref;
  n_al_ref  = param.n_al_ref;
  n_csh2_0  = param.n_csh2_0;
  n_afm_0  = param.n_afm_0;
  n_aft_0  = param.n_aft_0;
  n_c3ah6_0 = param.n_c3ah6_0;
  r_afm = param.r_afm;
  r_aft = param.r_aft;
  r_c3ah6 = param.r_c3ah6;
  r_csh2 = param.r_csh2;
  K_bulk = param.K_bulk;
  strain0 = param.strain0;
  strainf = param.strainf;
  alphacoef = param.alphacoef;
  betacoef = param.betacoef;
  Biot = param.Biot;
  ar_AFt = param.ar_AFt;
  ap_AFt = param.ap_AFt;
  dr_AFt = param.dr_AFt;
  dp_AFt = param.dp_AFt;
  
  ComputePhysicoChemicalProperties() ;

  satcurve  = Element_FindCurve(el,"S_r") ;
}


int SetModelProp(Model_t* model)
{
  Model_GetNbOfEquations(model) = NEQ ;
  
  Model_CopyNameOfEquation(model,E_SULFUR, "sulfur") ;
  Model_CopyNameOfEquation(model,E_CALCIUM,"calcium") ;
  Model_CopyNameOfEquation(model,E_CHARGE, "charge") ;
  Model_CopyNameOfEquation(model,E_POTASSIUM, "potassium") ;
  Model_CopyNameOfEquation(model,E_ALUMINIUM,"aluminium") ;
#ifdef E_ENEUTRAL
  Model_CopyNameOfEquation(model,E_ENEUTRAL,"electroneutrality") ;
#endif

#if   defined (U_LogC_H2SO4)
  Model_CopyNameOfUnknown(model,E_SULFUR,"logc_h2so4") ;
#elif defined (U_C_H2SO4)
  Model_CopyNameOfUnknown(model,E_SULFUR,"c_h2so4") ;
#elif defined (U_LogC_SO4)
  Model_CopyNameOfUnknown(model,E_SULFUR,"logc_so4") ;
#elif defined (U_C_SO4)
  Model_CopyNameOfUnknown(model,E_SULFUR,"c_so4") ;
#endif

  Model_CopyNameOfUnknown(model,E_CALCIUM,"z_ca") ;
  Model_CopyNameOfUnknown(model,E_CHARGE,    "psi") ;
  
#if   defined (U_LogC_K)
  Model_CopyNameOfUnknown(model,E_POTASSIUM,    "logc_k") ;
#elif defined (U_C_K)
  Model_CopyNameOfUnknown(model,E_POTASSIUM,    "c_k") ;
#endif

  Model_CopyNameOfUnknown(model,E_ALUMINIUM,"z_al") ;
  
#ifdef E_ENEUTRAL
  #if   defined (U_LogC_OH)
    Model_CopyNameOfUnknown(model,E_ENEUTRAL, "logc_oh") ;
  #elif defined (U_C_OH)
    Model_CopyNameOfUnknown(model,E_ENEUTRAL, "c_oh") ;
  #endif
#endif

  Model_GetComputeMaterialProperties(model) = &GetProperties;
  
  return(0) ;
}


int ReadMatProp(Material_t* mat,DataFile_t* datafile)
/* Lecture des donnees materiaux dans le fichier ficd */
{
  int  NbOfProp = ((int) sizeof(Parameters_t)/sizeof(double)) ;

  {
    /* Self-initialization */
    Material_GetProperty(mat)[pm("InitialContent_CH")]   = 1 ;
    Material_GetProperty(mat)[pm("InitialContent_CSH")]   = 1 ;
    Material_GetProperty(mat)[pm("InitialContent_AH3")]  = 1 ;
    Material_GetProperty(mat)[pm("InitialContent_CSH2")] = 0 ;
    Material_GetProperty(mat)[pm("InitialContent_AFm")]  = 0 ;
    Material_GetProperty(mat)[pm("InitialContent_AFt")]  = 0 ;
    Material_GetProperty(mat)[pm("InitialContent_C3AH6")]  = 0 ;
    Material_GetProperty(mat)[pm("PrecipitationRate_AFm")]  = 4.6e-4 ; /* 4.6e-4 (mol/L/s) Salgues 2013 */
    Material_GetProperty(mat)[pm("PrecipitationRate_AFt")]  = 4.6e-4 ;
    Material_GetProperty(mat)[pm("PrecipitationRate_C3AH6")] = 1.e-10 ;
    Material_GetProperty(mat)[pm("PrecipitationRate_CSH2")]  = 1.e-10 ;
    Material_GetProperty(mat)[pm("AdsorbedSulfatePerUnitMoleOfCSH_coefA")] = 0 ;
    Material_GetProperty(mat)[pm("AdsorbedSulfatePerUnitMoleOfCSH_coefB")]  = 0 ;
    //Material_GetProperty(mat)[pm("r0")] = 16*nm ;
    
    Material_GetProperty(mat)[pm("DissolutionRateAtInterface_AFt")] = -1 ;
    Material_GetProperty(mat)[pm("DissolutionRateAtPoreWall_AFt")] = -1 ;

    Material_ScanProperties(mat,datafile,pm) ;
    
    if(Material_GetProperty(mat)[pm("DissolutionRateAtInterface_AFt")] < 0) {
      double c = Material_GetProperty(mat)[pm("PrecipitationRateAtInterface_AFt")] ;
      
      Material_GetProperty(mat)[pm("DissolutionRateAtInterface_AFt")] = c ;
    }
    
    if(Material_GetProperty(mat)[pm("DissolutionRateAtPoreWall_AFt")] < 0) {
      double c = Material_GetProperty(mat)[pm("PrecipitationRateAtPoreWall_AFt")] ;
      
      Material_GetProperty(mat)[pm("DissolutionRateAtPoreWall_AFt")] = c ;
    }
  }

  {
    ComputePhysicoChemicalProperties() ;
  }

  {
    if(!csd) csd = CementSolutionDiffusion_Create() ;
    if(!hcc) hcc = HardenedCementChemistry_Create<double>() ;
    
    HardenedCementChemistry_SetRoomTemperature(hcc,TEMPERATURE) ;
    
    CementSolutionDiffusion_SetRoomTemperature(csd,TEMPERATURE) ;
  
    {
      Curves_t* curves = Material_GetCurves(mat) ;
      int i ;

      if((i = Curves_FindCurveIndex(curves,"S_r")) < 0) {
        arret("ReadMatProp: no cumulative pore volume fraction") ;
      }

      if((i = Curves_FindCurveIndex(curves,"X_CSH")) >= 0) {
        Curve_t* curve = Curves_GetCurve(curves) + i ;
      
        HardenedCementChemistry_SetCurveOfCalciumSiliconRatioInCSH(hcc,curve) ;
      }

      if((i = Curves_FindCurveIndex(curves,"Z_CSH")) >= 0) {
        Curve_t* curve = Curves_GetCurve(curves) + i ;
      
        HardenedCementChemistry_SetCurveOfWaterSiliconRatioInCSH(hcc,curve) ;
      }

      if((i = Curves_FindCurveIndex(curves,"S_SH")) >= 0) {
        Curve_t* curve = Curves_GetCurve(curves) + i ;
      
        HardenedCementChemistry_SetCurveOfSaturationIndexOfSH(hcc,curve) ;
      }
    }
  }

  return(NbOfProp) ;
}



int PrintModelChar(Model_t* model,FILE *ficd)
/* Saisie des donnees materiaux */
{
  
  printf(TITLE) ;
  printf("\n") ;
  
  if(!ficd) return(NEQ) ;
  
  printf("\n") ;
  printf("The 5/6 equations are:\n") ;
  printf("\t- Mass balance of S      (sulfur)\n") ;
  printf("\t- Charge balance         (charge)\n") ;
  printf("\t- Mass balance of Ca     (calcium)\n") ;
  printf("\t- Mass balance of K      (potassium)\n") ;
  printf("\t- Mass balance of Al     (aluminium)\n") ;
#ifdef E_ENEUTRAL
  printf("\t- Electroneutrality      (electroneutrality)\n") ;
#endif
  
  printf("\n") ;
  printf("The 5/6 primary unknowns are:\n") ;
  printf("\t- Sulfuric acid concentration     (c_h2so4 or logc_h2so4)\n") ;
  printf("\t- Electric potential              (psi)\n") ;
  printf("\t- Zeta unknown for calcium        (z_ca)\n") ;
  printf("\t- Potassium concentration         (c_k)\n") ;
  printf("\t- Zeta unknown for aluminium      (z_al)\n") ;
#if defined (U_C_OH)
  printf("\t- Hydroxide ion concentration     (c_oh or logc_oh)\n") ;
#endif

  printf("\n") ;
  printf("PAY ATTENTION to units : \n") ;
  printf("\t length : dm !\n") ;
  printf("\t time   : s !\n") ;

  printf("\n") ;
  printf("Some other informations\n") ;
  printf("Example of input data\n") ;
  printf("\n") ;

  fprintf(ficd,"porosity = 0.38   # Porosity\n") ;
  fprintf(ficd,"N_CH  = 6.1       # CH mole content (moles/L)\n") ;
  fprintf(ficd,"N_K   = 0.4       # K mole content  (moles/L)\n") ;
  fprintf(ficd,"N_AH3  = 0.4      # Al mole content (moles/L)\n") ;
  fprintf(ficd,"N_AFm  = 0.1      # AFm mole content (moles/L)\n") ;
  fprintf(ficd,"N_AFt  = 0.4      # AFt mole content (moles/L)\n") ;
  fprintf(ficd,"Curves = file     # Pore volume fraction curve:  r  S_r\n") ;
  fprintf(ficd,"Curves = solid    # File name: S_CH  X_CSH  Z_CSH  S_SH\n") ;

  return(NEQ) ;
}


int DefineElementProp(Element_t* el,IntFcts_t* intfcts)
{
  int nn = Element_GetNbOfNodes(el) ;
  
  mpm.DefineNbOfInternalValues(el,nn);
  
  return(0) ;
}


int  ComputeLoads(Element_t* el,double t,double dt,Load_t* cg,double* r)
/* Residu du aux chargements (r) */
{
  int nn = Element_GetNbOfNodes(el) ;
  int ndof = nn*NEQ ;
  FVM_t* fvm = FVM_GetInstance(el) ;
  int    i ;

  {
    double* r1 = FVM_ComputeSurfaceLoadResidu(fvm,cg,t,dt) ;
    
    for(i = 0 ; i < ndof ; i++) r[i] = -r1[i] ;
  }
  
  return(0) ;
}


int ComputeInitialState(Element_t* el)
/* Initialise les variables du systeme (f,va) */ 
{
  double** u = Element_ComputePointerToNodalUnknowns(el) ;
  
  /* Modifying the nodal unknowns */
  {
    int nn = Element_GetNbOfNodes(el) ;

    for(int i = 0 ; i < nn ; i++) {
      Values_d& val = *mpm.InitializeValues(el,0,i);
      
      #ifdef U_LogC_K
        LogC_K(i)   = val.U_potassium ;
      #else
        C_K(i)      = pow(10,val.U_potassium) ;
      #endif

      #ifdef E_ENEUTRAL
        #if defined (U_LogC_OH)
          LogC_OH(i) = log10(val.Concentration_oh) ;
        #elif defined (U_C_OH)
          C_OH(i)    = val.Concentration_oh ;
        #endif
      #else
        C_OH(i)    = val.Concentration_oh ;
      #endif
    }
  }
  
  {
    int i = mpm.ComputeInitialStateByFVM(el,0);
  
    return(i);
  }
}


int  ComputeExplicitTerms(Element_t* el,double t)
{
  int i = mpm.ComputeExplicitTermsByFVM(el,t);
  
  return(i);
}


int  ComputeImplicitTerms(Element_t* el,double t,double dt)
{
  int i = mpm.ComputeImplicitTermsByFVM(el,t,dt);
  
  return(i);
}



int  ComputeMatrix(Element_t* el,double t,double dt,double* k)
/* Matrice (k) */
{
  int i = mpm.ComputeMassConservationMatrixByFVM(el,t,dt,k);


  #define K(i,j)    (k[(i)*ndof + (j)])
  #if defined (U_C_H2SO4)
    {
      double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
      for(i = 0 ; i < 2*NEQ ; i++){
        K(i,E_SULFUR)     /= Ln10*C_H2SO4(0) ;
        K(i,E_SULFUR+NEQ) /= Ln10*C_H2SO4(1) ;
      }
    }
  #elif defined (U_C_SO4)
    {
      double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
      for(i = 0 ; i < 2*NEQ ; i++){
        K(i,E_SULFUR)     /= Ln10*C_SO4(0) ;
        K(i,E_SULFUR+NEQ) /= Ln10*C_SO4(1) ;
      }
    }
  #endif


  #if defined (U_C_K)
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(i = 0 ; i < 2*NEQ ; i++){
      K(i,E_POTASSIUM)     /= Ln10*C_K(0) ;
      K(i,E_POTASSIUM+NEQ) /= Ln10*C_K(1) ;
    }
  }
  #endif
  
  
  #ifdef E_ENEUTRAL
  #if defined (U_C_OH)
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(i = 0 ; i < 2*NEQ ; i++){
      K(i,E_ENEUTRAL)     /= Ln10*C_OH(0) ;
      K(i,E_ENEUTRAL+NEQ) /= Ln10*C_OH(1) ;
    }
  }
  #endif
  #endif
  #undef K

  return(0) ;
}


int  ComputeResidu(Element_t* el,double t,double dt,double* r)
{
  /* Initialization */
  {
    int ndof = Element_GetNbOfDOF(el) ;
    
    for(int i = 0 ; i < ndof ; i++) r[i] = 0. ;
  }
  /* 1. Conservation of Sulfur */
  {
    int imass = Values_Index(Mole_sulfur);
    int iflow = Values_Index(MolarFlow_sulfur[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_SULFUR,imass,iflow);
  }
  
  /* 2. Conservation of charge: div(W_q) = 0 */
  {
    int iflow = Values_Index(MolarFlow_charge[0]);
    mpm.ComputeFluxResiduByFVM(el,t,dt,r,E_CHARGE,iflow);
  }
  
  /* 3. Conservation of Calcium */
  {
    int imass = Values_Index(Mole_calcium);
    int iflow = Values_Index(MolarFlow_calcium[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_CALCIUM,imass,iflow);
  }
  
  /* 4. Conservation of Aluminium */
  {
    int imass = Values_Index(Mole_aluminium);
    int iflow = Values_Index(MolarFlow_aluminium[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_ALUMINIUM,imass,iflow);
  }
  
  /* 5. Conservation of Potassium */
  {
    int imass = Values_Index(Mole_potassium);
    int iflow = Values_Index(MolarFlow_potassium[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_POTASSIUM,imass,iflow);
  }
  
  /* 6. Electroneutrality */
  #ifdef E_ENEUTRAL
  {
    int imass = Values_Index(Mole_charge);
    mpm.ComputeBodyForceResiduByFVM(el,t,dt,r,E_ENEUTRAL,imass);
  }
  #endif
  
  return(0) ;
}


int  ComputeOutputs(Element_t* el,double t,double* s,Result_t* r)
/* Les valeurs exploitees (s) */
{
  double* f = Element_GetCurrentImplicitTerm(el) ;
  FVM_t* fvm = FVM_GetInstance(el) ;
  double** u   = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  int    nso = 59 ;
  double zero = 0 ;
  //double one = 1 ;

  /* if(Element_IsSubmanifold(el)) return(0) ; */
  
  /*
    Input data
  */
  GetProperties(el,t) ;


  /* Initialization */
  for(int i = 0 ; i < nso ; i++) {
    Result_SetValuesToZero(r + i) ;
  }


  /* output quantities */
  {
    int j = FVM_FindLocalCellIndex(fvm,s) ;
    Values_d& val = *mpm.OutputValues(el,t,j) ;
    
    /* Macros */
#define ptC(CPD)   &(HardenedCementChemistry_GetAqueousConcentrationOf(hcc,CPD))
#define ptE(CPD)   &(HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,CPD))
#define ptS(CPD)   &(HardenedCementChemistry_GetSaturationIndexOf(hcc,CPD))
#define ptPSI      &(HardenedCementChemistry_GetElectricPotential(hcc))
#define ptX_CSH    &(HardenedCementChemistry_GetCalciumSiliconRatioInCSH(hcc))
#define ptZ_CSH    &(HardenedCementChemistry_GetWaterSiliconRatioInCSH(hcc))

    int i = 0 ;
    
    {
      double ph        = - log10(*(ptC(H ))) ;
      
      Result_Store(r + i++,&ph,"ph",1) ;
    }
/*
    Result_Store(r + i++,ptC(OH),"c_oh",1) ;
    Result_Store(r + i++,ptC(H ),"c_h",1) ;
*/
    Result_Store(r + i++,ptC(Ca  ),"c_ca",1) ;
    Result_Store(r + i++,ptC(CaOH),"c_caoh",1) ;
    
    Result_Store(r + i++,ptC(H2SiO4),"c_h2sio4",1) ;
    Result_Store(r + i++,ptC(H3SiO4),"c_h3sio4",1) ;
    Result_Store(r + i++,ptC(H4SiO4),"c_h4sio4",1) ;
    
    Result_Store(r + i++,ptC(CaH2SiO4),"c_cah2sio4",1) ;
    Result_Store(r + i++,ptC(CaH3SiO4),"c_cah3sio4",1) ;
    
    Result_Store(r + i++,ptC(H2SO4),"c_h2so4",1) ;
    Result_Store(r + i++,ptC(HSO4 ),"c_hso4",1) ;
    Result_Store(r + i++,ptC(SO4  ),"c_so4",1) ;
    
    Result_Store(r + i++,ptC(CaSO4  ),"c_caso4aq",1) ;
    //Result_Store(r + i++,ptC(CaHSO4 ),"c_cahso4",1) ;
    Result_Store(r + i++,&zero,"c_cahso4",1) ;
    
    Result_Store(r + i++,ptC(Al    ),"c_al",1) ;
    Result_Store(r + i++,ptC(AlO4H4),"c_alo4h4",1) ;

    Result_Store(r + i++,ptC(K  ),"c_k",1) ;
    Result_Store(r + i++,ptC(KOH),"c_koh",1) ;

/*
    Result_Store(r + i++,(x + I_ZN_Ca_S),"zn_ca_s",1) ;
    Result_Store(r + i++,&one           ,"zn_si_s",1) ;
    Result_Store(r + i++,(x + I_ZN_Al_S),"zn_al_s",1) ;
*/
    
    Result_Store(r + i++,ptE(Ca),"C_Ca",1) ;
    Result_Store(r + i++,ptE(Si),"C_Si",1) ;
    Result_Store(r + i++,ptE(S ),"C_S",1) ;
    Result_Store(r + i++,ptE(Al),"C_Al",1) ;
    Result_Store(r + i++,ptE(K ),"C_K",1) ;
    
    Result_Store(r + i++,ptS(CH   ),"s_ch",1) ;
    Result_Store(r + i++,ptS(CSH2 ),"s_csh2",1) ;
    Result_Store(r + i++,ptS(AH3  ),"s_ah3",1) ;
    Result_Store(r + i++,ptS(AFm  ),"s_afm",1) ;
    Result_Store(r + i++,ptS(AFt  ),"s_aft",1) ;
    Result_Store(r + i++,ptS(C3AH6),"s_c3ah6",1) ;
    
    Result_Store(r + i++,&val.Mole_solidportlandite,"n_ch",1) ;
    Result_Store(r + i++,&val.Mole_solidgypsum,"n_csh2",1) ;
    Result_Store(r + i++,&val.Mole_solidCSH,"n_csh",1) ;
    Result_Store(r + i++,&val.Mole_solidAH3,"n_ah3",1) ;
    Result_Store(r + i++,&val.Mole_solidAFm,"n_afm",1) ;
    Result_Store(r + i++,&val.Mole_solidAFt,"n_aft",1) ;
    Result_Store(r + i++,&val.Mole_solidC3AH6,"n_c3ah6",1) ;
    {
      double n_csh = val.Mole_solidCSH ;
      double n_so4ads = n_csh*AdsorbedSulfatePerUnitMoleOfCSH(*ptC(SO4),*ptC(OH)) ;
      
      Result_Store(r + i++,&n_so4ads,"n_so4^ads",1) ;
    }
    
    Result_Store(r + i++,&val.Porosity,"porosite",1) ;
    Result_Store(r + i++,ptPSI,"potentiel_electrique",1) ;
    
    Result_Store(r + i++,&val.Mole_charge,"charge",1) ;
    
    Result_Store(r + i++,&val.Volume_solidCSH,"V_CSH",1) ;
    Result_Store(r + i++,ptX_CSH,"C/S",1) ;
    
    {
      CustomValues_t<double,ImplicitValues_t>* vi = (CustomValues_t<double,ImplicitValues_t>*) f ;
    
      Result_Store(r + i++,vi[0].MolarFlow_silicon+1,"W_Si",1) ;
      Result_Store(r + i++,vi[0].MolarFlow_calcium+1,"W_Ca",1) ;
      Result_Store(r + i++,vi[0].MolarFlow_sulfur+1 ,"W_S",1) ;
      Result_Store(r + i++,vi[0].MolarFlow_aluminium+1,"W_Al",1) ;
      Result_Store(r + i++,vi[0].MolarFlow_charge+1 ,"W_q",1) ;
    
      Result_Store(r + i++,&zero,"P_CSH2",1) ;
      Result_Store(r + i++,&zero,"Damage",1) ;
    
      Result_Store(r + i++,&vi[j].Mole_calcium,"N_Ca",1) ;
      Result_Store(r + i++,&vi[j].Mole_silicon,"N_Si",1) ;
      Result_Store(r + i++,&vi[j].Mole_sulfur,"N_S",1) ;
      Result_Store(r + i++,&vi[j].Mole_aluminium,"N_Al",1) ;
      Result_Store(r + i++,&vi[j].Mole_potassium,"N_K",1) ;
      Result_Store(r + i++,&vi[j].Mole_chlorine,"N_Cl",1) ;
    }
    
    Result_Store(r + i++,&val.SaturationDegree_crystal,"Saturation degree of crystal",1) ;
    
    {
      double radius = val.PoreRadius ;
      double beta = InterfaceEquilibriumSaturationIndex(radius) ;
      Result_Store(r + i++,&beta,"Interface equilibrium saturation index of AFt",1) ;
    }
    
    {
      Result_Store(r + i++,&val.SaturationIndexOfAFtAtPoreWall,"Pore wall equilibrium saturation index of AFt",1) ;
    }
    
    Result_Store(r + i++,&val.CrystalPressure,"Crystallization pressure",1) ;
    
    Result_Store(r + i++,&val.Strain,"Strain",1) ;
    
    
    if(i != nso) {
      Message_RuntimeError("ComputeOutputs: wrong number of outputs") ;
    }
  }

  return(nso) ;
}



int MPM_t::SetTangentMatrix(Element_t* el,double const& t,double const& dt,int const& i,Values_d const& val,Values_d const& dval,int const& k,double* c)
{
  int nn = Element_GetNbOfNodes(el) ;
  int ndof = nn*NEQ ;
  int    dec = NEQ*NEQ ;
  FVM_t* fvm   = FVM_GetInstance(el) ;
  double* dist = FVM_ComputeIntercellDistances(fvm) ;

  {        
    for(int j = 0 ; j < nn ; j++) {
      double* cij = c + (i*nn + j)*NEQ*NEQ ;
      double dij  = dist[nn*i + j] ;
      double dtdij = dt/dij ;
          
      /* Content terms at node i */
      if(j == i) {
        cij[E_SULFUR*NEQ    + k] = dval.Mole_sulfur ;
        cij[E_CALCIUM*NEQ   + k] = dval.Mole_calcium ;
        cij[E_POTASSIUM*NEQ + k] = dval.Mole_potassium ;
        cij[E_ALUMINIUM*NEQ + k] = dval.Mole_aluminium ;
        #ifdef E_ENEUTRAL
        //cij[E_CHARGE*NEQ    + k] = dxi[I_N_Q] ;
        cij[E_ENEUTRAL*NEQ   + k] = dval.Mole_charge ;
        #endif
      }
          
      /* Transfer terms from node i to node j: d(wij)/d(ui_k) */
      if(j != i) {
        cij[E_SULFUR*NEQ    + k] = - dtdij * dval.MolarFlow_sulfur[j] ;
        cij[E_CALCIUM*NEQ   + k] = - dtdij * dval.MolarFlow_calcium[j] ;
        cij[E_POTASSIUM*NEQ + k] = - dtdij * dval.MolarFlow_potassium[j] ;
        cij[E_ALUMINIUM*NEQ + k] = - dtdij * dval.MolarFlow_aluminium[j] ;
        cij[E_CHARGE*NEQ    + k] = - dval.MolarFlow_charge[j]/dij ;
      }
    }
  }

  return(dec) ;
}



void  MPM_t::SetIndexOfPrimaryVariables(Element_t* el,int* ind)
{
  ind[0] = Values_Index(U_sulfur);
  
  for(int k = 1 ; k < NEQ ; k++) {
    ind[k] = ind[0] + k;
  }
}



  
  

void MPM_t::SetIncrementOfPrimaryVariables(Element_t* el,double* dui)
{
  double** u = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el) ;
  int ncols = NEQ;
  int nn = Element_GetNbOfNodes(el);
    
  {
      ObVal_t* obval = Element_GetObjectiveValue(el) ;
    
      for(int i = 0 ; i < NEQ ; i++) {
        dui[i] =  1.e-2 * ObVal_GetValue(obval + i) ;
      }
      
      
      /* Derivation wrt LogC_H2SO4 or LogC_SO4 -> relative value */
      #if defined (U_C_H2SO4) || defined (U_C_SO4)
      {
        double un_sulfur = 0;
          
        for(int i = 0 ; i < nn ; i++) {
          un_sulfur += Un_Sulfur(i)/nn;
        }
        
        dui[E_SULFUR] =  1.e-2*ObVal_GetValue(obval + E_SULFUR)/(Ln10*un_sulfur) ;
      }
      #endif
      
      /* Derivation wrt LogC_K -> relative value */
      #if defined (U_C_K)
      {
        double un_potassium = 0;
          
        for(int i = 0 ; i < nn ; i++) {
          un_potassium += Un_Potassium(i)/nn;
        }
        
        dui[E_POTASSIUM] =  1.e-2*ObVal_GetValue(obval + E_POTASSIUM)/(Ln10*un_potassium) ;
      }
      #endif
    
      #ifdef E_ENEUTRAL
      /* Derivation wrt LogC_OH -> relative value */
      #if defined (U_C_OH)
      {
        double un_eneutral = 0;
          
        for(int i = 0 ; i < nn ; i++) {
          un_eneutral += Un_eneutral(i)/nn;
        }
        
        dui[E_ENEUTRAL] =  1.e-2*ObVal_GetValue(obval + E_ENEUTRAL)/(Ln10*un_eneutral) ;
      }
      #endif
      #endif
  }
}



Values_d* MPM_t::SetInputs(Element_t* el,const double& t,const int& n,double const* const* u,Values_d& val)
{
  #if defined (U_C_H2SO4) || defined (U_LogC_H2SO4)
  val.U_sulfur = LogC_H2SO4(n) ;
  #elif defined (U_C_SO4) || defined (U_LogC_SO4)
  val.U_sulfur = LogC_SO4(n) ;
  #endif
  val.U_calcium = ZN_Ca_S(n) ;
  val.U_potassium = LogC_K(n) ;
  val.U_charge = PSI(n) ;
  val.U_aluminium = ZN_Al_S(n) ;
  #if defined (E_ENEUTRAL)
  val.U_eneutral = LogC_OH(n) ;
  #endif
  
  return(&val) ;
}



Values_d* MPM_t::Integrate(Element_t* el,const double& t,const double& dt,Values_d const& val_n,Values_d& val)
/** Compute the secondary variables from the primary ones. */
{
  /* Primary variables */
  double zn_si_s    = 1 ;
  double zn_ca_s    = val.U_calcium ;
  double zn_al_s    = val.U_aluminium ;
  double psi        = val.U_charge ;
  
  /* Solve cement chemistry */
  {
    #if defined (U_C_H2SO4) || defined (U_LogC_H2SO4)
      double logc_h2so4 = val.U_sulfur ;
    #elif defined (U_C_SO4) || defined (U_LogC_SO4)
      double logc_so4 = val.U_sulfur ;
    #endif
    double logc_na    = -99 ;
    double logc_k     = val.U_potassium ;
#if defined (E_ENEUTRAL)
    double logc_oh    = val.U_eneutral ;
#else
    double logc_oh    = log10(val_n.Concentration_oh) ;
#endif
  
    HardenedCementChemistry_SetInput(hcc,SI_CH,MIN(zn_ca_s,0)) ;
    HardenedCementChemistry_SetInput(hcc,SI_CSH,MIN(zn_si_s,0)) ;
    HardenedCementChemistry_SetInput(hcc,SI_AH3,MIN(zn_al_s,0)) ;
    #if defined (U_C_H2SO4) || defined (U_LogC_H2SO4)
      HardenedCementChemistry_SetInput(hcc,LogC_H2SO4,logc_h2so4) ;
    #elif defined (U_C_SO4) || defined (U_LogC_SO4)
      HardenedCementChemistry_SetInput(hcc,LogC_SO4,logc_so4) ;
    #endif
    HardenedCementChemistry_SetInput(hcc,LogC_Na,logc_na) ;
    HardenedCementChemistry_SetInput(hcc,LogC_K,logc_k) ;
    HardenedCementChemistry_SetInput(hcc,LogC_OH,logc_oh) ;
    HardenedCementChemistry_SetElectricPotential(hcc,psi) ;
    
    HardenedCementChemistry_SetAqueousConcentrationOf(hcc,Cl,1.e-99) ;
    HardenedCementChemistry_SetLogAqueousConcentrationOf(hcc,Cl,-99) ;
  
    HardenedCementChemistry_ComputeSystem(hcc,CaO_SiO2_Na2O_K2O_SO3_Al2O3_H2O) ;

#ifndef E_ENEUTRAL
  #if (ELECTRONEUTRALITY == IMPLICIT)
    HardenedCementChemistry_SolveElectroneutrality(hcc) ;
  #endif
#endif
  }
  

  
  /* Backup */
  double c_q_l  = HardenedCementChemistry_GetLiquidChargeDensity(hcc) ;
  
  //double I = HardenedCementChemistry_GetIonicStrength(hcc) ;
  
  //double rho_l  = HardenedCementChemistry_GetLiquidMassDensity(hcc) ;
  
  double c_ca_l = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,Ca) ;
  double c_si_l = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,Si) ;
  double c_k_l  = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,K) ;
  double c_s_l  = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,S) ;
  double c_al_l = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,Al) ;
  double c_cl_l = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,Cl) ;
  
  //double s_ch   = HardenedCementChemistry_GetSaturationIndexOf(hcc,CH) ;
  //double s_sh   = HardenedCementChemistry_GetSaturationIndexOf(hcc,SH) ;
  double s_csh2 = HardenedCementChemistry_GetSaturationIndexOf(hcc,CSH2) ;
  //double s_ah3  = HardenedCementChemistry_GetSaturationIndexOf(hcc,AH3) ;
  double s_afm  = HardenedCementChemistry_GetSaturationIndexOf(hcc,AFm) ;
  double s_aft  = HardenedCementChemistry_GetSaturationIndexOf(hcc,AFt) ;
  double s_c3ah6 = HardenedCementChemistry_GetSaturationIndexOf(hcc,C3AH6) ;
  
  double c_so4  = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,SO4) ;
  //double c_oh   = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,OH) ;
       
    
  /* The crystal saturation index */
  //double beta = s_csh2 ;
  double beta = s_aft ;
  //double beta = Math_Max(s_aft,s_csh2) ;
  
  /* Compute the saturation degree of the crystal phase as a function of beta */
  double r_n = val_n.PoreRadius ;
  double r   = Radius(r_n,beta,dt,el) ;
  double s_l = LiquidSaturationDegree(r) ;
  double s_c = 1 - s_l ;
  
#ifdef E_MECH
  /* Compute the saturation index at the pore wall, beta_p */
  double beta_pn   = val_n.SaturationIndexOfAFtAtPoreWall ;
  double varphi_cn = val_n.CrystalPoreDeformation ;
  double strain_n  = val_n.Strain ;
  double straind_n = val_n.DamageStrain ;
  double beta_p = PoreWallEquilibriumSaturationIndex(beta_pn,varphi_cn,strain_n,straind_n,beta,s_c,dt) ;
  
  /* The crystallization pressure */
  double p_c       = CrystallizationPressure(beta_p) ;
  /* Compute the crystal pore deformation */
  /* Kinetic law */
  double phicrate  = PoreCrystalGrowthRate(s_c,beta,beta_p) ;
  double varphi_c  = varphi_cn + dt*phicrate ;
  /* Compute the strain */
  //double strain  = (s_c > 0) ?  varphi_c/s_c - ( p_c/N_Biot +  p_c*s_l/G_Biot) : 0 ;
  double strain    = strain_n + ((s_c > 0) ? dt*phicrate/s_c : 0) ;
  double straind   = DamageStrain(strain,straind_n) ;
#else
  double beta_p    = 1 ;
  double p_c       = 0 ;
  double varphi_c  = 0 ;
  double strain    = 0 ;
  double straind   = 0 ;
#endif
  
  
  /* Solid contents
   * -------------- */
  /* ... as components: CH, CSH2, CSH, AH3, AFm, AFt, C3AH6 */
  
  /* The crystal responsible for strains is either AFt or CSH2 (gypsum) */
  double v_crystal  =  phi0*s_c + varphi_c ;
  double n_crystal  =  v_crystal/V_Crystal ;

  double n_aftn     = val_n.Mole_solidAFt ;
  //double n_aft      = AFtSolidContent(n_aftn,s_aft,dt) ;
  double n_aft      = v_crystal/V_AFt ;  
  //double n_aft      = (s_aft > s_csh2) ? n_crystal : 0 ;
  
  double n_csh2n    = val_n.Mole_solidgypsum ;
  double n_csh2     = CSH2SolidContent(n_csh2n,s_csh2,dt) ;
  //double n_csh2     = v_crystal/V_CSH2 ;
  //double n_csh2     = (s_aft > s_csh2) ? 0 : n_crystal ;
  
  //double n_chn      = x[I_N_CHn] ;
  double n_ch       = CHSolidContent(zn_ca_s) ;
  double n_ah3      = AH3SolidContent(zn_al_s) ;
  double n_afmn     = val_n.Mole_solidAFm ;
  double n_afm      = AFmSolidContent(n_afmn,s_afm,dt) ;
  double n_c3ah6n   = val_n.Mole_solidC3AH6 ;
  double n_c3ah6    = C3AH6SolidContent(n_c3ah6n,s_c3ah6,dt) ;
  double n_csh      = CSHSolidContent(zn_si_s) ;
  double n_so4ads   = n_csh * AdsorbedSulfatePerUnitMoleOfCSH(c_so4,c_oh) ;
  
  /* ... as elements: S, Ca, Si, Al */
  //double x_csh      = CalciumSiliconRatioInCSH(s_ch) ;
  double x_csh      = HardenedCementChemistry_GetCalciumSiliconRatioInCSH(hcc) ;
  double n_si_s     = n_csh ;
  double n_ca_s     = n_ch + n_csh2 + x_csh*n_csh + 4*n_afm + 6*n_aft + 3*n_c3ah6 ;
  double n_s_s      = n_csh2 + n_afm + 3*n_aft + n_so4ads ;
  double n_al_s     = 2*(n_ah3 + n_afm + n_aft + n_c3ah6) ;
  /* ... as volume */
  double v_csh      = MolarVolumeOfCSH(x_csh) ;
  //double v_gyp      = V_Gyp*n_csh2 ;
  //double v_csh2     = V_CSH2*n_csh2 ;
  double v_cem      = V_CH*n_ch + v_csh*n_csh + V_AH3*n_ah3 + V_AFm*n_afm + V_AFt*n_aft + V_C3AH6*n_c3ah6 + V_CSH2*n_csh2 ;


  /* Porosities in unconfined conditions (no pressure) */
  double v_cem0     = val_n.InitialVolume_solidtotal ;
  /* ... of concrete */
  double phi_con    = phi0 + v_cem0 - v_cem ;
  /* ... of gypsum */
  //double phi_gyp    = PHI_Gyp ;


  /* Total porosity */
  double varphi     = strain ;
  double phi_c      = phi_con + varphi ;
  //double phi_t      = phi_con - v_csh2 ;
  double phi_t      = MAX(phi_c,phimin) ;
  

#if (U_PHI == IMPLICIT)
  double phi_l        = phi_t ;
#else
  double phi_l        = val_n.Porosity ;
#endif
    
    
  /* Liquid contents 
   * --------------- */
  /* ... as elements: S, Ca, Si, K, Cl, Al */
  double n_s_l  = phi_l*c_s_l ;
  double n_ca_l = phi_l*c_ca_l ;
  double n_si_l = phi_l*c_si_l ;
  double n_al_l = phi_l*c_al_l ;
  double n_k_l  = phi_l*c_k_l ;
  double n_cl_l = phi_l*c_cl_l ;
  double n_q_l  = phi_l*c_q_l ;


#ifndef E_ENEUTRAL
  #if (ELECTRONEUTRALITY == EXPLICIT)
  c_q_l = HardenedCementChemistry_SolveExplicitElectroneutrality(hcc) ;
  n_q_l = phi_l*c_q_l ;
  #endif
#endif

  

  /* Back up */


  /* Solid components */
  val.Mole_solidportlandite = n_ch ;
  val.Mole_solidgypsum = n_csh2 ;
  val.Mole_solidAH3 = n_ah3 ;
  val.Mole_solidAFm = n_afm ;
  val.Mole_solidAFt = n_aft ;
  val.Mole_solidC3AH6 = n_c3ah6 ;
  val.Mole_solidCSH = n_csh ;
  
  val.Mole_solidCa = n_ca_s;
  val.Mole_liquidCa = n_ca_l;
  
  //x[I_ZN_Ca_S   ] = zn_ca_s ;  
  //x[I_ZN_Al_S   ] = zn_al_s ;  
  
  val.Volume_solidCSH = v_csh ;
  
  val.Volume_solidtotal = v_cem ;
  
  
  /* Porosities */
  val.Porosity = phi_t ;
  //x[I_PHI_C     ] = phi_c ;
  
  /* Saturation degree of crystal */
  val.SaturationDegree_crystal = s_c ;
  
  /* Crystallization pressure */
  val.CrystalPressure = p_c ;
  
  /* Radius */
  val.PoreRadius = r ;
  
  /* Strains */
  val.SaturationIndexOfAFtAtPoreWall = beta_p ;
  val.Strain = strain ;
  val.DamageStrain = straind ;
  val.CrystalPoreDeformation = varphi_c ;
  
  
  
  /* Element contents */
  val.Mole_sulfur = n_s_l  + n_s_s ;
  val.Mole_calcium = n_ca_l + n_ca_s ;
  val.Mole_silicon = n_si_l + n_si_s ;
  val.Mole_potassium = n_k_l  ;
  val.Mole_chlorine = n_cl_l  ;
  val.Mole_aluminium = n_al_l + n_al_s ;

  /* Charge density */
  val.Mole_charge = n_q_l ;
  
  /* Hydroxide ion concentration */
  val.Concentration_oh  = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,OH) ;

  /* Chemical potentials */
  HardenedCementChemistry_CopyChemicalPotential(hcc,val.ChemicalPotential) ;
  
  /*
    Transfer coefficients
  */
  {    
    /* Liquid tortuosity */
    {
      double phi    = val.Porosity ;
        
      val.Tortuosity_liquid = LiquidTortuosity(phi) ;
    }
    
    /* Concentrations */
    {
      double* c = HardenedCementChemistry_GetAqueousConcentration(hcc) ;
      int n = HardenedCementChemistry_NbOfConcentrations ;
    
      for(int j = 0 ; j < n ; j++) {
        val.AqueousConcentration[j] = c[j] ;
      }
    }
  }
  
  return(&val);
}



Values_d*  MPM_t::SetFluxes(Element_t* el,double const& t,int const& i,int const& j,Values_d const& grdval,Values_d* val)
{
  Values_d& vali = val[i];
  Values_d& valj = val[j];
  CustomValues_t<double,ExplicitValues_t> valij ;
  
  {
    double* va = Element_GetExplicitTerm(el) ;
    CustomValues_t<double,ExplicitValues_t>* vale = (CustomValues_t<double,ExplicitValues_t>*) va ;
    
    valij = 0.5 * (vale[i] + vale[j]);
  }
    
  /* Diffusion in the cement solution */
  {
    /* Gradients */
    {
      double* g = CementSolutionDiffusion_GetGradient(csd) ;
      int n = CementSolutionDiffusion_NbOfConcentrations ;
      double* cij = valij.AqueousConcentration ;
      double tortuosity = valij.Tortuosity_liquid ;
      
      for(int k = 0 ; k < n ; k++) {
        double rho = cij[k] ;
      
        g[k] = tortuosity * rho * grdval.ChemicalPotential[k] ;
      }
    }
    /* Fluxes */
    {
      
      CementSolutionDiffusion_ComputeFluxes(csd) ;
      
      vali.MolarFlow_calcium[j]   = CementSolutionDiffusion_GetElementFluxOf(csd,Ca) ;
      vali.MolarFlow_silicon[j]   = CementSolutionDiffusion_GetElementFluxOf(csd,Si) ;
      vali.MolarFlow_sulfur[j]    = CementSolutionDiffusion_GetElementFluxOf(csd,S) ;
      vali.MolarFlow_potassium[j] = CementSolutionDiffusion_GetElementFluxOf(csd,K) ;
      vali.MolarFlow_chlorine[j]  = CementSolutionDiffusion_GetElementFluxOf(csd,Cl) ;
      vali.MolarFlow_aluminium[j] = CementSolutionDiffusion_GetElementFluxOf(csd,Al) ;
      vali.MolarFlow_charge[j]    = CementSolutionDiffusion_GetIonCurrent(csd) ;
    }
  }
  
  {
    valj.MolarFlow_calcium[i]   = - vali.MolarFlow_calcium[j]  ;
    valj.MolarFlow_silicon[i]   = - vali.MolarFlow_silicon[j]  ;
    valj.MolarFlow_sulfur[i]    = - vali.MolarFlow_sulfur[j]  ;
    valj.MolarFlow_charge[i]    = - vali.MolarFlow_charge[j]  ;
    valj.MolarFlow_potassium[i] = - vali.MolarFlow_potassium[j]  ;
    valj.MolarFlow_aluminium[i] = - vali.MolarFlow_aluminium[j]  ;
    valj.MolarFlow_chlorine[i]  = - vali.MolarFlow_chlorine[j]  ;
    valj.MolarFlow_sulfur[i]    = - vali.MolarFlow_sulfur[j]  ;
  }
    
  return(val+i) ;
}






Values_d*  MPM_t::Initialize(Element_t* el,double const& t,Values_d& val)
{
  {    
    {
      double zn_ca_s    = val.U_calcium ;
      double zn_si_s    = 1 ;
      double zn_al_s    = val.U_aluminium ;
  
      /* Solve cement chemistry */
      {
        #if defined (U_C_H2SO4) || defined (U_LogC_H2SO4)
          double logc_h2so4 = val.U_sulfur ;
        #elif defined (U_C_SO4) || defined (U_LogC_SO4)
          double logc_so4 = val.U_sulfur ;
        #endif
        double logc_na    = -99 ;
        double logc_k     = val.U_potassium ;
        double logc_oh    = val.U_eneutral ;
  
        HardenedCementChemistry_SetInput(hcc,SI_CH,MIN(zn_ca_s,0)) ;
        HardenedCementChemistry_SetInput(hcc,SI_CSH,MIN(zn_si_s,0)) ;
        HardenedCementChemistry_SetInput(hcc,SI_AH3,MIN(zn_al_s,0)) ;
        #if defined (U_C_H2SO4) || defined (U_LogC_H2SO4)
          HardenedCementChemistry_SetInput(hcc,LogC_H2SO4,logc_h2so4) ;
        #elif defined (U_C_SO4) || defined (U_LogC_SO4)
          HardenedCementChemistry_SetInput(hcc,LogC_SO4,logc_so4) ;
        #endif
        HardenedCementChemistry_SetInput(hcc,LogC_Na,logc_na) ;
        HardenedCementChemistry_SetInput(hcc,LogC_K,logc_k) ;
        HardenedCementChemistry_SetInput(hcc,LogC_OH,logc_oh) ;
    
        HardenedCementChemistry_SetAqueousConcentrationOf(hcc,Cl,1.e-99) ;
        HardenedCementChemistry_SetLogAqueousConcentrationOf(hcc,Cl,-99) ;
  
        HardenedCementChemistry_ComputeSystem(hcc,CaO_SiO2_Na2O_K2O_SO3_Al2O3_H2O) ;
      
        HardenedCementChemistry_SolveElectroneutrality(hcc) ;
      }
    
  
      {
        /* Liquid components 
         * ----------------- */
        double x_csh      = HardenedCementChemistry_GetCalciumSiliconRatioInCSH(hcc) ;
        //double s_ch   = HardenedCementChemistry_GetSaturationIndexOf(hcc,CH) ;
        //double s_csh2 = HardenedCementChemistry_GetSaturationIndexOf(hcc,CSH2) ;
        double c_oh   = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,OH) ;
    
        /* Solid contents 
         * -------------- */
        /* ... as components: CH, CSH2, CSH, AH3, AFm, AFt, C3AH6 */
        double n_ch       = CHSolidContent(zn_ca_s) ;
        double n_csh2     = n_csh2_0 ;
        double n_ah3      = AH3SolidContent(zn_al_s) ;
        double n_afm      = n_afm_0 ;
        double n_aft      = n_aft_0 ;
        double n_c3ah6    = n_c3ah6_0 ;
        double n_csh      = CSHSolidContent(zn_si_s) ;
        /* ... as volume */
        double v_csh      = MolarVolumeOfCSH(x_csh) ;
        double v_cem      = V_CH*n_ch + v_csh*n_csh + V_AH3*n_ah3 + V_AFm*n_afm + V_AFt*n_aft + V_C3AH6*n_c3ah6 + V_CSH2*n_csh2 ;

        /* Porosity */
        double phi_c = phi0 ;
        //double phi   = phi_c - V_CSH2*n_csh2 ;
        double phi   = phi_c ;
    

        /* Back up what is needed to compute components */
        val.Mole_solidportlandite = n_ch ;
        val.Mole_solidgypsum  = n_csh2 ;
        val.Mole_solidAFm   = n_afm ;
        val.Mole_solidAFt   = n_aft ;
        val.Mole_solidC3AH6 = n_c3ah6 ;
        val.Porosity     = phi ;
        //PHI_C(i)   = phi_c ;

        val.InitialVolume_solidtotal  = v_cem ;
  
        val.Concentration_oh  = c_oh; ;
      }
    
      {
        val.PoreRadius = PoreEntryRadiusMax ;
        val.SaturationIndexOfAFtAtPoreWall = 1 ;
        val.Strain = 0 ;
        val.DamageStrain = strain0 ;
        val.CrystalPoreDeformation = 0 ;
      }

    }
  }
  
  return(&val);
}




double TortuosityToLiquid_OhJang(double phi)
/* Ref:
 * Byung Hwan Oh, Seung Yup Jang, 
 * Prediction of diffusivity of concrete based on simple analytic equations, 
 * Cement and Concrete Research 34 (2004) 463 - 480.
 * tau = (m_p + sqrt(m_p**2 + phi_c/(1 - phi_c) * (Ds/D0)**(1/n)))**n
 * m_p = 0.5 * ((phi_cap - phi_c) + (Ds/D0)**(1/n) * (1 - phi_c - phi_cap)) / (1 - phi_c)
 */
{
  double phi_cap = 0.5 * phi  ;
  double phi_c   = 0.17 ;         /* Percolation capilar porosity */
  double n       = 2.7 ;          /* OPC n  = 2.7  --------  Fly ash n  = 4.5 */
  double ds      = 1.e-4 ;        /* OPC ds = 1e-4 --------  Fly ash ds = 5e-5 */
  double dsn     = pow(ds,1/n) ;
  double m_phi   = 0.5 * ((phi_cap - phi_c) + dsn * (1 - phi_c - phi_cap)) / (1 - phi_c) ;
  double tausat  = pow(m_phi + sqrt(m_phi*m_phi + dsn * phi_c/(1 - phi_c)),n) ;
  
  double tau =  tausat ;
    
  return tau ;
}




double TortuosityToLiquid_BazantNajjar(double phi)
/* Ref:
 * Z. P. BAZANT, L.J. NAJJAR,
 * Nonlinear water diffusion in nonsaturated concrete,
 * Materiaux et constructions, 5(25), 1972.
 */
{
  double iff = 2.9e-4 * exp(9.95 * phi) ;
  double tausat = (iff < 1) ? iff : 1 ;
    
  return(tausat) ;
}



double Radius(double r_n,double beta,double dt,Element_t* el)
{
  double r_max  = PoreEntryRadiusMax ;
  double r = r_n ;
  
  //if(beta < 1) return(r_max) ;
  
  {
    double s_ln     = LiquidSaturationDegree(r_n) ;
    //double beta_r_in  = InterfaceEquilibriumSaturationIndex(r_n) ;
    double beta_r_min = InterfaceEquilibriumSaturationIndex(r_max) ;
    //double beta_r_inf = (beta > beta_r_min) ? beta : beta_r_min ;
    double r_inf = (beta > beta_r_min) ? InverseOfInterfaceEquilibriumSaturationIndex(beta) : r_max ;
    //double s_linf  = LiquidSaturationDegree(r_inf) ;
    int iterations = 40 ;
    double tol = 1.e-6 ;
    int i ;
    
    if(r_n == r_inf) return(r_n) ;
    
    for(i = 0 ; i < iterations ; i++) {
      double beta_r  =  InterfaceEquilibriumSaturationIndex(r) ;
      double dbeta_r = dInterfaceEquilibriumSaturationIndex(r) ;
      //double dbeta_r = (beta_r_inf - beta_r_in)/(r_inf - r_n) ;
      
      double s_l   =  LiquidSaturationDegree(r) ;
      double ds_l  = dLiquidSaturationDegree(r) ;
      //double ds_l  = (s_linf - s_ln)/(r_inf - r_n) ;
      
      /* Kinetic law */
      /* Modified 03/06/2017 */
      double  scrate =  InterfaceCrystalGrowthRate(beta,beta_r) ;
      double dscrate = dInterfaceCrystalGrowthRate(beta,beta_r)*dbeta_r ;
      double  eq   =  s_l - s_ln + dt*scrate ;
      double deq   = ds_l        + dt*dscrate ;
      
      double dr    = (fabs(deq) > 0) ? - eq/deq : 0. ;
      
      /* The solution r should be in the range between r_n and r_inf.
       * So let us assume that, at a given iteration, an estimation r
       * has been found between r_n and r_inf. Then we look for a new 
       * estimation r + dr in the range between r_0 and r_1 by using
       * an under-relaxation technique so that dr should be given by 
       *         r + dr = a*r_1 + (1 - a)*r_0
       * with a being in the range between 0 and 1.
       * The under-relaxation technique is such that r_0 should be in 
       * the range between r_n and r and r_1 should be in the range 
       * between r_inf and r i.e
       *         r_0 = b0*r_n   + (1 - b0)*r
       *         r_1 = b1*r_inf + (1 - b1)*r
       * with b0 and b1 being in between 0 and 1. So we get
       *         dr = a*b1*(r_inf - r) + (1 - a)*b0*(r_n - r)
       * If b0=b1=b then
       *         dr = (a*(r_inf - r) + (1 - a)*(r_n - r))*b
       * The bigger b the larger the range where r is looked for, without
       * exceding the initial range defined by r_n and r_inf.
       * The value b=0.5 corresponds to half the initial range.
       */
      {
        /* The predicted a is computed from the predicted dr */
        //double b = 0.5 ;
        double b = 0.5 ;
        double a = (dr/b - r_n + r)/(r_inf - r_n) ;
       
        /* If the predicted a is < 0 then the used value is set to 0 */
        if(a < 0) a = 0 ;
      
        /* if the predicted a is > 1 then the used value is set to 1 */
        if(a > 1) a = 1 ;
        
        {
          dr = b*(a*(r_inf - r) + (1 - a)*(r_n - r)) ;
        }
        
        /*
        printf("\n") ;
        printf("a      = %e\n",a) ;
        printf("s_ln   = %e, ",s_ln) ;
        printf("s_linf = %e, ",s_linf) ;
        printf("s_l    = %e\n",s_l) ;
        printf("ds_l   = %e, ",ds_l) ;
        printf("dbeta_r= %e\n",dbeta_r) ;
        printf("r_n    = %e, ",r_n) ;
        printf("r_inf  = %e, ",r_inf) ;
        printf("r      = %e\n",r) ;
        */
      }
      
      r += dr ;
      if(fabs(dr/r_n) < tol) {
        return(r) ;
      }
    }
  }
  
  Message_Warning("Radius: not converged") ;
  Exception_Interrupt ;

  return(r) ;
}




double PoreWallEquilibriumSaturationIndex(double beta_pn,double varphi_cn,double strain_n,double straind_n,double beta,double sc,double dt)
{
  double beta_p   = beta_pn ;
  //double sl       = 1 - sc ;
  
  {
    int iterations = 40 ;
    double tol = 1.e-6 ;
    int i ;
    
    for(i = 0 ; i < iterations ; i++) {
      double phicrate  =  PoreCrystalGrowthRate(sc,beta,beta_p) ;
      double dphicrate = dPoreCrystalGrowthRate(sc,beta,beta_p) ;
      double pc        = CrystallizationPressure(beta_p) ;
      double dpc       = dCrystallizationPressure(beta_p) ;
      //double varphi_c  = varphi_cn + dt*phicrate ;
      //double dvarphi_c =             dt*dphicrate ;
      double strain    = strain_n + ((sc > 0) ? dt*phicrate/sc : 0) ;
      double dstrain   = (sc > 0) ? dt*dphicrate/sc : 0 ;
      //double strain  = (sc > 0) ?  varphi_c/sc - ( pc/N_Biot +  pc*sl/G_Biot) : 0 ;
      //double dstrain = (sc > 0) ? dvarphi_c/sc - (dpc/N_Biot + dpc*sl/G_Biot) : 0 ;
      /* Effective stress */
      double stress  =  ElasticDamageStress(strain,straind_n) ;
      double dstress = dElasticDamageStress(strain,straind_n) ;
      double straind = DamageStrain(strain,straind_n) ;
      double d       = Damage(straind) ;
      double bd      = Biot + d * (1 - Biot) ;
      double eq      = stress - bd*sc*pc ;
      /* We don't derive bd */
      double deq     = dstress*dstrain - bd*sc*dpc ;
      double dbeta_p = (fabs(deq) > 0) ? - eq/deq : 0. ;
      
      if(dbeta_p < - 0.5*beta_p) dbeta_p = - 0.5*beta_p ;
      
      beta_p += dbeta_p ;
          
      //if(beta_p < 1) beta_p = 1 ;
      
      if(fabs(dbeta_p/beta_pn) < tol) {
        return(beta_p) ;
      }
    }
  }
  
  Message_Direct("PoreWallEquilibriumSaturationIndex: not converged\n") ;
  Message_Direct("beta_p = %e ; beta_pn = %e\n",beta_p,beta_pn) ;
  Exception_Interrupt ;
  
  return(-1) ;
}




double DamageStrain(double strain,double straind)
{
  //double Y = 0.5*strain*K_bulk*strain ;
  //double K = 0.5*straind*K_bulk*straind ;
  //double crit = Y - K ;
  double crit = strain - straind ;
  
  if(crit > 0) {
    straind = strain ;
  }
  
  return(straind) ;
}



double ElasticDamageStress(double strain,double straind_n)
{
  double straind = DamageStrain(strain,straind_n) ;
  double d       = Damage(straind) ;
  double Kd      = (1 - d) * K_bulk ;
  double stress  = Kd * strain ;
  
  return(stress) ;
}



double dElasticDamageStress(double strain,double straind_n)
{
  double dstrain = 1.e-4 * strain ;
  double a       = 0.5 ;
  double strain2 = strain - (1 - a) * dstrain ;
  double stress2 = ElasticDamageStress(strain2,straind_n) ;
  double strain1 = strain + a * dstrain ;
  double stress1 = ElasticDamageStress(strain1,straind_n) ;
  double dstress = (stress1 - stress2) / dstrain ;
  
  return(dstress) ;
}
