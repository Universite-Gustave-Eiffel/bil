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

#include "PredefinedMethods.h"



/* Indices of equations/unknowns */
enum {
  E_Sulfur,
  E_charge,
  E_Calcium,
  E_Potassium,
  E_Aluminium,
  /* Uncomment/comment the two next lines to consider/suppress electroneutrality */
  E_eneutral,
  #define E_eneutral E_eneutral 
  E_Last
} ;

#define E_Mech



/* Nb of equations */
#define NbOfEquations      E_Last
#define NEQ                NbOfEquations





/* Value of the nodal unknown (u and el must be used below) */
#define UNKNOWN(n,i)     Element_GetValueOfNodalUnknown(el,u,n,i)
#define UNKNOWNn(n,i)    Element_GetValueOfNodalUnknown(el,u_n,n,i)


/* Generic names of nodal unknowns */
#define U_Sulfur(n)    (UNKNOWN(n,E_Sulfur))
#define Un_Sulfur(n)   (UNKNOWNn(n,E_Sulfur))

#define U_charge(n)     (UNKNOWN(n,E_charge))
#define Un_charge(n)    (UNKNOWNn(n,E_charge))

#define U_Calcium(n)    (UNKNOWN(n,E_Calcium))
#define Un_Calcium(n)   (UNKNOWNn(n,E_Calcium))

#define U_Potassium(n)  (UNKNOWN(n,E_Potassium))
#define Un_Potassium(n) (UNKNOWNn(n,E_Potassium))

#define U_Aluminium(n)  (UNKNOWN(n,E_Aluminium))
#define Un_Aluminium(n) (UNKNOWNn(n,E_Aluminium))

#define U_eneutral(n)   (UNKNOWN(n,E_eneutral))
#define Un_eneutral(n)  (UNKNOWNn(n,E_eneutral))




/* Method chosen at compiling time. 
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
#ifdef E_eneutral
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
  #define LogC_H2SO4n(n)  Un_Sulfur(n)
  #define C_H2SO4(n)      (pow(10,LogC_H2SO4(n)))
  #define C_H2SO4n(n)     (pow(10,LogC_H2SO4n(n)))
#elif defined (U_C_H2SO4) && !defined (U_LogC_H2SO4) && !defined (U_LogC_SO4) && !defined (U_C_SO4)
  #define C_H2SO4(n)      U_Sulfur(n)
  #define C_H2SO4n(n)     Un_Sulfur(n)
  #define LogC_H2SO4(n)   (log10(C_H2SO4(n)))
  #define LogC_H2SO4n(n)  (log10(C_H2SO4n(n)))
#elif defined (U_LogC_SO4) && !defined (U_C_SO4) && !defined (U_LogC_H2SO4) && !defined (U_C_H2SO4)
  #define LogC_SO4(n)     U_Sulfur(n)
  #define LogC_SO4n(n)    Un_Sulfur(n)
  #define C_SO4(n)        (pow(10,LogC_SO4(n)))
  #define C_SO4n(n)       (pow(10,LogC_SO4n(n)))
#elif defined (U_C_SO4) && !defined (U_LogC_SO4) && !defined (U_LogC_H2SO4) && !defined (U_C_H2SO4)
  #define C_SO4(n)        U_Sulfur(n)
  #define C_SO4n(n)       Un_Sulfur(n)
  #define LogC_SO4(n)     (log10(C_SO4(n)))
  #define LogC_SO4n(n)    (log10(C_SO4n(n)))
#else
  #error "Ambiguous or undefined unknown"
#endif



#define ZN_Ca_S(n)   U_Calcium(n)
#define ZN_Ca_Sn(n)  Un_Calcium(n)

#define PSI(n)       U_charge(n)
#define PSIn(n)      Un_charge(n)

#if defined (U_LogC_K) && !defined (U_C_K)
  #define LogC_K(n)     U_Potassium(n)
  #define LogC_Kn(n)    Un_Potassium(n)
  #define C_K(n)        (pow(10,LogC_K(n)))
  #define C_Kn(n)       (pow(10,LogC_Kn(n)))
#elif defined (U_C_K) && !defined (U_LogC_K)
  #define C_K(n)        U_Potassium(n)
  #define C_Kn(n)       Un_Potassium(n)
  #define LogC_K(n)     (log10(C_K(n)))
  #define LogC_Kn(n)    (log10(C_Kn(n)))
#else
  #error "Ambiguous or undefined unknown"
#endif

#define ZN_Al_S(n)   U_Aluminium(n)
#define ZN_Al_Sn(n)  Un_Aluminium(n)

#ifdef E_eneutral
  #if defined (U_LogC_OH) && !defined (U_C_OH)
    #define LogC_OH(n)   U_eneutral(n)
    #define LogC_OHn(n)  Un_eneutral(n)
    #define C_OH(n)      (pow(10,LogC_OH(n)))
    #define C_OHn(n)     (pow(10,LogC_OHn(n)))
  #elif defined (U_C_OH) && !defined (U_LogC_OH)
    #define C_OH(n)      U_eneutral(n)
    #define C_OHn(n)     Un_eneutral(n)
    #define LogC_OH(n)   (log10(C_OH(n)))
    #define LogC_OHn(n)  (log10(C_OHn(n)))
  #else
    #error "Ambiguous or undefined unknown"
  #endif
#endif




/* Nb of nodes (el must be used below) */
#define NN     Element_GetNbOfNodes(el)

/* Nb of implicit terms */
#define NVI     (7*NN*NN + 14*NN)



/* Names used for implicit terms */
#define MassAndFlux(f,i,j)  ((f)[((i)*NN + (j))])


#define NW_S          (f)
#define NW_Sn         (f_n)
#define N_S(i)        MassAndFlux(NW_S,i,i)
#define N_Sn(i)       MassAndFlux(NW_Sn,i,i)
#define W_S(i,j)      MassAndFlux(NW_S,i,j)

#define NW_q          (f   + NN*NN)
#define NW_qn         (f_n + NN*NN)
#define N_q(i)        MassAndFlux(NW_q,i,i)
#define N_qn(i)       MassAndFlux(NW_qn,i,i)
#define W_q(i,j)      MassAndFlux(NW_q,i,j)

#define NW_Ca         (f   + 2*NN*NN)
#define NW_Can        (f_n + 2*NN*NN)
#define N_Ca(i)       MassAndFlux(NW_Ca,i,i)
#define N_Can(i)      MassAndFlux(NW_Can,i,i)
#define W_Ca(i,j)     MassAndFlux(NW_Ca,i,j)

#define NW_K          (f   + 3*NN*NN)
#define NW_Kn         (f_n + 3*NN*NN)
#define N_K(i)        MassAndFlux(NW_K,i,i)
#define N_Kn(i)       MassAndFlux(NW_Kn,i,i)
#define W_K(i,j)      MassAndFlux(NW_K,i,j)

#define NW_Si         (f   + 4*NN*NN)
#define NW_Sin        (f_n + 4*NN*NN)
#define N_Si(i)       MassAndFlux(NW_Si,i,i)
#define N_Sin(i)      MassAndFlux(NW_Sin,i,i)
#define W_Si(i,j)     MassAndFlux(NW_Si,i,j)

#define NW_Al         (f   + 5*NN*NN)
#define NW_Aln        (f_n + 5*NN*NN)
#define N_Al(i)       MassAndFlux(NW_Al,i,i)
#define N_Aln(i)      MassAndFlux(NW_Aln,i,i)
#define W_Al(i,j)     MassAndFlux(NW_Al,i,j)

#define NW_Cl         (f   + 6*NN*NN)
#define NW_Cln        (f_n + 6*NN*NN)
#define N_Cl(i)       MassAndFlux(NW_Cl,i,i)
#define N_Cln(i)      MassAndFlux(NW_Cln,i,i)
#define W_Cl(i,j)     MassAndFlux(NW_Cl,i,j)


