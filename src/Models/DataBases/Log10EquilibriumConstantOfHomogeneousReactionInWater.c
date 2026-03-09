#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "Exception.h"
#include "Log10EquilibriumConstantOfHomogeneousReactionInWater.h"



#define LogKr(R) Log10EquilibriumConstantOfHomogeneousReactionInWater(R,T)


void Log10EquilibriumConstantOfHomogeneousReactionInWater_PrintCEMDATA(double T)
{
#define REACTITLE(...) \
  {\
    int c1 = 54 ;\
    int c2 = c1+11 ;\
    int n = printf(__VA_ARGS__) ;\
    while(n < c1) n += printf(" ") ;\
    n += printf("|  Log(k)") ;\
    while(n < c2) n += printf(" ") ;\
    printf("\n") ;\
    n = 0 ;\
    while(n < c1) n += printf("-") ;\
    n += printf("|") ;\
    while(n < c2) n += printf("-") ;\
    printf("\n") ;\
  }

#define REACSUBTITLE(R) \
  do {\
    int c1 = 54 ;\
    int c2 = c1+11 ;\
    int n = printf(R) ;\
    while(n < c1) n += printf(" ") ;\
    n += printf("|") ;\
    printf("\n") ;\
  } while(0)

#define PREACT(R,LogK) \
  {\
    double logk = LogK ;\
    int c1 = 54 ;\
    int c2 = c1+11 ;\
    int n = printf(R) ;\
    while(n < c1) n += printf(" ") ;\
    n += printf("| % 3.1f",logk) ;\
    while(n < c2) n += printf(" ") ;\
    printf("\n") ;\
  }
  
#define XREACTISDEF(...) 0 __VA_OPT__(1) // only from C++20
#define REACTISDEF(R)  XREACTISDEF(LogKr(R))

  

  printf("\n") ;
  
  REACTITLE("Homogeneous reaction at T = %g (CEMDATA)",T) ;
  
  REACSUBTITLE("Oxygen") ;
  REACSUBTITLE("------") ;
  PREACT("H2O          = H[+] + OH[-]",LogKr(H2O__H_OH)) ;
  PREACT("2H2O         = O2 + 4e[-] + 4H[+]",LogKr(2H2O__O2_4e_4H)) ;
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Hydrogen") ;
  REACSUBTITLE("--------") ;
  PREACT("2e[-] + 2H[+] = H2",LogKr(2e_2H__H2));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Homogeneous reactions involving compounds of type I") ;
  REACSUBTITLE("---------------------------------------------------") ;
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Aluminium compounds:") ;
  PREACT("AlO2[-] + 2H[+] = AlO[+] + H2O",LogKr(AlO2_2H__AlO_H2O));
  PREACT("AlO2[-] + 3H[+] = AlOH[+2] + H2O",LogKr(AlO2_3H__AlOH_H2O));
  PREACT("AlO2[-] + 4H[+] = Al[+3] + 2H2O",LogKr(AlO2_4H__Al_2H2O));
  PREACT("AlO2[-] + H[+]  = AlO2H",LogKr(AlO2_H__AlO2H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Calcium");
  PREACT("Ca[+2] + H2O = CaOH[+] + H[+]",LogKr(Ca_H2O__CaOH_H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Carbon compounds");
  PREACT("CO3[-2] + H[+] = HCO3[-]",LogKr(CO3_H__HCO3));
  PREACT("CO3[-2] + 8e[-] + 10H[+] = CH4 + 3H2O",LogKr(CO3_8e_10H__CH4_3H2O));
  PREACT("CO3[-2] + 2H[+] = CO2 + H2O",LogKr(CO3_2H__CO2_H2O));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Chlorine compounds");
  PREACT("Cl[-] + 4H2O = ClO4[-] + 8e[-] + 8H[+]",LogKr(Cl_4H2O__ClO4_8e_8H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Iron compounds");
  PREACT("H[+] + FeO2[-] = FeO2H",LogKr(H_FeO2__FeO2H));
  PREACT("e[-] + 4H[+] + FeO2[-] = Fe[+2] + 2H2O",LogKr(e_4H_FeO2__Fe_2H2O));
  PREACT("e[-] + 3H[+] + FeO2[-] = FeOH[+] + H2O",LogKr(e_3H_FeO2__FeOH_H2O));
  PREACT("4H[+] + FeO2[-] = Fe[+3] + 2H2O",LogKr(4H_FeO2__Fe_2H2O));
  PREACT("3H[+] + FeO2[-] = FeOH[+2] + H2O",LogKr(3H_FeO2__FeOH_H2O));
  PREACT("2H[+] + FeO2[-] = FeO[+] + H2O",LogKr(2H_FeO2__FeO_H2O));
  PREACT("2FeO2[-] + 6H[+] = Fe2O2H2[+4] + 2H2O",LogKr(2FeO2_6H__Fe2OHOH_2H2O));
  PREACT("3FeO2[-] + 8H[+] = Fe3O4H4[+5] + 2H2O",LogKr(3FeO2_8H__Fe3OHOHOHOH_2H2O));

  REACSUBTITLE(" ") ;
  REACSUBTITLE("Magnesium compounds");
  PREACT("Mg[+2] + H2O = MgOH[+] + H[+]",LogKr(Mg_H2O__MgOH_H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Nitrogen compounds");
  PREACT("NO3[-] + 8e[-] + 9H[+] = NH3 + 3H2O",LogKr(NO3_8e_9H__NH3_3H2O));
  PREACT("NO3[-] + 8e[-] + 10H[+] = NH4[+] + 3H2O",LogKr(NO3_8e_10H__NH4_3H2O));
  PREACT("2NO3[-] + 10e[-] + 12H[+] = N2 + 6H2O",LogKr(2NO3_10e_12H__N2_6H2O));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Potassium compounds");
  PREACT("H2O + K[+] = KOH + H[+]",LogKr(H2O_K__KOH_H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Silicon compounds");
  PREACT("H2O + SiO2 = HSiO3[-] + H[+]",LogKr(H2O_SiO2__HSiO3_H));
  PREACT("H2O + SiO2 = SiO3[-2] + 2H[+]",LogKr(H2O_SiO2__SiO3_2H));
  PREACT("2H2O + 4SiO2 = Si4O10[-4] + 4H[+]",LogKr(2H2O_4SiO2__Si4O10_4H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Sodium compounds");
  PREACT("Na[+] + H2O = NaOH + H[+]",LogKr(Na_H2O__NaOH_H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Strontium compounds");
  PREACT("Sr[+2] + H2O = SrOH[+] + H[+]",LogKr(Sr_H2O__SrOH_H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Sulfur compounds");
  PREACT("SO4[-2] + 2e[-] + 2H[+] = SO3[-2] + H2O",LogKr(SO4_2e_2H__SO3_H2O));
  PREACT("SO4[-2] + H[+] = HSO4[-]",LogKr(SO4_H__HSO4));
  PREACT("SO4[-2] + 2e[-] + 3H[+] = HSO3[-] + H2O",LogKr(SO4_2e_3H__HSO3_H2O));
  PREACT("SO4[-2] + 8e[-] + 9H[+] = HS[-] + 4H2O",LogKr(SO4_8e_9H__HS_4H2O));
  PREACT("SO4[-2] + 8e[-] + 10H[+] = H2S + 4H2O",LogKr(SO4_8e_10H__H2S_4H2O));
  PREACT("2SO4[-2] + 8e[-] + 10H[+] = S2O3[-2] + 5H2O",LogKr(2SO4_8e_10H__S2O3_5H2O));
  PREACT("SO4[-2] + 8e[-] + 8H[+] = S[-2] + 4H2O",LogKr(SO4_8e_8H__S_4H2O));
  PREACT("SO4[-2] + 2H[+] = H2SO4",LogKr(SO4_2H__H2SO4));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Chemical reactions involving compounds of type II");
  REACSUBTITLE("-------------------------------------------------");
  REACSUBTITLE("Aluminium-Silicon: compounds");
  PREACT("AlO2[-] + H2O + SiO2 = AlSiO5[-3] + 2H[+]",LogKr(AlO2_H2O_SiO2__AlSiO5_2H));
  PREACT("AlO2[-] + 3H[+] + SiO2 = AlHSiO3[+2] + H2O",LogKr(AlO2_3H_SiO2__AlHSiO3_H2O));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Aluminium-Sulfur: compounds");
  PREACT("SO4[-2] + AlO2[-] + 4H[+] = AlSO4[+] + 2H2O",LogKr(SO4_AlO2_4H__AlSO4_2H2O));
  PREACT("2SO4[-2] + AlO2[-] + 4H[+] = AlS2O8[-] + 2H2O",LogKr(2SO4_AlO2_4H__AlSO4SO4_2H2O));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Calcium-Carbon: compounds");
  PREACT("CO3[-2] + Ca[+2] + H[+] = CaHCO3[+]",LogKr(CO3_Ca_H__CaHCO3));
  PREACT("CO3[-2] + Ca[+2] = CaCO3",LogKr(CO3_Ca__CaCO3));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Calcium-Silicon compounds");
  PREACT("Ca[+2] + H2O + SiO2 = CaSiO3 + 2H[+]",LogKr(Ca_H2O_SiO2__CaSiO3_2H));
  PREACT("Ca[+2] + H2O + SiO2 = CaHSiO3[+] + H[+]",LogKr(Ca_H2O_SiO2__CaHSiO3_H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Calcium-Sulfur: compounds");
  PREACT("Ca[+2] + SO4[-2] = CaSO4[0]",LogKr(Ca_SO4__CaSO4));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Iron-Carbon: compounds");
  PREACT("CO3[-2] + e[-] + 4H[+] + FeO2[-] = FeCO3 + 2H2O",LogKr(CO3_e_4H_FeO2__FeCO3_2H2O));
  PREACT("CO3[-2] + e[-] + 5H[+] + FeO2[-] = FeHCO3[+] + 2H2O",LogKr(CO3_e_5H_FeO2__FeHCO3_2H2O));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Iron-Chlorine: compounds");
  PREACT("3Cl[-] + 4H[+] + FeO2[-] = FeCl3 + 2H2O",LogKr(3Cl_4H_FeO2__FeCl3_2H2O));
  PREACT("Cl[-] + e[-] + 4H[+] + FeO2[-] = FeCl[+] + 2H2O",LogKr(Cl_e_4H_FeO2__FeCl_2H2O));
  PREACT("Cl[-] + 4H[+] + FeO2[-] = FeCl[+2] + 2H2O",LogKr(Cl_4H_FeO2__FeCl_2H2O));
  PREACT("2Cl[-] + 4H[+] + FeO2[-] = FeCl2[+] + 2H2O",LogKr(2Cl_4H_FeO2__FeCl2_2H2O));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Iron-Silicon: compounds");
  PREACT("FeO2[-] + 3H[+] + SiO2 = FeHSiO3[+2] + H2O",LogKr(FeO2_3H_SiO2__FeHSiO3_H2O));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Iron-Sulfur: compounds");
  PREACT("SO4[-2] + e[-] + 5H[+] + FeO2[-] = FeHSO4[+] + 2H2O",LogKr(SO4_e_5H_FeO2__FeHSO4_2H2O));
  PREACT("SO4[-2] + e[-] + 4H[+] + FeO2[-] = FeSO4 + 2H2O",LogKr(SO4_e_4H_FeO2__FeSO4_2H2O));
  PREACT("SO4[-2] + 4H[+] + FeO2[-] = FeSO4[+] + 2H2O",LogKr(SO4_4H_FeO2__FeSO4_2H2O));
  PREACT("SO4[-2] + 5H[+] + FeO2[-] = FeHSO4[+2] + 2H2O",LogKr(SO4_5H_FeO2__FeHSO4_2H2O));
  PREACT("2SO4[-2] + 4H[+] + FeO2[-] = FeS2O8[-] + 2H2O",LogKr(2SO4_4H_FeO2__FeSO4SO4_2H2O));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Magnesium-Carbon: compounds");
  PREACT("CO3[-2] + Mg[+2] + H[+] = MgHCO3[+]",LogKr(CO3_Mg_H__MgHCO3));
  PREACT("CO3[-2] + Mg[+2] = MgCO3",LogKr(CO3_Mg__MgCO3));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Magnesium-Silicon: compounds");
  PREACT("Mg[+2] + H2O + SiO2 = MgHSiO3[+] + H[+]",LogKr(Mg_H2O_SiO2__MgHSiO3_H));
  PREACT("Mg[+2] + H2O + SiO2 = MgSiO3 + 2H[+]",LogKr(Mg_H2O_SiO2__MgSiO3_2H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Magnesium-Sulfur: compounds");
  PREACT("SO4[-2] + Mg[+2] = MgSO4",LogKr(SO4_Mg__MgSO4));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Nitrogen-Carbon compounds");
  PREACT("CO3[-2] + NO3[-] + 10e[-] + 13H[+] = HCN + 6H2O",LogKr(CO3_NO3_10e_13H__HCN_6H2O));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Potassium-Sulfur: compounds");
  PREACT("SO4[-2] + K[+] = KSO4[-]",LogKr(SO4_K__KSO4));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Sodium-Carbon: compounds");
  PREACT("CO3[-2] + Na[+] + H[+] = NaHCO3",LogKr(CO3_Na_H__NaHCO3));
  PREACT("CO3[-2] + Na[+] = NaCO3[-]",LogKr(CO3_Na__NaCO3));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Sodium-Sulfur: compounds");
  PREACT("SO4[-2] + Na[+] = NaSO4[-]",LogKr(SO4_Na__NaSO4));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Strontium-Carbon: compounds");
  PREACT("CO3[-2] + Sr[+2] = SrCO3",LogKr(CO3_Sr__SrCO3));
  PREACT("CO3[-2] + Sr[+2] + H[+] = SrHCO3[+]",LogKr(CO3_Sr_H__SrHCO3));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Strontium-Silicon: compounds");
  PREACT("Sr[+2] + H2O + SiO2 = SrSiO3 + 2H[+]",LogKr(Sr_H2O_SiO2__SrSiO3_2H));
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Strontium-Sulfur: compounds");
  PREACT("SO4[-2] + Sr[+2] = SrSO4",LogKr(SO4_Sr__SrSO4));
  

  REACSUBTITLE(" ") ;
  REACSUBTITLE("Compound of type III");
  REACSUBTITLE("--------------------");
  REACSUBTITLE("Sulfur-Carbon-Nitrogen: compounds");
  PREACT("CO3[-2] + NO3[-] + SO4[-2] + 16e[-] + 20H[+] = SCN[-] + 10H2O",LogKr(CO3_NO3_SO4_16e_20H__SCN_10H2O));
  
  
  
  REACSUBTITLE(" ") ;
  
  fflush(stdout) ;
  
  
#undef XREACTISDEF
#undef REACTISDEF
#undef PREACT
#undef REACTITLE
#undef REACSUBTITLE
}



void Log10EquilibriumConstantOfHomogeneousReactionInWater_Print(double T)
{
#define REACTITLE(...) \
  {\
    int c1 = 54 ;\
    int c2 = c1+11 ;\
    int n = printf(__VA_ARGS__) ;\
    while(n < c1) n += printf(" ") ;\
    n += printf("|  Log(k)") ;\
    while(n < c2) n += printf(" ") ;\
    printf("\n") ;\
    n = 0 ;\
    while(n < c1) n += printf("-") ;\
    n += printf("|") ;\
    while(n < c2) n += printf("-") ;\
    printf("\n") ;\
  }

#define REACSUBTITLE(R) \
  do {\
    int c1 = 54 ;\
    int c2 = c1+11 ;\
    int n = printf(R) ;\
    while(n < c1) n += printf(" ") ;\
    n += printf("|") ;\
    printf("\n") ;\
  } while(0)

#define PREACT(R,LogK) \
  {\
    double logk = LogK ;\
    int c1 = 54 ;\
    int c2 = c1+11 ;\
    int n = printf(R) ;\
    while(n < c1) n += printf(" ") ;\
    n += printf("| % 3.1f",logk) ;\
    while(n < c2) n += printf(" ") ;\
    printf("\n") ;\
  }
  
#define XREACTISDEF(...) 0 __VA_OPT__(1) // only from C++20
#define REACTISDEF(R)  XREACTISDEF(LogKr(R))

  

  printf("\n") ;
  
  REACTITLE("Homogeneous reaction at T = %g",T) ;
  
  
  REACSUBTITLE("Homogeneous reactions involving compounds of type I") ;
  REACSUBTITLE("---------------------------------------------------") ;
  REACSUBTITLE("Water") ;
  REACSUBTITLE("-----") ;
  PREACT("H2O          = H[+] + OH[-]",LogKr(H2O__H_OH)) ;
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Calcium compounds:") ;
  PREACT("CaOH[+]      = Ca[2+] + OH[-]",LogKr(CaOH__Ca_OH)) ;
  //PREACT("Ca(OH)2[0]   = Ca[2+] + 2OH[-]",LogKr(CaO2H2__Ca_2OH)) ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_H2O__CaOH_H
  PREACT("Ca[2+]       = CaOH[+] + H[+] - H2O",LogKr(Ca_H2O__CaOH_H)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Silicon compounds:") ;
  REACSUBTITLE("(below H4SiO4[0] is similar to SiO2[0])") ;
  REACSUBTITLE("(below H3SiO4[-] is similar to HSiO3[-])") ;
  REACSUBTITLE("(below H2SiO4[2-] is similar to SiO3[2-])") ;
  PREACT("H3SiO4[-]    = H4SiO4[0] + OH[-] - H2O",LogKr(H3SiO4_H2O__H4SiO4_OH)) ;
  PREACT("H2SiO4[2-]   = H3SiO4[-] + OH[-] - H2O",LogKr(H2SiO4_H2O__H3SiO4_OH)) ;
  PREACT("H2SiO4[2-]   = H3SiO4[-] - H[+]",LogKr(H2SiO4_H2O__H3SiO4_OH) - LogKr(H2O__H_OH)) ;
  PREACT("H2SiO4[2-]   = H4SiO4[0] - 2H[+]",LogKr(H3SiO4_H2O__H4SiO4_OH) + LogKr(H2SiO4_H2O__H3SiO4_OH) - (2*LogKr(H2O__H_OH))) ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O_SiO2__HSiO3_H
  PREACT("SiO2[0]      = HSiO3[-] + H[+] - H2O",LogKr(H2O_SiO2__HSiO3_H)) ;
  #endif
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O_SiO2__SiO3_2H
  PREACT("SiO2[0]      = SiO3[2-] + 2H[+] - H2O",LogKr(H2O_SiO2__SiO3_2H)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Sodium compounds:") ;
  PREACT("NaOH[0]      = Na[+] + OH[-]",LogKr(NaOH__Na_OH)) ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_Na_H2O__NaOH_H
  PREACT("Na[+]        = NaOH[0] + H[+] - H2O",LogKr(Na_H2O__NaOH_H)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Potassium compounds:") ;
  PREACT("KOH[0]       = K[+] + OH[-]",LogKr(KOH__K_OH)) ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_H2O_K__KOH_H
  PREACT("K[+]         = KOH[0] + H[+] - H2O",LogKr(H2O_K__KOH_H)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Carbon compounds:") ;
  //PREACT("H2CO3[0]     = CO2[0] + H2O",LogKr(H2CO3__CO2_H2O)) ;
  PREACT("HCO3[-]      = CO2[0] + OH[-]",LogKr(HCO3__CO2_OH)) ;
  PREACT("CO3[2-]      = HCO3[-] + OH[-] - H2O",LogKr(CO3_H2O__HCO3_OH)) ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_2H__CO2_H2O
  PREACT("CO3[2-]      = CO2[0] - 2H[+] + H2O",LogKr(CO3_2H__CO2_H2O)) ;
  #endif
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_H__HCO3
  PREACT("CO3[2-]      = HCO3[0] - H[+]",LogKr(CO3_H__HCO3)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Sulfur compounds:") ;
  PREACT("H2SO4[0]     = HSO4[-] + H[+]",LogKr(H2SO4__HSO4_H)) ;
  PREACT("HSO4[-]      = SO4[2-] + H[+]",LogKr(HSO4__SO4_H)) ;
  PREACT("H2SO4[0]     = SO4[2-] - 2OH[-] + 2H2O",LogKr(H2SO4__HSO4_H) + LogKr(HSO4__SO4_H) - 2*LogKr(H2O__H_OH)) ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_SO4_H__HSO4
  PREACT("SO4[2-]      = HSO4[-] - H[+]",LogKr(SO4_H__HSO4)) ;
  #endif
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_SO4_2e_2H__SO3_H2O
  PREACT("SO4[2-]      = SO3[2-] - 2e[-]- 2H[+] + H2O",LogKr(SO4_2e_2H__SO3_H2O)) ;
  #endif
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_SO4_2e_3H__HSO3_H2O
  PREACT("SO4[2-]      = HSO3[-] - 2e[-]- 3H[+] + H2O",LogKr(SO4_2e_3H__HSO3_H2O)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Aluminium compounds:") ;
  PREACT("AlO4H4[-]    = Al[3+] + 4OH[-]",LogKr(AlO4H4__Al_4OH)) ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_AlO2_4H__Al_2H2O
  PREACT("AlO2[-]      = Al[3+] - 4H[+] + 2H2O",LogKr(AlO2_4H__Al_2H2O)) ;
  #endif
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_AlO2_2H__AlO_H2O
  PREACT("AlO2[-]      = AlO[+] - 2H[+] + H2O",LogKr(AlO2_2H__AlO_H2O)) ;
  #endif
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_AlO2_H__AlO2H
  PREACT("AlO2[-]      = AlO2H[0] - H[+]",LogKr(AlO2_H__AlO2H)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  
  REACSUBTITLE("Homogeneous reactions involving compounds of type II") ;
  REACSUBTITLE("----------------------------------------------------") ;
  REACSUBTITLE("Calcium-silicon compounds:") ;
  REACSUBTITLE("(below CaH3SiO4[+] is similar to CaHSiO3[+])") ;
  REACSUBTITLE("(below CaH2SiO4[0] is similar to CaSiO3[0])") ;
  PREACT("CaH3SiO4[+]  = Ca[2+] + H3SiO4[-]",LogKr(CaH3SiO4__Ca_H3SiO4)) ;
  PREACT("CaH2SiO4[0]  = Ca[2+] + H2SiO4[2-]",LogKr(CaH2SiO4__Ca_H2SiO4)) ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_H2O_SiO2__CaHSiO3_H
  PREACT("CaHSiO3[+]   = Ca[2+] + SiO2[0] - H[+] + H2O",-LogKr(Ca_H2O_SiO2__CaHSiO3_H)) ;
  #endif
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_H2O_SiO2__CaSiO3_2H
  PREACT("CaSiO3[0]    = Ca[2+] + SiO2[0] - 2H[+] + H2O",-LogKr(Ca_H2O_SiO2__CaSiO3_2H)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Calcium-carbon compounds:") ;
  PREACT("CaHCO3[+]    = Ca[2+] + HCO3[-]",LogKr(CaHCO3__Ca_HCO3)) ;
  PREACT("CaCO3[0]     = Ca[2+] + CO3[2-]",LogKr(CaCO3__Ca_CO3)) ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_CO3_Ca_H__CaHCO3
  PREACT("CaHCO3[+]    = Ca[2+] + CO3[2-] + H[+]",-LogKr(CO3_Ca_H__CaHCO3)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Calcium-sulfur compounds:") ;
  //PREACT("CaHSO4[+]    = Ca[2+] + HSO4[-]",LogKr(CaHSO4__Ca_HSO4)) ;
  PREACT("CaSO4[0]     = Ca[2+] + SO4[2-]",LogKr(CaSO4__Ca_SO4)) ;
  //#ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_Ca_SO4_H__CaHSO4
  //PREACT("CaHSO4[+]    = Ca[2+] + SO4[2-] + H[+]",-LogKr(Ca_SO4_H__CaHSO4)) ;
  //#endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Sodium-carbon compounds:") ;
  PREACT("NaHCO3[0]    = Na[+] + HCO3[-]",LogKr(NaHCO3__Na_HCO3)) ;
  PREACT("NaCO3[-]     = Na[+] + CO3[2-]",LogKr(NaCO3__Na_CO3)) ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_Na_CO2_H__NaHCO3
  PREACT("NaHCO3[0]    = Na[+] + CO3[2-] + H[+]",-LogKr(Na_CO2_H__NaHCO3)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Sodium-sulfur compounds:") ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_SO4_Na__NaSO4
  PREACT("NaSO4[-]     = Na[+] + SO4[2-]",-LogKr(SO4_Na__NaSO4)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  REACSUBTITLE("Potassium-sulfur compounds:") ;
  #ifdef Log10EquilibriumConstantOfHomogeneousReactionInWater_SO4_K__KSO4
  PREACT("KSO4[-]     = K[+] + SO4[2-]",-LogKr(SO4_K__KSO4)) ;
  #endif
  
  REACSUBTITLE(" ") ;
  
  fflush(stdout) ;
  
  
#undef XREACTISDEF
#undef REACTISDEF
#undef PREACT
#undef REACTITLE
#undef REACSUBTITLE
}
