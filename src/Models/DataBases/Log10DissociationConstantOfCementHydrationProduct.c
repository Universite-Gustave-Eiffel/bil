#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "Exception.h"
#include "Log10EquilibriumConstantOfHomogeneousReactionInWater.h"
#include "Log10DissociationConstantOfCementHydrationProduct.h"



void Log10DissociationConstantOfCementHydrationProduct_PrintCEMDATA(double T)
{
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
    n += printf(" % 3.1f",logk) ;\
    while(n < c2) n += printf(" ") ;\
    printf("\n") ;\
  } while(0)
  
  printf("\n") ;
  
  
  /* Cement hydrates dissociation reactions */
  #define LogKd(R)   Log10DissociationConstantOfCementHydrationProduct(R,T)
  {
    REACTITLE("Cement hydrates dissociation reactions at T = %g (CEMDATA)",T) ;
    
    PREACT("5CA: (CaO)1.25(SiO2)1(Al2O3)0.125(H2O)1.625 + 2.25H+ = 1.25Ca+2 + 0.25AlO2- + 2.75H2O + SiO2",LogKd(5CA_2d25H__1d25Ca_0d25AlO2_2d75H2O_SiO2));
    PREACT("5CNA: (CaO)1.25(SiO2)1(Al2O3)0.125(Na2O)0.25(H2O)1.375 + 2.75H+ = 1.25Ca+2 + 0.5Na+ + 0.25AlO2- + 2.75H2O + SiO2",LogKd(5CNA_2d75H__1d25Ca_0d5Na_0d25AlO2_2d75H2O_SiO2));
    PREACT("AlOHam: Al(OH)3 = AlO2- + H+ + H2O",LogKd(AlOHam__AlO2_H_H2O));
    PREACT("AlOHmic: Al(OH)3 = AlO2- + H+ + H2O",LogKd(AlOHmic__AlO2_H_H2O));
    PREACT("Amor-Sl: SiO2 = SiO2",LogKd(AmorSl__SiO2));
    PREACT("Anh: CaSO4 = Ca+2 + SO4-2",LogKd(Anh__Ca_SO4));
    PREACT("Arg: CaCO3 = CO3-2 + Ca+2",LogKd(Arg__CO3_Ca));
    PREACT("Brc: Mg(OH)2 + 2H+ = Mg+2 + 2H2O",LogKd(Brc_2H__Mg_2H2O));
    PREACT("C2AClH5: Ca2AlCl(OH)6(H2O)2 + 2H+ = 2Ca+2 + Cl- + AlO2- + 6H2O",LogKd(C2AClH5_2H__2Ca_Cl_AlO2_6H2O));
    PREACT("C2AH7.5: Ca2Al2(OH)10(H2O)2.5 + 2H+ = 2Ca+2 + 2AlO2- + 8.5H2O",LogKd(C2AH7d5_2H__2Ca_2AlO2_8d5H2O));
    PREACT("C2AH65: Ca2Al(OH)7(H2O)3 + 3H+ = 2Ca+2 + AlO2- + 8H2O",LogKd(C2AH65_3H__2Ca_AlO2_8H2O));
    PREACT("C2S: (CaO)2SiO2 + 4H+ = 2Ca+2 + 2H2O + SiO2",LogKd(C2S_4H__2Ca_2H2O_SiO2));
    PREACT("C3A: (CaO)3Al2O3 + 4H+ = 3Ca+2 + 2AlO2- + 2H2O",LogKd(C3A_4H__3Ca_2AlO2_2H2O));
    PREACT("C3AFS0.84H4.32: (AlFeO3)(Ca3O3(SiO2)0.84(H2O)4.32) + 4H+ = FeO2- + 3Ca+2 + AlO2- + 6.32H2O + 0.84SiO2",LogKd(C3AFS0d84H4d32_4H__FeO2_3Ca_AlO2_6d32H2O_0d84SiO2));
    PREACT("C3AH6: Ca3Al2O6(H2O)6 + 4H+ = 3Ca+2 + 2AlO2- + 8H2O",LogKd(C3AH6_4H__3Ca_2AlO2_8H2O));
    PREACT("C3AS0.41H5.18: Ca3Al2O6(SiO2)0.41(H2O)5.18 + 4H+ = 3Ca+2 + 2AlO2- + 7.18H2O + 0.41SiO2",LogKd(C3AS0d41H5d18_4H__3Ca_2AlO2_7d18H2O_0d41SiO2));
    PREACT("C3AS0.84H4.32: AlCa3AlO6(SiO2)0.84(H2O)4.32 + 4H+ = 3Ca+2 + 2AlO2- + 6.32H2O + 0.84SiO2",LogKd(C3AS0d84H4d32_4H__3Ca_2AlO2_6d32H2O_0d84SiO2));
    PREACT("C3FH6: Ca3Fe2O6(H2O)6 + 4H+ = 2FeO2- + 3Ca+2 + 8H2O",LogKd(C3FH6_4H__2FeO2_3Ca_8H2O));
    PREACT("C3FS0.84H4.32: (FeFeO3)(Ca3O3(SiO2)0.84(H2O)4.32) + 4H+ = 2FeO2- + 3Ca+2 + 6.32H2O + 0.84SiO2",LogKd(C3FS0d84H4d32_4H__2FeO2_3Ca_6d32H2O_0d84SiO2));
    PREACT("C3FS1.34H3.32: Ca3Fe2O6(SiO2)1.34(H2O)3.32 + 4H+ = 2FeO2- + 3Ca+2 + 5.32H2O + 1.34SiO2",LogKd(C3FS1d34H3d32_4H__2FeO2_3Ca_5d32H2O_1d34SiO2));
    PREACT("C3S: (CaO)3SiO2 + 6H+ = 3Ca+2 + 3H2O + SiO2",LogKd(C3S_6H__3Ca_3H2O_SiO2));
    PREACT("C4AClH10: Ca4Al2Cl2(OH)12(H2O)4 + 4H+ = 2Cl- + 4Ca+2 + 2AlO2- + 12H2O",LogKd(C4AClH10_4H__2Cl_4Ca_2AlO2_12H2O));
    PREACT("C4FeCl2H10: Ca4Fe2Cl2(OH)12(H2O)4 + 4H+ = 12H2O + 2FeO2- + 2Cl- + 4Ca+2",LogKd(C4FeCl2H10_4H__12H2O_2FeO2_2Cl_4Ca));
    PREACT("C4AF: (CaO)4(Al2O3)(Fe2O3) + 4H+ = 2FeO2- + 4Ca+2 + 2AlO2- + 2H2O",LogKd(C4AF_4H__2FeO2_4Ca_2AlO2_2H2O));
    PREACT("C4AH11: Ca4Al2(OH)14(H2O)4 + 6H+ = 4Ca+2 + 2AlO2- + 14H2O",LogKd(C4AH11_6H__4Ca_2AlO2_14H2O));
    PREACT("C4AH13: Ca4Al2(OH)14(H2O)6 + 6H+ = 4Ca+2 + 2AlO2- + 16H2O",LogKd(C4AH13_6H__4Ca_2AlO2_16H2O));
    PREACT("C4AH19: Ca4Al2(OH)14(H2O)12 + 6H+ = 4Ca+2 + 2AlO2- + 22H2O",LogKd(C4AH19_6H__4Ca_2AlO2_22H2O));
    PREACT("C4AsClH12: Ca4Al2Cl(SO4)0.5(OH)12(H2O)6 + 4H+ = Cl- + 4Ca+2 + 0.5SO4-2 + 2AlO2- + 14H2O",LogKd(C4AsClH12_4H__Cl_4Ca_0d5SO4_2AlO2_14H2O));
    PREACT("C4FH13: Ca4Fe2(OH)14(H2O)6 + 6H+ = 2FeO2- + 4Ca+2 + 16H2O",LogKd(C4FH13_6H__2FeO2_4Ca_16H2O));
    PREACT("C12A7: (CaO)12(Al2O3)7 + 10H+ = 12Ca+2 + 14AlO2- + 5H2O",LogKd(C12A7_10H__12Ca_14AlO2_5H2O));
    PREACT("CA2: CaO(Al2O3)2 + H2O = Ca+2 + 4AlO2- + 2H+",LogKd(CA2_H2O__Ca_4AlO2_2H));
    PREACT("CA: CaOAl2O3 = Ca+2 + 2AlO2-",LogKd(CA__Ca_2AlO2));
    PREACT("CAH10: CaOAl2O3(H2O)10 = Ca+2 + 2AlO2- + 10H2O",LogKd(CAH10__Ca_2AlO2_10H2O));
    PREACT("Cal: CaCO3 = CO3-2 + Ca+2",LogKd(Cal__CO3_Ca));
    PREACT("Cls: SrSO4 = SO4-2 + Sr+2",LogKd(Cls__SO4_Sr));
    PREACT("CSH3T-T2C: ((CaO)0.75(SiO2)0.5(H2O)1.25)2 + 3H+ = 1.5Ca+2 + 4H2O + SiO2",LogKd(CSH3TT2C_3H__1d5Ca_4H2O_SiO2));
    PREACT("CSH3T-T5C: ((CaO)1(SiO2)1(H2O)2)1.25 + 2.5H+ = 1.25Ca+2 + 3.75H2O + 1.25SiO2",LogKd(CSH3TT5C_2d5H__1d25Ca_3d75H2O_1d25SiO2));
    PREACT("CSH3T-TobH: (CaO)1(SiO2)1.5(H2O)2.5 + 2H+ = Ca+2 + 3.5H2O + 1.5SiO2",LogKd(CSH3TTobH_2H__Ca_3d5H2O_1d5SiO2));
    PREACT("CSHQ-JenD: (CaO)1.5(SiO2)0.6667(H2O)2.5 + 3H+ = 1.5Ca+2 + 4H2O + 0.6667SiO2",LogKd(CSHQJenD_3H__1d5Ca_4H2O_0d6667SiO2));
    PREACT("CSHQ-JenH: (CaO)1.3333(SiO2)1(H2O)2.1667 + 2.6666H+ = 1.3333Ca+2 + 3.5H2O + SiO2",LogKd(CSHQJenH_2d6666H__1d3333Ca_3d5H2O_SiO2));
    PREACT("CSHQ-TobD: ((CaO)1.25(SiO2)1(H2O)2.75)0.6667 + 1.66675H+ = 0.833375Ca+2 + 2.6668H2O + 0.6667SiO2",LogKd(CSHQTobD_1d66675H__0d833375Ca_2d6668H2O_0d6667SiO2));
    PREACT("CSHQ-TobH: (CaO)0.6667(SiO2)1(H2O)1.5 + 1.3334H+ = 0.6667Ca+2 + 2.1667H2O + SiO2",LogKd(CSHQTobH_1d3334H__0d6667Ca_2d1667H2O_SiO2));
    PREACT("Dis-Dol: CaMg(CO3)2 = 2CO3-2 + Ca+2 + Mg+2",LogKd(DisDol__2CO3_Ca_Mg));
    PREACT("ECSH1-KSH: ((KOH)2.5SiO2H2O)0.2 + 0.5H+ = 0.7H2O + 0.2SiO2 + 0.5K+",LogKd(ECSH1KSH_0d5H__0d7H2O_0d2SiO2_0d5K));
    PREACT("ECSH1-NaSH: ((NaOH)2.5SiO2H2O)0.2 + 0.5H+ = 0.5Na+ + 0.7H2O + 0.2SiO2",LogKd(ECSH1NaSH_0d5H__0d5Na_0d7H2O_0d2SiO2));
    PREACT("ECSH1-SH: (SiO2H2O)1 = H2O + SiO2",LogKd(ECSH1SH__H2O_SiO2));
    PREACT("ECSH1-SrSH: ((Sr(OH)2)1SiO2H2O)1 + 2H+ = Sr+2 + 3H2O + SiO2",LogKd(ECSH1SrSH_2H__Sr_3H2O_SiO2));
    PREACT("ECSH1-TobCa: ((Ca(OH)2)0.8333SiO2H2O)1 + 1.6666H+ = 0.8333Ca+2 + 2.6666H2O + SiO2",LogKd(ECSH1TobCa_1d6666H__0d8333Ca_2d6666H2O_SiO2));
    PREACT("ECSH2-JenCa: ((Ca(OH)2)1.6667SiO2H2O)0.6 + 2.00004H+ = 1.00002Ca+2 + 2.60004H2O + 0.6SiO2",LogKd(ECSH2JenCa_2d00004H__1d00002Ca_2d60004H2O_0d6SiO2));
    PREACT("ECSH2-KSH: ((KOH)2.5SiO2H2O)0.2 + 0.5H+ = 0.7H2O + 0.2SiO2 + 0.5K+",LogKd(ECSH2KSH_0d5H__0d7H2O_0d2SiO2_0d5K));
    PREACT("ECSH2-NaSH: ((NaOH)2.5SiO2H2O)0.2 + 0.5H+ = 0.5Na+ + 0.7H2O + 0.2SiO2",LogKd(ECSH2NaSH_0d5H__0d5Na_0d7H2O_0d2SiO2));
    PREACT("ECSH2-SrSH: ((Sr(OH)2)1SiO2H2O)1 + 2H+ = Sr+2 + 3H2O + SiO2",LogKd(ECSH2SrSH_2H__Sr_3H2O_SiO2));
    PREACT("ECSH2-TobCa: ((Ca(OH)2)0.8333SiO2H2O)1 + 1.6666H+ = 0.8333Ca+2 + 2.6666H2O + SiO2",LogKd(ECSH2TobCa_1d6666H__0d8333Ca_2d6666H2O_SiO2));
    PREACT("ettringite: ((H2O)2)Ca6Al2(SO4)3(OH)12(H2O)24 + 4H+ = 6Ca+2 + 3SO4-2 + 2AlO2- + 34H2O",LogKd(ettringite_4H__6Ca_3SO4_2AlO2_34H2O));
    PREACT("ettringite03_ss: (SO4)Ca2Al0.6666667(OH)4(H2O)8.6666667 + 1.3333332H+ = 2Ca+2 + SO4-2 + 0.6666667AlO2- + 11.3333333H2O + 0.0000001e-",LogKd(ettringite03_ss_1d3333332H__2Ca_SO4_0d6666667AlO2_11d3333333H2O_0d0000001e));
    PREACT("ettringite05: Ca3Al(SO4)1.5(OH)6(H2O)13 + 2H+ = 3Ca+2 + 1.5SO4-2 + AlO2- + 17H2O",LogKd(ettringite05_2H__3Ca_1d5SO4_AlO2_17H2O));
    PREACT("ettringite9: Ca6Al2(SO4)3(OH)12(H2O)3 + 4H+ = 6Ca+2 + 3SO4-2 + 2AlO2- + 11H2O",LogKd(ettringite9_4H__6Ca_3SO4_2AlO2_11H2O));
    PREACT("Ettringite9_des: Ca6Al2(SO4)3(OH)12(H2O)3 + 4H+ = 6Ca+2 + 3SO4-2 + 2AlO2- + 11H2O",LogKd(Ettringite9_des_4H__6Ca_3SO4_2AlO2_11H2O));
    PREACT("ettringite13: Ca6Al2(SO4)3(OH)12(H2O)7 + 4H+ = 6Ca+2 + 3SO4-2 + 2AlO2- + 15H2O",LogKd(ettringite13_4H__6Ca_3SO4_2AlO2_15H2O));
    PREACT("Ettringite13_des: Ca6Al2(SO4)3(OH)12(H2O)7 + 4H+ = 6Ca+2 + 3SO4-2 + 2AlO2- + 15H2O",LogKd(Ettringite13_des_4H__6Ca_3SO4_2AlO2_15H2O));
    PREACT("ettringite30: Ca6Al2(SO4)3(OH)12(H2O)24 + 4H+ = 6Ca+2 + 3SO4-2 + 2AlO2- + 32H2O",LogKd(ettringite30_4H__6Ca_3SO4_2AlO2_32H2O));
    PREACT("Fe-ettringite05: Ca3Fe(SO4)1.5(OH)6(H2O)13 + 2H+ = FeO2- + 3Ca+2 + 1.5SO4-2 + 17H2O",LogKd(Feettringite05_2H__FeO2_3Ca_1d5SO4_17H2O));
    PREACT("Fe-ettringite: Ca6Fe2(SO4)3(OH)12(H2O)26 + 4H+ = 2FeO2- + 6Ca+2 + 3SO4-2 + 34H2O",LogKd(Feettringite_4H__2FeO2_6Ca_3SO4_34H2O));
    PREACT("Fe-hemicarbonate: Ca3O3Fe2O3(CaCO3)0.5(CaO2H2)0.5(H2O)9.5 + 5H+ = 2FeO2- + 0.5CO3-2 + 4Ca+2 + 12.5H2O",LogKd(Fehemicarbonate_5H__2FeO2_0d5CO3_4Ca_12d5H2O));
    PREACT("Fe-monosulph05: Ca2FeS0.5O5(H2O)6 + 2H+ = FeO2- + 2Ca+2 + 0.5SO4-2 + 7H2O",LogKd(Femonosulph05_2H__FeO2_2Ca_0d5SO4_7H2O));
    PREACT("Fe-monosulphate: Ca4Fe2SO10(H2O)12 + 4H+ = 2FeO2- + 4Ca+2 + SO4-2 + 14H2O",LogKd(Femonosulphate_4H__2FeO2_4Ca_SO4_14H2O));
    PREACT("Fe: Fe + 2H2O = FeO2- + 3e- + 4H+",LogKd(Fe_2H2O__FeO2_3e_4H));
    PREACT("Femonocarbonate: Ca4O4Fe2O3CO2(H2O)12 + 4H+ = 2FeO2- + CO3-2 + 4Ca+2 + 14H2O",LogKd(Femonocarbonate_4H__2FeO2_CO3_4Ca_14H2O));
    PREACT("FeOOHmic: FeOOH  = FeO2- + H+",LogKd(FeOOHmic__FeO2_H));
    PREACT("Fe(OH)3(am): Fe(OH)3 = H2O + H+ + FeO2-",LogKd(FeOHOHOHam__H2O_H_FeO2));
    PREACT("Fe(OH)3(mic): Fe(OH)3 = H2O + H+ + FeO2-",LogKd(FeOHOHOHmic__H2O_H_FeO2));
    PREACT("FeCO3(pr): FeCO3 + 2H2O = 4H+ + FeO2- + e- + CO3-2",LogKd(FeCO3pr_2H2O__4H_FeO2_e_CO3));
    PREACT("Gbs: Al(OH)3 = AlO2- + H+ + H2O",LogKd(Gbs__AlO2_H_H2O));
    PREACT("Gp: CaSO4(H2O)2 = Ca+2 + SO4-2 + 2H2O",LogKd(Gp__Ca_SO4_2H2O));
    PREACT("Gr: C + 3H2O = CO3-2 + 4e- + 6H+",LogKd(Gr_3H2O__CO3_4e_6H));
    PREACT("Gt: FeO(OH) = FeO2- + H+",LogKd(Gt__FeO2_H));
    PREACT("Hem: Fe2O3 + H2O = 2FeO2- + 2H+",LogKd(Hem_H2O__2FeO2_2H));
    PREACT("hemicarbonat10.5: (CaO)3Al2O3(CaCO3)0.5(CaO2H2)0.5(H2O)10 + 5H+ = 0.5CO3-2 + 4Ca+2 + 2AlO2- + 13H2O",LogKd(hemicarbonat10d5_5H__0d5CO3_4Ca_2AlO2_13H2O));
    PREACT("hemicarbonate: (CaO)3Al2O3(CaCO3)0.5(CaO2H2)0.5(H2O)11.5 + 5H+ = 0.5CO3-2 + 4Ca+2 + 2AlO2- + 14.5H2O",LogKd(hemicarbonate_5H__0d5CO3_4Ca_2AlO2_14d5H2O));
    PREACT("hemicarbonate9: (CaO)3Al2O3(CaCO3)0.5(CaO2H2)0.5(H2O)8.5 + 5H+ = 0.5CO3-2 + 4Ca+2 + 2AlO2- + 11.5H2O",LogKd(hemicarbonate9_5H__0d5CO3_4Ca_2AlO2_11d5H2O));
    PREACT("hemihydrate: CaSO4(H2O)0.5 = Ca+2 + SO4-2 + 0.5H2O",LogKd(hemihydrate__Ca_SO4_0d5H2O));
    PREACT("hydrotalcite: Mg4Al2O7(H2O)10 + 6H+ = 4Mg+2 + 2AlO2- + 13H2O",LogKd(hydrotalcite_6H__4Mg_2AlO2_13H2O));
    PREACT("INFCA: (CaO)1(SiO2)1.1875(Al2O3)0.15625(H2O)1.65625 + 1.6875H+ = Ca+2 + 0.3125AlO2- + 2.5H2O + 1.1875SiO2",LogKd(INFCA_1d6875H__Ca_0d3125AlO2_2d5H2O_1d1875SiO2));
    PREACT("INFCN: (CaO)1(SiO2)1.5(Na2O)0.3125(H2O)1.1875 + 2.625H+ = Ca+2 + 0.625Na+ + 2.5H2O + 1.5SiO2",LogKd(INFCN_2d625H__Ca_0d625Na_2d5H2O_1d5SiO2));
    PREACT("INFCNA: (CaO)1(SiO2)1.1875(Al2O3)0.15625(Na2O)0.34375(H2O)1.3125 + 2.375H+ = 0.3125AlO2- + Ca+2 + 0.6875Na+ + 2.5H2O + 1.1875SiO2",LogKd(INFCNA_2d375H__0d3125AlO2_Ca_0d6875Na_2d5H2O_1d1875SiO2));
    PREACT("Jennite: (SiO2)1(CaO)1.666667(H2O)2.1 + 3.333334H+ = 1.666667Ca+2 + 3.766667H2O + SiO2",LogKd(Jennite_3d333334H__1d666667Ca_3d766667H2O_SiO2));
    PREACT("K2O: K2O + 2H+ = H2O + 2K+",LogKd(K2O_2H__H2O_2K));
    PREACT("K2SO4: K2SO4 = SO4-2 + 2K+",LogKd(K2SO4__SO4_2K));
    PREACT("Kln: Al2Si2O5(OH)4 = 2AlO2- + 2H+ + H2O + 2SiO2",LogKd(Kln__2AlO2_2H_H2O_2SiO2));
    PREACT("KSiOH: ((KOH)2.5SiO2H2O)0.2 + 0.5H+ = 0.7H2O + 0.2SiO2 + 0.5K+",LogKd(KSiOH_0d5H__0d7H2O_0d2SiO2_0d5K));
    PREACT("Lim: CaO + 2H+ = Ca+2 + H2O",LogKd(Lim_2H__Ca_H2O));
    PREACT("M4A-OH-LDH: Mg4Al2(OH)14(H2O)3 + 6H+ = 4Mg+2 + 2AlO2- + 13H2O",LogKd(M4AOHLDH_6H__4Mg_2AlO2_13H2O));
    PREACT("M6A-OH-LDH: Mg6Al2(OH)18(H2O)3 + 10H+ = 6Mg+2 + 2AlO2- + 17H2O",LogKd(M6AOHLDH_10H__6Mg_2AlO2_17H2O));
    PREACT("M8A-OH-LDH: Mg8Al2(OH)22(H2O)3 + 14H+ = 8Mg+2 + 2AlO2- + 21H2O",LogKd(M8AOHLDH_14H__8Mg_2AlO2_21H2O));
    PREACT("Mag: FeFe2O4 + 2H2O = 3FeO2- + e- + 4H+",LogKd(Mag_2H2O__3FeO2_e_4H));
    PREACT("Melanterite: FeSO4(H2O)7 = FeO2- + SO4-2 + e- + 4H+ + 5H2O",LogKd(Melanterite__FeO2_SO4_e_4H_5H2O));
    PREACT("Mg2AlC0.5OH: Mg2Al(OH)6(CO3)0.5(H2O)2 + 2H+ = 0.5CO3-2 + 2Mg+2 + AlO2- + 6H2O",LogKd(Mg2AlC0d5OH_2H__0d5CO3_2Mg_AlO2_6H2O));
    PREACT("Mg2FeC0.5OH: Mg2Fe(OH)6(CO3)0.5(H2O)2 + 2H+ = FeO2- + 0.5CO3-2 + 2Mg+2 + 6H2O",LogKd(Mg2FeC0d5OH_2H__FeO2_0d5CO3_2Mg_6H2O));
    PREACT("Mg3AlC0.5OH: Mg3Al(OH)8(CO3)0.5(H2O)2.5 + 4H+ = 0.5CO3-2 + 3Mg+2 + AlO2- + 8.5H2O",LogKd(Mg3AlC0d5OH_4H__0d5CO3_3Mg_AlO2_8d5H2O));
    PREACT("Mg3FeC0.5OH: Mg3Fe(OH)8(CO3)0.5(H2O)2.5 + 4H+ = FeO2- + 0.5CO3-2 + 3Mg+2 + 8.5H2O",LogKd(Mg3FeC0d5OH_4H__FeO2_0d5CO3_3Mg_8d5H2O));
    PREACT("Mgs: MgCO3 = CO3-2 + Mg+2",LogKd(Mgs__CO3_Mg));
    PREACT("monocarbonate05: Ca2AlC0.5O4.5(H2O)5.5 + 2H+ = 0.5CO3-2 + 2Ca+2 + AlO2- + 6.5H2O",LogKd(monocarbonate05_2H__0d5CO3_2Ca_AlO2_6d5H2O));
    PREACT("monocarbonate9: Ca4Al2CO9(H2O)9 + 4H+ = CO3-2 + 4Ca+2 + 2AlO2- + 11H2O",LogKd(monocarbonate9_4H__CO3_4Ca_2AlO2_11H2O));
    PREACT("monocarbonate: Ca4Al2CO9(H2O)11 + 4H+ = CO3-2 + 4Ca+2 + 2AlO2- + 13H2O",LogKd(monocarbonate_4H__CO3_4Ca_2AlO2_13H2O));
    PREACT("mononitrate: Ca4Al2(OH)12N2O6(H2O)4 + 4H+ = 4Ca+2 + 2NO3- + 2AlO2- + 12H2O",LogKd(mononitrate_4H__4Ca_2NO3_2AlO2_12H2O));
    PREACT("mononitrite: Ca4Al2(OH)12N2O4(H2O)4 = 4Ca+2 + 2NO3- + 4e- + 2AlO2- + 10H2O",LogKd(mononitrite__4Ca_2NO3_4e_2AlO2_10H2O));
    PREACT("monosulphate9: Ca4Al2SO10(H2O)9 + 4H+ = 4Ca+2 + SO4-2 + 2AlO2- + 11H2O",LogKd(monosulphate9_4H__4Ca_SO4_2AlO2_11H2O));
    PREACT("monosulphate10_5: Ca4Al2SO10(H2O)10.5 + 4H+ = 4Ca+2 + SO4-2 + 2AlO2- + 12.5H2O",LogKd(monosulphate10_5_4H__4Ca_SO4_2AlO2_12d5H2O));
    PREACT("monosulphate12: Ca4Al2SO10(H2O)12 + 4H+ = 4Ca+2 + SO4-2 + 2AlO2- + 14H2O",LogKd(monosulphate12_4H__4Ca_SO4_2AlO2_14H2O));
    PREACT("monosulphate14: Ca4Al2SO10(H2O)14 + 4H+ = 4Ca+2 + SO4-2 + 2AlO2- + 16H2O",LogKd(monosulphate14_4H__4Ca_SO4_2AlO2_16H2O));
    PREACT("monosulphate16: Ca4Al2SO10(H2O)16 + 4H+ = 4Ca+2 + SO4-2 + 2AlO2- + 18H2O",LogKd(monosulphate16_4H__4Ca_SO4_2AlO2_18H2O));
    PREACT("monosulphate1205: Ca2AlS0.5O5(H2O)6 + 2H+ = 2Ca+2 + 0.5SO4-2 + AlO2- + 7H2O",LogKd(monosulphate1205_2H__2Ca_0d5SO4_AlO2_7H2O));
    PREACT("Na2O: Na2O + 2H+ = 2Na+ + H2O",LogKd(Na2O_2H__2Na_H2O));
    PREACT("Na2SO4: Na2SO4 = SO4-2 + 2Na+",LogKd(Na2SO4__SO4_2Na));
    PREACT("NaSiOH: ((NaOH)2.5SiO2H2O)0.2 + 0.5H+ = 0.5Na+ + 0.7H2O + 0.2SiO2",LogKd(NaSiOH_0d5H__0d5Na_0d7H2O_0d2SiO2));
    PREACT("Ord-Dol: CaMg(CO3)2 = 2CO3-2 + Ca+2 + Mg+2",LogKd(OrdDol__2CO3_Ca_Mg));
    PREACT("Portlandite: Ca(OH)2 + 2H+ = Ca+2 + 2H2O",LogKd(Portlandite_2H__Ca_2H2O));
    PREACT("Py: FeSS + 10H2O = FeO2- + 2SO4-2 + 15e- + 20H+",LogKd(Py_10H2O__FeO2_2SO4_15e_20H));
    PREACT("Qtz: SiO2 = SiO2",LogKd(Qtz__SiO2));
    PREACT("Sd: FeCO3 + 2H2O = FeO2- + CO3-2 + e- + 4H+",LogKd(Sd_2H2O__FeO2_CO3_e_4H));
    PREACT("straetlingite5_5: Ca2Al2SiO7(H2O)5.5 + 2H+ = 2Ca+2 + 2AlO2- + 6.5H2O + SiO2",LogKd(straetlingite5_5_2H__2Ca_2AlO2_6d5H2O_SiO2));
    PREACT("straetlingite7: Ca2Al2SiO7(H2O)7 + 2H+ = 2Ca+2 + 2AlO2- + 8H2O + SiO2",LogKd(straetlingite7_2H__2Ca_2AlO2_8H2O_SiO2));
    PREACT("straetlingite: Ca2Al2SiO7(H2O)8 + 2H+ = 2Ca+2 + 2AlO2- + 9H2O + SiO2",LogKd(straetlingite_2H__2Ca_2AlO2_9H2O_SiO2));
    PREACT("Str: SrCO3 = CO3-2 + Sr+2",LogKd(Str__CO3_Sr));
    PREACT("Sulfur: S + 4H2O = SO4-2 + 6e- + 8H+",LogKd(Sulfur_4H2O__SO4_6e_8H));
    PREACT("syngenite: K2Ca(SO4)2H2O = Ca+2 + 2SO4-2 + H2O + 2K+",LogKd(syngenite__Ca_2SO4_H2O_2K));
    PREACT("T2C-CNASHss: (CaO)1.5(SiO2)1(H2O)2.5 + 3H+ = 1.5Ca+2 + 4H2O + SiO2",LogKd(T2CCNASHss_3H__1d5Ca_4H2O_SiO2));
    PREACT("T5C-CNASHss: (CaO)1.25(SiO2)1.25(H2O)2.5 + 2.5H+ = 1.25Ca+2 + 3.75H2O + 1.25SiO2",LogKd(T5CCNASHss_2d5H__1d25Ca_3d75H2O_1d25SiO2));
    PREACT("thaumasite: (CaSiO3)(CaSO4)(CaCO3)(H2O)15 + 2H+ = CO3-2 + 3Ca+2 + SO4-2 + 16H2O + SiO2",LogKd(thaumasite_2H__CO3_3Ca_SO4_16H2O_SiO2));
    PREACT("Tob-I: (SiO2)2.4(CaO)2(H2O)3.2 + 4H+ = 2Ca+2 + 5.2H2O + 2.4SiO2",LogKd(TobI_4H__2Ca_5d2H2O_2d4SiO2));
    PREACT("Tob-II: (SiO2)1(CaO)0.833333(H2O)1.333333 + 1.666666H+ = 0.833333Ca+2 + 2.166666H2O + SiO2",LogKd(TobII_1d666666H__0d833333Ca_2d166666H2O_SiO2));
    PREACT("TobH-CNASHss: (CaO)1(SiO2)1.5(H2O)2.5 + 2H+ = Ca+2 + 3.5H2O + 1.5SiO2",LogKd(TobHCNASHss_2H__Ca_3d5H2O_1d5SiO2));
    PREACT("tricarboalu03: (CO3)Ca2Al0.6666667(OH)4(H2O)8.6666667 + 1.3333332H+ = CO3-2 + 2Ca+2 + 0.6666667AlO2- + 11.3333333H2O + 0.0000001e-",LogKd(tricarboalu03_1d3333332H__CO3_2Ca_0d6666667AlO2_11d3333333H2O_0d0000001e));
    PREACT("Tro: FeS + 6H2O = FeO2- + SO4-2 + 9e- + 12H+",LogKd(Tro_6H2O__FeO2_SO4_9e_12H));
    PREACT("zeoliteP_Ca: Ca(Al2Si2)O8(H2O)4.5 = 2AlO2- + Ca+2 + 2SiO2 + 4.5H2O ",LogKd(zeoliteP_Ca__2AlO2_Ca_2SiO2_4d5H2O));
    PREACT("chabazite: Ca(Al2Si4)O12(H2O)6 = 2AlO2- + Ca+2 + 4SiO2 + 6H2O",LogKd(chabazite__2AlO2_Ca_4SiO2_6H2O));
    PREACT("M075SH: Mg1.5Si2O5.5(H2O)2.5 = 1.5Mg+2 + 2SiO2 + 3OH- + H2O",LogKd(M075SH__1d5Mg_2SiO2_3OH_H2O));
    PREACT("M15SH: Mg1.5SiO3.5(H2O)2.5 = 1.5Mg+2 + 1SiO2 + 3OH- + H2O",LogKd(M15SH__1d5Mg_1SiO2_3OH_H2O));
    PREACT("zeoliteX: Na2(Al2Si2.5)O9(H2O)6.2 = 2AlO2- + 2Na+ + 2.5SiO2 + 6.2H2O",LogKd(zeoliteX__2AlO2_2Na_2d5SiO2_6d2H2O));
    PREACT("natrolite: Na2(Al2Si3)O10(H2O)2 = 2AlO2- + 2Na+ + 3SiO2 + 2H2O",LogKd(natrolite__2AlO2_2Na_3SiO2_2H2O));
    PREACT("zeoliteY: Na2(Al2Si4)O12(H2O)8 = 2AlO2- + 2Na+ + 4SiO2 + 8H2O",LogKd(zeoliteY__2AlO2_2Na_4SiO2_8H2O));
  }
  
  REACSUBTITLE(" ");
  REACSUBTITLE("Other constants:") ;
  REACSUBTITLE("---------------") ;
  #define LogKeq(R) Log10EquilibriumConstantOfHomogeneousReactionInWater(R,T)
  {
    double logk_ch       = LogKd(Portlandite_2H__Ca_2H2O);
    double logk_cc       = LogKd(Cal__CO3_Ca);
    double logk_csh2     = LogKd(Gp__Ca_SO4_2H2O);
    /* Some other constants */
    double loga_h2o   = 0 ;
    /* Carbon compounds */
    double logk_co2      = LogKeq(CO3_2H__CO2_H2O);
    /* Sulfur compounds */
    double logk_h2so4    = LogKeq(SO4_2H__H2SO4);

    double loga_co2eq = logk_cc - logk_ch + logk_co2 + loga_h2o ;
    double loga_h2so4eq = logk_csh2 - logk_ch + logk_h2so4 ;
  
    PREACT("Log(a_co2eq)",loga_co2eq) ;
    PREACT("Log(a_h2so4eq)",loga_h2so4eq) ;
  }
  #undef LogKeq
  #undef LogKd
  
  printf("\n") ;
  
  fflush(stdout) ;
  
  
