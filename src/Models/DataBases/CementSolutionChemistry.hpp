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

#define CementSolutionChemistry_GetLogConcentration(CSC) \
       ((CSC)->GetLogConcentration())

#define CementSolutionChemistry_GetActivity(CSC) \
        ((CSC)->GetActivity())

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

#define CementSolutionChemistry_SetLogConcentration(CSC,A) \
       ((CSC)->SetLogConcentration(A))

#define CementSolutionChemistry_SetActivity(CSC,A) \
        ((CSC)->SetActivity(A))

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
        Temperature_SetRoomTemperature(CementSolutionChemistry_GetTemperature(CSC),T)



/* Macros for primary variables
 * ----------------------------*/
 
#define CementSolutionChemistry_NbOfPrimaryVariables  (15)

/* Different sets of primary variable may be used */
#define CementSolutionChemistry_P_H2O          (0)
#define CementSolutionChemistry_P_LogA_H2O     CementSolutionChemistry_P_H2O

#define CementSolutionChemistry_P_CaO          (1)
#define CementSolutionChemistry_P_LogQ_CH      CementSolutionChemistry_P_CaO
#define CementSolutionChemistry_P_LogA_Ca \
        (CementSolutionChemistry_P_CaO + CementSolutionChemistry_NbOfPrimaryVariables) // (cemdata) not used yet

#define CementSolutionChemistry_P_SiO2         (2)
#define CementSolutionChemistry_P_LogQ_SH      CementSolutionChemistry_P_SiO2
#define CementSolutionChemistry_P_LogA_SiO2 \
        (CementSolutionChemistry_P_SiO2 + CementSolutionChemistry_NbOfPrimaryVariables) // (cemdata) not used yet

#define CementSolutionChemistry_P_Al2O3        (3)
#define CementSolutionChemistry_P_LogQ_AH3     CementSolutionChemistry_P_Al2O3
#define CementSolutionChemistry_P_LogA_AlO4H4 \
        (CementSolutionChemistry_P_Al2O3 + CementSolutionChemistry_NbOfPrimaryVariables)
#define CementSolutionChemistry_P_LogA_AlO2 \
        (CementSolutionChemistry_P_Al2O3 + CementSolutionChemistry_NbOfPrimaryVariables) // synonym (cemdata)

#define CementSolutionChemistry_P_Na2O         (4)
#define CementSolutionChemistry_P_LogA_Na      CementSolutionChemistry_P_Na2O // (cemdata)

#define CementSolutionChemistry_P_K2O          (5)
#define CementSolutionChemistry_P_LogA_K       CementSolutionChemistry_P_K2O // (cemdata)

#define CementSolutionChemistry_P_CO2          (6)
#define CementSolutionChemistry_P_LogA_CO2     CementSolutionChemistry_P_CO2
#define CementSolutionChemistry_P_LogA_CO3 \
        (CementSolutionChemistry_P_CO2 + CementSolutionChemistry_NbOfPrimaryVariables) // (cemdata) not used yet

#define CementSolutionChemistry_P_SO3          (7)
#define CementSolutionChemistry_P_LogA_SO4     CementSolutionChemistry_P_SO3
#define CementSolutionChemistry_P_LogA_H2SO4 \
        (CementSolutionChemistry_P_SO3 + CementSolutionChemistry_NbOfPrimaryVariables) // (cemdata)

#define CementSolutionChemistry_P_Cl           (8)
#define CementSolutionChemistry_P_LogA_Cl      CementSolutionChemistry_P_Cl // (cemdata)

#define CementSolutionChemistry_P_Fe2O3        (9)
#define CementSolutionChemistry_P_LogA_FeO2    CementSolutionChemistry_P_Fe2O3 // (cemdata) not used yet

#define CementSolutionChemistry_P_MgO          (10)
#define CementSolutionChemistry_P_LogA_Mg      CementSolutionChemistry_P_MgO // (cemdata) not used yet

#define CementSolutionChemistry_P_TiO2         (11) // not used

#define CementSolutionChemistry_P_P2O5         (12) // not used

#define CementSolutionChemistry_P_SrO          (13) // not used

#define CementSolutionChemistry_P_H            (14)
#define CementSolutionChemistry_P_LogA_OH      CementSolutionChemistry_P_H
#define CementSolutionChemistry_P_LogA_H \
        (CementSolutionChemistry_P_H + CementSolutionChemistry_NbOfPrimaryVariables) // (cemdata) not used yet


       
/* Inputs: definition
 * ------------------ */
