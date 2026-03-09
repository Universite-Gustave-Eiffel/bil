#ifndef CEMENTSOLUTIONCHEMISTRY_HPP
#define CEMENTSOLUTIONCHEMISTRY_HPP

/* Forward declarations */
//struct CementSolutionChemistry_t;
template<typename T> struct CementSolutionChemistry_t;
struct Temperature_t;


#define CementSolutionChemistry_GetTemperature(CSC) \
        ((CSC)->GetTemperature())
        
#define CementSolutionChemistry_GetPrimaryVariableIndex(CSC) \
        ((CSC)->GetPrimaryVariableIndex())
        
#define CementSolutionChemistry_GetPrimaryVariable(CSC) \
        ((CSC)->GetPrimaryVariable())

#define CementSolutionChemistry_GetConcentration(CSC) \
        ((CSC)->GetConcentration())

#define CementSolutionChemistry_GetLogActivity(CSC) \
       ((CSC)->GetLogActivity())

#define CementSolutionChemistry_GetElementConcentration(CSC) \
        ((CSC)->GetElementConcentration())

#define CementSolutionChemistry_GetOtherVariable(CSC) \
        ((CSC)->GetOtherVariable())

#define CementSolutionChemistry_GetLog10Keq(CSC) \
        ((CSC)->GetLog10Keq())

#define CementSolutionChemistry_GetElectricPotential(CSC) \
        ((CSC)->GetElectricPotential())
        
        



#define CementSolutionChemistry_SetTemperature(CSC,A) \
        ((CSC)->SetTemperature(A))
        
#define CementSolutionChemistry_SetPrimaryVariableIndex(CSC,A) \
        ((CSC)->SetPrimaryVariableIndex(A))
        
#define CementSolutionChemistry_SetPrimaryVariable(CSC,A) \
        ((CSC)->SetPrimaryVariable(A))

#define CementSolutionChemistry_SetConcentration(CSC,A) \
        ((CSC)->SetConcentration(A))

#define CementSolutionChemistry_SetLogActivity(CSC,A) \
       ((CSC)->SetLogActivity(A))

#define CementSolutionChemistry_SetElementConcentration(CSC,A) \
        ((CSC)->SetElementConcentration(A))

#define CementSolutionChemistry_SetOtherVariable(CSC,A) \
        ((CSC)->SetOtherVariable(A))

#define CementSolutionChemistry_SetLog10Keq(CSC,A) \
        ((CSC)->SetLog10Keq(A))

#define CementSolutionChemistry_SetElectricPotential(CSC,A) \
        ((CSC)->SetElectricPotential(A))




/* Macros for the room temperature
 * -------------------------------*/
#define CementSolutionChemistry_GetRoomTemperature(CSC) \
        Temperature_GetRoomValue(CementSolutionChemistry_GetTemperature(CSC))

#define CementSolutionChemistry_SetRoomTemperature(CSC,T) \
        do {\
          Temperature_SetRoomTemperature(CementSolutionChemistry_GetTemperature(CSC),T);\
          CementSolutionChemistry_UpdateChemicalConstantsCEMDATA(CSC);\
        } while(0)



/* Macros for primary variables
 * ----------------------------*/
 
#define CementSolutionChemistry_NbOfPrimaryVariables  (14)

/* Different sets of primary variable may be used for:
   O,H,Al,Ca,C,Cl,Fe,Mg,N,K,Si,Na,Sr,S */
/* Oxygen (O) */
#define CementSolutionChemistry_P_O            (0)
#define CementSolutionChemistry_P_LogA_H2O     CementSolutionChemistry_P_O
/* Hydrogen (H) */
#define CementSolutionChemistry_P_H            (1)
#define CementSolutionChemistry_P_LogA_H       CementSolutionChemistry_P_H // (cemdata) 
#define CementSolutionChemistry_P_LogA_OH \
        (CementSolutionChemistry_P_H + CementSolutionChemistry_NbOfPrimaryVariables)
/* Aluminium (Al) */
#define CementSolutionChemistry_P_Al           (2)
#define CementSolutionChemistry_P_LogA_AlO2    CementSolutionChemistry_P_Al
#define CementSolutionChemistry_P_LogA_AlO4H4  CementSolutionChemistry_P_LogA_AlO2 // synonym
#define CementSolutionChemistry_P_LogQ_AH3 \
        (CementSolutionChemistry_P_Al + CementSolutionChemistry_NbOfPrimaryVariables)
