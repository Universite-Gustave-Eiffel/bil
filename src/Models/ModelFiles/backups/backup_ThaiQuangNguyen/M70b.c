/*

 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "model.h"
#include "FVM.h"

#define TITLE   "Sulfuric acid attack of concrete (02/2012)"
#define AUTHORS "Yuan"

#include "PredefinedMethods.h"

/* Macros */
#define NEQ     (5)
#define NVE     (28)
#define NVE_TR  (28)
#define NVI     (25)
#define NV0     (8)

#define E_S     (0)
#define E_q     (1)
#define E_Ca    (2)
#define E_Si    (3)
#define E_K     (4)

#define I_C_H2SO4  (0)
#define I_PSI      (1)
#define I_ZN_Ca_S  (2)
#define I_ZN_Si_S  (3)
#define I_C_K      (4)

#define RHO     1
#define LOG_RHO 2
#define Ln10    2.302585093
#define U_H2SO4   LOG_RHO
#define EXPLICIT  1
#define IMPLICIT  2
#define U_PHI     IMPLICIT

#define UNKNOWN(n,i)     (u[n][Element_GetNodalUnknownPosition(el,n,i)])
#define UNKNOWN_n(n,i)   (u_n[n][Element_GetNodalUnknownPosition(el,n,i)])

/*
#define UNKNOWN(n,i)     (Element_GetValueOfNodalUnknown(el,n,i))
#define UNKNOWN_n(n,i)   (Element_GetValueOfPreviousNodalUnknown(el,n,i))
*/


#if (U_H2SO4 == LOG_RHO)
  #define C_H2SO4(n)   (exp(Ln10*UNKNOWN(n,I_C_H2SO4)))
  #define C_H2SO4n(n)  (exp(Ln10*UNKNOWN_n(n,I_C_H2SO4)))
#else
  #define C_H2SO4(n)   (UNKNOWN(n,I_C_H2SO4))
  #define C_H2SO4n(n)  (UNKNOWN_n(n,I_C_H2SO4))
#endif
#define ZN_Ca_S(n)   (UNKNOWN(n,I_ZN_Ca_S))
#define ZN_Si_S(n)   (UNKNOWN(n,I_ZN_Si_S))
#define PSI(n)       (UNKNOWN(n,I_PSI))
#define C_K(n)       (UNKNOWN(n,I_C_K))

#define ZN_Ca_Sn(n)  (UNKNOWN_n(n,I_ZN_Ca_S))
#define ZN_Si_Sn(n)  (UNKNOWN_n(n,I_ZN_Si_S))
#define PSIn(n)      (UNKNOWN_n(n,I_PSI))
#define C_Kn(n)      (UNKNOWN_n(n,I_C_K))

#define N_S(n)     (f[(n)])
#define N_q(n)     (f[(2+n)])
#define N_Ca(n)    (f[(4+n)])
#define N_Si(n)    (f[(6+n)])
#define N_K(n)     (f[(8+n)])
#define W_S        (f[10])
#define W_q        (f[11])
#define W_Ca       (f[12])
#define W_Si       (f[13])
#define W_K        (f[14])
#define N_CH(n)    (f[(15+n)])
#define N_CSH2(n)  (f[(17+n)])
#define N_Si_S(n)  (f[(19+n)])
#define ZQ_CH(n)   (f[(21+n)])
#define PHI(n)     (f[(23+n)])

#define N_Sn(n)    (f_n[(n)])
#define N_qn(n)    (f_n[(2+n)])
#define N_Can(n)   (f_n[(4+n)])
#define N_Sin(n)   (f_n[(6+n)])
#define N_Kn(n)    (f_n[(8+n)])
#define N_CHn(n)   (f_n[(15+n)])
#define N_CSH2n(n) (f_n[(17+n)])
#define N_Si_Sn(n) (f_n[(19+n)])
#define ZQ_CHn(n)  (f_n[(21+n)])
#define PHIn(n)    (f_n[(23+n)])


#define KF_OH       (va[(0)])
#define KF_H        (va[(1)])
#define KF_CO2      (va[(2)])
#define KF_H2SO4    (va[(3)])
#define KF_HSO4     (va[(4)])
#define KF_SO4      (va[(5)])
#define KF_Ca       (va[(6)])
#define KF_CaHSO4   (va[(7)])
#define KF_CaH3SiO4 (va[(8)])
#define KF_H3SiO4   (va[(9)])
#define KF_H4SiO4   (va[(10)])
#define KF_H2SiO4   (va[(11)])
#define KF_CaH2SiO4 (va[(12)])
#define KF_CaSO4aq  (va[(13)])
#define KF_CaOH     (va[(14)])
#define KF_K        (va[(15)])

#define Kpsi_OH       (va[(16)])
#define Kpsi_H        (va[(17)])
#define Kpsi_HSO4     (va[(18)])
#define Kpsi_SO4      (va[(19)])
#define Kpsi_Ca       (va[(20)])
#define Kpsi_CaHSO4   (va[(21)])
#define Kpsi_CaH3SiO4 (va[(22)])
#define Kpsi_H3SiO4   (va[(23)])
#define Kpsi_q        (va[(24)])
#define Kpsi_H2SiO4   (va[(25)])
#define Kpsi_CaOH     (va[(26)])
#define Kpsi_K        (va[(27)])

#define N_CH0(n)      (v0[(0+n)])
#define N_CSH20(n)    (v0[(2+n)])
#define N_Si_S0(n)    (v0[(4+n)])
#define ZQ_CH0(n)     (v0[(6+n)])

/*
  Solution aqueuse
*/

/* les valences */
#define z_ca       (2.)
#define z_h        (1.)
#define z_oh       (-1.)
#define z_hso4     (-1.)
#define z_so4      (-2.)
#define z_h3sio4   (-1.)
#define z_cahso4   (1.)
#define z_cah3sio4 (1.)
#define z_h2sio4   (-2.)
#define z_caoh     (1.)
#define z_k        (1.)

/* volumes molaires partiels des ions (dm3/mole) */
#define v_h        (-5.5e-3)     /* (-5.50e-3)  d'apres TQN */
#define v_oh       (-4.71e-3)    /* d'apres Lothenbach */
#define v_h2o      (18.e-3)
#define v_co2      (32.81e-3)
#define v_h2so4    (50.e-3)
#define v_hso4     (24.21.e-3)   /* d'apres Lothenbach */
#define v_so4      (22.9e-3)    /* d'apres Lothenbach */
#define v_ca       (-18.7e-3)    /* d'apres Lothenbach */
#define v_sioh4    (xxx)
#define v_h3sio4   (4.53e-3)     /* d'apres Lothenbach */
#define v_h2sio4   (34.13e-3)    /* d'apres Lothenbach */
#define v_cah2sio4 (15.69e-3)    /* d'apres Lothenbach */
#define v_cah3sio4 (-6.74e-3)
#define v_caso4aq  (26.20e-3)    /* a modifier */
#define v_caoh     (26.20e-3)    /* a modifier */
#define v_k        (43.93e-3)    /* d'apres Antoine */
#define v_koh      (27.44e-3)    /* d'apres Antoine */

/* coefficients de diffusion moleculaire (dm2/s) */
/* Stokes-Einstein ~ kT/(6*pi*mu*r)  kT/(6*pi*mu) = 2.1451e-19 m3/s  */
#define d_oh       (1.22e-7)    /* 1.22e-7 (radius = 1.75e-10 m) */
#define d_h        (9.310e-7)    /* 4.76e-8 (radius = 4.5e-10 m) */
#define d_h2so4    (1.5e-7)
#define d_hso4     (1.385e-7)    /* 1.07e-7 (radius = 2e-10 m) */
#define d_so4      (1.065e-7)    /* 9.53e-8 (radius = 2.25e-10 m) */
#define d_ca       (7.92e-8)
#define d_cahso4   (1.07e-7)    /* (radius = 2e-10 m) */
#define d_sioh4    (xxx)
#define d_h4sio4   (1.07e-7)     /* */
#define d_h3sio4   (1.07e-7)   /* (radius = 2e-10 m) */
#define d_h2sio4   (1.07e-7)    /*(radius = 2e-10 m) */
#define d_cah2sio4 (1.07e-7)    /* a modifier */
#define d_cah3sio4 (1.07e-7)     /*(radius = 2e-10 m) */
#define d_caso4aq  (1.43e-7)    /* (radius = 1.5e-10 m) */
#define d_caoh     (1.07e-7)    /* (radius = 2e-10 m) */
#define d_k        (1.43e-7)   /* (radius = 1.5e-10 m) */
#define d_koh      (1.43e-7)   /* (radius = 1.5e-10 m) */

/* constantes d'equilibre (ref = 1 mole/L) */
#define K_h2o      (1.e-14)          /* autoprotolyse de l'eau */
#define K_henry    (1.238)           /* cste de Henry du CO2 / RT */

#define K_hso4     (1.e3)            /* H2SO4   = HSO4[-] + H[+] */
#define K_so4      (1.e-2)           /* HSO4[-] = SO4[2-] + H[+] */

#define K_h2sio4   (4.68)            /* H3SiO4[-] + OH[-] = H2SiO4[2-] + H2O */
#define K_h4sio4   (6.45e9)          /* H3SiO4[-] + H[+] = H4SiO4 */
#define K_h3sio4   (1.55e-10)        /* H4SiO4    = H3SiO4[-] + H[+] */

#define K_cahso4   (1.276e+1)        /* Ca[2+] + HSO4[-]    = CaHSO4[+] */
#define K_caso4aq  (1.4e+3)          /* Ca[2+] + SO4[2-]    = CaSO4[0] */
#define K_cah2sio4 (3.98e+4)         /* Ca[2+] + H2SiO4[2-] = CaH2SiO4 */
#define K_cah3sio4 (1.58e+1)         /* Ca[2+] + H3SiO4[-]  = CaH3SiO4[+] */
#define K_caoh     (1.66e+1)         /* Ca[2+] + OH[-] = CaOH[+] */

/*
  Solides
  CH  = Portlandite
  CSH2  = Gypsum
  CSH = Hydrated Calcium Silicates
  SH  = Amorphous Silica
*/

/* constantes d'equilibre (ref = 1 mole/L) */
#define K_CH       (6.456e-6)        /* CH  = Ca[2+] + 2OH[-] */
#define K_SH       (1.93642e-3)      /* SHt = H4SiO4 + (t-2)H2O */
/* volumes molaires solides (dm3/mole) */
#define V_CH       (33.e-3)      /* (33.e-3) */

/* CSH2 */
/* constantes d'equilibre (ref = 1 mole/L) */
#define K_CSH2     (2.5e-5)         /* CSH2 = Ca[2+] + SO4[2-] + 2H2O */
/* volumes molaires solides (dm3/mole) */
#define V_CSH2     (75.e-3)      /* (75.e-3) */


/* Curves for C-S-H */
/* C/S ratio */
#define X_CSH(q)    (Curve_ComputeValue(Element_GetCurve(el),q))
#define DX_CSH(q)   (Curve_ComputeDerivative(Element_GetCurve(el),q))
/* Molar Volume */
#define V_CSH(q)    (Curve_ComputeValue(Element_GetCurve(el) + 2,q))
#define DV_CSH(q)   (Curve_ComputeDerivative(Element_GetCurve(el) + 2,q))
/* Amorphous Silica Q_SH/K_SH */
#define Q_SH(q)     (Curve_ComputeValue(Element_GetCurve(el) + 3,q))
#define DQ_SH(q)    (Curve_ComputeDerivative(Element_GetCurve(el) + 3,q))


#define C_H2SO4_eq (K_h2o*K_h2o*K_CSH2/(K_hso4*K_so4*K_CH))

/* constantes physiques */
#define FARADAY   (9.64846e7) /* Faraday (mC/mole = Pa.dm3/V/mole) */
#define RT        (2436.e3)   /* produit de R=8.3143 et T=293 (Pa.dm3/mole) */
#define FsRT      (3.961e1)   /* F/RT (1/V) */


/* Fonctions */
static int    pm(char *s) ;
static double concentration_oh(double,elem_t*,double,double,double) ;
static double poly4(double,double,double,double,double) ;
static void   transfert(Element_t*,double**,double*) ;
static void   flux(Element_t*,double**) ;

static double* ComputeAqueousVariables(Element_t*,double**,int) ;
static void    ComputeAqueousParameters(Element_t*,double*) ;
static double* ComputeAqueousDerivatives(Element_t*,double*,double,int) ;
static double* ComputeAqueousDerivatives1(Element_t*,double*,double,int) ;

#define NEGEXP(y)  ((y < 0.) ? exp(y) : 1.)
#define DNEGEXP(y) ((y < 0.) ? exp(y) : 0.)

/* Ion Activity Products */
#define IAP_CSH2(zc_h2so4,zn_ca_s)       (K_CSH2*NEGEXP(zn_ca_s)*MIN(zc_h2so4,1.))
#define IAP_CH(zc_h2so4,zn_ca_s)         (K_CH*NEGEXP(zn_ca_s)/MAX(zc_h2so4,1.))
#define IAP_SH(zq_ch,zn_si_s)            (K_SH*NEGEXP(zn_si_s)*Q_SH(zq_ch))
#define DIAP_SHSDQ_CH(zq_ch,zn_si_s)     (K_SH*NEGEXP(zn_si_s)*DQ_SH(zq_ch))
#define DIAP_SHSDZN_Si_S(zq_ch,zn_si_s)  (K_SH*DNEGEXP(zn_si_s)*Q_SH(zq_ch))


#define GetProperty(a)   (Element_GetProperty(el)[pm(a)])