#define CementSolutionChemistry_GetInput(CSC,U) \
        CementSolutionChemistry_GetPrimaryVariable(CSC)[CementSolutionChemistry_InputIndex(U)]

#define CementSolutionChemistry_SetIndex(CSC,U) \
        do{ \
          CementSolutionChemistry_GetIndex(CSC,U) = CementSolutionChemistry_Index(U) ; \
        } while(0)
        
#define CementSolutionChemistry_SetInput(CSC,U,...) \
        do{ \
          CementSolutionChemistry_GetIndex(CSC,U) = CementSolutionChemistry_Index(U) ; \
          CementSolutionChemistry_GetInput(CSC,U) = __VA_ARGS__ ; \
        } while(0)
        
#define CementSolutionChemistry_InputCaOIs(CSC,V) \
        CementSolutionChemistry_InputIs(CSC,CaO,V)
        
#define CementSolutionChemistry_InputSO3Is(CSC,V) \
        CementSolutionChemistry_InputIs(CSC,SO3,V)

#define CementSolutionChemistry_InputAl2O3Is(CSC,V) \
        CementSolutionChemistry_InputIs(CSC,Al2O3,V)

        
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
#define CementSolutionChemistry_NbOfSpecies  (84)


/* Macros for the solution species (source CEMDATA18)
 * --------------------------------------------------*/

/* Hydrogen-Oxygen
 * ---------------*/
#define CementSolutionChemistry_H2O         (0)
#define CementSolutionChemistry_H           (1)
#define CementSolutionChemistry_OH          (2)
#define CementSolutionChemistry_H2          (3)
#define CementSolutionChemistry_O2          (4)

/* Compound of type I
 * ------------------*/
/* Aluminium: */
#define CementSolutionChemistry_Al          (5)
#define CementSolutionChemistry_AlO4H4      (6)
#define CementSolutionChemistry_AlO2        (6) // synonym
#define CementSolutionChemistry_AlO         (7)
#define CementSolutionChemistry_AlOH        (8)
#define CementSolutionChemistry_AlO2H       (9)
/* Calcium: */
#define CementSolutionChemistry_Ca          (10)
#define CementSolutionChemistry_CaOH        (11)
//#define CementSolutionChemistry_CaO2H2      (5) // To be removed!
/* Carbon: */
//#define CementSolutionChemistry_H2CO3       (15) // To be removed because small compared to CO2
#define CementSolutionChemistry_HCO3        (12)
#define CementSolutionChemistry_CO3         (13)
#define CementSolutionChemistry_CO2         (14)
#define CementSolutionChemistry_CH4         (15)
/* Chlorine: */
#define CementSolutionChemistry_Cl          (16)
#define CementSolutionChemistry_ClO4        (17)
/* Iron: */
#define CementSolutionChemistry_Fe          (18)
#define CementSolutionChemistry_FeO2        (19)
#define CementSolutionChemistry_FeO         (20)
#define CementSolutionChemistry_FeO2H       (21)
#define CementSolutionChemistry_FeOH        (22)
#define CementSolutionChemistry_FeO2H2      (23)
#define CementSolutionChemistry_Fe3O4H4     (24)
#define CementSolutionChemistry_Fe_p3       (25)
/* Magnesium: */
#define CementSolutionChemistry_Mg          (26)
#define CementSolutionChemistry_MgOH        (27)
/* Nitrogen: */
#define CementSolutionChemistry_NO3         (28)
#define CementSolutionChemistry_N2          (29)
#define CementSolutionChemistry_NH3         (30)
#define CementSolutionChemistry_NH4         (31)
/* Potassium: */
#define CementSolutionChemistry_K           (32)
#define CementSolutionChemistry_KOH         (33)
/* Silicon: */
#define CementSolutionChemistry_H2SiO4      (34)
#define CementSolutionChemistry_SiO3        (34) // synonym
#define CementSolutionChemistry_H3SiO4      (35)
#define CementSolutionChemistry_HSiO3       (35) // synonym
#define CementSolutionChemistry_H4SiO4      (36)
#define CementSolutionChemistry_SiO2        (36) // synonym
#define CementSolutionChemistry_Si4O10      (37)
/* Sodium: */
#define CementSolutionChemistry_Na          (38)
#define CementSolutionChemistry_NaOH        (39)
/* Strontium: */
#define CementSolutionChemistry_Sr          (40)
#define CementSolutionChemistry_SrOH        (41)
/* Sulfur: */
#define CementSolutionChemistry_H2SO4       (42) // sulfuric acid (not present in cemdata)
#define CementSolutionChemistry_HSO4        (43)
#define CementSolutionChemistry_SO4         (44)
#define CementSolutionChemistry_S2O3        (45)
#define CementSolutionChemistry_HS          (46)
#define CementSolutionChemistry_S           (47)
#define CementSolutionChemistry_SO3         (48)
#define CementSolutionChemistry_HSO3        (49)

