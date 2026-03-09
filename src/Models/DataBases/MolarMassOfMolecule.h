#ifndef MOLARMASSOFMOLECULE_H
#define MOLARMASSOFMOLECULE_H

#include "InternationalSystemOfUnits.h"

#define MolarMassOfMolecule_Unit \
       (InternationalSystemOfUnits_OneKilogram/InternationalSystemOfUnits_OneMole)



#define MolarMassOfMolecule(I) \
       (MolarMassOfMolecule_##I*MolarMassOfMolecule_Unit)



/* Molar Mass of Chemicals (kg/mol) */

/* Hydrogen-Oxygen
 * --------------- */
#define MolarMassOfMolecule_H           (1.e-3)
#define MolarMassOfMolecule_O           (16.e-3)
#define MolarMassOfMolecule_OH          (17.e-3)
#define MolarMassOfMolecule_H2O         (18.e-3)
#define MolarMassOfMolecule_H2          (2.e-3)
#define MolarMassOfMolecule_O2          (32.e-3)

/* Compound of type I
 * ------------------ */
/* Aluminium */
#define MolarMassOfMolecule_Al          (26.98e-3)
#define MolarMassOfMolecule_AlO4H4      (95.01e-3)
#define MolarMassOfMolecule_AlO2        (59.01e-3)
#define MolarMassOfMolecule_AlO         (43.e-3)
#define MolarMassOfMolecule_AlOH        (44.e-3)
#define MolarMassOfMolecule_Al2O3       (101.96e-3)
#define MolarMassOfMolecule_AlO2H       (60.e-3)
/* Calcium */
#define MolarMassOfMolecule_Ca          (40.e-3)
#define MolarMassOfMolecule_CaO2H2      (74.e-3)
#define MolarMassOfMolecule_CaO         (56.e-3)
#define MolarMassOfMolecule_CaOH        (57.e-3)
/* Carbon */
#define MolarMassOfMolecule_C           (12.e-3)
#define MolarMassOfMolecule_CO2         (44.e-3)
#define MolarMassOfMolecule_H2CO3       (62.e-3)
#define MolarMassOfMolecule_HCO3        (61.e-3)
#define MolarMassOfMolecule_CO3         (60.e-3)
#define MolarMassOfMolecule_CH4         (16.e-3)
/* Chlorine */
#define MolarMassOfMolecule_Cl          (35.45e-3)
#define MolarMassOfMolecule_ClO4        (99.45e-3)
/* Iron */
#define MolarMassOfMolecule_Fe          (55.85e-3)
#define MolarMassOfMolecule_FeO         (71.85e-3)
#define MolarMassOfMolecule_FeO2        (87.85e-3)
#define MolarMassOfMolecule_FeOH        (72.85e-3)
#define MolarMassOfMolecule_FeO2H       (88.85e-3)
#define MolarMassOfMolecule_FeOH_p2     (72.85e-3)
#define MolarMassOfMolecule_Fe2O3       (159.7e-3)
#define MolarMassOfMolecule_Fe3O4H4     (235.55e-3)
#define MolarMassOfMolecule_Fe_p3       (55.85e-3)
#define MolarMassOfMolecule_Fe2O2H2     (145.7e-3)
/* Magnesium */
#define MolarMassOfMolecule_Mg          (24.3e-3)
#define MolarMassOfMolecule_MgO         (40.3e-3)
#define MolarMassOfMolecule_MgOH        (41.3e-3)
/* Nitrogen */
#define MolarMassOfMolecule_N           (28.e-3)
#define MolarMassOfMolecule_N2          (56.e-3)
#define MolarMassOfMolecule_NO3         (76.e-3)
#define MolarMassOfMolecule_NH3         (31.e-3)
#define MolarMassOfMolecule_NH4         (32.e-3)
/* Phosphorus */
#define MolarMassOfMolecule_P           (30.97e-3)
#define MolarMassOfMolecule_P2O5        (141.93e-3)
/* Potassium */
#define MolarMassOfMolecule_K           (39.e-3)
#define MolarMassOfMolecule_KOH         (56.e-3)
/* Silicon */
#define MolarMassOfMolecule_Si          (28.e-3)
#define MolarMassOfMolecule_SiO2        (60.e-3)
#define MolarMassOfMolecule_H2SiO4      (94.e-3)
#define MolarMassOfMolecule_SiO3        (76.e-3)
#define MolarMassOfMolecule_H3SiO4      (95.e-3)
#define MolarMassOfMolecule_HSiO3       (77.e-3)
#define MolarMassOfMolecule_H4SiO4      (96.e-3)
#define MolarMassOfMolecule_Si4O10      (272.e-3)
/* Sodium */
#define MolarMassOfMolecule_Na          (23.e-3)
#define MolarMassOfMolecule_NaOH        (40.e-3)
/* Strontium */
#define MolarMassOfMolecule_Sr          (87.62e-3)
#define MolarMassOfMolecule_SrOH        (104.62e-3)
/* Sulfur */
#define MolarMassOfMolecule_S           (32.e-3)
#define MolarMassOfMolecule_HS          (33.e-3)
#define MolarMassOfMolecule_H2S         (34.e-3)
#define MolarMassOfMolecule_H2SO4       (98.e-3)
#define MolarMassOfMolecule_HSO4        (97.e-3)
#define MolarMassOfMolecule_SO4         (96.e-3)
#define MolarMassOfMolecule_SO3         (80.e-3)
#define MolarMassOfMolecule_HSO3        (81.e-3)
#define MolarMassOfMolecule_S2O3        (112.e-3)
/* Titanium */
#define MolarMassOfMolecule_Ti          (47.867e-3)
#define MolarMassOfMolecule_TiO         (63.863e-3)

/* Compound of type II
 * ------------------- */
/* Aluminium-Silicon */
#define MolarMassOfMolecule_AlSiO5      (135.e-3)
#define MolarMassOfMolecule_AlHSiO3     (104.e-3)
/* Aluminium-Sulfur */
#define MolarMassOfMolecule_AlSO4       (123.e-3)
#define MolarMassOfMolecule_AlS2O8      (219.e-3)
/* Calcium-Carbon */
#define MolarMassOfMolecule_CaCO3       (100.e-3)
#define MolarMassOfMolecule_CaHCO3      (101.e-3)
/* Calcium-Silicon */
#define MolarMassOfMolecule_CaH2SiO4    (134.e-3)
#define MolarMassOfMolecule_CaH3SiO4    (135.e-3)
#define MolarMassOfMolecule_CaSiO3      (116.e-3)
#define MolarMassOfMolecule_CaHSiO3     (117.e-3)
/* Calcium-Sulfur */
#define MolarMassOfMolecule_CaSO4       (136.e-3)
#define MolarMassOfMolecule_CaHSO4      (137.e-3)
/* Iron-Carbon */
#define MolarMassOfMolecule_FeCO3       (115.85e-3)
#define MolarMassOfMolecule_FeHCO3      (116.85e-3)
/* Iron-Chlorine */
#define MolarMassOfMolecule_FeCl        (91.30e-3)
#define MolarMassOfMolecule_FeCl_p2     (91.30e-3)
#define MolarMassOfMolecule_FeCl2       (126.75e-3)
#define MolarMassOfMolecule_FeCl3       (162.20e-3)
/* Iron-Silicon */
#define MolarMassOfMolecule_FeHSiO3     (132.85e-3)
/* Iron-Sulfur */
#define MolarMassOfMolecule_FeHSO4      (152.85e-3)
#define MolarMassOfMolecule_FeSO4       (151.85e-3)
#define MolarMassOfMolecule_FeSO4_p1    (151.85e-3)
#define MolarMassOfMolecule_FeS2O8      (247.85e-3)
#define MolarMassOfMolecule_FeHSO4_p2   (152.85e-3)
/* Magnesium-Carbon */
#define MolarMassOfMolecule_MgCO3       (84.3e-3)
#define MolarMassOfMolecule_MgHCO3      (85.3e-3)
/* Magnesium-Silicon */
#define MolarMassOfMolecule_MgHSiO3     (101.3e-3)
#define MolarMassOfMolecule_MgSiO3      (100.3e-3)
/* Magnesium-Sulfur */
#define MolarMassOfMolecule_MgSO4       (120.3e-3)
/* Nitrogen-Carbon */
#define MolarMassOfMolecule_HCN         (43.e-3)
/* Potassium-Sulfur */
#define MolarMassOfMolecule_KSO4        (135.e-3)
/* Sodium-Carbon */
#define MolarMassOfMolecule_NaHCO3      (84.e-3)
#define MolarMassOfMolecule_NaCO3       (83.e-3)
/* Sodium-Sulfur */
#define MolarMassOfMolecule_NaSO4       (119.e-3)
/* Strontium-Carbon */
#define MolarMassOfMolecule_SrCO3       (147.62e-3)
#define MolarMassOfMolecule_SrHCO3      (148.62e-3)
/* Strontium-Silicon */
#define MolarMassOfMolecule_SrSiO3      (163.62e-3)
/* Strontium-Sulfur */
#define MolarMassOfMolecule_SrSO4       (183.62e-3)

/* Compound of type III
 * -------------------- */
/* Sulfur-Carbon-Nitrogen */
#define MolarMassOfMolecule_SCN         (72.e-3)
#endif