/* Calcium (Ca) */
#define CementSolutionChemistry_P_Ca           (3)
#define CementSolutionChemistry_P_LogA_Ca      CementSolutionChemistry_P_Ca // (cemdata)
#define CementSolutionChemistry_P_LogQ_CH \
        (CementSolutionChemistry_P_Ca + CementSolutionChemistry_NbOfPrimaryVariables)
/* Carbon (C) */
#define CementSolutionChemistry_P_C            (4)
#define CementSolutionChemistry_P_LogA_CO3     CementSolutionChemistry_P_C // (cemdata)
#define CementSolutionChemistry_P_LogA_CO2 \
        (CementSolutionChemistry_P_C + CementSolutionChemistry_NbOfPrimaryVariables)
/* Chlorine (Cl) */
#define CementSolutionChemistry_P_Cl           (5)
#define CementSolutionChemistry_P_LogA_Cl      CementSolutionChemistry_P_Cl // (cemdata)
/* Iron (Fe) */
#define CementSolutionChemistry_P_Fe           (6)
#define CementSolutionChemistry_P_LogA_FeO2    CementSolutionChemistry_P_Fe // (cemdata)
/* Magnesium (Mg) */
#define CementSolutionChemistry_P_Mg           (7)
#define CementSolutionChemistry_P_LogA_Mg      CementSolutionChemistry_P_Mg // (cemdata)
/* Nitrogen (N) */
#define CementSolutionChemistry_P_N            (8)
#define CementSolutionChemistry_P_LogA_NO3     CementSolutionChemistry_P_N // (cemdata)
/* Potassium (K) */
#define CementSolutionChemistry_P_K            (9)
#define CementSolutionChemistry_P_LogA_K       CementSolutionChemistry_P_K // (cemdata)
/* Silicon (Si) */
#define CementSolutionChemistry_P_Si           (10)
#define CementSolutionChemistry_P_LogA_SiO2    CementSolutionChemistry_P_Si // (cemdata) 
#define CementSolutionChemistry_P_LogQ_SH \
        (CementSolutionChemistry_P_Si + CementSolutionChemistry_NbOfPrimaryVariables)
/* Sodium (Na) */
#define CementSolutionChemistry_P_Na           (11)
#define CementSolutionChemistry_P_LogA_Na      CementSolutionChemistry_P_Na // (cemdata)
/* Strontium (Sr) */
#define CementSolutionChemistry_P_Sr           (12)
#define CementSolutionChemistry_P_LogA_Sr      CementSolutionChemistry_P_Sr // not used
/* Sulfur (S) */
#define CementSolutionChemistry_P_S            (13)
#define CementSolutionChemistry_P_LogA_SO4     CementSolutionChemistry_P_S // (cemdata)
#define CementSolutionChemistry_P_LogA_H2SO4 \
        (CementSolutionChemistry_P_S + CementSolutionChemistry_NbOfPrimaryVariables)


       
/* Inputs: definition
 * ------------------ */
#define CementSolutionChemistry_GetInput(CSC,U) \
        CementSolutionChemistry_GetPrimaryVariable(CSC)[CementSolutionChemistry_InputIndex(U)]
        
#define CementSolutionChemistry_SetInput(CSC,U,...) \
        do{ \
          CementSolutionChemistry_GetIndex(CSC,U) = CementSolutionChemistry_Index(U) ; \
          CementSolutionChemistry_GetInput(CSC,U) = __VA_ARGS__ ; \
        } while(0)
        
#define CementSolutionChemistry_InputSulfurIs(CSC,V) \
        CementSolutionChemistry_InputIs(CSC,S,V)

#define CementSolutionChemistry_InputAluminiumIs(CSC,V) \
        CementSolutionChemistry_InputIs(CSC,Al,V)

        
/* Inputs: implementation
 * ---------------------- */
