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

#define TemperatureDependenceOfLog10EquilibriumConstant_298(...) \
        Utils_CAT(Utils_CAT_NARG(TemperatureDependenceOfLog10EquilibriumConstant,__VA_ARGS__),_298)(__VA_ARGS__)



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
 * d/dT(R ln(K)) = -d/dT(DrG/T) = DrH/T^2 = DrH(T0)/T^2 + 1/T^2 Int_T0^T DrCp(T) dT
 * 
 * DrG(T) = DrG(T0) - (T - T0) * DrS(T0) + (Int_T0^T DrCp(T) dT) - T * (Int_T0^T DrCp(T)/T dT)
 * 
 * ie.
 * 
 * R ln(K) = R ln(K0) - DrH(T0) * (1/T - 1/T0) 
 *         - (1/T) * (Int_T0^T DrCp(T) dT) + (Int_T0^T DrCp(T)/T dT)
 * 
 * 0. Seven-term temperature approximation: DrCp(T) = a0 + a1*T + a2/T^2 + a3*sqrt(T) + a4*T^2
 * -------------------------------------------------------------------------------------------
 * We find (in PHREEQC A6=A7=0)
 * 
 * log(K) = A1 + A2*T + A3/T + A4*log(T) + A5/T^2 + A6*T^2 + A7*sqrt(T)
 *
 * 1. Five-term approximation: DrCp(T) = a0 + a1 * T + a2 / T^2 (adopted in PHREEQC and GEMS)
 * ------------------------------------------------------------------------------------------
 * [1] Maier, C.G., and Kelly, K.K., 1932, 
 *     An equation for the representation of high temperature heat content data: 
 *     Amer. Chem. Soc. Jour., v. 54, pp. 3243-3246.
 *
 * R ln(K) = R ln(K0) - DrH(T0) *  (1/T - 1/T0) + a0 * ( T0/T - 1 + ln(T/T0) )
 *         + 1/2 * a1 * (T - T0)*(1 - T0/T) + 1/2 * a2 * (1/T - 1/T0)^2
 *
 * i.e.
 
 * log(K) = A1 + A2*T + A3/T + A4*log(T) + A5/T^2
 *
 * 
 * 2. Three-term temperature extrapolation: DrCP(T) = cst (adopted in CEMDATA)
 * ---------------------------------------------------------------------------
 * This corresponds with a constant heat capacity of reaction DrCp(T) = DrCp. Then
 * 
 *       DrG(T) = DrH(T) - T * DrS(T)
 *       DrH(T) = DrH(T0) + DrCp * (T - T0)
 *       DrS(T) = DrS(T0) + DrCp * ln(T/T0)
 * 
 * R ln(K) = R ln(K0) - DrH(T0) * (1/T - 1/T0) + DrCp * ( T0/T - 1 + ln(T/T0) )
 * 
 * 
 * So with the approximation (A2=A5=A6=A7=0): 
 * log(K) = A1 + A3 / T + A4 * log(T) = log(K0) + A3 * (1/T - 1/T0) + A4 * log(T/T0)
 * 
 * A1 = + (1/(ln(10) * R)) * ( (DrS(T0)) - (DrCp) * ( 1 + ln(T0) )
 * A3 = - (1/(ln(10) * R)) * ( (DrH(T0)) - (DrCp) * T0 )
 * A4 = + (1/(R) * (DrCp)
 */


/* Full expression */
#define TemperatureDependenceOfLog10EquilibriumConstant8(T,a,b,c,d,e,f,g) \
        ((a) + (b)*((T)) + (c)*(1/(T)) + (d)*log10((T)) + (e)*(1/((T)*(T))) + (f)*(T)*(T) + (g)*sqrt(T))
        
#define TemperatureDependenceOfLog10EquilibriumConstant6(T,a,b,c,d,e) \
        ((a) + (b)*((T)) + (c)*(1/(T)) + (d)*log10((T)) + (e)*(1/((T)*(T))))

#define TemperatureDependenceOfLog10EquilibriumConstant7(T0,T,logk0,b,c,d,e) \
        ((logk0) + (b)*((T) - (T0)) + (c)*(1/(T) - 1/(T0)) + (d)*log10((T)/(T0)) + (e)*(1/((T)*(T)) - 1/((T0)*(T0))))

#define TemperatureDependenceOfLog10EquilibriumConstant6_293(T,logk0,b,c,d,e) \
        TemperatureDependenceOfLog10EquilibriumConstant7(293.,T,logk0,b,c,d,e)

#define TemperatureDependenceOfLog10EquilibriumConstant6_298(T,logk0,b,c,d,e) \
        TemperatureDependenceOfLog10EquilibriumConstant7(298.,T,logk0,b,c,d,e)


/* Maier-Kelly's expression (for the record) */
#define TemperatureDependenceOfLog10EquilibriumConstant_MaierKelly(T0,T,logk0,DrH,a0,a1,a2) \
        ((logk0) + (-(DrH)*(1/(T) - 1/(T0)) + (a0)*((T0)/(T) - 1 + log((T)/(T0))) +\
        0.5*(a1)*((T0) - (T))*((T0)/(T) - 1) + 0.5*(a2)*(1/(T) - 1/(T0))*(1/(T) - 1/(T0)))\
        /(2.3026*PhysicalConstant(PerfectGasConstant)))
        

/* Three-term approximation */
#define TemperatureDependenceOfLog10EquilibriumConstant_ThreeTerm(T0,T,logk0,DrH,DrCp)\
        ((logk0) + (-(DrH)*(1/(T) - 1/(T0)) + (DrCp)*((T0)/(T) - 1 + log((T)/(T0))))\
        /(2.3026*PhysicalConstant(PerfectGasConstant)))
        
#define TemperatureDependenceOfLog10EquilibriumConstant_ThreeTerm_293(T,logk0,DrH,DrCp) \
        TemperatureDependenceOfLog10EquilibriumConstant_ThreeTerm(293.,T,logk0,DrH,DrCp)
        
#define TemperatureDependenceOfLog10EquilibriumConstant_ThreeTerm_298(T,logk0,DrH,DrCp) \
        TemperatureDependenceOfLog10EquilibriumConstant_ThreeTerm(298.,T,logk0,DrH,DrCp)


#define TemperatureDependenceOfLog10EquilibriumConstant5 \
        TemperatureDependenceOfLog10EquilibriumConstant_ThreeTerm


#define TemperatureDependenceOfLog10EquilibriumConstant4_293 \
        TemperatureDependenceOfLog10EquilibriumConstant_ThreeTerm_293


#define TemperatureDependenceOfLog10EquilibriumConstant4_298 \
        TemperatureDependenceOfLog10EquilibriumConstant_ThreeTerm_298

#endif
