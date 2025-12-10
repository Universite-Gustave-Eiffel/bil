/* This is a "mother" model from which "daughter" models are derived.
 * The following models have been derived accordingly:
 * - Carbocem       (consider E_CARBON, suppress E_CHLORINE)
 * - Chloricem      (consider E_CHLORINE, suppress E_CARBON)
 * - Carbochloricem (consider E_CARBON and E_CHLORINE)
 * 
 * General features of the model:
 * 
 * - Take into account the composition of cements 
 *   i.e. in terms of the conservation equations of 
 *     Calcium
 *     Silicon
 *     Sodium
 *     Potassium
 *     Aluminium
 *     Sulfur
 *     Carbon
 *     Chlorine
 * 
 * - Take into account the conservation equations of
 *     Total mass
 *     Total charge
 *     Dry air
 * 
 * - Curves for CSH:
 *     C/S ratio
 *     H/S ratio
 *     Molar Volume
 * 
 * - Dissolution kinetics for CH based on spherical crystal 
 *   coated by a calcite layer.
 * 
 * - Dissolution and continuous decalcification of CSH
 * 
 * - Precipitation/Dissolution of CC (possibly with kinetics)
 * 
 * - Chloride ingress
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

#ifdef HAVE_AUTODIFF
#define USE_AUTODIFF ((bool) Element_GetProperty(el)[18])
#endif

#define TITLE   "Mother model of durability of CBM (2024)"
#define AUTHORS "Dangla and many others"

#include "PredefinedModelMethods.h"




/* Indices of equations/unknowns */
enum {
  E_CALCIUM   ,
  /* Uncomment/Comment the next two lines to consider/suppress silicon */
  E_SILICON   ,
  #define E_SILICON E_SILICON
  E_SODIUM    ,
  E_POTASSIUM ,
  E_CHARGE    ,
  E_MASS      ,
  /* Uncomment/Comment the next two lines to consider/suppress carbon */
  E_CARBON    ,
  #define E_CARBON E_CARBON
  /* Uncomment/comment the next two lines to consider/suppress electroneutrality */
  E_ENEUTRAL  ,
  #define E_ENEUTRAL E_ENEUTRAL 
  /* Uncomment/Comment the next two lines to consider/suppress chlorine */
  //E_CHLORINE  ,
  //#define E_CHLORINE E_CHLORINE
  /* Uncomment/Comment the next two lines to consider/suppress air */
  //E_AIR    ,
  //#define E_AIR E_AIR
  E_LAST
} ;


/* Nb of equations */
#define NEQ      E_LAST



/* Value of the nodal unknown (el must be used below) */
#define UNKNOWN(u,n,i)     Element_GetValueOfNodalUnknown(el,u,n,i)


/* Generic names of nodal unknowns */
#define U_CARBON(u,n)     (UNKNOWN(u,n,E_CARBON))
#define U_CHARGE(u,n)     (UNKNOWN(u,n,E_CHARGE))
#define U_MASS(u,n)       (UNKNOWN(u,n,E_MASS))
#define U_CALCIUM(u,n)    (UNKNOWN(u,n,E_CALCIUM))
#define U_SILICON(u,n)    (UNKNOWN(u,n,E_SILICON))
#define U_SODIUM(u,n)     (UNKNOWN(u,n,E_SODIUM))
#define U_POTASSIUM(u,n)  (UNKNOWN(u,n,E_POTASSIUM))
#define U_ENEUTRAL(u,n)   (UNKNOWN(u,n,E_ENEUTRAL))
#define U_CHLORINE(u,n)   (UNKNOWN(u,n,E_CHLORINE))
#define U_AIR(u,n)        (UNKNOWN(u,n,E_AIR))




/* Method chosen at compile time. 
 * Each equation is associated to a specific unknown.
 * Each unknown can deal with a specific model.
 * Uncomment/comment to let only one unknown per equation */

/* Carbon: unknown either C or logC */
#ifdef E_CARBON
  //#define U_C_CO2
  #define U_LogC_CO2
  #if defined (U_LogC_CO2) && !defined (U_C_CO2)
    #define LogC_CO2(u,n)   U_CARBON(u,n)
    #define C_CO2(u,n)      (pow(10,LogC_CO2(u,n)))
  #elif defined (U_C_CO2) && !defined (U_LogC_CO2)
    #define C_CO2(u,n)      U_CARBON(u,n)
    #define LogC_CO2(u,n)   (log10(C_CO2(u,n)))
  #else
    #error "Ambiguous or undefined unknown"
  #endif
#endif

/* Mass: the liquid pressure */
#define U_P_L
#define P_L(u,n)        U_MASS(u,n)

/* Calcium:
 * - U_ZN_Ca_S: dissolution kinetics of CH; Cc at equilibrium 
 * - U_LogS_CH: dissolution kinetics of CH; precipitation kinetics of Cc */
  /* Definition of U_CALCIUM = ZN_Ca_S: 
   * ZN_Ca_S = N/N0 + log(S) 
   * with N = calcium content in CH and CC  (>= 0)
   * and  S = saturation index of CcH ie max(log(S_CH),log(S_Cc)) (<= 0) */
#define U_ZN_Ca_S
//#define U_LogS_CH

/* Silicon: */
/* Definition of U_SILICON = ZN_Si_S:
 * ZN_Si_S = N/N0 + log(S_CSH)
 * with N = silicon content in CSH
 * and S_CSH = saturation index of CSH */
#ifdef E_SILICON
#define U_ZN_Si_S
#endif

/* charge: */
#define U_PSI
#define PSI(u,n)        U_CHARGE(u,n)

/* Sodium: unknown either C or logC */
//#define U_C_Na
#define U_LogC_Na
#if defined (U_LogC_Na) && !defined (U_C_Na)
  #define LogC_Na(u,n)    U_SODIUM(u,n)
  #define C_Na(u,n)       (pow(10,LogC_Na(u,n)))
#elif defined (U_C_Na) && !defined (U_LogC_Na)
  #define C_Na(u,n)       U_SODIUM(u,n)
  #define LogC_Na(u,n)    (log10(C_Na(u,n)))
#else
  #error "Ambiguous or undefined unknown"
#endif

/* Potassium: unknown either C or logC */
//#define U_C_K
#define U_LogC_K
#if defined (U_LogC_K) && !defined (U_C_K)
  #define LogC_K(u,n)     U_POTASSIUM(u,n)
  #define C_K(u,n)        (pow(10,LogC_K(u,n)))
#elif defined (U_C_K) && !defined (U_LogC_K)
  #define C_K(u,n)        U_POTASSIUM(u,n)
  #define LogC_K(u,n)     (log10(C_K(u,n)))
#else
  #error "Ambiguous or undefined unknown"
#endif

/* Electroneutrality: unknown either C_OH, logC_OH or Z_OH = C_H - C_OH */
#ifdef E_ENEUTRAL
  //#define U_C_OH
  #define U_LogC_OH
  //#define U_Z_OH
  #if defined (U_LogC_OH) && !defined (U_C_OH) && !defined (U_Z_OH)
    #define LogC_OH(u,n)   U_ENEUTRAL(u,n)
    #define C_OH(u,n)      (pow(10,LogC_OH(u,n)))
  #elif defined (U_C_OH) && !defined (U_LogC_OH) && !defined (U_Z_OH)
    #define C_OH(u,n)      U_ENEUTRAL(u,n)
    #define LogC_OH(u,n)   (log10(C_OH(u,n)))
  #elif defined (U_Z_OH) && !defined (U_LogC_OH) && !defined (U_C_OH)
    #define Z_OH(u,n)      U_ENEUTRAL(u,n)
    #define Z_OHDefinition(c_oh,c_h)   ((c_h) - (c_oh))
    #define C_OHDefinition(z_oh)       (0.5 * (sqrt((z_oh)*(z_oh) + 4*K_w) - (z_oh)))
    #define C_OH(u,n)      C_OHDefinition(Z_OH(u,n))
    #define LogC_OH(u,n)   (log10(C_OH(u,n)))
    #define dLogC_OHdZ_OH(z_oh)    (-1/(Ln10 * sqrt((z_oh)*(z_oh) + 4*K_w)))
  #else
    #error "Ambiguous or undefined unknown"
  #endif
#endif

/* Chlorine: unknown either C or logC */
#ifdef E_CHLORINE
  #define U_LogC_Cl
  //#define U_C_Cl
  //#define U_Z_Cl
  #if defined (U_LogC_Cl) && !defined (U_C_Cl) && !defined (U_Z_Cl)
    #define LogC_Cl(u,n)     U_CHLORINE(u,n)
    #define C_Cl(u,n)        (pow(10,LogC_Cl(u,n)))
  #elif defined (U_C_Cl) && !defined (U_LogC_Cl) && !defined (U_Z_Cl)
    #define C_Cl(u,n)        U_CHLORINE(u,n)
    #define LogC_Cl(u,n)     (log10(C_Cl(u,n)))
  #elif defined (U_Z_Cl) && !defined (U_C_Cl) && !defined (U_LogC_Cl)
    #define Z_Cl(u,n)        U_CHLORINE(u,n)
  #else
    #error "Ambiguous or undefined unknown"
  #endif
#endif

/* Air: the gas pressure */
#ifdef E_AIR
  #define U_P_G
  #define P_G(u,n)        U_AIR(u,n)
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
  T U_calcium;
  #ifdef E_SILICON
  T U_silicon;
  #endif
  T U_sodium;
  T U_potassium;
  T U_charge;
  T U_mass;
  #ifdef E_CARBON
  T U_carbon;
  #endif
  #ifdef E_ENEUTRAL
  T U_eneutral;
  #endif
  #ifdef E_CHLORINE
  T U_chlorine;
  #endif
  #ifdef E_AIR
  T U_air;
  #endif
  T Mole_carbon;
  T MolarFlow_carbon[Element_MaxNbOfNodes];
  T Mole_charge;
  T MolarFlow_charge[Element_MaxNbOfNodes];
  T Mass_total;
  T MassFlow_total[Element_MaxNbOfNodes];
  T Mole_calcium;
  T MolarFlow_calcium[Element_MaxNbOfNodes];
  T Mole_sodium;
  T MolarFlow_sodium[Element_MaxNbOfNodes];
  T Mole_potassium;
  T MolarFlow_potassium[Element_MaxNbOfNodes];
  T Mole_silicon;
  T MolarFlow_silicon[Element_MaxNbOfNodes];
  T Mole_chlorine;
  T MolarFlow_chlorine[Element_MaxNbOfNodes];
  T Mass_air;
  T MassFlow_air[Element_MaxNbOfNodes];
  T Mole_solidportlandite;
  T Mole_solidcalcite;
  T Mole_solidfriedelsalt;
  T Concentration_oh;
  T Concentration_carbondioxide;
  T ChemicalPotential[CementSolutionDiffusion_NbOfConcentrations];
  T Pressure_liquid;
  T Pressure_gas;
  T MassDensity_watervapor;
} ;



template<typename T = double>
struct OtherValues_t {
  T Mole_solidcsh;
  T Mole_solidcalcium;
  T Mole_solidsilicon;
  T Volume_solidtotal;
  T Volume_solidcsh;
  T SaturationDegree_liquid;
  T Porosity;
};





template<typename T = double>
struct ExplicitValues_t {
  T DiffusionCoefficient_carbondioxide;
  T DiffusionCoefficient_watervapor;
  T Permeability_liquid;
  T Permeability_gas;
  T Tortuosity_liquid;
  T MoleFractionLiquid_carbon;
  T MoleFractionLiquid_calcium;
  T MoleFractionLiquid_sodium;
  T MoleFractionLiquid_potassium;
  T MoleFractionLiquid_silicon;
  T MoleFractionLiquid_chlorine;
  T MoleFractionGas_carbondioxide;
  T MassFractionGas_watervapor;
  T AqueousConcentration[CementSolutionDiffusion_NbOfConcentrations];
};



template<typename T = double>
struct ConstantValues_t {
  T InitialVolume_solidtotal;
};


struct MPM_t: public MaterialPointMethod_t<Values_t> {
  MaterialPointMethod_SetInputs_t<Values_t> SetInputs;
  template<typename T>
  MaterialPointMethod_Integrate_t<Values_t,T> Integrate;
  
  #if 1
  Values_t<double>* Integrate(Element_t* el,double const& t,double const& dt,Values_t<double> const& val_n,Values_t<double>& val) {return(Integrate<double>(el,t,dt,val_n,val));}
  #ifdef HAVE_AUTODIFF
  Values_t<real>* Integrate(Element_t* el,double const& t,double const& dt,Values_t<double> const& val_n,Values_t<real>& val) {return(Integrate<real>(el,t,dt,val_n,val));}
  #endif
  #endif
  
  MaterialPointMethod_Initialize_t<Values_t> Initialize;
  MaterialPointMethod_SetTangentMatrix_t<Values_t> SetTangentMatrix;
  MaterialPointMethod_SetFluxes_t<Values_t> SetFluxes;
  MaterialPointMethod_SetIndexOfPrimaryVariables_t SetIndexOfPrimaryVariables;
  MaterialPointMethod_SetIncrementOfPrimaryVariables_t SetIncrementOfPrimaryVariables;
} ;

  /* This instantiation doesn't provide a correct virtual call (I don't know why!) */
  #if 0
  template Values_t<double>* MPM_t::Integrate<double>(Element_t*,const double&,const double&,Values_d const&,Values_t<double>&);
  #ifdef HAVE_AUTODIFF
  template Values_t<real>* MPM_t::Integrate<real>(Element_t*,double const&,double const&,Values_t<double> const&,Values_t<real>&);
  #endif
  #endif