#define N_CH(i)       (f   + 7*NN*NN)[i]
#define N_CHn(i)      (f_n + 7*NN*NN)[i]

#define N_CSH2(i)     (f   + 7*NN*NN + NN)[i]
#define N_CSH2n(i)    (f_n + 7*NN*NN + NN)[i]

#define N_AH3(i)      (f   + 7*NN*NN + 2*NN)[i]
#define N_AH3n(i)     (f_n + 7*NN*NN + 2*NN)[i]

#define N_AFm(i)      (f   + 7*NN*NN + 3*NN)[i]
#define N_AFmn(i)     (f_n + 7*NN*NN + 3*NN)[i]

#define N_AFt(i)      (f   + 7*NN*NN + 4*NN)[i]
#define N_AFtn(i)     (f_n + 7*NN*NN + 4*NN)[i]

#define N_C3AH6(i)    (f   + 7*NN*NN + 5*NN)[i]
#define N_C3AH6n(i)   (f_n + 7*NN*NN + 5*NN)[i]

#define PHI(i)        (f   + 7*NN*NN + 6*NN)[i]
#define PHIn(i)       (f_n + 7*NN*NN + 6*NN)[i]

#define PHI_C(i)      (f   + 7*NN*NN + 7*NN)[i]
#define PHI_Cn(i)     (f_n + 7*NN*NN + 7*NN)[i]


#ifndef E_eneutral
  #define C_OH(i)     (f   + 7*NN*NN + 8*NN)[i]
  #define C_OHn(i)    (f_n + 7*NN*NN + 8*NN)[i]
  
  #define LogC_OH(n)  (log10(C_OH(n)))
  #define LogC_OHn(n) (log10(C_OHn(n)))
#endif


#define PoreRadius(i)  (f   + 7*NN*NN + 9*NN)[i]
#define PoreRadiusn(i) (f_n + 7*NN*NN + 9*NN)[i]


#define Beta_p(i)      (f   + 7*NN*NN + 10*NN)[i]
#define Beta_pn(i)     (f_n + 7*NN*NN + 10*NN)[i]

#define Strain(i)      (f   + 7*NN*NN + 11*NN)[i]
#define Strain_n(i)    (f_n + 7*NN*NN + 11*NN)[i]

#define Straind(i)     (f   + 7*NN*NN + 12*NN)[i]
#define Straind_n(i)   (f_n + 7*NN*NN + 12*NN)[i]

#define VarPHI_C(i)    (f   + 7*NN*NN + 13*NN)[i]
#define VarPHI_Cn(i)   (f_n + 7*NN*NN + 13*NN)[i]





/* Nb of explicit terms */
#define NVE     ((1 + CementSolutionDiffusion_NbOfConcentrations)*NN + 1)
/* Names used for explicit terms */
#define TransferCoefficient(f,n)  ((f) + (n)*NN)

#define TORTUOSITY        TransferCoefficient(va,0)

#define CONCENTRATION(i)  (TransferCoefficient(va,1) + (i)*CementSolutionDiffusion_NbOfConcentrations)

#define StrainYY          (va + (1 + CementSolutionDiffusion_NbOfConcentrations)*NN)[0]



/* Nb of constant terms */
#define NV0     (2)
/* Names used for constant terms */
#define V_Cem0(n)   (v0[(0+n)])





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
static void    GetProperties(Element_t*) ;


static double* ComputeVariables(Element_t*,double**,double**,double*,double,double,int) ;
static void    ComputeSecondaryVariables(Element_t*,double,double,double*,double*) ;
static double* ComputeVariableDerivatives(Element_t*,double,double,double*,double,int) ;


static void    ComputeTransferCoefficients(Element_t*,double**,double*) ;
static double* ComputeVariableFluxes(Element_t*,int,int) ;
static double* ComputeFluxes(Element_t*,double*,int,int) ;
//static double* ComputeVariableFluxDerivatives(Element_t*,int,int,double) ;

static int     TangentCoefficients(Element_t*,double,double,double*) ;


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
static double phi0 ;
static double phimin = 0.01 ;
static double r_afm,r_aft,r_c3ah6,r_csh2 ;
static double n_ca_ref,n_si_ref,n_al_ref ;
static double n_afm_0,n_aft_0,n_c3ah6_0,n_csh2_0 ;
static double ar_AFt,dr_AFt ;
static double RT ;
static Curve_t* satcurve ;
static double K_bulk ;
static double Biot ;
//static double K_s ;
//static double G_s ;
//static double N_Biot ;
//static double G_Biot ;
static double strain0 ;
static double strainf ;
static double ap_AFt,dp_AFt ;
static double alphacoef ;
static double betacoef ;
static double r0 ;


#include "PhysicalConstant.h"
#include "Temperature.h"

void ComputePhysicoChemicalProperties(void)
{
  RT = PhysicalConstant_PerfectGasConstant * TEMPERATURE ;
}


static CementSolutionDiffusion_t* csd = NULL ;
static HardenedCementChemistry_t<double>* hcc = NULL ;

enum VariableIndexes_e {
I_ZN_Ca_S = NEQ,
I_ZN_Si_S,
I_ZN_Al_S,

I_N_Q,
I_N_S,
I_N_Ca,
I_N_Si,
I_N_K,
I_N_Cl,
I_N_Al,

I_N_CH,
I_N_CSH2,
I_N_AH3,
I_N_AFm,
I_N_AFt,
I_N_C3AH6,
I_N_CSH,

I_PHI,
I_PHI_C,

I_V_CSH,
I_V_Cem,
I_V_Cem0,

I_C_OH,

I_Radius,

I_S_C,

I_P_C,

I_Beta_p,

I_Strain,

I_StrainYY,

I_Straind,

I_VarPHI_C,

I_N_solidCa,
I_N_liquidCa,

I_Last
} ;


//#define NbOfVariables    (NEQ+31)
#define NbOfVariables    (I_Last)
static double Variables[Element_MaxNbOfNodes][2*NbOfVariables] ;
#define Variables_n(x)   ((x) + NbOfVariables)
static double dVariables[NbOfVariables] ;



enum VariableFluxIndexes_e {
I_W_S,
I_W_Ca,
I_W_Si,
I_W_K,
I_W_Cl,
I_W_q,
I_W_Al,
I_W_Last
} ;
  
  
#define NbOfVariableFluxes    (I_W_Last)
static double VariableFluxes[Element_MaxNbOfNodes][NbOfVariableFluxes] ;
//static double dVariableFluxes[NbOfVariableFluxes] ;



