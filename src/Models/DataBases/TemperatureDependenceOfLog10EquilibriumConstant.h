#ifndef TEMPERATUREDEPENDENCEOFLOG10EQUILIBRIUMCONSTANT_H
#define TEMPERATUREDEPENDENCEOFLOG10EQUILIBRIUMCONSTANT_H

/* Refs. 
 * W. Hummel, U. Berner, E. Curti, F.J. Pearson, T. Thoenen
 * Nagra/PSI, Chemical Thermodynamic Data Base 01/01, Technical Report 02-16, 2002.
* */

#include "Utils.h"


#define TemperatureDependenceOfLog10EquilibriumConstant(...) \
        Utils_CAT_NARG(TemperatureDependenceOfLog10EquilibriumConstant,__VA_ARGS__)(__VA_ARGS__)

#define TemperatureDependenceOfLog10EquilibriumConstant_293(...) \
        Utils_CAT(Utils_CAT_NARG(TemperatureDependenceOfLog10EquilibriumConstant,__VA_ARGS__),_293)(__VA_ARGS__)



/* Implementation */

#include <math.h>
#include "PhysicalConstant.h"

/*
 * The Gibbs free energy change of reaction, 
 * 
 *       DrG = SUM_i (nu_i * mu_i) 
 * 
 * is equal to 0 at equilibrium: DrG = 0. The ln(K) is linked to the Gibbs free
 * energy change of the reaction in standard state, DrG0 = SUM_i (nu_i * mu0_i):
 * 
 *       DrG0 = - RT * ln(K)
 * 
 * which itself is linked to the formation Gibbs free energy: DrG = SUM_i nu_i DfG(i).
 * 
 * We compute DrG0 through the followings steps
 * 
 *       DrG(T) = DrH(T) - T * DrS(T)
 *       DrH(T) = DrH(T0) + Int_T0^T DrCp(T) dT
 *       DrS(T) = DrS(T0) + Int_T0^T DrCp(T)/T dT
 * 
 * and so
 * 
 * DrG(T) = DrG(T0) - (T - T0) * DrS(T0) + (Int_T0^T DrCp(T) dT) - T * (Int_T0^T DrCp(T)/T dT)
 * 
 * ie.
 * 
 * ln(K) = ln(K0) - DrH(T0) / R * (1/T - 1/T0) - (1/RT) * (Int_T0^T DrCp(T) dT) + (1/R) * (Int_T0^T DrCp(T)/T dT)
 * 
 * 
 * Three-term temperature extrapolation
 * ------------------------------------
 * This corresponds with a constant heat capacity of reaction DrCp(T) = DrCp. Then
 * 
 * ln(K) = ln(K0) - (DrH(T0)/R) * (1/T - 1/T0) + (DrCp/R) * ( T0/T - 1 + ln(T/T0) )
 * 
 * 
 * So with the approximation: log(K) = A1 + A2 * T + A3 / T + A4 * log(T) + A5 / (T*T)
 * 
 * A1 = + (1/(ln(10) * R)) * ( (DrS(T0)) - (DrCp) * ( 1 + ln(T0) )
 * A2 = 0
 * A3 = - (1/(ln(10) * R)) * ( (DrH(T0)) - (DrCp) * T0 )
 * A4 = + (1/(R)) * (DrCp)
 * A5 = 0
 */


/* Full expression */
#define TemperatureDependenceOfLog10EquilibriumConstant6(T,a,b,c,d,e) \
        ((a) + (b)*((T)) + (c)*(1/(T)) + (d)*log10((T)) + (e)*(1/((T)*(T))))


#define TemperatureDependenceOfLog10EquilibriumConstant6_293(T,a0,b,c,d,e) \
        ((a0) + (b)*((T) - 293.) + (c)*(1/(T) - 1/293.) + (d)*log10((T)/293.) + (e)*(1/((T)*(T)) - 1/(85849.)))
        

/* Three-term approximation */
#define TemperatureDependenceOfLog10EquilibriumConstant4(T,DrH,DrS,DrCp) \
        TemperatureDependenceOfLog10EquilibriumConstant6(T, \
        TemperatureDependenceOfLog10EquilibriumConstant_A1(DrH,DrS,DrCp), 0, \
        TemperatureDependenceOfLog10EquilibriumConstant_A3(DrH,DrS,DrCp), \
        TemperatureDependenceOfLog10EquilibriumConstant_A4(DrH,DrS,DrCp), 0)
        

#define TemperatureDependenceOfLog10EquilibriumConstant_A1(DrH,DrS,DrCp) \
        (( (DrS) - (DrCp) * (1 + log(298.15)))/(log(10.) * PhysicalConstant(PerfectGasConstant)))
       
#define TemperatureDependenceOfLog10EquilibriumConstant_A3(DrH,DrS,DrCp) \
        ((-(DrH) + (DrCp) * (298.15))/(log(10.) * PhysicalConstant(PerfectGasConstant)))

#define TemperatureDependenceOfLog10EquilibriumConstant_A4(DrH,DrS,DrCp) \
        ((DrCp)/(PhysicalConstant(PerfectGasConstant)))
        
        


/*
#define TemperatureDependenceOfLog10EquilibriumConstant_293(T,a0,b,c,d,e) \
        ((a0) + \
         TemperatureDependenceOfLog10EquilibriumConstant(T   ,0,b,c,d,e) - \
         TemperatureDependenceOfLog10EquilibriumConstant(293.,0,b,c,d,e))
*/
#endif