#undef PREACT
#undef REACTITLE
#undef REACSUBTITLE
}



void Log10DissociationConstantOfCementHydrationProduct_Print(double T)
{
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
    n += printf(" % 3.1f",logk) ;\
    while(n < c2) n += printf(" ") ;\
    printf("\n") ;\
  } while(0)
  
  printf("\n") ;
  
  
  /* Cement hydrates dissociation reactions */
  #define LogKd(R) Log10DissociationConstantOfCementHydrationProduct(R,T)
  {
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
  }
  
  REACSUBTITLE(" ");
  REACSUBTITLE("Other constants:") ;
  REACSUBTITLE("---------------") ;
  
  #define LogKeq(R) Log10EquilibriumConstantOfHomogeneousReactionInWater(R,T)
  {
    double logk_ch       = LogKd(CH__Ca_2OH) ;
    double logk_cc       = LogKd(Calcite__Ca_CO3) ;
    double logk_csh2     = LogKd(CSH2__Ca_SO4_2H2O) ;
    /* Some other constants */
    double loga_h2o   = 0 ;
    double logk_h2o      = LogKeq(H2O__H_OH);
    /* Carbon compounds */
    double logk_hco3     = LogKeq(HCO3__CO2_OH);
    double logk_co3      = LogKeq(CO3_H2O__HCO3_OH);
    /* Sulfur compounds */
    double logk_h2so4    = LogKeq(H2SO4__HSO4_H);
    double logk_hso4     = LogKeq(HSO4__SO4_H);

    double loga_co2eq = logk_cc - logk_ch + logk_co3 + logk_hco3 + loga_h2o ;
    double loga_h2so4eq = logk_csh2 - logk_ch + 2*logk_h2o - logk_hso4 - logk_h2so4 ;
  
    PREACT("Log(a_co2eq)",loga_co2eq) ;
    PREACT("Log(a_h2so4eq)",loga_h2so4eq) ;
  }
  #undef LogKeq
  #undef LogKd
  
  printf("\n") ;
  
  fflush(stdout) ;
  
  
#undef PREACT
#undef REACTITLE
#undef REACSUBTITLE
}
