/*
	(ancien modele 70) 
	7 ions    : Cl-, Na+, K+, Ca2+, Al(OH)4-, H+, OH-
	4 solides : C3A, CH, Sel de Friedel, Al(OH)3
	- C3A = (CaO)3.(Al2O3) aluminate tricalcique
	- CH  = Ca(OH)2 hydroxyde de calcium (portlandite)
	- (CaO)3.(Al2O3).(CaCl2);(H2O)10 monochloroaluminate (sel de Friedel)
	- Al(OH)3 hydroxyde d'aluminium
	Isotherme d'interaction Chlorure - CSH de type Langmuir
	Dissolution-cristallisation des 4 solides
	Diffusion effective calculee a partir de celle des ions chlores
	Electroneutralite
	Modification de la porosite
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "CommonModel.h"

#define MODELINDEX  42
#define TITLE "Chlorures dans les betons satures"
#define AUTHORS "Nguyen"

#include "OldMethods.h"

/* Macros */
#define NEQ 	 (8)

#define NVI      (26)
#define NVE      (18)

#define E_Cl	 (0)
#define E_q  	 (1)
#define E_Al	 (2)
#define E_Ca	 (3)
#define E_Na	 (4)
#define E_K 	 (5)
#define E_A  	 (6)
#define E_el  	 (7)

#define I_c_cl   (0)
#define I_zp     (7)
#define I_zc  	 (3)
#define I_zf 	 (2)
#define I_c_na   (4)
#define I_c_k  	 (5)
#define I_za	 (6)
#define I_psi	 (1)

/* valences */
#define z_cl   	 (-1.)
#define z_oh   	 (-1.)
#define z_na  	 (1.)
#define z_k    	 (1.)
#define z_h    	 (1.)
#define z_ca   	 (2.)
#define z_aloh4  (-1.)

/* coefficients de diffusion moleculaire (m2/s) */
#define do_cl    (2.032e-9)
#define do_oh    (5.273e-9)
#define do_na    (1.334e-9)
#define do_k     (1.957e-9)
#define do_h     (9.310e-9)
#define do_ca    (0.792e-9)
#define do_aloh4 (0.542e-9)

/* constantes d'equilibre */
#define k_e      (1.e-8)  /* (1.35e-8) */
#define k_p      (189.06) /* (6.309573e3) */    /* logK = -5.2  */
#define k_c      (32.73)  /* (1.e7) */          /* logK = -20   */
#define k_f      (303.85) /* (7.94328234e6) */  /* logK = -29.1 */
#define k_a      (9.77e-2)

#ifdef NOTDEFINED
#define K_p      (3.574368e4)  /* (6.309573e3) */   /* logK = -5.2  */
#define K_c      (1.147583e6)  /* (1.e7) */          /* logK = -20   */
#define K_f      (8.523873e9)  /* (7.94328234e6) */  /* logK = -29.1 */
#define K_a      (9.77e-2)
#endif

/* volumes molaires (m3/mole) */
#define v_caoh2  (33.21e-6)
#define v_c3ah6  (150.1e-6)
#define v_sel    (296.69e-6)
#define v_aloh3  (32.23e-6)

/* constantes physiques */
#define F     	 (9.64846e4) /* cste de Faraday (C/mole) */
#define RT       (2436.)     /* produit de R et T */
                             /* R = 8.3143 J/K/mol cste des gaz parfait */
                             /* T = 293 K */

/* valeur arbitraire */
#define s_oho    (0.)        /* (1000.) */

/* Fonctions */
static int    pm(const char *s) ;
static double tortuosite(double) ;

/* Parametres */
static double phi0,d_cl,s_chrf,s_c3arf,s_sfrf,s_aloh3rf,s_csh,r_d,alpha,beta ;

int pm(const char *s)
{
  if(strcmp(s,"porosite") == 0) return (0) ;
  else if(strcmp(s,"D_Cl") == 0) return (1) ;
  else if(strcmp(s,"r_d") == 0) return (2) ;
  else if(strcmp(s,"s_caoh2") == 0) return (3) ;
  else if(strcmp(s,"s_c3ah6") == 0) return (4) ;
  else if(strcmp(s,"s_sel") == 0) return (5) ;
  else if(strcmp(s,"s_aloh3") == 0) return (6) ;
  else if(strcmp(s,"s_csh") == 0) return (7) ;
  else if(strcmp(s,"alpha") == 0) return (8);
  else if(strcmp(s,"beta") == 0) return (9);
  else { 
    printf("donnee \"%s\" non connue (pm42)\n",s) ; exit(0) ;
  }
}


int dm42(int dim,mate_t *mat,FILE *ficd)
/* Lecture des donnees materiaux dans le fichier ficd */
{
  int n_donnees = 10 ;

  if(dim > 1) arret("dm42 : dimension > 1 non prevue") ;

  mat->neq = NEQ ;

  strcpy(mat->eqn[E_Cl],"E_Cl") ;
  strcpy(mat->eqn[E_q],"E_q") ;
  strcpy(mat->eqn[E_Al],"E_Al") ;
  strcpy(mat->eqn[E_Ca],"E_Ca") ;
  strcpy(mat->eqn[E_Na],"E_Na") ;
  strcpy(mat->eqn[E_K] ,"E_K") ;
  strcpy(mat->eqn[E_A] ,"E_A") ;
  strcpy(mat->eqn[E_el],"E_el") ;

  strcpy(mat->inc[I_c_cl],"c_cl") ;
  strcpy(mat->inc[I_zp]  ,"z_caoh2") ;
  strcpy(mat->inc[I_zc]  ,"z_c3a") ;
  strcpy(mat->inc[I_zf]  ,"z_sf") ;
  strcpy(mat->inc[I_c_na],"c_na") ;
  strcpy(mat->inc[I_c_k] ,"c_k") ;
  strcpy(mat->inc[I_za]  ,"z_aloh3") ;
  strcpy(mat->inc[I_psi] ,"psi") ;

  dmat(mat,ficd,pm,n_donnees) ;

  return(mat->n) ;
}

int qm42(int dim,FILE *ficd)
/* Saisie des donnees materiaux */
{ 
  printf(TITLE) ;
  
  if(!ficd) return(NEQ) ;
  
  printf("\n\n\
Le systeme est forme de 8 equations :\n\
\t 1. Conservation de la masse de Cl        (c_cl)\n\
\t 2. Conservation de la charge             (psi)\n\
\t 3. Conservation de la masse de Al        (z_c3a)\n\
\t 4. Conservation de la masse de Ca        (z_sf)\n\
\t 5. Conservation de la masse de Na        (c_na)\n\
\t 6. Conservation de la masse de K         (c_k)\n\
\t 7. Dissolution-precipitation de Al(OH)3  (z_aloh3)\n\
\t 8. Electroneutralite                     (z_caoh2)\n") ;

  
  printf("\n\
Exemple de donnees\n\n") ;

  fprintf(ficd,"porosite = 0.121 # Porosite\n") ;
  fprintf(ficd,"D_Cl = 2.6e-12   # Diffusion effective de Cl\n") ;
  fprintf(ficd,"r_d = 1.         # Rapport des tortuosites des anions et des cations\n") ;
  fprintf(ficd,"s_caoh2 = 1640.  # Contenu en CaOH2 de reference (mol/m3)\n") ;
  fprintf(ficd,"s_c3ah6 = 8.43   # Contenu en C3AH6 de reference (mol/m3)\n") ;
  fprintf(ficd,"s_sel = 50.      # Contenu en sel de Friedel de reference (mol/m3)\n") ;
  fprintf(ficd,"s_aloh3 = 10.    # Contenu en AlOH3 de reference (mol/m3)\n") ;
  fprintf(ficd,"s_csh = 635.     # Contenu en CSH initial (mol/m3)\n") ;
  fprintf(ficd,"alpha = 0.12     # Coefficient de l'isotherme \n") ;
  fprintf(ficd,"beta = 2.66      # Coefficient de l'isotherme \n") ;
  
  return(NEQ) ;
}

void tb42(elem_t *el,inte_t *fi,unsigned int *n_fi,int dim)
{
  el->n_vi = NVI ; /* implicite */
  el->n_ve = NVE ; /* explicite */
}


void ch42(double **x,double **u_1,double **u_n,double *f_1,double *f_n,double *va,double *r,elem_t el,int dim,geom_t geom,double dt,double t,char_t cg)
/* Residu du aux chargements (r) */
{
}