int pm(const char *s)
{
  if(strcmp(s,"porosity") == 0)        return (0) ;
  else if(strcmp(s,"N_CH") == 0)       return (1) ;
  else if(strcmp(s,"N_Si") == 0)       return (2) ;
  else if(strcmp(s,"N_CSH") == 0)      return (2) ; /* synonym */
  else if(strcmp(s,"T_CH") == 0)       return (3) ;
  else if(strcmp(s,"T_CSH2") == 0)     return (4) ;
  else if(strcmp(s,"N_CSH2") == 0)     return (5) ;
  else if(strcmp(s,"N_AH3") == 0)      return (6) ;
  else if(strcmp(s,"N_AFm") == 0)      return (7) ;
  else if(strcmp(s,"N_AFt") == 0)      return (8) ;
  else if(strcmp(s,"N_C3AH6") == 0)    return (9) ;
  else if(strcmp(s,"T_AFm") == 0)      return (10) ;
  else if(strcmp(s,"T_AFt") == 0)      return (11) ;
  else if(strcmp(s,"R_AFm") == 0)      return (12) ;
  else if(strcmp(s,"R_AFt") == 0)      return (13) ;
  else if(strcmp(s,"R_C3AH6") == 0)    return (14) ;
  else if(strcmp(s,"R_CSH2") == 0)     return (15) ;
  else if(strcmp(s,"A_i") == 0)        return (16) ;
  else if(strcmp(s,"A_r") == 0)        return (16) ; /* synonym */
  else if(strcmp(s,"K_bulk") == 0)     return (17) ;
  else if(strcmp(s,"Strain0") == 0)    return (18) ;
  else if(strcmp(s,"Strainf") == 0)    return (19) ;
  else if(strcmp(s,"A_p") == 0)        return (20) ;
  else if(strcmp(s,"AlphaCoef") == 0)  return (21) ;
  else if(strcmp(s,"BetaCoef") == 0)   return (22) ;
  else if(strcmp(s,"r0") == 0)         return (23) ;
  else if(strcmp(s,"Biot") == 0)       return (24) ;
  else if(strcmp(s,"G_s") == 0)        return (25) ;
  else if(strcmp(s,"B_i") == 0)        return (26) ;
  else if(strcmp(s,"B_r") == 0)        return (26) ; /* synonym */
  else if(strcmp(s,"B_p") == 0)        return (27) ;
  else {
    return(-1) ;
  }
}


void GetProperties(Element_t* el)
{
  phi0      = GetProperty("porosity") ;
  n_ca_ref  = GetProperty("N_CH") ;
  n_si_ref  = GetProperty("N_CSH") ;
  n_al_ref  = GetProperty("N_AH3") ;
  n_csh2_0  = GetProperty("N_CSH2") ;
  n_afm_0   = GetProperty("N_AFm") ;
  n_aft_0   = GetProperty("N_AFt") ;
  n_c3ah6_0 = GetProperty("N_C3AH6") ;
  r_afm     = GetProperty("R_AFm") ;
  r_aft     = GetProperty("R_AFt") ;
  r_c3ah6   = GetProperty("R_C3AH6") ;
  r_csh2    = GetProperty("R_CSH2") ;
  K_bulk    = GetProperty("K_bulk") ;
  strain0   = GetProperty("Strain0") ;
  strainf   = GetProperty("Strainf") ;
  alphacoef = GetProperty("AlphaCoef") ;
  betacoef  = GetProperty("BetaCoef") ;
  r0        = GetProperty("r0") ;
  Biot      = GetProperty("Biot") ;
  //G_s       = GetProperty("G_s") ;
  //K_s       = K_bulk/(1-Biot) ;
  //N_Biot    = K_s/(Biot - phi0) ;
  //G_Biot    = G_s/(0.75*phi0) ;
  ar_AFt    = GetProperty("A_r") ;
  ap_AFt    = GetProperty("A_p") ;
  dr_AFt    = GetProperty("B_r") ;
  dp_AFt    = GetProperty("B_p") ;
  
  satcurve  = Element_FindCurve(el,"S_r") ;
}


int SetModelProp(Model_t* model)
{
  Model_GetNbOfEquations(model) = NEQ ;
  
  Model_CopyNameOfEquation(model,E_Sulfur, "sulfur") ;
  Model_CopyNameOfEquation(model,E_Calcium,"calcium") ;
  Model_CopyNameOfEquation(model,E_charge, "charge") ;
  Model_CopyNameOfEquation(model,E_Potassium, "potassium") ;
  Model_CopyNameOfEquation(model,E_Aluminium,"aluminium") ;
#ifdef E_eneutral
  Model_CopyNameOfEquation(model,E_eneutral,"electroneutrality") ;
#endif

#if   defined (U_LogC_H2SO4)
  Model_CopyNameOfUnknown(model,E_Sulfur,"logc_h2so4") ;
#elif defined (U_C_H2SO4)
  Model_CopyNameOfUnknown(model,E_Sulfur,"c_h2so4") ;
#elif defined (U_LogC_SO4)
  Model_CopyNameOfUnknown(model,E_Sulfur,"logc_so4") ;
#elif defined (U_C_SO4)
  Model_CopyNameOfUnknown(model,E_Sulfur,"c_so4") ;
#endif

  Model_CopyNameOfUnknown(model,E_Calcium,"z_ca") ;
  Model_CopyNameOfUnknown(model,E_charge,    "psi") ;
  
#if   defined (U_LogC_K)
  Model_CopyNameOfUnknown(model,E_Potassium,    "logc_k") ;
#elif defined (U_C_K)
  Model_CopyNameOfUnknown(model,E_Potassium,    "c_k") ;
#endif

  Model_CopyNameOfUnknown(model,E_Aluminium,"z_al") ;
  
#ifdef E_eneutral
  #if   defined (U_LogC_OH)
    Model_CopyNameOfUnknown(model,E_eneutral, "logc_oh") ;
  #elif defined (U_C_OH)
    Model_CopyNameOfUnknown(model,E_eneutral, "c_oh") ;
  #endif
#endif
  
  return(0) ;
}


