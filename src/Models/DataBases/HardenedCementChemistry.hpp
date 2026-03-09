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

#define HardenedCementChemistry_CopyChemicalPotential(HCC,v)  \
        (CementSolutionChemistry_CopyChemicalPotential(HardenedCementChemistry_GetCementSolutionChemistry(HCC),v))


/* Macros for the temperature
 * --------------------------*/
#define HardenedCementChemistry_GetRoomTemperature(HCC) \
        CementSolutionChemistry_GetRoomTemperature(HardenedCementChemistry_GetCementSolutionChemistry(HCC))

#define HardenedCementChemistry_SetRoomTemperature(HCC,T) \
        do {\
          CementSolutionChemistry_SetRoomTemperature(HardenedCementChemistry_GetCementSolutionChemistry(HCC),T);\
          HardenedCementChemistry_UpdateChemicalConstantsCEMDATA(HCC);\
        } while(0)


/* The getters */
#define HardenedCementChemistry_GetPrimaryVariableIndex(HCC) \
        ((HCC)->GetPrimaryVariableIndex())
        
#define HardenedCementChemistry_GetPrimaryVariable(HCC) \
        ((HCC)->GetPrimaryVariable())
        
#define HardenedCementChemistry_GetVariable(HCC) \
        ((HCC)->GetVariable())
        
#define HardenedCementChemistry_GetSaturationIndex(HCC) \
        ((HCC)->GetSaturationIndex())
        
#define HardenedCementChemistry_GetConstant(HCC) \
        ((HCC)->GetConstant())
        
#define HardenedCementChemistry_GetLog10SolubilityProductConstant(HCC) \
        ((HCC)->GetLog10SolubilityProductConstant())
        
#define HardenedCementChemistry_GetCSHCurves(HCC) \
        ((HCC)->GetCSHCurves())
        
#define HardenedCementChemistry_GetCementSolutionChemistry(HCC) \
        ((HCC)->GetCementSolutionChemistry())
        
#define HardenedCementChemistry_GetCurveOfCalciumSiliconRatioInCSH(HCC) \
        ((HCC)->GetCurveOfCalciumSiliconRatioInCSH())
        
#define HardenedCementChemistry_GetCurveOfWaterSiliconRatioInCSH(HCC) \
        ((HCC)->GetCurveOfWaterSiliconRatioInCSH())
        
#define HardenedCementChemistry_GetCurveOfSaturationIndexOfSH(HCC) \
        ((HCC)->GetCurveOfSaturationIndexOfSH())
        
#define HardenedCementChemistry_GetSolidProductName(HCC) \
        ((HCC)->GetSolidProductName())
        

/* The setters */
#define HardenedCementChemistry_SetPrimaryVariableIndex(HCC,A) \
        (HCC)->SetPrimaryVariableIndex(A)
        
#define HardenedCementChemistry_SetPrimaryVariable(HCC,A) \
        (HCC)->SetPrimaryVariable(A)
        
#define HardenedCementChemistry_SetVariable(HCC,A) \
        (HCC)->SetVariable(A)
        
#define HardenedCementChemistry_SetSaturationIndex(HCC,A) \
        (HCC)->SetSaturationIndex(A)
        
#define HardenedCementChemistry_SetConstant(HCC,A) \
        (HCC)->SetConstant(A)
        
#define HardenedCementChemistry_SetLog10SolubilityProductConstant(HCC,A) \
        (HCC)->SetLog10SolubilityProductConstant(A)
        
#define HardenedCementChemistry_SetCSHCurves(HCC,A) \
        (HCC)->SetCSHCurves(A)
        
#define HardenedCementChemistry_SetCementSolutionChemistry(HCC,A) \
        (HCC)->SetCementSolutionChemistry(A)
        
#define HardenedCementChemistry_SetCurveOfCalciumSiliconRatioInCSH(HCC,A) \
        (HCC)->SetCurveOfCalciumSiliconRatioInCSH(A)
        
#define HardenedCementChemistry_SetCurveOfWaterSiliconRatioInCSH(HCC,A) \
        (HCC)->SetCurveOfWaterSiliconRatioInCSH(A)
        
#define HardenedCementChemistry_SetCurveOfSaturationIndexOfSH(HCC,A) \
        (HCC)->SetCurveOfSaturationIndexOfSH(A)
        
#define HardenedCementChemistry_SetSolidProductName(HCC,A) \
        ((HCC)->SetSolidProductName(A))



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
#define HardenedCementChemistry_NbOfPrimaryVariables  CementSolutionChemistry_NbOfPrimaryVariables

/* Different sets of primary variable may be used for:
   O,H,Al,Ca,C,Cl,Fe,Mg,N,K,Si,Na,Sr,S */
/* Oxygen (O) */
#define HardenedCementChemistry_P_O            CementSolutionChemistry_P_O
#define HardenedCementChemistry_P_LogA_H2O     HardenedCementChemistry_P_O
/* Hydrogen (H) */
#define HardenedCementChemistry_P_H            CementSolutionChemistry_P_H
#define HardenedCementChemistry_P_LogC_OH      HardenedCementChemistry_P_H
#define HardenedCementChemistry_P_LogC_H   \
        (HardenedCementChemistry_P_H + HardenedCementChemistry_NbOfPrimaryVariables)
/* Aluminium (Al) */
#define HardenedCementChemistry_P_Al           CementSolutionChemistry_P_Al
#define HardenedCementChemistry_P_LogC_AlO2    HardenedCementChemistry_P_Al
#define HardenedCementChemistry_P_SI_AH3 \
        (HardenedCementChemistry_P_Al + HardenedCementChemistry_NbOfPrimaryVariables)