void in42(double **x,double **u,double *f,double *va,elem_t el,int dim,geom_t geom)
/* Initialise les variables du systeme (f,va) */ 
{
#define C_Cl(n)     (u[(n)][I_c_cl])
#define ZP(n)       (u[(n)][I_zp])
#define ZC(n)       (u[(n)][I_zc])
#define ZF(n)	    (u[(n)][I_zf])
#define C_Na(n)     (u[(n)][I_c_na])
#define C_K(n)      (u[(n)][I_c_k])
#define ZA(n)       (u[(n)][I_za])
#define PSI(n)      (u[(n)][I_psi])

#define N_Cl(n)     (f[(n)])
#define N_Na(n)     (f[(n+4)])
#define N_K(n)      (f[(n+6)])
#define N_Ca(n)     (f[(n+8)])
#define N_Al(n)     (f[(n+10)])
#define N_q(n)      (f[(n+12)])

#define W_Cl        (f[14])
#define W_q         (f[15])
#define W_Na        (f[16])
#define W_K         (f[17])
#define W_Ca        (f[18])
#define W_Al        (f[19])

#define C_OH(n)     (f[(n+20)])
#define C_Ca(n)     (f[(n+22)])
#define C_AlOH4(n)  (f[(n+24)])

#define KF_Cl       (va[(0)])
#define KF_OH       (va[(1)])
#define KF_Na       (va[(2)])
#define KF_K        (va[(3)])
#define KF_H        (va[(4)])
#define KF_Ca       (va[(5)])
#define KF_AlOH4    (va[(6)])

#define KM_Cl       (va[(7)])
#define KM_OH       (va[(8)])
#define KM_Na       (va[(9)])
#define KM_K        (va[(10)])
#define KM_H        (va[(11)])
#define KM_Ca       (va[(12)])
#define KM_AlOH4    (va[(13)])

#define s_ch0       (va[(14)])
#define s_c3a0      (va[(15)])
#define s_sf0       (va[(16)])
#define s_aloh30    (va[(17)])

  double c_cl,c_oh,c_na,c_k,c_h,c_ca,c_aloh4 ;
  double zp,kk_p ;
  double zc,kk_c ;
  double zf,kk_f ;
  double za,kk_a ;
  double grd_cl,grd_oh,grd_na,grd_k,grd_h,grd_ca,grd_aloh4,grd_psi ;
  double s_cl,s_ch,s_c3a,s_sf,s_aloh3,s_oh ;
  double w_oh,w_h ;
  double dx,phi;
  double tau_p,tau_n;
  int    i ;
  double deux = 2. ;

  if(el.dim < dim) return ;

  /*
    Donnees
  */

  phi0      = el.mat->pr[pm("porosite")] ;
  d_cl      = el.mat->pr[pm("D_Cl")] ;
  r_d       = el.mat->pr[pm("r_d")] ;

  s_chrf    = el.mat->pr[pm("s_caoh2")] ;
  s_c3arf   = el.mat->pr[pm("s_c3ah6")] ;
  s_sfrf    = el.mat->pr[pm("s_sel")] ;
  s_aloh3rf = el.mat->pr[pm("s_aloh3")] ;

  s_csh     = el.mat->pr[pm("s_csh")] ;
  alpha     = el.mat->pr[pm("alpha")] ;
  beta      = el.mat->pr[pm("beta")] ;

  /* Contenus molaires */
  for(i=0;i<2;i++) {
    c_cl     = C_Cl(i) ;
    c_na     = C_Na(i) ;
    c_k      = C_K(i) ;

    /* variables : z = s/s_rf + P/K - 1 */
    zc       = ZC(i) ;
    zp	     = ZP(i) ;
    zf	     = ZF(i) ;
    za       = ZA(i) ;

    if(zf < -1. && zp > 0. && zc > 0.) {
      /* 
	 calcul de zf par la resolution de l'electroneutralite
	 d*x^2 + a*x + b + c/x = 0 avec x = (1 + z_f)^2
      */
      kk_c = k_c*(1. + 0.5*(zc - fabs(zc))) ;
      kk_p = k_p*(1. + 0.5*(zp - fabs(zp))) ;
      kk_f = k_f ;
      c_ca     = pow(kk_f/kk_c,4.)/(c_cl*c_cl) ;
      c_oh     = kk_p*pow(kk_c/kk_f,2.)*c_cl ;
      c_aloh4  = pow(kk_c*kk_c/(kk_p*kk_f),2.)*c_cl ;
      c_h	     = k_e/c_oh ;
      {
	double d = z_ca*c_ca ;
	double a = z_h*c_h ;
	double b = z_na*c_na + z_k*c_k + z_cl*c_cl ;
	double c = z_oh*c_oh + z_aloh4*c_aloh4 ;
	double p = b/d - a*a/(3*d*d) ;
	double q = c/d - a*b/(3*d*d) + 2*a*a*a/(27*d*d*d) ;
	double r = sqrt(q*q/4. + p*p*p/9.) ;
	double uu = pow(-q/2. + r,1./3.), vv = pow(q/2. + r,1./3.) ;
	double x = uu - vv ; /* formule de Cardan */
	zf = sqrt(x) - 1. ;
	ZF(i) = zf ;
	if(zf < -1.) {
	  printf("zf = %e\n",zf) ;
	  arret("in42") ;
	}
      }
    }

    if(za < -1.) {
      /* calcul de za par l'equilibre de Al(OH)3 */
      kk_c = k_c*(1. + 0.5*(zc - fabs(zc))) ;
      kk_p = k_p*(1. + 0.5*(zp - fabs(zp))) ;
      kk_a = kk_c*kk_c/(kk_p*kk_p*kk_p) ;
      za   = (kk_a < k_a) ? kk_a/k_a - 1. : 0. ;
      ZA(i) = za ;
	if(za < -1.) {
	  printf("za = %e\n",za) ;
	  arret("in42") ;
	}
    }

    /* teneurs en solides */
    s_ch     = 0.5*(zp + fabs(zp))*s_chrf ;	 		
    s_c3a    = 0.5*(zc + fabs(zc))*s_c3arf ;		
    s_sf     = 0.5*(zf + fabs(zf))*s_sfrf ;
    s_aloh3  = 0.5*(za + fabs(za))*s_aloh3rf ; 

    /* valeurs initiales */
    s_ch0    = s_ch ;
    s_c3a0   = s_c3a ;
    s_sf0    = s_sf ;
    s_aloh30 = s_aloh3 ;

    /* les produits kk_i */
    /*
       kk_p = (c_ca)^1*(c_oh)^2
       kk_c = (c_ca)^3*(c_aloh4)^2*(c_oh)^4
       kk_f = (c_ca)^4*(c_aloh4)^2*(c_oh)^4*(c_cl)^2
       kk_a = (c_aloh4)^1*(c_oh)^-1
    */
    kk_c = k_c*(1. + 0.5*(zc - fabs(zc))) ;
    kk_p = k_p*(1. + 0.5*(zp - fabs(zp))) ;
    kk_f = k_f*(1. + 0.5*(zf - fabs(zf))) ;
    kk_a = k_a*(1. + 0.5*(za - fabs(za))) ;

    /* concentrations a partir des produits */
    c_ca     = pow(kk_f/kk_c,4.)/(c_cl*c_cl) ;
    c_oh     = kk_p*pow(kk_c/kk_f,2.)*c_cl ;
    c_aloh4  = pow(kk_c*kk_c/(kk_p*kk_f),2.)*c_cl ;
    c_h	     = k_e/c_oh ;

    C_OH(i)   = c_oh ;
    C_AlOH4(i)= c_aloh4 ;
    C_Ca(i)   = c_ca ;

    /* chlores et hydroxyles fixes sur les CSH */
    s_cl     = alpha*s_csh*c_cl*beta/(c_oh + beta*c_cl) ;
    s_oh     = s_oho - s_cl ; 

    /* porosite */
    phi      = phi0 ;

    /* contenus en atomes */
    N_Cl(i)  = phi*c_cl + s_cl + 2*s_sf ;
    N_Na(i)  = phi*c_na ;
    N_K(i)   = phi*c_k ;
    N_Ca(i)  = phi*c_ca + s_ch + 3*s_c3a + 4*s_sf ;
    N_Al(i)  = phi*c_aloh4 + 2*s_c3a + 2*s_sf + s_aloh3 ;
    /* charge */
    N_q(i)   = z_cl*c_cl + z_oh*c_oh + z_h*c_h + z_na*c_na + z_k*c_k + z_ca*c_ca + z_aloh4*c_aloh4 ;
  }
  
  /* Coefficients de transfert */
  c_cl     = (C_Cl(0) + C_Cl(1))/deux ;
  c_na     = (C_Na(0) + C_Na(1))/deux ;
  c_k      = (C_K(0)  + C_K(1))/deux ;
  
  /* variables : z = s/s_rf + P/K - 1 */
  zc       = (ZC(0) + ZC(1))/deux ;
  zp       = (ZP(0) + ZP(1))/deux ;
  zf       = (ZF(0) + ZF(1))/deux ;
  za       = (ZA(0) + ZA(1))/deux ;    
  
  /* les produits kk_i */
  /*
    kk_p = (c_ca)^1*(c_oh)^2
    kk_c = (c_ca)^3*(c_aloh4)^2*(c_oh)^4
    kk_f = (c_ca)^4*(c_aloh4)^2*(c_oh)^4*(c_cl)^2
    kk_a = (c_aloh4)^1*(c_oh)^-1
  */
  kk_c = k_c*(1. + 0.5*(zc - fabs(zc))) ;
  kk_p = k_p*(1. + 0.5*(zp - fabs(zp))) ;
  kk_f = k_f*(1. + 0.5*(zf - fabs(zf))) ;
  kk_a = k_a*(1. + 0.5*(za - fabs(za))) ;

  /* calcul des concentrations a partir des produits */
  c_ca     = pow(kk_f/kk_c,4.)/(c_cl*c_cl) ;
  c_oh     = kk_p*pow(kk_c/kk_f,2.)*c_cl ;
  c_aloh4  = pow(kk_c*kk_c/(kk_p*kk_f),2.)*c_cl ;
  c_h	   = k_e/c_oh ;

  /* tortuosites */
  tau_n    = d_cl/do_cl ;
  tau_p    = tau_n/r_d ;
  
  /* Coefficients de transfert */
  KF_Cl    = tau_n*do_cl ;
  KF_OH    = tau_n*do_oh ;
  KF_Na    = tau_p*do_na ;
  KF_K     = tau_p*do_k ;
  KF_H     = tau_p*do_h ;
  KF_Ca    = tau_p*do_ca ;
  KF_AlOH4 = tau_n*do_aloh4 ;
  
  KM_Cl    = z_cl*KF_Cl*c_cl*F/RT ;	
  KM_OH    = z_oh*KF_OH*c_oh*F/RT ;
  KM_Na    = z_na*KF_Na*c_na*F/RT ;
  KM_K     = z_k*KF_K*c_k*F/RT ;	
  KM_H     = z_h*KF_H*c_h*F/RT ;	
  KM_Ca    = z_ca*KF_Ca*c_ca*F/RT ;
  KM_AlOH4 = z_aloh4*KF_AlOH4*c_aloh4*F/RT ;

  /* gradients */
  dx     = x[1][0] - x[0][0] ;
  
  grd_cl    = (C_Cl(1) - C_Cl(0))/dx ;
  grd_na    = (C_Na(1) - C_Na(0))/dx ;
  grd_k     = (C_K(1)  - C_K(0))/dx ;
  
  grd_psi   = (PSI(1) - PSI(0))/dx ;
  
  grd_oh    = (C_OH(1) - C_OH(0))/dx ;
  grd_ca    = (C_Ca(1) - C_Ca(0))/dx ;
  grd_aloh4 = (C_AlOH4(1) - C_AlOH4(0))/dx ;
  grd_h     = (k_e/C_OH(1) - k_e/C_OH(0))/dx ;

  /*
  grd_zp    = (ZP(1) - ZP(0))/dx ;
  grd_zc    = (ZC(1) - ZC(0))/dx ;
  grd_zf    = (ZF(1) - ZF(0))/dx ;

  grd_oh    = DC_OHSDC_CL*grd_cl + DC_OHSDZP*grd_zp + DC_OHSDZC*grd_zc + DC_OHSDZF*grd_zf ;
  grd_h     = DC_HSDC_CL*grd_cl + DC_HSDZP*grd_zp + DC_HSDZC*grd_zc + DC_HSDZF*grd_zf ;
  grd_ca    = DC_CASDC_CL*grd_cl + DC_CASDZC*grd_zc + DC_CASDZF*grd_zf ;
  grd_aloh4 = DC_ALOH4SDC_CL*grd_cl + DC_ALOH4SDZP*grd_zp + DC_ALOH4SDZC*grd_zc + DC_ALOH4SDZF*grd_zf ;
  */
  
  /* Flux */
  w_oh   = - KF_OH*grd_oh - KM_OH*grd_psi ;
  w_h    = - KF_H*grd_h - KM_H*grd_psi ;
  W_Cl   = - KF_Cl*grd_cl - KM_Cl*grd_psi ;
  W_Na   = - KF_Na*grd_na - KM_Na*grd_psi ;
  W_K    = - KF_K*grd_k - KM_K*grd_psi ;
  W_Ca   = - KF_Ca*grd_ca - KM_Ca*grd_psi ;
  W_Al   = - KF_AlOH4*grd_aloh4 - KM_AlOH4*grd_psi ;
  W_q    = z_oh*w_oh + z_h*w_h + z_cl*W_Cl + z_na*W_Na + z_k*W_K + z_ca*W_Ca + z_aloh4*W_Al ;
  
#undef C_Cl
#undef ZC
#undef C_Na
#undef C_K
#undef ZP
#undef ZF
#undef ZA
#undef PSI

#undef N_Cl
#undef N_Na
#undef N_K
#undef N_Ca
#undef N_Al
#undef N_q

#undef W_Cl 
#undef W_q
#undef W_Na 
#undef W_K  
#undef W_Ca 
#undef W_Al

#undef C_OH
#undef C_Ca
#undef C_AlOH4

#undef KF_Cl
#undef KF_OH
#undef KF_Na      
#undef KF_K  
#undef KF_H  
#undef KF_Ca  
#undef KF_AlOH4  

#undef KM_Cl
#undef KM_OH
#undef KM_Na      
#undef KM_K  
#undef KM_H  
#undef KM_Ca  
#undef KM_AlOH4

#undef s_ch0
#undef s_c3a0
#undef s_sf0
#undef s_aloh30
}


