#ifndef DIFFUSIONCOEFFICIENTOFMOLECULEINWATER_H
#define DIFFUSIONCOEFFICIENTOFMOLECULEINWATER_H



#include "InternationalSystemOfUnits.h"

#define DiffusionCoefficientOfMoleculeInWater_Unit  \
       (InternationalSystemOfUnits_OneMeter*InternationalSystemOfUnits_OneMeter/InternationalSystemOfUnits_OneSecond)


#include "WaterViscosity.h"

/* Water viscosity at T = 298 (Pa.s) */
/* www.engineeringtoolbox.com, equation from chemistry handbook */
#define T298                 (298.)
#define WaterViscosity298    WaterViscosity(T298)   //(8.904695e-04)

#define DiffusionCoefficientOfMoleculeInWater(I,T) \
       (DiffusionCoefficientOfMoleculeInWater_##I*DiffusionCoefficientOfMoleculeInWater_Unit* \
       ((T*WaterViscosity298)/(T298*WaterViscosity(T))))


/* 
 * Useful methods/equations to calculate diffusion coefficients
 * 
 * 1- William A. Tucker and Leslie H. Nelken:
 * 
 * D_water = 13.26e-5 / (h^{1.14} (VB')^{0.589})
 * 
 * h is the viscosity of water
 * VB' is the LaBas estimate of molar volume
 * 
 * These estimation methods were developed by Diffusion Coefficients in Air and Water by William A. Tucker and Leslie H. Nelken, Chapter 17 in Handbook of Chemical Property Estimation Methods, Warren J. Lyman, William F. Reehl and David H. Rosenblatt, editors, American Chemical Society, 1982.
 * 
 * 2- Stokes-Einstein Relation (for large molecules):
 * 
 * D = kT/(6 p h r)
 * 
 * k Boltzman constant (1.38e-23 J/K)
 * h is the viscosity of water
 * r is the radius of sphere (molecule)
 * 
 * 3- Sutherland-Einstein Correlation (for smaller molecules):
 * Modification of Stokes-Einstein which considers sliding friction
 * between solute and solvent.
 * 
 * D = kT/(6 p h r) * (b r + 3h)/(b r + 2h)
 * 
 * 4- Wilke-Chang Correlation (for small molecules):
 * Semi-empirical modification of the Stokes-Einstein relation
 * 10-15% error.
 * Wike C.R., Chang P., Correlation of diffusion coefficients in diulte solutions, A.1.Ch.E. Journal, p 264-270, 1955.
 * 
 * D = 7.4e-8 (xM)^0.5 T / (h V^0.6)
 * 
 * where x = 2.6 (for water)
 * M = Molecular weight of solvent (18 g/mole for water)
 * V = Molal volume of solute (m3/kg)
 */
 
 
/*
 * Molecular Diffusion Coefficient in Water At Temperature T = 298 K (m2/s) 
 * kT/(6 p h)   = 24.5e-20  m3/s at T = 298 K
 * kT/(6 p h r) = 24.5e-10  m2/s for r = 1 Angstrom = 1.e-10 m
 */
 
/* Hydrogen-Oxygen
 * --------------- */
#define DiffusionCoefficientOfMoleculeInWater_H2O          (0)
#define DiffusionCoefficientOfMoleculeInWater_H            (93.11e-10)   /* (g) */
#define DiffusionCoefficientOfMoleculeInWater_OH           (52.73e-10)
#define DiffusionCoefficientOfMoleculeInWater_H2           (51.1e-10)     /* (g) */
#define DiffusionCoefficientOfMoleculeInWater_O2           (24.2e-10)     /* (g)*/
 
/* Compounds of type I
 * ------------------- */