/* The parameters below are read in the input data file */
struct Parameters_t {
  double InitialPorosity;
  double MinimumPorosity;
  double IntrinsicPermeability_liquid;
  double IntrinsicPermeability_gas;
  double FractionalLengthOfPoreBodies;
  double PorosityFractionAtVanishingPermeability;
  double DissolutionRate_portlandite;
  double DissolutionKineticCoef_portlandite ;
  double PrecipitationRate_calcite;
  double InitialContent_portlandite;
  double InitialContent_csh;
  double InitialConcentration_sodium;
  double InitialConcentration_potassium;
  double PrecipitationRate_friedelsalt;
  double CapillaryPressureLimitOfAsymptoticSaturation;
  double CrystalRadius_portlandite;
  double DiffusionCoefficientInCalcite_co2;
  double CharacteristicTimeOfDiffusionInCalcite_co2;
  double UseAutodiff;
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




#include "MolarMassOfMolecule.h"
#include "MolarVolumeOfCementHydrate.h"


/* Water property
 * -------------- */
 /* Molar mass */
#define M_H2O          MolarMassOfMolecule(H2O)
/* Molar volume of liquid water */
#define V_H2O          (18 * cm3)
/* Mass density */
#define MassDensityOfWaterVapor(p_v)   (M_H2O*(p_v)/RT)
/* Vapor-Liquid Equilibrium */
#define RelativeHumidity(p_l)          (exp(V_H2O/RT*((p_l) - p_l0)))
#define VaporPressure(p_l)             (p_v0*RelativeHumidity(p_l))
//#define LiquidPressure(hr)             (p_l0 + RT/V_H2O*(log(hr)))



/* Liquid properties
 * ----------------- */
//#undef  HardenedCementChemistry_GetLiquidMassDensity
//#define HardenedCementChemistry_GetLiquidMassDensity(hcc)    (rho_l0)



/* Dry air properties
 * ------------------ */
/* Molar mass */
#define M_AIR          (28.8 * gr)
/* Mass density */
#define MassDensityOfDryAir(p_a)       (M_AIR*(p_a)/RT)


/* CO2 gas properties
 * ------------------ */
#define M_CO2          MolarMassOfMolecule(CO2)
/* Partial pressure of CO2 */
#define PartialPressureOfCO2(rho_co2)   ((rho_co2)*RT)
/* Henry's law constant for the solubility of CO2 gas */
#define k_h           (0.9983046)                /* CO2(g) = CO2(aq) (T = 293K)*/




/* Material Properties
 * ------------------- */
#if defined (E_AIR)
  #define SaturationDegree(p)              (saturationdegree(p,p_c3,saturationcurve))
#else
  #define SaturationDegree(p)              (Curve_ComputeValue(saturationcurve,p))
#endif
#define RelativePermeabilityToLiquid(s)  (Curve_ComputeValue(relativepermliqcurve,s))
#ifdef E_AIR
  #define RelativePermeabilityToGas(s)     (Curve_ComputeValue(relativepermgascurve,s))
#else
  #define RelativePermeabilityToGas(s)     (1)
#endif
//#define TortuosityToLiquid               TortuosityToLiquid_Xie
#define TortuosityToLiquid               TortuosityToLiquid_OhJang
#define PermeabilityCoefficient          PermeabilityCoefficient_VermaPruess




/* Calcium Silicate Hydrate Properties (C-S-H)
 * ------------------------------------------- */
#define M_CaO          MolarMassOfMolecule(CaO)
#define M_SiO2         MolarMassOfMolecule(SiO2)
#define MolarMassOfCSH(x,z)     (M_CaO*(x) + M_SiO2 + M_H2O*(z))
#define MolarVolumeOfCSH(x_ch)           (Curve_ComputeValue(molarvolumeofcshcurve,x_ch))
//#define V_CSH         (78 * cm3)
//#define V_SH          (43 * cm3)
//#define MolarVolumeOfCSH(x)    ((x)/1.7*V_CSH + (1 - (x)/1.7)*V_SH)
/* Below is how to manage dissolution/precipitation */
/* Definition of U_SILICON = ZN_Si_S in the next lines:
 * ZN_Si_S = N/N0 + log(S_CSH)
 * with N = silicon content in CSH
 * and S_CSH = saturation index of CSH */
/* Log of saturation index */
#define Log10SaturationIndexOfCSH(zn_si_s)  MIN(zn_si_s,0.)
#define SiliconContentInCSH(zn_si_s)        (n_si_ref*MAX(zn_si_s,0.))
#define CSHSolidContent(zn_si_s)            SiliconContentInCSH(zn_si_s)



/* Calcium Hydroxide (Portlandite) Properties (CH)
 * ----------------------------------------------- */
#define M_CaOH2        MolarMassOfMolecule(CaO2H2)
/* Molar volume of CH solid */
#define V_CH           MolarVolumeOfCementHydrate(CH)
/* Below is how to manage dissolution/precipitation kinetics */
#define CHSolidContent_kin(n_chn,s_ch,dt) \
        MAX(n_chn + dt*a_2*dn1_caoh2sdt(1 - n_chn/n_ch0,c_2)*log(s_ch), 0.)
#if defined (U_ZN_Ca_S)
  /* Definition of U_CALCIUM = ZN_Ca_S in the next lines: 
   * ZN_Ca_S = N/N0 + log(S) 
   * with N = calcium content in CH and CC  (>= 0)
   * and  S = saturation index of CcH ie max(log(S_CH),log(S_Cc)) (<= 0) */
  /* Log of saturation index of CcH = CaO-CO2-H2O  */
  #define Log10SaturationIndexOfCcH(zn_ca_s)   MIN(zn_ca_s,0.)
  /* Calcium solid content in CcH ie in CH and Cc */
  #define CalciumContentInCcH(zn_ca_s)        (n_ca_ref*MAX(zn_ca_s,0.))
  /* Initial solid content of CH */
  #define InitialCHSolidContent(zn_ca_s,s_ch,s_cc) \
          (((s_cc) > (s_ch)) ? 0 : CalciumContentInCcH(zn_ca_s))
  /* Current solid content of CH */
  #define CHSolidContent(zn_ca_s,n_chn,n_ccn,s_ch,s_cc,dt) \
          (((s_cc) > (s_ch)) ? CHSolidContent_kin(n_chn,s_ch,dt) : \
          CalciumContentInCcH(zn_ca_s))
#elif defined (U_LogS_CH)
  /* Definition of U_CALCIUM = LogS_CH in the next lines:
   * LogS_CH = log(S_CH)
   * with S_CH = saturation index of CH */
  /* Initial solid content of CH */
  #define InitialCHSolidContent(logs_ch,s_ch,s_cc)     (n_ca_ref)
  /* Log of saturation index of CH */
  #define Log10SaturationIndexOfCH(logs_ch)            (logs_ch)
  /* Current solid content of CH */
  #define CHSolidContent(logs_ch,n_chn,n_ccn,s_ch,s_cc,dt) \
          CHSolidContent_kin(n_chn,s_ch,dt)
#endif



/* Calcium Carbonate (Calcite) Properties (CC)
 * ------------------------------------------- */
#define M_CaCO3        MolarMassOfMolecule(CaCO3)
/* Molar volume of CC */
#define V_CC           (37 * cm3)
/* Below is how to manage dissolution/precipitation kinetics */
#if defined (U_ZN_Ca_S)
  /* See above the definition of ZN_Ca_S */
  /* Initial solid content of Cc */
  #define InitialCCSolidContent(zn_ca_s,s_ch,s_cc) \
          (((s_cc) > (s_ch)) ? CalciumContentInCcH(zn_ca_s) : 0)
  /* Current solid content of Cc */
  #define CCSolidContent(zn_ca_s,n_chn,n_ccn,s_ch,s_cc,dt) \
          (CalciumContentInCcH(zn_ca_s) - CHSolidContent(zn_ca_s,n_chn,n_ccn,s_ch,s_cc,dt))
#elif defined (U_LogS_CH)
  /* See above the definition of LogS_CH */
  /* Initial solid content of Cc */
  #define InitialCCSolidContent(logs_ch,s_ch,s_cc)   (0)
  /* Current solid content */
  #define CCSolidContent_kin(n,s,dt)        MAX((n + dt*rate_calcite*(s - 1)),0.)
  #define CCSolidContent(logs_ch,n_chn,n_ccn,s_ch,s_cc,dt) \
          CCSolidContent_kin(n_ccn,s_cc,dt)
#endif






/* Chloride properties
 * ------------------- */
#define M_Cl        MolarMassOfMolecule(Cl)
#define M_OH        MolarMassOfMolecule(OH)
/* Chloride adsorption curve */
#define AlphaCoef(x) \
        (Curve_ComputeValue(adsorbedchloridecurve_a,x))
#define BetaCoef(x) \
        (Curve_ComputeValue(adsorbedchloridecurve_b,x))
#ifdef E_CHLORINE
#define AdsorbedChloridePerUnitMoleOfCSH(c_cl,x) \
        (AlphaCoef(x) * (c_cl) / (1. + BetaCoef(x) * (c_cl)))
#else
#define AdsorbedChloridePerUnitMoleOfCSH(c_cl,x) \
        (0)
#endif





/* Friedel's salt Properties
 * ------------------ ------ */
/* Molar mass of Friedel's salt */
#define M_CaCl2          (MolarMassOfMolecule(Ca) + 2*MolarMassOfMolecule(Cl))
#define M_Al2O3           MolarMassOfMolecule(Al2O3)
#define M_FriedelSalt    (3*M_CaO + M_Al2O3 + 10*M_H2O + M_CaCl2)
/* Molar volume of Friedel's salt */
#define V_FriedelSalt    MolarVolumeOfCementHydrate(FriedelSalt)
#define V_CaCl2          (51.62 * cm3)
/* Below is how to manage dissolution/precipitation kinetics */
#if defined (E_CHLORINE)
  #define FriedelSaltContent_kin(n,s,dt) \
          MAX((n + dt*rate_friedelsalt*(s - 1)),0.)
          