int ex42(double **x,double **u,double *f,double *va,elem_t el,int dim,geom_t geom,double t) 
/* Termes explicites (va)  */
{
#define C_Cl(n)     (u[(n)][I_c_cl])
#define ZP(n)       (u[(n)][I_zp])
#define ZC(n)       (u[(n)][I_zc])
#define ZF(n)	    (u[(n)][I_zf])
#define C_Na(n)     (u[(n)][I_c_na])
#define C_K(n)      (u[(n)][I_c_k])
#define ZA(n)       (u[(n)][I_za])
#define PSI(n)      (u[(n)][I_psi])

#define C_OH(n)     (f[(n+20)])
#define C_Ca(n)     (f[(n+22)])
#define C_AlOH4(n)  (f[(n+24)])

#define KF_Cl       (va[(0)])
#define KF_OH       (va[(1)])
#define KF_Na       (va[(2)])
#define KF_K        (va[(3)])
#define KF_H        (va[(4)])
#define KF_Ca       (va[(5)])
#define KF_AlOH4    (va[(6)])

#define KM_Cl       (va[(7)])
#define KM_OH       (va[(8)])
#define KM_Na       (va[(9)])
#define KM_K        (va[(10)])
#define KM_H        (va[(11)])
#define KM_Ca       (va[(12)])
#define KM_AlOH4    (va[(13)])

#define s_ch0       (va[(14)])
#define s_c3a0      (va[(15)])
#define s_sf0       (va[(16)])
#define s_aloh30    (va[(17)])

  double c_cl,c_oh,c_na,c_k,c_h,c_ca,c_aloh4 ;
  double zp,kk_p ;
  double zc,kk_c ;
  double zf,kk_f ;
  double za,kk_a ;
  double phi ;
  double tau_n,tau_p ;
  double s_ch,s_c3a,s_sf,s_aloh3 ;
  double deux = 2. ;

  if(el.dim < dim) return(0) ;

  /*
    Donnees
  */
  phi0    = el.mat->pr[pm("porosite")] ;
  d_cl    = el.mat->pr[pm("D_Cl")] ;
  r_d     = el.mat->pr[pm("r_d")] ;

  s_chrf   = el.mat->pr[pm("s_caoh2")] ;
  s_c3arf  = el.mat->pr[pm("s_c3ah6")] ;
  s_sfrf   = el.mat->pr[pm("s_sel")] ;
  s_aloh3rf= el.mat->pr[pm("s_aloh3")] ;

  s_csh    = el.mat->pr[pm("s_csh")] ;
  alpha    = el.mat->pr[pm("alpha")] ;
  beta     = el.mat->pr[pm("beta")] ;

  /* concentrations */
  c_cl     = (C_Cl(0) + C_Cl(1))/deux ;
  c_na     = (C_Na(0) + C_Na(1))/deux ;
  c_k      = (C_K(0)  + C_K(1))/deux ;
  
  /* variables : z = s/s_rf + P/K - 1 */
  zc       = (ZC(0) + ZC(1))/deux ;
  zp       = (ZP(0) + ZP(1))/deux ;
  zf       = (ZF(0) + ZF(1))/deux ;
  za       = (ZA(0) + ZA(1))/deux ;
  
  /* teneurs en solides */
  s_ch     = 0.5*(zp + fabs(zp))*s_chrf ;                      
  s_c3a    = 0.5*(zc + fabs(zc))*s_c3arf ;             
  s_sf     = 0.5*(zf + fabs(zf))*s_sfrf ;
  s_aloh3  = 0.5*(za + fabs(za))*s_aloh3rf ;	
  
  /* les produits kk_i */
  /*
    kk_p = (c_ca)^1*(c_oh)^2
    kk_c = (c_ca)^3*(c_aloh4)^2*(c_oh)^4
    kk_f = (c_ca)^4*(c_aloh4)^2*(c_oh)^4*(c_cl)^2
    kk_a = (c_aloh4)^1*(c_oh)^-1
  */
  kk_c = k_c*(1. + 0.5*(zc - fabs(zc))) ;
  kk_p = k_p*(1. + 0.5*(zp - fabs(zp))) ;
  kk_f = k_f*(1. + 0.5*(zf - fabs(zf))) ;
  kk_a = k_a*(1. + 0.5*(za - fabs(za))) ;

  /* concentrations a partir des produits */
  c_ca    = pow(kk_f/kk_c,4.)/(c_cl*c_cl) ;
  c_oh    = kk_p*pow(kk_c/kk_f,2.)*c_cl ;
  c_aloh4 = pow(kk_c*kk_c/(kk_p*kk_f),2.)*c_cl ;
  c_h	  = k_e/c_oh ;
  
  /* porosite */
  phi  = phi0 + (s_ch0 - s_ch)*v_caoh2 + (s_sf0 - s_sf)*v_sel + (s_c3a0 - s_c3a)*v_c3ah6 + (s_aloh30 - s_aloh3)*v_aloh3 ;
  
  /* tortuosites */
  tau_n    = d_cl/do_cl*phi/phi0*tortuosite(phi)/tortuosite(phi0) ;
  tau_p    = tau_n/r_d ;
  
  /* Coefficients de transfert */
  KF_Cl    = tau_n*do_cl ;
  KF_OH    = tau_n*do_oh ;
  KF_Na    = tau_p*do_na ;
  KF_K     = tau_p*do_k ;
  KF_H     = tau_p*do_h ;
  KF_Ca    = tau_p*do_ca ;
  KF_AlOH4 = tau_n*do_aloh4 ;
  
  KM_Cl    = z_cl*KF_Cl*c_cl*F/RT ;	
  KM_OH    = z_oh*KF_OH*c_oh*F/RT ;
  KM_Na    = z_na*KF_Na*c_na*F/RT ;
  KM_K     = z_k*KF_K*c_k*F/RT ;	
  KM_H     = z_h*KF_H*c_h*F/RT ;	
  KM_Ca    = z_ca*KF_Ca*c_ca*F/RT ;
  KM_AlOH4 = z_aloh4*KF_AlOH4*c_aloh4*F/RT ;
  
  /* les derivees */
  /*
  dkk_csdzc = (zc < 0.) ? k_c : 0. ;
  dkk_psdzp = (zp < 0.) ? k_p : 0. ;
  dkk_fsdzf = (zf < 0.) ? k_f : 0. ;
  dkk_asdza = (za < 0.) ? k_a : 0. ;

  DC_CASDC_CL    = -2*c_ca/c_cl ;
  DC_CASDZC      = -4*c_ca/kk_c*dkk_csdzc ;
  DC_CASDZF      =  4*c_ca/kk_f*dkk_fsdzf ;
  
  DC_OHSDC_CL    = -0.5*c_oh/c_ca*DC_CASDC_CL ;
  DC_OHSDZP      = c_oh/kk_p*dkk_psdzp ;
  DC_OHSDZC      = -0.5*c_oh/c_ca*DC_CASDZC ;
  DC_OHSDZF      = -0.5*c_oh/c_ca*DC_CASDZF ;
  
  DC_HSDC_CL     = -c_h/c_oh*DC_OHSDC_CL ;
  DC_HSDZP       = -c_h/c_oh*DC_OHSDZP ;
  DC_HSDZC       = -c_h/c_oh*DC_OHSDZC ;
  DC_HSDZF       = -c_h/c_oh*DC_OHSDZF ;
  
  DC_ALOH4SDC_CL = -0.5*c_aloh4/c_ca*DC_CASDC_CL ;
  DC_ALOH4SDZP   = -2*c_aloh4/kk_p*dkk_psdzp ;
  DC_ALOH4SDZC   = c_aloh4*(2./kk_c*dkk_csdzc - 0.5/c_ca*DC_CASDZC) ;
  DC_ALOH4SDZF   = -0.5*c_aloh4/c_ca*DC_CASDZF ;
  */
   
  return(0) ;
  
#undef C_Cl
#undef ZC
#undef C_Na
#undef C_K
#undef ZP
#undef ZF
#undef ZA
#undef PSI

#undef C_OH
#undef C_Ca
#undef C_AlOH4

#undef KF_Cl
#undef KF_OH
#undef KF_Na      
#undef KF_K  
#undef KF_H  
#undef KF_Ca  
#undef KF_AlOH4  

#undef KM_Cl
#undef KM_OH
#undef KM_Na      
#undef KM_K  
#undef KM_H  
#undef KM_Ca  
#undef KM_AlOH4 

#undef s_ch0
#undef s_c3a0
#undef s_sf0
#undef s_aloh30
}