int ReadMatProp(Material_t* mat,DataFile_t* datafile)
/* Lecture des donnees materiaux dans le fichier ficd */
{
  int  NbOfProp = 28 ;

  {
    /* Self-initialization */
    Material_GetProperty(mat)[pm("N_CH")]   = 1 ;
    Material_GetProperty(mat)[pm("N_Si")]   = 1 ;
    Material_GetProperty(mat)[pm("N_AH3")]  = 1 ;
    Material_GetProperty(mat)[pm("N_CSH2")] = 0 ;
    Material_GetProperty(mat)[pm("N_AFm")]  = 0 ;
    Material_GetProperty(mat)[pm("N_AFt")]  = 0 ;
    Material_GetProperty(mat)[pm("N_C3AH6")]  = 0 ;
    Material_GetProperty(mat)[pm("R_AFm")]  = 4.6e-4 ; /* 4.6e-4 (mol/L/s) Salgues 2013 */
    Material_GetProperty(mat)[pm("R_AFt")]  = 4.6e-4 ;
    Material_GetProperty(mat)[pm("R_C3AH6")] = 1.e-10 ;
    Material_GetProperty(mat)[pm("R_CSH2")]  = 1.e-10 ;
    Material_GetProperty(mat)[pm("AlphaCoef")] = 0 ;
    Material_GetProperty(mat)[pm("BetaCoef")]  = 0 ;
    Material_GetProperty(mat)[pm("r0")] = 16*nm ;
    
    Material_GetProperty(mat)[pm("B_r")] = -1 ;
    Material_GetProperty(mat)[pm("B_p")] = -1 ;

    Material_ScanProperties(mat,datafile,pm) ;
    
    if(Material_GetProperty(mat)[pm("B_r")] < 0) {
      double c = Material_GetProperty(mat)[pm("A_r")] ;
      
      Material_GetProperty(mat)[pm("B_r")] = c ;
    }
    
    if(Material_GetProperty(mat)[pm("B_p")] < 0) {
      double c = Material_GetProperty(mat)[pm("A_p")] ;
      
      Material_GetProperty(mat)[pm("B_p")] = c ;
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
#ifdef E_eneutral
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
  Element_SetNbOfImplicitTerms(el,NVI) ;
  Element_SetNbOfExplicitTerms(el,NVE) ;
  Element_SetNbOfConstantTerms(el,NV0) ;
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
  double* f = Element_GetImplicitTerm(el) ;
  double* v0 = Element_GetConstantTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  double** u = Element_ComputePointerToNodalUnknowns(el) ;
  
  /*
    Input data
  */
  GetProperties(el) ;
  
  
  /* Pre-initialization */
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      double zn_ca_s    = ZN_Ca_S(i) ;
      double zn_si_s    = 1 ;
      double zn_al_s    = ZN_Al_S(i) ;
  
      /* Solve cement chemistry */
      {
        #if defined (U_C_H2SO4) || defined (U_LogC_H2SO4)
          double logc_h2so4 = LogC_H2SO4(i) ;
        #elif defined (U_C_SO4) || defined (U_LogC_SO4)
          double logc_so4 = LogC_SO4(i) ;
        #endif
        double logc_na    = -99 ;
        double logc_k     = LogC_K(i) ;
        double logc_oh    = LogC_OH(i) ;
  
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
        N_CH(i)    = n_ch ;
        N_CSH2(i)  = n_csh2 ;
        N_AFm(i)   = n_afm ;
        N_AFt(i)   = n_aft ;
        N_C3AH6(i) = n_c3ah6 ;
        PHI(i)     = phi ;
        PHI_C(i)   = phi_c ;

        #ifdef E_eneutral
          #if defined (U_LogC_OH)
            LogC_OH(i) = log10(c_oh) ;
          #elif defined (U_C_OH)
            C_OH(i)    = c_oh ;
          #endif
        #else
          C_OH(i)    = c_oh ;
        #endif

        V_Cem0(i)  = v_cem ;
      }
    
      {
        PoreRadius(i)    = PoreEntryRadiusMax ;
      
        Beta_p(i)        = 1 ;
        Strain(i)        = 0 ;
        Straind(i)       = strain0 ;
        VarPHI_C(i)      = 0 ;
      }

    }
  }
  
  
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      /* Variables */
      double* x = ComputeVariables(el,u,u,f,0,0,i) ;
      double* mui = CementSolutionDiffusion_GetPotentialAtPoint(csd,i) ;
    
      HardenedCementChemistry_CopyChemicalPotential(hcc,mui) ;

    
      /* Back up */
      N_S(i)  = x[I_N_S] ;
      N_Ca(i) = x[I_N_Ca] ;
      N_Si(i) = x[I_N_Si] ;
      N_K(i)  = x[I_N_K] ;
      N_Cl(i) = x[I_N_Cl] ;
      N_Al(i) = x[I_N_Al] ;
      /* charge density */
      N_q(i)  = x[I_N_Q] ;

    
      N_CH(i)    = x[I_N_CH] ;
      N_CSH2(i)  = x[I_N_CSH2] ;
      N_AFm(i)   = x[I_N_AFm] ;
      N_AFt(i)   = x[I_N_AFt] ;
      N_C3AH6(i) = x[I_N_C3AH6] ;
      PHI(i)     = x[I_PHI] ;
      PHI_C(i)   = x[I_PHI_C] ;

      #ifndef E_eneutral
        C_OH(i)    = x[I_C_OH] ;
      #endif
    
      PoreRadius(i)    = x[I_Radius] ;
      
      Beta_p(i)        = x[I_Beta_p] ;
      Strain(i)        = x[I_Strain] ;
      Straind(i)       = x[I_Straind] ;
      VarPHI_C(i)      = x[I_VarPHI_C] ;
    }
  }
  
  if(Element_IsSubmanifold(el)) return(0) ;

  /* Coefficient de transfert */
  ComputeTransferCoefficients(el,u,f) ;

  /* Flux */
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      int j ;
      
      for(j = i + 1 ; j < nn ; j++) {
        double* w = ComputeVariableFluxes(el,i,j) ;
        

        W_S(i,j)     = w[I_W_S  ] ;
        W_Ca(i,j)    = w[I_W_Ca ] ;
        W_Si(i,j)    = w[I_W_Si ] ;
        W_q(i,j)     = w[I_W_q  ] ;
        W_K(i,j)     = w[I_W_K  ] ;
        W_Cl(i,j)    = w[I_W_Cl ] ;
        W_Al(i,j)    = w[I_W_Al ] ;

        W_S(j,i)     = - w[I_W_S  ] ;
        W_Ca(j,i)    = - w[I_W_Ca ] ;
        W_Si(j,i)    = - w[I_W_Si ] ;
        W_q(j,i)     = - w[I_W_q  ] ;
        W_K(j,i)     = - w[I_W_K  ] ;
        W_Cl(j,i)    = - w[I_W_Cl ] ;
        W_Al(j,i)    = - w[I_W_Al ] ;
      }
    }
  }
  
  return(0) ;
}


int  ComputeExplicitTerms(Element_t* el,double t)
/* Thermes explicites (va)  */
{
  double* va = Element_GetExplicitTerm(el) ;
  double* f = Element_GetPreviousImplicitTerm(el) ;
  double** u = Element_ComputePointerToPreviousNodalUnknowns(el) ;
  
  if(Element_IsSubmanifold(el)) return(0) ;
  
  /*
    Input data
  */
  GetProperties(el) ;
  
  /*
    Transfer coefficient
  */
  
  ComputeTransferCoefficients(el,u,f) ;
  
  {
    DataSet_t* dataset = Element_GetDataSet(el) ;
    Mesh_t* mesh = DataSet_GetMesh(dataset) ;
    int index = &(Strain(0)) - f ;
    int shift = 1 ;
    double strainyy = FVM_AveragePreviousImplicitTerm(mesh,"Sulfaco",index,shift) ;
    
    StrainYY = strainyy ;
  }
  

  return(0) ;
}


int  ComputeImplicitTerms(Element_t* el,double t,double dt)
/* Les variables donnees par la loi de comportement (f_1) */
{
  double* f = Element_GetCurrentImplicitTerm(el) ;
  double* f_n = Element_GetPreviousImplicitTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  double** u   = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el) ;
  
  /*
    Input data
  */
  GetProperties(el) ;
  
  
  /* Contenus molaires */
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      /* Variables */
      double* x = ComputeVariables(el,u,u_n,f_n,t,dt,i) ;
      double* mui = CementSolutionDiffusion_GetPotentialAtPoint(csd,i) ;
    
      HardenedCementChemistry_CopyChemicalPotential(hcc,mui) ;

      /* Back up */

      /* Molar contents */
      N_S(i)  = x[I_N_S] ;
      N_Ca(i) = x[I_N_Ca] ;
      N_Si(i) = x[I_N_Si] ;
      N_K(i)  = x[I_N_K] ;
      N_Cl(i) = x[I_N_Cl] ;
      N_Al(i) = x[I_N_Al] ;
    
      /* Charge density */
      N_q(i)  = x[I_N_Q] ;

    
      N_CH(i)    = x[I_N_CH] ;
      N_CSH2(i)  = x[I_N_CSH2] ;
      N_AFm(i)   = x[I_N_AFm] ;
      N_AFt(i)   = x[I_N_AFt] ;
      N_C3AH6(i) = x[I_N_C3AH6] ;
      PHI(i)     = x[I_PHI] ;
      PHI_C(i)   = x[I_PHI_C] ;
      
#ifndef E_eneutral
      C_OH(i)    = x[I_C_OH] ;