int SetModelProp(Model_t *model)
{
  Model_GetNbOfEquations(model) = NEQ ;
  
  Model_CopyNameOfEquation(model,E_S,"carbone") ;
  Model_CopyNameOfEquation(model,E_Ca,"calcium") ;
  Model_CopyNameOfEquation(model,E_Si,"silicium") ;
  Model_CopyNameOfEquation(model,E_q,"charge") ;
  Model_CopyNameOfEquation(model,E_K,"potassium") ;

#if (U_H2SO4 == LOG_RHO)
  Model_CopyNameOfUnknown(model,I_C_H2SO4,"logc_h2so4") ;
#else
  Model_CopyNameOfUnknown(model,I_C_H2SO4,"c_h2so4") ;
#endif
  Model_CopyNameOfUnknown(model,I_ZN_Ca_S,"z_ca") ;
  Model_CopyNameOfUnknown(model,I_PSI,"psi") ;
  Model_CopyNameOfUnknown(model,I_ZN_Si_S,"z_si") ;
  Model_CopyNameOfUnknown(model,I_C_K,"c_k") ;
  
  return(0) ;
}


/* Parametres */
static double phi0,c_h2so4_eq,t_ch,t_csh2 ;
static double n_ca_ref,n_si_ref ;

#define NbOfAqueousVariables    (24)
static double var[NbOfAqueousVariables],dvar[NbOfAqueousVariables] ;

#define I_C_OH         (5)
#define I_C_H          (6)
#define I_C_HSO4       (7)
#define I_C_SO4        (8)
#define I_C_Ca         (9)
#define I_C_CaOH       (10)
#define I_C_CaHSO4     (11)
#define I_C_CaSO4aq    (12)
#define I_C_H2SiO4     (13)
#define I_C_H3SiO4     (14)
#define I_C_H4SiO4     (15)
#define I_C_CaH2SiO4   (16)
#define I_C_CaH3SiO4   (17)
#define I_ZQ_CH        (18)
#define I_N_Q          (19)
#define I_C_S_L        (20)
#define I_C_Ca_L       (21)
#define I_C_Si_L       (22)
#define I_C_K_L        (23)


int pm(char *s)
{
  if(strcmp(s,"porosite") == 0)      return (0) ;
  else if(strcmp(s,"N_CH") == 0)     return (1) ;
  else if(strcmp(s,"N_Si") == 0)     return (2) ;
  else if(strcmp(s,"C_H2SO4_eq") == 0) return (3) ;
  else if(strcmp(s,"T_CH") == 0)     return (4) ;
  else if(strcmp(s,"T_CSH2") == 0)     return (5) ;
  else return(-1) ;
}


int ReadMatProp(Material_t *mat,DataFile_t *datafile)
/* Lecture des donnees materiaux dans le fichier ficd */
{
  int  n_donnees = 6 ;

  {
    /* initialisation automatique */
    double t_ch        = 600. ;
    double t_csh2      = 0. ;
    double n_ca_ref    = 1. ;
    double n_si_ref    = 1. ;

    Material_GetProperty(mat)[pm("N_CH")] = n_ca_ref ;
    Material_GetProperty(mat)[pm("N_Si")] = n_si_ref ;
    Material_GetProperty(mat)[pm("T_CH")] = t_ch ;
    Material_GetProperty(mat)[pm("T_CSH2")] = t_csh2 ;

    Material_ScanProperties(mat,datafile,pm) ;

    t_ch      = Material_GetProperty(mat)[pm("T_CH")] ;
    t_csh2    = Material_GetProperty(mat)[pm("T_CSH2")] ;

    if(t_csh2  == 0.) Material_GetProperty(mat)[pm("T_CSH2")]  = t_ch ;

    Material_GetProperty(mat)[pm("C_H2SO4_eq")] = C_H2SO4_eq ;
  }
  
  return(n_donnees) ;
}



int PrintModelChar(Model_t *model,FILE *ficd)
/* Saisie des donnees materiaux */
{
  
  printf(TITLE) ;
  
  if(!ficd) return(NEQ) ;
  
  printf("\n\n") ;
  printf("Le systeme est forme de 5 equations:\n") ;
#if (U_H2SO4 == LOG_RHO)
  printf("\t- la conservation de la masse de C      (logc_h2so4)\n") ;
#else
  printf("\t- la conservation de la masse de C      (c_h2so4)\n") ;
#endif
  printf("\t- la conservation de la charge          (psi)\n") ;
  printf("\t- la conservation de la masse de Ca     (zn_ca_s)\n") ;
  printf("\t- la conservation de la masse de Si     (zn_si_s)\n") ;
  printf("\t- la conservation de la masse de K      (c_k)\n") ;

  printf("\n\
ATTENTION aux unites : \n\
\t longueur : dm !\n\
\t temps    : s !\n") ;

  printf("Exemple de donnees\n\n") ;

  fprintf(ficd,"porosite = 0.38   # La porosite\n") ;
  fprintf(ficd,"N_CH  = 6.1       # Contenu en CH (moles/L)\n") ;
  fprintf(ficd,"N_Si  = 2.4       # contenu en Si solide (moles/L)\n") ;
  fprintf(ficd,"N_K   = 0.4       # contenu en K (moles/L)\n") ;
  fprintf(ficd,"T_CH  = 1.e5      # Cinetique de dissolution de CH (s)\n") ;
  fprintf(ficd,"T_CSH2  = 1.e5      # Cinetique de dissolution de CSH2 (s)\n") ;
  fprintf(ficd,"courbes = solid   # Nom du fichier: q_ch X Y V f_S\n") ;

  return(NEQ) ;
}


int DefineElementProp(Element_t *el,IntFcts_t *intfcts)
{
  Element_GetNbOfImplicitTerms(el) = NVI ;
  Element_GetNbOfExplicitTerms(el) = NVE ;
  Element_GetNbOfConstantTerms(el) = NV0 ;
  return(0) ;
}


int  ComputeLoads(Element_t *el,double t,double dt,Load_t *cg,double *r)
/* Residu du aux chargements (r) */
{
  int nn = Element_GetNbOfNodes(el) ;
  FVM_t *fvm = FVM_GetInstance(el) ;
  int    i ;

  {
    double *r1 = FVM_ComputeSurfaceLoadResidu(fvm,cg,t,dt) ;
    
    for(i = 0 ; i < NEQ*nn ; i++) r[i] = -r1[i] ;
  }
  return(0) ;
}


int ComputeInitialState(Element_t *el)
/* Initialise les variables du systeme (f,va) */ 
{
  double *f = Element_GetImplicitTerm(el) ;
  double *v0 = Element_GetConstantTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  double *u[MAX_NOEUDS] ;
  int i ;
  
  for(i = 0 ; i < nn ; i++) {
    u[i] = Element_GetNodalUnknown(el,i) ;
  }
  
  /*
    Donnees
  */
  phi0      = Element_GetProperty(el)[pm("porosite")] ;
  n_ca_ref  = Element_GetProperty(el)[pm("N_CH")] ;
  n_si_ref  = Element_GetProperty(el)[pm("N_Si")] ;
  c_h2so4_eq  = Element_GetProperty(el)[pm("C_H2SO4_eq")] ;
  
  /* Default initialization */
  for(i = 0 ; i < nn ; i++) {
    double c_h2so4      = (C_H2SO4(i) > 0.) ? C_H2SO4(i) : c_h2so4_eq ;

#if (U_H2SO4 == LOG_RHO)
    UNKNOWN(i,I_C_H2SO4) = log(c_h2so4)/Ln10 ;
#else
    C_H2SO4(i)   = c_h2so4 ;
#endif
  }
  
  
  for(i = 0 ; i < nn ; i++) {
    /* molarities */
    double *x = ComputeAqueousVariables(el,u,i) ;
    
    double c_h2so4    = x[I_C_H2SO4] ;
    double zn_si_s    = x[I_ZN_Si_S] ;
    double zn_ca_s    = x[I_ZN_Ca_S] ;
    
    double zc_h2so4   = c_h2so4/c_h2so4_eq ;
    double zq_ch      = x[I_ZQ_CH] ;
    
    /* solid contents : CH, CSH2, CSH */
    double n_ch_eq    = n_ca_ref*MAX(zn_ca_s,0.) ;
    double n_csh2_eq  = n_ca_ref*MAX(zn_ca_s,0.) ;
    double n_ch       = (zc_h2so4 <= 1) ? n_ch_eq  : 0 ;
    double n_csh2     = (zc_h2so4 >  1) ? n_csh2_eq  : 0 ;
    double x_csh      = X_CSH(zq_ch) ;
    double n_si_s     = n_si_ref*MAX(zn_si_s,0.) ;
    double n_ca_s     = n_ch + n_csh2 + x_csh*n_si_s ;
    double n_s_s      = n_csh2 ;

    /* porosity */
    double phi = phi0 ;

    /* liquid contents */
    double n_s_l      = phi*x[I_C_S_L] ;
    double n_ca_l     = phi*x[I_C_Ca_L] ;
    double n_si_l     = phi*x[I_C_Si_L] ;
    double n_k_l      = phi*x[I_C_K_L] ;
    
    N_S(i)  = n_s_l  + n_s_s ;
    N_Ca(i) = n_ca_l + n_ca_s ;
    N_Si(i) = n_si_l + n_si_s ;
    N_K(i)  = n_k_l ;
    /* charge density */
    N_q(i)  = x[I_N_Q] ;

    N_CH(i)    = n_ch ;
    N_CSH2(i)  = n_csh2 ;
    N_Si_S(i)  = n_si_s ;
    ZQ_CH(i)   = zq_ch ;
    PHI(i)     = phi ;

    N_CH0(i)   = n_ch ;
    N_CSH20(i) = n_csh2 ;
    N_Si_S0(i) = n_si_s ;
    ZQ_CH0(i)  = zq_ch ;
  }
  
  if(Element_IsSubmanifold(el)) return(0) ;

  /* Coefficient de transfert */
  transfert(el,u,f) ;

  /* Flux */
  flux(el,u) ;
  return(0) ;
}


int  ComputeExplicitTerms(Element_t *el,double t)
/* Thermes explicites (va)  */
{
  double *f = Element_GetPreviousImplicitTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  double *u[MAX_NOEUDS] ;
  int i ;
  
  for(i = 0 ; i < nn ; i++) {
    u[i] = Element_GetPreviousNodalUnknown(el,i) ;
  }
  
  if(Element_IsSubmanifold(el)) return(0) ;
  
  /*
    Coefficients de transfert
  */
  transfert(el,u,f) ;

  return(0) ;
}


int  ComputeImplicitTerms(Element_t *el,double t,double dt)
/* Les variables donnees par la loi de comportement (f_1) */
{
  double *f = Element_GetCurrentImplicitTerm(el) ;
  double *f_n = Element_GetPreviousImplicitTerm(el) ;
  double *v0 = Element_GetConstantTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  double *u[MAX_NOEUDS] ;
  int i ;
  
  for(i = 0 ; i < nn ; i++) {
    u[i] = Element_GetNodalUnknown(el,i) ;
  }
  
  /*
    Donnees
  */
  phi0      = Element_GetProperty(el)[pm("porosite")] ;
  n_ca_ref  = Element_GetProperty(el)[pm("N_CH")] ;
  n_si_ref  = Element_GetProperty(el)[pm("N_Si")] ;
  c_h2so4_eq  = Element_GetProperty(el)[pm("C_H2SO4_eq")] ;
  t_ch      = Element_GetProperty(el)[pm("T_CH")] ;
  t_csh2    = Element_GetProperty(el)[pm("T_CSH2")] ;
  
  
  /* Contenus molaires */
  for(i = 0 ; i < nn ; i++) {
    /* molarities */
    double *x = ComputeAqueousVariables(el,u,i) ;
    
    double c_h2so4    = x[I_C_H2SO4] ;
    double zn_si_s    = x[I_ZN_Si_S] ;
    double zn_ca_s    = x[I_ZN_Ca_S] ;
    
    double zc_h2so4   = c_h2so4/c_h2so4_eq ;
    double zq_ch      = x[I_ZQ_CH] ;
    
    /* solid contents : CH, CSH2, CSH */
    double n_chn      = N_CHn(i) ;
    double n_csh2n    = N_CSH2n(i) ;
    /*   kinetics */
    double n_ch_ci    = n_chn*pow(zc_h2so4,-dt/t_ch) ;  /* if zc_h2so4 > 1 */
    double n_csh2_ci  = n_csh2n*pow(zc_h2so4,dt/t_csh2) ;   /* if zc_h2so4 < 1 */
    /*   equilibriums */
    double n_ch_eq    = n_ca_ref*MAX(zn_ca_s,0.) ;
    double n_csh2_eq  = n_ca_ref*MAX(zn_ca_s,0.) ;
    double n_ch       = (zc_h2so4 <= 1) ? n_ch_eq   : n_ch_ci ;
    double n_csh2     = (zc_h2so4 >  1) ? n_csh2_eq : n_csh2_ci ;
    double x_csh      = X_CSH(zq_ch) ;
    double n_si_s     = n_si_ref*MAX(zn_si_s,0.) ;
    double n_ca_s     = n_ch + n_csh2 + x_csh*n_si_s ;
    double n_s_s      = n_csh2 ;

    /* porosity */
    double n_ch0      = N_CH0(i) ;
    double n_csh20    = N_CSH20(i) ;
    double n_si_s0    = N_Si_S0(i) ;
    double zq_ch0     = ZQ_CH0(i) ;
    double v_csh0     = V_CSH(zq_ch0) ;
    double v_csh      = V_CSH(zq_ch) ;
    double phi1       = phi0 + V_CH*(n_ch0 - n_ch) + V_CSH2*(n_csh20 - n_csh2) \
                      + v_csh0*n_si_s0 - v_csh*n_si_s ;
#if (U_PHI == IMPLICIT)
    double phi        = phi1 ;
#else
    double phi        = PHIn(i) ;
#endif

    /* liquid contents */
    double n_s_l      = phi*x[I_C_S_L] ;
    double n_ca_l     = phi*x[I_C_Ca_L] ;
    double n_si_l     = phi*x[I_C_Si_L] ;
    double n_k_l      = phi*x[I_C_K_L] ;
    
    /* Molar contents */
    N_S(i)  = n_s_l  + n_s_s ;
    N_Ca(i) = n_ca_l + n_ca_s ;
    N_Si(i) = n_si_l + n_si_s ;
    N_K(i)  = n_k_l ;
    /* charge density */
    N_q(i)  = x[I_N_Q] ;

    N_CH(i)    = n_ch ;
    N_CSH2(i)  = n_csh2 ;
    N_Si_S(i)  = n_si_s ;
    ZQ_CH(i)   = zq_ch ;
    PHI(i)     = phi1 ;

    if(c_h2so4 < 0. || n_ca_s < 0. || n_si_s < 0.) {
      double xx = Element_GetNodeCoordinate(el,i)[0] ;
      double c_h3sio4 = x[I_C_H3SiO4] ;
      double c_oh     = x[I_C_OH] ;
      printf("x         = %e\n",xx) ;
      printf("c_h2so4   = %e\n",c_h2so4) ;
      printf("n_csh2    = %e\n",n_csh2) ;
      printf("n_ca_s    = %e\n",n_ca_s) ;
      printf("n_si_s    = %e\n",n_si_s) ;
      printf("zn_si_s   = %e\n",zn_si_s) ;
      printf("zn_ca_s   = %e\n",zn_ca_s) ;
      printf("c_h3sio4  = %e\n",c_h3sio4) ;
      printf("c_oh      = %e\n",c_oh) ;
      return(-1) ;
    }
    if(phi < 0.) {
      printf("phi = %e\n",phi) ;
      return(-1) ;
    }
  }
  
  if(Element_IsSubmanifold(el)) return(0) ;

  /* Flux */
  flux(el,u) ;

  return(0) ;
}


