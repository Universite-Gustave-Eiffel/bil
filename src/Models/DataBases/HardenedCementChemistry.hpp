#ifndef HARDENEDCEMENTCHEMISTRY_HPP
#define HARDENEDCEMENTCHEMISTRY_HPP


/* Forward declarations */
//struct HardenedCementChemistry_t; typedef struct HardenedCementChemistry_t     HardenedCementChemistry_t ;
template<typename T> struct HardenedCementChemistry_t;
template<typename T> struct CementSolutionChemistry_t;
struct Curves_t;
struct Curve_t;


/* Synonyms */
#define HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_CO2 \
        HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_CO2_H2O
        
#define HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3 \
        HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_H2O
        
#define HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_2 \
        HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_H2O_2
        
#define HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_Al2O3 \
        HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_Al2O3_H2O
        
#define HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_Al2O3_2 \
        HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_Al2O3_H2O_2
        


/* Copying */
#define HardenedCementChemistry_CopyConcentrations(HCC,v)  \
        (CementSolutionChemistry_CopyConcentrations(HardenedCementChemistry_GetCementSolutionChemistry(HCC),v))

#define HardenedCementChemistry_CopyLogConcentrations(HCC,v)  \
        (CementSolutionChemistry_CopyLogConcentrations(HardenedCementChemistry_GetCementSolutionChemistry(HCC),v))

#define HardenedCementChemistry_CopyChemicalPotential(HCC,v)  \
        (CementSolutionChemistry_CopyChemicalPotential(HardenedCementChemistry_GetCementSolutionChemistry(HCC),v))


/* Macros for the temperature
 * --------------------------*/
#define HardenedCementChemistry_GetRoomTemperature(HCC) \
        CementSolutionChemistry_GetRoomTemperature(HardenedCementChemistry_GetCementSolutionChemistry(HCC))

       
#define HardenedCementChemistry_SetRoomTemperature(HCC,T) \
        CementSolutionChemistry_SetRoomTemperature(HardenedCementChemistry_GetCementSolutionChemistry(HCC),T)


/* The getters for attributes */
#define HardenedCementChemistry_GetPrimaryVariableIndex(HCC) \
        ((HCC)->primaryvariableindex)
        
#define HardenedCementChemistry_GetPrimaryVariable(HCC) \
        ((HCC)->primaryvariable)
        
#define HardenedCementChemistry_GetVariable(HCC) \
        ((HCC)->variable)
        
#define HardenedCementChemistry_GetSaturationIndex(HCC) \
        ((HCC)->saturationindex)
        
#define HardenedCementChemistry_GetConstant(HCC) \
        ((HCC)->constant)
        
#define HardenedCementChemistry_GetLog10Ksp(HCC) \
        ((HCC)->log10solubilityproductconstant)
        
#define HardenedCementChemistry_GetCSHCurves(HCC) \
        ((HCC)->cshcurves)
        
#define HardenedCementChemistry_GetCementSolutionChemistry(HCC) \
        ((HCC)->csc)
        
#define HardenedCementChemistry_GetLog10SaturationIndex(HCC) \
        ((HCC)->log10saturationindex)
        
#define HardenedCementChemistry_GetCurveOfCalciumSiliconRatioInCSH(HCC) \
        ((HCC)->curveofcalciumsiliconratioincsh)
        
#define HardenedCementChemistry_GetCurveOfWaterSiliconRatioInCSH(HCC) \
        ((HCC)->curveofwatersiliconratioincsh)
        
#define HardenedCementChemistry_GetCurveOfSaturationIndexOfSH(HCC) \
        ((HCC)->curveofsaturationindexofsh)
        
        
        