int ct42(double **x,double **u_1,double **u_n,double *f_1,double *f_n,double *va,elem_t el,int dim,geom_t geom,double dt,double t)
/* Les variables donnees par la loi de comportement (f_1) */
{

#define C_Cl(n)     (u_1[(n)][I_c_cl])
#define ZP(n)       (u_1[(n)][I_zp])
#define ZC(n)       (u_1[(n)][I_zc])
#define ZF(n)	    (u_1[(n)][I_zf])
#define C_Na(n)     (u_1[(n)][I_c_na])
#define C_K(n)      (u_1[(n)][I_c_k])
#define ZA(n)       (u_1[(n)][I_za])
#define PSI(n)      (u_1[(n)][I_psi])

#define N_Cl(n)     (f_1[(n)])
#define N_Na(n)     (f_1[(n+4)])
#define N_K(n)      (f_1[(n+6)])
#define N_Ca(n)     (f_1[(n+8)])
#define N_Al(n)     (f_1[(n+10)])
#define N_q(n)      (f_1[(n+12)])

#define W_Cl        (f_1[14])
#define W_q        (f_1[15])
#define W_Na        (f_1[16])
#define W_K         (f_1[17])
#define W_Ca        (f_1[18])
#define W_Al        (f_1[19])

#define C_OH(n)     (f_1[(n+20)])
#define C_Ca(n)     (f_1[(n+22)])
#define C_AlOH4(n)  (f_1[(n+24)])

#define KF_Cl       (va[(0)])
#define KF_OH       (va[(1)])
#define KF_Na       (va[(2)])
#define KF_K        (va[(3)])
#define KF_H        (va[(4)])
#define KF_Ca       (va[(5)])
#define KF_AlOH4    (va[(6)])

#define KM_Cl       (va[(7)])
#define KM_OH       (va[(8)])
#define KM_Na       (va[(9)])
#define KM_K        (va[(10)])
#define KM_H        (va[(11)])
#define KM_Ca       (va[(12)])
#define KM_AlOH4    (va[(13)])

#define s_ch0       (va[(14)])
#define s_c3a0      (va[(15)])
#define s_sf0       (va[(16)])
#define s_aloh30    (va[(17)])

  double c_cl,c_oh,c_na,c_k,c_h,c_ca,c_aloh4;
  double zp,kk_p ;
  double zc,kk_c ;
  double zf,kk_f ;
  double za,kk_a ;
  double grd_cl,grd_oh,grd_na,grd_k,grd_h,grd_ca,grd_aloh4,grd_psi ;
  double s_cl,s_ch,s_c3a,s_sf,s_aloh3,s_oh ;
  double w_oh,w_h ;
  double dx,phi;
  int    i ;
  double zero = 0. ;
  
  if(el.dim < dim) return(0) ;

  /*
    Donnees
  */

  phi0    = el.mat->pr[pm("porosite")] ;
  d_cl    = el.mat->pr[pm("D_Cl")] ;

  s_chrf   = el.mat->pr[pm("s_caoh2")] ;
  s_c3arf  = el.mat->pr[pm("s_c3ah6")] ;
  s_sfrf   = el.mat->pr[pm("s_sel")] ;
  s_aloh3rf= el.mat->pr[pm("s_aloh3")] ;

  s_csh    = el.mat->pr[pm("s_csh")] ;
  alpha    = el.mat->pr[pm("alpha")] ;
  beta     = el.mat->pr[pm("beta")] ;
   
  /* Contenus molaires */

  for(i=0;i<2;i++) {
    c_cl    = C_Cl(i) ;
    c_na    = C_Na(i) ;
    c_k     = C_K(i) ;

    /* variables : z = s/s_rf + P/K - 1 */
    zc      = ZC(i) ;
    zp	    = ZP(i) ;
    zf	    = ZF(i) ;
    za      = ZA(i) ;

    /* teneurs en solides */
    s_ch    = 0.5*(zp + fabs(zp))*s_chrf ;
    s_c3a   = 0.5*(zc + fabs(zc))*s_c3arf ;
    s_sf    = 0.5*(zf + fabs(zf))*s_sfrf ;
    s_aloh3 = 0.5*(za + fabs(za))*s_aloh3rf ;

    /* les produits kk_i */
    /*
       kk_p = (c_ca)^1*(c_oh)^2
       kk_c = (c_ca)^3*(c_aloh4)^2*(c_oh)^4
       kk_f = (c_ca)^4*(c_aloh4)^2*(c_oh)^4*(c_cl)^2
       kk_a = (c_aloh4)^1*(c_oh)^-1
    */
    kk_c    = k_c*(1. + 0.5*(zc - fabs(zc))) ;
    kk_p    = k_p*(1. + 0.5*(zp - fabs(zp))) ;
    kk_f    = k_f*(1. + 0.5*(zf - fabs(zf))) ;
    kk_a    = k_a*(1. + 0.5*(za - fabs(za))) ;

    /* concentrations a partir des produits */
    c_ca     = pow(kk_f/kk_c,4.)/(c_cl*c_cl) ;
    c_oh     = kk_p*pow(kk_c/kk_f,2.)*c_cl ;
    c_aloh4  = pow(kk_c*kk_c/(kk_p*kk_f),2.)*c_cl ;
    c_h	     = k_e/c_oh ;

    C_OH(i)   = c_oh ;
    C_AlOH4(i)= c_aloh4 ;
    C_Ca(i)   = c_ca ;

    if(c_cl < zero || c_na < zero || c_k < zero || zc < -1. || zp < -1. || zf < -1. || za < -1. || (zc > zero && zp > zero && za > zero)) {
      fprintf(stdout,"\n\
\t En x    = %e\n",x[i][0]) ;
      fprintf(stdout,"\n\
\t Concentrations\n\
\t c_cl    = %e\n\
\t c_na    = %e\n\
\t c_k     = %e\n\
\t c_ca    = %e\n\
\t c_oh    = %e\n\
\t c_aloh4 = %e\n\
",c_cl,c_na,c_k,c_ca,c_oh,c_aloh4) ;
      fprintf(stdout,"\n\
\t z_i\n\
\t zp      = %e\n\
\t zc      = %e\n\
\t zf      = %e\n\
\t za      = %e\n\
",zp,zc,zf,za) ;
      fprintf(stdout,"\n\
\t produits kk_i\n\
\t kk_c    = %e\n\
\t kk_p    = %e\n\
\t kk_f    = %e\n\
\t kk_a    = %e\n\
",kk_c,kk_p,kk_f,kk_a) ;
      fprintf(stdout,"\n\
\t Teneurs solides kk_i\n\
\t s_ch    = %e\n\
\t s_c3a   = %e\n\
\t s_sf    = %e\n\
\t s_aloh3 = %e\n\
",s_ch,s_c3a,s_sf,s_aloh3) ;
	return(-1);
    }

    /* chlores et hydroxyles fixes sur les CSH */
    s_cl    = alpha*s_csh*c_cl*beta/(c_oh+beta*c_cl) ;
    s_oh    = s_oho - s_cl ;

    /* porosite */
    phi = phi0 + (s_ch0 - s_ch)*v_caoh2 + (s_sf0 - s_sf)*v_sel + (s_c3a0 - s_c3a)*v_c3ah6 + (s_aloh30 - s_aloh3)*v_aloh3 ;
  
    /* contenus en atomes */
    N_Cl(i)  = phi*c_cl + s_cl + 2*s_sf ;
    N_Na(i)  = phi*c_na ;
    N_K(i)   = phi*c_k ;
    N_Ca(i)  = phi*c_ca + s_ch + 3*s_c3a + 4*s_sf ;
    N_Al(i)  = phi*c_aloh4 + 2*s_c3a + 2*s_sf + s_aloh3 ;
    /* charge */
    N_q(i)   = z_cl*c_cl + z_oh*c_oh + z_h*c_h + z_na*c_na + z_k*c_k + z_ca*c_ca + z_aloh4*c_aloh4 ;
  }
  
    /* gradients */
    dx     = x[1][0] - x[0][0] ;

    grd_cl    = (C_Cl(1) - C_Cl(0))/dx ;
    grd_na    = (C_Na(1) - C_Na(0))/dx ;
    grd_k     = (C_K(1)  - C_K(0))/dx ;

    grd_psi   = (PSI(1) - PSI(0))/dx ;  
 
    grd_oh    = (C_OH(1) - C_OH(0))/dx ;
    grd_ca    = (C_Ca(1) - C_Ca(0))/dx ;
    grd_aloh4 = (C_AlOH4(1) - C_AlOH4(0))/dx ;
    grd_h     = (k_e/C_OH(1) - k_e/C_OH(0))/dx ;

    /*
    grd_zp    = (ZP(1) - ZP(0))/dx ;
    grd_zc    = (ZC(1) - ZC(0))/dx ;
    grd_zf    = (ZF(1) - ZF(0))/dx ;

    grd_oh    = DC_OHSDC_CL*grd_cl + DC_OHSDZP*grd_zp + DC_OHSDZC*grd_zc + DC_OHSDZF*grd_zf ;
    grd_h     = DC_HSDC_CL*grd_cl + DC_HSDZP*grd_zp + DC_HSDZC*grd_zc + DC_HSDZF*grd_zf ;
    grd_ca    = DC_CASDC_CL*grd_cl + DC_CASDZC*grd_zc + DC_CASDZF*grd_zf ;
    grd_aloh4 = DC_ALOH4SDC_CL*grd_cl + DC_ALOH4SDZP*grd_zp + DC_ALOH4SDZC*grd_zc + DC_ALOH4SDZF*grd_zf ;
    */
    

    /* flux */
  w_oh   = - KF_OH*grd_oh - KM_OH*grd_psi ;
  w_h    = - KF_H*grd_h - KM_H*grd_psi ;
    W_Cl   = - KF_Cl*grd_cl - KM_Cl*grd_psi ;
    W_Na   = - KF_Na*grd_na - KM_Na*grd_psi ;
    W_K    = - KF_K*grd_k - KM_K*grd_psi ;
    W_Ca   = - KF_Ca*grd_ca - KM_Ca*grd_psi ;
    W_Al   = - KF_AlOH4*grd_aloh4 - KM_AlOH4*grd_psi ;
  W_q    = z_oh*w_oh + z_h*w_h + z_cl*W_Cl + z_na*W_Na + z_k*W_K + z_ca*W_Ca + z_aloh4*W_Al ;

    return(0) ;
		
#undef C_Cl
#undef ZC
#undef C_Na
#undef C_K
#undef ZP
#undef ZF
#undef ZA
#undef PSI

#undef N_Cl
#undef N_Na
#undef N_K
#undef N_Ca
#undef N_Al
#undef N_q

#undef W_Cl 
#undef W_q 
#undef W_Na 
#undef W_K  
#undef W_Ca 
#undef W_Al

#undef C_OH
#undef C_Ca
#undef C_AlOH4

#undef KF_Cl
#undef KF_OH
#undef KF_Na      
#undef KF_K  
#undef KF_H  
#undef KF_Ca
#undef KF_AlOH4

#undef KM_Cl
#undef KM_OH
#undef KM_Na      
#undef KM_K  
#undef KM_H  
#undef KM_Ca  
#undef KM_AlOH4

#undef s_ch0
#undef s_c3a0
#undef s_sf0
#undef s_aloh30
}