#endif
    
      PoreRadius(i)    = x[I_Radius] ;
      
      Beta_p(i)        = x[I_Beta_p] ;
      Strain(i)        = x[I_Strain] ;
      Straind(i)       = x[I_Straind] ;
      VarPHI_C(i)      = x[I_VarPHI_C] ;


      {
        int test = 0 ;
        /*
        if(x[I_C_H2SO4] < 0.) test = -1 ;
        if(x[I_N_Ca_S]  < 0.) test = -1 ;
        if(x[I_N_Si_S]  < 0.) test = -1 ;
        if(x[I_N_Al_S]  < 0.) test = -1 ;
        */
        /*
        if(x[I_PHI]     < 0.) test = -1 ;
        */
      
        if(test < 0) {
          double c_h2so4 = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,H2SO4) ;
          double c_oh = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,OH) ;
          double s_ch   = HardenedCementChemistry_GetSaturationIndexOf(hcc,CH) ;
          double s_csh2 = HardenedCementChemistry_GetSaturationIndexOf(hcc,CSH2) ;
          double s_ah3  = HardenedCementChemistry_GetSaturationIndexOf(hcc,AH3) ;
          double s_afm  = HardenedCementChemistry_GetSaturationIndexOf(hcc,AFm) ;
          double s_aft  = HardenedCementChemistry_GetSaturationIndexOf(hcc,AFt) ;
          double s_c3ah6  = HardenedCementChemistry_GetSaturationIndexOf(hcc,C3AH6) ;


          double xx = Element_GetNodeCoordinate(el,i)[0] ;
        
          printf("x         = %e\n",xx) ;
          printf("phi       = %e\n",x[I_PHI]) ;
          printf("phi_c     = %e\n",x[I_PHI_C]) ;
          printf("c_h2so4   = %e\n",c_h2so4) ;
          printf("n_ch      = %e\n",x[I_N_CH]) ;
          printf("n_csh2    = %e\n",x[I_N_CSH2]) ;
          printf("n_csh     = %e\n",x[I_N_CSH]) ;
          printf("n_ah3     = %e\n",x[I_N_AH3]) ;
          printf("n_afm     = %e\n",x[I_N_AFm]) ;
          printf("n_aft     = %e\n",x[I_N_AFt]) ;
          printf("n_c3ah6   = %e\n",x[I_N_C3AH6]) ;
          printf("s_ch      = %e\n",s_ch) ;
          printf("s_csh2    = %e\n",s_csh2) ;
          printf("s_ah3     = %e\n",s_ah3) ;
          printf("s_afm     = %e\n",s_afm) ;
          printf("s_aft     = %e\n",s_aft) ;
          printf("s_c3ah6   = %e\n",s_c3ah6) ;
          printf("zn_ca_s   = %e\n",x[I_ZN_Ca_S]) ;
          printf("zn_al_s   = %e\n",x[I_ZN_Al_S]) ;
          printf("c_oh      = %e\n",c_oh) ;
          return(-1) ;
        }
      }
    }
  }
  
  if(Element_IsSubmanifold(el)) return(0) ;

  /* Flux */
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      int j ;
      
      for(j = i + 1 ; j < nn ; j++) {
        double* w = ComputeVariableFluxes(el,i,j) ;
        

        W_S(i,j)     = w[I_W_S  ] ;
        W_Ca(i,j)    = w[I_W_Ca ] ;
        W_Si(i,j)    = w[I_W_Si ] ;
        W_q(i,j)     = w[I_W_q  ] ;
        W_K(i,j)     = w[I_W_K  ] ;
        W_Cl(i,j)    = w[I_W_Cl ] ;
        W_Al(i,j)    = w[I_W_Al ] ;

        W_S(j,i)     = - w[I_W_S  ] ;
        W_Ca(j,i)    = - w[I_W_Ca ] ;
        W_Si(j,i)    = - w[I_W_Si ] ;
        W_q(j,i)     = - w[I_W_q  ] ;
        W_K(j,i)     = - w[I_W_K  ] ;
        W_Cl(j,i)    = - w[I_W_Cl ] ;
        W_Al(j,i)    = - w[I_W_Al ] ;
      }
    }
  }

  return(0) ;
}



int  ComputeMatrix(Element_t* el,double t,double dt,double* k)
/* Matrice (k) */
{
#define K(i,j)    (k[(i)*ndof + (j)])
  int nn = Element_GetNbOfNodes(el) ;
  int ndof = nn*NEQ ;
  int    i ;
  
  
  /*
    Initialisation 
  */
  for(i = 0 ; i < ndof*ndof ; i++) k[i] = 0. ;

  if(Element_IsSubmanifold(el)) return(0) ;
  
  /*
    Input data
  */
  GetProperties(el) ;
  
  {
    FVM_t* fvm = FVM_GetInstance(el) ;
    double c[4*NEQ*NEQ] ;
    
    TangentCoefficients(el,t,dt,c) ;
    
    {
      double* km = FVM_ComputeMassAndIsotropicConductionMatrix(fvm,c,NEQ) ;
    
      for(i = 0 ; i < ndof*ndof ; i++) k[i] = km[i] ;
    }
  }
  


#if defined (U_C_H2SO4)
    {
      double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
      for(i = 0 ; i < 2*NEQ ; i++){
        K(i,E_Sulfur)     /= Ln10*C_H2SO4(0) ;
        K(i,E_Sulfur+NEQ) /= Ln10*C_H2SO4(1) ;
      }
    }
#elif defined (U_C_SO4)
    {
      double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
      for(i = 0 ; i < 2*NEQ ; i++){
        K(i,E_Sulfur)     /= Ln10*C_SO4(0) ;
        K(i,E_Sulfur+NEQ) /= Ln10*C_SO4(1) ;
      }
    }
#endif


#if defined (U_C_K)
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(i = 0 ; i < 2*NEQ ; i++){
      K(i,E_Potassium)     /= Ln10*C_K(0) ;
      K(i,E_Potassium+NEQ) /= Ln10*C_K(1) ;
    }
  }
#endif
  
  
#ifdef E_eneutral
  #if defined (U_C_OH)
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(i = 0 ; i < 2*NEQ ; i++){
      K(i,E_eneutral)     /= Ln10*C_OH(0) ;
      K(i,E_eneutral+NEQ) /= Ln10*C_OH(1) ;
    }
  }
  #endif
#endif


  return(0) ;

#undef K
}