/* Compound of type II
 * -------------------*/
/* Aluminium-Silicon: */
#define CementSolutionChemistry_AlSiO5      (50)
#define CementSolutionChemistry_AlHSiO3     (51)
/* Aluminium-Sulfur: */
#define CementSolutionChemistry_AlSO4       (52)
#define CementSolutionChemistry_AlS2O8      (53)
/* Calcium-Carbon: */
#define CementSolutionChemistry_CaHCO3      (54)
#define CementSolutionChemistry_CaCO3       (55)
/* Calcium-Silicon: */
#define CementSolutionChemistry_CaH2SiO4    (56)
#define CementSolutionChemistry_CaSiO3      (56) // synonym
#define CementSolutionChemistry_CaH3SiO4    (57)
#define CementSolutionChemistry_CaHSiO3     (57) // synonym
/* Calcium-Sulfur: */
//#define CementSolutionChemistry_CaHSO4      (26) // To be removed (not present in cemdata)
#define CementSolutionChemistry_CaSO4       (58)
/* Iron-Carbon: */
#define CementSolutionChemistry_FeCO3       (59)
#define CementSolutionChemistry_FeHCO3      (60)
/* Iron-Chlorine: */
#define CementSolutionChemistry_FeCl        (61)
#define CementSolutionChemistry_FeCl_p2     (62)
#define CementSolutionChemistry_FeCl2       (63)
#define CementSolutionChemistry_FeCl3       (64)
/* Iron-Silicon: */
#define CementSolutionChemistry_FeHSiO3     (65)
/* Iron-Sulfur: */
#define CementSolutionChemistry_FeHSO4      (66)
#define CementSolutionChemistry_FeSO4       (67)
#define CementSolutionChemistry_FeSO4_p1    (68)
#define CementSolutionChemistry_FeS2O8      (69)
/* Magnesium-Carbon: */
#define CementSolutionChemistry_MgCO3       (70)
#define CementSolutionChemistry_MgHCO3      (71)
/* Magnesium-Silicon: */
#define CementSolutionChemistry_MgHSiO3     (72)
#define CementSolutionChemistry_MgSiO3      (73)
/* Magnesium-Sulfur: */
#define CementSolutionChemistry_MgSO4       (74)
/* Potassium-Sulfur: */
#define CementSolutionChemistry_KSO4        (75)
/* Sodium-Carbon: */
#define CementSolutionChemistry_NaHCO3      (76)
#define CementSolutionChemistry_NaCO3       (77)
/* Sodium-Sulfur: */
#define CementSolutionChemistry_NaSO4       (78)
/* Strontium-Carbon: */
#define CementSolutionChemistry_SrCO3       (79)
#define CementSolutionChemistry_SrHCO3      (80)
/* Strontium-Silicon: */
#define CementSolutionChemistry_SrSiO3      (81)
/* Strontium-Sulfur: */
#define CementSolutionChemistry_SrSO4       (82)

/* Compound of type III
 * --------------------*/
/* Sulfur-Carbon-Nitrogen: */
#define CementSolutionChemistry_SCN         (83)











#define CementSolutionChemistry_GetIndexOf(CPD) \
        Utils_CAT(CementSolutionChemistry_,CPD)


#define CementSolutionChemistry_GetConcentrationOf(CSC,CPD) \
       (CementSolutionChemistry_GetConcentration(CSC)[CementSolutionChemistry_GetIndexOf(CPD)])

       
#define CementSolutionChemistry_GetLogConcentrationOf(CSC,CPD) \
       (CementSolutionChemistry_GetLogConcentration(CSC)[CementSolutionChemistry_GetIndexOf(CPD)])


#define CementSolutionChemistry_GetActivityOf(CSC,CPD) \
       (CementSolutionChemistry_GetActivity(CSC)[CementSolutionChemistry_GetIndexOf(CPD)])

       
#define CementSolutionChemistry_GetLogActivityOf(CSC,CPD) \
       (CementSolutionChemistry_GetLogActivity(CSC)[CementSolutionChemistry_GetIndexOf(CPD)])



/* Macros for other variables
 * --------------------------*/
#define CementSolutionChemistry_NbOfOtherVariables       (3)

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



/* Macros for element concentrations
 * ---------------------------------*/
#define CementSolutionChemistry_NbOfElementConcentrations       (12)

