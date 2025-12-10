#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "Exception.h"
#include "Log10EquilibriumConstantOfHomogeneousReactionInWater.h"



#define LogKr(R) Log10EquilibriumConstantOfHomogeneousReactionInWater(R,T)


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
    n += printf("| % g",logk) ;\
    while(n < c2) n += printf(" ") ;\
    printf("\n") ;\
  }
  
#define XREACTISDEF(...) 0 __VA_OPT__(1) // only from C++20
#define REACTISDEF(R)  XREACTISDEF(LogKr(R))

  

  printf("\n") ;
  
  REACTITLE("Homogeneous reaction at T = %g",T) ;
  
  REACSUBTITLE("Homogeneous reactions involving compounds of type I") ;
  REACSUBTITLE("---------------------------------------------------") ;
  REACSUBTITLE("Water:") ;
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
  
  
#undef PREACT
#undef REACTITLE
#undef REACSUBTITLE
}