int mx42(double **x,double **u_1,double **u_n,double *f_1,double *f_n,double *va,double *k,elem_t el,int dim,geom_t geom,double dt,double t)
/* Matrice (k) */
{
#define C_Cl(n)     (u_1[(n)][I_c_cl])
#define ZP(n)       (u_1[(n)][I_zp])
#define ZC(n)       (u_1[(n)][I_zc])
#define ZF(n)	    (u_1[(n)][I_zf])
#define C_Na(n)     (u_1[(n)][I_c_na])
#define C_K(n)      (u_1[(n)][I_c_k])
#define ZA(n)       (u_1[(n)][I_za])
#define PSI(n)      (u_1[(n)][I_psi])

#define C_OH(n)     (f_1[(n+20)])
#define C_Ca(n)     (f_1[(n+22)])
#define C_AlOH4(n)  (f_1[(n+24)])

#define KF_Cl       (va[(0)])
#define KF_OH       (va[(1)])
#define KF_Na       (va[(2)])
#define KF_K        (va[(3)])
#define KF_H        (va[(4)])
#define KF_Ca       (va[(5)])
#define KF_AlOH4    (va[(6)])

#define KM_Cl       (va[(7)])
#define KM_OH       (va[(8)])
#define KM_Na       (va[(9)])
#define KM_K        (va[(10)])
#define KM_H        (va[(11)])
#define KM_Ca       (va[(12)])
#define KM_AlOH4    (va[(13)])

#define s_ch0       (va[(14)])
#define s_c3a0      (va[(15)])
#define s_sf0       (va[(16)])
#define s_aloh30    (va[(17)])

#define K(i,j)    (k[(i)*el.nn*NEQ+(j)])
  
  double c_oh,c_cl,s_cl, c_ca,c_h,c_aloh4,c_na,c_k ;
  double s_ch,s_c3a,s_sf,s_aloh3 ;
  double zp,kk_p,dkk_psdzp ;
  double zc,kk_c,dkk_csdzc ;
  double zf,kk_f,dkk_fsdzf ;
  double za,kk_a,dkk_asdza ;
  double ds_chsdzp,ds_c3asdzc,ds_sfsdzf,ds_aloh3sdza ;
  double ds_clsdc_cl,ds_clsdzp,ds_clsdzc,ds_clsdzf ;
  double dc_ohsdc_cl[2],dc_ohsdzc[2],dc_ohsdzp[2],dc_ohsdzf[2] ;
  double dc_hsdc_cl[2],dc_hsdzc[2],dc_hsdzp[2],dc_hsdzf[2] ;
  double dc_casdc_cl[2],dc_casdzf[2],dc_casdzc[2] ;
  double dc_aloh4sdc_cl[2],dc_aloh4sdzp[2],dc_aloh4sdzc[2],dc_aloh4sdzf[2] ;
  double phi,dphisdzp,dphisdzf,dphisdzc,dphisdza ;
  double tr ;
  double trf_cl,trf_oh,trf_h,trf_ca,trf_na,trf_k,trf_aloh4 ;
  double tre_cl,tre_oh,tre_h,tre_ca,tre_na,tre_k,tre_aloh4 ;
  double dx,xm ;
  double volume[2],surf ;
  double c[2] ;
  int    i ;
  double zero = 0.,un = 1.,deux = 2. ;
  
  /* initialisation */
  for(i=0;i<el.nn*el.nn*NEQ*NEQ;i++) k[i] = zero ;

  if(el.dim < dim) return(0) ;

  /*
    Donnees
  */
 
  phi0    = el.mat->pr[pm("porosite")] ;
  d_cl    = el.mat->pr[pm("D_Cl")] ;

  s_chrf   = el.mat->pr[pm("s_caoh2")] ;
  s_c3arf  = el.mat->pr[pm("s_c3ah6")] ;
  s_sfrf   = el.mat->pr[pm("s_sel")] ;
  s_aloh3rf= el.mat->pr[pm("s_aloh3")] ;

  s_csh    = el.mat->pr[pm("s_csh")] ;
  alpha    = el.mat->pr[pm("alpha")] ;
  beta     = el.mat->pr[pm("beta")] ;
	
  /*
    CALCUL DE volume ET DE surf 
  */
  dx = x[1][0] - x[0][0] ;
  xm = (x[1][0] + x[0][0])/deux ;
  for(i=0;i<2;i++) {
    volume[i] = fabs(dx)/deux ; 
    if(geom == AXIS) volume[i] *= M_PI*(x[i][0] + xm) ; 
  }
  if(geom == AXIS) surf = deux*M_PI*xm ; else surf = un ;

  /* termes d'accumulation */
  for(i=0;i<2;i++) {
    c_cl     = C_Cl(i) ;
    c_na     = C_Na(i) ;
    c_k      = C_K(i) ;

    /* variables : z = s/s_rf + P/K - 1 */
    zc       = ZC(i) ;
    zp       = ZP(i) ;
    zf       = ZF(i) ;
    za       = ZA(i) ;

    /* teneurs en solides */
    s_ch    = 0.5*(zp + fabs(zp))*s_chrf ;                      
    s_c3a   = 0.5*(zc + fabs(zc))*s_c3arf ;             
    s_sf    = 0.5*(zf + fabs(zf))*s_sfrf ;
    s_aloh3 = 0.5*(za + fabs(za))*s_aloh3rf ;

    /* les produits kk_i */
    /*
       kk_p = (c_ca)^1*(c_oh)^2
       kk_c = (c_ca)^3*(c_aloh4)^2*(c_oh)^4
       kk_f = (c_ca)^4*(c_aloh4)^2*(c_oh)^4*(c_cl)^2
       kk_a = (c_aloh4)^1*(c_oh)^-1
    */
    kk_c = k_c*(1. + 0.5*(zc - fabs(zc))) ;
    kk_p = k_p*(1. + 0.5*(zp - fabs(zp))) ;
    kk_f = k_f*(1. + 0.5*(zf - fabs(zf))) ;
    kk_a = k_a*(1. + 0.5*(za - fabs(za))) ;

    /* concentrations a partir des produits */
    c_ca    = pow(kk_f/kk_c,4.)/(c_cl*c_cl) ;
    c_oh    = kk_p*pow(kk_c/kk_f,2.)*c_cl ;
    c_aloh4 = pow(kk_c*kk_c/(kk_p*kk_f),2.)*c_cl ;
    c_h	    = k_e/c_oh ;

    /* chlores fixes sur les CSH */
    s_cl     = alpha*s_csh*c_cl*beta/(c_oh + beta*c_cl) ;
    
    /* porosite */
    phi = phi0 + (s_ch0 - s_ch)*v_caoh2 + (s_sf0 - s_sf)*v_sel + (s_c3a0 - s_c3a)*v_c3ah6 + (s_aloh30 - s_aloh3)*v_aloh3 ;

    /* les derivees */
    dkk_csdzc = (zc < 0.) ? k_c : 0. ;
    dkk_psdzp = (zp < 0.) ? k_p : 0. ;
    dkk_fsdzf = (zf < 0.) ? k_f : 0. ;
    dkk_asdza = (za < 0.) ? k_a : 0. ;	
    
    dc_casdc_cl[i]    = - 2*c_ca/c_cl ;
    dc_casdzc[i]      = - 4*c_ca/kk_c*dkk_csdzc ;
    dc_casdzf[i]      =   4*c_ca/kk_f*dkk_fsdzf ;

    dc_ohsdc_cl[i]    =   c_oh/c_cl ;
    dc_ohsdzp[i]      =   c_oh/kk_p*dkk_psdzp ;
    dc_ohsdzc[i]      =   2*c_oh/kk_c*dkk_csdzc ;
    dc_ohsdzf[i]      = - 2*c_oh/kk_f*dkk_fsdzf ;

    dc_hsdc_cl[i]     = - c_h/c_oh*dc_ohsdc_cl[i] ;
    dc_hsdzp[i]       = - c_h/c_oh*dc_ohsdzp[i] ;
    dc_hsdzc[i]       = - c_h/c_oh*dc_ohsdzc[i] ;
    dc_hsdzf[i]       = - c_h/c_oh*dc_ohsdzf[i] ;
    
    dc_aloh4sdc_cl[i] =   c_aloh4/c_cl ;
    dc_aloh4sdzp[i]   = - 2*c_aloh4/kk_p*dkk_psdzp ;
    dc_aloh4sdzc[i]   =   4*c_aloh4/kk_c*dkk_csdzc ;
    dc_aloh4sdzf[i]   = - 2*c_aloh4/kk_f*dkk_fsdzf ;

    ds_chsdzp      = (zp > 0.) ? s_chrf    : 0. ;
    ds_c3asdzc     = (zc > 0.) ? s_c3arf   : 0. ;
    ds_sfsdzf      = (zf > 0.) ? s_sfrf    : 0. ;
    ds_aloh3sdza   = (za > 0.) ? s_aloh3rf : 0. ;

    ds_clsdc_cl    =   s_cl/(c_oh + beta*c_cl)*(c_oh/c_cl - dc_ohsdc_cl[i]) ;
    ds_clsdzp      = - s_cl/(c_oh + beta*c_cl)*dc_ohsdzp[i] ;
    ds_clsdzc      = - s_cl/(c_oh + beta*c_cl)*dc_ohsdzc[i] ;
    ds_clsdzf      = - s_cl/(c_oh + beta*c_cl)*dc_ohsdzf[i] ;

    dphisdzp       = - ds_chsdzp*v_caoh2 ;
    dphisdzc       = - ds_c3asdzc*v_c3ah6 ;
    dphisdzf       = - ds_sfsdzf*v_sel ;
    dphisdza       = - ds_aloh3sdza*v_aloh3 ;

    /*
      Conservation de Cl : (n_Cl1 - n_Cln) + dt * div(w_Cl) = 0
    */

    K(i*NEQ+E_Cl,i*NEQ+I_c_cl) += volume[i]*(phi + ds_clsdc_cl) ;
    K(i*NEQ+E_Cl,i*NEQ+I_zp)   += volume[i]*(dphisdzp*c_cl + ds_clsdzp) ;
    K(i*NEQ+E_Cl,i*NEQ+I_zc)   += volume[i]*(dphisdzc*c_cl + ds_clsdzc) ;
    K(i*NEQ+E_Cl,i*NEQ+I_zf)   += volume[i]*(dphisdzf*c_cl + ds_clsdzf + 2*ds_sfsdzf) ;
    K(i*NEQ+E_Cl,i*NEQ+I_za)   += volume[i]*(dphisdza*c_cl) ;

    /*
      Conservation de la charge OH (hydroxy-hydro) : div(w_q) = 0
    */

    /*
      Conservation de Na : (n_Na1 - n_Nan) + dt * div(w_Na) = 0
    */
    K(i*NEQ+E_Na,i*NEQ+I_c_na) += volume[i]*(phi) ;
    K(i*NEQ+E_Na,i*NEQ+I_zp)   += volume[i]*(dphisdzp*c_na) ;
    K(i*NEQ+E_Na,i*NEQ+I_zf)   += volume[i]*(dphisdzf*c_na) ;
    K(i*NEQ+E_Na,i*NEQ+I_zc)   += volume[i]*(dphisdzc*c_na) ;
    K(i*NEQ+E_Na,i*NEQ+I_za)   += volume[i]*(dphisdza*c_na) ;
    
    /*
      Conservation de K : (n_K1 - n_Kn) + dt * div(w_K) = 0
    */
    K(i*NEQ+E_K,i*NEQ+I_c_k) += volume[i]*(phi) ;
    K(i*NEQ+E_K,i*NEQ+I_zp)  += volume[i]*(dphisdzp*c_k) ;
    K(i*NEQ+E_K,i*NEQ+I_zf)  += volume[i]*(dphisdzf*c_k) ;
    K(i*NEQ+E_K,i*NEQ+I_zc)  += volume[i]*(dphisdzc*c_k) ;
    K(i*NEQ+E_K,i*NEQ+I_za)  += volume[i]*(dphisdza*c_k) ;

    /* 
       Conservation de Ca : (n_Ca1 - n_Can) + dt * div(w_Ca) = 0
    */
    K(i*NEQ+E_Ca,i*NEQ+I_c_cl) +=  volume[i]*(phi*dc_casdc_cl[i]) ;
    K(i*NEQ+E_Ca,i*NEQ+I_zp)   +=  volume[i]*(dphisdzp*c_ca + ds_chsdzp) ;
    K(i*NEQ+E_Ca,i*NEQ+I_zc)   +=  volume[i]*(phi*dc_casdzc[i] + dphisdzc*c_ca + 3*ds_c3asdzc) ;
    K(i*NEQ+E_Ca,i*NEQ+I_zf)   +=  volume[i]*(phi*dc_casdzf[i] + dphisdzf*c_ca + 4*ds_sfsdzf) ;
    K(i*NEQ+E_Ca,i*NEQ+I_za)   +=  volume[i]*(dphisdza*c_ca);
    
    /* 
      Conservation de Al : (n_Al1 - n_Aln) + dt * div(w_Al) = 0
    */
    K(i*NEQ+E_Al,i*NEQ+I_c_cl) += volume[i]*(phi*dc_aloh4sdc_cl[i]) ;
    K(i*NEQ+E_Al,i*NEQ+I_zp)   += volume[i]*(phi*dc_aloh4sdzp[i] + dphisdzp*c_aloh4) ;
    K(i*NEQ+E_Al,i*NEQ+I_zc)   += volume[i]*(phi*dc_aloh4sdzc[i] + dphisdzc*c_aloh4 + 2*ds_c3asdzc) ;
    K(i*NEQ+E_Al,i*NEQ+I_zf)   += volume[i]*(phi*dc_aloh4sdzf[i] + dphisdzf*c_aloh4 + 2*ds_sfsdzf) ;
    K(i*NEQ+E_Al,i*NEQ+I_za)   += volume[i]*(dphisdza*c_aloh4 + ds_aloh3sdza) ;

    /*
      Precipitation-Dissolution de AlOH3 : c_aloh4/c_oh <= K_a ; s_aloh3 >= 0
    */

    K(i*NEQ+E_A,i*NEQ+I_zp) += volume[i]*(-3*kk_c*kk_c/(kk_p*kk_p*kk_p*kk_p)*dkk_psdzp) ;
    K(i*NEQ+E_A,i*NEQ+I_zc) += volume[i]*(2*kk_c/(kk_p*kk_p*kk_p)*dkk_csdzc) ;
    K(i*NEQ+E_A,i*NEQ+I_za) += volume[i]*(-dkk_asdza) ;
    
    /*
      Electroneuralite : q = 0
    */
    K(i*NEQ+E_el,i*NEQ+I_c_cl) += volume[i]*(z_cl + z_oh*dc_ohsdc_cl[i] + z_h*dc_hsdc_cl[i] + z_ca*dc_casdc_cl[i] + z_aloh4*dc_aloh4sdc_cl[i]) ;
    K(i*NEQ+E_el,i*NEQ+I_zp)   += volume[i]*(z_oh*dc_ohsdzp[i] + z_h*dc_hsdzp[i] + z_aloh4*dc_aloh4sdzp[i]) ;
    K(i*NEQ+E_el,i*NEQ+I_c_na) += volume[i]*z_na ;
    K(i*NEQ+E_el,i*NEQ+I_c_k)  += volume[i]*z_k ;
    K(i*NEQ+E_el,i*NEQ+I_zc)   += volume[i]*(z_oh*dc_ohsdzc[i] + z_h*dc_hsdzc[i] + z_ca*dc_casdzc[i] + z_aloh4*dc_aloh4sdzc[i]) ;
    K(i*NEQ+E_el,i*NEQ+I_zf)   += volume[i]*(z_oh*dc_ohsdzf[i] + z_h*dc_hsdzf[i] + z_ca*dc_casdzf[i] + z_aloh4*dc_aloh4sdzf[i]) ;  
  }

  /* termes d'ecoulement */
  tr = dt*surf/dx ;

  trf_cl    = tr*KF_Cl ;
  trf_oh    = tr*KF_OH ;
  trf_h     = tr*KF_H ;
  trf_ca    = tr*KF_Ca ;
  trf_na    = tr*KF_Na ;
  trf_k     = tr*KF_K ;
  trf_aloh4 = tr*KF_AlOH4 ;

  tre_cl    = tr*KM_Cl ;
  tre_oh    = tr*KM_OH ;
  tre_h     = tr*KM_H ;
  tre_ca    = tr*KM_Ca ;
  tre_na    = tr*KM_Na ;
  tre_k     = tr*KM_K ;
  tre_aloh4 = tr*KM_AlOH4 ;
  
  /*
    Conservation de Cl : (n_Cl1 - n_Cln) + dt * div(w_Cl) = 0
  */

  K(E_Cl,I_c_cl)           += + trf_cl ;
  K(E_Cl,I_c_cl+NEQ)       += - trf_cl ;
  K(E_Cl+NEQ,I_c_cl)       += - trf_cl ;
  K(E_Cl+NEQ,I_c_cl+NEQ)   += + trf_cl ;
  
  K(E_Cl,I_psi)            += + tre_cl ;
  K(E_Cl,I_psi+NEQ)        += - tre_cl ;
  K(E_Cl+NEQ,I_psi)        += - tre_cl ;
  K(E_Cl+NEQ,I_psi+NEQ)    += + tre_cl ;          

  /*
    Conservation de la charge div(w_q) = 0
  */

  for(i=0;i<2;i++) {
    c[i] = z_oh*trf_oh*dc_ohsdc_cl[i] + z_h*trf_h*dc_hsdc_cl[i] + z_cl*trf_cl + z_ca*trf_ca*dc_casdc_cl[i] + z_aloh4*trf_aloh4*dc_aloh4sdc_cl[i] ;
  }
  K(E_q,I_c_cl)           += + c[0] ; 
  K(E_q,I_c_cl+NEQ)       += - c[1] ;
  K(E_q+NEQ,I_c_cl)       += - c[0] ;
  K(E_q+NEQ,I_c_cl+NEQ)   += + c[1] ;

  for(i=0;i<2;i++) {
    c[i] = z_oh*trf_oh*dc_ohsdzp[i] + z_h*trf_h*dc_hsdzp[i] + z_aloh4*trf_aloh4*dc_aloh4sdzp[i] ;
  }
  K(E_q,I_zp)             += + c[0] ;
  K(E_q,I_zp+NEQ)         += - c[1] ;
  K(E_q+NEQ,I_zp)         += - c[0] ;
  K(E_q+NEQ,I_zp+NEQ)     += + c[1] ;

  for(i=0;i<2;i++) {
    c[i] = z_oh*trf_oh*dc_ohsdzc[i] + z_h*trf_h*dc_hsdzc[i] + z_ca*trf_ca*dc_casdzc[i] + z_aloh4*trf_aloh4*dc_aloh4sdzc[i] ;
  }
  K(E_q,I_zc)             += + c[0] ;
  K(E_q,I_zc+NEQ)         += - c[1] ;
  K(E_q+NEQ,I_zc)         += - c[0] ;
  K(E_q+NEQ,I_zc+NEQ)     += + c[1] ;

  for(i=0;i<2;i++) {
    c[i] = z_oh*trf_oh*dc_ohsdzf[i] + z_h*trf_h*dc_hsdzf[i] + z_ca*trf_ca*dc_casdzf[i] + z_aloh4*trf_aloh4*dc_aloh4sdzf[i] ;
  }
  K(E_q,I_zf)             += + c[0] ;
  K(E_q,I_zf+NEQ)         += - c[1] ;
  K(E_q+NEQ,I_zf)         += - c[0] ;
  K(E_q+NEQ,I_zf+NEQ)     += + c[1] ;

  for(i=0;i<2;i++) {
    c[i] = z_na*trf_na ;
  }
  K(E_q,I_c_na)           += + c[0] ;
  K(E_q,I_c_na+NEQ)       += - c[1] ;
  K(E_q+NEQ,I_c_na)       += - c[0] ;
  K(E_q+NEQ,I_c_na+NEQ)   += + c[1] ;

  for(i=0;i<2;i++) {
    c[i] = z_k*trf_k ;
  }
  K(E_q,I_c_k)            += + c[0] ;
  K(E_q,I_c_k+NEQ)        += - c[1] ;
  K(E_q+NEQ,I_c_k)        += - c[0] ;
  K(E_q+NEQ,I_c_k+NEQ)    += + c[1] ;

  for(i=0;i<2;i++) {
    c[i] = z_oh*tre_oh + z_h*tre_h + z_cl*tre_cl + z_na*tre_na + z_k*tre_k + z_ca*tre_ca + z_aloh4*tre_aloh4 ;
  }
  K(E_q,I_psi)            += + c[0] ;
  K(E_q,I_psi+NEQ)        += - c[1] ;
  K(E_q+NEQ,I_psi)        += - c[0] ;
  K(E_q+NEQ,I_psi+NEQ)    += + c[1] ;

  /*
    Conservation de Na : (n_Na1 - n_Nan) + dt * div(w_Na) = 0
  */

  K(E_Na,I_c_na)           += + trf_na;
  K(E_Na,I_c_na+NEQ)       += - trf_na;
  K(E_Na+NEQ,I_c_na)       += - trf_na;
  K(E_Na+NEQ,I_c_na+NEQ)   += + trf_na;

  K(E_Na,I_psi)            += + tre_na;
  K(E_Na,I_psi+NEQ)        += - tre_na;
  K(E_Na+NEQ,I_psi)        += - tre_na;
  K(E_Na+NEQ,I_psi+NEQ)    += + tre_na;

  /*
    Conservation de K : (n_K1 - n_Kn) + dt * div(w_K) = 0
  */

  K(E_K,I_c_k)             += + trf_k;
  K(E_K,I_c_k+NEQ)         += - trf_k;
  K(E_K+NEQ,I_c_k)         += - trf_k;
  K(E_K+NEQ,I_c_k+NEQ)     += + trf_k;

  K(E_K,I_psi)             += + tre_k;
  K(E_K,I_psi+NEQ)         += - tre_k;
  K(E_K+NEQ,I_psi)         += - tre_k;
  K(E_K+NEQ,I_psi+NEQ)     += + tre_k;

  /*
    Conservation de Ca : (n_Ca1 - n_Can) + dt * div(w_Ca) = 0
  */
  
  for(i=0;i<2;i++) {
    c[i] = trf_ca*dc_casdc_cl[i] ;
  }
  K(E_Ca,I_c_cl)           += + c[0] ;
  K(E_Ca,I_c_cl+NEQ)       += - c[1] ;
  K(E_Ca+NEQ,I_c_cl)       += - c[0] ;
  K(E_Ca+NEQ,I_c_cl+NEQ)   += + c[1] ;

  for(i=0;i<2;i++) {
    c[i] = trf_ca*dc_casdzc[i] ;
  }
  K(E_Ca,I_zc)             += + c[0] ;
  K(E_Ca,I_zc+NEQ)         += - c[1] ;
  K(E_Ca+NEQ,I_zc)         += - c[0] ;
  K(E_Ca+NEQ,I_zc+NEQ)     += + c[1] ;
  
  for(i=0;i<2;i++) {
    c[i] = trf_ca*dc_casdzf[i] ;
  }
  K(E_Ca,I_zf)             += + c[0] ;
  K(E_Ca,I_zf+NEQ)         += - c[1] ;
  K(E_Ca+NEQ,I_zf)         += - c[0] ;
  K(E_Ca+NEQ,I_zf+NEQ)     += + c[1] ;   

  K(E_Ca,I_psi)            += + tre_ca;
  K(E_Ca,I_psi+NEQ)        += - tre_ca;
  K(E_Ca+NEQ,I_psi)        += - tre_ca;
  K(E_Ca+NEQ,I_psi+NEQ)    += + tre_ca;

  /*
    Conservation de Al : (n_Al1 - n_Aln) + dt * div(w_Al) = 0
  */

  for(i=0;i<2;i++) {
    c[i] = trf_aloh4*dc_aloh4sdc_cl[i] ;
  }
  K(E_Al,I_c_cl)           += + c[0] ;
  K(E_Al,I_c_cl+NEQ)       += - c[1] ;
  K(E_Al+NEQ,I_c_cl)       += - c[0] ;
  K(E_Al+NEQ,I_c_cl+NEQ)   += + c[1] ;

  for(i=0;i<2;i++) {
    c[i] = trf_aloh4*dc_aloh4sdzp[i] ;
  }
  K(E_Al,I_zp)             += + c[0] ;
  K(E_Al,I_zp+NEQ)         += - c[1] ;
  K(E_Al+NEQ,I_zp)         += - c[0] ;
  K(E_Al+NEQ,I_zp+NEQ)     += + c[1] ;
  
  for(i=0;i<2;i++) {
    c[i] = trf_aloh4*dc_aloh4sdzc[i] ;
  }
  K(E_Al,I_zc)             += + c[0] ;
  K(E_Al,I_zc+NEQ)         += - c[1] ;
  K(E_Al+NEQ,I_zc)         += - c[0] ;
  K(E_Al+NEQ,I_zc+NEQ)     += + c[1] ;

  for(i=0;i<2;i++) {
    c[i] = trf_aloh4*dc_aloh4sdzf[i] ;
  }
  K(E_Al,I_zf)             += + c[0] ;
  K(E_Al,I_zf+NEQ)         += - c[1] ;
  K(E_Al+NEQ,I_zf)         += - c[0] ;
  K(E_Al+NEQ,I_zf+NEQ)     += + c[1] ;

  K(E_Al,I_psi)            += + tre_aloh4;
  K(E_Al,I_psi+NEQ)        += - tre_aloh4;
  K(E_Al+NEQ,I_psi)        += - tre_aloh4;
  K(E_Al+NEQ,I_psi+NEQ)    += + tre_aloh4;

  /*
    Precipitation-Dissolution de AlOH3 : c_aloh4/c_oh <= K_a ; s_aloh3 >= 0
  */

  /*
    Electroneutralite : q = 0
  */

  return(0) ;
 
#undef C_Cl
#undef ZC
#undef C_Na
#undef C_K
#undef ZP
#undef ZF
#undef ZA
#undef PSI

#undef C_OH
#undef C_Ca
#undef C_AlOH4

#undef KF_Cl
#undef KF_OH
#undef KF_Na
#undef KF_K 
#undef KF_H 
#undef KF_Ca 
#undef KF_AlOH4

#undef KM_Cl
#undef KM_OH
#undef KM_Na      
#undef KM_K  
#undef KM_H  
#undef KM_Ca  
#undef KM_AlOH4

#undef s_ch0
#undef s_c3a0
#undef s_sf0
#undef s_aloh30

#undef K
}