  #define FriedelSaltContent(n_fsn,s_fs,dt) \
          FriedelSaltContent_kin(n_fsn,s_fs,dt)
#else
  #define FriedelSaltContent(...)  (0)
#endif


/* Hydrogarnet properties
 * ---------------------- */
/* Molar mass of C3AH6 */
#define M_C3A      (3 * M_CaO + M_Al2O3 + 6 * M_H2O)
/* Molar volume of C3AH6 */
#define V_C3A      MolarVolumeOfCementHydrate(C3AH6)


/* Gibbsite property
 * ----------------- */
#define M_AH3      (M_Al2O3 + 3 * M_H2O)



/* Sodium adsorption curve 
 * ----------------------- */
#define RNa(x) \
        (Curve_ComputeValue(Element_GetCurve(el) + 4,x))
#define AdsorbedSodiumPerUnitMoleOfCSH(c_na,x) \
        ((c_na < 0.3 * (mol/dm3)) ? (RNa(x) * (c_na)) : (RNa(x) * 0.3 * (mol/dm3)))



/* Potassium adsorption curve 
 * -------------------------- */
#define RK(x) \
        (Curve_ComputeValue(Element_GetCurve(el) + 5,x))
#define AdsorbedPotassiumPerUnitMoleOfCSH(c_k,x) \
        ((c_k < 0.3 * (mol/dm3)) ? (RK(x) * (c_k)) : (RK(x) * 0.3 * (mol/dm3))) 



/* Element contents in solid phases  */
#define n_ca_ref                           (n_ch0)
#define n_si_ref                           (n_csh0)




static int     pm(const char* s) ;
static void    GetProperties(Element_t*,double) ;

template<typename T>
static T  dn1_caoh2sdt(T const,T const) ;
//static double  CHSolidContent_kin1(double const,double const,double const) ;

static void    ComputePhysicoChemicalProperties(double) ;

static int     concentrations_oh_na_k(double,double,double,double,double,double) ;

template<typename T>
static T  PermeabilityCoefficient_KozenyCarman(Element_t const*,T const) ;
template<typename T>
static T  PermeabilityCoefficient_VermaPruess(Element_t const*,T const) ;
template<typename T>
static T  TortuosityToLiquid_OhJang(T const,T const) ;
template<typename T>
static T  TortuosityToLiquid_BazantNajjar(T const,T const) ;
template<typename T>
static T  TortuosityToLiquid_Xie(T const,T const) ;
template<typename T>
static T  TortuosityToGas(T const,T const) ;

template<typename T>
static T  saturationdegree(T const,T const,Curve_t const*) ;


/* Internal parameters */
static Curve_t* saturationcurve ;
static Curve_t* relativepermliqcurve ;
static Curve_t* relativepermgascurve ;
static Curve_t* molarvolumeofcshcurve ;
#ifdef E_CHLORINE
static Curve_t* adsorbedchloridecurve_a ;
static Curve_t* adsorbedchloridecurve_b ;
#endif

static double p_g0 ;
static double p_l0 ;
static double p_v0 ;
static double d_co2 ;
static double d_vap ;
static double mu_l ;
static double mu_g ;
static double RT ;
static double K_w ;
static double rho_l0 ;

static CementSolutionDiffusion_t* csd = NULL ;
static HardenedCementChemistry_t<double>* hcc_d = NULL ;
#ifdef HAVE_AUTODIFF
static HardenedCementChemistry_t<real>* hcc_r = NULL ;
#endif

template<typename T>
static HardenedCementChemistry_t<T>* hcc_func(void) {
  if constexpr(std::is_same_v<T,double>) {
    return(hcc_d);
    #ifdef HAVE_AUTODIFF
  } else if constexpr(std::is_same_v<T,real>) {
    return(hcc_r);
    #endif
  }
  
  return(NULL);
}




#include "PhysicalConstant.h"
#include "AtmosphericPressure.h"
#include "WaterViscosity.h"
#include "AirViscosity.h"
#include "WaterVaporPressure.h"
#include "DiffusionCoefficientOfMoleculeInAir.h"
#include "EquilibriumConstantOfHomogeneousReactionInWater.h"


void ComputePhysicoChemicalProperties(double TK)
{
  /* Diffusion Coefficient Of Molecules In Air (dm2/s) */
  d_co2   = DiffusionCoefficientOfMoleculeInAir(CO2,TK) ;
  d_vap   = DiffusionCoefficientOfMoleculeInAir(H2O,TK) ;
  
  /* Viscosities */
  mu_l    = WaterViscosity(TK) ;
  mu_g    = AirViscosity(TK) ;
  
  /* Water vapor pressure */
  p_v0    = WaterVaporPressure(TK) ;
  
  /* Reference pressures */
  p_l0    = 0 ; //AtmosphericPressure ;
  p_g0    = 0 ; //AtmosphericPressure ;
  
  /* Physical constants */
  RT      = PhysicalConstant(PerfectGasConstant)*TK ;
  
  /* Chemical constants */
  K_w = EquilibriumConstantOfHomogeneousReactionInWater(H2O__H_OH,TK) ;
  
  /* Liquid mass density */
  rho_l0 = 1 * kg/dm3 ;
}



int pm(const char* s)
{
#define Parameters_Index(V)  CustomValues_Index(Parameters_t,V,double)
               if(!strcmp(s,"InitialPorosity")) {
    return (Parameters_Index(InitialPorosity)) ;
  } else       if(!strcmp(s,"IntrinsicPermeability")) {
    return (Parameters_Index(IntrinsicPermeability_liquid)) ;
  } else       if(!strcmp(s,"IntrinsicPermeability_liquid")) {
    return (Parameters_Index(IntrinsicPermeability_liquid)) ;
  } else       if(!strcmp(s,"IntrinsicPermeability_gas")) {
    return (Parameters_Index(IntrinsicPermeability_gas)) ;
  } else       if(!strcmp(s,"InitialContent_portlandite")) {
    return (Parameters_Index(InitialContent_portlandite)) ;
  } else       if(!strcmp(s,"InitialContent_csh")) {
    return (Parameters_Index(InitialContent_csh)) ;
  } else       if(!strcmp(s,"InitialConcentration_potassium")) {
    return (Parameters_Index(InitialConcentration_potassium)) ;
  } else       if(!strcmp(s,"InitialConcentration_sodium")) {
    return (Parameters_Index(InitialConcentration_sodium)) ;
  } else       if(!strcmp(s,"DissolutionRate_portlandite")) {
    return (Parameters_Index(DissolutionRate_portlandite)) ;
  } else       if(!strcmp(s,"DissolutionKineticCoef_portlandite")) {
    return (Parameters_Index(DissolutionKineticCoef_portlandite)) ;
  } else       if(!strcmp(s,"CrystalRadius_portlandite")) {
    return (Parameters_Index(CrystalRadius_portlandite)) ;
  } else       if(!strcmp(s,"DiffusionCoefficientInCalcite_co2")) {
    return (Parameters_Index(DiffusionCoefficientInCalcite_co2)) ;
  } else       if(!strcmp(s,"CharacteristicTimeOfDiffusionInCalcite_co2")) {
    return (Parameters_Index(CharacteristicTimeOfDiffusionInCalcite_co2)) ;
  } else       if(!strcmp(s,"FractionalLengthOfPoreBodies")) {
    return (Parameters_Index(FractionalLengthOfPoreBodies)) ;
  } else       if(!strcmp(s,"PorosityFractionAtVanishingPermeability")) {
    return (Parameters_Index(PorosityFractionAtVanishingPermeability)) ;
  } else       if(!strcmp(s,"MinimumPorosity")) {
    return (Parameters_Index(MinimumPorosity)) ;
  } else       if(!strcmp(s,"PrecipitationRate_calcite")) {
    return (Parameters_Index(PrecipitationRate_calcite)) ;
  } else       if(!strcmp(s,"CapillaryPressureLimitOfAsymptoticSaturation")) {
    return (Parameters_Index(CapillaryPressureLimitOfAsymptoticSaturation)) ;
  } else       if(!strcmp(s,"PrecipitationRate_friedelsalt")) {
    return (Parameters_Index(PrecipitationRate_friedelsalt)) ;
  } else       if(!strcmp(s,"UseAutodiff")) {
    return (Parameters_Index(UseAutodiff)) ;
  } else return(-1) ;
#undef Parameters_Index
}


void GetProperties(Element_t* el,double t)
{
  saturationcurve         = Element_FindCurve(el,"s_l") ;
  relativepermliqcurve    = Element_FindCurve(el,"kl_r") ;
#ifdef E_AIR
  relativepermgascurve    = Element_FindCurve(el,"kg_r") ;
#endif
  molarvolumeofcshcurve   = Element_FindCurve(el,"v_csh") ;
#ifdef E_CHLORINE
  adsorbedchloridecurve_a = Element_FindCurve(el,"alpha") ;
  adsorbedchloridecurve_b = Element_FindCurve(el,"beta") ;
#endif
}


int SetModelProp(Model_t* model)
{
  Model_GetNbOfEquations(model) = NEQ ;
  
#ifdef E_CARBON
  Model_CopyNameOfEquation(model,E_CARBON   ,"carbon") ;
#endif

  Model_CopyNameOfEquation(model,E_CHARGE   ,"charge") ;
  Model_CopyNameOfEquation(model,E_MASS     ,"mass") ;
  Model_CopyNameOfEquation(model,E_CALCIUM  ,"calcium") ;
  Model_CopyNameOfEquation(model,E_SODIUM   ,"sodium") ;
  Model_CopyNameOfEquation(model,E_POTASSIUM,"potassium") ;
  
#ifdef E_SILICON
  Model_CopyNameOfEquation(model,E_SILICON  ,"silicon") ;
#endif

#ifdef E_ENEUTRAL
  Model_CopyNameOfEquation(model,E_ENEUTRAL ,"electroneutrality") ;
#endif

#ifdef E_CHLORINE
  Model_CopyNameOfEquation(model,E_CHLORINE ,"chlorine") ;
#endif

#ifdef E_AIR
  Model_CopyNameOfEquation(model,E_AIR      ,"air") ;
#endif
  
  
#if defined (U_LogC_CO2)
  Model_CopyNameOfUnknown(model,E_CARBON ,"logc_co2") ;
#elif defined (U_C_CO2)
  Model_CopyNameOfUnknown(model,E_CARBON ,"c_co2") ;
#endif

#ifdef E_SILICON
  Model_CopyNameOfUnknown(model,E_SILICON,"z_si") ;
#endif

  Model_CopyNameOfUnknown(model,E_MASS    ,"p_l") ;

#if defined (U_ZN_Ca_S)
  Model_CopyNameOfUnknown(model,E_CALCIUM,"z_ca") ;
#elif defined (U_LogS_CH)
  Model_CopyNameOfUnknown(model,E_CALCIUM,"logs_ch") ;
#endif

  Model_CopyNameOfUnknown(model,E_CHARGE    ,"psi") ;
  
#ifdef U_LogC_Na
  Model_CopyNameOfUnknown(model,E_SODIUM   ,"logc_na") ;
#else
  Model_CopyNameOfUnknown(model,E_SODIUM   ,"c_na") ;
#endif

#ifdef U_LogC_K
  Model_CopyNameOfUnknown(model,E_POTASSIUM    ,"logc_k") ;
#else
  Model_CopyNameOfUnknown(model,E_POTASSIUM    ,"c_k") ;
#endif

#ifdef E_ENEUTRAL
  #if defined (U_LogC_OH)
    Model_CopyNameOfUnknown(model,E_ENEUTRAL, "logc_oh") ;
  #elif defined (U_C_OH)
    Model_CopyNameOfUnknown(model,E_ENEUTRAL, "c_oh") ;
  #else
    Model_CopyNameOfUnknown(model,E_ENEUTRAL, "z_oh") ;
  #endif
#endif

#ifdef E_CHLORINE
  #if defined U_LogC_Cl
    Model_CopyNameOfUnknown(model,E_CHLORINE, "logc_cl") ;
  #elif defined U_C_Cl
    Model_CopyNameOfUnknown(model,E_CHLORINE, "c_cl") ;
  #else
    Model_CopyNameOfUnknown(model,E_CHLORINE, "z_cl") ;
  #endif
#endif

#ifdef E_AIR
  Model_CopyNameOfUnknown(model,E_AIR, "p_g") ;
#endif
  
  //Model_GetComputePropertyIndex(model) = &pm ;
  Model_GetComputeMaterialProperties(model) = &GetProperties;
  
  return(0) ;
}


int ReadMatProp(Material_t* mat,DataFile_t* datafile)
/* Lecture des donnees materiaux dans le fichier ficd */
{
  int  NbOfProp = ((int) sizeof(Parameters_t)/sizeof(double)) ;
  /* Parameters */
  Parameters_t& par = ((Parameters_t*) Material_GetProperty(mat))[0] ;
  
  InternationalSystemOfUnits_UseAsLength("decimeter") ;
  InternationalSystemOfUnits_UseAsMass("hectogram") ;

  Material_SetPropertiesToZero(mat,NbOfProp);
  Material_ScanProperties(mat,datafile,pm) ;
  
    
  /* Default initialization */
  {
    double n_csh0 = par.InitialContent_csh;
    
    if(n_csh0 == 0) {
      n_csh0 = 1. ;
      par.InitialContent_csh = n_csh0 ;
    }
  }
  
  {
    double n_ch0 = par.InitialContent_portlandite;
    
    if(n_ch0 == 0) {
      n_ch0 = 1. ;
      par.InitialContent_portlandite = n_ch0 ;
    }
  }
  
  {
    double R_0 = par.CrystalRadius_portlandite; /* (dm) */
    
    if(R_0 == 0.) {
      R_0 = 40.e-5 * dm ;
      par.CrystalRadius_portlandite = R_0 ;
    }
  }
  
  {
    double D   = par.DiffusionCoefficientInCalcite_co2; /* (mol/dm/s) */
    
    if(D == 0.) {
      D = 7e-15 * (mol/dm/sec) ;
      par.DiffusionCoefficientInCalcite_co2 = D;
    }
  }
  
  {
    double t_ch = par.CharacteristicTimeOfDiffusionInCalcite_co2; /* (s) */

    if(t_ch == 0) {
      double h   = 5.6e-6 * (mol/dm2/sec) ;  /* (mol/dm2/s) these MT p 223 */
      double R_0 = par.CrystalRadius_portlandite; /* (dm) */
      t_ch = R_0/(3*h*V_CH) ;     /* (s) approx 721.5 s */
      par.CharacteristicTimeOfDiffusionInCalcite_co2 = t_ch ;
    }
  }
  
  {
    double h   = 5.6e-6 * (mol/dm2/sec) ;  /* (mol/dm2/s) these MT p 223 */
    double R_0 = par.CrystalRadius_portlandite; /* (dm) */
    double t_ch = par.CharacteristicTimeOfDiffusionInCalcite_co2; /* (s) */
    double D   = par.DiffusionCoefficientInCalcite_co2; /* (mol/dm/s) */
    double n_ch0 = par.InitialContent_portlandite;
    double a_2 = n_ch0/t_ch ;  /* (mol/dm3/s) M. Thiery, PhD thesis, p 227 */
    double c_2 = h*R_0/D ;     /* (no dim) M. Thiery, PhD thesis p 228 */
  
    par.DissolutionRate_portlandite = a_2 ;
    par.DissolutionKineticCoef_portlandite = c_2 ;
  }
  
  {
    double frac = par.FractionalLengthOfPoreBodies ;
    
    if(frac == 0) {
      frac = 0.8 ;
      par.FractionalLengthOfPoreBodies = frac ;
    }
  }
  
  
  ComputePhysicoChemicalProperties(TEMPERATURE) ;

  {
    if(!csd) csd = CementSolutionDiffusion_Create() ;
    if(!hcc_d) hcc_d = HardenedCementChemistry_Create<double>() ;
    HardenedCementChemistry_SetRoomTemperature(hcc_d,TEMPERATURE) ;
    
    #ifdef HAVE_AUTODIFF
    if(!hcc_r) hcc_r = HardenedCementChemistry_Create<real>() ;
    HardenedCementChemistry_SetRoomTemperature(hcc_r,TEMPERATURE) ;
    #endif
    
    CementSolutionDiffusion_SetRoomTemperature(csd,TEMPERATURE) ;
  }
  
  
  {
      Curves_t* curves = Material_GetCurves(mat) ;
      int i ;

      if((i = Curves_FindCurveIndex(curves,"s_l")) < 0) {
        arret("ReadMatProp: no s_l - p_c curve") ;
      }

      if((i = Curves_FindCurveIndex(curves,"kl_r")) < 0) {
        arret("ReadMatProp: no kl_r - p_c curve") ;
      }

#ifdef E_AIR
      if((i = Curves_FindCurveIndex(curves,"kg_r")) < 0) {
        arret("ReadMatProp: no kg_r - p_c curve") ;
      }
#endif

      if((i = Curves_FindCurveIndex(curves,"v_csh")) < 0) {
        arret("ReadMatProp: no v_csh - x_csh curve") ;
      }

      if((i = Curves_FindCurveIndex(curves,"X_CSH")) >= 0) {
        Curve_t* curve = Curves_GetCurve(curves) + i ;
      
        HardenedCementChemistry_SetCurveOfCalciumSiliconRatioInCSH(hcc_d,curve) ;
        #ifdef HAVE_AUTODIFF
        HardenedCementChemistry_SetCurveOfCalciumSiliconRatioInCSH(hcc_r,curve) ;
        #endif
      }

      if((i = Curves_FindCurveIndex(curves,"Z_CSH")) >= 0) {
        Curve_t* curve = Curves_GetCurve(curves) + i ;
      
        HardenedCementChemistry_SetCurveOfWaterSiliconRatioInCSH(hcc_d,curve) ;
        #ifdef HAVE_AUTODIFF
        HardenedCementChemistry_SetCurveOfWaterSiliconRatioInCSH(hcc_r,curve) ;
        #endif
      }

      if((i = Curves_FindCurveIndex(curves,"S_SH")) >= 0) {
        Curve_t* curve = Curves_GetCurve(curves) + i ;
      
        HardenedCementChemistry_SetCurveOfSaturationIndexOfSH(hcc_d,curve) ;
        #ifdef HAVE_AUTODIFF
        HardenedCementChemistry_SetCurveOfSaturationIndexOfSH(hcc_r,curve) ;
        #endif
      }
  }
  
  return(NbOfProp) ;
}


int PrintModelChar(Model_t* model,FILE *ficd)
/* Saisie des donnees materiaux */
{
  
  printf(TITLE) ;
  
  if(!ficd) return(NEQ) ;
  
  printf("\n") ;
  printf("The set of 7 equations is:\n") ;
#ifdef E_CARBON
  printf("\t- Mass balance of C      (carbon)\n") ;
#endif
  printf("\t- Mass balance of Ca     (calcium)\n") ;
  printf("\t- Mass balance of Si     (silicon)\n") ;
  printf("\t- Mass balance of Na     (sodium)\n") ;
  printf("\t- Mass balance of K      (potassium)\n") ;
#ifdef E_CHLORINE
  printf("\t- Mass balance of Cl     (chlorine)\n") ;
#endif
  printf("\t- Total mass balance     (mass)\n") ;
  printf("\t- Charge balance         (charge)\n") ;
#ifdef E_ENEUTRAL
  printf("\t- Electroneutrality      (electroneutrality)\n") ;
#endif
#ifdef E_AIR
  printf("\t- Mass blance of air     (air)\n") ;
#endif
  
  printf("\n") ;
  printf("The 7-10 primary unknowns are:\n") ;
  printf("\t- Liquid pressure                  (p_l)\n") ;
#ifdef E_AIR
  printf("\t- Gas pressure                     (p_g)\n") ;
#endif
  printf("\t- Electric potential x F/RT        (psi) \n") ;
  printf("\t- Carbon dioxide gas concentration (c_co2 or logc_co2)\n") ;
  printf("\t- Potassium concentration          (c_k or logc_k)\n") ;
  printf("\t- Sodium concentration             (c_na or logc_na)\n") ;
#if defined (U_ZN_Ca_S)
  printf("\t- Zeta unknown for calcium         (z_ca)\n") ;
  printf("\t   \t z_ca is defined as:\n") ;
  printf("\t   \t z_ca = n_ch/n0 + log(s_ch)  for c_co2 < c_co2_eq\n") ;
  printf("\t   \t z_ca = n_cc/n0 + log(s_cc)  for c_co2 > c_co2_eq\n") ;
#elif defined (U_LogS_CH)
  printf("\t- Log10 of saturation index of CH  (logs_ch)\n") ;
#endif
  printf("\t- Zeta unknown for silicon         (z_si)\n") ;
  printf("\t   \t z_si is defined as:\n") ;
  printf("\t   \t z_si = n_si/n0 + log(s_sh/s_sh_eq)\n") ;
#ifdef E_CHLORINE
  printf("\t- Chloride ion concentration       (c_cl or logc_cl)\n") ;
#endif
#ifdef E_ENEUTRAL
  printf("\t- Hydroxide ion concentration     (c_oh or logc_oh or z_oh)\n") ;
#endif
  
  printf("\n") ;
  printf("PAY ATTENTION to units : \n") ;
  printf("\t length    : dm !\n") ;
  printf("\t time      : s !\n") ;
  printf("\t mass      : hg !\n") ;
  printf("\t pressure  : Pa !\n") ;
  
  printf("\n") ;
  printf("Example of input data\n") ;


  fprintf(ficd,"porosity = 0.38   # Porosity\n") ;
  fprintf(ficd,"kl_int = 1.4e-17   # Intrinsic permeability (dm2)\n") ;
  fprintf(ficd,"N_CH = 3.9        # Initial content in Ca(OH)2 (mol/L)\n") ;
  fprintf(ficd,"CrystalRadius_portlandite = 40.e-5  # Portlandite crystal radius \n") ;
  fprintf(ficd,"N_CSH = 2.4        # Initial content in CSH (mol/L)\n") ;
  fprintf(ficd,"C_Na = 0.019      # Total content in Na (mol/L)\n") ;
  fprintf(ficd,"C_K  = 0.012      # Total content in K  (mol/L)\n") ;
  fprintf(ficd,"D = 7.e-15        # Diffusion coef in CC (dm/mol/s)\n") ;
  fprintf(ficd,"A_2 = 1e-2        # Kinetic coef 2 (dm/mol/s)\n") ;
  fprintf(ficd,"frac = 0.8        # Fractionnal length of pore bodies\n") ;
  fprintf(ficd,"phi_r = 0.7       # Porosity for which permeability vanishes\n") ;
  fprintf(ficd,"Curves = my_file  # File name: p_c S_l kl_r\n") ;  
  fprintf(ficd,"Curves = my_file  # File name: si_ch C/S H/S V_csh\n") ;  
  fprintf(ficd,"Curves = my_file  # File name: s_l kl_r kg_r\n") ;
  fprintf(ficd,"Curves = my_file  # File name: x_csh alpha beta\n") ;

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

  {
    double* r1 = FVM_ComputeSurfaceLoadResidu(fvm,cg,t,dt) ;
    
    for(int i = 0 ; i < NEQ*nn ; i++) r[i] = -r1[i] ;
  }
  
  return(0) ;
}



int ComputeInitialState(Element_t* el)
{
  double** u = Element_ComputePointerToNodalUnknowns(el) ;
  
  /* Modifying the nodal unknowns */
  {
    int nn = Element_GetNbOfNodes(el) ;

    for(int i = 0 ; i < nn ; i++) {
      Values_d& val = *mpm.InitializeValues(el,0,i);
      
      #ifdef U_LogC_Na
        LogC_Na(u,i)  = val.U_sodium ;
      #else
        C_Na(u,i)     = pow(10,val.U_sodium) ;
      #endif
      
      #ifdef U_LogC_K
        LogC_K(u,i)   = val.U_potassium ;
      #else
        C_K(u,i)      = pow(10,val.U_potassium) ;
      #endif

      #ifdef E_ENEUTRAL
        #if defined (U_LogC_OH)
          LogC_OH(u,i) = log10(val.Concentration_oh) ;
        #elif defined (U_C_OH)
          C_OH(u,i)    = val.Concentration_oh ;
        #elif defined (U_Z_OH)
          Z_OH(u,i)    = Z_OHDefinition(c_oh,c_h) ;
        #endif
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
{
  int i = mpm.ComputeMassConservationMatrixByFVM(el,t,dt,k);


/* On output SetTangentMatrix has computed the derivatives wrt
 * LogC_CO2, LogC_Na, LogC_K, LogC_OH, LogC_Cl
 * (see Integrate and Differentiate). */

  #define K(i,j)    (k[(i)*ndof + (j)])
  #ifdef E_CARBON
  #ifdef U_C_CO2
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(int i = 0 ; i < 2*NEQ ; i++){
      K(i,E_CARBON)     /= Ln10*C_CO2(u,0) ;
      K(i,E_CARBON+NEQ) /= Ln10*C_CO2(u,1) ;
    }
  }
  #endif
  #endif

  #ifdef U_C_Na
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(int i = 0 ; i < 2*NEQ ; i++){
      K(i,E_SODIUM)     /= Ln10*C_Na(u,0) ;
      K(i,E_SODIUM+NEQ) /= Ln10*C_Na(u,1) ;
    }
  }
  #endif

  #ifdef U_C_K
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(int i = 0 ; i < 2*NEQ ; i++){
      K(i,E_POTASSIUM)     /= Ln10*C_K(u,0) ;
      K(i,E_POTASSIUM+NEQ) /= Ln10*C_K(u,1) ;
    }
  }
  #endif
  
  #ifdef E_ENEUTRAL
  #if defined (U_C_OH)
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(int i = 0 ; i < 2*NEQ ; i++){
      K(i,E_ENEUTRAL)     /= Ln10*C_OH(u,0) ;
      K(i,E_ENEUTRAL+NEQ) /= Ln10*C_OH(u,1) ;
    }
  }
  #elif defined (U_Z_OH)
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(int i = 0 ; i < 2*NEQ ; i++){
      K(i,E_ENEUTRAL)     *= dLogC_OHdZ_OH(Z_OH(u,0)) ;
      K(i,E_ENEUTRAL+NEQ) *= dLogC_OHdZ_OH(Z_OH(u,1)) ;
    }
  }
  #endif
  #endif
  
  #ifdef E_CHLORINE
  #ifdef U_C_Cl
  {
    double** u = Element_ComputePointerToNodalUnknowns(el) ;
    
    for(int i = 0 ; i < 2*NEQ ; i++){
      K(i,E_CHLORINE)     /= Ln10*C_Cl(u,0) ;
      K(i,E_CHLORINE+NEQ) /= Ln10*C_Cl(u,1) ;
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
  /* 1. Conservation of Carbon: (N_C - N_Cn) + dt * div(W_C) = 0 */
  #ifdef E_CARBON
  {
    int imass = Values_Index(Mole_carbon);
    int iflow = Values_Index(MolarFlow_carbon[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_CARBON,imass,iflow);
  }
  #endif
  
  /* 2. Conservation of charge: div(W_q) = 0 */
  {
    int iflow = Values_Index(MolarFlow_charge[0]);
    mpm.ComputeFluxResiduByFVM(el,t,dt,r,E_CHARGE,iflow);
  }
  
  /* 3. Conservation of total mass: (M_tot - M_totn) + dt * div(W_tot) = 0 */
  {
    int imass = Values_Index(Mass_total);
    int iflow = Values_Index(MassFlow_total[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_MASS,imass,iflow);
  }
  
  /* 4. Conservation of Calcium: (N_Ca - N_Can) + dt * div(W_Ca) = 0 */
  {
    int imass = Values_Index(Mole_calcium);
    int iflow = Values_Index(MolarFlow_calcium[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_CALCIUM,imass,iflow);
  }
  
  /* 5. Conservation of Sodium: (N_Na - N_Nan) + dt * div(W_Na) = 0 */
  {
    int imass = Values_Index(Mole_sodium);
    int iflow = Values_Index(MolarFlow_sodium[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_SODIUM,imass,iflow);
  }
  
  /* 6. Conservation of Potassium: (N_K - N_Kn) + dt * div(W_K) = 0 */
  {
    int imass = Values_Index(Mole_potassium);
    int iflow = Values_Index(MolarFlow_potassium[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_POTASSIUM,imass,iflow);
  }

  /* 7. Conservation of Silicon: (N_Si - N_Sin) + dt * div(W_Si) = 0 */
  #ifdef E_SILICON
  {
    int imass = Values_Index(Mole_silicon);
    int iflow = Values_Index(MolarFlow_silicon[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_SILICON,imass,iflow);
  }
  #endif

  /* 8. Conservation of Chlorine: (N_Cl - N_Cln) + dt * div(W_Cl) = 0 */
  #ifdef E_CHLORINE
  {
    int imass = Values_Index(Mole_chlorine);
    int iflow = Values_Index(MolarFlow_chlorine[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_CHLORINE,imass,iflow);
  }
  #endif
  
  /* 9. Electroneutrality */
  #ifdef E_ENEUTRAL
  {
    int imass = Values_Index(Mole_charge);
    mpm.ComputeBodyForceResiduByFVM(el,t,dt,r,E_ENEUTRAL,imass);
  }
  #endif

  /* 10.  Conservation of dry air mass: (M_Air - M_Airn) + dt * div(W_Air) = 0 */
  #ifdef E_AIR
  {
    int imass = Values_Index(Mass_air);
    int iflow = Values_Index(MassFlow_air[0]);
    mpm.ComputeMassConservationResiduByFVM(el,t,dt,r,E_AIR,imass,iflow);
  }
  #endif

  return(0) ;
}



int  ComputeOutputs(Element_t* el,double t,double* s,Result_t* r)
{
  double* f = Element_GetCurrentImplicitTerm(el) ;
  FVM_t* fvm = FVM_GetInstance(el) ;
  double** u = Element_ComputePointerToNodalUnknowns(el) ;
  int    nso = 70 ;

  if(Element_IsSubmanifold(el)) return(0) ;
  
  /*
    Input data
  */
  Element_ComputeMaterialProperties(el,t) ;
  

  /* Initialization */
  for(int i = 0 ; i < nso ; i++) {
    Result_SetValuesToZero(r + i) ;
  }

  {
    int j = FVM_FindLocalCellIndex(fvm,s) ;
    Values_d& val = *mpm.OutputValues(el,t,j) ;
    HardenedCementChemistry_t<double>* hcc = hcc_d ;
    int i;

    /* Macros */
#define ptC(CPD)   &(HardenedCementChemistry_GetAqueousConcentrationOf(hcc,CPD))
#define ptEC(CPD)  &(HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,CPD))
#define ptS(CPD)   &(HardenedCementChemistry_GetSaturationIndexOf(hcc,CPD))
#define ptPSI      &(HardenedCementChemistry_GetElectricPotential(hcc))
#define ptX_CSH    &(HardenedCementChemistry_GetCalciumSiliconRatioInCSH(hcc))
#define ptZ_CSH    &(HardenedCementChemistry_GetWaterSiliconRatioInCSH(hcc))



    /* Outputs */
    i = 0 ;
    
    /* Liquid pressure */
    Result_Store(r + i++,&val.Pressure_liquid,"p_l",1) ;
    
    /* Liquid saturation degree */
    Result_Store(r + i++,&val.SaturationDegree_liquid,"saturation",1) ;
    
    Result_Store(r + i++,&val.Porosity,"porosity",1) ;
    
    /* Concentration in gas phase */
    Result_Store(r + i++,&val.Concentration_carbondioxide,"c_co2",1) ;
    
    /* Element concentrations in liquid phase */
    Result_Store(r + i++,ptEC(Ca ),"c_ca_l",1) ;
    Result_Store(r + i++,ptEC(Si ),"c_si_l",1) ;
    Result_Store(r + i++,ptEC(Na ),"c_na_l",1) ;
    Result_Store(r + i++,ptEC(K  ),"c_k_l" ,1) ;
    Result_Store(r + i++,ptEC(C  ),"c_c_l" ,1) ;
    Result_Store(r + i++,ptEC(Cl ),"c_cl_l",1) ;
    
    /* Portlandite */
    Result_Store(r + i++,&val.Mole_solidportlandite,"n_CH",1) ;
    Result_Store(r + i++,ptS(CH),"s_ch",1) ;
    
    /* C-S-H */
    Result_Store(r + i++,&val.Mole_solidcsh,"n_CSH",1) ;
    Result_Store(r + i++,ptX_CSH,"x_csh",1) ;
    Result_Store(r + i++,ptS(SH),"s_sh",1) ;
    
    /* Calcite */
    Result_Store(r + i++,&val.Mole_solidcalcite,"n_CC",1) ;
    Result_Store(r + i++,ptS(CC),"s_cc",1) ;
    
    /* Friedel's salt */
    Result_Store(r + i++,&val.Mole_solidfriedelsalt,"n_Friedel's salt",1) ;
    Result_Store(r + i++,ptS(FriedelSalt),"s_friedelsalt",1) ;
    
    
    /* Ion concentrations in liquid phase */
    Result_Store(r + i++,ptC(H ),"c_h",1) ;
    Result_Store(r + i++,ptC(OH),"c_oh",1) ;
    {
      double c_h       = *(ptC(H )) ;
      double ph        = - log10(c_h) ;
      
      Result_Store(r + i++,&ph,"ph",1) ;
    }
    
    Result_Store(r + i++,ptC(Ca  ),"c_ca",1) ;
    Result_Store(r + i++,ptC(CaOH),"c_caoh",1) ;
    
    Result_Store(r + i++,ptC(H2SiO4),"c_h2sio4",1) ;
    Result_Store(r + i++,ptC(H3SiO4),"c_h3sio4",1) ;
    Result_Store(r + i++,ptC(H4SiO4),"c_h4sio4",1) ;
    
    Result_Store(r + i++,ptC(Na  ),"c_na",1) ;
    Result_Store(r + i++,ptC(NaOH),"c_naoh",1) ;
    
    Result_Store(r + i++,ptC(K  ),"c_k",1) ;
    Result_Store(r + i++,ptC(KOH),"c_koh",1) ;
    
    Result_Store(r + i++,ptC(CO3 ),"c_co3",1) ;
    Result_Store(r + i++,ptC(HCO3),"c_hco3",1) ;
    
    Result_Store(r + i++,ptC(CaH2SiO4),"c_cah2sio4",1) ;
    Result_Store(r + i++,ptC(CaH3SiO4),"c_cah3sio4",1) ;
    
    Result_Store(r + i++,ptC(CaHCO3),"c_cahco3",1) ;
    Result_Store(r + i++,ptC(CaCO3),"c_caco3aq",1) ;
    //Result_Store(r + i++,ptC(CaO2H2),"c_caoh2aq",1) ;
    {
      double zero = 0;
      Result_Store(r + i++,&zero,"c_caoh2aq",1) ;
    }
    
    Result_Store(r + i++,ptC(NaHCO3),"c_nahco3",1) ;
    Result_Store(r + i++,ptC(NaCO3),"c_naco3",1) ;
    
    Result_Store(r + i++,ptC(Cl),"c_cl",1) ;
    
    /* Total element contents */
    Result_Store(r + i++,&val.Mole_calcium,"n_Ca",1) ;
    Result_Store(r + i++,&val.Mole_silicon,"n_Si",1) ;
    Result_Store(r + i++,&val.Mole_sodium,"n_Na",1) ;
    Result_Store(r + i++,&val.Mole_potassium ,"n_K" ,1) ;
    Result_Store(r + i++,&val.Mole_carbon ,"n_C" ,1) ;
    Result_Store(r + i++,&val.Mole_chlorine,"n_Cl" ,1) ;
    
    /* Total mass content */
    Result_Store(r + i++,&val.Mass_total,"total mass",1) ;
    
    /* Mass flows */
    {
      CustomValues_t<double,ImplicitValues_t>* vi = (CustomValues_t<double,ImplicitValues_t>*) f ;
      
      Result_Store(r + i++,vi[0].MassFlow_total+1,"total mass flow",1) ;
      Result_Store(r + i++,vi[0].MolarFlow_carbon+1,"carbon mass flow",1) ;
      Result_Store(r + i++,vi[0].MolarFlow_calcium+1,"calcium mass flow",1) ;
      Result_Store(r + i++,vi[0].MolarFlow_silicon+1,"silicon mass flow",1) ;
      Result_Store(r + i++,vi[0].MolarFlow_sodium+1,"sodium mass flow",1) ;
      Result_Store(r + i++,vi[0].MolarFlow_potassium+1,"potassium mass flow",1) ;
      Result_Store(r + i++,vi[0].MolarFlow_chlorine+1,"chlorine mass flow",1) ;
    }
    
    
    /* Miscellaneous */
    {
      double CS = val.Mole_solidcalcium/val.Mole_solidsilicon ;
      
      Result_Store(r + i++,&CS,"Ca/Si ratio",1) ;
    }
    
    {
      double psi = PSI(u,j) ;
      
      Result_Store(r + i++,&psi,"Electric potential",1) ;
    }
    
    Result_Store(r + i++,&val.Mole_charge,"charge",1) ;
    
    {
      double I = HardenedCementChemistry_GetIonicStrength(hcc) ;
      
      Result_Store(r + i++,&I,"I",1) ;
    }
    
    /* Molar volumes */
    {
      double v_solide_csh   = val.Volume_solidcsh * val.Mole_solidcsh ;
      
      Result_Store(r + i++,&v_solide_csh,"v_csh",1) ;
    }
    {
      double v_solide_ch    = V_CH * val.Mole_solidportlandite ;
      
      Result_Store(r + i++,&v_solide_ch,"v_ch",1) ;
    }
    {
      double v_solide_cc    = V_CC * val.Mole_solidcalcite ;
      
      Result_Store(r + i++,&v_solide_cc,"v_cc",1) ;
    }
    
    /* Gas pressures */
    {
      double p_l        = val.Pressure_liquid ;
      double p_g        = val.Pressure_gas ;
      double p_v        = VaporPressure(p_l) ;
      double h_r        = RelativeHumidity(p_l) ;
      double p_co2      = val.Concentration_carbondioxide * RT ;
      double p_atm      = 0.101325 * MPa ;
      double c_co2      = p_co2 / p_atm * 1.e6 ;
      double p_air      = p_g - p_v - p_co2 ;
      
      Result_Store(r + i++,&p_air,"air pressure",1) ;
      Result_Store(r + i++,&h_r,"humidity",1) ;
      Result_Store(r + i++,&c_co2,"CO2 ppm",1) ;
      Result_Store(r + i++,&p_g,"gas pressure",1) ;
    }
      

    /* Additional outputs */
    {
      double s_l        = val.SaturationDegree_liquid ;
      double phi        = val.Porosity ;
      double coeff_permeability = PermeabilityCoefficient(el,phi) ;
      double taul       = TortuosityToLiquid(phi,s_l) ;

      Result_Store(r + i++,&taul,"tortuosity to liquid",1) ;
      Result_Store(r + i++,&coeff_permeability,"permeability coef",1) ;
    }
    
    /* Adsorbed chloride */
    {
      double n_csh  = val.Mole_solidcsh ;
      double c_cl   = (ptC(Cl))[0] ;
      double x_csh  = (ptX_CSH)[0] ;
      double n_cl_s = n_csh * AdsorbedChloridePerUnitMoleOfCSH(c_cl,x_csh) ;
      
      Result_Store(r + i++,&n_cl_s,"adsorbed chloride",1) ;
    }
    
    /* Liquid mass density */
    {
      double rho_l  = HardenedCementChemistry_GetLiquidMassDensity(hcc) ;
      
      Result_Store(r + i++,&rho_l,"liquid mass density",1) ;
    }
    
    if(i != nso) arret("ComputeOutputs") ;
  }
  
  
  return(nso) ;
}



int MPM_t::SetTangentMatrix(Element_t* el,double const& t,double const& dt,int const& i,Values_d const& val,Values_d const& dval,int const& k,double* c)
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
        #ifdef E_CARBON
        cij[E_CARBON*NEQ    + k] = dval.Mole_carbon ;
        #endif
        cij[E_CALCIUM*NEQ   + k] = dval.Mole_calcium ;
        cij[E_SODIUM*NEQ    + k] = dval.Mole_sodium ;
        #ifdef E_SILICON
        cij[E_SILICON*NEQ   + k] = dval.Mole_silicon ;
        #endif
        cij[E_POTASSIUM*NEQ + k] = dval.Mole_potassium ;
        cij[E_MASS*NEQ      + k] = dval.Mass_total ;
        #ifdef E_ENEUTRAL
        cij[E_ENEUTRAL*NEQ  + k] = dval.Mole_charge ;
        #endif
        #ifdef E_CHLORINE
        cij[E_CHLORINE*NEQ  + k] = dval.Mole_chlorine ;
        #endif
        #ifdef E_AIR
        cij[E_AIR*NEQ       + k] = dval.Mass_air ;
        #endif
      }

      /* Transfer terms from node i to node j: d(wij)/d(ui_k) */
      if(j != i) {        
        #ifdef E_CARBON
        cij[E_CARBON*NEQ    + k] = - dtdij*dval.MolarFlow_carbon[j] ;
        #endif
        cij[E_CALCIUM*NEQ   + k] = - dtdij*dval.MolarFlow_calcium[j] ;
        cij[E_SODIUM*NEQ    + k] = - dtdij*dval.MolarFlow_sodium[j] ;
        #ifdef E_SILICON
        cij[E_SILICON*NEQ   + k] = - dtdij*dval.MolarFlow_silicon[j] ;
        #endif
        cij[E_POTASSIUM*NEQ + k] = - dtdij*dval.MolarFlow_potassium[j] ;
        cij[E_MASS*NEQ      + k] = - dtdij*dval.MassFlow_total[j] ;
        cij[E_CHARGE*NEQ    + k] = - dval.MolarFlow_charge[j]/dij;
        #ifdef E_CHLORINE
        cij[E_CHLORINE*NEQ  + k] = - dtdij*dval.MolarFlow_chlorine[j]  ;
        #endif
        #ifdef E_AIR
        cij[E_AIR*NEQ       + k] = - dtdij*dval.MassFlow_air[j] ;
        #endif
      }
    }
  }

  return(dec) ;
}



void  MPM_t::SetIndexOfPrimaryVariables(Element_t* el,int* ind)
{
  ind[E_CALCIUM] = Values_Index(U_calcium);
  #ifdef E_SILICON
  ind[E_SILICON] = Values_Index(U_silicon);
  #endif
  ind[E_SODIUM] = Values_Index(U_sodium);
  ind[E_POTASSIUM] = Values_Index(U_potassium);
  ind[E_CHARGE] = Values_Index(U_charge);
  ind[E_MASS] = Values_Index(U_mass);
  #ifdef E_CARBON
  ind[E_CARBON] = Values_Index(U_carbon);
  #endif
  #ifdef E_ENEUTRAL
  ind[E_ENEUTRAL] = Values_Index(U_eneutral);
  #endif
  #ifdef E_CHLORINE
  ind[E_CHLORINE] = Values_Index(U_chlorine);
  #endif
  #ifdef E_AIR
  ind[E_AIR] = Values_Index(U_air);
  #endif
}



  
  

void MPM_t::SetIncrementOfPrimaryVariables(Element_t* el,double* dui)
{
  double** u = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  double** u_n = Element_ComputePointerToPreviousNodalUnknowns(el) ;
  int ncols = NEQ;
  int nn = Element_GetNbOfNodes(el);
    
    {
      ObVal_t* obval = Element_GetObjectiveValue(el) ;
      double un_calcium = 0;
      double u_calcium = 0;
    
      for(int i = 0 ; i < NEQ ; i++) {
        dui[i] =  1.e-2 * ObVal_GetValue(obval + i) ;
      }

    
      #ifdef E_CARBON
        dui[E_CARBON   ] =  1.e-4 * ObVal_GetValue(obval + E_CARBON) ;
        /* Derivation wrt LogC_CO2 -> relative value */
        #ifdef U_C_CO2
        {
          double un_carbon = 0;
          
          for(int i = 0 ; i < nn ; i++) {
            un_carbon    += U_CARBON(u_n,i)/nn;
          }
          
          dui[E_CARBON   ] =  1.e-4 * ObVal_GetRelativeValue(obval + E_CARBON,un_carbon) ;
        }
        #endif
      #endif

      dui[E_SODIUM   ] =  1.e-3 * ObVal_GetValue(obval + E_SODIUM) ;
      /* Derivation wrt LogC_Na -> relative value */
      #ifdef U_C_Na
      {
        double un_sodium = 0;
        
        for(int i = 0 ; i < nn ; i++) {
          un_sodium    += U_SODIUM(u_n,i)/nn;
        }
        
        dui[E_SODIUM   ] =  1.e-3 * ObVal_GetRelativeValue(obval + E_SODIUM,un_sodium) ;
      }
      #endif
    
      dui[E_POTASSIUM] =  1.e-3 * ObVal_GetValue(obval + E_POTASSIUM) ;
      /* Derivation wrt LogC_K -> relative value */
      #ifdef U_C_K
      {
        double un_potassium = 0;
        
        for(int i = 0 ; i < nn ; i++) {
          un_potassium += U_POTASSIUM(u_n,i)/nn;
        }
        
        dui[E_POTASSIUM] =  1.e-3 * ObVal_GetRelativeValue(obval + E_POTASSIUM,un_potassium) ;
      }
      #endif

      dui[E_CALCIUM  ] =  1.e-4 * ObVal_GetValue(obval + E_CALCIUM) ;
      for(int i = 0 ; i < nn ; i++) {
        un_calcium  += U_CALCIUM(u_n,i)/nn;
        u_calcium   += U_CALCIUM(u,i)/nn;
      }
      #if defined (U_ZN_Ca_S)
        dui[E_CALCIUM  ] =  1.e-4 * ObVal_GetAbsoluteValue(obval + E_CALCIUM,un_calcium) ;
        dui[E_CALCIUM  ] *= ((u_calcium > un_calcium) ? 1 : -1) ;
      #elif defined (U_LogS_CH)
        dui[E_CALCIUM  ] =  1.e-4 * ObVal_GetAbsoluteValue(obval + E_CALCIUM,un_calcium) ;
      #endif
  
      #ifdef E_SILICON
      {
        double un_silicon = 0;
        double u_silicon = 0;
        
        for(int i = 0 ; i < nn ; i++) {
          un_silicon  += U_SILICON(u_n,i)/nn;
          u_silicon   += U_SILICON(u,i)/nn;
        }
        
        dui[E_SILICON  ] =  1.e-4 * ObVal_GetValue(obval + E_SILICON) ;
        dui[E_SILICON  ] =  1.e-4 * ObVal_GetAbsoluteValue(obval + E_SILICON,un_silicon) ;
        dui[E_SILICON  ] *= ((u_silicon > un_silicon) ? 1 : -1) ; 
      }
      #endif

      dui[E_MASS     ] =  1.e-4 * ObVal_GetValue(obval + E_MASS) ;
      dui[E_CHARGE   ] =  1.e+0 * ObVal_GetValue(obval + E_CHARGE) ;
  
      #ifdef E_ENEUTRAL
      {
        double un_eneutral = 0;
        
        for(int i = 0 ; i < nn ; i++) {
          un_eneutral += U_ENEUTRAL(u_n,i)/nn;
        }
        dui[E_ENEUTRAL ] =  1.e-2 * ObVal_GetValue(obval + E_ENEUTRAL) ;
        #if defined (U_C_OH)
          /* Derivation wrt LogC_OH -> relative value */
          dui[E_ENEUTRAL ] =  1.e-2 * ObVal_GetRelativeValue(obval + E_ENEUTRAL,un_eneutral) ;
        #elif defined (U_Z_OH)
        {
          double z_oh = un_eneutral;
          double c_oh = C_OHDefinition(z_oh);
          dui[E_ENEUTRAL ] =  1.e-3 * ObVal_GetAbsoluteValue(obval + E_ENEUTRAL,z_oh) * c_oh * fabs(  dLogC_OHdZ_OH(z_oh)) ;
        }
        #endif
      }
      #endif

      #ifdef E_CHLORINE
      dui[E_CHLORINE ] =  1.e-3 * ObVal_GetValue(obval + E_CHLORINE) ;
        /* Derivation wrt LogC_Cl -> relative value */
        #ifdef U_C_Cl
        {
          double un_chlorine = 0;
          
          for(int i = 0 ; i < nn ; i++) {
            un_chlorine  += U_CHLORINE(u_n,i)/nn;
          }
          
          dui[E_CHLORINE ] =  1.e-3 * ObVal_GetRelativeValue(obval + E_CHLORINE,un_chlorine) ;
        }
        #endif
      #endif

      #ifdef E_AIR
      dui[E_AIR      ] =  1.e-4 * ObVal_GetValue(obval + E_AIR) ;
      #endif
    }
}




Values_d* MPM_t::SetInputs(Element_t* el,const double& t,const int& n,double const* const* u,Values_d& val)
{
  #ifdef E_CARBON
  val.U_carbon = LogC_CO2(u,n) ;
  #endif

  val.U_sodium = LogC_Na(u,n) ;
  val.U_potassium = LogC_K(u,n) ;
  val.U_calcium = U_CALCIUM(u,n) ;

  #ifdef E_SILICON
  val.U_silicon = U_SILICON(u,n) ;
  #endif

  val.U_mass = P_L(u,n) ;
  val.U_charge = PSI(u,n) ;

  #ifdef E_ENEUTRAL
  val.U_eneutral = LogC_OH(u,n) ;
  #endif

  #ifdef E_CHLORINE
  val.U_chlorine = LogC_Cl(u,n) ;
  #endif

  #ifdef E_AIR
  val.U_air = P_G(u,n) ;
  #endif
  
  return(&val) ;
}



template <typename T>
Values_t<T>* MPM_t::Integrate(Element_t* el,const double& t,const double& dt,Values_d const& val_n,Values_t<T>& val)
/** Compute the secondary variables from the primary ones. */
{
  /* To retrieve the material properties */
  Parameters_t& par = ((Parameters_t*) Element_GetProperty(el))[0] ;
  double phi0             = par.InitialPorosity;
  double phi_min          = par.MinimumPorosity;
  double kl_int           = par.IntrinsicPermeability_liquid;
  double kg_int           = par.IntrinsicPermeability_gas;
  double a_2              = par.DissolutionRate_portlandite;
  double c_2              = par.DissolutionKineticCoef_portlandite ;
  double rate_calcite     = par.PrecipitationRate_calcite;
  double rate_friedelsalt = par.PrecipitationRate_friedelsalt;
  double p_c3             = par.CapillaryPressureLimitOfAsymptoticSaturation;
  double n_ch0            = par.InitialContent_portlandite;
  double n_csh0           = par.InitialContent_csh;
  
  #ifdef E_CARBON
  T logc_co2   = val.U_carbon ;
  #else
  T logc_co2   = -99 ;
  #endif
  T u_calcium  = val.U_calcium ;
  #ifdef E_SILICON
  T u_silicon  = val.U_silicon ;
  #else
  T u_silicon  = 1 ;
  #endif
  T p_l        = val.U_mass ;
  #ifdef E_AIR
  T p_g        = val.U_air ;
  #else
  T p_g        = p_g0 ;
  #endif
  #ifdef E_CHLORINE
  T logc_cl    = val.U_chlorine ;
  #else
  T logc_cl    = -99 ;
  #endif
  T c_cl       = pow(10,logc_cl) ;
  
  
  /* Liquid components */
  T c_co2      = pow(10,logc_co2) ;
  T logc_co2aq = log(k_h) + logc_co2 ;
  //T c_co2aq    = k_h*c_co2 ;
  
  HardenedCementChemistry_t<T>* hcc = hcc_func<T>();


  /* Solve cement chemistry */
  {
    T logc_na  = val.U_sodium ;
    T logc_k   = val.U_potassium ;
    //T logc_co2aq = log10(c_co2aq) ;
    #ifdef E_ENEUTRAL
    T logc_oh  = val.U_eneutral ;
    #else
    double logc_oh  = log10(val_n.Concentration_oh) ;
    #endif
    //double logc_oh  = log10(c_oh) ;
    T psi      = val.U_charge ;

    #if defined (U_ZN_Ca_S)
    {
      T si_ch_cc = Log10SaturationIndexOfCcH(u_calcium) ;
          
      HardenedCementChemistry_SetInput(hcc,SI_CH_CC,si_ch_cc) ;
    }
    #elif defined (U_LogS_CH)
    {
      T si_ch = Log10SaturationIndexOfCH(u_calcium) ;
          
      HardenedCementChemistry_SetInput(hcc,SI_CH,si_ch) ;
    }
    #endif
        
    {
      T si_csh = Log10SaturationIndexOfCSH(u_silicon) ;
          
      HardenedCementChemistry_SetInput(hcc,SI_CSH,si_csh) ;
    }
  
    HardenedCementChemistry_SetInput(hcc,LogC_CO2,logc_co2aq) ;
    HardenedCementChemistry_SetInput(hcc,LogC_Na,logc_na) ;
    HardenedCementChemistry_SetInput(hcc,LogC_K,logc_k) ;
    HardenedCementChemistry_SetInput(hcc,LogC_OH,logc_oh) ;
    HardenedCementChemistry_SetElectricPotential(hcc,psi) ;
    HardenedCementChemistry_SetInput(hcc,LogC_Cl,logc_cl) ;
    
    #ifdef E_CHLORINE
    HardenedCementChemistry_ComputeSystem(hcc,CaO_SiO2_Na2O_K2O_CO2_Cl_H2O) ;
    #else
    HardenedCementChemistry_SetAqueousConcentrationOf(hcc,Cl,c_cl) ;
    HardenedCementChemistry_SetLogAqueousConcentrationOf(hcc,Cl,logc_cl) ;
    HardenedCementChemistry_ComputeSystem(hcc,CaO_SiO2_Na2O_K2O_CO2_H2O) ;
    #endif

    #ifndef E_ENEUTRAL
    {
      int k = HardenedCementChemistry_SolveElectroneutrality(hcc) ;
      
      if(k < 0) return(NULL) ;
    }
    #endif
  }
  
  
  
  /* Backup */
  
  T c_q_l  = HardenedCementChemistry_GetLiquidChargeDensity(hcc) ;
  
  T rho_l  = HardenedCementChemistry_GetLiquidMassDensity(hcc) ;
  
  T c_c_l  = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,C) ;
  T c_ca_l = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,Ca) ;
  T c_na_l = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,Na) ;
  T c_k_l  = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,K) ;
  T c_si_l = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,Si) ;
  T c_cl_l = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,Cl) ;
  
  T s_ch   = HardenedCementChemistry_GetSaturationIndexOf(hcc,CH) ;
  T s_cc   = HardenedCementChemistry_GetSaturationIndexOf(hcc,CC) ;
  T s_friedelsalt = HardenedCementChemistry_GetSaturationIndexOf(hcc,FriedelSalt) ;
       
    
  /* Solid contents */
  /* ... as components: CH, CC, CSH, C3A, Friedel's salt */
  double n_chn      = val_n.Mole_solidportlandite ;
  double n_ccn      = val_n.Mole_solidcalcite ;
  double n_friedelsaltn = val_n.Mole_solidfriedelsalt ;
  T n_ch       = CHSolidContent(u_calcium,n_chn,n_ccn,s_ch,s_cc,dt) ;
  T n_cc       = CCSolidContent(u_calcium,n_chn,n_ccn,s_ch,s_cc,dt) ;
  T n_csh      = CSHSolidContent(u_silicon) ;
  T n_friedelsalt = FriedelSaltContent(n_friedelsaltn,s_friedelsalt,dt) ;
  T n_c3a      = - n_friedelsalt ;
  
  /* ... as elements: C, Ca, Si, Cl */
  T x_csh      = HardenedCementChemistry_GetCalciumSiliconRatioInCSH(hcc) ;
  T n_si_s     = n_csh ;
  T n_ca_s     = n_ch + n_cc + x_csh * n_csh + 4 * n_friedelsalt + 3 * n_c3a ;
  T n_c_s      = n_cc ;
  T n_cl_ads   = n_csh * AdsorbedChloridePerUnitMoleOfCSH(c_cl,x_csh) ;
  T n_cl_s     = n_cl_ads + 2 * n_friedelsalt ;
  
  /* ... as mass */
  T z_csh      = HardenedCementChemistry_GetWaterSiliconRatioInCSH(hcc) ;
  T m_csh      = MolarMassOfCSH(x_csh,z_csh) * n_csh ;
  T m_ch       = M_CaOH2 * n_ch ;
  T m_cc       = M_CaCO3 * n_cc ;
  T m_c3a      = M_C3A * n_c3a ;
  T m_friedelsalt = M_FriedelSalt * n_friedelsalt ;
  T m_cl_ads   = (M_Cl - M_OH) * n_cl_ads ;
  T m_s        = m_ch + m_cc + m_csh + m_cl_ads + m_friedelsalt + m_c3a ;
  
  /* ... as volume */
  T v_csh      = MolarVolumeOfCSH(x_csh) ;
  T v_s        = V_CH * n_ch + V_CC * n_cc + v_csh * n_csh ;
  
  
  /* Porosity */
  double v_s0     = val_n.InitialVolume_solidtotal ;
  T phi_th   = phi0 + v_s0 - v_s ;
  T phi      = MAX(phi_th,phi_min) ;
  
  
  /* Pressures */
  T p_c      = p_g - p_l ;
  T p_v      = VaporPressure(p_l) ;
  T p_co2    = c_co2 * RT ;
  T p_air    = p_g - p_v - p_co2 ;
  
  
  /* Saturation */
  T s_l      = SaturationDegree(p_c) ;
  T s_g      = 1 - s_l ;
  
  
  /* Liquid contents */
  T phi_l  = phi * s_l ;
  /* ... as elements: C, Ca, Si */
  T n_c_l  = phi_l * c_c_l ;
  T n_ca_l = phi_l * c_ca_l ;
  T n_na_l = phi_l * c_na_l ;
  T n_k_l  = phi_l * c_k_l ;
  T n_si_l = phi_l * c_si_l ;
  T n_cl_l = phi_l * c_cl_l ;
  /* ... as charge */
  //T n_q_l  = phi_l * c_q_l ;
  /* ... as mass */
  T m_l    = phi_l * rho_l ;
       
       
  /* Gas contents */
  T phi_g  = phi * s_g ;
  /* ... as elements */
  T n_c_g  = phi_g * c_co2 ;
  /* ... as densities */
  T rho_co2_g   = M_CO2 * c_co2 ;
  T rho_h2o_g   = MassDensityOfWaterVapor(p_v) ;
  T rho_air_g   = MassDensityOfDryAir(p_air) ;
  T rho_noair_g = rho_h2o_g + rho_co2_g ;
  T rho_g       = rho_air_g + rho_noair_g ;
  /* ... as masses */
  T m_air_g   = phi_g * rho_air_g ;
  T m_noair_g = phi_g * rho_noair_g ;
  //T m_g       = phi_g * rho_g ;
  
  
      
  if constexpr(std::is_same_v<T,double>) {
  if(c_co2 < 0) {
    double c_naoh   = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,NaOH) ;
    double c_nahco3 = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,NaHCO3) ;
    double c_naco3  = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,NaCO3) ;
    // c_cl     = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,Cl) ;
    printf("\n") ;
    printf("c_co2    = %e\n",c_co2) ;
    //printf("c_oh     = %e\n",pow(10,logc_oh)) ;
    printf("n_cc     = %e\n",n_cc) ;
    //printf("c_na     = %e\n",pow(10,logc_na)) ;
    //printf("c_k      = %e\n",pow(10,logc_k)) ;
    printf("n_csh    = %e\n",n_csh) ;
    printf("c_naoh   = %e\n",c_naoh) ;
    printf("c_nahco3 = %e\n",c_nahco3) ;
    printf("c_naco3  = %e\n",c_naco3) ;
    printf("c_cl     = %e\n",c_cl) ;
    return(NULL) ;
  }
  }


  /* Back up */
  

  /* Gas components */
  val.Pressure_gas = p_g ;
  val.Concentration_carbondioxide = c_co2 ;
  val.MassDensity_watervapor = rho_h2o_g ;
  
  /* Liquid components */
  val.Pressure_liquid = p_l ;
  val.SaturationDegree_liquid = s_l ;
  
  /* Solid components */
  val.Mole_solidportlandite = n_ch ;
  val.Volume_solidtotal = v_s ;
  val.Mole_solidsilicon = n_si_s ;
  val.Mole_solidcalcium = n_ca_s ;
  val.Mole_solidcalcite = n_cc ;
  val.Mole_solidcsh = n_csh ;
  val.Mole_solidfriedelsalt = n_friedelsalt ;
  val.Volume_solidcsh = v_csh ;
  
  /* Porosity */
  val.Porosity = phi ;
  
  /* Element contents */
  val.Mole_carbon = n_c_l  + n_c_s  + n_c_g ;
  val.Mole_calcium  = n_ca_l + n_ca_s ;
  val.Mole_sodium  = n_na_l ; 
  val.Mole_potassium  = n_k_l  ;
  val.Mole_silicon  = n_si_l + n_si_s ;
  val.Mole_chlorine  = n_cl_l + n_cl_s ;
  
  /* Mass of dry air */
  val.Mass_air = m_air_g ;
  
  /* Total mass */
  val.Mass_total  = m_noair_g + m_l + m_s ;
  
  /* Charge density */
  //x[I_N_Q]   = n_q_l ;
  val.Mole_charge   = c_q_l ;
  
  /* Hydroxide ion concentration */
  val.Concentration_oh  = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,OH) ;

  /* Chemical potentials */
  HardenedCementChemistry_CopyChemicalPotential(hcc,val.ChemicalPotential) ;
  
  /*
    Transfer coefficients
  */
  {
    /* Advective transport in liquid and gas phases */
    {
      /* Permeabilities */
      T coeff_permeability = PermeabilityCoefficient(el,phi) ;
      T k_l  = (kl_int/mu_l)*RelativePermeabilityToLiquid(s_l)*coeff_permeability ;
      T k_g  = (kg_int/mu_g)*RelativePermeabilityToGas(s_l)*coeff_permeability ;
    
      val.Permeability_liquid = rho_l * k_l ;
      val.Permeability_gas = rho_g * k_g ;

      val.MoleFractionLiquid_carbon = c_c_l  / rho_l ;
      val.MoleFractionLiquid_calcium = c_ca_l / rho_l ;
      val.MoleFractionLiquid_sodium = c_na_l / rho_l ;
      val.MoleFractionLiquid_potassium = c_k_l  / rho_l ;
      val.MoleFractionLiquid_silicon = c_si_l / rho_l ;
      val.MoleFractionLiquid_chlorine = c_cl_l / rho_l ;
      
      val.MoleFractionGas_carbondioxide = c_co2 / rho_g ;
      val.MassFractionGas_watervapor    = rho_h2o_g / rho_g ;
    }
    
    
    /* Diffusive transport in liquid and gas phases */
    {
      /* tortuosity liquid */
      T tauliq =  TortuosityToLiquid(phi,s_l) ;
      /* tortuosity gas */
      T taugas  = TortuosityToGas(phi,s_l) ;
      
      val.DiffusionCoefficient_carbondioxide = phi_g * taugas * d_co2 ;
      val.DiffusionCoefficient_watervapor    = phi_g * taugas * d_vap ;
      
      val.Tortuosity_liquid = tauliq ;
    }


    /* Concentrations */
    HardenedCementChemistry_CopyConcentrations(hcc,val.AqueousConcentration) ;
  }
  
  return(&val) ;
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
  
  
  /* Transport in liquid phase */
  {
    /* Diffusion in the cement solution */
    {
      /* Gradients */
      double* g = CementSolutionDiffusion_GetGradient(csd) ;
      int n = CementSolutionDiffusion_NbOfConcentrations ;
      double* cij = valij.AqueousConcentration ;
      double tortuosity = valij.Tortuosity_liquid ;
      
      for(int k = 0 ; k < n ; k++) {
        double rho = cij[k] ;
      
        g[k] = tortuosity * rho * grdval.ChemicalPotential[k] ;
      }
      
      /* Molar diffusive fluxes */
      CementSolutionDiffusion_ComputeFluxes(csd) ;
    }
      
    {
      vali.MolarFlow_carbon[j]    = CementSolutionDiffusion_GetElementFluxOf(csd,C) ;
      vali.MolarFlow_calcium[j]   = CementSolutionDiffusion_GetElementFluxOf(csd,Ca) ;
      vali.MolarFlow_silicon[j]   = CementSolutionDiffusion_GetElementFluxOf(csd,Si) ;
      vali.MolarFlow_sodium[j]    = CementSolutionDiffusion_GetElementFluxOf(csd,Na) ;
      vali.MolarFlow_potassium[j] = CementSolutionDiffusion_GetElementFluxOf(csd,K) ;
      vali.MolarFlow_charge[j]    = CementSolutionDiffusion_GetIonCurrent(csd) ;
      vali.MolarFlow_chlorine[j]  = CementSolutionDiffusion_GetElementFluxOf(csd,Cl) ;
    }
  
    /* Advection in the cement solution */
    {
      /* Mass flux of liquid */
      double grd_p_l = grdval.Pressure_liquid ;
      double kd_l    = valij.Permeability_liquid ;
      double w_l     = - kd_l * grd_p_l  ;
      
      /* Transfer terms */
      double kc_c_l   = valij.MoleFractionLiquid_carbon ;
      double kc_ca_l  = valij.MoleFractionLiquid_calcium ;
      double kc_si_l  = valij.MoleFractionLiquid_silicon ;
      double kc_na_l  = valij.MoleFractionLiquid_sodium ;
      double kc_k_l   = valij.MoleFractionLiquid_potassium ;
      double kc_cl_l  = valij.MoleFractionLiquid_chlorine ;

      /* Mass flux */
      vali.MassFlow_total[j]   = w_l  ;
   
      /* Molar fluxes */
      vali.MolarFlow_carbon[j]    += kc_c_l  * w_l  ;
      vali.MolarFlow_calcium[j]   += kc_ca_l * w_l  ;
      vali.MolarFlow_silicon[j]   += kc_si_l * w_l  ;
      vali.MolarFlow_sodium[j]    += kc_na_l * w_l  ;
      vali.MolarFlow_potassium[j] += kc_k_l  * w_l  ;
      vali.MolarFlow_chlorine[j]  += kc_cl_l * w_l  ;
    }
  }
  
  /* Transport in gas phase */
  {
    /* Advection and diffusion in gas phase */
    {
      /* Mass flux of gas */
      #ifdef E_AIR
      double grd_p_g = grdval.Pressure_gas ;
      double kd_g    = valij.Permeability_gas ;
      double w_g     = - kd_g * grd_p_g  ;
      #else
      /* Actually kg_int -> infinity and w_g is undetermined! */
      double w_g     = 0  ;
      #endif
      
      
      /* Molar flux of CO2 */
      double grd_co2 = grdval.Concentration_carbondioxide ;
      double kf_co2  = valij.DiffusionCoefficient_carbondioxide ;
      double j_co2_g = - kf_co2 * grd_co2  ;
      #ifdef E_AIR
      double kc_co2  = valij.MoleFractionGas_carbondioxide ;
      double w_co2_g =   kc_co2 * w_g + j_co2_g  ;
      #else
      double w_co2_g =   j_co2_g  ;
      #endif
      
      /* Mass flux of water vapor */
      double grd_h2o = grdval.MassDensity_watervapor ;
      double kf_h2o  = valij.DiffusionCoefficient_watervapor ;
      double j_h2o_g = - kf_h2o * grd_h2o  ;
      #ifdef E_AIR
      double kc_h2o  = valij.MassFractionGas_watervapor ;
      double w_h2o_g =   kc_h2o * w_g + j_h2o_g  ;
      #else
      double w_h2o_g =   j_h2o_g  ;
      #endif
      
      /* Mass flux of dry air */
      double w_air   =   w_g - w_h2o_g - M_CO2 * w_co2_g ;
  
      vali.MassFlow_air[j]      =  w_air ;
      vali.MassFlow_total[j]   +=  w_h2o_g + M_CO2 * w_co2_g ;
      vali.MolarFlow_carbon[j] +=  w_co2_g ;
    }
  }
  
  {
    valj.MolarFlow_carbon[i]    = - vali.MolarFlow_carbon[j] ;
    valj.MolarFlow_calcium[i]   = - vali.MolarFlow_calcium[j]  ;
    valj.MolarFlow_sodium[i]    = - vali.MolarFlow_sodium[j]  ;
    valj.MolarFlow_silicon[i]   = - vali.MolarFlow_silicon[j]  ;
    valj.MolarFlow_charge[i]    = - vali.MolarFlow_charge[j]  ;
    valj.MolarFlow_potassium[i] = - vali.MolarFlow_potassium[j]  ;
    valj.MassFlow_total[i]      = - vali.MassFlow_total[j]  ;
    valj.MolarFlow_chlorine[i]  = - vali.MolarFlow_chlorine[j]  ;
    valj.MassFlow_air[i]        = - vali.MassFlow_air[j]  ;
  }
    
  return(val + i) ;
}