int  ComputeMatrix(Element_t *el,double t,double dt,double *k)
/* Matrice (k) */
{
#define K(i,j)    (k[(i)*2*NEQ+(j)])
  Symmetry_t sym = Element_GetSymmetry(el) ;
  double *f = Element_GetCurrentImplicitTerm(el) ;
  double *f_n = Element_GetPreviousImplicitTerm(el) ;
  double *va = Element_GetExplicitTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  double dx,xm ;
  double volume[2],surf ;
  int    i ;
  double c[2] ;

  double Dc_hSDc_h2so4[2]        ;
  double Dc_ohSDc_h2so4[2]       ;
  double Dc_hso4SDc_h2so4[2]     ;
  double Dc_so4SDc_h2so4[2]      ;
  double Dc_caSDc_h2so4[2]       ;
  double Dc_cahso4SDc_h2so4[2]   ;
  double Dc_h3sio4SDc_h2so4[2]   ;
  double Dc_h4sio4SDc_h2so4[2]   ;
  double Dc_cah3sio4SDc_h2so4[2] ;
  double Dc_h2sio4SDc_h2so4[2]   ;
  double Dc_cah2sio4SDc_h2so4[2] ;
  double Dc_caso4aqSDc_h2so4[2]  ;  
  double Dc_caohSDc_h2so4[2]     ;
  
  double Dc_hSDzn_si_s[2]        ;
  double Dc_ohSDzn_si_s[2]       ;
  double Dc_hso4SDzn_si_s[2]     ;
  double Dc_so4SDzn_si_s[2]      ;
  double Dc_caSDzn_si_s[2]       ;
  double Dc_cahso4SDzn_si_s[2]   ;
  double Dc_h3sio4SDzn_si_s[2]   ;
  double Dc_h4sio4SDzn_si_s[2]   ;
  double Dc_cah3sio4SDzn_si_s[2] ;
  double Dc_h2sio4SDzn_si_s[2]   ;
  double Dc_cah2sio4SDzn_si_s[2] ;
  double Dc_caso4aqSDzn_si_s[2]  ;
  double Dc_caohSDzn_si_s[2]     ;

  double Dc_hSDzn_ca_s[2]        ;
  double Dc_ohSDzn_ca_s[2]       ;
  double Dc_hso4SDzn_ca_s[2]     ;
  double Dc_so4SDzn_ca_s[2]      ;
  double Dc_caSDzn_ca_s[2]       ;
  double Dc_cahso4SDzn_ca_s[2]   ;
  double Dc_h3sio4SDzn_ca_s[2]   ;
  double Dc_h4sio4SDzn_ca_s[2]   ;
  double Dc_cah3sio4SDzn_ca_s[2] ;
  double Dc_h2sio4SDzn_ca_s[2]   ;
  double Dc_cah2sio4SDzn_ca_s[2] ;
  double Dc_caso4aqSDzn_ca_s[2]  ;  
  double Dc_caohSDzn_ca_s[2]     ;
  
  double Dc_hSDc_k[2]         ;
  double Dc_ohSDc_k[2]        ;
  double Dc_hso4SDc_k[2]      ;
  double Dc_so4SDc_k[2]       ;
  double Dc_caSDc_k[2]        ;
  double Dc_cahso4SDc_k[2]    ;
  double Dc_h3sio4SDc_k[2]    ;
  double Dc_cah3sio4SDc_k[2]  ;
  double Dc_h2sio4SDc_k[2]    ;
  double Dc_cah2sio4SDc_k[2]  ;
  double Dc_caso4aqSDc_k[2]   ;  
  double Dc_caohSDc_k[2]      ;
  double *u[MAX_NOEUDS] ;
  double *u_n[MAX_NOEUDS] ;
  
  for(i = 0 ; i < nn ; i++) {
    u[i] = Element_GetNodalUnknown(el,i) ;
    u_n[i] = Element_GetPreviousNodalUnknown(el,i) ;
  }
  
  /*
    Donnees
  */
  phi0      = Element_GetProperty(el)[pm("porosite")] ;
  n_ca_ref  = Element_GetProperty(el)[pm("N_CH")] ;
  n_si_ref  = Element_GetProperty(el)[pm("N_Si")] ;
  c_h2so4_eq  = Element_GetProperty(el)[pm("C_H2SO4_eq")] ;
  t_ch      = Element_GetProperty(el)[pm("T_CH")] ;
  t_csh2      = Element_GetProperty(el)[pm("T_CSH2")] ;


  /*
    Initialisation 
  */
  for(i=0;i<nn*nn*NEQ*NEQ;i++) k[i] = 0. ;

  if(Element_IsSubmanifold(el)) return(0) ;

  /*
    CALCUL DE volume ET DE surf 
  */
  {
    double x1 = Element_GetNodeCoordinate(el,1)[0] ;
    double x0 = Element_GetNodeCoordinate(el,0)[0] ;
    dx = x1 - x0 ;
    xm = (x1 + x0)*0.5 ;
  }
  for(i=0;i<nn;i++) {
    double x = Element_GetNodeCoordinate(el,i)[0] ;
    volume[i] = fabs(dx)*0.5 ; 
    if(sym == AXIS) volume[i] *= M_PI*(x + xm) ; 
  }
  if(sym == AXIS) surf = 2*M_PI*xm ; else surf = 1 ;
  /*
    termes d'accumulation
  */
  for(i=0;i<nn;i++) {
    /* molarities */
    double *x = ComputeAqueousVariables(el,u,i) ;

    double c_h2so4    = x[I_C_H2SO4] ;
    double zn_si_s    = x[I_ZN_Si_S];
    double zn_ca_s    = x[I_ZN_Ca_S] ;
    double zc_h2so4   = c_h2so4/c_h2so4_eq ;
    double zq_ch      = x[I_ZQ_CH] ;
    
    /* solid contents : CH, CSH2, CSH */
    double n_csh2n    = N_CSH2n(i) ;
    double n_chn      = N_CHn(i) ;
    /*   kinetics */
    double n_ch_ci    = n_chn*pow(zc_h2so4,-dt/t_ch) ;
    double n_csh2_ci  = n_csh2n*pow(zc_h2so4,dt/t_csh2) ;
    double n_si_s     = N_Si_S(i) ;
    double x_csh      = X_CSH(zq_ch) ;

    /* porosity */
    double v_csh      = V_CSH(zq_ch) ;
#if (U_PHI == IMPLICIT)
    double phi        = PHI(i) ;
#else
    double phi        = PHIn(i) ;
#endif

    /* Atom concentrations in the liquid phase */
    double c_s_l      = x[I_C_S_L] ;
    double c_ca_l     = x[I_C_Ca_L] ;
    double c_si_l     = x[I_C_Si_L] ;
    double c_k_l      = x[I_C_K_L] ;
    

    int j = i*NEQ ;

    /* derivatives ... */
    /* ... with respect to c_h2so4 */
    {
      double dc_h2so4     = 1.e-6*c_h2so4*((c_h2so4 > C_H2SO4n(i)) ? 1 : -1) ;
      double *dcsdc_h2so4 = ComputeAqueousDerivatives(el,x,dc_h2so4,I_C_H2SO4) ;
		                    
      /* solid */
      double dn_ch_cisdc_h2so4   = n_ch_ci*(-dt/t_ch)/c_h2so4 ;
      double dn_csh2_cisdc_h2so4 = n_csh2_ci*(dt/t_csh2)/c_h2so4 ;
      double dn_chsdc_h2so4      = (zc_h2so4 <= 1) ? 0  : dn_ch_cisdc_h2so4 ;
      double dn_csh2sdc_h2so4    = (zc_h2so4 >  1) ? 0  : dn_csh2_cisdc_h2so4 ;
      
      double dzq_chsdc_h2so4     = dcsdc_h2so4[I_ZQ_CH] ;
      double dx_cshsdc_h2so4     = DX_CSH(zq_ch)*dzq_chsdc_h2so4 ;
      
      double dn_ca_ssdc_h2so4    = dn_chsdc_h2so4 + dn_csh2sdc_h2so4 \
                                 + dx_cshsdc_h2so4*n_si_s ;
      double dn_si_ssdc_h2so4    = 0. ;
      double dn_s_ssdc_h2so4     = dn_csh2sdc_h2so4 ;
      
      /* porosity */
#if (U_PHI == IMPLICIT)
      double dv_cshsdc_h2so4     = DV_CSH(zq_ch)*dzq_chsdc_h2so4 ;
      double dphisdc_h2so4       = - V_CH*dn_chsdc_h2so4 - V_CSH2*dn_csh2sdc_h2so4 \
                                 - dv_cshsdc_h2so4*n_si_s ;
#else
      double dphisdc_h2so4       = 0 ;
#endif
    
      /* liquid */
      double dc_s_lsdc_h2so4     = dcsdc_h2so4[I_C_S_L] ;
      double dc_ca_lsdc_h2so4    = dcsdc_h2so4[I_C_Ca_L] ;
      double dc_si_lsdc_h2so4    = dcsdc_h2so4[I_C_Si_L] ;
      double dc_k_lsdc_h2so4     = dcsdc_h2so4[I_C_K_L] ;
      double dn_s_lsdc_h2so4     = phi*dc_s_lsdc_h2so4  + dphisdc_h2so4*c_s_l ;
      double dn_ca_lsdc_h2so4    = phi*dc_ca_lsdc_h2so4 + dphisdc_h2so4*c_ca_l ;
      double dn_si_lsdc_h2so4    = phi*dc_si_lsdc_h2so4 + dphisdc_h2so4*c_si_l ;
      double dn_k_lsdc_h2so4     = phi*dc_k_lsdc_h2so4  + dphisdc_h2so4*c_k_l ;
    
      /* Balance of S (sulfur)  : (n_S1 - n_Sn) + dt * div(w_S) = 0 */
      K(E_S+j,I_C_H2SO4+j)   += volume[i]*(dn_s_lsdc_h2so4 + dn_s_ssdc_h2so4) ;
      /* Balance of Ca (calcium) : (n_Ca1 - n_Can) + dt * div(w_Ca) = 0 */
      K(E_Ca+j,I_C_H2SO4+j)  += volume[i]*(dn_ca_lsdc_h2so4 + dn_ca_ssdc_h2so4) ;
      /* Balance of Si (silicon) : (n_Si1 - n_Sin) + dt * div(w_Si) = 0 */
      K(E_Si+j,I_C_H2SO4+j)  += volume[i]*(dn_si_lsdc_h2so4 + dn_si_ssdc_h2so4) ;
      /* Balance of K (potassium): (n_K1 - n_Kn) + dt * div(w_K) = 0 */
      K(E_K+j,I_C_H2SO4+j)   += volume[i]*(dn_k_lsdc_h2so4) ;
    

      /* sauvegardes pour les termes de transport */
      Dc_hSDc_h2so4[i]        = dcsdc_h2so4[I_C_H] ;
      Dc_ohSDc_h2so4[i]       = dcsdc_h2so4[I_C_OH] ;
      Dc_hso4SDc_h2so4[i]     = dcsdc_h2so4[I_C_HSO4] ;
      Dc_so4SDc_h2so4[i]      = dcsdc_h2so4[I_C_SO4] ;
      Dc_caSDc_h2so4[i]       = dcsdc_h2so4[I_C_Ca] ;
      Dc_cahso4SDc_h2so4[i]   = dcsdc_h2so4[I_C_CaHSO4] ;
      Dc_h3sio4SDc_h2so4[i]   = dcsdc_h2so4[I_C_H3SiO4] ;
      Dc_h4sio4SDc_h2so4[i]   = dcsdc_h2so4[I_C_H4SiO4] ;
      Dc_cah3sio4SDc_h2so4[i] = dcsdc_h2so4[I_C_CaH3SiO4] ;
      Dc_h2sio4SDc_h2so4[i]   = dcsdc_h2so4[I_C_H2SiO4] ;
      Dc_cah2sio4SDc_h2so4[i] = dcsdc_h2so4[I_C_CaH2SiO4] ;
      Dc_caso4aqSDc_h2so4[i]  = dcsdc_h2so4[I_C_CaSO4aq] ;
      Dc_caohSDc_h2so4[i]     = dcsdc_h2so4[I_C_CaOH] ;
    }
			                    
    /* with respect to zn_si_s */
    {
      double dzn_si_s     = 1.e-8*((zn_si_s > ZN_Si_Sn(i)) ? 1 : -1) ; 
      double *dcsdzn_si_s = ComputeAqueousDerivatives(el,x,dzn_si_s,I_ZN_Si_S) ;
 
      /* solid */
      double dn_si_ssdzn_si_s    = (zn_si_s > 0) ? n_si_ref : 0. ;
      double dn_ca_ssdzn_si_s    = x_csh*dn_si_ssdzn_si_s ;
    
      /* porosity */
#if (U_PHI == IMPLICIT)
      double dphisdzn_si_s       = -v_csh*dn_si_ssdzn_si_s ;
#else
      double dphisdzn_si_s       = 0 ;
#endif
    
      /* liquid */
      double dc_s_lsdzn_si_s     = dcsdzn_si_s[I_C_S_L] ;
      double dc_ca_lsdzn_si_s    = dcsdzn_si_s[I_C_Ca_L] ;
      double dc_si_lsdzn_si_s    = dcsdzn_si_s[I_C_Si_L] ;
      double dc_k_lsdzn_si_s     = dcsdzn_si_s[I_C_K_L] ;      
      double dn_s_lsdzn_si_s     = phi*dc_s_lsdzn_si_s  + dphisdzn_si_s*c_s_l ;
      double dn_ca_lsdzn_si_s    = phi*dc_ca_lsdzn_si_s + dphisdzn_si_s*c_ca_l ;
      double dn_si_lsdzn_si_s    = phi*dc_si_lsdzn_si_s + dphisdzn_si_s*c_si_l ;
      double dn_k_lsdzn_si_s     = phi*dc_k_lsdzn_si_s  + dphisdzn_si_s*c_k_l ;
    
      K(E_S+j,I_ZN_Si_S+j)   += volume[i]*(dn_s_lsdzn_si_s) ;
      K(E_Ca+j,I_ZN_Si_S+j)  += volume[i]*(dn_ca_lsdzn_si_s + dn_ca_ssdzn_si_s) ;
      K(E_Si+j,I_ZN_Si_S+j)  += volume[i]*(dn_si_lsdzn_si_s + dn_si_ssdzn_si_s) ;
      K(E_K+j,I_ZN_Si_S+j)   += volume[i]*(dn_k_lsdzn_si_s) ;
    
      /* sauvegardes pour les termes de transport */
      Dc_hSDzn_si_s[i]        = dcsdzn_si_s[I_C_H] ;
      Dc_ohSDzn_si_s[i]       = dcsdzn_si_s[I_C_OH] ;
      Dc_hso4SDzn_si_s[i]     = dcsdzn_si_s[I_C_HSO4] ;
      Dc_so4SDzn_si_s[i]      = dcsdzn_si_s[I_C_SO4] ;   
      Dc_caSDzn_si_s[i]       = dcsdzn_si_s[I_C_Ca] ;
      Dc_cahso4SDzn_si_s[i]   = dcsdzn_si_s[I_C_CaHSO4] ;
      Dc_h3sio4SDzn_si_s[i]   = dcsdzn_si_s[I_C_H3SiO4] ;
      Dc_h4sio4SDzn_si_s[i]   = dcsdzn_si_s[I_C_H4SiO4] ;
      Dc_cah3sio4SDzn_si_s[i] = dcsdzn_si_s[I_C_CaH3SiO4] ;
      Dc_h2sio4SDzn_si_s[i]   = dcsdzn_si_s[I_C_H2SiO4] ;
      Dc_cah2sio4SDzn_si_s[i] = dcsdzn_si_s[I_C_CaH2SiO4] ;
      Dc_caso4aqSDzn_si_s[i]  = dcsdzn_si_s[I_C_CaSO4aq] ;
      Dc_caohSDzn_si_s[i]     = dcsdzn_si_s[I_C_CaOH] ;  
    }
    
    /* with respect to zn_ca_s */
    {
      double dzn_ca_s            = 1.e-10*((zn_ca_s > ZN_Ca_Sn(i)) ? 1 : -1) ;
      double *dcsdzn_ca_s = ComputeAqueousDerivatives(el,x,dzn_ca_s,I_ZN_Ca_S) ;

      /* solid */
      double dzq_chsdzn_ca_s    = dcsdzn_ca_s[I_ZQ_CH] ;
      double dx_cshsdzn_ca_s    = DX_CSH(zq_ch)*dzq_chsdzn_ca_s ;     

      double dn_ch_eqsdzn_ca_s   = (zn_ca_s > 0) ? n_ca_ref : 0 ;
      double dn_csh2_eqsdzn_ca_s = (zn_ca_s > 0) ? n_ca_ref : 0 ;
      double dn_chsdzn_ca_s      = (zc_h2so4 <= 1) ? dn_ch_eqsdzn_ca_s    : 0 ;
      double dn_csh2sdzn_ca_s    = (zc_h2so4 >  1) ? dn_csh2_eqsdzn_ca_s  : 0 ;
    
      double dn_si_ssdzn_ca_s    = 0. ;
      double dn_ca_ssdzn_ca_s    = dn_chsdzn_ca_s + dn_csh2sdzn_ca_s \
                                 + dx_cshsdzn_ca_s*n_si_s ;
      double dn_s_ssdzn_ca_s     = dn_csh2sdzn_ca_s ;

      /* porosity */
#if (U_PHI == IMPLICIT)
      double dv_cshsdzn_ca_s     = DV_CSH(zq_ch)*dzq_chsdzn_ca_s ;
      double dphisdzn_ca_s       = - V_CH*dn_chsdzn_ca_s \
                                 - V_CSH2*dn_csh2sdzn_ca_s \
                                 - dv_cshsdzn_ca_s*n_si_s ;
#else
      double dphisdzn_ca_s       = 0 ;
#endif

      /* liquid */
      double dc_s_lsdzn_ca_s     = dcsdzn_ca_s[I_C_S_L] ;
      double dc_ca_lsdzn_ca_s    = dcsdzn_ca_s[I_C_Ca_L] ;
      double dc_si_lsdzn_ca_s    = dcsdzn_ca_s[I_C_Si_L] ;
      double dc_k_lsdzn_ca_s     = dcsdzn_ca_s[I_C_K_L] ;   
      double dn_s_lsdzn_ca_s     = phi*dc_s_lsdzn_ca_s  + dphisdzn_ca_s*c_s_l ;
      double dn_ca_lsdzn_ca_s    = phi*dc_ca_lsdzn_ca_s + dphisdzn_ca_s*c_ca_l ;
      double dn_si_lsdzn_ca_s    = phi*dc_si_lsdzn_ca_s + dphisdzn_ca_s*c_si_l ;
      double dn_k_lsdzn_ca_s     = phi*dc_k_lsdzn_ca_s  + dphisdzn_ca_s*c_k_l ;
    
      K(E_S+j,I_ZN_Ca_S+j)   += volume[i]*(dn_s_lsdzn_ca_s + dn_s_ssdzn_ca_s) ;
      K(E_Ca+j,I_ZN_Ca_S+j)  += volume[i]*(dn_ca_lsdzn_ca_s + dn_ca_ssdzn_ca_s) ;
      K(E_Si+j,I_ZN_Ca_S+j)  += volume[i]*(dn_si_lsdzn_ca_s + dn_si_ssdzn_ca_s) ;
      K(E_K+j,I_ZN_Ca_S+j)   += volume[i]*(dn_k_lsdzn_ca_s) ; 
    
      /* sauvegardes pour les termes de transport */
      Dc_hSDzn_ca_s[i]        = dcsdzn_ca_s[I_C_H] ;
      Dc_ohSDzn_ca_s[i]       = dcsdzn_ca_s[I_C_OH] ;
      Dc_hso4SDzn_ca_s[i]     = dcsdzn_ca_s[I_C_HSO4] ;
      Dc_so4SDzn_ca_s[i]      = dcsdzn_ca_s[I_C_SO4] ;
      Dc_caSDzn_ca_s[i]       = dcsdzn_ca_s[I_C_Ca] ;      
      Dc_cahso4SDzn_ca_s[i]   = dcsdzn_ca_s[I_C_CaHSO4] ;
      Dc_h3sio4SDzn_ca_s[i]   = dcsdzn_ca_s[I_C_H3SiO4] ;
      Dc_h4sio4SDzn_ca_s[i]   = dcsdzn_ca_s[I_C_H4SiO4] ;
      Dc_cah3sio4SDzn_ca_s[i] = dcsdzn_ca_s[I_C_CaH3SiO4] ;
      Dc_h2sio4SDzn_ca_s[i]   = dcsdzn_ca_s[I_C_H2SiO4] ;
      Dc_cah2sio4SDzn_ca_s[i] = dcsdzn_ca_s[I_C_CaH2SiO4] ;
      Dc_caso4aqSDzn_ca_s[i]  = dcsdzn_ca_s[I_C_CaSO4aq] ;
      Dc_caohSDzn_ca_s[i]     = dcsdzn_ca_s[I_C_CaOH] ;
    }
    
    /* ... with respect to c_k */
    {
      double dc_k                = 1.e-6 ;
      double *dcsdc_k = ComputeAqueousDerivatives(el,x,dc_k,I_C_K) ;
      
      /* liquid */
      double dc_s_lsdc_k     = dcsdc_k[I_C_S_L] ;
      double dc_ca_lsdc_k    = dcsdc_k[I_C_Ca_L] ;
      double dc_si_lsdc_k    = dcsdc_k[I_C_Si_L] ;
      double dc_k_lsdc_k     = dcsdc_k[I_C_K_L] ;
      double dn_s_lsdc_k     = phi*dc_s_lsdc_k ;
      double dn_ca_lsdc_k    = phi*dc_ca_lsdc_k ;
      double dn_si_lsdc_k    = phi*dc_si_lsdc_k ;
      double dn_k_lsdc_k     = phi*dc_k_lsdc_k ;
      
      K(E_S+j,I_C_K+j)     += volume[i]*(dn_s_lsdc_k) ; 
      K(E_Ca+j,I_C_K+j)    += volume[i]*(dn_ca_lsdc_k) ;
      K(E_Si+j,I_C_K+j)    += volume[i]*(dn_si_lsdc_k) ; 
      K(E_K+j,I_C_K+j)     += volume[i]*(dn_k_lsdc_k) ;
    
      /* sauvegardes pour les termes de transport */
      Dc_hSDc_k[i]        = dcsdc_k[I_C_H] ;
      Dc_ohSDc_k[i]       = dcsdc_k[I_C_OH] ;
      Dc_hso4SDc_k[i]     = dcsdc_k[I_C_HSO4] ;
      Dc_so4SDc_k[i]      = dcsdc_k[I_C_SO4] ;
      Dc_caSDc_k[i]       = dcsdc_k[I_C_Ca] ;      
      Dc_cahso4SDc_k[i]   = dcsdc_k[I_C_CaHSO4] ;
      Dc_h3sio4SDc_k[i]   = dcsdc_k[I_C_H3SiO4] ;
      Dc_cah3sio4SDc_k[i] = dcsdc_k[I_C_CaH3SiO4] ;
      Dc_h2sio4SDc_k[i]   = dcsdc_k[I_C_H2SiO4] ;
      Dc_cah2sio4SDc_k[i] = dcsdc_k[I_C_CaH2SiO4] ;
      Dc_caso4aqSDc_k[i]  = dcsdc_k[I_C_CaSO4aq] ;
      Dc_caohSDc_k[i]     = dcsdc_k[I_C_CaOH] ;    
    }

  }

  /* termes d'ecoulement */
  {
  double tr        = dt*surf/dx ;

  double trf_oh       = tr*KF_OH ;
  double trf_h        = tr*KF_H ;
  double trf_h2so4    = tr*KF_H2SO4 ;
  double trf_hso4     = tr*KF_HSO4 ;
  double trf_so4      = tr*KF_SO4 ;
  double trf_ca       = tr*KF_Ca ;
  double trf_cahso4   = tr*KF_CaHSO4 ;
  double trf_cah3sio4 = tr*KF_CaH3SiO4 ;
  double trf_h3sio4   = tr*KF_H3SiO4 ;
  double trf_h4sio4   = tr*KF_H4SiO4 ;
  double trf_h2sio4   = tr*KF_H2SiO4 ;
  double trf_cah2sio4 = tr*KF_CaH2SiO4 ;
  double trf_caso4aq  = tr*KF_CaSO4aq ;
  double trf_caoh     = tr*KF_CaOH ;
  double trf_k        = tr*KF_K ;
  
  double tre_hso4     = tr*Kpsi_HSO4 ;
  double tre_so4      = tr*Kpsi_SO4 ;
  double tre_ca       = tr*Kpsi_Ca ;
  double tre_cahso4   = tr*Kpsi_CaHSO4 ;
  double tre_cah3sio4 = tr*Kpsi_CaH3SiO4 ;
  double tre_h3sio4   = tr*Kpsi_H3SiO4 ;
  double tre_h2sio4   = tr*Kpsi_H2SiO4 ;
  double tre_caoh     = tr*Kpsi_CaOH ;
  double tre_k        = tr*Kpsi_K ;

  double tre_q        = tr*Kpsi_q ;
  
  /*
    Conservation de C (carbone) : (n_C1 - n_Cn) + dt * div(w_C) = 0
  */
  for(i=0;i<2;i++){
    c[i] = trf_h2so4 + trf_hso4*Dc_hso4SDc_h2so4[i] + trf_so4*Dc_so4SDc_h2so4[i] + trf_cahso4*Dc_cahso4SDc_h2so4[i] \
           + trf_caso4aq*Dc_caso4aqSDc_h2so4[i] ;
  }
  K(E_S,I_C_H2SO4)          += + c[0] ;
  K(E_S,I_C_H2SO4+NEQ)      += - c[1] ;
  K(E_S+NEQ,I_C_H2SO4)      += - c[0] ;
  K(E_S+NEQ,I_C_H2SO4+NEQ)  += + c[1] ;

  for(i=0;i<2;i++){
    c[i] = trf_hso4*Dc_hso4SDzn_si_s[i] + trf_so4*Dc_so4SDzn_si_s[i] + trf_cahso4*Dc_cahso4SDzn_si_s[i] + trf_caso4aq*Dc_caso4aqSDzn_si_s[i] ;
  }
  K(E_S,I_ZN_Si_S)          += + c[0] ;
  K(E_S,I_ZN_Si_S+NEQ)      += - c[1] ;
  K(E_S+NEQ,I_ZN_Si_S)      += - c[0] ;
  K(E_S+NEQ,I_ZN_Si_S+NEQ)  += + c[1] ;

  for(i=0;i<2;i++){
    c[i] = trf_hso4*Dc_hso4SDzn_ca_s[i] + trf_so4*Dc_so4SDzn_ca_s[i] + trf_cahso4*Dc_cahso4SDzn_ca_s[i] + trf_caso4aq*Dc_caso4aqSDzn_ca_s[i] ;
  }
  K(E_S,I_ZN_Ca_S)           += + c[0] ;
  K(E_S,I_ZN_Ca_S+NEQ)       += - c[1] ;
  K(E_S+NEQ,I_ZN_Ca_S)       += - c[0] ;
  K(E_S+NEQ,I_ZN_Ca_S+NEQ)   += + c[1] ;
  
  for(i=0;i<2;i++){
    c[i] = trf_hso4*Dc_hso4SDc_k[i] + trf_so4*Dc_so4SDc_k[i] + trf_cahso4*Dc_cahso4SDc_k[i] + trf_caso4aq*Dc_caso4aqSDc_k[i] ;
  }
  K(E_S,I_C_K)          += + c[0] ;
  K(E_S,I_C_K+NEQ)      += - c[1] ;
  K(E_S+NEQ,I_C_K)      += - c[0] ;
  K(E_S+NEQ,I_C_K+NEQ)  += + c[1] ;

  for(i=0;i<2;i++){
    c[i] = tre_hso4 + tre_so4 + tre_cahso4 ;
  }
  K(E_S,I_PSI)          += + c[0] ;
  K(E_S,I_PSI+NEQ)      += - c[1] ;
  K(E_S+NEQ,I_PSI)      += - c[0] ;
  K(E_S+NEQ,I_PSI+NEQ)  += + c[1] ;

  /*
    Conservation de Ca (calcium) : (n_Ca1 - n_Can) + dt * div(w_Ca) = 0
  */
  for(i=0;i<2;i++){
    c[i] = trf_ca*Dc_caSDc_h2so4[i] + trf_cahso4*Dc_cahso4SDc_h2so4[i] + trf_cah3sio4*Dc_cah3sio4SDc_h2so4[i] + trf_cah2sio4*Dc_cah2sio4SDc_h2so4[i] \
    + trf_caoh*Dc_caohSDc_h2so4[i] + trf_caso4aq*Dc_caso4aqSDc_h2so4[i] ;
  }
  K(E_Ca,I_C_H2SO4)         += + c[0] ;
  K(E_Ca,I_C_H2SO4+NEQ)     += - c[1] ;
  K(E_Ca+NEQ,I_C_H2SO4)     += - c[0] ;
  K(E_Ca+NEQ,I_C_H2SO4+NEQ) += + c[1] ;

  for(i=0;i<2;i++){
    c[i] = trf_ca*Dc_caSDzn_si_s[i] + trf_cahso4*Dc_cahso4SDzn_si_s[i] + trf_cah3sio4*Dc_cah3sio4SDzn_si_s[i] + trf_cah2sio4*Dc_cah2sio4SDzn_si_s[i] \
    + trf_caoh*Dc_caohSDzn_si_s[i] + trf_caso4aq*Dc_caso4aqSDzn_si_s[i] ;
  }
  K(E_Ca,I_ZN_Si_S)         += + c[0] ;
  K(E_Ca,I_ZN_Si_S+NEQ)     += - c[1] ;
  K(E_Ca+NEQ,I_ZN_Si_S)     += - c[0] ;
  K(E_Ca+NEQ,I_ZN_Si_S+NEQ) += + c[1] ;

  for(i=0;i<2;i++){
    c[i] = trf_ca*Dc_caSDzn_ca_s[i] + trf_cahso4*Dc_cahso4SDzn_ca_s[i] + trf_cah3sio4*Dc_cah3sio4SDzn_ca_s[i] + trf_cah2sio4*Dc_cah2sio4SDzn_ca_s[i] \
    + trf_caoh*Dc_caohSDzn_ca_s[i] + trf_caso4aq*Dc_caso4aqSDzn_ca_s[i] ;
  }
  K(E_Ca,I_ZN_Ca_S)         += + c[0] ;
  K(E_Ca,I_ZN_Ca_S+NEQ)     += - c[1] ;
  K(E_Ca+NEQ,I_ZN_Ca_S)     += - c[0] ;
  K(E_Ca+NEQ,I_ZN_Ca_S+NEQ) += + c[1] ;
  
  for(i=0;i<2;i++){
    c[i] = trf_ca*Dc_caSDc_k[i] + trf_cahso4*Dc_cahso4SDc_k[i] + trf_cah3sio4*Dc_cah3sio4SDc_k[i] + trf_cah2sio4*Dc_cah2sio4SDc_k[i] \
    + trf_caoh*Dc_caohSDc_k[i] + trf_caso4aq*Dc_caso4aqSDc_k[i] ;
  }
  K(E_Ca,I_C_K)         += + c[0] ;
  K(E_Ca,I_C_K+NEQ)     += - c[1] ;
  K(E_Ca+NEQ,I_C_K)     += - c[0] ;
  K(E_Ca+NEQ,I_C_K+NEQ) += + c[1] ; 

  for(i=0;i<2;i++){
    c[i] = tre_ca + tre_cahso4 + tre_cah3sio4 + tre_caoh;
  }
  K(E_Ca,I_PSI)          += + c[0] ;
  K(E_Ca,I_PSI+NEQ)      += - c[1] ;
  K(E_Ca+NEQ,I_PSI)      += - c[0] ;
  K(E_Ca+NEQ,I_PSI+NEQ)  += + c[1] ;
  /*
    Conservation de Si (silicium) : (n_Si1 - n_Sin) + dt * div(w_Si) = 0
  */
  for(i=0;i<2;i++){
    c[i] = trf_h3sio4*Dc_h3sio4SDc_h2so4[i] + trf_h4sio4*Dc_h4sio4SDc_h2so4[i] + trf_cah3sio4*Dc_cah3sio4SDc_h2so4[i] + trf_cah2sio4*Dc_cah2sio4SDc_h2so4[i] \
    + trf_h2sio4*Dc_h2sio4SDc_h2so4[i] ;
  }
  K(E_Si,I_C_H2SO4)         += + c[0] ;
  K(E_Si,I_C_H2SO4+NEQ)     += - c[1] ;
  K(E_Si+NEQ,I_C_H2SO4)     += - c[0] ;
  K(E_Si+NEQ,I_C_H2SO4+NEQ) += + c[1] ;

  for(i=0;i<2;i++){
    c[i] = trf_h3sio4*Dc_h3sio4SDzn_si_s[i] + trf_h4sio4*Dc_h4sio4SDzn_si_s[i] + trf_cah3sio4*Dc_cah3sio4SDzn_si_s[i]  + trf_cah2sio4*Dc_cah2sio4SDzn_si_s[i] \
    + trf_h2sio4*Dc_h2sio4SDzn_si_s[i];
  }
  K(E_Si,I_ZN_Si_S)         += + c[0] ;
  K(E_Si,I_ZN_Si_S+NEQ)     += - c[1] ;
  K(E_Si+NEQ,I_ZN_Si_S)     += - c[0] ;
  K(E_Si+NEQ,I_ZN_Si_S+NEQ) += + c[1] ;

  for(i=0;i<2;i++){
    c[i] = trf_h3sio4*Dc_h3sio4SDzn_ca_s[i] + trf_h4sio4*Dc_h4sio4SDzn_ca_s[i] + trf_cah3sio4*Dc_cah3sio4SDzn_ca_s[i]  + trf_cah2sio4*Dc_cah2sio4SDzn_ca_s[i] \
    + trf_h2sio4*Dc_h2sio4SDzn_ca_s[i];
  }
  K(E_Si,I_ZN_Ca_S)         += + c[0] ;
  K(E_Si,I_ZN_Ca_S+NEQ)     += - c[1] ;
  K(E_Si+NEQ,I_ZN_Ca_S)     += - c[0] ;
  K(E_Si+NEQ,I_ZN_Ca_S+NEQ) += + c[1] ;
  
  for(i=0;i<2;i++){
    c[i] = trf_h3sio4*Dc_h3sio4SDc_k[i] + trf_cah3sio4*Dc_cah3sio4SDc_k[i]  + trf_cah2sio4*Dc_cah2sio4SDc_k[i] \
    + trf_h2sio4*Dc_h2sio4SDc_k[i];
  }
  K(E_Si,I_C_K)         += + c[0] ;
  K(E_Si,I_C_K+NEQ)     += - c[1] ;
  K(E_Si+NEQ,I_C_K)     += - c[0] ;
  K(E_Si+NEQ,I_C_K+NEQ) += + c[1] ; 

  for(i=0;i<2;i++){
    c[i] = tre_h3sio4 + tre_cah3sio4 + tre_h2sio4 ;
  }
  K(E_Si,I_PSI)          += + c[0] ;
  K(E_Si,I_PSI+NEQ)      += - c[1] ;
  K(E_Si+NEQ,I_PSI)      += - c[0] ;
  K(E_Si+NEQ,I_PSI+NEQ)  += + c[1] ;
  /*
    Conservation de la charge  : div(w_q) = 0
  */

  for(i=0;i<2;i++){
    c[i] = z_h*trf_h*Dc_hSDc_h2so4[i] + z_oh*trf_oh*Dc_ohSDc_h2so4[i] + z_hso4*trf_hso4*Dc_hso4SDc_h2so4[i] + z_so4*trf_so4*Dc_so4SDc_h2so4[i] \
           + z_ca*trf_ca*Dc_caSDc_h2so4[i] + z_cahso4*trf_cahso4*Dc_cahso4SDc_h2so4[i] + z_h3sio4*trf_h3sio4*Dc_h3sio4SDc_h2so4[i] \
           + z_cah3sio4*trf_cah3sio4*Dc_cah3sio4SDc_h2so4[i] + z_caoh*trf_caoh*Dc_caohSDc_h2so4[i] + z_h2sio4*trf_h2sio4*Dc_h2sio4SDc_h2so4[i];
  }
  K(E_q,I_C_H2SO4)           += + c[0] ;
  K(E_q,I_C_H2SO4+NEQ)       += - c[1] ;
  K(E_q+NEQ,I_C_H2SO4)       += - c[0] ;
  K(E_q+NEQ,I_C_H2SO4+NEQ)   += + c[1] ;

  for(i=0;i<2;i++){
    c[i] = z_h*trf_h*Dc_hSDzn_si_s[i] + z_oh*trf_oh*Dc_ohSDzn_si_s[i] + z_hso4*trf_hso4*Dc_hso4SDzn_si_s[i] + z_so4*trf_so4*Dc_so4SDzn_si_s[i] \
           + z_ca*trf_ca*Dc_caSDzn_si_s[i] + z_cahso4*trf_cahso4*Dc_cahso4SDzn_si_s[i] + z_h3sio4*trf_h3sio4*Dc_h3sio4SDzn_si_s[i] \
           + z_cah3sio4*trf_cah3sio4*Dc_cah3sio4SDzn_si_s[i] + z_caoh*trf_caoh*Dc_caohSDzn_si_s[i] + z_h2sio4*trf_h2sio4*Dc_h2sio4SDzn_si_s[i];
  }
  K(E_q,I_ZN_Si_S)           += + c[0] ;
  K(E_q,I_ZN_Si_S+NEQ)       += - c[1] ;
  K(E_q+NEQ,I_ZN_Si_S)       += - c[0] ;
  K(E_q+NEQ,I_ZN_Si_S+NEQ)   += + c[1] ;

  for(i=0;i<2;i++){
    c[i] = z_h*trf_h*Dc_hSDzn_ca_s[i] + z_oh*trf_oh*Dc_ohSDzn_ca_s[i] + z_hso4*trf_hso4*Dc_hso4SDzn_ca_s[i] + z_so4*trf_so4*Dc_so4SDzn_ca_s[i] \
           + z_ca*trf_ca*Dc_caSDzn_ca_s[i] + z_cahso4*trf_cahso4*Dc_cahso4SDzn_ca_s[i] + z_h3sio4*trf_h3sio4*Dc_h3sio4SDzn_ca_s[i] \
           + z_cah3sio4*trf_cah3sio4*Dc_cah3sio4SDzn_ca_s[i] + z_caoh*trf_caoh*Dc_caohSDzn_ca_s[i] + z_h2sio4*trf_h2sio4*Dc_h2sio4SDzn_ca_s[i];
  }
  K(E_q,I_ZN_Ca_S)           += + c[0] ;
  K(E_q,I_ZN_Ca_S+NEQ)       += - c[1] ;
  K(E_q+NEQ,I_ZN_Ca_S)       += - c[0] ;
  K(E_q+NEQ,I_ZN_Ca_S+NEQ)   += + c[1] ;
  
  for(i=0;i<2;i++){
    c[i] = z_k*trf_k + z_h*trf_h*Dc_hSDc_k[i] + z_oh*trf_oh*Dc_ohSDc_k[i] + z_hso4*trf_hso4*Dc_hso4SDc_k[i] + z_so4*trf_so4*Dc_so4SDc_k[i] \
           + z_ca*trf_ca*Dc_caSDc_k[i] + z_cahso4*trf_cahso4*Dc_cahso4SDc_k[i] + z_h3sio4*trf_h3sio4*Dc_h3sio4SDc_k[i] \
           + z_cah3sio4*trf_cah3sio4*Dc_cah3sio4SDc_k[i] + z_caoh*trf_caoh*Dc_caohSDc_k[i] + z_h2sio4*trf_h2sio4*Dc_h2sio4SDc_k[i];
  }
  K(E_q,I_C_K)           += + c[0] ;
  K(E_q,I_C_K+NEQ)       += - c[1] ;
  K(E_q+NEQ,I_C_K)       += - c[0] ;
  K(E_q+NEQ,I_C_K+NEQ)   += + c[1] ; 

  for(i=0;i<2;i++){
    c[i] = tre_q ;
  }
  K(E_q,I_PSI)          += + c[0] ;
  K(E_q,I_PSI+NEQ)      += - c[1] ;
  K(E_q+NEQ,I_PSI)      += - c[0] ;
  K(E_q+NEQ,I_PSI+NEQ)  += + c[1] ;
  
    /*
  Conservation de K (potassium)  : (n_K1 - n_Kn) + dt * div(w_K) = 0
  */
  for(i=0;i<2;i++){
    c[i] = tre_k ;
  }
  K(E_K,I_PSI)          += + c[0] ;
  K(E_K,I_PSI+NEQ)      += - c[1] ;
  K(E_K+NEQ,I_PSI)      += - c[0] ;
  K(E_K+NEQ,I_PSI+NEQ)  += + c[1] ;
    
   
  for(i=0;i<2;i++){
    c[i] = trf_k ;
  }
  K(E_K,I_C_K)          += + c[0] ;
  K(E_K,I_C_K+NEQ)      += - c[1] ;
  K(E_K+NEQ,I_C_K)      += - c[0] ;
  K(E_K+NEQ,I_C_K+NEQ)  += + c[1] ; 
  
 }

#if (U_H2SO4 == LOG_RHO)
  for(i=0;i<2*NEQ;i++){
    K(i,I_C_H2SO4)     *= Ln10*C_H2SO4(0) ;
    K(i,I_C_H2SO4+NEQ) *= Ln10*C_H2SO4(1) ;
  }
#endif
  

  return(0) ;

#undef K
}