void rs42(double **x,double **u_1,double **u_n,double *f_1,double *f_n,double *va,double *r,elem_t el,int dim,geom_t geom,double dt,double t)
/* Residu (r) */
{
#define ZP(n)       (u_1[(n)][I_zp])
#define ZC(n)       (u_1[(n)][I_zc])
#define ZA(n)       (u_1[(n)][I_za])


#define N_Cln(n)    (f_n[(n)])
#define N_Nan(n)    (f_n[(n+4)])
#define N_Kn(n)     (f_n[(n+6)])
#define N_Can(n)    (f_n[(n+8)])
#define N_Aln(n)    (f_n[(n+10)])
#define N_qn(n)     (f_n[(n+12)])

#define N_Cl1(n)    (f_1[(n)])
#define N_Na1(n)    (f_1[(n+4)])
#define N_K1(n)     (f_1[(n+6)])
#define N_Ca1(n)    (f_1[(n+8)])
#define N_Al1(n)    (f_1[(n+10)])
#define N_q1(n)     (f_1[(n+12)])

#define W_Cl        (f_1[14])
#define W_q        (f_1[15])
#define W_Na        (f_1[16])
#define W_K         (f_1[17])
#define W_Ca        (f_1[18])
#define W_Al        (f_1[19])

#define R(n,i)    (r[(n)*NEQ+(i)])

  double zp,kk_p ;
  double zc,kk_c ;
  double za,kk_a ;

  double dx ,xm ;
  double volume[2],surf ;
  int    i ;
  double zero = 0.,un = 1.,deux = 2. ;

  /* initialisation */
  for(i=0;i<el.nn*NEQ;i++) r[i] = zero ;

  if(el.dim < dim) return ;

  /*
    CALCUL DE volume ET DE surf
  */
  dx = x[1][0] - x[0][0] ;
  xm = (x[1][0] + x[0][0])/deux ;
  for(i=0;i<2;i++) {
    volume[i] = fabs(dx)/deux ; 
    if(geom == AXIS) volume[i] *= M_PI*(x[i][0] + xm) ; 
  }
  if(geom == AXIS) surf = deux*M_PI*xm ; else surf = un ;
  /*
    Conservation de Cl : (n_Cl1 - n_Cln) + dt * div(w_Cl) = 0
  */
  R(0,E_Cl) -= volume[0]*(N_Cl1(0) - N_Cln(0)) + dt*surf*(W_Cl) ;
  R(1,E_Cl) -= volume[1]*(N_Cl1(1) - N_Cln(1)) - dt*surf*(W_Cl) ;
  /*
    Conservation de la charge OH (hydroxy-hydro) : div(w_q) = 0
  */
  R(0,E_q) -= + dt*surf*(W_q) ;
  R(1,E_q) -= - dt*surf*(W_q) ;
  /*
    Conservation de Na : (n_Na1 - n_Nan) + dt * div(w_Na) = 0
  */
  R(0,E_Na) -= volume[0]*(N_Na1(0) - N_Nan(0)) + dt*surf*(W_Na) ;
  R(1,E_Na) -= volume[1]*(N_Na1(1) - N_Nan(1)) - dt*surf*(W_Na) ;
  /*
    Conservation de K : (n_K1 - n_Kn) + dt * div(w_K) = 0
  */
  R(0,E_K) -= volume[0]*(N_K1(0) - N_Kn(0)) + dt*surf*(W_K) ;
  R(1,E_K) -= volume[1]*(N_K1(1) - N_Kn(1)) - dt*surf*(W_K) ;
  /*
    Conservation de Ca : (n_Ca1 - n_Can) + dt * div(w_Ca) = 0
  */
  R(0,E_Ca) -= volume[0]*(N_Ca1(0) - N_Can(0)) + dt*surf*(W_Ca) ;
  R(1,E_Ca) -= volume[1]*(N_Ca1(1) - N_Can(1)) - dt*surf*(W_Ca) ;
  
  /*
    Conservation de Al : (n_Al1 - n_Aln) + dt * div(w_Al) = 0
  */
  R(0,E_Al) -= volume[0]*(N_Al1(0) - N_Aln(0)) + dt*surf*(W_Al) ;
  R(1,E_Al) -= volume[1]*(N_Al1(1) - N_Aln(1)) - dt*surf*(W_Al) ;
  
  /*
    Precipitation-Dissolution de AlOH3 : c_aloh4/c_oh <= K_a ; s_aloh3 >= 0
  */
  for(i=0;i<2;i++) {

    zc      = ZC(i) ;
    zp      = ZP(i) ;
    za      = ZA(i) ;

    kk_c    = k_c*(1. + 0.5*(zc - fabs(zc))) ;
    kk_p    = k_p*(1. + 0.5*(zp - fabs(zp))) ;
    kk_a    = k_a*(1. + 0.5*(za - fabs(za))) ;

    R(i,E_A) -= volume[i]*(kk_c*kk_c/(kk_p*kk_p*kk_p) - kk_a) ;
  }
  
  /*
    Electroneutralite : q = 0
  */
  R(0,E_el) -= volume[0]*N_q1(0) ;
  R(1,E_el) -= volume[1]*N_q1(1) ;

#undef ZP
#undef ZC
#undef ZA

#undef N_Cln
#undef N_Nan
#undef N_Kn
#undef N_Can
#undef N_Aln
#undef N_qn

#undef N_Cl1
#undef N_Na1
#undef N_K1
#undef N_Ca1
#undef N_Al1
#undef N_q1

#undef W_Cl
#undef W_q
#undef W_Na
#undef W_K
#undef W_Ca
#undef W_Al

#undef s_ch0
#undef s_c3a0
#undef s_sf0
#undef s_aloh30

#undef R
}