#define HardenedCementChemistry_P_SI_AlOHmic \
        (HardenedCementChemistry_P_Al + 2*HardenedCementChemistry_NbOfPrimaryVariables)
/* Calcium (Ca) */
#define HardenedCementChemistry_P_Ca           CementSolutionChemistry_P_Ca
#define HardenedCementChemistry_P_LogC_Ca      HardenedCementChemistry_P_Ca
#define HardenedCementChemistry_P_SI_CH \
        (HardenedCementChemistry_P_Ca + HardenedCementChemistry_NbOfPrimaryVariables)
#define HardenedCementChemistry_P_SI_CH_CC \
        (HardenedCementChemistry_P_Ca + 2*HardenedCementChemistry_NbOfPrimaryVariables)
#define HardenedCementChemistry_P_SI_CH_Cc     HardenedCementChemistry_P_SI_CH_CC
#define HardenedCementChemistry_P_SI_CH_CSH2 \
        (HardenedCementChemistry_P_Ca + 3*HardenedCementChemistry_NbOfPrimaryVariables)
#define HardenedCementChemistry_P_SI_CH_CsH2   HardenedCementChemistry_P_SI_CH_CSH2
/* Carbon (C) */
#define HardenedCementChemistry_P_C            CementSolutionChemistry_P_C
#define HardenedCementChemistry_P_LogC_CO3     HardenedCementChemistry_P_C
#define HardenedCementChemistry_P_LogC_CO2  \
        (HardenedCementChemistry_P_C + HardenedCementChemistry_NbOfPrimaryVariables)
/* Chlorine (Cl) */
#define HardenedCementChemistry_P_Cl           CementSolutionChemistry_P_Cl
#define HardenedCementChemistry_P_LogC_Cl      HardenedCementChemistry_P_Cl
/* Iron (Fe) */
#define HardenedCementChemistry_P_Fe           CementSolutionChemistry_P_Fe
#define HardenedCementChemistry_P_LogC_FeO2    HardenedCementChemistry_P_Fe
/* Magnesium (Mg) */
#define HardenedCementChemistry_P_Mg           CementSolutionChemistry_P_Mg
#define HardenedCementChemistry_P_LogC_Mg      HardenedCementChemistry_P_Mg
/* Nitrogen (N) */
#define HardenedCementChemistry_P_N            CementSolutionChemistry_P_N
#define HardenedCementChemistry_P_LogC_NO3     HardenedCementChemistry_P_N
/* Potassium (K) */
#define HardenedCementChemistry_P_K            CementSolutionChemistry_P_K
#define HardenedCementChemistry_P_LogC_K       HardenedCementChemistry_P_K
#define HardenedCementChemistry_P_LogA_K \
        (HardenedCementChemistry_P_K + HardenedCementChemistry_NbOfPrimaryVariables)
/* Silicon (Si) */
#define HardenedCementChemistry_P_Si           CementSolutionChemistry_P_Si
#define HardenedCementChemistry_P_LogC_SiO2    HardenedCementChemistry_P_Si
#define HardenedCementChemistry_P_SI_CSH \
        (HardenedCementChemistry_P_Si + HardenedCementChemistry_NbOfPrimaryVariables)
/* Sodium (Na) */
#define HardenedCementChemistry_P_Na           CementSolutionChemistry_P_Na
#define HardenedCementChemistry_P_LogC_Na      HardenedCementChemistry_P_Na
/* Strontium (Sr) */
#define HardenedCementChemistry_P_Sr           CementSolutionChemistry_P_Sr
#define HardenedCementChemistry_P_LogC_Sr      HardenedCementChemistry_P_Sr
/* Sulfur (S) */
#define HardenedCementChemistry_P_S            CementSolutionChemistry_P_S
#define HardenedCementChemistry_P_LogC_SO4     HardenedCementChemistry_P_S
#define HardenedCementChemistry_P_LogC_H2SO4 \
        (HardenedCementChemistry_P_S + HardenedCementChemistry_NbOfPrimaryVariables)



/* Inputs: definition
 * ------------------ */
#define HardenedCementChemistry_GetInput(HCC,U) \
        HardenedCementChemistry_GetPrimaryVariable(HCC)[HardenedCementChemistry_InputIndex(U)]
        
#define HardenedCementChemistry_SetInput(HCC,U,...) \
        do{ \
          HardenedCementChemistry_GetIndex(HCC,U) = HardenedCementChemistry_Index(U) ; \
          HardenedCementChemistry_GetInput(HCC,U) = __VA_ARGS__ ; \
        } while(0)
        
#define HardenedCementChemistry_InputCalciumIs(HCC,V) \
        HardenedCementChemistry_InputIs(HCC,Ca,V)
        
#define HardenedCementChemistry_InputSulfurIs(HCC,V) \
        HardenedCementChemistry_InputIs(HCC,S,V)
        
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
        
#define HardenedCementChemistry_InputIsNot(HCC,U,V) \
        (HardenedCementChemistry_GetIndex(HCC,U) != HardenedCementChemistry_Index(V))





/* Macros for the type of solid products (source CEMDATA18)
 * -------------------------------------------------------- */
#define HardenedCementChemistry_NbOfSolidProducts  (142)

/* The following macros are composed by the index and the stoichiometry 
 * coefficients. The latters are ordered using the index of the primary 
 * variables, i.e.:
 * (H2O[0],H[+1],AlO2[-1],Ca[+2],CO3[-2],Cl[-1],FeO2[-1],Mg[+2],NO3[-1],
 *  K[+1],SiO2[0],Na[+1],Sr[+2],SO4[-2])
 */