int  ComputeResidu(Element_t *el,double t,double dt,double *r)
/* Residu (r) */
{
#define R(n,i)    (r[(n)*NEQ+(i)])
  Symmetry_t sym = Element_GetSymmetry(el) ;
  double *f = Element_GetCurrentImplicitTerm(el) ;
  double *f_n = Element_GetPreviousImplicitTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  double dx ,xm ;
  double volume[2],surf ;
  int    i ;
  double zero = 0.,un = 1.,deux = 2. ;
  /*
    INITIALISATION DU RESIDU
  */
  for(i=0;i<NEQ*nn;i++) r[i] = zero ;

  if(Element_IsSubmanifold(el)) return(0) ;
  
  /*
    CALCUL DE volume ET DE surf
  */
  {
    double x1 = Element_GetNodeCoordinate(el,1)[0] ;
    double x0 = Element_GetNodeCoordinate(el,0)[0] ;
    dx = x1 - x0 ;
    xm = (x1 + x0)/deux ;
  }
  for(i=0;i<nn;i++) {
    double x = Element_GetNodeCoordinate(el,i)[0] ;
    volume[i] = fabs(dx)/deux ; 
    if(sym == AXIS) volume[i] *= M_PI*(x + xm) ; 
  }
  if(sym == AXIS) surf = deux*M_PI*xm ; else surf = un ;
  /*
    Conservation de C (carbone) : (n_C1 - n_Cn) + dt * div(w_C) = 0
  */
  R(0,E_S) -= volume[0]*(N_S(0) - N_Sn(0)) + dt*surf*W_S ;
  R(1,E_S) -= volume[1]*(N_S(1) - N_Sn(1)) - dt*surf*W_S ;
  /*
    Conservation de Ca (calcium) : (n_Ca1 - n_Can) + dt * div(w_Ca) = 0
  */
  R(0,E_Ca) -= volume[0]*(N_Ca(0) - N_Can(0)) + dt*surf*W_Ca ;
  R(1,E_Ca) -= volume[1]*(N_Ca(1) - N_Can(1)) - dt*surf*W_Ca ;
  /*
    Conservation de Si (silicium) : (n_Si1 - n_Sin) + dt * div(w_Si) = 0
  */
  R(0,E_Si) -= volume[0]*(N_Si(0) - N_Sin(0)) + dt*surf*W_Si ;
  R(1,E_Si) -= volume[1]*(N_Si(1) - N_Sin(1)) - dt*surf*W_Si ;
  /*
      Conservation de K (potassium) : (n_K1 - n_Kn) + dt * div(w_K) = 0
  */
  R(0,E_K) -= volume[0]*(N_K(0) - N_Kn(0)) + dt*surf*W_K ;
  R(1,E_K) -= volume[1]*(N_K(1) - N_Kn(1)) - dt*surf*W_K ; 
  /*
    Conservation de la charge  : div(w_q) = 0
  */
  R(0,E_q) -= + dt*surf*W_q ;
  R(1,E_q) -= - dt*surf*W_q ;
  
  return(0) ;

#undef R
}