/* Aluminium */
#define DiffusionCoefficientOfMoleculeInWater_Al           (5.41e-10)   /* (g) */
#define DiffusionCoefficientOfMoleculeInWater_AlO4H4       (1.04e-9)    /* (h) */
#define DiffusionCoefficientOfMoleculeInWater_AlO2         (1.04e-9)
#define DiffusionCoefficientOfMoleculeInWater_AlO          (0)
#define DiffusionCoefficientOfMoleculeInWater_AlOH         (0)
#define DiffusionCoefficientOfMoleculeInWater_AlO2H        (0)
/* Calcium */
#define DiffusionCoefficientOfMoleculeInWater_Ca           (7.92e-10)
#define DiffusionCoefficientOfMoleculeInWater_CaOH         (12.25e-10)   /* r = 2 A */
#define DiffusionCoefficientOfMoleculeInWater_CaO2H2       (7.92e-10)
/* Carbon */
#define DiffusionCoefficientOfMoleculeInWater_CO2          (19.1e-10)
#define DiffusionCoefficientOfMoleculeInWater_H2CO3        (7.2e-10)
#define DiffusionCoefficientOfMoleculeInWater_HCO3         (11.85e-10)
#define DiffusionCoefficientOfMoleculeInWater_CO3          (9.55e-10)
#define DiffusionCoefficientOfMoleculeInWater_CH4          (18.4e-10) /* (g) */
/* Chlorine */
#define DiffusionCoefficientOfMoleculeInWater_Cl           (20.32e-10)
#define DiffusionCoefficientOfMoleculeInWater_ClO4         (17.92e-10)
/* Iron */
#define DiffusionCoefficientOfMoleculeInWater_Fe           (7.19e-10)
#define DiffusionCoefficientOfMoleculeInWater_Fe_p3        (6.04e-10)
#define DiffusionCoefficientOfMoleculeInWater_FeO2         (0)
#define DiffusionCoefficientOfMoleculeInWater_FeO          (0)
#define DiffusionCoefficientOfMoleculeInWater_FeO2H        (0)
#define DiffusionCoefficientOfMoleculeInWater_FeOH         (0)
#define DiffusionCoefficientOfMoleculeInWater_FeOH_p2      (0)
#define DiffusionCoefficientOfMoleculeInWater_Fe3O4H4      (0)
#define DiffusionCoefficientOfMoleculeInWater_Fe2O2H2      (0)
/* Magnesium */
#define DiffusionCoefficientOfMoleculeInWater_Mg           (7.06e-10)
#define DiffusionCoefficientOfMoleculeInWater_MgOH         (0)
/* Nitrogen */
#define DiffusionCoefficientOfMoleculeInWater_NO3          (19.02e-10)
#define DiffusionCoefficientOfMoleculeInWater_N2           (20.e-10) /* (g) */
#define DiffusionCoefficientOfMoleculeInWater_NH3          (15.e-10) /* (g) */
#define DiffusionCoefficientOfMoleculeInWater_NH4          (19.57e-10) /* r = 1.36 A */
/* Potassium */
#define DiffusionCoefficientOfMoleculeInWater_K            (19.57e-10)
#define DiffusionCoefficientOfMoleculeInWater_KOH          (19.6e-10)
/* Silicon */
#define DiffusionCoefficientOfMoleculeInWater_H4SiO4       (8.16e-10)    /* r = 3 A (e) */
#define DiffusionCoefficientOfMoleculeInWater_SiO2         (8.16e-10)    /* r = 3 A (e) */
#define DiffusionCoefficientOfMoleculeInWater_H3SiO4       (10.7e-10)
#define DiffusionCoefficientOfMoleculeInWater_HSiO3        (10.7e-10)
#define DiffusionCoefficientOfMoleculeInWater_H2SiO4       (12.25e-10)   /* r = 2 A (e) */
#define DiffusionCoefficientOfMoleculeInWater_SiO3         (12.25e-10)   /* r = 2 A (e) */
#define DiffusionCoefficientOfMoleculeInWater_Si4O10       (0)
/* Sodium */
#define DiffusionCoefficientOfMoleculeInWater_Na           (13.34e-10)  /* (g) */
#define DiffusionCoefficientOfMoleculeInWater_NaOH         (13.34e-10)  /* (g) */
/* Strontium */
#define DiffusionCoefficientOfMoleculeInWater_Sr           (7.9e-10)    /* (i) */
#define DiffusionCoefficientOfMoleculeInWater_SrOH         (0)
/* Sulfur */
#define DiffusionCoefficientOfMoleculeInWater_SO4          (10.6e-10)   /* (a,b) */
#define DiffusionCoefficientOfMoleculeInWater_H2SO4        (1.5e-9)
#define DiffusionCoefficientOfMoleculeInWater_HSO4         (13.85e-10)  /* (g)*/
#define DiffusionCoefficientOfMoleculeInWater_HSO3         (15.45e-10)  /* (g)*/
#define DiffusionCoefficientOfMoleculeInWater_HS           (17.31e-10)  /* (g)*/
#define DiffusionCoefficientOfMoleculeInWater_S            (0)
#define DiffusionCoefficientOfMoleculeInWater_SO3          (9.53e-10)
#define DiffusionCoefficientOfMoleculeInWater_S2O3         (11.32e-10)
#define DiffusionCoefficientOfMoleculeInWater_H2S          (0)
/* Cesium */
#define DiffusionCoefficientOfMoleculeInWater_Cs           (20.5e-10)   /* (d) */


/* Compounds of type II
 * -------------------- */