Values_d*  MPM_t::Initialize(Element_t* el,double const& t,Values_d& val)
{
  /* To retrieve the material properties */
  Parameters_t& par = ((Parameters_t*) Element_GetProperty(el))[0] ;
  
  double n_ch0            = par.InitialContent_portlandite;
  double n_csh0           = par.InitialContent_csh;
  double c_na0            = par.InitialConcentration_sodium;
  double c_k0             = par.InitialConcentration_potassium;
  
  double c_na_tot = c_na0 ;
  double c_k_tot  = c_k0 ;
  double c_na       = pow(10,val.U_sodium) ;
  double c_k        = pow(10,val.U_potassium) ;
  #ifdef E_CARBON
  double c_co2      = pow(10,val.U_carbon) ;
  #else
  double c_co2     = 1.e-99 ;
  #endif
  double u_calcium  = val.U_calcium ;
  #ifdef E_SILICON
  double u_silicon  = val.U_silicon ;
  #else
  double u_silicon  = 1 ;
  #endif
  #ifdef E_CHLORINE
  double c_cl       = pow(10,val.U_chlorine) ;
  #else
  double c_cl       = 1.e-99 ;
  #endif
  double logc_oh    = -7 ;
  double c_oh       = pow(10,logc_oh) ;
  HardenedCementChemistry_t<double>* hcc = hcc_d ;
      
  if(c_na_tot > 0 && c_k_tot > 0) {
    c_na   = c_na_tot ;
    c_k    = c_k_tot ;

    /* Compute the concentrations of alkalis Na and K */
    concentrations_oh_na_k(c_co2,u_calcium,u_silicon,c_cl,c_na_tot,c_k_tot) ;
  
    c_na = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,Na) ;
    c_k  = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,K) ;
        
    /* We modify the nodal unknowns */
    val.U_sodium = log10(c_na) ;
    val.U_potassium = log10(c_k) ;
        
  /* Solve cement chemistry */
  } else {
    double c_co2aq    = k_h*c_co2 ;
    double logc_co2aq = log10(c_co2aq) ;
    double logc_na    = log10(c_na) ;
    double logc_k     = log10(c_k) ;
    double logc_cl    = log10(c_cl) ;
    double psi        = 0 ;

    #if defined (U_ZN_Ca_S)
    {
      double si_ch_cc = Log10SaturationIndexOfCcH(u_calcium) ;
        
      HardenedCementChemistry_SetInput(hcc,SI_CH_CC,si_ch_cc) ;
    }
    #elif defined (U_LogS_CH)
    {
      double si_ch = Log10SaturationIndexOfCH(u_calcium) ;
          
      HardenedCementChemistry_SetInput(hcc,SI_CH,si_ch) ;
    }
    #endif
        
    {
      double si_csh = Log10SaturationIndexOfCSH(u_silicon) ;
          
      HardenedCementChemistry_SetInput(hcc,SI_CSH,si_csh) ;
    }
        
    HardenedCementChemistry_SetInput(hcc,LogC_CO2,logc_co2aq) ;
    HardenedCementChemistry_SetInput(hcc,LogC_Na,logc_na) ;
    HardenedCementChemistry_SetInput(hcc,LogC_K,logc_k) ;
    HardenedCementChemistry_SetInput(hcc,LogC_OH,logc_oh) ;
    HardenedCementChemistry_SetElectricPotential(hcc,psi) ;
    HardenedCementChemistry_SetInput(hcc,LogC_Cl,logc_cl) ;
  
    #ifdef E_CHLORINE
    HardenedCementChemistry_ComputeSystem(hcc,CaO_SiO2_Na2O_K2O_CO2_Cl_H2O) ;
    #else
    HardenedCementChemistry_SetAqueousConcentrationOf(hcc,Cl,c_cl) ;
    HardenedCementChemistry_SetLogAqueousConcentrationOf(hcc,Cl,logc_cl) ;
    HardenedCementChemistry_ComputeSystem(hcc,CaO_SiO2_Na2O_K2O_CO2_H2O) ;
    #endif

    {
      int k = HardenedCementChemistry_SolveElectroneutrality(hcc) ;
  
      if(k < 0) return(NULL) ;
    }
  }
      
  /* pH */
  {
    c_oh = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,OH) ;
    //double c_h  = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,H) ;
        
    val.Concentration_oh = c_oh ;
  }
      
  /* Solid contents */
  {
    double s_ch       = HardenedCementChemistry_GetSaturationIndexOf(hcc,CH) ;
    double s_cc       = HardenedCementChemistry_GetSaturationIndexOf(hcc,CC) ;
    double n_ch       = InitialCHSolidContent(u_calcium,s_ch,s_cc) ;
    double n_cc       = InitialCCSolidContent(u_calcium,s_ch,s_cc) ;
    double n_csh      = CSHSolidContent(u_silicon) ;
    double x_csh      = HardenedCementChemistry_GetCalciumSiliconRatioInCSH(hcc) ;
    double v_csh      = MolarVolumeOfCSH(x_csh) ;
    double v_s0       = V_CH*n_ch + V_CC*n_cc + v_csh*n_csh ;
        
    val.InitialVolume_solidtotal = v_s0 ;
    val.Mole_solidportlandite    = n_ch ;
    val.Mole_solidcalcite     = n_cc ;
    val.Mole_solidfriedelsalt = 0 ;
  }
  
  return(&val);
}
    
    
    