int  ComputeOutputs(Element_t *el,double t,double *s,Result_t *r)
/* Les valeurs exploitees (s) */
{
  double *f = Element_GetCurrentImplicitTerm(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  double *u[Element_MaxNbOfNodes] ;
  int    i,nso ;
  double zero = 0. ;
#define UNKNOWN_s(i)   UNKNOWN(j,i)
  
  for(i = 0 ; i < nn ; i++) {
    u[i] = Element_GetNodalUnknown(el,i) ;
  }

  /* if(Element_IsSubmanifold(el)) return(0) ; */
  
  /*
    Donnees
  */
  c_h2so4_eq  = Element_GetProperty(el)[pm("C_H2SO4_eq")] ;

  /* initialisation */
  nso = 27 ;
  for(i = 0 ; i < nso ; i++) {
    int j ;
    for(j = 0 ; j < 9 ; j++) Result_GetValue(r+i)[j] = zero ;
  }

  /* output quantities */
  {
    double x1 = Element_GetNodeCoordinate(el,1)[0] ;
    double x0 = Element_GetNodeCoordinate(el,0)[0] ;
    double xm = (x0 + x1)*0.5 ;
    int j = (Element_IsSubmanifold(el)) ? 0 : ((s[0] < xm) ? 0 : 1) ;
    /* molarites */
    double *x = ComputeAqueousVariables(el,u,j) ;
    
    double c_h2so4    = x[I_C_H2SO4] ;
    double zn_si_s    = x[I_ZN_Si_S] ;
    double zn_ca_s    = x[I_ZN_Ca_S] ;
    double c_k        = x[I_C_K] ;
    double c_oh       = x[I_C_OH] ;
    double c_hso4     = x[I_C_HSO4] ;
    double c_so4      = x[I_C_SO4] ;
    double c_ca       = x[I_C_Ca] ;
    double c_h2sio4   = x[I_C_H2SiO4] ;
    double c_h3sio4   = x[I_C_H3SiO4] ;
    double c_h4sio4   = x[I_C_H4SiO4] ;
    double c_cah2sio4 = x[I_C_CaH2SiO4] ;
    double c_cah3sio4 = x[I_C_CaH3SiO4] ;
    double c_cahso4   = x[I_C_CaHSO4] ;
    double c_caso4aq  = x[I_C_CaSO4aq] ;
    double c_caoh     = x[I_C_CaOH] ;
    double zq_ch      = x[I_ZQ_CH] ;
    double zc_h2so4   = c_h2so4/c_h2so4_eq ;

    /* charge density */
    double c_q = x[I_N_Q] ;
    /* solid contents */
    double n_ch       = N_CH(j) ;
    double n_csh2     = N_CSH2(j) ;
    double n_si_s     = N_Si_S(j)  ;
    
    /* porosity */
    double v_csh      = V_CSH(zq_ch) ;
    double phi        = PHI(j) ;

    double psi        = UNKNOWN_s(I_PSI) ;
    double ph         = 14 + log(c_oh)/log(10.) ;
    double pk_ch      = log10(zq_ch) ;
    double x_csh      = X_CSH(zq_ch) ;

    i = 0 ;
    Result_Store(r + i++,&ph,"ph",1) ;
    Result_Store(r + i++,&c_h2so4,"c_h2so4",1) ;
    Result_Store(r + i++,&c_hso4,"c_hso4",1) ;
    Result_Store(r + i++,&c_so4,"c_so4",1) ;
    Result_Store(r + i++,&c_ca,"c_ca",1) ;
    Result_Store(r + i++,&c_caoh,"c_caoh",1) ;
    Result_Store(r + i++,&c_h2sio4,"c_h2sio4",1) ;
    Result_Store(r + i++,&c_h3sio4,"c_h3sio4",1) ;
    Result_Store(r + i++,&c_h4sio4,"c_h4sio4",1) ;
    Result_Store(r + i++,&c_cah2sio4,"c_cah2sio4",1) ;
    Result_Store(r + i++,&c_cah3sio4,"c_cah3sio4",1) ;
    Result_Store(r + i++,&c_caso4aq,"c_caso4aq",1) ;
    Result_Store(r + i++,&c_cahso4,"c_cahso4",1) ;
    Result_Store(r + i++,&c_k,"c_k",1) ;
    Result_Store(r + i++,&c_oh,"c_oh",1) ;
    Result_Store(r + i++,&n_ch,"n_ch",1) ;
    Result_Store(r + i++,&n_csh2,"n_csh2",1) ;
    Result_Store(r + i++,&n_si_s,"n_si_s",1) ;
    Result_Store(r + i++,&phi,"porosite",1) ;
    Result_Store(r + i++,&psi,"potentiel_electrique",1) ;
    Result_Store(r + i++,&c_q,"charge",1) ;
    Result_Store(r + i++,&zn_ca_s,"zn_ca_s",1) ;
    Result_Store(r + i++,&zc_h2so4,"zc_h2so4",1) ;
    Result_Store(r + i++,&pk_ch,"pk_ch",1) ;
    Result_Store(r + i++,&zn_si_s,"zn_si_s",1) ;
    Result_Store(r + i++,&v_csh,"V_CSH",1) ;
    Result_Store(r + i++,&x_csh,"C/S",1) ;
  }
  
  if(i != nso) arret("ComputeOutputs (M70b)") ;

  return(nso) ;
}


void transfert(Element_t *el,double **u,double *f)
/* Termes explicites (va)  */
{
  double *va = Element_GetExplicitTerm(el) ;
  int i ;
  /*
    Donnees
  */
  phi0      = Element_GetProperty(el)[pm("porosite")] ;
  c_h2so4_eq  = Element_GetProperty(el)[pm("C_H2SO4_eq")] ;

  /* initialisation */
  for(i=0;i<NVE_TR;i++) va[i] = 0. ;
  /*
    Coefficients de transfert
  */
  for(i=0;i<2;i++) {
    /* molarities */
    double *x = ComputeAqueousVariables(el,u,i) ;

    double c_oh       = x[I_C_OH] ;
    double c_h        = x[I_C_H] ;
    double c_hso4     = x[I_C_HSO4] ;
    double c_so4      = x[I_C_SO4] ;
    double c_ca       = x[I_C_Ca] ;
    double c_cahso4   = x[I_C_CaHSO4] ;
    double c_h3sio4   = x[I_C_H3SiO4] ;
    double c_cah3sio4 = x[I_C_CaH3SiO4] ;
    double c_h2sio4   = x[I_C_H2SiO4] ;
    double c_caoh     = x[I_C_CaOH] ;
    double c_k        = x[I_C_K] ;

    /* porosity */
    double phi        = PHI(i) ;

    /* tortuosite liquide */
    double iff    = (phi < 0.8) ? 2.9e-4*exp(9.95*phi) : phi ;
    
    KF_OH         += d_oh*iff ;
    KF_H          += d_h*iff ;
    KF_H2SO4      += d_h2so4*iff ;
    KF_HSO4       += d_hso4*iff ;
    KF_SO4        += d_so4*iff ;

    KF_Ca         += d_ca*iff ;
    KF_CaHSO4     += d_cahso4*iff ;
    KF_CaH3SiO4   += d_cah3sio4*iff ;

    KF_H3SiO4     += d_h3sio4*iff ;
    KF_H4SiO4     += d_h4sio4*iff ;
    KF_H2SiO4     += d_h2sio4*iff ;
    KF_CaH2SiO4   += d_cah2sio4*iff ;
    KF_CaSO4aq    += d_caso4aq*iff ;
    KF_CaOH       += d_caoh*iff ;
    
    KF_K          += d_k*iff;


    Kpsi_H        += FsRT*KF_H*z_h*c_h ;
    Kpsi_OH       += FsRT*KF_OH*z_oh*c_oh ;
    Kpsi_HSO4     += FsRT*KF_HSO4*z_hso4*c_hso4 ;
    Kpsi_SO4      += FsRT*KF_SO4*z_so4*c_so4 ;

    Kpsi_Ca       += FsRT*KF_Ca*z_ca*c_ca ;
    Kpsi_CaHSO4   += FsRT*KF_CaHSO4*z_cahso4*c_cahso4 ;
    Kpsi_CaH3SiO4 += FsRT*KF_CaH3SiO4*z_cah3sio4*c_cah3sio4 ;
    Kpsi_H3SiO4   += FsRT*KF_H3SiO4*z_h3sio4*c_h3sio4 ;
    Kpsi_H2SiO4   += FsRT*KF_H2SiO4*z_h2sio4*c_h2sio4 ;
    Kpsi_CaOH     += FsRT*KF_CaOH*z_caoh*c_caoh ;
    
    Kpsi_K        += FsRT*KF_K*z_k*c_k ;

    Kpsi_q        += z_h*Kpsi_H + z_oh*Kpsi_OH + z_hso4*Kpsi_HSO4 + z_so4*Kpsi_SO4 + z_ca*Kpsi_Ca + z_cahso4*Kpsi_CaHSO4 + z_h3sio4*Kpsi_H3SiO4 \
                   + z_cah3sio4*Kpsi_CaH3SiO4 + z_caoh*Kpsi_CaOH + z_h2sio4*Kpsi_H2SiO4 + z_k*Kpsi_K ;
  }
  
  /* moyenne */
  for(i=0;i<NVE_TR;i++) va[i] *= 0.5 ;
}


void flux(Element_t *el,double **u)
/* Les flux (f) */
{
  double *f = Element_GetImplicitTerm(el) ;
  double *va = Element_GetExplicitTerm(el) ;
  double r_h[2],r_oh[2] ;
  double r_h2so4[2],r_hso4[2],r_so4[2] ;
  double r_h3sio4[2],r_h4sio4[2],r_h2sio4[2] ;
  double r_ca[2],r_caoh[2] ;
  double r_cahso4[2],r_caso4aq[2],r_cah3sio4[2],r_cah2sio4[2] ;
  double r_k[2] ;

  int    i ;

  for(i=0;i<2;i++) {
    /* molarities */
    double *x = ComputeAqueousVariables(el,u,i) ;

    r_oh[i]       = x[I_C_OH] ;
    r_h[i]        = x[I_C_H] ;
    r_h2so4[i]    = x[I_C_H2SO4] ;
    r_hso4[i]     = x[I_C_HSO4] ;
    r_so4[i]      = x[I_C_SO4] ;
    r_ca[i]       = x[I_C_Ca] ;
    r_cahso4[i]   = x[I_C_CaHSO4] ;
    r_h3sio4[i]   = x[I_C_H3SiO4] ;
    r_h4sio4[i]   = x[I_C_H4SiO4] ;
    r_cah3sio4[i] = x[I_C_CaH3SiO4] ;
    r_h2sio4[i]   = x[I_C_H2SiO4] ;
    r_cah2sio4[i] = x[I_C_CaH2SiO4] ;
    r_caoh[i]     = x[I_C_CaOH] ;
    r_caso4aq[i]  = x[I_C_CaSO4aq] ;
    r_k[i]        = x[I_C_K] ;
  }

  /* Gradients */
  {
    double x1 = Element_GetNodeCoordinate(el,1)[0] ;
    double x0 = Element_GetNodeCoordinate(el,0)[0] ;
    double dx = x1 - x0 ;
    double grd_h        = (r_h[1]        - r_h[0]       )/dx ;
    double grd_oh       = (r_oh[1]       - r_oh[0]      )/dx ;
    double grd_h2so4    = (r_h2so4[1]    - r_h2so4[0]   )/dx ;
    double grd_hso4     = (r_hso4[1]     - r_hso4[0]    )/dx ;
    double grd_so4      = (r_so4[1]      - r_so4[0]     )/dx ;
    double grd_cahso4   = (r_cahso4[1]   - r_cahso4[0]  )/dx ;
    double grd_ca       = (r_ca[1]       - r_ca[0]      )/dx ;
    double grd_cah3sio4 = (r_cah3sio4[1] - r_cah3sio4[0])/dx ;
    double grd_h3sio4   = (r_h3sio4[1]   - r_h3sio4[0]  )/dx ;
    double grd_h4sio4   = (r_h4sio4[1]   - r_h4sio4[0]  )/dx ;
    double grd_h2sio4   = (r_h2sio4[1]   - r_h2sio4[0]  )/dx ;
    double grd_cah2sio4 = (r_cah2sio4[1] - r_cah2sio4[0])/dx ;
    double grd_caso4aq  = (r_caso4aq[1]  - r_caso4aq[0] )/dx ;
    double grd_caoh     = (r_caoh[1]     - r_caoh[0]    )/dx ;
    double grd_k        = (r_k[1]        - r_k[0]       )/dx ;
    
    double grd_psi      = (PSI(1)        - PSI(0)       )/dx ;
    
    /* Flux */
    double w_h2so4    = - KF_H2SO4*grd_h2so4   ;
    double w_hso4     = - KF_HSO4*grd_hso4          - Kpsi_HSO4*grd_psi ;
    double w_so4      = - KF_SO4*grd_so4            - Kpsi_SO4*grd_psi      ;
    double w_cahso4   = - KF_CaHSO4*grd_cahso4      - Kpsi_CaHSO4*grd_psi ;
    double w_ca       = - KF_Ca*grd_ca              - Kpsi_Ca*grd_psi ;
    double w_cah3sio4 = - KF_CaH3SiO4*grd_cah3sio4  - Kpsi_CaH3SiO4*grd_psi ;
    double w_h3sio4   = - KF_H3SiO4*grd_h3sio4      - Kpsi_H3SiO4*grd_psi ;
    double w_h2sio4   = - KF_H2SiO4*grd_h2sio4      - Kpsi_H2SiO4*grd_psi ;
    double w_caoh     = - KF_CaOH*grd_caoh          - Kpsi_CaOH*grd_psi ;
    double w_h4sio4   = - KF_H4SiO4*grd_h4sio4 ;
    double w_cah2sio4 = - KF_CaH2SiO4*grd_cah2sio4 ;
    double w_caso4aq  = - KF_CaSO4aq*grd_caso4aq ;    
    double w_k        = - KF_K*grd_k                - Kpsi_K*grd_psi ; 
    
    double w_q        = - z_h*KF_H*grd_h		      \
                        - z_oh*KF_OH*grd_oh		      \
                        - z_hso4*KF_HSO4*grd_hso4             \
                        - z_so4*KF_SO4*grd_so4		      \
                        - z_ca*KF_Ca*grd_ca		      \
                        - z_cahso4*KF_CaHSO4*grd_cahso4	      \
                        - z_h3sio4*KF_H3SiO4*grd_h3sio4	      \
                        - z_cah3sio4*KF_CaH3SiO4*grd_cah3sio4 \
                        - z_h2sio4*KF_H2SiO4*grd_h2sio4 \
                        - z_caoh*KF_CaOH*grd_caoh \
                        - z_k*KF_K*grd_k \
                        - Kpsi_q*grd_psi ;

    W_S     = w_h2so4 + w_hso4 + w_so4 + w_cahso4 + w_caso4aq ;
    W_Ca    = w_ca + w_cahso4 + w_cah3sio4 + w_caso4aq + w_caoh + w_cah2sio4 ;
    W_Si    = w_h3sio4 + w_h4sio4 + w_cah3sio4 + w_cah2sio4 + w_h2sio4 ;
    W_q     = w_q ;
    W_K     = w_k ;
  }
}


double concentration_oh(double c_h2so4,elem_t *el,double zn_ca_s,double c_k,double zn_si_s)
/* on resout l'electroneutralie : SUM(z_i c_i) = 0
   racine de ax^4 + bx^3 + cx^2 + dx + e = 0 */
{
  /* c_h2so4, zn_si_s et zn_ca_s sont fixes */
  double c_h2so4_eq   = C_H2SO4_eq ;
  double zc_h2so4     = c_h2so4/c_h2so4_eq ;
  /* les produits sont donc aussi fixes */
  double Q_CSH2     = IAP_CSH2(zc_h2so4,zn_ca_s) ;
  double Q_CH       = IAP_CH(zc_h2so4,zn_ca_s) ;
  double zq_ch      = Q_CH/K_CH ;
  double c_h4sio4   = IAP_SH(zq_ch,zn_si_s) ;

  /*
  rappel des expressions c_i = A_i*(c_h)**n   : n
     c_h        = K_h2o/c_oh                     : +1
     c_hso4     = K_hso4*c_h2so4/c_h             : -1
     c_so4      = K_so4*c_hso4/c_h               : -2
     c_ca       = Q_CSH2/c_so4                     : +2
     c_cahso4   = K_cahso4*c_ca*c_hso4           : +1
     c_h4sio4   = IAP_SH                         :  0
     c_h3sio4   = c_h4sio4/(K_h4sio4*c_h)        : -1
     c_cah3sio4 = K_cah3sio4*c_ca*c_h3sio4       : +1
     c_caso4aq  = K_caso4aq*c_ca*c_so4 ;         :  0      
     c_h2sio4   = K_h2sio4*c_h3sio4*c_oh ;       : -2       
     c_cah2sio4 = K_cah2sio4*c_h2sio4*c_ca ;     :  0      
     c_caoh     = K_caoh*c_ca*c_oh ;             : +1       
  */
  double A_hso4     = K_hso4*c_h2so4 ;
  double A_so4      = K_so4*A_hso4 ;
  double A_ca       = Q_CSH2/A_so4 ;
  double A_cahso4   = K_cahso4*A_ca*A_hso4 ;
  double A_h3sio4   = c_h4sio4/K_h4sio4 ;
  double A_cah3sio4 = K_cah3sio4*A_ca*A_h3sio4 ;
  double A_h2sio4   = K_h2sio4*A_h3sio4*K_h2o ;
  double A_caoh     = K_caoh*A_ca*K_h2o ;

  double a = z_ca*A_ca ;
  double b = z_h + z_cahso4*A_cahso4 + z_cah3sio4*A_cah3sio4 + z_caoh*A_caoh ;
  double c = z_k*c_k ;
  double d = z_oh*K_h2o + z_hso4*A_hso4 + z_h3sio4*A_h3sio4 ;
  double e = z_so4*A_so4 + z_h2sio4*A_h2sio4 ;

  /* a > 0 ; b > 0 ; c > 0 ; d < 0 ; e < 0 */
  double c_h = poly4(a,b,c,d,e) ;
 
  return(K_h2o/c_h) ;
}

double poly4(double a,double b,double c,double d,double e)
/* on resout ax^4 + bx^3 + cx^2 + dx + e = 0 */
{
  double err,tol = 1e-8 ;
  double x0 = pow(-d/a,1./3) ;/*ana_abcd(a,b,c,d);*/
  double x  = x0 ;
  int    i  = 0 ;
  /* 
     on neglige a et c
     solution de bx^3 + dx + e = 0 
     que l'on met sous la forme x^3 + px + q = 0
  double p = d/b,q = e/b ;
  double r = sqrt(q*q/4 + p*p*p/27) ;
  double uu = pow(-q/2. + r,1/3.), vv = pow(-q/2. - r,1/3.) ;
  double x = uu + vv ; *//* formule de Cardan */
  
  /* Newton */
  do {
    double x2 = x*x,x3 = x2*x,x4 = x2*x2 ;
    double f  = a*x4 + b*x3 + c*x2 + d*x + e ;
    double df = 4*a*x3 + 3*b*x2 + 2*c*x + d ;
    double dx = -f/df ;
    err = fabs(dx/x) ;
    x += dx ;
    if(i++ > 40) {
      printf("x0 = %e\n",x0) ;
      printf("x  = %e\n",x) ;
      arret("poly4 : non convergence") ;
    }
  } while(err > tol) ;
  return(x) ;
}



double* ComputeAqueousVariables(Element_t *el,double **u,int n)
{
  double *x = var ;
  
  x[I_C_H2SO4] = C_H2SO4(n) ;
  x[I_ZN_Ca_S ] = ZN_Ca_S(n) ;
  x[I_ZN_Si_S ] = ZN_Si_S(n) ;
  x[I_C_K    ] = C_K(n) ;
  
  ComputeAqueousParameters(el,x) ;
  return(x) ;
}


double* ComputeAqueousDerivatives(Element_t *el,double *x,double dxi,int i)
{
  double *dx = dvar ;
  int j ;
  
  dx[I_C_H2SO4] = x[I_C_H2SO4] ;
  dx[I_ZN_Ca_S ] = x[I_ZN_Ca_S ] ;
  dx[I_ZN_Si_S ] = x[I_ZN_Si_S ] ;
  dx[I_C_K    ] = x[I_C_K    ] ;
  
  dx[i] += dxi ;
  
  ComputeAqueousParameters(el,dx) ;
  
  for(j = 0 ; j < NbOfAqueousVariables ; j++) {
    dx[j] -= x[j] ;
    dx[j] /= dxi ;
  }

  return(dx) ;
} 



void  ComputeAqueousParameters(Element_t *el,double *x)
{
  double c_h2so4    = x[I_C_H2SO4] ;
  double zn_si_s    = x[I_ZN_Si_S] ;
  double zn_ca_s    = x[I_ZN_Ca_S] ;
  double c_k        = x[I_C_K] ;
  
  double zc_h2so4   = c_h2so4/c_h2so4_eq ;

  double Q_CSH2     = IAP_CSH2(zc_h2so4,zn_ca_s) ;
  double Q_CH       = IAP_CH(zc_h2so4,zn_ca_s) ;
  double zq_ch      = Q_CH/K_CH ;
  double c_h4sio4   = IAP_SH(zq_ch,zn_si_s) ;

  double c_oh       = concentration_oh(c_h2so4,el,zn_ca_s,c_k,zn_si_s) ;
  double c_h        = K_h2o/c_oh ;
  double c_hso4     = K_hso4*c_h2so4/c_h ;
  double c_so4      = K_so4*c_hso4/c_h ;
  double c_ca       = Q_CSH2/c_so4 ;
  double c_cahso4   = K_cahso4*c_ca*c_hso4 ;
  double c_h3sio4   = c_h4sio4/(K_h4sio4*c_h) ;
  double c_cah3sio4 = K_cah3sio4*c_ca*c_h3sio4 ;
  double c_caso4aq  = K_caso4aq*c_ca*c_so4 ;
  double c_h2sio4   = K_h2sio4*c_h3sio4*c_oh ;
  double c_cah2sio4 = K_cah2sio4*c_h2sio4*c_ca ;
  double c_caoh     = K_caoh*c_ca*c_oh ;
  
  x[I_C_OH      ] = c_oh ;
  x[I_C_H       ] = c_h ;
  
  x[I_C_HSO4    ] = c_hso4 ;
  x[I_C_SO4     ] = c_so4 ;
  
  x[I_C_Ca      ] = c_ca ;
  
  x[I_C_H4SiO4  ] = c_h4sio4 ;
  x[I_C_H3SiO4  ] = c_h3sio4 ;
  x[I_C_H2SiO4  ] = c_h2sio4 ;
  
  x[I_C_CaH3SiO4] = c_cah3sio4 ;
  x[I_C_CaH2SiO4] = c_cah2sio4 ;
  
  x[I_C_CaHSO4  ] = c_cahso4 ;
  x[I_C_CaSO4aq ] = c_caso4aq ;
  
  x[I_C_CaOH    ] = c_caoh ;
  
  x[I_ZQ_CH   ] = zq_ch ;

  x[I_C_S_L     ] = c_h2so4 + c_hso4 + c_so4 + c_cahso4 + c_caso4aq ;
  x[I_C_Ca_L    ] = c_ca + c_cahso4 + c_cah3sio4 + c_cah2sio4 + c_caso4aq + c_caoh ;
  x[I_C_Si_L    ] = c_h3sio4 + c_h4sio4 + c_cah3sio4 + c_h2sio4 + c_cah2sio4 ;
  x[I_C_K_L     ] = c_k ;

  /* charge density */
  x[I_N_Q     ]  = z_h*c_h + z_oh*c_oh \
                 + z_hso4*c_hso4 + z_so4*c_so4 \
                 + z_ca*c_ca + z_caoh*c_caoh \
                 + z_h3sio4*c_h3sio4 + z_h2sio4*c_h2sio4 \
                 + z_cah3sio4*c_cah3sio4 + z_cahso4*c_cahso4 \
                 + z_k*c_k ;
}


double* ComputeAqueousDerivatives1(Element_t *el,double *x,double dxi,int i)
{
  double c_h2so4    = x[I_C_H2SO4] ;
  double zn_si_s    = x[I_ZN_Si_S] ;
  double zn_ca_s    = x[I_ZN_Ca_S] ;
  double c_k        = x[I_C_K] ;
  double c_oh       = x[I_C_OH] ;
  double c_h        = x[I_C_H] ;
  double c_hso4     = x[I_C_HSO4] ;
  double c_so4      = x[I_C_SO4] ;
  double c_ca       = x[I_C_Ca] ;
  double c_h2sio4   = x[I_C_H2SiO4] ;
  double c_h3sio4   = x[I_C_H3SiO4] ;
  double c_h4sio4   = x[I_C_H4SiO4] ;
  double zq_ch      = x[I_ZQ_CH] ;
  double zc_h2so4   = c_h2so4/c_h2so4_eq ;
  double Q_CSH2     = IAP_CSH2(zc_h2so4,zn_ca_s) ;
  double Q_CH       = IAP_CH(zc_h2so4,zn_ca_s) ;

  double dc_oh       = 0 ;
  double dc_h        = 0 ;
  double dc_h2so4    = 0 ;
  double dc_hso4     = 0 ;
  double dc_so4      = 0 ;
  double dc_ca       = 0 ;
  double dc_h4sio4   = 0 ;
  double dc_h3sio4   = 0 ;
  double dc_h2sio4   = 0 ;
  double dc_cah3sio4 = 0 ;
  double dc_cah2sio4 = 0 ;
  double dc_caso4aq  = 0 ;
  double dc_cahso4   = 0 ;
  double dc_caoh     = 0 ;
  double dc_k        = 0 ;
  double dzq_ch      = 0 ;
  
  double *dx = dvar ;
  int j ;
  
  for(j = 0 ; j < NbOfAqueousVariables ; j++) dx[j] = 0 ;
  
  if(i == I_C_H2SO4) {
    double dc          = dxi ;
    double cc_h2so4    = c_h2so4 + dc ;
    double cc_oh       = concentration_oh(cc_h2so4,el,zn_ca_s,c_k,zn_si_s) ;
			                     
    double dQ_CSH2     = (zc_h2so4 < 1.) ? Q_CSH2/c_h2so4 : 0. ;
    double dQ_CH       = (zc_h2so4 < 1.) ? 0. : -Q_CH/c_h2so4 ;
    
    dc_h2so4    = 1 ;
    dc_oh       = (cc_oh - c_oh)/dc ;
    dc_h        = - c_h*dc_oh/c_oh ;
    dc_hso4     = c_hso4*(dc_h2so4/c_h2so4 + dc_oh/c_oh) ;
    dc_so4      = c_so4*(dc_hso4/c_hso4 + dc_oh/c_oh) ;
    
    dc_ca       = (dQ_CSH2 - c_ca*dc_so4)/c_so4 ;
    dc_cahso4   = K_cahso4*(dc_ca*c_hso4 + c_ca*dc_hso4) ;

    dzq_ch      = dQ_CH/K_CH ;
    dc_h4sio4   = DIAP_SHSDQ_CH(zq_ch,zn_si_s)*dzq_ch  ;
    dc_h3sio4   = c_h3sio4*(dc_h4sio4/c_h4sio4 - dc_h/c_h) ;
    dc_cah3sio4 = K_cah3sio4*(dc_ca*c_h3sio4 + c_ca*dc_h3sio4) ;
    dc_h2sio4   = K_h2sio4*(dc_h3sio4*c_oh	+ dc_oh*c_h3sio4) ;
    dc_cah2sio4 = K_cah2sio4*(dc_h2sio4*c_ca	+ dc_ca*c_h2sio4) ;
    dc_caso4aq  = K_caso4aq*(dc_so4*c_ca + dc_ca*c_so4) ;
    dc_caoh     = K_caoh*(dc_ca*c_oh + dc_oh*c_ca) ;
    dc_k        = 0 ;
    
  } else if(i == I_ZN_Ca_S) {
    double dz          = dxi ;
    double zzn_ca_s    = zn_ca_s + dz ;
    double cc_oh       = concentration_oh(c_h2so4,el,zzn_ca_s,c_k,zn_si_s) ;

    double dQ_CSH2     = (zn_ca_s < 0) ? Q_CSH2 : 0 ;
    double dQ_CH       = (zn_ca_s < 0) ? Q_CH : 0 ;
    
    dc_oh       = (cc_oh - c_oh)/dz ;
    dc_h        = - c_h*dc_oh/c_oh ;
    dc_hso4     = c_hso4*(dc_oh/c_oh) ;
    dc_so4      = c_so4*(dc_hso4/c_hso4 + dc_oh/c_oh) ;
    
    dc_ca       = (dQ_CSH2 - c_ca*dc_so4)/c_so4 ;
    dc_cahso4   = K_cahso4*(dc_ca*c_hso4 + c_ca*dc_hso4) ;
    
    dzq_ch      = dQ_CH/K_CH ;
    dc_h4sio4   = DIAP_SHSDQ_CH(zq_ch,zn_si_s)*dzq_ch  ;
    dc_h3sio4   = c_h3sio4*(dc_h4sio4/c_h4sio4 - dc_h/c_h) ;
    dc_cah3sio4 = K_cah3sio4*(dc_ca*c_h3sio4 + c_ca*dc_h3sio4) ;
    dc_h2sio4   = K_h2sio4*(dc_h3sio4*c_oh + dc_oh*c_h3sio4) ;
    dc_cah2sio4 = K_cah2sio4*(dc_h2sio4*c_ca	+ dc_ca*c_h2sio4) ;
    dc_caso4aq  = K_caso4aq*(dc_so4*c_ca + dc_ca*c_so4) ;
    dc_caoh     = K_caoh*(dc_ca*c_oh + dc_oh*c_ca) ;	
    dc_k        = 0 ;
    dc_h2so4    = 0 ;
    
  } else if(i == I_ZN_Si_S) {
    double dz          = dxi ;
    double zzn_si_s    = zn_si_s + dz ;
    double cc_oh       = concentration_oh(c_h2so4,el,zn_ca_s,c_k,zzn_si_s) ;
    
    dc_oh       = (cc_oh - c_oh)/dz ;
    dc_h        = - c_h*dc_oh/c_oh ;
    dc_hso4     = c_hso4*(dc_oh/c_oh) ;
    dc_so4      = c_so4*(dc_hso4/c_hso4 + dc_oh/c_oh) ;
    dc_ca       = - c_ca*dc_so4/c_so4 ;
    dc_cahso4   = K_cahso4*(dc_ca*c_hso4 + c_ca*dc_hso4) ;
    dc_h4sio4   = DIAP_SHSDZN_Si_S(zq_ch,zn_si_s) ;
    dc_h3sio4   = c_h3sio4*(dc_h4sio4/c_h4sio4 - dc_h/c_h) ;
    dc_cah3sio4 = K_cah3sio4*(dc_ca*c_h3sio4 + c_ca*dc_h3sio4) ;
    dc_h2sio4   = K_h2sio4*(dc_h3sio4*c_oh	+ dc_oh*c_h3sio4) ;
    dc_cah2sio4 = K_cah2sio4*(dc_h2sio4*c_ca + dc_ca*c_h2sio4) ;
    dc_caso4aq  = K_caso4aq*(dc_so4*c_ca + dc_ca*c_so4) ;
    dc_caoh     = K_caoh*(dc_ca*c_oh + dc_oh*c_ca) ;
    dc_k        = 0 ;
    dc_h2so4    = 0 ;
    dzq_ch      = 0 ;
    
  } else if(i == I_C_K) {
    double dc          = dxi ;
    double cc_k        = c_k + dc ;
    double cc_oh       = concentration_oh(c_h2so4,el,zn_ca_s,cc_k,zn_si_s) ;
    
    dc_oh       = (cc_oh - c_oh)/dc ;
    dc_h        = - c_h*dc_oh/c_oh ;
    dc_hso4     = c_hso4*(dc_oh/c_oh) ;
    dc_so4      = c_so4*(dc_hso4/c_hso4 + dc_oh/c_oh) ; 
	                           
    dc_ca       = - c_ca*dc_so4/c_so4 ;
    dc_cahso4   = K_cahso4*(dc_ca*c_hso4 + c_ca*dc_hso4) ;

    dc_h3sio4   = - c_h3sio4*dc_h/c_h ;
    dc_cah3sio4 = K_cah3sio4*(dc_ca*c_h3sio4 + c_ca*dc_h3sio4) ;
    dc_h2sio4   = K_h2sio4*(dc_h3sio4*c_oh + dc_oh*c_h3sio4) ;
    dc_cah2sio4 = K_cah2sio4*(dc_h2sio4*c_ca + dc_ca*c_h2sio4) ;
    dc_caso4aq  = K_caso4aq*(dc_so4*c_ca + dc_ca*c_so4) ;
    dc_caoh     = K_caoh*(dc_ca*c_oh + dc_oh*c_ca) ;
    dc_h4sio4   = 0 ;
    dc_k        = 1 ;
    dc_h2so4    = 0 ;
    dzq_ch      = 0 ;
  } else {
    arret("ComputeAqueousDerivatives1") ;
  }
  
    
  dx[I_C_OH      ] = dc_oh ;
  dx[I_C_H       ] = dc_h ;
  dx[I_C_H2SO4   ] = dc_h2so4 ;
  dx[I_C_HSO4    ] = dc_hso4 ;
  dx[I_C_SO4     ] = dc_so4 ;
  dx[I_C_Ca      ] = dc_ca ;
  dx[I_C_H4SiO4  ] = dc_h4sio4 ;
  dx[I_C_H3SiO4  ] = dc_h3sio4 ;
  dx[I_C_H2SiO4  ] = dc_h2sio4 ;
  dx[I_C_CaH3SiO4] = dc_cah3sio4 ;
  dx[I_C_CaH2SiO4] = dc_cah2sio4 ;
  dx[I_C_CaHSO4  ] = dc_cahso4 ;
  dx[I_C_CaSO4aq ] = dc_caso4aq ;
  dx[I_C_CaOH    ] = dc_caoh ;
  
  dx[I_ZQ_CH   ] = dzq_ch ;

  dx[I_C_S_L     ] = dc_h2so4 + dc_hso4 + dc_so4 + dc_cahso4 + dc_caso4aq ;
  dx[I_C_Ca_L    ] = dc_ca + dc_cahso4 + dc_cah3sio4 + dc_cah2sio4 + dc_caso4aq + dc_caoh ;
  dx[I_C_Si_L    ] = dc_h3sio4 + dc_h4sio4 + dc_cah3sio4 + dc_h2sio4 + dc_cah2sio4 ;
  dx[I_C_K_L     ] = dc_k ;

  dx[I_N_Q     ] = z_h*dc_h + z_oh*dc_oh \
                 + z_hso4*dc_hso4 + z_so4*dc_so4 \
                 + z_ca*dc_ca + z_caoh*dc_caoh \
                 + z_h3sio4*dc_h3sio4 + z_h2sio4*dc_h2sio4 \
                 + z_cah3sio4*dc_cah3sio4 + z_cahso4*dc_cahso4 \
                 + z_k*dc_k ;

  return(dx) ;
} 