int  ComputeResidu(Element_t* el,double t,double dt,double* r)
/* Residu (r) */
{
#define R(n,i)    (r[(n)*NEQ+(i)])
  double* f = Element_GetCurrentImplicitTerm(el) ;
  double* f_n = Element_GetPreviousImplicitTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  int ndof = nn*NEQ ;
  FVM_t* fvm = FVM_GetInstance(el) ;
  int    i ;
  double zero = 0. ;
  /*
    INITIALISATION DU RESIDU
  */
  for(i = 0 ; i < ndof ; i++) r[i] = zero ;

  if(Element_IsSubmanifold(el)) return(0) ;

  
  /*
    Conservation of element S: (N_S - N_Sn) + dt * div(W_S) = 0
  */
  {
    double g[Element_MaxNbOfNodes*Element_MaxNbOfNodes] ;
    
    for(i = 0 ; i < nn ; i++) {
      int j ;
      
      for(j = 0 ; j < nn ; j++) {
        if(i == j) {
          g[i*nn + i] = N_S(i) - N_Sn(i) ;
        } else {
          g[i*nn + j] = dt * W_S(i,j) ;
        }
      }
    }
    
    {
      double* r1 = FVM_ComputeMassAndFluxResidu(fvm,g,nn) ;
      
      for(i = 0 ; i < nn ; i++) {
        R(i,E_Sulfur) -= r1[i] ;
      }
    }
  }
  
  /*
    Conservation of element Ca: (N_Ca - N_Can) + dt * div(W_Ca) = 0
  */
  {
    double g[Element_MaxNbOfNodes*Element_MaxNbOfNodes] ;
    
    for(i = 0 ; i < nn ; i++) {
      int j ;
      
      for(j = 0 ; j < nn ; j++) {
        if(i == j) {
          g[i*nn + i] = N_Ca(i) - N_Can(i) ;
        } else {
          g[i*nn + j] = dt * W_Ca(i,j) ;
        }
      }
    }
    
    {
      double* r1 = FVM_ComputeMassAndFluxResidu(fvm,g,nn) ;
      
      for(i = 0 ; i < nn ; i++) {
        R(i,E_Calcium) -= r1[i] ;
      }
    }
  }
  
  /*
    Conservation of element K: (N_K - N_Kn) + dt * div(W_K) = 0
  */
  {
    double g[Element_MaxNbOfNodes*Element_MaxNbOfNodes] ;
    
    for(i = 0 ; i < nn ; i++) {
      int j ;
      
      for(j = 0 ; j < nn ; j++) {
        if(i == j) {
          g[i*nn + i] = N_K(i) - N_Kn(i) ;
        } else {
          g[i*nn + j] = dt * W_K(i,j) ;
        }
      }
    }
    
    {
      double* r1 = FVM_ComputeMassAndFluxResidu(fvm,g,nn) ;
      
      for(i = 0 ; i < nn ; i++) {
        R(i,E_Potassium) -= r1[i] ;
      }
    }
  }
  
  /*
    Conservation of element Al: (N_Al - N_Aln) + dt * div(W_Al) = 0
  */
  {
    double g[Element_MaxNbOfNodes*Element_MaxNbOfNodes] ;
    
    for(i = 0 ; i < nn ; i++) {
      int j ;
      
      for(j = 0 ; j < nn ; j++) {
        if(i == j) {
          g[i*nn + i] = N_Al(i) - N_Aln(i) ;
        } else {
          g[i*nn + j] = dt * W_Al(i,j) ;
        }
      }
    }
    
    {
      double* r1 = FVM_ComputeMassAndFluxResidu(fvm,g,nn) ;
      
      for(i = 0 ; i < nn ; i++) {
        R(i,E_Aluminium) -= r1[i] ;
      }
    }
  }
  
  /*
    Conservation of charge: div(W_q) = 0
  */
  {
    double g[Element_MaxNbOfNodes*Element_MaxNbOfNodes] ;
    
    for(i = 0 ; i < nn ; i++) {
      int j ;
      
      for(j = 0 ; j < nn ; j++) {
        if(i == j) {
          g[i*nn + i] = 0 ;
        } else {
          g[i*nn + j] = dt * W_q(i,j) ;
        }
      }
    }
    
    {
      double* r1 = FVM_ComputeMassAndFluxResidu(fvm,g,nn) ;
      
      for(i = 0 ; i < nn ; i++) {
        R(i,E_charge) -= r1[i] ;
      }
    }
  }
  
  
#ifdef E_eneutral
  /*
    Electroneutrality
  */
  {
    double g[Element_MaxNbOfNodes*Element_MaxNbOfNodes] ;
    
    for(i = 0 ; i < nn ; i++) {
      int j ;
      
      for(j = 0 ; j < nn ; j++) {
        if(i == j) {
          g[i*nn + i] = N_q(i) ;
        } else {
          g[i*nn + j] = 0 ;
        }
      }
    }
    
    {
      double* r1 = FVM_ComputeMassAndFluxResidu(fvm,g,nn) ;
      
      for(i = 0 ; i < nn ; i++) {
        R(i,E_eneutral) -= r1[i] ;
      }
    }
  }
#endif
  
  return(0) ;

#undef R
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
  int    i ;

  /* if(Element_IsSubmanifold(el)) return(0) ; */
  
  /*
    Input data
  */
  GetProperties(el) ;


  /* Initialization */
  for(i = 0 ; i < nso ; i++) {
    Result_SetValuesToZero(r + i) ;
  }


  /* output quantities */
  {
    int j = FVM_FindLocalCellIndex(fvm,s) ;
    /* molarites */
    double* x = ComputeVariables(el,u,u,f,t,0,j) ;
    /* Concentrations */
#define ptC(CPD)   &(HardenedCementChemistry_GetAqueousConcentrationOf(hcc,CPD))
#define ptE(CPD)   &(HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,CPD))
#define ptS(CPD)   &(HardenedCementChemistry_GetSaturationIndexOf(hcc,CPD))
#define ptPSI      &(HardenedCementChemistry_GetElectricPotential(hcc))
#define ptX_CSH    &(HardenedCementChemistry_GetCalciumSiliconRatioInCSH(hcc))
#define ptZ_CSH    &(HardenedCementChemistry_GetWaterSiliconRatioInCSH(hcc))

    i = 0 ;
    
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
    Result_Store(r + i++,ptC(CaHSO4 ),"c_cahso4",1) ;
    
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
    
    Result_Store(r + i++,(x + I_N_CH   ),"n_ch",1) ;
    Result_Store(r + i++,(x + I_N_CSH2 ),"n_csh2",1) ;
    Result_Store(r + i++,(x + I_N_CSH  ),"n_csh",1) ;
    Result_Store(r + i++,(x + I_N_AH3  ),"n_ah3",1) ;
    Result_Store(r + i++,(x + I_N_AFm  ),"n_afm",1) ;
    Result_Store(r + i++,(x + I_N_AFt  ),"n_aft",1) ;
    Result_Store(r + i++,(x + I_N_C3AH6),"n_c3ah6",1) ;
    {
      double n_csh = x[I_N_CSH] ;
      double n_so4ads = n_csh*AdsorbedSulfatePerUnitMoleOfCSH(*ptC(SO4),*ptC(OH)) ;
      
      Result_Store(r + i++,&n_so4ads,"n_so4^ads",1) ;
    }
    
    Result_Store(r + i++,(x + I_PHI),"porosite",1) ;
    Result_Store(r + i++,ptPSI,"potentiel_electrique",1) ;
    
    Result_Store(r + i++,(x + I_N_Q),"charge",1) ;
    
    Result_Store(r + i++,(x + I_V_CSH),"V_CSH",1) ;
    Result_Store(r + i++,ptX_CSH,"C/S",1) ;
    
    Result_Store(r + i++,&W_Si(0,1),"W_Si",1) ;
    Result_Store(r + i++,&W_Ca(0,1),"W_Ca",1) ;
    Result_Store(r + i++,&W_S(0,1) ,"W_S",1) ;
    Result_Store(r + i++,&W_Al(0,1),"W_Al",1) ;
    Result_Store(r + i++,&W_q(0,1) ,"W_q",1) ;
    
    Result_Store(r + i++,&zero,"P_CSH2",1) ;
    Result_Store(r + i++,&zero,"Damage",1) ;
    
    Result_Store(r + i++,&N_Ca(j),"N_Ca",1) ;
    Result_Store(r + i++,&N_Si(j),"N_Si",1) ;
    Result_Store(r + i++,&N_S(j) ,"N_S",1) ;
    Result_Store(r + i++,&N_Al(j),"N_Al",1) ;
    Result_Store(r + i++,&N_K(j) ,"N_K",1) ;
    Result_Store(r + i++,&N_Cl(j),"N_Cl",1) ;
    
    Result_Store(r + i++,(x + I_S_C    ),"Saturation degree of crystal",1) ;
    
    {
      double radius = x[I_Radius] ;
      double beta = InterfaceEquilibriumSaturationIndex(radius) ;
      Result_Store(r + i++,&beta,"Interface equilibrium saturation index of AFt",1) ;
    }
    
    {
      Result_Store(r + i++,x + I_Beta_p,"Pore wall equilibrium saturation index of AFt",1) ;
    }
    
    Result_Store(r + i++,(x + I_P_C    ),"Crystallization pressure",1) ;
    
    Result_Store(r + i++,(x + I_Strain ),"Strain",1) ;
    
    
    if(i != nso) {
      Message_RuntimeError("ComputeOutputs: wrong number of outputs") ;
    }
  }

  return(nso) ;
}