#define CementSolutionChemistry_E_Ca          (0)
#define CementSolutionChemistry_E_Si          (1)
#define CementSolutionChemistry_E_Na          (2)
#define CementSolutionChemistry_E_K           (3)
#define CementSolutionChemistry_E_C           (4)
#define CementSolutionChemistry_E_S           (5)
#define CementSolutionChemistry_E_Al          (6)
#define CementSolutionChemistry_E_Cl          (7)
#define CementSolutionChemistry_E_Fe          (8)
#define CementSolutionChemistry_E_Mg          (9)
#define CementSolutionChemistry_E_Ti          (10)
#define CementSolutionChemistry_E_P           (11)

#define CementSolutionChemistry_GetElementConcentrationOf(CSC,A) \
        (CementSolutionChemistry_GetElementConcentration(CSC)[CementSolutionChemistry_E_##A])

#define CementSolutionChemistry_SetElementConcentrationOf(CSC,A,B) \
        do {\
          (CementSolutionChemistry_GetElementConcentration(CSC)[CementSolutionChemistry_E_##A]) = B;\
        } while(0)




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
       (CementSolutionChemistry_ComputeSystem_##SYS(CSC))
       
#define CementSolutionChemistry_SupplementSystemWith(CSC,A) \
       (CementSolutionChemistry_SupplementSystemWith_##A(CSC))



/* Primary template */
template<typename T = double>
struct CementSolutionChemistry_t {
  private:
  Temperature_t* _temperature ;
  int*    _primaryvariableindex ;
  T* _primaryvariable ;
  T* _concentration ;
  T* _logconcentration ;
  T* _activity ;
  T* _logactivity ;
  T* _elementconcentration ;
  T* _othervariable ;
  double* _log10equilibriumconstant ;
  T  _electricpotential ;
  
  public:
  void SetTemperature(Temperature_t* x){_temperature = x;}
  void SetPrimaryVariableIndex(int* x){_primaryvariableindex = x;}
  void SetPrimaryVariable(T* x){_primaryvariable = x;}
  void SetConcentration(T* x){_concentration = x;}
  void SetLogConcentration(T* x){_logconcentration = x;}
  void SetActivity(T* x){_activity = x;}
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
  T* GetLogConcentration(void){return(_logconcentration);}
  T* GetActivity(void){return(_activity);}
  T* GetLogActivity(void){return(_logactivity);}
  T* GetElementConcentration(void){return(_elementconcentration);}
  T* GetOtherVariable(void){return(_othervariable);}
  double* GetLog10Keq(void){return(_log10equilibriumconstant);}
  T& GetElectricPotential(void){return(_electricpotential);}
} ;



inline double* (CementSolutionChemistry_GetValence)(void);
inline double* (CementSolutionChemistry_CreateValence)(void) ;
inline void    (CementSolutionChemistry_InitializeValence)(double*) ;
template<typename T>
inline T       (CementSolutionChemistry_SolvePoly4)(T,T,T,T,T,T,T) ;
template<typename T>
inline void   (CementSolutionChemistry_AllocateMemory)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_UpdateChemicalConstants)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline T (CementSolutionChemistry_ComputeChargeDensity)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline T (CementSolutionChemistry_ComputeLiquidMassDensity)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_UpdateElementConcentrations)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_Initialize)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_TranslateConcentrationsIntoActivities)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_SO3_LogA_H2SO4)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_SO3_LogA_SO4)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_Al2O3_LogQ_AH3)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void (CementSolutionChemistry_SupplementSystemWith_Al2O3_LogA_AlO4H4)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline CementSolutionChemistry_t<T>* (CementSolutionChemistry_Create)(void) ;
template<typename T>
inline void  (CementSolutionChemistry_Delete)(CementSolutionChemistry_t<T>*) ;
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
inline void   (CementSolutionChemistry_PrintChemicalConstants)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline int    (CementSolutionChemistry_SolveElectroneutrality)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline int    (CementSolutionChemistry_SolveExplicitElectroneutrality) (CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_UpdateSolution)(CementSolutionChemistry_t<T>*) ;
template<typename T>
inline void   (CementSolutionChemistry_CopyConcentrations)(CementSolutionChemistry_t<T>*,T*) ;
template<typename T>
inline void   (CementSolutionChemistry_CopyLogConcentrations)(CementSolutionChemistry_t<T>*,T*) ;
template<typename T>
inline void   (CementSolutionChemistry_CopyChemicalPotential)(CementSolutionChemistry_t<T>*,T*) ;

//inline double* instancevalence = NULL ;


#include "Utils.h"

#include "CementSolutionChemistry.h.in"

#endif