/* Aluminium-Silicon */
#define DiffusionCoefficientOfMoleculeInWater_AlSiO5        (0)
#define DiffusionCoefficientOfMoleculeInWater_AlHSiO3        (0)
/* Aluminium-Sulfur */
#define DiffusionCoefficientOfMoleculeInWater_AlSO4        (0)
#define DiffusionCoefficientOfMoleculeInWater_AlS2O8        (0)
/* Calcium-Carbon */
#define DiffusionCoefficientOfMoleculeInWater_CaCO3        (14.3e-10)
#define DiffusionCoefficientOfMoleculeInWater_CaHCO3       (9.42e-10)    /* r = 2.6 A */
/* Calcium-Silicon */
#define DiffusionCoefficientOfMoleculeInWater_CaH3SiO4     (12.25e-10)   /* r = 2 A */
#define DiffusionCoefficientOfMoleculeInWater_CaH2SiO4     (12.25e-10)   /* r = 2 A */
#define DiffusionCoefficientOfMoleculeInWater_CaHSiO3      (12.25e-10)
#define DiffusionCoefficientOfMoleculeInWater_CaSiO3       (12.25e-10)
/* Calcium-Sulfur */
#define DiffusionCoefficientOfMoleculeInWater_CaSO4        (1.43e-9)
#define DiffusionCoefficientOfMoleculeInWater_CaHSO4       (1.07e-9)
/* Iron-Carbon */
#define DiffusionCoefficientOfMoleculeInWater_FeCO3        (0)
#define DiffusionCoefficientOfMoleculeInWater_FeHCO3        (0)
/* Iron-Chlorine */
#define DiffusionCoefficientOfMoleculeInWater_FeCl        (0)
#define DiffusionCoefficientOfMoleculeInWater_FeCl_p2        (0)
#define DiffusionCoefficientOfMoleculeInWater_FeCl2        (0)
#define DiffusionCoefficientOfMoleculeInWater_FeCl3        (0)
/* Iron-Silicon */
#define DiffusionCoefficientOfMoleculeInWater_FeHSiO3        (0)
/* Iron-Sulfur */
#define DiffusionCoefficientOfMoleculeInWater_FeHSO4        (0)
#define DiffusionCoefficientOfMoleculeInWater_FeSO4        (0)
#define DiffusionCoefficientOfMoleculeInWater_FeSO4_p1        (0)
#define DiffusionCoefficientOfMoleculeInWater_FeS2O8        (0)
#define DiffusionCoefficientOfMoleculeInWater_FeHSO4_p2        (0)
/* Magnesium-Carbon */
#define DiffusionCoefficientOfMoleculeInWater_MgCO3        (0)
#define DiffusionCoefficientOfMoleculeInWater_MgHCO3        (0)
/* Magnesium-Silicon */
#define DiffusionCoefficientOfMoleculeInWater_MgHSiO3        (0)
#define DiffusionCoefficientOfMoleculeInWater_MgSiO3        (0)
/* Magnesium-Sulfur */
#define DiffusionCoefficientOfMoleculeInWater_MgSO4        (0)
/* Nitrogen-Carbon */
#define DiffusionCoefficientOfMoleculeInWater_HCN        (0)
/* Potassium-Sulfur */
#define DiffusionCoefficientOfMoleculeInWater_KSO4        (0)
/* Sodium-Carbon */
#define DiffusionCoefficientOfMoleculeInWater_NaCO3        (13.3e-10)
#define DiffusionCoefficientOfMoleculeInWater_NaHCO3       (13.3e-10)    /* r = 2.6 A */
/* Sodium-Sulfur */
#define DiffusionCoefficientOfMoleculeInWater_NaSO4        (0)
/* Strontium-Carbon */
#define DiffusionCoefficientOfMoleculeInWater_SrCO3        (0)
#define DiffusionCoefficientOfMoleculeInWater_SrHCO3        (0)
/* Strontium-Silicon */
#define DiffusionCoefficientOfMoleculeInWater_SrSiO3        (0)
/* Strontium-Sulfur */
#define DiffusionCoefficientOfMoleculeInWater_SrSO4        (0)

/* Compounds of type III
 * --------------------- */
/* Sulfur-Carbon-Nitrogen */
#define DiffusionCoefficientOfMoleculeInWater_SCN          (17.58e-10) /* (g) */


/*
 * References:
 * (a) R. Hober (1947) Physical Chemistry of Cells and Tissues, Churchill, London.
 * (b) F. Daniels and R.A. Alberty (1961) Physical Chemistry, Wiley, New-York.
 * (c) R. Weast, M. Astle, and W. Beyer. CRC handbook of chemistry and physics, volume 69. CRC press Boca Raton, FL, 1988. 
 * (d) Li, Y-H., and Gregory, S. (1974). "Diffusion of ions in sea water and in deep-sea sediments." Geochimica et Cosmochimica Acta, 38(5), 703-714.
 * (e) DataBase MINTEQA2
 * (f) DataBase data0 dataset of Wolery et al 
 * (g) D. R. Lide, ed., CRC Handbook of Chemistry and Physics, 90th Edition, CRC Press/Taylor and Francis, Boca Raton, FL, 2009.
 * (h) James E. Mackin and Robert C.Aller (1983), The infinite dilution diffusion coefficient for Al(OH)4 at 25C, Geochimica et Cosmochimica Acta Vol. 47. pp. 959-961.
 * (i) https://www.aqion.de/site/diffusion-coefficients
 */

#endif