#define HardenedCementChemistry_5CA               (0),(2.75,-2.25,0.25,1.25,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_5CNA              (1),(2.75,-2.75,0.25,1.25,0,0,0,0,0,0,0,0.5,0,0)
#define HardenedCementChemistry_AlOHam            (2),(1,1,1,0,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_AlO3H3am          HardenedCementChemistry_AlOHam // synonym
#define HardenedCementChemistry_AlOHmic           (3),(1,1,1,0,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_AlO3H3mic         HardenedCementChemistry_AlOHmic // synonym
#define HardenedCementChemistry_AH3               HardenedCementChemistry_AlOHmic // synonym
#define HardenedCementChemistry_AmorSl            (4),(0,0,0,0,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_AmSilica          HardenedCementChemistry_AmorSl // synonym
#define HardenedCementChemistry_SH                HardenedCementChemistry_AmorSl // synonym
#define HardenedCementChemistry_Anh               (5),(0,0,0,1,0,0,0,0,0,0,0,0,0,1)
#define HardenedCementChemistry_Anhydrite         HardenedCementChemistry_Anh // synonym
#define HardenedCementChemistry_Arg               (6),(0,0,0,1,1,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_Aragonite         HardenedCementChemistry_Arg // synonym
#define HardenedCementChemistry_Brc               (7),(2,-2,0,0,0,0,0,1,0,0,0,0,0,0)
#define HardenedCementChemistry_Brucite           HardenedCementChemistry_Brc // synonym
#define HardenedCementChemistry_C2AClH5           (8),(6,-2,1,2,0,1,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C2AH7d5           (9),(8.5,-2,2,2,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C2AH8             HardenedCementChemistry_C2AH7d5 // synonym
#define HardenedCementChemistry_C2AH65            (10),(8,-3,1,2,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C2S               (11),(2,-4,0,2,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_C3A               (12),(2,-4,2,3,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C3AFS0d84H4d32    (13),(6.32,-4,1,3,0,0,1,0,0,0,0.84,0,0,0)
#define HardenedCementChemistry_C3AH6             (14),(8,-4,2,3,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C3AS0d41H5d18     (15),(7.18,-4,2,3,0,0,0,0,0,0,0.41,0,0,0)
#define HardenedCementChemistry_C3AS0d84H4d32     (16),(6.32,-4,2,3,0,0,0,0,0,0,0.84,0,0,0)
#define HardenedCementChemistry_C3FH6             (17),(8,-4,0,3,0,0,2,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C3FS0d84H4d32     (18),(6.32,-4,0,3,0,0,2,0,0,0,0.84,0,0,0)
#define HardenedCementChemistry_C3FS1d34H3d32     (19),(5.32,-4,0,3,0,0,2,0,0,0,1.32,0,0,0)
#define HardenedCementChemistry_C3S               (20),(3,-6,0,3,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_C4AClH10          (21),(12,-4,2,4,0,2,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_FriedelSalt       HardenedCementChemistry_C4AClH10 // synonym
#define HardenedCementChemistry_C4FeCl2H10        (22),(12,-4,0,4,0,2,2,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C4AF              (23),(2,-4,2,4,0,0,2,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C4AH11            (24),(14,-6,2,4,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C4AH13            (25),(16,-6,2,4,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C4AH19            (26),(22,-6,2,4,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C4AsClH12         (27),(14,-4,2,4,0,1,0,0,0,0,0,0,0,0.5)
#define HardenedCementChemistry_KuzelSalt         HardenedCementChemistry_C4AsClH12 // synonym
#define HardenedCementChemistry_C4FH13            (28),(16,-6,0,4,0,0,2,0,0,0,0,0,0,0)
#define HardenedCementChemistry_C12A7             (29),(5,-10,14,12,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_CA2               (30),(-1,2,4,1,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_CA                (31),(0,0,2,1,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_CAH10             (32),(10,0,2,1,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_Cal               (33),(0,0,0,1,1,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_CC                HardenedCementChemistry_Cal // synonym
#define HardenedCementChemistry_Calcite           HardenedCementChemistry_Cal // synonym
#define HardenedCementChemistry_Cls               (34),(0,0,0,0,0,0,0,0,0,0,0,0,1,1)
#define HardenedCementChemistry_SrSO4             HardenedCementChemistry_Cls // synonym
#define HardenedCementChemistry_CSH3TT2C          (35),(4,-3,0,1.5,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_CSH3TT5C          (36),(3.75,-2.5,0,1.25,0,0,0,0,0,0,1.25,0,0,0)
#define HardenedCementChemistry_CSH3TTobH         (37),(3.5,-2,0,1,0,0,0,0,0,0,1.5,0,0,0)
#define HardenedCementChemistry_CSHQJenD          (38),(4,-3,0,1.5,0,0,0,0,0,0,0.6667,0,0,0)
#define HardenedCementChemistry_CSHQJenH          (39),(3.5,-2.6666,0,1.3333,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_CSHQTobD          (40),(2.6668,-1.66675,0,0.833375,0,0,0,0,0,0,0.6667,0,0,0)
#define HardenedCementChemistry_CSHQTobH          (41),(2.1667,-1.3334,0,0.6667,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_DisDol            (42),(0,0,0,1,2,0,0,1,0,0,0,0,0,0)
#define HardenedCementChemistry_ECSH1KSH          (43),(0.7,-0.5,0,0,0,0,0,0,0,0.5,0.2,0,0,0)
#define HardenedCementChemistry_ECSH1NaSH         (44),(0.7,-0.5,0,0,0,0,0,0,0,0,0.2,0.5,0,0)
#define HardenedCementChemistry_ECSH1SH           (45),(1,0,0,0,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_ECSH1SrSH         (46),(3,-2,0,0,0,0,0,0,0,0,1,0,1,0)
#define HardenedCementChemistry_ECSH1TobCa        (47),(2.6666,-1.6666,0,0.8333,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_ECSH2JenCa        (48),(2.60004,-2.00004,0,1.00002,0,0,0,0,0,0,0.6,0,0,0)
#define HardenedCementChemistry_ECSH2KSH          (49),(0.7,-0.5,0,0,0,0,0,0,0,0.5,0.2,0,0,0)
#define HardenedCementChemistry_ECSH2NaSH         (50),(0.7,-0.5,0,0,0,0,0,0,0,0,0.2,0.5,0,0)
#define HardenedCementChemistry_ECSH2SrSH         (51),(3,-2,0,0,0,0,0,0,0,0,0,1,1,0)
#define HardenedCementChemistry_ECSH2TobCa        (52),(2.66,-1.66,0,0.83,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_ettringite        (53),(34,-4,2,6,0,0,0,0,0,0,0,0,0,3)
#define HardenedCementChemistry_AFt               HardenedCementChemistry_ettringite // synonym
#define HardenedCementChemistry_ettringite03_ss   (54),(11.3,-1.32,0.67,2,0,0,0,0,0,0,0,0,0,1)
#define HardenedCementChemistry_ettringite05      (55),(17,-2,1,3,0,0,0,0,0,0,0,0,0,1.5)
#define HardenedCementChemistry_ettringite9       (56),(11,-4,2,6,0,0,0,0,0,0,0,0,0,3)
#define HardenedCementChemistry_Ettringite9_des   (57),(11,-4,2,6,0,0,0,0,0,0,0,0,0,3)
#define HardenedCementChemistry_ettringite13      (58),(15,-4,2,6,0,0,0,0,0,0,0,0,0,3)
#define HardenedCementChemistry_Ettringite13_des  (59),(15,-4,2,6,0,0,0,0,0,0,0,0,0,3)
#define HardenedCementChemistry_ettringite30      (60),(32,-4,2,6,0,0,0,0,0,0,0,0,0,3)
#define HardenedCementChemistry_Feettringite05    (61),(17,-2,0,3,0,0,1,0,0,0,0,0,0,1.5)
#define HardenedCementChemistry_Feettringite      (62),(34,-4,0,6,0,0,2,0,0,0,0,0,0,3)
#define HardenedCementChemistry_Fehemicarbonate   (63),(12.5,-5,0,4,0.5,0,2,0,0,0,0,0,0,0)
#define HardenedCementChemistry_Femonosulph05     (64),(7,-2,0,2,0,0,1,0,0,0,0,0,0,0.5)
#define HardenedCementChemistry_Femonosulphate    (65),(14,-4,0,4,0,0,2,0,0,0,0,0,0,1)
#define HardenedCementChemistry_Fe                (66),(-2,4,0,0,0,0,1,0,0,0,0,0,0,0)
#define HardenedCementChemistry_Femonocarbonate   (67),(14,-4,0,4,1,0,2,0,0,0,0,0,0,0)
#define HardenedCementChemistry_FeOOHmic          (68),(0,1,0,0,0,0,1,0,0,0,0,0,0,0)
#define HardenedCementChemistry_FeO3H3am          (69),(1,1,0,0,0,0,1,0,0,0,0,0,0,0)
#define HardenedCementChemistry_FeO3H3mic         (70),(1,1,0,0,0,0,1,0,0,0,0,0,0,0)
#define HardenedCementChemistry_FeCO3pr           (71),(-2,4,0,0,1,0,1,0,0,0,0,0,0,0)
#define HardenedCementChemistry_Gbs               (72),(1,1,1,0,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_Gibbsite          HardenedCementChemistry_Gbs // synonym
#define HardenedCementChemistry_Gp                (73),(2,0,0,1,0,0,0,0,0,0,0,0,0,1)
#define HardenedCementChemistry_CSH2              HardenedCementChemistry_Gp // synonym
#define HardenedCementChemistry_Gypsum            HardenedCementChemistry_Gp // synonym
#define HardenedCementChemistry_Gr                (74),(-3,6,0,0,1,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_Gt                (75),(0,1,0,0,0,0,1,0,0,0,0,0,0,0)
#define HardenedCementChemistry_Hem               (76),(-1,2,0,0,0,0,2,0,0,0,0,0,0,0)
#define HardenedCementChemistry_hemicarbonat10d5  (77),(13,-5,2,4,0.5,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_hemicarbonate     (78),(14.5,-5,2,4,0.5,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_hemicarbonate9    (79),(11.5,-5,2,4,0.5,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_hemihydrate       (80),(0.5,0,0,1,0,0,0,0,0,0,0,0,0,1)
#define HardenedCementChemistry_hydrotalcite      (81),(13,-6,2,0,0,0,0,4,0,0,0,0,0,0)
#define HardenedCementChemistry_INFCA             (82),(2.5,-1.6875,0.3125,1,0,0,0,0,0,0,1.1875,0,0,0)
#define HardenedCementChemistry_INFCN             (83),(2.5,-2.625,0,1,0,0,0,0,0,0,1.5,0.625,0,0)
#define HardenedCementChemistry_INFCNA            (84),(2.5,-2.375,0.3125,1,0,0,0,0,0,0,1.1875,0.6875,0,0)
#define HardenedCementChemistry_Jennite           (85),(3.766667,-3.33334,0,1.66667,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_K2O               (86),(1,-2,0,0,0,0,0,0,0,2,0,0,0,0)
#define HardenedCementChemistry_K2SO4             (87),(0,0,0,0,0,0,0,0,0,2,0,0,0,1)
#define HardenedCementChemistry_Kln               (88),(1,2,2,0,0,0,0,0,0,0,2,0,0,0)
#define HardenedCementChemistry_KSiOH             (89),(0.7,-0.5,0,0,0,0,0,0,0,0.5,0.2,0,0,0)
#define HardenedCementChemistry_Lim               (90),(1,-2,0,1,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_M4AOHLDH          (91),(13,-6,2,0,0,0,0,4,0,0,0,0,0,0)
#define HardenedCementChemistry_M6AOHLDH          (92),(17,-10,2,0,0,0,0,6,0,0,0,0,0,0)
#define HardenedCementChemistry_M8AOHLDH          (93),(21,-14,2,0,0,0,0,8,0,0,0,0,0,0)
#define HardenedCementChemistry_Mag               (94),(-2,4,0,0,0,0,3,0,0,0,0,0,0,0)
#define HardenedCementChemistry_Melanterite       (95),(5,4,0,0,0,0,1,0,0,0,0,0,0,1)
#define HardenedCementChemistry_Mg2AlC0d5OH       (96),(6,-2,1,0,0.5,0,0,2,0,0,0,0,0,0)
#define HardenedCementChemistry_Mg2FeC0d5OH       (97),(6,-2,0,0,0.5,0,1,2,0,0,0,0,0,0)
#define HardenedCementChemistry_Mg3AlC0d5OH       (98),(8.5,-4,1,0,0.5,0,0,3,0,0,0,0,0,0)
#define HardenedCementChemistry_Mg3FeC0d5OH       (99),(8.5,-4,0,0,0.5,0,1,3,0,0,0,0,0,0)
#define HardenedCementChemistry_Mgs               (100),(0,0,0,0,1,0,0,1,0,0,0,0,0,0)
#define HardenedCementChemistry_monocarbonate05   (101),(6.5,-2,1,2,0.5,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_monocarbonate9    (102),(11,-4,2,4,1,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_monocarbonate     (103),(13,-4,2,4,1,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_mononitrate       (104),(12,-4,2,4,0,0,0,0,2,0,0,0,0,0)
#define HardenedCementChemistry_mononitrite       (105),(10,0,2,4,0,0,0,0,2,0,0,0,0,0)
#define HardenedCementChemistry_monosulphate9     (106),(11,-4,2,4,0,0,0,0,0,0,0,0,0,1)
#define HardenedCementChemistry_monosulphate10_5  (107),(12.5,-4,2,4,0,0,0,0,0,0,0,0,0,1)
#define HardenedCementChemistry_monosulphate12    (108),(14,-4,2,4,0,0,0,0,0,0,0,0,0,1)
#define HardenedCementChemistry_AFm               HardenedCementChemistry_monosulphate12 // synonym
#define HardenedCementChemistry_monosulphate14    (109),(16,-4,2,4,0,0,0,0,0,0,0,0,0,1)
#define HardenedCementChemistry_monosulphate16    (110),(18,-4,2,4,0,0,0,0,0,0,0,0,0,1)
#define HardenedCementChemistry_monosulphate1205  (111),(7,-2,1,2,0,0,0,0,0,0,0,0,0,0.5)
#define HardenedCementChemistry_Na2O              (112),(1,-2,0,0,0,0,0,0,0,0,0,2,0,0)
#define HardenedCementChemistry_Na2SO4            (113),(0,0,0,0,0,0,0,0,0,0,0,2,0,1)
#define HardenedCementChemistry_NaSiOH            (114),(0.7,-0.5,0,0,0,0,0,0,0,0,0,0.5,0,0)
#define HardenedCementChemistry_OrdDol            (115),(0,0,0,1,2,0,0,1,0,0,0,0,0,0)
#define HardenedCementChemistry_Portlandite       (116),(2,-2,0,1,0,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_CH                HardenedCementChemistry_Portlandite // synonym
#define HardenedCementChemistry_Py                (117),(-10,20,0,0,0,0,0,0,0,0,1,0,0,2)
#define HardenedCementChemistry_Qtz               (118),(0,0,0,0,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_Sd                (119),(-2,4,0,0,1,0,1,0,0,0,0,0,0,0)
#define HardenedCementChemistry_straetlingite5_5  (120),(6.5,-2,2,2,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_straetlingite7    (121),(8,-2,2,2,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_straetlingite     (122),(9,-2,2,2,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_Str               (123),(0,0,0,0,1,0,0,0,0,0,0,0,1,0)
#define HardenedCementChemistry_Sulfur            (124),(-4,8,0,0,0,0,0,0,0,0,0,0,0,1)
#define HardenedCementChemistry_syngenite         (125),(1,0,0,1,0,0,0,0,0,2,0,0,0,2)
#define HardenedCementChemistry_T2CCNASHss        (126),(4,-3,0,1.5,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_T5CCNASHss        (127),(3.75,-2.5,0,1.25,0,0,0,0,0,0,1.25,0,0,0)
#define HardenedCementChemistry_thaumasite        (128),(16,-2,0,3,1,0,0,0,0,0,1,0,0,1)
#define HardenedCementChemistry_TobI              (129),(5.2,-4,0,2,0,0,0,0,0,0,2.4,0,0,0)
#define HardenedCementChemistry_TobII             (130),(2.166666,-1.666666,0,0.833333,0,0,0,0,0,0,1,0,0,0)
#define HardenedCementChemistry_TobHCNASHss       (131),(3.5,-2,0,1,0,0,0,0,0,0,1.5,0,0,0)
#define HardenedCementChemistry_tricarboalu03     (132),(11.3333333,-1.3333332,0.6666667,2,1,0,0,0,0,0,0,0,0,0)
#define HardenedCementChemistry_Tro               (133),(-6,12,0,0,0,0,1,0,0,0,0,0,0,1)
#define HardenedCementChemistry_zeoliteP_Ca       (134),(4.5,0,2,1,0,0,0,0,0,0,2,0,0,0)
#define HardenedCementChemistry_chabazite         (135),(6,0,2,1,0,0,0,0,0,0,4,0,0,0)
#define HardenedCementChemistry_M075SH            (136),(4,-3,0,0,0,0,0,1.5,0,0,2,0,0,0)
#define HardenedCementChemistry_M15SH             (137),(4,-3,0,0,0,0,0,1.5,0,0,1,0,0,0)
#define HardenedCementChemistry_zeoliteX          (138),(6.2,0,2,0,0,0,0,0,0,0,3,2,0,0)
#define HardenedCementChemistry_natrolite         (139),(2,0,2,0,0,0,0,0,0,0,3,2,0,0)
#define HardenedCementChemistry_zeoliteY          (140),(8,0,2,0,0,0,0,0,0,0,4,2,0,0)