int concentrations_oh_na_k(double c_co2,double u_calcium,double u_silicon,double c_cl,double c_na_tot,double c_k_tot)
{
/* Solve a set of 3 equations:
 * 1. Electroneutralilty
 * 2. Mass balance of Na
 * 3. Mass balance of K
 * Unknowns: c_oh, c_na, c_k.
 * On input, c_na_tot and c_k_tot are the total contents of Na and K
 */
  
  /* Initialization */
  double c_na = c_na_tot ;
  double c_k  = c_k_tot ;
  double c_oh0 = c_na + c_k ;
  double c_oh = c_oh0 ;
  
  /* c_na_tot =  c_na * (A_Na + B_Na*c_oh + C_Na*c_oh*c_oh) */
  //double A_Na = 1 ;
  //double B_Na = k_naoh/k_e + k_nahco3*k_h*c_co2/k_1 ;
  //double C_Na = k_naco3*k_h*c_co2/(k_1*k_e) ;

  /* c_k_tot =  c_k * (A_K + B_K*c_oh) */
  //double A_K = 1 ;
  //double B_K = k_koh/k_e  ;
  
  double err,tol = 1.e-8 ;
  HardenedCementChemistry_t<double>* hcc = hcc_d ;
  

  /* Solve cement chemistry */
  {
    double c_co2aq    = k_h*c_co2 ;
    double logc_co2aq = log10(c_co2aq) ;
    double logc_cl    = log10(c_cl) ;

    #if defined (U_ZN_Ca_S)
    {
      double si_ch_cc = Log10SaturationIndexOfCcH(u_calcium) ;
          
      HardenedCementChemistry_SetInput(hcc,SI_CH_CC,si_ch_cc) ;
    }
    #elif defined (U_LogS_CH)
    {
      double si_ch = Log10SaturationIndexOfCH(u_calcium) ;
          
      HardenedCementChemistry_SetInput(hcc,SI_CH,si_ch) ;
    }
    #endif
        
    {
      double si_csh = Log10SaturationIndexOfCSH(u_silicon) ;
          
      HardenedCementChemistry_SetInput(hcc,SI_CSH,si_csh) ;
    }
  
    HardenedCementChemistry_SetInput(hcc,LogC_CO2,logc_co2aq) ;
    HardenedCementChemistry_SetInput(hcc,LogC_Cl,logc_cl) ;
  }
  
  int i = 0 ;
    
  
  do {
    double dc_oh = - c_oh ;
    double logc_na    = log10(c_na) ;
    double logc_k     = log10(c_k) ;
    double logc_cl    = log10(c_cl) ;
    
    HardenedCementChemistry_SetInput(hcc,LogC_Na,logc_na) ;
    HardenedCementChemistry_SetInput(hcc,LogC_K,logc_k) ;
    HardenedCementChemistry_SetInput(hcc,LogC_OH,-7) ;
  
#ifdef E_CHLORINE
    HardenedCementChemistry_ComputeSystem(hcc,CaO_SiO2_Na2O_K2O_CO2_Cl_H2O) ;
#else
    HardenedCementChemistry_SetAqueousConcentrationOf(hcc,Cl,c_cl) ;
    HardenedCementChemistry_SetLogAqueousConcentrationOf(hcc,Cl,logc_cl) ;
    HardenedCementChemistry_ComputeSystem(hcc,CaO_SiO2_Na2O_K2O_CO2_H2O) ;
#endif

    {
      int k = HardenedCementChemistry_SolveElectroneutrality(hcc) ;
      
      if(k < 0) return(-1) ;
    }
    
    {
      double c_na_l = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,Na) ;
      
      c_na *= c_na_tot/c_na_l ;
    }
    
    //c_na = c_na_tot/(A_Na + B_Na*c_oh + C_Na*c_oh*c_oh) ;
    
    {
      double c_k_l = HardenedCementChemistry_GetElementAqueousConcentrationOf(hcc,K) ;
      
      c_k *= c_k_tot/c_k_l ;
    }
    
    //c_k  = c_k_tot/(A_K + B_K*c_oh) ;
    
    c_oh = HardenedCementChemistry_GetAqueousConcentrationOf(hcc,OH) ;
    
    dc_oh += c_oh ;
    
    err = fabs(dc_oh/c_oh) ;
    
    if(i++ > 20) {
      printf("c_na_tot = %e\n",c_na_tot) ;
      printf("c_na     = %e\n",c_na) ;
      printf("c_k_tot  = %e\n",c_k_tot) ;
      printf("c_k      = %e\n",c_k) ;
      printf("c_oh0    = %e\n",c_oh0) ;;
      printf("c_oh     = %e\n",c_oh) ;
      Message_Direct("concentrations_oh_na_k: non convergence") ;
      return(-1) ;
    }

  } while(err > tol || c_oh < 0) ;
  
  /*
  {
    printf("\n") ;
    printf("c_oh = %e \n", c_oh) ;
    printf("c_na = %e \n", c_na) ;
    printf("c_k  = %e \n", c_k) ;
    printf("c_na(kcc) = %e \n", HardenedCementChemistry_GetAqueousConcentrationOf(hcc,Na)) ;
    printf("c_k(hcc)  = %e \n", HardenedCementChemistry_GetAqueousConcentrationOf(hcc,K)) ;
  }
  */
  
  return(0) ;
}