void ComputeTransferCoefficients(Element_t* el,double** u,double* f)
/* Transfer coefficients  */
{
  double* va = Element_GetExplicitTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  int i ;

  /* Initialization */
  for(i = 0 ; i < NVE ; i++) va[i] = 0. ;


  for(i = 0 ; i < nn ; i++) {
    double* x = ComputeVariables(el,u,u,f,0,0,i) ;
    
    /* Liquid tortuosity */
    {
      double phi    = x[I_PHI] ;
      double iff    = LiquidTortuosity(phi) ;
        
      TORTUOSITY[i] = iff ;
    }
    
    /* Concentrations */
    {
      double* c = HardenedCementChemistry_GetAqueousConcentration(hcc) ;
      int n = HardenedCementChemistry_NbOfConcentrations ;
      int j ;
    
      for(j = 0 ; j < n ; j++) {
        CONCENTRATION(i)[j] = c[j] ;
      }
    }
  }
}



int TangentCoefficients(Element_t* el,double t,double dt,double* c)
/**  Tangent matrix coefficients (c) */
{
  double* f_n = Element_GetPreviousImplicitTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  int ndof = nn*NEQ ;
  ObVal_t* obval = Element_GetObjectiveValue(el) ;
  double** u   = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el) ;
  int    dec = NEQ*NEQ ;
  double dui[NEQ] ;
  FVM_t* fvm   = FVM_GetInstance(el) ;
  double* dist = FVM_ComputeIntercellDistances(fvm) ;
  int    i ;
  
  /* Initialization */
  for(i = 0 ; i < ndof*ndof ; i++) c[i] = 0. ;

  if(Element_IsSubmanifold(el)) return(0) ;
  
  
  for(i = 0 ; i < NEQ ; i++) {
    dui[i] =  1.e-2*ObVal_GetValue(obval + i) ;
  }

  
  for(i = 0 ; i < nn ; i++) {
    /* Variables */
    double* xi   = ComputeVariables(el,u,u_n,f_n,t,dt,i) ;
    double* mui = CementSolutionDiffusion_GetPotentialAtPoint(csd,i) ;
    int k ;
    
    HardenedCementChemistry_CopyChemicalPotential(hcc,mui) ;


    #if defined (U_C_H2SO4) || defined (U_C_SO4)
      dui[E_Sulfur] =  1.e-2*ObVal_GetValue(obval + E_Sulfur)/(Ln10*Un_Sulfur(i)) ;
    #endif
    
    #if defined (U_C_K)
      dui[E_Potassium] =  1.e-2*ObVal_GetValue(obval + E_Potassium)/(Ln10*Un_Potassium(i)) ;
    #endif
    
    #ifdef E_eneutral
      #if defined (U_C_OH)
      dui[E_eneutral] =  1.e-2*ObVal_GetValue(obval + E_eneutral)/(Ln10*Un_eneutral(i)) ;
      #endif
    #endif
    
    for(k = 0 ; k < NEQ ; k++) {
      double  dui_k  = dui[k] ;
      double* dxi    = ComputeVariableDerivatives(el,t,dt,xi,dui_k,k) ;
    
      /* Content terms at node i */
      {
        double* cii = c + (i*nn + i)*NEQ*NEQ ;
        
        cii[E_Sulfur*NEQ    + k] = dxi[I_N_S ] ;
        cii[E_Calcium*NEQ   + k] = dxi[I_N_Ca] ;
        cii[E_Potassium*NEQ + k] = dxi[I_N_K ] ;
        cii[E_Aluminium*NEQ + k] = dxi[I_N_Al] ;
#ifdef E_eneutral
        //cii[E_charge*NEQ    + k] = dxi[I_N_Q] ;
        cii[E_eneutral*NEQ   + k] = dxi[I_N_Q] ;
#endif
      }

      /* Transfer terms from node i to node j: d(wij)/d(ui_k) */
      {
        int j ;
        
        for(j = 0 ; j < nn ; j++) {
          if(j != i) {
            
            {
              double* g = CementSolutionDiffusion_GetGradient(csd) ;
              double* muj = CementSolutionDiffusion_GetPotentialAtPoint(csd,j) ;
              int n = CementSolutionDiffusion_NbOfConcentrations ;
              int l ;
    
              /* On output ComputeVariableDerivatives has computed
               * mui + d(mui) (through hcc). Then it is copied into muj */
              HardenedCementChemistry_CopyChemicalPotential(hcc,muj) ;

              /* The derivatives d(mui)/d(ui_k) */
              for(l = 0 ; l < n ; l++) {
                g[l] = (muj[l] - mui[l]) / dui_k ;
              }
            }
            
            {
              double* cij = c + (i*nn + j)*NEQ*NEQ ;
              double dij  = dist[nn*i + j] ;
              double dtdij = dt/dij ;
              double* dw = ComputeFluxes(el,dxi,i,j) ;
        
              cij[E_Sulfur*NEQ    + k] = - dtdij * dw[I_W_S ] ;
              cij[E_Calcium*NEQ   + k] = - dtdij * dw[I_W_Ca] ;
              cij[E_Potassium*NEQ + k] = - dtdij * dw[I_W_K ] ;
              cij[E_Aluminium*NEQ + k] = - dtdij * dw[I_W_Al] ;
              cij[E_charge*NEQ    + k] = - dtdij * dw[I_W_q ] ;
            }
          }
        }
      }
    }
  }

  return(dec) ;
}



double* ComputeVariables(Element_t* el,double** u,double** u_n,double* f_n,double t,double dt,int n)
{
  double* va = Element_GetExplicitTerm(el) ;
  double* x   = Variables[n] ;
  double* x_n = Variables_n(x) ;
  
  /* Primary Variables */
#if defined (U_C_H2SO4) || defined (U_LogC_H2SO4)
  x[E_Sulfur    ] = LogC_H2SO4(n) ;
#elif defined (U_C_SO4) || defined (U_LogC_SO4)
  x[E_Sulfur    ] = LogC_SO4(n) ;
#endif
  x[E_Calcium   ] = ZN_Ca_S(n) ;
  x[E_Potassium ] = LogC_K(n) ;
  x[E_charge    ] = PSI(n) ;
  x[E_Aluminium ] = ZN_Al_S(n) ;
#if defined (E_eneutral)
  x[E_eneutral  ] = LogC_OH(n) ;
#endif
  
  /* Needed variables to compute secondary components */
  x_n[I_N_CH  ]  = N_CHn(n) ;
  x_n[I_N_CSH2]  = N_CSH2n(n) ;
  x_n[I_N_AFm ]  = N_AFmn(n) ;
  x_n[I_N_AFt ]  = N_AFtn(n) ;
  x_n[I_N_C3AH6] = N_C3AH6n(n) ;
  x_n[I_PHI   ]  = PHIn(n) ;
  x_n[I_PHI_C ]  = PHI_Cn(n) ;
  x_n[I_C_OH  ]  = C_OHn(n) ;
  x_n[I_Radius]  = PoreRadiusn(n) ;
  x_n[I_Beta_p]  = Beta_pn(n) ;
  x_n[I_Strain]  = Strain_n(n);
  x_n[I_Straind] = Straind_n(n);
  x_n[I_VarPHI_C] = VarPHI_Cn(n) ;
  
  x_n[I_StrainYY] = StrainYY ;

  {
    double* v0   = Element_GetConstantTerm(el) ;
    
    x[I_V_Cem0 ]  = V_Cem0(n) ;
  }
  
  ComputeSecondaryVariables(el,t,dt,x_n,x) ;
  return(x) ;
}