#define HardenedCementChemistry_GetIndexOf(A) \
        Tuple_1ST(Tuple_TUPLE(Utils_CAT(HardenedCementChemistry_,A)))
        
#define HardenedCementChemistry_GetStoichioOf(A) \
        Tuple_2ND(Tuple_TUPLE(Utils_CAT(HardenedCementChemistry_,A)))



/* Indexes of element contents
 * ---------------------------*/
#define HardenedCementChemistry_NbOfElementContents  CementSolutionChemistry_NbOfElementConcentrations

#define HardenedCementChemistry_E_Al       CementSolutionChemistry_E_Al
#define HardenedCementChemistry_E_Ca       CementSolutionChemistry_E_Ca
#define HardenedCementChemistry_E_C        CementSolutionChemistry_E_C
#define HardenedCementChemistry_E_Cl       CementSolutionChemistry_E_Cl
#define HardenedCementChemistry_E_Fe       CementSolutionChemistry_E_Fe
#define HardenedCementChemistry_E_Mg       CementSolutionChemistry_E_Mg
#define HardenedCementChemistry_E_N        CementSolutionChemistry_E_N
#define HardenedCementChemistry_E_K        CementSolutionChemistry_E_K
#define HardenedCementChemistry_E_Si       CementSolutionChemistry_E_Si
#define HardenedCementChemistry_E_Na       CementSolutionChemistry_E_Na
#define HardenedCementChemistry_E_Sr       CementSolutionChemistry_E_Sr
#define HardenedCementChemistry_E_S        CementSolutionChemistry_E_S