template<typename T>
double PermeabilityCoefficient_KozenyCarman(Element_t const* el,T const phi)
/* Kozeny-Carman model */
{
  /* To retrieve the material properties */
  Parameters_t& par = ((Parameters_t*) Element_GetProperty(el))[0] ;
  double phi0             = par.InitialPorosity;
  T coeff_permeability ;
  
  {
    T kozeny_carman  = (phi > 0) ? pow(phi/phi0,3.)*pow(((1 - phi0)/(1 - phi)),2.) : 0 ;

    coeff_permeability = kozeny_carman ;
  }
  
  return(coeff_permeability) ;
}


template<typename T>
T PermeabilityCoefficient_VermaPruess(Element_t const* el,T const phi)
/* Ref:
 * A. Verma and K. Pruess,
 * Thermohydrological Conditions and Silica Redistribution Near High-Level
 * Nuclear Wastes Emplaced in Saturated Geological Formations,
 * Journal of Geophysical Research, 93(B2) 1159-1173, 1988
 * frac  = fractionnal length of pore bodies (0.8) 
 * phi_r = fraction of initial porosity (phi/phi0) at which permeability is 0 
 */
{
  /* To retrieve the material properties */
  Parameters_t& par = ((Parameters_t*) Element_GetProperty(el))[0] ;
  double phi0             = par.InitialPorosity;
  double frac             = par.FractionalLengthOfPoreBodies;
  double phi_r            = par.PorosityFractionAtVanishingPermeability;
  T coeff_permeability ;
  
  {
    T phi_c = phi0 * phi_r ;
    T w = 1 + (phi_r/frac)/(1 - phi_r) ;
    T t = (phi - phi_c)/(phi0 - phi_c) ;
    T verma_pruess = (t > 0) ? t*t*(1 - frac + (frac/(w*w)))/(1 - frac + frac*(pow(t/(t + w - 1),2.))) : 0 ;

    coeff_permeability = verma_pruess ;
  }
  
  return(coeff_permeability) ;
}