#define CementSolutionChemistry_Index(U) \
        (CementSolutionChemistry_P_##U)

#define CementSolutionChemistry_InputIndex(U) \
        (CementSolutionChemistry_Index(U) % CementSolutionChemistry_NbOfPrimaryVariables)
        
#define CementSolutionChemistry_GetIndex(CSC,U) \
        CementSolutionChemistry_GetPrimaryVariableIndex(CSC)[CementSolutionChemistry_InputIndex(U)]
        
#define CementSolutionChemistry_InputIs(CSC,U,V) \
        (CementSolutionChemistry_GetIndex(CSC,U) == CementSolutionChemistry_Index(V))


       
       
/* List of compound names
 * ----------------------*/
#define CementSolutionChemistry_NbOfSpecies  (88)


/* Macros for the solution species (source CEMDATA18)
 * --------------------------------------------------*/
/* The following stoichiometry coefficients are ordered using the index of primary variables
 * H2O[0],H[+1],AlO2[-1],Ca[+2],CO3[-2],Cl[-1],FeO2[-1],Mg[+2],NO3[-1],K[+1],SiO2[0],Na[+1],Sr[+2],SO4[-2]
 */

/* Hydrogen-Oxygen
 * ---------------*/
#define CementSolutionChemistry_H2O         (0),(1,0,0,0,0,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_H           (1),(0,1,0,0,0,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_OH          (2),(1,-1,0,0,0,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_H2          (3),(0,2,0,0,0,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_O2          (4),(2,-4,0,0,0,0,0,0,0,0,0,0,0,0)

/* Compound of type I
 * ------------------*/
/* Aluminium: */
#define CementSolutionChemistry_Al          (5),(-2,4,1,0,0,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_AlO2        (6),(0,0,1,0,0,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_AlO4H4      CementSolutionChemistry_AlO2 // synonym
#define CementSolutionChemistry_AlO         (7),(-1,2,1,0,0,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_AlOH        (8),(-1,3,1,0,0,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_AlO2H       (9),(0,1,1,0,0,0,0,0,0,0,0,0,0,0)
/* Calcium: */
#define CementSolutionChemistry_Ca          (10),(0,0,0,1,0,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_CaOH        (11),(1,-1,0,1,0,0,0,0,0,0,0,0,0,0)
/* Carbon: */
#define CementSolutionChemistry_HCO3        (12),(0,1,0,0,1,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_CO3         (13),(0,0,0,0,1,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_CO2         (14),(-1,2,0,0,1,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_CH4         (15),(-3,10,0,0,1,0,0,0,0,0,0,0,0,0)
/* Chlorine: */
#define CementSolutionChemistry_Cl          (16),(0,0,0,0,0,1,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_ClO4        (17),(4,-8,0,0,0,1,0,0,0,0,0,0,0,0)
/* Iron: */
#define CementSolutionChemistry_Fe          (18),(-2,4,0,0,0,0,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_FeO2        (19),(0,0,0,0,0,0,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_FeO         (20),(-1,2,0,0,0,0,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_FeO2H       (21),(0,1,0,0,0,0,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_FeOH        (22),(-1,3,0,0,0,0,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_FeOH_p2     (23),(-1,3,0,0,0,0,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_Fe3O4H4     (24),(-2,8,0,0,0,0,3,0,0,0,0,0,0,0)
#define CementSolutionChemistry_Fe_p3       (25),(-2,4,0,0,0,0,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_Fe2O2H2     (87),(-2,6,0,0,0,0,2,0,0,0,0,0,0,0)
/* Magnesium: */
#define CementSolutionChemistry_Mg          (26),(0,0,0,0,0,0,0,1,0,0,0,0,0,0)
#define CementSolutionChemistry_MgOH        (27),(1,-1,0,0,0,0,0,1,0,0,0,0,0,0)
/* Nitrogen: */
#define CementSolutionChemistry_NO3         (28),(0,0,0,0,0,0,0,0,1,0,0,0,0,0)
#define CementSolutionChemistry_N2          (29),(-6,12,0,0,0,0,0,0,2,0,0,0,0,0)
#define CementSolutionChemistry_NH3         (30),(-3,9,0,0,0,0,0,0,1,0,0,0,0,0)
#define CementSolutionChemistry_NH4         (31),(-3,10,0,0,0,0,0,0,1,0,0,0,0,0)
/* Potassium: */
#define CementSolutionChemistry_K           (32),(0,0,0,0,0,0,0,0,0,1,0,0,0,0)
#define CementSolutionChemistry_KOH         (33),(1,-1,0,0,0,0,0,0,0,1,0,0,0,0)
/* Silicon: */
#define CementSolutionChemistry_SiO3        (34),(1,-2,0,0,0,0,0,0,0,0,1,0,0,0)
#define CementSolutionChemistry_H2SiO4      CementSolutionChemistry_SiO3 // synonym
#define CementSolutionChemistry_HSiO3       (35),(1,-1,0,0,0,0,0,0,0,0,1,0,0,0)
#define CementSolutionChemistry_H3SiO4      CementSolutionChemistry_HSiO3 // synonym
#define CementSolutionChemistry_SiO2        (36),(0,0,0,0,0,0,0,0,0,0,1,0,0,0)
#define CementSolutionChemistry_H4SiO4      CementSolutionChemistry_SiO2 // synonym
#define CementSolutionChemistry_Si4O10      (37),(2,-4,0,0,0,0,0,0,0,0,4,0,0,0)
/* Sodium: */
#define CementSolutionChemistry_Na          (38),(0,0,0,0,0,0,0,0,0,0,0,1,0,0)
#define CementSolutionChemistry_NaOH        (39),(-1,1,0,0,0,0,0,0,0,0,0,1,0,0)
/* Strontium: */
#define CementSolutionChemistry_Sr          (40),(0,0,0,0,0,0,0,0,0,0,0,0,1,0)
#define CementSolutionChemistry_SrOH        (41),(-1,1,0,0,0,0,0,0,0,0,0,0,1,0)
/* Sulfur: */
#define CementSolutionChemistry_H2SO4       (42),(0,2,0,0,0,0,0,0,0,0,0,0,0,1) // sulfuric acid (not present in cemdata)
#define CementSolutionChemistry_HSO4        (43),(0,1,0,0,0,0,0,0,0,0,0,0,0,1)
#define CementSolutionChemistry_SO4         (44),(0,0,0,0,0,0,0,0,0,0,0,0,0,1)
#define CementSolutionChemistry_S2O3        (45),(-5,10,0,0,0,0,0,0,0,0,0,0,0,2)
#define CementSolutionChemistry_HS          (46),(-4,9,0,0,0,0,0,0,0,0,0,0,0,1)
#define CementSolutionChemistry_S           (47),(-4,8,0,0,0,0,0,0,0,0,0,0,0,1)
#define CementSolutionChemistry_SO3         (48),(-1,2,0,0,0,0,0,0,0,0,0,0,0,1)
#define CementSolutionChemistry_HSO3        (49),(-1,3,0,0,0,0,0,0,0,0,0,0,0,1)
#define CementSolutionChemistry_H2S         (86),(-4,10,0,0,0,0,0,0,0,0,0,0,0,1)

/* Compound of type II
 * -------------------*/
/* Aluminium-Silicon: */
#define CementSolutionChemistry_AlSiO5      (50),(1,-2,1,0,0,0,0,0,0,0,1,0,0,0)
#define CementSolutionChemistry_AlHSiO3     (51),(-1,3,1,0,0,0,0,0,0,0,1,0,0,0)
/* Aluminium-Sulfur: */
#define CementSolutionChemistry_AlSO4       (52),(-2,4,1,0,0,0,0,0,0,0,0,0,0,1)
#define CementSolutionChemistry_AlS2O8      (53),(-2,4,1,0,0,0,0,0,0,0,0,0,0,2)
/* Calcium-Carbon: */
#define CementSolutionChemistry_CaHCO3      (54),(0,1,0,1,1,0,0,0,0,0,0,0,0,0)
#define CementSolutionChemistry_CaCO3       (55),(0,0,0,1,1,0,0,0,0,0,0,0,0,0)
/* Calcium-Silicon: */
#define CementSolutionChemistry_CaSiO3      (56),(1,-2,0,1,0,0,0,0,0,0,1,0,0,0)
#define CementSolutionChemistry_CaH2SiO4    CementSolutionChemistry_CaSiO3 // synonym
#define CementSolutionChemistry_CaHSiO3     (57),(1,-1,0,1,0,0,0,0,0,0,1,0,0,0)
#define CementSolutionChemistry_CaH3SiO4    CementSolutionChemistry_CaHSiO3 // synonym
/* Calcium-Sulfur: */
#define CementSolutionChemistry_CaSO4       (58),(0,0,0,1,0,0,0,0,0,0,0,0,0,1)
/* Iron-Carbon: */
#define CementSolutionChemistry_FeCO3       (59),(-2,4,0,0,1,0,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_FeHCO3      (60),(-2,5,0,0,1,0,1,0,0,0,0,0,0,0)
/* Iron-Chlorine: */
#define CementSolutionChemistry_FeCl        (61),(-2,4,0,0,0,1,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_FeCl_p2     (62),(-2,4,0,0,0,1,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_FeCl2       (63),(-2,4,0,0,0,2,1,0,0,0,0,0,0,0)
#define CementSolutionChemistry_FeCl3       (64),(-2,4,0,0,0,3,1,0,0,0,0,0,0,0)
/* Iron-Silicon: */
#define CementSolutionChemistry_FeHSiO3     (65),(-1,3,0,0,0,0,1,0,0,0,1,0,0,0)
/* Iron-Sulfur: */
#define CementSolutionChemistry_FeHSO4      (66),(-2,5,0,0,0,0,1,0,0,0,0,0,0,1)
#define CementSolutionChemistry_FeSO4       (67),(-2,4,0,0,0,0,1,0,0,0,0,0,0,1)
#define CementSolutionChemistry_FeSO4_p1    (68),(-2,4,0,0,0,0,1,0,0,0,0,0,0,1)
#define CementSolutionChemistry_FeS2O8      (69),(-2,4,0,0,0,0,1,0,0,0,0,0,0,2)
#define CementSolutionChemistry_FeHSO4_p2   (84),(-2,5,0,0,0,0,1,0,0,0,0,0,0,1)
/* Magnesium-Carbon: */
#define CementSolutionChemistry_MgCO3       (70),(0,0,0,0,1,0,0,1,0,0,0,0,0,0)
#define CementSolutionChemistry_MgHCO3      (71),(0,1,0,0,1,0,0,1,0,0,0,0,0,0)
/* Magnesium-Silicon: */
#define CementSolutionChemistry_MgHSiO3     (72),(1,-1,0,0,0,0,0,1,0,0,1,0,0,0)
#define CementSolutionChemistry_MgSiO3      (73),(1,-2,0,0,0,0,0,1,0,0,1,0,0,0)
/* Magnesium-Sulfur: */
#define CementSolutionChemistry_MgSO4       (74),(0,0,0,0,0,0,0,1,0,0,0,0,0,1)
/* Nitrogen-Carbon: */
#define CementSolutionChemistry_HCN         (85),(-6,13,0,0,1,0,0,0,1,0,0,0,0,0)
/* Potassium-Sulfur: */
#define CementSolutionChemistry_KSO4        (75),(0,0,0,0,0,0,0,0,0,1,0,0,0,1)
/* Sodium-Carbon: */
#define CementSolutionChemistry_NaHCO3      (76),(0,1,0,0,1,0,0,0,0,0,0,1,0,0)
#define CementSolutionChemistry_NaCO3       (77),(0,0,0,0,1,0,0,0,0,0,0,1,0,0)
/* Sodium-Sulfur: */
#define CementSolutionChemistry_NaSO4       (78),(0,0,0,0,0,0,0,0,0,0,0,1,0,1)
/* Strontium-Carbon: */
#define CementSolutionChemistry_SrCO3       (79),(0,0,0,0,1,0,0,0,0,0,0,0,1,0)
#define CementSolutionChemistry_SrHCO3      (80),(0,1,0,0,1,0,0,0,0,0,0,0,1,0)
/* Strontium-Silicon: */
#define CementSolutionChemistry_SrSiO3      (81),(1,-2,0,0,0,0,0,0,0,0,1,0,1,0)
/* Strontium-Sulfur: */
#define CementSolutionChemistry_SrSO4       (82),(0,0,0,0,0,0,0,0,0,0,0,0,1,1)

/* Compound of type III
 * --------------------*/
/* Sulfur-Carbon-Nitrogen: */
#define CementSolutionChemistry_SCN         (83),(-10,20,0,0,1,0,0,0,1,0,0,0,0,1)




#define CementSolutionChemistry_GetIndexOf(CPD) \
        Tuple_1ST(Tuple_TUPLE(Utils_CAT(CementSolutionChemistry_,CPD)))
        
#define CementSolutionChemistry_GetStoichioOf(CPD) \
        Tuple_2ND(Tuple_TUPLE(Utils_CAT(CementSolutionChemistry_,CPD)))


#define CementSolutionChemistry_GetConcentrationOf(CSC,CPD) \
       (CementSolutionChemistry_GetConcentration(CSC)[CementSolutionChemistry_GetIndexOf(CPD)])

       
#define CementSolutionChemistry_GetLogActivityOf(CSC,CPD) \
       (CementSolutionChemistry_GetLogActivity(CSC)[CementSolutionChemistry_GetIndexOf(CPD)])



/* Macros for other variables
 * --------------------------*/
#define CementSolutionChemistry_NbOfOtherVariables       (4)

/* 1. Liquid mass density */
#define CementSolutionChemistry_GetLiquidMassDensity(CSC) \
       (CementSolutionChemistry_GetOtherVariable(CSC)[0])
        
#define CementSolutionChemistry_SetLiquidMassDensity(CSC,A) \
        do {\
          CementSolutionChemistry_GetLiquidMassDensity(CSC) = A;\
        } while(0)


/* 2. Charge density */
#define CementSolutionChemistry_GetChargeDensity(CSC) \
       (CementSolutionChemistry_GetOtherVariable(CSC)[1])
        
#define CementSolutionChemistry_SetChargeDensity(CSC,A) \
        do {\
          CementSolutionChemistry_GetChargeDensity(CSC) = A;\
        } while(0)


/* 3. Ionic strength */
#define CementSolutionChemistry_GetIonicStrength(CSC) \
       (CementSolutionChemistry_GetOtherVariable(CSC)[2])
        
#define CementSolutionChemistry_SetIonicStrength(CSC,A) \
        do {\
          CementSolutionChemistry_GetIonicStrength(CSC) = A;\
        } while(0)


/* 4. Ideal water activity */
#define CementSolutionChemistry_GetLog10IdealWaterActivity(CSC) \
       (CementSolutionChemistry_GetOtherVariable(CSC)[3])
        
#define CementSolutionChemistry_SetLog10IdealWaterActivity(CSC,A) \
        do {\
          CementSolutionChemistry_GetLog10IdealWaterActivity(CSC) = A;\
        } while(0)



/* Indexes of element concentrations
 * ---------------------------------*/
#define CementSolutionChemistry_NbOfElementConcentrations       (12)

#define CementSolutionChemistry_E_Al          (0)
#define CementSolutionChemistry_E_Ca          (1)
#define CementSolutionChemistry_E_C           (2)
#define CementSolutionChemistry_E_Cl          (3)
#define CementSolutionChemistry_E_Fe          (4)
#define CementSolutionChemistry_E_Mg          (5)
#define CementSolutionChemistry_E_N           (6)
#define CementSolutionChemistry_E_K           (7)
#define CementSolutionChemistry_E_Si          (8)
#define CementSolutionChemistry_E_Na          (9)
#define CementSolutionChemistry_E_Sr          (10)
#define CementSolutionChemistry_E_S           (11)

/* Range of species indexes belonging to element
 * --------------------------------------------- */
#define CementSolutionChemistry_Range_Al   {5,6,7,8,9,50,51,52,53}
#define CementSolutionChemistry_Range_Ca   {10,11,54,55,56,57,58}
#define CementSolutionChemistry_Range_C    {12,13,14,15,54,55,59,60,70,71,76,77,79,80,83,85}
#define CementSolutionChemistry_Range_Cl   {16,17,61,62,63,63,64,64,64}
#define CementSolutionChemistry_Range_Fe   {18,19,20,21,22,23,24,24,24,25,59,60,61,62,63,64,65,66,67,68,69,84,87}
#define CementSolutionChemistry_Range_Mg   {26,27,70,71,72,73,74}
#define CementSolutionChemistry_Range_N    {28,29,29,30,31,83,85}
#define CementSolutionChemistry_Range_K    {32,33,75}
#define CementSolutionChemistry_Range_Si   {34,35,36,37,37,37,37,50,51,56,57,65,72,73,81}
#define CementSolutionChemistry_Range_Na   {38,39,76,77,78}
#define CementSolutionChemistry_Range_Sr   {40,41,79,80,81,82}
#define CementSolutionChemistry_Range_S    {42,43,44,45,45,46,47,48,49,52,53,53,58,66,67,68,69,69,74,75,78,82,83,84,86}


#define CementSolutionChemistry_GetElementConcentrationOf(CSC,A) \
        (CementSolutionChemistry_GetElementConcentration(CSC)[CementSolutionChemistry_E_##A])
        

#if 1
#define CementSolutionChemistry_SetElementConcentrationOf(CSC,A) \
        do {\
          auto* c = CementSolutionChemistry_GetConcentration(CSC);\
          auto z = c[0];\
          z = 0;\
          for(int i : CementSolutionChemistry_Range_##A) {\
            z += c[i];\
          }\
          (CementSolutionChemistry_GetElementConcentration(CSC)[CementSolutionChemistry_E_##A]) = z;\
        } while(0)
#else
#define CementSolutionChemistry_SetElementConcentrationOf(CSC,A) \
        do {\
          auto* c = CementSolutionChemistry_GetConcentration(CSC);\
          double* stoic = CementSolutionChemistry_GetStoichiometry();
          auto z = c[0];\
          z = 0;\
          for(int i = 0 ; i < CementSolutionChemistry_NbOfSpecies ; i++) {\
            double* s = stoic + i*CementSolutionChemistry_NbOfPrimaryVariables;\
              z += c[i]*s[CementSolutionChemistry_Index(A)];\
          }\
          CementSolutionChemistry_GetElementConcentration(CSC)[CementSolutionChemistry_E_##A] = z;\
        } while(0)
#endif




/* Macros for equilibrium constants (same indices as CementSolutionChemistry_CPD)
 * ------------------------------------------------------------------------------*/
#define CementSolutionChemistry_GetLog10EquilibriumConstantOf(CSC,CPD) \
       (CementSolutionChemistry_GetLog10Keq(CSC)[CementSolutionChemistry_GetIndexOf(CPD)])
       
#define CementSolutionChemistry_SetLog10EquilibriumConstantOf(CSC,CPD,V) \
        do {\
         (CementSolutionChemistry_GetLog10Keq(CSC)[CementSolutionChemistry_GetIndexOf(CPD)]) = V;\
       } while(0)




/* Macro for the resolution of the system
 * --------------------------------------*/
#define CementSolutionChemistry_ComputeSystem(CSC,SYS) \
       (CementSolutionChemistry_ComputeSystemDEFAULT(CSC))
       
//       (CementSolutionChemistry_ComputeSystem_##SYS(CSC))
       
#if 0
#define CementSolutionChemistry_SupplementSystemWith(CSC,A) \
       (CementSolutionChemistry_SupplementSystemWith_##A(CSC))
#endif
       

/* Macros for setting properties of species
 * ---------------------------------------- */
#define CementSolutionChemistry_SetProperty(CPD,AFFECT,Z,PROP) \
        do {\
          AFFECT((Z)[CementSolutionChemistry_GetIndexOf(CPD)],PROP(CPD));\
        } while(0)
        
#define CementSolutionChemistry_SetProperties(...) \
        do {\
          CementSolutionChemistry_SetProperty(H2O,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(OH,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(H,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(H2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(O2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Al,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(AlO2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(AlO,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(AlOH,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(AlO2H,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Ca,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(CaOH,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(HCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(CO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(CO2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(CH4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Cl,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(ClO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Fe,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Fe_p3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeO,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeO2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeOH,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeO2H,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeOH_p2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Fe3O4H4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Fe2O2H2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Mg,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(MgOH,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(N2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(NH3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(NH4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(NO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(K,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(KOH,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(SiO2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(SiO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(HSiO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Si4O10,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Na,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(NaOH,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(Sr,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(SrOH,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(H2SO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(HSO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(SO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(SO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(HSO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(S,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(HS,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(S2O3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(H2S,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(AlSiO5,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(AlHSiO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(AlSO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(AlS2O8,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(CaHCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(CaCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(CaSiO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(CaHSiO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(CaSO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeHCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeCl,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeCl_p2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeCl2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeCl3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeHSiO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeSO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeSO4_p1,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeHSO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeS2O8,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(FeHSO4_p2,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(MgCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(MgHCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(MgSiO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(MgHSiO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(MgSO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(HCN,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(KSO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(NaHCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(NaCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(NaSO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(SrCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(SrHCO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(SrSiO3,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(SrSO4,__VA_ARGS__);\
          CementSolutionChemistry_SetProperty(SCN,__VA_ARGS__);\
        } while(0)



/* Primary template */
template<typename T = double>
struct CementSolutionChemistry_t {
  private:
  Temperature_t* _temperature ;
  int*    _primaryvariableindex ;
  T* _primaryvariable ;
  T* _concentration ;
  T* _logactivity ;
  T* _elementconcentration ;
  T* _othervariable ;
  T  _electricpotential ;
  double* _log10equilibriumconstant ;
  
  public:
  void SetTemperature(Temperature_t* x){_temperature = x;}
  void SetPrimaryVariableIndex(int* x){_primaryvariableindex = x;}
  void SetPrimaryVariable(T* x){_primaryvariable = x;}
  void SetConcentration(T* x){_concentration = x;}
  void SetLogActivity(T* x){_logactivity = x;}
  void SetElementConcentration(T* x){_elementconcentration = x;}
  void SetOtherVariable(T* x){_othervariable = x;}
  void SetLog10Keq(double* x){_log10equilibriumconstant = x;}
  void SetElectricPotential(T x){_electricpotential = x;}
  
  /* Accessors */
  Temperature_t* GetTemperature(void){return(_temperature);}
  int*    GetPrimaryVariableIndex(void){return(_primaryvariableindex);}
  T* GetPrimaryVariable(void){return(_primaryvariable);}
  T* GetConcentration(void){return(_concentration);}
  T* GetLogActivity(void){return(_logactivity);}
  T* GetElementConcentration(void){return(_elementconcentration);}
  T* GetOtherVariable(void){return(_othervariable);}
  double* GetLog10Keq(void){return(_log10equilibriumconstant);}
  T& GetElectricPotential(void){return(_electricpotential);}

  void Init(void) {
    constexpr double min = std::numeric_limits<double>::min();
    constexpr double logmin = 2*log10(min);
    int n_pv = CementSolutionChemistry_NbOfPrimaryVariables;
    int n_sp = CementSolutionChemistry_NbOfSpecies ;

    for(int i = 0 ; i < n_pv ; i++) {
      _primaryvariableindex[i] = i;
      _primaryvariable[i] = logmin;
    }
  
    for(int i = 0 ; i < n_sp ; i++) {
      _logactivity[i] = logmin;
    }
  }
} ;



#define CementSolutionChemistry_Init(CSC) (CSC)->Init()


#define CementSolutionChemistry_GetValence()\
        CementSolutionChemistry_GetSpeciesProperties()
        
#define CementSolutionChemistry_GetMolarMass()\
        CementSolutionChemistry_GetSpeciesProperties() + CementSolutionChemistry_NbOfSpecies
        
#define CementSolutionChemistry_GetPartialMolarVolume()\
        CementSolutionChemistry_GetSpeciesProperties() + 2*CementSolutionChemistry_NbOfSpecies
        
#define CementSolutionChemistry_GetStoichiometry()\
        CementSolutionChemistry_GetSpeciesProperties() + 3*CementSolutionChemistry_NbOfSpecies
        
inline double* (CementSolutionChemistry_GetSpeciesProperties)(void);
inline double* (CementSolutionChemistry_CreateSpeciesProperties)(void) ;

template<typename T>
inline CementSolutionChemistry_t<T>* (CementSolutionChemistry_Create)(void);
template<typename T>
inline void (CementSolutionChemistry_AllocateMemory)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void  (CementSolutionChemistry_Delete)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_UpdateChemicalConstantsCEMDATA)(CementSolutionChemistry_t<T>*);
template<typename T>
inline T    (CementSolutionChemistry_ComputeChargeDensity)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline T    (CementSolutionChemistry_ComputeIonicStrength)(CementSolutionChemistry_t<T>*);
template<typename T>
inline T    (CementSolutionChemistry_ComputeLiquidMassDensity)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline T    (CementSolutionChemistry_ComputeLog10IdealWaterActivity)(CementSolutionChemistry_t<T>*);
template<typename T>
inline void (CementSolutionChemistry_UpdateElementConcentrations)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_TranslateActivitiesIntoConcentrations)(CementSolutionChemistry_t<T>*,double) ;
template<typename T>
inline void   (CementSolutionChemistry_PrintChemicalConstants)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_UpdateSolution)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_CopyConcentrations)(CementSolutionChemistry_t<T>*,T*) ;
template<typename T>
inline void   (CementSolutionChemistry_CopyChemicalPotential)(CementSolutionChemistry_t<T>*,T*) ;
template<typename T>
inline void   (CementSolutionChemistry_ComputeSystemCEMDATA)(CementSolutionChemistry_t<T>*,double);
template<typename T>
inline void   (CementSolutionChemistry_ComputeSystemDEFAULT)(CementSolutionChemistry_t<T>*,double);
template<typename T>
inline int    (CementSolutionChemistry_SolveElectroneutralityCEMDATA)(CementSolutionChemistry_t<T>*);
template<typename T>
inline int    (CementSolutionChemistry_SolveExplicitElectroneutrality) (CementSolutionChemistry_t<T>*) ;
template<typename T>
inline T    (CementSolutionChemistry_SolvePoly4)(T,T,T,T,T) ;

#if 0
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_SO3_LogA_H2SO4)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_SO3_LogA_SO4)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_Al2O3_LogQ_AH3)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_Al2O3_LogA_AlO4H4)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_H2O)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_CO2_H2O)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_H2O)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_Al2O3_H2O)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_Al2O3_Cl_H2O)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_Al2O3_CO2_H2O)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_Al2O3_CO2_Cl_H2O)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_Al2O3_SO3_H2O)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_ComputeSystem_CaO_SiO2_Na2O_K2O_SO3_Al2O3_H2O)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_Cl)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_CO2)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_SO3)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_Al2O3)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline int    (CementSolutionChemistry_SolveElectroneutralityDEFAULT)(CementSolutionChemistry_t<T>*) ;
#endif
//inline double* instancevalence = NULL ;


#include "Utils.h"
#include "Tuple.h"

#include "CementSolutionChemistry.h.in"

#endif