/* Range of species indexes belonging to element
 * --------------------------------------------- */
#define HardenedCementChemistry_Range_Al   {0,1,2,3,8,9,10,12,13,14,15,16,21,23,24,25,26,27,29,30,31,32,53,54,55,56,57,58,59,72,77,78,79,80,81,82,84,88,91,92,93,96,98,101,102,103,104,105,106,107,108,109,110,111,120,121,122,132,134,135,138,139,140}
#define HardenedCementChemistry_Range_Ca   {}
#define HardenedCementChemistry_Range_C    {}
#define HardenedCementChemistry_Range_Cl   {}
#define HardenedCementChemistry_Range_Fe   {}
#define HardenedCementChemistry_Range_Mg   {}
#define HardenedCementChemistry_Range_N    {}
#define HardenedCementChemistry_Range_K    {}
#define HardenedCementChemistry_Range_Si   {}
#define HardenedCementChemistry_Range_Na   {}
#define HardenedCementChemistry_Range_Sr   {}
#define HardenedCementChemistry_Range_S    {}




/* Macros for saturation indexes
 * -----------------------------*/
#define HardenedCementChemistry_GetSaturationIndexOf(HCC,A) \
        (HardenedCementChemistry_GetSaturationIndex(HCC)[HardenedCementChemistry_GetIndexOf(A)])
        