template<typename T>
T TortuosityToLiquid_OhJang(T const phi,T const s_l)
/* Ref:
 * Byung Hwan Oh, Seung Yup Jang, 
 * Prediction of diffusivity of concrete based on simple analytic equations, 
 * Cement and Concrete Research 34 (2004) 463 - 480.
 * 
 * tau = tau_paste * tau_agg
 * 
 * tau_agg   = ratio of diffusivity of concrete and diffusivity of matrix (cement paste)
 * tau_paste = ratio of diffusivity of cement paste and diffusivity of liquid bulk
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
  T phi_cap = (phi > 0) ? 0.5 * phi : 0  ;
  T phi_c = 0.18 ;          /* Critical porosity */
  T n     = 2.7 ;           /* n  = 2.7   (OPC) ; 4.5  (OPC + 10% Silica fume) */
  T ds    = 2.e-4 ;         /* ds = 2.e-4 (OPC) ; 5e-5 (OPC + 10% Silica fume) */
  T dsn   = pow(ds,1/n) ;
  T m_phi = 0.5 * ((phi_cap - phi_c) + dsn * (1 - phi_c - phi_cap)) / (1 - phi_c) ;
  T tau_paste = pow(m_phi + sqrt(m_phi*m_phi + dsn * phi_c/(1 - phi_c)),n) ;
  T tau_agg = 0.27 ;
  T tausat =  tau_paste * tau_agg ;
  
  T tau =  tausat * pow(s_l,4.5) ;
    
  return(tau) ;
}



