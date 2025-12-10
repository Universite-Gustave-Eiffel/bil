#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "Exception.h"
#include "Log10EquilibriumConstantOfHomogeneousReactionInWater.h"
#include "Log10DissociationConstantOfCementHydrationProduct.h"



void Log10DissociationConstantOfCementHydrationProduct_Print(double T)
{
  /* Some other constants */
  
  #define LogKr(R) Log10EquilibriumConstantOfHomogeneousReactionInWater(R,T)
  #undef LogKr
  
#define REACTITLE(...) \
  do {\
    int c1 = 72 ;\
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
  } while(0)

#define REACSUBTITLE(R) \
  do {\
    int c1 = 72 ;\
    int c2 = c1+11 ;\
    int n = printf(R) ;\
    while(n < c1) n += printf(" ") ;\
    n += printf("|") ;\
    printf("\n") ;\
  } while(0)

#define PREACT(R,LogK) \
  do {\
    double logk = LogK ;\
    int c1 = 72 ;\
    int c2 = c1+11 ;\
    int n = printf(R) ;\
    while(n < c1) n += printf(" ") ;\
    n += printf("|") ;\
    n += printf(" % g",logk) ;\
    while(n < c2) n += printf(" ") ;\
    printf("\n") ;\
  } while(0)
  
  printf("\n") ;
  
  
  /* Cement hydrates dissociation reactions */
  {
    #define LogKd(R) \
            Log10DissociationConstantOfCementHydrationProduct(R,T)
  
    REACTITLE("Cement hydrates dissociation reactions at T = %g",T) ;
    REACSUBTITLE("Hydroxides:") ;
    PREACT("CH(s)   =  Ca[2+] + 2OH[-]",LogKd(CH__Ca_2OH)) ;
    PREACT("S(am)   =  SiO2[0]",LogKd(S__SiO2)) ;
    PREACT("AH3(s)  = 2Al[3+] + 6OH[-]",LogKd(AH3__2Al_6OH)) ;
    PREACT("AH3(s)  = 2Al(OH)4[-] - 2OH[-]",LogKd(AH3_2OH__2AlO4H4)) ;
    
    REACSUBTITLE(" ");
    REACSUBTITLE("Sulfates:");
    PREACT("CSH2(s) =  Ca[2+] + SO4[2-] + 2H2O",LogKd(CSH2__Ca_SO4_2H2O)) ;
    
    REACSUBTITLE(" ");
    REACSUBTITLE("AFm phases (C3(A,F).CaX.Hn with X = SO4,CO3,(OH)2,Cl2,...):");
    REACSUBTITLE("X = SO4 -> Monosulfoaluminate:");
    REACSUBTITLE("X = CO3 -> Monocarboaluminate:");
    REACSUBTITLE("X = (OH)2 -> Hydroxy-AFm:");
    REACSUBTITLE("X = Cl2 -> Fridel's salt (C4AClH10):");
    REACSUBTITLE("X = Cl(SO4)0.5 -> Kuzel's salt (C4AsClH12):");
    PREACT("SO4-AFm(s)        = 4Ca[2+] + 2Al[3+] + SO4[2-] - 12H[+] + 18H2O",LogKd(AFm_12H__4Ca_2Al_SO4_18H2O)) ;
    PREACT("SO4-AFm(s)        = 4Ca[2+] + 2Al(OH)4[-] + SO4[2-] + 4OH[-] + 6H2O",LogKd(AFm__4Ca_2AlO4H4_SO4_4OH_6H2O)) ;
    PREACT("Friedel's salt(s) = 4Ca[2+] + 2Al(OH)4[-] + 2Cl[-] + 4OH[-] + 4H2O",LogKd(FriedelSalt__4Ca_2AlO4H4_2Cl_4OH_4H2O)) ;
    PREACT("Kuzel's salt(s)   = Friedel's salt(l) - Cl[-] + 0.5SO4[2-] + 2H2O", LogKd(KuzelSalt__4Ca_2AlO4H4_Cl_0d5SO4_4OH_6H2O)) ;
    
    REACSUBTITLE(" ");
    REACSUBTITLE("AFt phases (C3(A,F).3(CaX).Hn with X = SO4,CO3,(OH)2,Cl2,...):");
    REACSUBTITLE("X = SO4 -> Trisulfoaluminate:");
    PREACT("Al-Ettringite(s) = 6Ca[2+] + 2Al[3+] + 3SO4[2-] - 12H[+] + 38H2O",LogKd(AFt_12H__6Ca_2Al_3SO4_38H2O)) ;
    PREACT("Al-Ettringite(s) = 6Ca[2+] + 2Al(OH)4[-] + 3SO4[2-] + 4OH[-] + 26H2O",LogKd(AFt__6Ca_2AlO4H4_3SO4_4OH_26H2O)) ;
        
    REACSUBTITLE(" ");
    //REACSUBTITLE("Hydrogarnets:");
    REACSUBTITLE("Calcium aluminate hydrates:");
    PREACT("C3AH6(s)  = 3Ca[2+] + 2Al[3+] - 12H[+] + 12H2O",LogKd(C3AH6_12H__3Ca_2Al_12H2O)) ;
    PREACT("C3AH6(s)  = 3Ca[2+] + 2Al(OH)4[-] + 4OH[-]",LogKd(C3AH6__3Ca_2AlO4H4_4OH)) ;
    PREACT("C2AH8(s)  = 2Ca[2+] + 2Al(OH)4[-] + 2OH[-] + 3H2O",LogKd(C2AH8__2Ca_2AlO4H4_2OH_3H2O)) ;
    PREACT("CAH10(s)  =  Ca[2+] + 2Al(OH)4[-] + 6H2O",LogKd(CAH10__Ca_2AlO4H4_6H2O)) ;
        
    REACSUBTITLE(" ");
    REACSUBTITLE("Calcium carbonates:");
    #ifdef Log10DissociationConstantOfCementHydrationProduct_Calcite__Ca_CO3
    PREACT("Calcite(s)    = Ca[2+] + CO3[2-]",LogKd(Calcite__Ca_CO3)) ;
    #endif
    #ifdef Log10DissociationConstantOfCementHydrationProduct_Aragonite__Ca_CO3
    PREACT("Aragonite(s)  = Ca[2+] + CO3[2-]",LogKd(Aragonite__Ca_CO3)) ;
    #endif
        
    #undef LogKd
  }
  
  
  printf("\n") ;
  
  fflush(stdout) ;
  
  
#undef PREACT
#undef REACTITLE
#undef REACSUBTITLE

}