#define HardenedCementChemistry_SetPrimaryVariableIndex(HCC,A) \
        do {\
          HardenedCementChemistry_GetPrimaryVariableIndex(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetPrimaryVariable(HCC,A) \
        do {\
          HardenedCementChemistry_GetPrimaryVariable(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetVariable(HCC,A) \
        do {\
          HardenedCementChemistry_GetVariable(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetSaturationIndex(HCC,A) \
        do {\
          HardenedCementChemistry_GetSaturationIndex(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetConstant(HCC,A) \
        do {\
          HardenedCementChemistry_GetConstant(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetLog10Ksp(HCC,A) \
        do {\
          HardenedCementChemistry_GetLog10Ksp(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetCSHCurves(HCC,A) \
        do {\
          HardenedCementChemistry_GetCSHCurves(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetCementSolutionChemistry(HCC,A) \
        do {\
          HardenedCementChemistry_GetCementSolutionChemistry(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetLog10SaturationIndex(HCC,A) \
        do {\
          HardenedCementChemistry_GetLog10SaturationIndex(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetCurveOfCalciumSiliconRatioInCSH(HCC,A) \
        do {\
          HardenedCementChemistry_GetCurveOfCalciumSiliconRatioInCSH(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetCurveOfWaterSiliconRatioInCSH(HCC,A) \
        do {\
          HardenedCementChemistry_GetCurveOfWaterSiliconRatioInCSH(HCC) = A;\
        } while(0)
        
#define HardenedCementChemistry_SetCurveOfSaturationIndexOfSH(HCC,A) \
        do {\
          HardenedCementChemistry_GetCurveOfSaturationIndexOfSH(HCC) = A;\
        } while(0)



/* Macro for the electric potential
 * --------------------------------*/
#define HardenedCementChemistry_GetElectricPotential(HCC) \
        (CementSolutionChemistry_GetElectricPotential(HardenedCementChemistry_GetCementSolutionChemistry(HCC)))
        
#define HardenedCementChemistry_SetElectricPotential(HCC,A) \
        do {\
          HardenedCementChemistry_GetElectricPotential(HCC) = A;\
        } while(0)




/* Indexes for the primary variables
 * ---------------------------------*/
#define HardenedCementChemistry_NbOfPrimaryVariables  (15)


/* Different primary variables may be used */
#define HardenedCementChemistry_P_H2O          (0)
#define HardenedCementChemistry_P_LogA_H2O     HardenedCementChemistry_P_H2O

#define HardenedCementChemistry_P_CaO          (1)
#define HardenedCementChemistry_P_SI_CH        HardenedCementChemistry_P_CaO
#define HardenedCementChemistry_P_SI_CH_CC \
        (HardenedCementChemistry_P_CaO + HardenedCementChemistry_NbOfPrimaryVariables)
#define HardenedCementChemistry_P_SI_CH_Cc \
        (HardenedCementChemistry_P_CaO + HardenedCementChemistry_NbOfPrimaryVariables)
#define HardenedCementChemistry_P_SI_CH_CSH2 \
        (HardenedCementChemistry_P_CaO + 2*HardenedCementChemistry_NbOfPrimaryVariables)
#define HardenedCementChemistry_P_SI_CH_CsH2 \
        (HardenedCementChemistry_P_CaO + 2*HardenedCementChemistry_NbOfPrimaryVariables)

#define HardenedCementChemistry_P_SiO2         (2)
#define HardenedCementChemistry_P_SI_CSH       HardenedCementChemistry_P_SiO2

#define HardenedCementChemistry_P_Al2O3        (3)
#define HardenedCementChemistry_P_SI_AH3       HardenedCementChemistry_P_Al2O3

#define HardenedCementChemistry_P_Na2O         (4)
#define HardenedCementChemistry_P_LogA_Na      HardenedCementChemistry_P_Na2O
#define HardenedCementChemistry_P_LogC_Na \
        (HardenedCementChemistry_P_Na2O + HardenedCementChemistry_NbOfPrimaryVariables)

#define HardenedCementChemistry_P_K2O          (5)
#define HardenedCementChemistry_P_LogA_K       HardenedCementChemistry_P_K2O
#define HardenedCementChemistry_P_LogC_K \
        (HardenedCementChemistry_P_K2O + HardenedCementChemistry_NbOfPrimaryVariables)

#define HardenedCementChemistry_P_CO2          (6)
#define HardenedCementChemistry_P_LogA_CO2     HardenedCementChemistry_P_CO2
#define HardenedCementChemistry_P_LogC_CO2  \
        (HardenedCementChemistry_P_CO2 + HardenedCementChemistry_NbOfPrimaryVariables)

#define HardenedCementChemistry_P_SO3          (7)
#define HardenedCementChemistry_P_LogA_H2SO4   HardenedCementChemistry_P_SO3
#define HardenedCementChemistry_P_LogC_H2SO4  \
        (HardenedCementChemistry_P_SO3 + HardenedCementChemistry_NbOfPrimaryVariables)
#define HardenedCementChemistry_P_LogA_SO4 \
        (HardenedCementChemistry_P_SO3 + 2*HardenedCementChemistry_NbOfPrimaryVariables)
#define HardenedCementChemistry_P_LogC_SO4 \
        (HardenedCementChemistry_P_SO3 + 3*HardenedCementChemistry_NbOfPrimaryVariables)

#define HardenedCementChemistry_P_Cl           (8)
#define HardenedCementChemistry_P_LogA_Cl      HardenedCementChemistry_P_Cl
#define HardenedCementChemistry_P_LogC_Cl  \
        (HardenedCementChemistry_P_Cl + HardenedCementChemistry_NbOfPrimaryVariables)

#define HardenedCementChemistry_P_Fe2O3        (9)
#define HardenedCementChemistry_P_LogA_Fe      HardenedCementChemistry_P_Fe2O3
#define HardenedCementChemistry_P_LogC_Fe \
        (HardenedCementChemistry_P_Fe2O3 + HardenedCementChemistry_NbOfPrimaryVariables)

#define HardenedCementChemistry_P_MgO          (10)
#define HardenedCementChemistry_P_LogA_Mg      HardenedCementChemistry_P_MgO
#define HardenedCementChemistry_P_LogC_Mg \
        (HardenedCementChemistry_P_MgO + HardenedCementChemistry_NbOfPrimaryVariables)

#define HardenedCementChemistry_P_TiO2         (11)
#define HardenedCementChemistry_P_LogA_Ti      HardenedCementChemistry_P_TiO2
#define HardenedCementChemistry_P_LogC_Ti \
        (HardenedCementChemistry_P_TiO2 + HardenedCementChemistry_NbOfPrimaryVariables)

#define HardenedCementChemistry_P_P2O5         (12)
#define HardenedCementChemistry_P_LogA_P       HardenedCementChemistry_P_P2O5
#define HardenedCementChemistry_P_LogC_P \
        (HardenedCementChemistry_P_P2O5 + HardenedCementChemistry_NbOfPrimaryVariables)

#define HardenedCementChemistry_P_SrO          (13)

#define HardenedCementChemistry_P_H            (14)
#define HardenedCementChemistry_P_LogA_OH      HardenedCementChemistry_P_H
#define HardenedCementChemistry_P_LogC_OH   \
        (HardenedCementChemistry_P_H + HardenedCementChemistry_NbOfPrimaryVariables)
#define HardenedCementChemistry_P_LogC_H   \
        (HardenedCementChemistry_P_H + 2*HardenedCementChemistry_NbOfPrimaryVariables)



/* Inputs: definition
 * ------------------ */
#define HardenedCementChemistry_GetInput(HCC,U) \
        HardenedCementChemistry_GetPrimaryVariable(HCC)[HardenedCementChemistry_InputIndex(U)]
        
#define HardenedCementChemistry_SetInput(HCC,U,...) \
        do{ \
          HardenedCementChemistry_GetIndex(HCC,U) = HardenedCementChemistry_Index(U) ; \
          HardenedCementChemistry_GetInput(HCC,U) = __VA_ARGS__ ; \
        } while(0)
        
#define HardenedCementChemistry_InputCaOIs(HCC,V) \
        HardenedCementChemistry_InputIs(HCC,CaO,V)
        
#define HardenedCementChemistry_InputSO3Is(HCC,V) \
        HardenedCementChemistry_InputIs(HCC,SO3,V)
        
/* Inputs: implementation
 * ---------------------- */
#define HardenedCementChemistry_Index(U) \
        (HardenedCementChemistry_P_##U)

#define HardenedCementChemistry_InputIndex(U) \
        (HardenedCementChemistry_Index(U) % HardenedCementChemistry_NbOfPrimaryVariables)
        
#define HardenedCementChemistry_GetIndex(HCC,U) \
        HardenedCementChemistry_GetPrimaryVariableIndex(HCC)[HardenedCementChemistry_InputIndex(U)]
        
#define HardenedCementChemistry_InputIs(HCC,U,V) \
        (HardenedCementChemistry_GetIndex(HCC,U) == HardenedCementChemistry_Index(V))




/* Macros for the type of hydrates
 * -------------------------------*/
#define HardenedCementChemistry_NbOfSolidHydrates  (13)

#define HardenedCementChemistry_CH          (0)
#define HardenedCementChemistry_Portlandite (0)
#define HardenedCementChemistry_SH          (1)
#define HardenedCementChemistry_AmSilica    (1)
#define HardenedCementChemistry_CC          (2)
#define HardenedCementChemistry_Cc          (2)
#define HardenedCementChemistry_Calcite     (2)
#define HardenedCementChemistry_CSH2        (3)
#define HardenedCementChemistry_CsH2        (3)
#define HardenedCementChemistry_Gypsum      (3)
#define HardenedCementChemistry_AH3         (4)
#define HardenedCementChemistry_AFm         (5)
#define HardenedCementChemistry_AFt         (6)
#define HardenedCementChemistry_C3AH6       (7)
#define HardenedCementChemistry_C2AH8       (8)
#define HardenedCementChemistry_CAH10       (9)
#define HardenedCementChemistry_FriedelSalt (10)
#define HardenedCementChemistry_KuzelSalt   (11)
#define HardenedCementChemistry_Aragonite   (12)


/* Macros for saturation indexes
 * -----------------------------*/
#define HardenedCementChemistry_NbOfSaturationIndexes  HardenedCementChemistry_NbOfSolidHydrates

#define HardenedCementChemistry_GetSaturationIndexOf(HCC,A) \
        (HardenedCementChemistry_GetSaturationIndex(HCC)[HardenedCementChemistry_##A])
        
#define HardenedCementChemistry_SetSaturationIndexOf(HCC,A,B) \
        do {\
          HardenedCementChemistry_GetSaturationIndexOf(HCC,A) = B;\
        } while(0)

#define HardenedCementChemistry_GetLog10SaturationIndexOf(HCC,A) \
        (HardenedCementChemistry_GetLog10SaturationIndex(HCC)[HardenedCementChemistry_##A])
        
#define HardenedCementChemistry_SetLog10SaturationIndexOf(HCC,A,B) \
        do {\
          HardenedCementChemistry_GetLog10SaturationIndexOf(HCC,A) = B;\
        } while(0)





/* Macros for variables
 * --------------------*/
#define HardenedCementChemistry_NbOfVariables  (3)

#define HardenedCementChemistry_X_CSH         (0)
#define HardenedCementChemistry_Z_CSH         (1)
//#define HardenedCementChemistry_V_CSH         (2)

/* Ca/Si ratio in C-S-H */
#define HardenedCementChemistry_GetCalciumSiliconRatioInCSH(HCC) \
        (HardenedCementChemistry_GetVariable(HCC)[HardenedCementChemistry_X_CSH])
        
#define HardenedCementChemistry_SetCalciumSiliconRatioInCSH(HCC,A) \
        do {\
          HardenedCementChemistry_GetCalciumSiliconRatioInCSH(HCC) = A;\
        } while(0)

/* Water/Si ratio in C-S-H */
#define HardenedCementChemistry_GetWaterSiliconRatioInCSH(HCC) \
        (HardenedCementChemistry_GetVariable(HCC)[HardenedCementChemistry_Z_CSH])
        
#define HardenedCementChemistry_SetWaterSiliconRatioInCSH(HCC,A) \
        do {\
          HardenedCementChemistry_GetWaterSiliconRatioInCSH(HCC) = A;\
        } while(0)

/* Molar volume of C-S-H */
/*
#define HardenedCementChemistry_GetMolarVolumeOfCSH(HCC) \
       (HardenedCementChemistry_GetVariable(HCC)[HardenedCementChemistry_V_CSH])
*/



/* Macros for Aqueous concentrations
 * ---------------------------------*/
#define HardenedCementChemistry_NbOfSpecies \
        CementSolutionChemistry_NbOfSpecies
        
#define HardenedCementChemistry_GetAqueousConcentration(HCC) \
        CementSolutionChemistry_GetConcentration(HardenedCementChemistry_GetCementSolutionChemistry(HCC))

#define HardenedCementChemistry_GetLogAqueousConcentration(HCC) \
        CementSolutionChemistry_GetLogConcentration(HardenedCementChemistry_GetCementSolutionChemistry(HCC))
        
#define HardenedCementChemistry_GetAqueousConcentrationOf(HCC,CPD) \
        CementSolutionChemistry_GetConcentrationOf(HardenedCementChemistry_GetCementSolutionChemistry(HCC),CPD)
        
#define HardenedCementChemistry_SetAqueousConcentrationOf(HCC,CPD,A) \
        do {\
          HardenedCementChemistry_GetAqueousConcentrationOf(HCC,CPD) = A;\
        } while(0)

#define HardenedCementChemistry_GetLogAqueousConcentrationOf(HCC,CPD) \
        CementSolutionChemistry_GetLogConcentrationOf(HardenedCementChemistry_GetCementSolutionChemistry(HCC),CPD)
        
#define HardenedCementChemistry_SetLogAqueousConcentrationOf(HCC,CPD,A) \
        do {\
          HardenedCementChemistry_GetLogAqueousConcentrationOf(HCC,CPD) = A;\
        } while(0)
        

/* Synonyms */
#define HardenedCementChemistry_NbOfConcentrations \
        HardenedCementChemistry_NbOfSpecies


/* Other macros
 * ------------*/
/* Liquid mass density */
#define HardenedCementChemistry_GetLiquidMassDensity(HCC) \
        CementSolutionChemistry_GetLiquidMassDensity(HardenedCementChemistry_GetCementSolutionChemistry(HCC))

/* Liquid charge density */
#define HardenedCementChemistry_GetLiquidChargeDensity(HCC) \
        CementSolutionChemistry_GetChargeDensity(HardenedCementChemistry_GetCementSolutionChemistry(HCC))

/* Ionic strength of aqueous phase*/
#define HardenedCementChemistry_GetIonicStrength(HCC) \
        CementSolutionChemistry_GetIonicStrength(HardenedCementChemistry_GetCementSolutionChemistry(HCC))

/* Element aqueous concentrations */
#define HardenedCementChemistry_GetElementAqueousConcentration(HCC) \
        CementSolutionChemistry_GetElementConcentration(HardenedCementChemistry_GetCementSolutionChemistry(HCC))
        
#define HardenedCementChemistry_GetElementAqueousConcentrationOf(HCC,A) \
        CementSolutionChemistry_GetElementConcentrationOf(HardenedCementChemistry_GetCementSolutionChemistry(HCC),A)






/* Macros for constants
 * --------------------*/
#define HardenedCementChemistry_NbOfConstants (2)

#define HardenedCementChemistry_A_CO2_EQ      (0)
#define HardenedCementChemistry_A_H2SO4_EQ    (1)


/* Equilibrium CO2 concentration */
#define HardenedCementChemistry_GetLog10EquilibriumCO2Activity(HCC) \
        (HardenedCementChemistry_GetConstant(HCC)[HardenedCementChemistry_A_CO2_EQ])
        
#define HardenedCementChemistry_SetLog10EquilibriumCO2Activity(HCC,A) \
        do {\
          HardenedCementChemistry_GetLog10EquilibriumCO2Activity(HCC) = A;\
        } while(0)

/* Equilibrium H2SO4 concentration */
#define HardenedCementChemistry_GetLog10EquilibriumH2SO4Activity(HCC) \
        (HardenedCementChemistry_GetConstant(HCC)[HardenedCementChemistry_A_H2SO4_EQ])
        
#define HardenedCementChemistry_SetLog10EquilibriumH2SO4Activity(HCC,A) \
        do {\
          HardenedCementChemistry_GetLog10EquilibriumH2SO4Activity(HCC) = A;\
        } while(0)




/* Macros for solubility product constants
 * ---------------------------------------*/
#define HardenedCementChemistry_NbOfSolubilityProductConstants HardenedCementChemistry_NbOfSolidHydrates


#define HardenedCementChemistry_GetLog10SolubilityProductConstantOf(HCC,CPD) \
        (HardenedCementChemistry_GetLog10Ksp(HCC)[HardenedCementChemistry_##CPD])
        
#define HardenedCementChemistry_SetLog10SolubilityProductConstantOf(HCC,CPD,A) \
        do {\
          HardenedCementChemistry_GetLog10SolubilityProductConstantOf(HCC,CPD) = A;\
        } while(0)



/* Macros for the resolution of the systems
 * ----------------------------------------*/
#define HardenedCementChemistry_ComputeSystem(HCC,SYS) \
        (HardenedCementChemistry_ComputeSystem_##SYS(HCC))

#define HardenedCementChemistry_SolveElectroneutrality(HCC) \
        (CementSolutionChemistry_SolveElectroneutrality(HardenedCementChemistry_GetCementSolutionChemistry(HCC)))

#define HardenedCementChemistry_SolveExplicitElectroneutrality(HCC) \
        (CementSolutionChemistry_SolveExplicitElectroneutrality(HardenedCementChemistry_GetCementSolutionChemistry(HCC)))



/* Macros for the CSH curves */
#define HardenedCementChemistry_GetCSHCurve(HCC) \
        (Curves_GetCurve(HardenedCementChemistry_GetCSHCurves(HCC)))

#define HardenedCementChemistry_SetDefaultCurveOfCalciumSiliconRatioInCSH(HCC) \
        do { \
          HardenedCementChemistry_GetCurveOfCalciumSiliconRatioInCSH(HCC) = HardenedCementChemistry_GetCSHCurve(HCC) + (0) ; \
        } while(0) 

#define HardenedCementChemistry_SetDefaultCurveOfWaterSiliconRatioInCSH(HCC) \
        do { \
          HardenedCementChemistry_GetCurveOfWaterSiliconRatioInCSH(HCC) = HardenedCementChemistry_GetCSHCurve(HCC) + (1) ; \
        } while(0) 

#define HardenedCementChemistry_SetDefaultCurveOfSaturationIndexOfSH(HCC) \
        do { \
          HardenedCementChemistry_GetCurveOfSaturationIndexOfSH(HCC) = HardenedCementChemistry_GetCSHCurve(HCC) + (2) ; \
        } while(0) 



template<typename T>
struct HardenedCementChemistry_t {
//  int nbofprimaryvariables ;
//  int nbofvariables ;
  int*    primaryvariableindex ;
  T* primaryvariable ;
  double* constant ;
  T* variable ;
  T* saturationindex ;
  T* log10saturationindex ;
  double* log10solubilityproductconstant ;
  CementSolutionChemistry_t<T>* csc ;
  Curves_t* cshcurves ;
  Curve_t*  curveofcalciumsiliconratioincsh ;
  Curve_t*  curveofwatersiliconratioincsh ;
  Curve_t*  curveofsaturationindexofsh ;
  
  #if 0
  public:
  void SetPrimaryVariableIndex(int* x){_primaryvariableindex = x;}
  void SetPrimaryVariable(T* x){_primaryvariable = x;}
  void SetVariable(T* x){_variable = x;}
  void SetConstant(T* x){_constant = x;}
  void SetSaturationIndex(T* x){_saturationindex = x;}
  void SetLog10SaturationIndex(T* x){_log10saturationindex = x;}
  void SetLog10Ksp(double* x){_log10solubilityproductconstant = x;}
  void SetCementSolutionChemistry(CementSolutionChemistry_t<T>* x){_csc = x;}
  void SetCSHCurves(Curves_t* x){return(_cshcurves);}
  void SetCurveOfCalciumSiliconRatioInCSH(Curve_t* x){_curveofcalciumsiliconratioincsh = x;}
  void SetCurveOfWaterSiliconRatioInCSH(Curve_t* x){_curveofwatersiliconratioincsh = x;}
  void SetCurveOfSaturationIndexOfSH(Curve_t* x){_curveofsaturationindexofsh = x;}
  
  /* Accessors */
  int* GetPrimaryVariableIndex(void){return(_primaryvariableindex);}
  T* GetPrimaryVariable(void){return(_primaryvariable);}
  T* GetVariable(void){return(_variable);}
  T* GetConstant(void){return(_constant);}
  T* GetSaturationIndex(void){return(_saturationindex);}
  T* GetLog10SaturationIndex(void){return(_log10saturationindex);}
  double* GetLog10Ksp(void){return(_log10solubilityproductconstant);}
  CementSolutionChemistry_t<T>* GetCementSolutionChemistry(void){return(_csc);}
  Curves_t* GetCSHCurves(void){return(_cshcurves);}
  Curve_t*  GetCurveOfCalciumSiliconRatioInCSH(void){return(_curveofcalciumsiliconratioincsh);}
  Curve_t*  GetCurveOfWaterSiliconRatioInCSH(void){return(_curveofwatersiliconratioincsh);}
  Curve_t*  GetCurveOfSaturationIndexOfSH(void){return(_curveofsaturationindexofsh);}
  #endif
} ;


template<typename T>
inline void (HardenedCementChemistry_AllocateMemory)(HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_UpdateChemicalConstants)(HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_CO2_H2O_0)      (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_H2O_0)      (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_Al2O3_H2O_0)(HardenedCementChemistry_t<T>*) ;
template<typename T>
inline HardenedCementChemistry_t<T>* (HardenedCementChemistry_Create)(void) ;
template<typename T>
inline void (HardenedCementChemistry_Delete)(HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_H2O)            (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_Al2O3_H2O)      (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_Cl_H2O)         (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_Al2O3_Cl_H2O)   (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_CO2_H2O)        (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_CO2_Cl_H2O)     (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_Al2O3_CO2_H2O)  (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_Al2O3_CO2_Cl_H2O)(HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_H2O)        (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_Al2O3_H2O)  (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_Al2O3_Cl_H2O)(HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_PrintChemicalConstants)(HardenedCementChemistry_t<T>*) ;



#include "CementSolutionChemistry.h"
#include "Curves.h"


#include "HardenedCementChemistry.h.in"

#endif