template<typename T>
T TortuosityToLiquid_BazantNajjar(T const phi,T const s_l)
/* Ref:
 * Z. P. BAZANT, L.J. NAJJAR,
 * Nonlinear water diffusion in nonsaturated concrete,
 * Materiaux et constructions, 5(25), 1972.
 */
{
  T iff = 0.00029 * exp(9.95 * phi) ;
  T tausat = (iff < 1) ? iff : 1 ;
  T tau    = tausat / (1 + 625*pow((1 - s_l),4)) ;
    
  return(tau) ;
}


template<typename T>
T TortuosityToLiquid_Xie(T const phi,T const s_l)
{
  T vca = 0.392 ;
  T vfa = 0.284 ;
  T vpa = 0.324  ;
  T fac = (1.-2.1*vca)/(1.-0.65*vfa)*vpa ;
  T hydrationdegree = 0.95 ;
  T wcratio = 0.5 ;
  T phiused = phi/vpa - 0.19*hydrationdegree/(wcratio + 0.32);
  T fporo = (phiused > 0.18) ? 0.001+0.07*phiused*phiused+1.8*(phiused-0.18)*(phiused-0.18) : 0.001+0.07*phiused*phiused ;
  T coefsa = pow(s_l,6.) ; 
  T iff = fac * fporo * coefsa ;
      
  return(iff) ;
}


template<typename T>
T TortuosityToGas(T const phi,T const s_l)
{
  T s_g    = 1 - s_l ;
  T tausat = (phi > 0) ? pow(phi,1.74) : 0 ;
  T tau    = (s_g > 0) ? tausat * pow(s_g,3.20) : 0 ;

  return(tau) ;
}



#if 0
double CHSolidContent_kin1(double const n_chn,double const s_ch,double const dt)
{
  double av         = 1 - n_chn/n_ch0 ;
  double dn1sdt     = a_2*dn1_caoh2sdt(av,c_2) ;
  double dn_chsdt   = dn1sdt*log(s_ch) ; /* Kinetics */
  double n_ch_ki    = MAX(n_chn + dt*dn_chsdt , 0.) ;
  
  return(n_ch_ki) ;
}
#endif


template<typename T>
T dn1_caoh2sdt(T const av0,T const c)
{
  T av = ((av0 > 0) ? ((av0 < 1) ? av0 : 1) : 0) ; /* av = 1 - n_ch/n_ch0 */
  T rp = (av < 1) ? pow(1 - av,1./3) : 0 ; /* rp = Rp/R0 */
  T rc = pow(1 - av + V_CC/V_CH*av,1./3) ; /* rc = Rc/R0 */
  T width = rc - rp ;
  T dn1dt = (rc > 0.) ? rp*rp/(1 + c*width*rp/rc) : 0 ;
  
  return(dn1dt) ;
}


template<typename T>
T saturationdegree(T const pc,T const pc3,Curve_t const* curve)
/* Saturation degree: regularization around 1 */
{
  T* x = Curve_GetXRange(curve) ;
  T* y = Curve_GetYValue(curve) ;
  T pc1 = x[0] ;
  T sl ;
  
  if(pc >= pc3 || pc1 >= pc3) {
    sl = Curve_ComputeValue(curve,pc) ;
  } else {
    T sl1 = y[0] ;
    T sl3 = Curve_ComputeValue(curve,pc3) ;
    
    sl  = sl1 - (sl1 - sl3) * exp(-(pc - pc3)/(pc1 - pc3)) ;
  }
  
  return(sl) ;
}