void  ComputeSecondaryVariables(Element_t* el,double t,double dt,double* x_n,double* x)
{
  /* Primary variables */
  double zn_si_s    = 1 ;
  double zn_ca_s    = x[E_Calcium] ;
  double zn_al_s    = x[E_Aluminium] ;
  double psi        = x[E_charge] ;
  
  /* Solve cement chemistry */
  {
    #if defined (U_C_H2SO4) || defined (U_LogC_H2SO4)
      double logc_h2so4 = x[E_Sulfur] ;
    #elif defined (U_C_SO4) || defined (U_LogC_SO4)
      double logc_so4 = x[E_Sulfur] ;
    #endif
    double logc_na    = -99 ;
    double logc_k     = x[E_Potassium] ;
#if defined (E_eneutral)
    double logc_oh    = x[E_eneutral] ;
#else
    double logc_oh    = log10(x_n[I_C_OH]) ;
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

#ifndef E_eneutral
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
  double r_n = x_n[I_Radius] ;
  double r   = Radius(r_n,beta,dt,el) ;
  double s_l = LiquidSaturationDegree(r) ;
  double s_c = 1 - s_l ;
  
#ifdef E_Mech
  /* Compute the saturation index at the pore wall, beta_p */
  double beta_pn   = x_n[I_Beta_p] ;
  double varphi_cn = x_n[I_VarPHI_C] ;
  double strain_n  = x_n[I_Strain] ;
  double straind_n = x_n[I_Straind] ;
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

  double n_aftn     = x_n[I_N_AFt] ;
  //double n_aft      = AFtSolidContent(n_aftn,s_aft,dt) ;
  double n_aft      = v_crystal/V_AFt ;  
  //double n_aft      = (s_aft > s_csh2) ? n_crystal : 0 ;
  
  double n_csh2n    = x_n[I_N_CSH2] ;
  double n_csh2     = CSH2SolidContent(n_csh2n,s_csh2,dt) ;
  //double n_csh2     = v_crystal/V_CSH2 ;
  //double n_csh2     = (s_aft > s_csh2) ? 0 : n_crystal ;
  
  //double n_chn      = x[I_N_CHn] ;
  double n_ch       = CHSolidContent(zn_ca_s) ;
  double n_ah3      = AH3SolidContent(zn_al_s) ;
  double n_afmn     = x_n[I_N_AFm] ;
  double n_afm      = AFmSolidContent(n_afmn,s_afm,dt) ;
  double n_c3ah6n   = x_n[I_N_C3AH6] ;
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
  double v_cem0     = x[I_V_Cem0] ;
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
  double phi_l        = x_n[I_PHI] ;
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


#ifndef E_eneutral
  #if (ELECTRONEUTRALITY == EXPLICIT)
  c_q_l = HardenedCementChemistry_SolveExplicitElectroneutrality(hcc) ;
  n_q_l = phi_l*c_q_l ;
  #endif
#endif

  

  /* Back up */


  /* Solid components */
  x[I_N_CH      ] = n_ch ;
  x[I_N_CSH2    ] = n_csh2 ;
  x[I_N_AH3     ] = n_ah3 ;
  x[I_N_AFm     ] = n_afm ;
  x[I_N_AFt     ] = n_aft ;
  x[I_N_C3AH6   ] = n_c3ah6 ;
  x[I_N_CSH     ] = n_csh ;
  
  x[I_ZN_Ca_S   ] = zn_ca_s ;  
  x[I_ZN_Al_S   ] = zn_al_s ;  
  
  x[I_V_CSH     ] = v_csh ;
  
  x[I_V_Cem     ] = v_cem ;
  
  x[I_N_solidCa ] = n_ca_s;
  x[I_N_liquidCa] = n_ca_l;
  
  
  /* Porosities */
  x[I_PHI       ] = phi_t ;
  x[I_PHI_C     ] = phi_c ;
  
  /* Saturation degree of crystal */
  x[I_S_C       ] = s_c ;
  
  /* Crystallization pressure */
  x[I_P_C       ] = p_c ;
  
  /* Radius */
  x[I_Radius    ] = r ;
  
  /* Strains */
  x[I_Beta_p    ] = beta_p ;
  x[I_Strain    ] = strain ;
  x[I_Straind   ] = straind ;
  x[I_VarPHI_C  ] = varphi_c ;
  
  
  
  /* Element contents */
  x[I_N_S       ] = n_s_l  + n_s_s ;
  x[I_N_Ca      ] = n_ca_l + n_ca_s ;
  x[I_N_Si      ] = n_si_l + n_si_s ;
  x[I_N_K       ] = n_k_l  ;
  x[I_N_Cl      ] = n_cl_l  ;
  x[I_N_Al      ] = n_al_l + n_al_s ;

  /* Charge density */
  x[I_N_Q       ] = n_q_l ;
}



double* ComputeVariableDerivatives(Element_t* el,double t,double dt,double* x,double dui,int i)
{
  double* dx  = dVariables ;
  double* x_n = Variables_n(x) ;
  int j ;
  
  /* Primary Variables */
  for(j = 0 ; j < NbOfVariables ; j++) {
    dx[j] = x[j] ;
  }
  
  /* We increment the variable as (x + dx) */
  dx[i] += dui ;
  
  ComputeSecondaryVariables(el,t,dt,x_n,dx) ;
  
  /* The numerical derivative as (f(x + dx) - f(x))/dx */
  for(j = 0 ; j < NbOfVariables ; j++) {
    dx[j] -= x[j] ;
    dx[j] /= dui ;
  }

  return(dx) ;
}




double* ComputeVariableFluxes(Element_t* el,int i,int j)
{
  double* grdij = dVariables ;

  /* Gradients */
  {
    int nn = Element_GetNbOfNodes(el) ;
    FVM_t* fvm   = FVM_GetInstance(el) ;
    double* dist = FVM_ComputeIntercellDistances(fvm) ;
    double dij  = dist[nn*i + j] ;

    {
      double* g = CementSolutionDiffusion_GetGradient(csd) ;
      double* mui = CementSolutionDiffusion_GetPotentialAtPoint(csd,i) ;
      double* muj = CementSolutionDiffusion_GetPotentialAtPoint(csd,j) ;
      int n = CementSolutionDiffusion_NbOfConcentrations ;
      int k ;
      
      for(k = 0 ; k < n ; k++) {
        g[k] = (muj[k] - mui[k]) / dij ;
      }
    }
  }

  /* Fluxes */
  {
    double* w = ComputeFluxes(el,grdij,i,j) ;
    
    return(w) ;
  }
}



double* ComputeFluxes(Element_t* el,double* grdij,int i,int j)
{
  double* va = Element_GetExplicitTerm(el) ;
  double* w  = VariableFluxes[i] ;
  
  /* Diffusion in the cement solution */
  {
    /* Gradients */
    {
      double* g = CementSolutionDiffusion_GetGradient(csd) ;
      int n = CementSolutionDiffusion_NbOfConcentrations ;
      double* ci = CONCENTRATION(i) ;
      double* cj = CONCENTRATION(j) ;
      double tortuosity = 0.5 * (TORTUOSITY[i] + TORTUOSITY[j]) ;
      int k ;
      
      for(k = 0 ; k < n ; k++) {
        double rho = 0.5 * (ci[k] + cj[k]) ;
      
        g[k] *= tortuosity * rho ;
      }
    }
    /* Fluxes */
    {
      
      CementSolutionDiffusion_ComputeFluxes(csd) ;
      
      w[I_W_Ca]  = CementSolutionDiffusion_GetElementFluxOf(csd,Ca) ;
      w[I_W_Si]  = CementSolutionDiffusion_GetElementFluxOf(csd,Si) ;
      w[I_W_S ]  = CementSolutionDiffusion_GetElementFluxOf(csd,S) ;
      w[I_W_K ]  = CementSolutionDiffusion_GetElementFluxOf(csd,K) ;
      w[I_W_Cl]  = CementSolutionDiffusion_GetElementFluxOf(csd,Cl) ;
      w[I_W_Al]  = CementSolutionDiffusion_GetElementFluxOf(csd,Al) ;
      w[I_W_q ]  = CementSolutionDiffusion_GetIonCurrent(csd) ;
    }
  }
    
  return(w) ;
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
