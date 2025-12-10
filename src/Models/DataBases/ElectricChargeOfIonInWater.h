#ifndef ELECTRICCHARGEOFIONSINWATER_H
#define ELECTRICCHARGEOFIONSINWATER_H


#define ElectricChargeOfIonInWater(I)      ElectricChargeOfIonInWater_##I

/* Electric Charge of Ions in Water */

/* Compound of type I
 * ------------------*/
/* Hydrogen-Oxygen */
#define ElectricChargeOfIonInWater_H2O         (0)
#define ElectricChargeOfIonInWater_H           (+1)
#define ElectricChargeOfIonInWater_OH          (-1)
#define ElectricChargeOfIonInWater_H2          (0)
#define ElectricChargeOfIonInWater_O2          (0)
/* Aluminium */
#define ElectricChargeOfIonInWater_Al          (+3)
#define ElectricChargeOfIonInWater_AlO4H4      (-1)
#define ElectricChargeOfIonInWater_AlO2        (-1)
#define ElectricChargeOfIonInWater_AlO         (+1)
#define ElectricChargeOfIonInWater_AlOH        (+2)
#define ElectricChargeOfIonInWater_AlO2H       (0)
/* Calcium  */
#define ElectricChargeOfIonInWater_Ca          (+2)
#define ElectricChargeOfIonInWater_CaOH        (+1)
#define ElectricChargeOfIonInWater_CaO2H2      (0)
/* Carbon */
#define ElectricChargeOfIonInWater_CO2         (0)
#define ElectricChargeOfIonInWater_H2CO3       (0)
#define ElectricChargeOfIonInWater_HCO3        (-1)
#define ElectricChargeOfIonInWater_CO3         (-2)
#define ElectricChargeOfIonInWater_CH4         (0)
/* Chlorine */
#define ElectricChargeOfIonInWater_Cl          (-1)
#define ElectricChargeOfIonInWater_ClO4        (-1)
/* Iron */
#define ElectricChargeOfIonInWater_Fe          (+2)
#define ElectricChargeOfIonInWater_Fe_p3       (+3)
#define ElectricChargeOfIonInWater_FeO         (+1)
#define ElectricChargeOfIonInWater_FeO2        (-1)
#define ElectricChargeOfIonInWater_FeOH        (+1)
#define ElectricChargeOfIonInWater_FeO2H       (0)
#define ElectricChargeOfIonInWater_FeO2H2      (+2)
#define ElectricChargeOfIonInWater_Fe3O4H4     (+5)
/* Magnesium */
#define ElectricChargeOfIonInWater_Mg          (+2)
#define ElectricChargeOfIonInWater_MgOH        (+1)
/* Nitrogen */
#define ElectricChargeOfIonInWater_N2          (0)
#define ElectricChargeOfIonInWater_NH3         (0)
#define ElectricChargeOfIonInWater_NH4         (+1)
#define ElectricChargeOfIonInWater_NO3         (-1)
/* Potassium */
#define ElectricChargeOfIonInWater_K           (+1)
#define ElectricChargeOfIonInWater_KOH         (0)
/* Silicon */
#define ElectricChargeOfIonInWater_H2SiO4      (-2)
#define ElectricChargeOfIonInWater_H3SiO4      (-1)
#define ElectricChargeOfIonInWater_H4SiO4      (0)
#define ElectricChargeOfIonInWater_SiO2        (0)
#define ElectricChargeOfIonInWater_SiO3        (-2)
#define ElectricChargeOfIonInWater_HSiO3       (-1)
#define ElectricChargeOfIonInWater_Si4O10      (-4)
/* Sodium */
#define ElectricChargeOfIonInWater_Na          (+1)
#define ElectricChargeOfIonInWater_NaOH        (0)
/* Strontium */
#define ElectricChargeOfIonInWater_Sr          (+2)
#define ElectricChargeOfIonInWater_SrOH        (+1)
/* Sulfur */
#define ElectricChargeOfIonInWater_S           (-2)
#define ElectricChargeOfIonInWater_SO4         (-2)
#define ElectricChargeOfIonInWater_SO3         (-2)
#define ElectricChargeOfIonInWater_HS          (-1)
#define ElectricChargeOfIonInWater_HSO4        (-1)
#define ElectricChargeOfIonInWater_HSO3        (-1)
#define ElectricChargeOfIonInWater_S2O3        (-2)
#define ElectricChargeOfIonInWater_H2SO4       (0)


