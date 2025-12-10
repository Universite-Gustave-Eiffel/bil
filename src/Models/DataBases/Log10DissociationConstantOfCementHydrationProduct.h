#ifndef LOG10DISSOCIATIONCONSTANTOFCEMENTHYDRATIONPRODUCT_H
#define LOG10DISSOCIATIONCONSTANTOFCEMENTHYDRATIONPRODUCT_H


/*
 * Cement chemistry notation:
 * H  = H2O  ; K  = K2O  ; N  = Na2O  ;
 * C  = CaO  ; S  = SiO2 ; A  = Al2O3 ; 
 * c  = CO2  ; s  = SO3  ; F  = Fe2O3 ;
 * P  = P2O5 ; T  = TiO2 ; M  = MgO   ;
 * ?  = SrO  ;
 */


extern void Log10DissociationConstantOfCementHydrationProduct_Print(double) ;


#define Log10DissociationConstantOfCementHydrationProduct(R,T) \
        (Log10DissociationConstantOfCementHydrationProduct_##R(T))


//#include "Log10DissociationConstantOfCementHydrationProduct_DEFAULT.h.in"
//#include "Log10DissociationConstantOfCementHydrationProduct298_Cemdata18.h.in"
#include "RefThermoDataBases/CEMDATA/Log10DissociationConstantOfCementHydrationProduct_CEMDATA18.h.in"


#if 1
/* Reactions written in a different way
 * ==================================== */

#include "RefThermoDataBases/CEMDATA/Log10EquilibriumConstantOfHomogeneousReactionInWater_CEMDATA18.h.in"

#define Log10DissociationConstantOfCementHydrationProduct_CH__Ca_2OH(T) \
        (Log10DissociationConstantOfCementHydrationProduct_Portlandite_2H__Ca_2H2O(T) \
        +2*Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__OH_H(T))

#define Log10DissociationConstantOfCementHydrationProduct_S_2H2O__H4SiO4(T) \
        Log10DissociationConstantOfCementHydrationProduct_AmorSl__SiO2(T)

#define Log10DissociationConstantOfCementHydrationProduct_S__SiO2(T) \
        Log10DissociationConstantOfCementHydrationProduct_AmorSl__SiO2(T)

#define Log10DissociationConstantOfCementHydrationProduct_CSH2__Ca_SO4_2H2O(T) \
        Log10DissociationConstantOfCementHydrationProduct_Gp__Ca_SO4_2H2O(T)

#define Log10DissociationConstantOfCementHydrationProduct_AH3__2Al_6OH(T) \
        (2*Log10DissociationConstantOfCementHydrationProduct_AlOHmic__AlO2_H_H2O(T) \
        +2*Log10EquilibriumConstantOfHomogeneousReactionInWater_AlO2_4H__Al_2H2O(T) \
        +6*Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))
        
#define Log10DissociationConstantOfCementHydrationProduct_AH3_2OH__2AlO4H4(T) \
        (2*Log10DissociationConstantOfCementHydrationProduct_AlOHmic__AlO2_H_H2O(T) \
        -2*Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))

#define Log10DissociationConstantOfCementHydrationProduct_AFm_12H__4Ca_2Al_SO4_18H2O(T) \
        (Log10DissociationConstantOfCementHydrationProduct_monosulphate12_4H__4Ca_SO4_2AlO2_14H2O(T) \
        +2*Log10EquilibriumConstantOfHomogeneousReactionInWater_AlO2_4H__Al_2H2O(T))

#define Log10DissociationConstantOfCementHydrationProduct_AFm__4Ca_2AlO4H4_SO4_4OH_6H2O(T) \
        (Log10DissociationConstantOfCementHydrationProduct_monosulphate12_4H__4Ca_SO4_2AlO2_14H2O(T) \
        +4*Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))
        
#define Log10DissociationConstantOfCementHydrationProduct_AFt_12H__6Ca_2Al_3SO4_38H2O(T) \
        (Log10DissociationConstantOfCementHydrationProduct_ettringite_4H__6Ca_3SO4_2AlO2_34H2O(T) \
        +2*Log10EquilibriumConstantOfHomogeneousReactionInWater_AlO2_4H__Al_2H2O(T))
        
#define Log10DissociationConstantOfCementHydrationProduct_AFt__6Ca_2AlO4H4_3SO4_4OH_26H2O(T) \
        (Log10DissociationConstantOfCementHydrationProduct_ettringite_4H__6Ca_3SO4_2AlO2_34H2O(T) \
        +4*Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))
        
#define Log10DissociationConstantOfCementHydrationProduct_C3AH6_12H__3Ca_2Al_12H2O(T) \
        (Log10DissociationConstantOfCementHydrationProduct_C3AH6_4H__3Ca_2AlO2_8H2O(T) \
        +2*Log10EquilibriumConstantOfHomogeneousReactionInWater_AlO2_4H__Al_2H2O(T))
        
#define Log10DissociationConstantOfCementHydrationProduct_C3AH6__3Ca_2AlO4H4_4OH(T) \
        (Log10DissociationConstantOfCementHydrationProduct_C3AH6_4H__3Ca_2AlO2_8H2O(T) \
        +4*Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))
        
#define Log10DissociationConstantOfCementHydrationProduct_C2AH8__2Ca_2AlO4H4_2OH_3H2O(T) \
        (Log10DissociationConstantOfCementHydrationProduct_C2AH7d5_2H__2Ca_2AlO2_8d5H2O(T) \
        +2*Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))
        
#define Log10DissociationConstantOfCementHydrationProduct_CAH10__Ca_2AlO4H4_6H2O(T) \
        Log10DissociationConstantOfCementHydrationProduct_CAH10__Ca_2AlO2_10H2O(T)
        
#define Log10DissociationConstantOfCementHydrationProduct_FriedelSalt__4Ca_2AlO4H4_2Cl_4OH_4H2O(T) \
        (Log10DissociationConstantOfCementHydrationProduct_C4AClH10_4H__2Cl_4Ca_2AlO2_12H2O(T) \
        +4*Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))
        
#define Log10DissociationConstantOfCementHydrationProduct_KuzelSalt__4Ca_2AlO4H4_Cl_0d5SO4_4OH_6H2O(T) \
        (Log10DissociationConstantOfCementHydrationProduct_C4AsClH12_4H__Cl_4Ca_0d5SO4_2AlO2_14H2O(T) \
        +4*Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O__H_OH(T))
        
#define Log10DissociationConstantOfCementHydrationProduct_Calcite__Ca_CO3(T) \
        Log10DissociationConstantOfCementHydrationProduct_Cal__CO3_Ca(T)
        
#define Log10DissociationConstantOfCementHydrationProduct_Aragonite__Ca_CO3(T) \
        Log10DissociationConstantOfCementHydrationProduct_Arg__CO3_Ca(T)

#endif

#endif