int so42(double **x,double **u,double *f,double *va,double *s,resu_t *r,elem_t el,int dim,geom_t geom,double t)
/* Les valeurs exploitees (s) */
{
#define C_Cl(n)     (u[(n)][I_c_cl])
#define ZP(n)       (u[(n)][I_zp])
#define ZC(n)       (u[(n)][I_zc])
#define ZF(n)	    (u[(n)][I_zf])
#define C_Na(n)     (u[(n)][I_c_na])
#define C_K(n)      (u[(n)][I_c_k])
#define ZA(n)       (u[(n)][I_za])
#define PSI(n)      (u[(n)][I_psi])

#define C_OH(n)     (f[(n+20)])
#define C_Ca(n)     (f[(n+22)])
#define C_AlOH4(n)  (f[(n+24)])

#define s_ch0       (va[(14)])
#define s_c3a0      (va[(15)])
#define s_sf0       (va[(16)])
#define s_aloh30    (va[(17)])

  int    i,j,nso ;
  double h_s[MAX_NOEUDS],dh_s[3*MAX_NOEUDS] ;
  double c_cl,s_cl,c_oh,c_na,c_k,c_ca,c_h,c_aloh4 ;
  double s_ch,s_c3a,s_sf,s_aloh3 ;
  double psi,q ;
  double zp,kk_p ;
  double zc,kk_c ;
  double zf,kk_f ;
  double za,kk_a ;
  double zero = 0. ;
  double phi;
  
  /*
    Donnees
  */
  phi0     = el.mat->pr[pm("porosite")] ;
  d_cl     = el.mat->pr[pm("D_Cl")] ;

  s_chrf   = el.mat->pr[pm("s_caoh2")] ;
  s_c3arf  = el.mat->pr[pm("s_c3ah6")] ;
  s_sfrf   = el.mat->pr[pm("s_sel")] ;
  s_aloh3rf= el.mat->pr[pm("s_aloh3")] ;

  s_csh    = el.mat->pr[pm("s_csh")] ;
  alpha    = el.mat->pr[pm("alpha")] ;
  beta     = el.mat->pr[pm("beta")] ;

  /* initialisation */
  nso = 15 ;

  for(i=0;i<nso;i++) for(j=0;j<9;j++) r[i].v[j] = zero ;

  /* fonctions d'interpolation en s */
  fint_abs(dim,el.nn,x,s,el.dim,h_s,dh_s) ;

  /* concentrations */
  c_cl    = param(u,h_s,el.nn,I_c_cl) ;
  c_na    = param(u,h_s,el.nn,I_c_na) ;
  c_k     = param(u,h_s,el.nn,I_c_k) ;	

  psi     = param(u,h_s,el.nn,I_psi) ;

  zc      = param(u,h_s,el.nn,I_zc) ;
  zp	  = param(u,h_s,el.nn,I_zp) ;
  zf	  = param(u,h_s,el.nn,I_zf) ;
  za	  = param(u,h_s,el.nn,I_za) ;

  /* les produits kk_i */
  /*
    kk_p = (c_ca)^1*(c_oh)^2
    kk_c = (c_ca)^3*(c_aloh4)^2*(c_oh)^4
    kk_f = (c_ca)^4*(c_aloh4)^2*(c_oh)^4*(c_cl)^2
    kk_a = (c_aloh4)^1*(c_oh)^-1
  */
  kk_c    = k_c*(1. + 0.5*(zc - fabs(zc))) ;
  kk_p    = k_p*(1. + 0.5*(zp - fabs(zp))) ;
  kk_f    = k_f*(1. + 0.5*(zf - fabs(zf))) ;
  kk_a    = k_a*(1. + 0.5*(za - fabs(za))) ;
  
  /* concentrations a partir des produits */
  c_ca     = pow(kk_f/kk_c,4.)/(c_cl*c_cl) ;
  c_oh     = kk_p*pow(kk_c/kk_f,2.)*c_cl ;
  c_aloh4  = pow(kk_c*kk_c/(kk_p*kk_f),2.)*c_cl ;
  c_h      = k_e/c_oh ;
  
  /* teneurs en solides */
  s_ch    = 0.5*(zp + fabs(zp))*s_chrf ;
  s_c3a   = 0.5*(zc + fabs(zc))*s_c3arf ;
  s_sf    = 0.5*(zf + fabs(zf))*s_sfrf ;
  s_aloh3 = 0.5*(za + fabs(za))*s_aloh3rf ;

  /* chloride bounded on CSH */
  s_cl    = alpha*s_csh*c_cl*beta/(c_oh + beta*c_cl) ;

  /* porosity */
  phi = phi0 + (s_ch0 - s_ch)*v_caoh2 + (s_sf0 - s_sf)*v_sel + (s_c3a0 - s_c3a)*v_c3ah6 + (s_aloh30 - s_aloh3)*v_aloh3 ;

  /* charge */
  q = z_cl*c_cl + z_oh*c_oh + z_h*c_h + z_aloh4*c_aloh4 + z_na*c_na + z_k*c_k + z_ca*c_ca ;

  /* quantites exploitees */
  strcpy(r[0].text,"c_Cl") ; r[0].n = 1 ;
  r[0].v[0] = c_cl ;
  strcpy(r[1].text,"s_Cl ") ; r[1].n = 1 ;
  r[1].v[0] = phi*c_cl + s_cl + 2*s_sf ;
  strcpy(r[2].text,"c_OH") ; r[2].n = 1 ;
  r[2].v[0] = c_oh ;
  strcpy(r[3].text,"c_Na") ; r[3].n = 1 ;
  r[3].v[0] = c_na ;
  strcpy(r[4].text,"c_K") ; r[4].n = 1 ;
  r[4].v[0] = c_k ;
  strcpy(r[5].text,"c_H") ; r[5].n = 1 ;
  r[5].v[0] = c_h ; 
  strcpy(r[6].text,"c_Ca") ; r[6].n = 1 ;
  r[6].v[0] = c_ca ; 
  strcpy(r[7].text,"c_AlOH4") ; r[7].n = 1 ;
  r[7].v[0] = c_aloh4 ; 
  strcpy(r[8].text,"s_caoh2") ; r[8].n = 1 ;
  r[8].v[0] = s_ch ; 
  strcpy(r[9].text,"s_c3a") ; r[9].n = 1 ;
  r[9].v[0] = s_c3a ; 
  strcpy(r[10].text,"s_sf") ; r[10].n = 1 ;
  r[10].v[0] = s_sf ; 
  strcpy(r[11].text,"s_aloh3") ; r[11].n = 1 ;
  r[11].v[0] = s_aloh3 ; 
  strcpy(r[12].text,"porosite") ; r[12].n = 1 ;
  r[12].v[0] = phi ; 
  strcpy(r[13].text,"charge") ; r[13].n = 1 ;
  r[13].v[0] = q ;
  strcpy(r[14].text,"potentiel") ; r[14].n = 1 ;
  r[14].v[0] = psi ;

  return(nso) ;

#undef C_Cl
#undef ZP
#undef ZC
#undef ZF
#undef C_Na
#undef C_K
#undef ZA
#undef PSI

#undef C_OH
#undef C_Ca
#undef C_AlOH4

#undef s_ch0
#undef s_c3a0
#undef s_sf0
#undef s_aloh30
}


double tortuosite(double phi)
{
  double phi0 = 0.18 ;
  double dphi = (phi > phi0) ? phi - phi0 : 0. ;
  return(0.001 + 0.07*phi*phi + 1.8*dphi*dphi) ;
}


#undef NEQ 

#undef E_Cl
#undef E_q
#undef E_Na
#undef E_K 
#undef E_Ca
#undef E_Al

#undef I_c_cl   
#undef I_zp   
#undef I_zc  	
#undef I_zf 	
#undef I_c_na   
#undef I_c_k
#undef I_za

/*valence*/
#undef z_cl   
#undef z_oh   
#undef z_na  	
#undef z_k    
#undef z_h    
#undef z_ca   
#undef z_aloh4

/*coefficient de diffusion dans l'eau dilue*/

#undef do_cl   
#undef do_oh   
#undef do_na   
#undef do_k    
#undef do_h    
#undef do_ca   
#undef do_aloh4

/* constant d'equilibre */
#undef k_e
#undef k_p 
#undef k_c 
#undef k_f 
#undef k_a 

/* volume molaire */
#undef v_caoh2
#undef v_c3ah6
#undef v_sel 
#undef v_aloh3 

#undef F
#undef RT

#undef s_oho  