/* Compound of type II
 * -------------------*/
 /* Aluminium-Silicon */
#define ElectricChargeOfIonInWater_AlSiO5      (-3)
#define ElectricChargeOfIonInWater_AlHSiO3     (+2)
 /* Aluminium-Sulfur */
#define ElectricChargeOfIonInWater_AlSO4       (+1)
#define ElectricChargeOfIonInWater_AlS2O8      (-1)
/* Calcium-Carbon */
#define ElectricChargeOfIonInWater_CaHCO3      (+1)
#define ElectricChargeOfIonInWater_CaCO3       (0)
/* Calcium-Silicon */
#define ElectricChargeOfIonInWater_CaH3SiO4    (+1)
#define ElectricChargeOfIonInWater_CaH2SiO4    (0)
#define ElectricChargeOfIonInWater_CaSiO3      (0)
#define ElectricChargeOfIonInWater_CaHSiO3     (+1)
/* Calcium-Sulfur */
#define ElectricChargeOfIonInWater_CaHS        (+1)
#define ElectricChargeOfIonInWater_CaHSO4      (+1)
#define ElectricChargeOfIonInWater_CaSO4       (0)
/* Iron-Carbon */
#define ElectricChargeOfIonInWater_FeCO3       (0)
#define ElectricChargeOfIonInWater_FeHCO3      (+1)
/* Iron-Chlorine */
#define ElectricChargeOfIonInWater_FeCl        (+1)
#define ElectricChargeOfIonInWater_FeCl_p2     (+2)
#define ElectricChargeOfIonInWater_FeCl2       (+1)
#define ElectricChargeOfIonInWater_FeCl3       (0)
/* Iron-Silicon */
#define ElectricChargeOfIonInWater_FeHSiO3     (+2)
/* Iron-Sulfur */
#define ElectricChargeOfIonInWater_FeSO4       (0)
#define ElectricChargeOfIonInWater_FeSO4_p1    (+1)
#define ElectricChargeOfIonInWater_FeHSO4      (+1)
#define ElectricChargeOfIonInWater_FeS2O8      (-1)
/* Magnesium-Carbon */
#define ElectricChargeOfIonInWater_MgCO3       (0)
#define ElectricChargeOfIonInWater_MgHCO3      (-1)
/* Magnesium-Silicon */
#define ElectricChargeOfIonInWater_MgSiO3      (0)
#define ElectricChargeOfIonInWater_MgHSiO3     (+1)
/* Magnesium-Sulfur */
#define ElectricChargeOfIonInWater_MgSO4       (0)
/* Potassium-Sulfur */
#define ElectricChargeOfIonInWater_KSO4        (-1)
/* Sodium-Carbon */
#define ElectricChargeOfIonInWater_NaCO3       (-1)
#define ElectricChargeOfIonInWater_NaHCO3      (0)
/* Sodium-Sulfur */
#define ElectricChargeOfIonInWater_NaSO4       (-1)
/* Strontium-Carbon */
#define ElectricChargeOfIonInWater_SrCO3       (0)
#define ElectricChargeOfIonInWater_SrHCO3      (+1)
/* Strontium-Silicon */
#define ElectricChargeOfIonInWater_SrSiO3      (0)
/* Strontium-Sulfur */
#define ElectricChargeOfIonInWater_SrSO4       (0)

/* Compound of type III
 * --------------------*/
/* Sulfur-Carbon-Nitrogen */
#define ElectricChargeOfIonInWater_SCN         (-1)

#endif
