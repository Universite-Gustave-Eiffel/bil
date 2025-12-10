#ifndef LOG10EQUILIBRIUMCONSTANTOFHOMOGENEOUSREACTIONINWATER_H
#define LOG10EQUILIBRIUMCONSTANTOFHOMOGENEOUSREACTIONINWATER_H


extern void Log10EquilibriumConstantOfHomogeneousReactionInWater_Print(double) ;


#define Log10EquilibriumConstantOfHomogeneousReactionInWater(R,T) \
        (Log10EquilibriumConstantOfHomogeneousReactionInWater_##R(T))


//#include "Log10EquilibriumConstantOfHomogeneousReactionInWater_DEFAULT.h.in"
//#include "Log10EquilibriumConstantOfHomogeneousReactionInWater_Cemdata18.h.in"
#include "RefThermoDataBases/CEMDATA/Log10EquilibriumConstantOfHomogeneousReactionInWater_CEMDATA18.h.in"




#if 1
/* Reactions written in a different way
 * ==================================== */


/* 0. Autoprotolysis of water
 * -------------------------- */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T) \
        Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__OH_H(T)


/* 1. Chemical reactions involving compounds of type I.
 * ---------------------------------------------------- */

/* 1.1 Aluminium compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_AlO4H4__Al_4OH(T) \
        (Log10EquilibriumConstantOfHomogeneousReactionInWater_AlO2_4H__Al_2H2O(T) \
        +4*Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))


/* 1.2 Calcium compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_OH__CaOH(T) \
        (Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_H2O__CaOH_H(T) \
        -Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_CaOH__Ca_OH(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_OH__CaOH(T))

/* 1.3 Carbon compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_H2O__HCO3_OH(T) \
        (Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_H__HCO3(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_HCO3__CO2_OH(T) \
        (Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_2H__CO2_H2O(T) \
        -Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_H__HCO3(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))

#define Log10EquilibriumConstantOfHomogeneousReactionInWater_CO2_H2O__HCO3_H(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_2H__CO2_H2O(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_H__HCO3(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_HCO3__H_CO3(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_H__HCO3(T))
        

/* 1.4 Potassium compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_KOH__K_OH(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O_K__KOH_H(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__OH_H(T))

/* 1.5 Silicon compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_H4SiO4__H3SiO4_H(T) \
        Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O_SiO2__HSiO3_H(T)
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_H4SiO4__H2SiO4_2H(T) \
        Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O_SiO2__SiO3_2H(T)

#define Log10EquilibriumConstantOfHomogeneousReactionInWater_H3SiO4__H2SiO4_H(T) \
        (Log10EquilibriumConstantOfHomogeneousReactionInWater_H4SiO4__H2SiO4_2H(T) \
        -Log10EquilibriumConstantOfHomogeneousReactionInWater_H4SiO4__H3SiO4_H(T))

#define Log10EquilibriumConstantOfHomogeneousReactionInWater_H3SiO4_H2O__H4SiO4_OH(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_H4SiO4__H3SiO4_H(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))

#define Log10EquilibriumConstantOfHomogeneousReactionInWater_H2SiO4_H2O__H3SiO4_OH(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_H3SiO4__H2SiO4_H(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_H4SiO4__H2SiO4_H(T) \
        Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O_SiO2__SiO3_H(T)
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_H3SiO4_OH__H2SiO4_H2O(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_H2SiO4_H2O__H3SiO4_OH(T))

/* 1.6 Sodium compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_NaOH__Na_OH(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_Na_H2O__NaOH_H(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__OH_H(T))

/* 1.7 Sulfur compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_HSO4__SO4_H(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_SO4_H__HSO4(T))


/* 2. Chemical reactions involving compounds of type II.
 * ----------------------------------------------------- */

/* 2.1 Aluminium-Potassium compounds */

/* 2.2 Aluminium-Silicon compounds */

/* 2.3 Aluminium-Sodium compounds */

/* 2.4 Aluminium-Sulfur compounds */

/* 2.5 Calcium-Carbon compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_CaHCO3__Ca_HCO3(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_Ca_H__CaHCO3(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_H__HCO3(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_HCO3__CaHCO3(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CaHCO3__Ca_HCO3(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_CaCO3__Ca_CO3(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_Ca__CaCO3(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_CO3__CaCO3(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CaCO3__Ca_CO3(T))

/* 2.6 Calcium-Silicon compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_CaH3SiO4__Ca_H3SiO4(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_H2O_SiO2__CaHSiO3_H(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O_SiO2__HSiO3_H(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_CaH2SiO4__Ca_H2SiO4(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_H2O_SiO2__CaSiO3_2H(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O_SiO2__SiO3_2H(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_H3SiO4__CaH3SiO4(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CaH3SiO4__Ca_H3SiO4(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_H2SiO4__CaH2SiO4(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CaH2SiO4__Ca_H2SiO4(T))

/* 2.7 Calcium-Sulfur compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_CaSO4__Ca_SO4(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_SO4__CaSO4(T))

/* 2.8 Carbon-Sodium compounds */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_NaCO3__Na_CO3(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_Na__NaCO3(T))
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_NaHCO3__Na_HCO3(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_Na_H__NaHCO3(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_H__HCO3(T))

/* 2.9 Potassium-Sulfur compounds */

/* 2.10 Sodium-Sulfur compounds */




 
/* Reactions involving additional species
 * -------------------------------------- */

/* H2CO3[0] (carbonic acid) */
#if 0
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_H2CO3__CO2_H2O(T) \
        (2.77)
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_HCO3_H2O__H2CO3_OH(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_H2CO3__CO2_H2O(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_HCO3__CO2_OH(T))
#endif

/* H2SO4[0] (sulfuric acid) */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_SO4_2H__H2SO4(T) \
        (Log10EquilibriumConstantOfHomogeneousReactionInWater_SO4_H__HSO4(T) - 6)
        
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_H2SO4__HSO4_H(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_SO4_2H__H2SO4(T) \
        +Log10EquilibriumConstantOfHomogeneousReactionInWater_SO4_H__HSO4(T))

#if 0
/* Ca(OH)2[0] (Ca(OH)2[0] is removed by setting a high IAP) */
#define Log10EquilibriumConstantOfHomogeneousReactionInWater_CaO2H2__Ca_2OH(T) \
        (9)

#define Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_2OH__CaO2H2(T) \
        (-Log10EquilibriumConstantOfHomogeneousReactionInWater_CaO2H2__Ca_2OH(T))
#endif
#endif



#endif
