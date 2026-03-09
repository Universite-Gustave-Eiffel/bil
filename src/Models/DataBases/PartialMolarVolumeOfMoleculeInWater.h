#ifndef PARTIALMOLARVOLUMEOFMOLECULEINWATER_H
#define PARTIALMOLARVOLUMEOFMOLECULEINWATER_H

#include "InternationalSystemOfUnits.h"

#define PartialMolarVolumeOfMoleculeInWater_Unit \
        (InternationalSystemOfUnits_OneMeter*InternationalSystemOfUnits_OneMeter*InternationalSystemOfUnits_OneMeter/InternationalSystemOfUnits_OneMole)



#define PartialMolarVolumeOfMoleculeInWater(I) \
        (PartialMolarVolumeOfMolecule_##I*PartialMolarVolumeOfMoleculeInWater_Unit)



/* Partial Molar Volume of Molecules in Water (m3/mole) at 25°C
 * ref [1] */
/* After Lothenbach and Kulik (2001) */


/* Hydrogen-Oxygen
 * --------------- */
#define PartialMolarVolumeOfMolecule_H           (-5.5e-6)
#define PartialMolarVolumeOfMolecule_OH          (-4.71e-6) 
#define PartialMolarVolumeOfMolecule_H2O         (18.e-6)
#define PartialMolarVolumeOfMolecule_H2          (0)
#define PartialMolarVolumeOfMolecule_O2          (0)

/** Compounds of type I
 *  ------------------- */
/* Aluminium */
#define PartialMolarVolumeOfMolecule_Al          (20.e-6)    /* Guess */
#define PartialMolarVolumeOfMolecule_AlO4H4      (42.3e-6)   /* [2] */
#define PartialMolarVolumeOfMolecule_AlO2        (42.3e-6)   /* synonym */
#define PartialMolarVolumeOfMolecule_AlO          (0)
#define PartialMolarVolumeOfMolecule_AlOH          (0)
#define PartialMolarVolumeOfMolecule_AlO2H          (0)
/* Calcium */
#define PartialMolarVolumeOfMolecule_Ca          (-18.44e-6)
#define PartialMolarVolumeOfMolecule_CaOH        (5.76e-6)
#define PartialMolarVolumeOfMolecule_CaO2H2      (26.20e-6)
/* Carbon */
#define PartialMolarVolumeOfMolecule_CO2         (32.81e-6)
#define PartialMolarVolumeOfMolecule_H2CO3       (50.e-6)
#define PartialMolarVolumeOfMolecule_HCO3        (24.21e-6) 
#define PartialMolarVolumeOfMolecule_CO3         (-6.06e-6)
#define PartialMolarVolumeOfMolecule_CH4          (0)
/* Chlorine */
#define PartialMolarVolumeOfMolecule_Cl          (17.34e-6)
#define PartialMolarVolumeOfMolecule_ClO4          (0)
/* Iron */
#define PartialMolarVolumeOfMolecule_Fe          (0)
#define PartialMolarVolumeOfMolecule_FeO2          (0)
#define PartialMolarVolumeOfMolecule_FeO          (0)
#define PartialMolarVolumeOfMolecule_FeO2H          (0)
#define PartialMolarVolumeOfMolecule_FeOH          (0)
#define PartialMolarVolumeOfMolecule_FeOH_p2          (0)
#define PartialMolarVolumeOfMolecule_Fe3O4H4          (0)
#define PartialMolarVolumeOfMolecule_Fe_p3          (0)
#define PartialMolarVolumeOfMolecule_Fe2O2H2          (0)
/* Magnesium */
#define PartialMolarVolumeOfMolecule_Mg          (0)
#define PartialMolarVolumeOfMolecule_MgOH          (0)
/* Nitrogen */
#define PartialMolarVolumeOfMolecule_NO3          (0)
#define PartialMolarVolumeOfMolecule_N2          (0)
#define PartialMolarVolumeOfMolecule_NH3          (0)
#define PartialMolarVolumeOfMolecule_NH4          (0)
/* Potassium */
#define PartialMolarVolumeOfMolecule_K           (9.06e-6)
#define PartialMolarVolumeOfMolecule_KOH         (3.511e-6)
/* Silicon */
#define PartialMolarVolumeOfMolecule_H4SiO4      (16.06e-6)
#define PartialMolarVolumeOfMolecule_SiO2        (16.06e-6) // synonym
#define PartialMolarVolumeOfMolecule_H3SiO4      (4.53e-6) 
#define PartialMolarVolumeOfMolecule_HSiO3       (4.53e-6) // synonym
#define PartialMolarVolumeOfMolecule_H2SiO4      (34.13e-6)
#define PartialMolarVolumeOfMolecule_SiO3        (34.13e-6) // synonym
#define PartialMolarVolumeOfMolecule_Si4O10      (0)        
/* Sodium */
#define PartialMolarVolumeOfMolecule_Na          (-1.21e-6)
#define PartialMolarVolumeOfMolecule_NaOH        (3.511e-6)
/* Strontium */
#define PartialMolarVolumeOfMolecule_Sr          (0)
#define PartialMolarVolumeOfMolecule_SrOH          (0)
/* Sulfur */
#define PartialMolarVolumeOfMolecule_H2SO4       (50.e-6)
#define PartialMolarVolumeOfMolecule_HSO4        (24.21e-6) 
#define PartialMolarVolumeOfMolecule_SO4         (22.9e-6)
#define PartialMolarVolumeOfMolecule_S2O3          (0)
#define PartialMolarVolumeOfMolecule_S          (0)
#define PartialMolarVolumeOfMolecule_HS          (0)
#define PartialMolarVolumeOfMolecule_SO3          (0)
#define PartialMolarVolumeOfMolecule_HSO3          (0)
#define PartialMolarVolumeOfMolecule_H2S          (0)


/** Compounds of type II 
 *  -------------------- */
/* Aluminium-Silicon */
#define PartialMolarVolumeOfMolecule_AlSiO5          (0)
#define PartialMolarVolumeOfMolecule_AlHSiO3          (0)
/* Aluminium-Sulfur */
#define PartialMolarVolumeOfMolecule_AlSO4          (0)
#define PartialMolarVolumeOfMolecule_AlS2O8          (0)
/* Calcium-Carbon */
#define PartialMolarVolumeOfMolecule_CaCO3       (-15.65e-6)
#define PartialMolarVolumeOfMolecule_CaHCO3      (13.33e-6)
/* Calcium-Silicon */
#define PartialMolarVolumeOfMolecule_CaH2SiO4    (15.69e-6)
#define PartialMolarVolumeOfMolecule_CaH3SiO4    (-6.74e-6)
#define PartialMolarVolumeOfMolecule_CaSiO3          (0)
#define PartialMolarVolumeOfMolecule_CaHSiO3          (0)
/* Calcium-Sulfur */
#define PartialMolarVolumeOfMolecule_CaSO4       (26.20e-6)
#define PartialMolarVolumeOfMolecule_CaHSO4      (27.e-6)
/* Iron-Carbon */
#define PartialMolarVolumeOfMolecule_FeCO3          (0)
#define PartialMolarVolumeOfMolecule_FeHCO3          (0)
/* Iron-Chlorine */
#define PartialMolarVolumeOfMolecule_FeCl          (0)
#define PartialMolarVolumeOfMolecule_FeCl_p2          (0)
#define PartialMolarVolumeOfMolecule_FeCl2          (0)
#define PartialMolarVolumeOfMolecule_FeCl3          (0)
/* Iron-Silicon */
#define PartialMolarVolumeOfMolecule_FeHSiO3          (0)
/* Iron-Sulfur */
#define PartialMolarVolumeOfMolecule_FeHSO4          (0)
#define PartialMolarVolumeOfMolecule_FeSO4          (0)
#define PartialMolarVolumeOfMolecule_FeSO4_p1          (0)
#define PartialMolarVolumeOfMolecule_FeS2O8          (0)
#define PartialMolarVolumeOfMolecule_FeHSO4_p2          (0)
/* Magnesium-Carbon */
#define PartialMolarVolumeOfMolecule_MgCO3          (0)
#define PartialMolarVolumeOfMolecule_MgHCO3          (0)
/* Magnesium-Silicon */
#define PartialMolarVolumeOfMolecule_MgHSiO3          (0)
#define PartialMolarVolumeOfMolecule_MgSiO3          (0)
/* Magnesium-Sulfur */
#define PartialMolarVolumeOfMolecule_MgSO4          (0)
/* Nitrogen-Carbon */
#define PartialMolarVolumeOfMolecule_HCN          (0)
/* Potassium-Sulfur */
#define PartialMolarVolumeOfMolecule_KSO4          (0)
/* Sodium-Carbon */
#define PartialMolarVolumeOfMolecule_NaCO3       (-0.42e-6)
#define PartialMolarVolumeOfMolecule_NaHCO3      (32.32e-6)
/* Sodium-Sulfur */
#define PartialMolarVolumeOfMolecule_NaSO4          (0)
/* Strontium-Carbon */
#define PartialMolarVolumeOfMolecule_SrCO3          (0)
#define PartialMolarVolumeOfMolecule_SrHCO3          (0)
/* Strontium-Silicon */
#define PartialMolarVolumeOfMolecule_SrSiO3          (0)
/* Strontium-Sulfur */
#define PartialMolarVolumeOfMolecule_SrSO4          (0)

/** Compounds of type III 
 *  --------------------- */
 /* Sulfur-Carbon-Nitrogen */
#define PartialMolarVolumeOfMolecule_SCN          (0)

/*
 * [1] Y. Marcus. The Standard Partial Molar Volumes of Ions in Solution. Part 4. Ionic Volumes in Water at
0-100 °C, J. Phys. Chem. B 2009, 113, 10285-10291.
 * [2] B. Sanjuan, G. Michard. Determination of the partial molar volume of aluminate ion (Al(OH)4-) at infinite dilution in water at 25.degree.C, J. Chem. Eng. Data, 1988, 33 (2), pp 78-80, DOI: 10.1021/je00052a003.
 * 
 */


#endif