#define HardenedCementChemistry_SetSaturationIndexOf(HCC,A,B) \
        do {\
          HardenedCementChemistry_GetSaturationIndexOf(HCC,A) = B;\
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
#define HardenedCementChemistry_NbOfConcentrations  CementSolutionChemistry_NbOfSpecies
        
#define HardenedCementChemistry_GetAqueousConcentration(HCC) \
        CementSolutionChemistry_GetConcentration(HardenedCementChemistry_GetCementSolutionChemistry(HCC))
        
#define HardenedCementChemistry_GetAqueousConcentrationOf(HCC,CPD) \
        CementSolutionChemistry_GetConcentrationOf(HardenedCementChemistry_GetCementSolutionChemistry(HCC),CPD)
        
#define HardenedCementChemistry_SetAqueousConcentrationOf(HCC,CPD,A) \
        do {\
          HardenedCementChemistry_GetAqueousConcentrationOf(HCC,CPD) = A;\
        } while(0)



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

/* Water activity */
#define HardenedCementChemistry_GetLog10IdealWaterActivity(HCC) \
        CementSolutionChemistry_GetLog10IdealWaterActivity(HardenedCementChemistry_GetCementSolutionChemistry(HCC))
        
#define HardenedCementChemistry_GetLog10WaterActivity(HCC) \
        (HardenedCementChemistry_GetLog10IdealWaterActivity(HCC) +\
        Log10ActivityCoefficientOfWaterInAqueousSolution(DAVIES,HardenedCementChemistry_GetIonicStrength(HCC)))

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
#define HardenedCementChemistry_GetLog10SolubilityProductConstantOf(HCC,CPD) \
        (HardenedCementChemistry_GetLog10SolubilityProductConstant(HCC)[HardenedCementChemistry_GetIndexOf(CPD)])
        
#define HardenedCementChemistry_SetLog10SolubilityProductConstantOf(HCC,CPD,A) \
        do {\
          HardenedCementChemistry_GetLog10SolubilityProductConstantOf(HCC,CPD) = A;\
        } while(0)


/* Macros for equilibrium constant of chemical reactions in solution
 * -----------------------------------------------------------------*/
#define HardenedCementChemistry_GetLog10EquilibriumConstantOf(HCC,CPD) \
        CementSolutionChemistry_GetLog10EquilibriumConstantOf(HardenedCementChemistry_GetCementSolutionChemistry(HCC),CPD)



/* Macros for the resolution of the systems
 * ----------------------------------------*/
 
#define HardenedCementChemistry_ComputeSystem(...) \
        Utils_CAT_NARG(HardenedCementChemistry_ComputeSystem,__VA_ARGS__)(__VA_ARGS__)

#define HardenedCementChemistry_DATABASE   CEMDATA
 
#define HardenedCementChemistry_ComputeSystem1(HCC) \
        Utils_CAT(HardenedCementChemistry_ComputeSystem,HardenedCementChemistry_DATABASE)(HCC,0)
 
#define HardenedCementChemistry_ComputeSystem2(HCC,I) \
        Utils_CAT(HardenedCementChemistry_ComputeSystem,HardenedCementChemistry_DATABASE)(HCC,I)
        
#define HardenedCementChemistry_ComputeSystem3(HCC,SYS,I) \
        HardenedCementChemistry_ComputeSystemDEFAULT(HCC,I)
        
//        Utils_CAT(HardenedCementChemistry_ComputeSystem_,SYS)(HCC)

#define HardenedCementChemistry_SolveElectroneutrality(HCC) \
        (CementSolutionChemistry_SolveElectroneutralityCEMDATA(HardenedCementChemistry_GetCementSolutionChemistry(HCC)))

#define HardenedCementChemistry_SolveExplicitElectroneutrality(HCC) \
        (CementSolutionChemistry_SolveExplicitElectroneutrality(HardenedCementChemistry_GetCementSolutionChemistry(HCC)))



/* Macros for the CSH curves */
#define HardenedCementChemistry_GetCSHCurve(HCC) \
        (Curves_GetCurve(HardenedCementChemistry_GetCSHCurves(HCC)))

#define HardenedCementChemistry_SetDefaultCurveOfCalciumSiliconRatioInCSH(HCC) \
        (HCC)->SetCurveOfCalciumSiliconRatioInCSH(HardenedCementChemistry_GetCSHCurve(HCC) + (0))

#define HardenedCementChemistry_SetDefaultCurveOfWaterSiliconRatioInCSH(HCC) \
        (HCC)->SetCurveOfWaterSiliconRatioInCSH(HardenedCementChemistry_GetCSHCurve(HCC) + (1))

#define HardenedCementChemistry_SetDefaultCurveOfSaturationIndexOfSH(HCC) \
        (HCC)->SetCurveOfSaturationIndexOfSH(HardenedCementChemistry_GetCSHCurve(HCC) + (2))


       

/* Macros for setting properties of species
 * ---------------------------------------- */
#define HardenedCementChemistry_SetProperty(CPD,AFFECT,Z,PROP) \
        do {\
          AFFECT((Z)[HardenedCementChemistry_GetIndexOf(CPD)],PROP(CPD));\
        } while(0)

#define HardenedCementChemistry_SetProperties(...) \
        do {\
          HardenedCementChemistry_SetProperty(5CA,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(5CNA,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(AlOHam,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(AlOHmic,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(AmorSl,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Anh,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Arg,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Brc,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C2AClH5,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C2AH7d5,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C2AH65,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C2S,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C3A,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C3AFS0d84H4d32,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C3AH6,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C3AS0d41H5d18,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C3AS0d84H4d32,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C3FH6,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C3FS0d84H4d32,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C3FS1d34H3d32,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C3S,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C4AClH10,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C4FeCl2H10,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C4AF,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C4AH11,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C4AH13,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C4AH19,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C4AsClH12,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C4FH13,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(C12A7,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(CA2,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(CA,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(CAH10,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Cal,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Cls,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(CSH3TT2C,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(CSH3TT5C,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(CSH3TTobH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(CSHQJenD,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(CSHQJenH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(CSHQTobD,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(CSHQTobH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(DisDol,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ECSH1KSH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ECSH1NaSH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ECSH1SH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ECSH1SrSH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ECSH1TobCa,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ECSH2JenCa,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ECSH2KSH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ECSH2NaSH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ECSH2SrSH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ECSH2TobCa,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ettringite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ettringite03_ss,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ettringite05,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ettringite9,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Ettringite9_des,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ettringite13,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Ettringite13_des,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(ettringite30,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Feettringite05,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Feettringite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Fehemicarbonate,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Femonosulph05,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Femonosulphate,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Fe,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Femonocarbonate,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(FeOOHmic,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(FeO3H3am,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(FeO3H3mic,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(FeCO3pr,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Gbs,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Gp,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Gr,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Gt,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Hem,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(hemicarbonat10d5,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(hemicarbonate,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(hemicarbonate9,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(hemihydrate,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(hydrotalcite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(INFCA,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(INFCN,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(INFCNA,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Jennite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(K2O,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(K2SO4,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Kln,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(KSiOH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Lim,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(M4AOHLDH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(M6AOHLDH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(M8AOHLDH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Mag,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Melanterite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Mg2AlC0d5OH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Mg2FeC0d5OH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Mg3AlC0d5OH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Mg3FeC0d5OH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Mgs,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(monocarbonate05,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(monocarbonate9,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(monocarbonate,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(mononitrate,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(mononitrite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(monosulphate9,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(monosulphate10_5,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(monosulphate12,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(monosulphate14,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(monosulphate16,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(monosulphate1205,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Na2O,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Na2SO4,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(NaSiOH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(OrdDol,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Portlandite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Py,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Qtz,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Sd,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(straetlingite5_5,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(straetlingite7,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(straetlingite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Str,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Sulfur,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(syngenite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(T2CCNASHss,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(T5CCNASHss,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(thaumasite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(TobI,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(TobII,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(TobHCNASHss,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(tricarboalu03,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(Tro,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(zeoliteP_Ca,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(chabazite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(M075SH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(M15SH,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(zeoliteX,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(natrolite,__VA_ARGS__);\
          HardenedCementChemistry_SetProperty(zeoliteY,__VA_ARGS__);\
        } while(0)

#define HardenedCementChemistry_MaxLengthOfSolidProductName  (20)

#include "CementSolutionChemistry.h"

template<typename T>
struct HardenedCementChemistry_t {
//  int nbofprimaryvariables ;
//  int nbofvariables ;
  int*    _primaryvariableindex ;
  T* _primaryvariable ;
  T* _variable ;
  T* _saturationindex ;
  double* _log10solubilityproductconstant ;
  double* _constant ;
  char** _solidproductname;
  CementSolutionChemistry_t<T>* _csc ;
  Curves_t* _cshcurves ;
  Curve_t*  _curveofcalciumsiliconratioincsh ;
  Curve_t*  _curveofwatersiliconratioincsh ;
  Curve_t*  _curveofsaturationindexofsh ;
  
  public:
  void SetPrimaryVariableIndex(int* x){_primaryvariableindex = x;}
  void SetPrimaryVariable(T* x){_primaryvariable = x;}
  void SetVariable(T* x){_variable = x;}
  void SetSaturationIndex(T* x){_saturationindex = x;}
  void SetLog10SolubilityProductConstant(double* x){_log10solubilityproductconstant = x;}
  void SetConstant(double* x){_constant = x;}
  void SetCementSolutionChemistry(CementSolutionChemistry_t<T>* x){_csc = x;}
  void SetCSHCurves(Curves_t* x){_cshcurves = x;}
  void SetCurveOfCalciumSiliconRatioInCSH(Curve_t* x){_curveofcalciumsiliconratioincsh = x;}
  void SetCurveOfWaterSiliconRatioInCSH(Curve_t* x){_curveofwatersiliconratioincsh = x;}
  void SetCurveOfSaturationIndexOfSH(Curve_t* x){_curveofsaturationindexofsh = x;}
  void SetSolidProductName(char** x){_solidproductname = x;}
  
  /* Accessors */
  int* GetPrimaryVariableIndex(void){return(_primaryvariableindex);}
  T* GetPrimaryVariable(void){return(_primaryvariable);}
  T* GetVariable(void){return(_variable);}
  T* GetSaturationIndex(void){return(_saturationindex);}
  double* GetLog10SolubilityProductConstant(void){return(_log10solubilityproductconstant);}
  double* GetConstant(void){return(_constant);}
  CementSolutionChemistry_t<T>* GetCementSolutionChemistry(void){return(_csc);}
  Curves_t* GetCSHCurves(void){return(_cshcurves);}
  Curve_t*  GetCurveOfCalciumSiliconRatioInCSH(void){return(_curveofcalciumsiliconratioincsh);}
  Curve_t*  GetCurveOfWaterSiliconRatioInCSH(void){return(_curveofwatersiliconratioincsh);}
  Curve_t*  GetCurveOfSaturationIndexOfSH(void){return(_curveofsaturationindexofsh);}
  char**    GetSolidProductName(){return(_solidproductname);}
  
  void Init(void) {
    constexpr double min = std::numeric_limits<double>::min();
    constexpr double logmin = 2*log10(min);
    int n_vp = HardenedCementChemistry_NbOfPrimaryVariables;
  
    _csc->Init();
    
    for(int i = 0; i < n_vp; i++) {
      _primaryvariableindex[i] = i;
      _primaryvariable[i] = logmin;
    }
  }
} ;


#define HardenedCementChemistry_Init(HCC) (HCC)->Init()


#define HardenedCementChemistry_GetStoichiometry()\
        HardenedCementChemistry_GetSolidProperties()

inline double* (HardenedCementChemistry_GetSolidProperties)(void);
inline double* (HardenedCementChemistry_CreateSolidProperties)(void);

template<typename T>
inline HardenedCementChemistry_t<T>* (HardenedCementChemistry_Create)(void) ;
template<typename T>
inline void (HardenedCementChemistry_AllocateMemory)(HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_Delete)(HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_UpdateChemicalConstantsCEMDATA)(HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_PrintChemicalConstants)(HardenedCementChemistry_t<T>*) ;

template<typename T>
inline void (HardenedCementChemistry_InitPrimaryVariables)(HardenedCementChemistry_t<T>*,double);
template<typename T>
inline void (HardenedCementChemistry_ComputeSystemCEMDATA)(HardenedCementChemistry_t<T>*);
template<typename T>
inline void (HardenedCementChemistry_ComputeSystemDEFAULT)(HardenedCementChemistry_t<T>*);

template<typename T>
inline void (HardenedCementChemistry_ComputeSaturationIndexesCEMDATA)(HardenedCementChemistry_t<T>*);

#if 0
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_CO2_H2O_0)      (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_H2O_0)      (HardenedCementChemistry_t<T>*) ;
template<typename T>
inline void (HardenedCementChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_Al2O3_H2O_0)(HardenedCementChemistry_t<T>*) ;
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
#endif


#include "Curves.h"
#include "Utils.h"


#include "HardenedCementChemistry.h.in"

#endif
