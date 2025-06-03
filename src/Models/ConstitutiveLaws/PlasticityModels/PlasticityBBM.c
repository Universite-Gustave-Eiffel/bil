#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "Message.h"
#include "Math_.h"
#include "Curves.h"
#include "Curve.h"
#include "Plasticity.h"
#include "autodiff.h"

template<typename T>
T* (PlasticityBBM_YF)(Plasticity_t*,const T*,const T*);

template<typename T>
T* (PlasticityBBM_FR)(Plasticity_t*,const T*,const T*);

static Plasticity_ComputeTangentStiffnessTensor_t    PlasticityBBM_CT ;
static Plasticity_ReturnMapping_t                    PlasticityBBM_RM ;
static Plasticity_YieldFunction_t                    PlasticityBBM_YF ;
static Plasticity_FlowRules_t                        PlasticityBBM_FR ;
#ifdef HAVE_AUTODIFF
static Plasticity_YieldFunctionDual_t                PlasticityBBM_YF ;
static Plasticity_FlowRulesDual_t                    PlasticityBBM_FR ;
#endif
static Plasticity_SetParameters_t                    PlasticityBBM_SP ;
extern Plasticity_SetModelProp_t                     PlasticityBBM_SetModelProp ;


#define Plasticity_GetSlopeSwellingLine(PL) \
        Plasticity_GetParameter(PL)[0]

#define Plasticity_GetSlopeVirginConsolidationLine(PL) \
        Plasticity_GetParameter(PL)[1]
        
#define Plasticity_GetSlopeCriticalStateLine(PL) \
        Plasticity_GetParameter(PL)[2]
        
#define Plasticity_GetInitialPreconsolidationPressure(PL) \
        Plasticity_GetParameter(PL)[3]
        
#define Plasticity_GetInitialVoidRatio(PL) \
        Plasticity_GetParameter(PL)[4]

#define Plasticity_GetSuctionCohesionCoefficient(PL) \
        Plasticity_GetParameter(PL)[5]

#define Plasticity_GetReferenceConsolidationPressure(PL) \
        Plasticity_GetParameter(PL)[6]
        
#define Plasticity_GetLoadingCollapseFactorCurve(PL) \
        Curves_GetCurve(Plasticity_GetCurves(PL))
        
        



void PlasticityBBM_SetModelProp(Plasticity_t* plasty)
{
  
  {
    Plasticity_GetComputeTangentStiffnessTensor(plasty) = PlasticityBBM_CT ;
    Plasticity_GetReturnMapping(plasty)                 = PlasticityBBM_RM ;
    Plasticity_GetYieldFunction(plasty)                 = PlasticityBBM_YF<double> ;
    Plasticity_GetFlowRules(plasty)                     = PlasticityBBM_FR<double> ;
    #ifdef HAVE_AUTODIFF
    Plasticity_GetYieldFunctionDual(plasty)             = PlasticityBBM_YF<real> ;
    Plasticity_GetFlowRulesDual(plasty)                 = PlasticityBBM_FR<real> ;
    #endif
    Plasticity_GetSetParameters(plasty)                 = PlasticityBBM_SP ;
    Plasticity_GetNbOfHardeningVariables(plasty)        = 2 ;
    
  }
  
}


void PlasticityBBM_SP(Plasticity_t* plasty,...)
{
  va_list args ;
  
  va_start(args,plasty) ;
  
  {
    Plasticity_GetSlopeSwellingLine(plasty)               = va_arg(args,double) ;
    Plasticity_GetSlopeVirginConsolidationLine(plasty)    = va_arg(args,double) ;
    Plasticity_GetSlopeCriticalStateLine(plasty)          = va_arg(args,double) ;
    Plasticity_GetInitialPreconsolidationPressure(plasty) = va_arg(args,double) ;
    Plasticity_GetInitialVoidRatio(plasty)                = va_arg(args,double) ;
    Plasticity_GetSuctionCohesionCoefficient(plasty)      = va_arg(args,double) ;
    Plasticity_GetReferenceConsolidationPressure(plasty)  = va_arg(args,double) ;
    Curve_t* lc                                           = va_arg(args,Curve_t*) ;
    int i = Curves_Append(Plasticity_GetCurves(plasty),lc) ;
    
    if(i != 0) {
      Message_RuntimeError("Plasticity_SetParameters: illegal curve") ;
    }
    //Plasticity_GetLoadingCollapseFactorCurve(plasty)[0] = lc[0] ;
    
    {
      double pc = Plasticity_GetInitialPreconsolidationPressure(plasty) ;
      
      //Plasticity_GetHardeningVariable(plasty)[0] = pc ;
      Plasticity_GetHardeningVariable(plasty)[0] = log(pc) ;
      
      Plasticity_GetTypicalSmallIncrementOfHardeningVariable(plasty)[0] = 1.e-6*log(pc) ;
      Plasticity_GetTypicalSmallIncrementOfHardeningVariable(plasty)[1] = 1.e-6*pc ;
      Plasticity_GetTypicalSmallIncrementOfStress(plasty) = 1.e-6*pc ;
    }
  }

  va_end(args) ;
}



double* (PlasticityBBM_CT)(Plasticity_t* plasty,const double* sig,const double* hardv,const double* plambda)
/** Barcelona Basic model criterion.
 * 
 *  Inputs are: 
 *  the slope of the swelling line (kappa),
 *  the slope of the virgin consolidation line (lambda),
 *  the shear modulus (mu),
 *  the slope of the critical state line (M),
 *  the suction cohesion coefficient (k),
 *  the reference consolidation pressure (p_r),
 *  the loading collapse factor curve (lc),
 *  the pre-consolidation pressure at suction=0 (pc0=exp(hardv[0])),
 *  the initial void ratio (e0),
 *  the suction (s=hardv[1]).
 * 
 *  On outputs the following values are modified:
 *  dfsds = derivative of the yield function wrt stresses
 *  dgsds = derivative of the potential function wrt stresses
 *  hm    = hardening modulus
 *  c     = tangent stiffness tensor
 * 
 *  Return the value of the yield function. 
 **/
{
  double* yield  = Plasticity_GetCriterionValue(plasty) ;
  double m       = Plasticity_GetSlopeCriticalStateLine(plasty) ;
  double kappa   = Plasticity_GetSlopeSwellingLine(plasty) ;
  double lambda  = Plasticity_GetSlopeVirginConsolidationLine(plasty) ;
  double e0      = Plasticity_GetInitialVoidRatio(plasty) ;
  double k       = Plasticity_GetSuctionCohesionCoefficient(plasty) ;
  double p_r     = Plasticity_GetReferenceConsolidationPressure(plasty) ;
  Curve_t* lc    = Plasticity_GetLoadingCollapseFactorCurve(plasty) ;
  double lnpc0   = hardv[0] ;
  double s       = hardv[1] ;
  double ps      = k*s ;
  double lc_s    = Curve_ComputeValue(lc,s) ;
  double lnp_r   = log(p_r) ;
  double lnpc    = lnp_r + lc_s * (lnpc0 - lnp_r) ;
  double pc      = exp(lnpc) ;
  double m2      = m*m ;
  double beta    = 1 ;
  double* dfsds  = Plasticity_GetYieldFunctionGradient(plasty) ;
  double* dgsds  = Plasticity_GetPotentialFunctionGradient(plasty) ;
  double* hm     = Plasticity_GetHardeningModulus(plasty) ;
  
  double id[9] = {1,0,0,0,1,0,0,0,1} ;
  double p,q,crit ;
  double dlambda = (plambda) ? plambda[0] : 0 ;
  
  /* 
     The yield criterion
  */
  p    = (sig[0] + sig[4] + sig[8])/3. ;
  q    = sqrt(3*Math_ComputeSecondDeviatoricStressInvariant(sig)) ;
  crit = q*q/m2 + (p - ps)*(p + pc) ;
  
  /*
    Gradients
    ---------
    dp/dsig_ij = 1/3 delta_ij
    dq/dsig_ij = 3/2 dev_ij/q 
    df/dsig_ij = 1/3 (df/dp) delta_ij + 3/2 (df/dq) dev_ij/q 
    df/dp      = 2*p + pc - ps
    df/dq      = 2*q/m2
    
    df/dsig_ij = 1/3 (2*p + pc - ps) delta_ij + (3/m2) dev_ij 
  */
  {
    int    i ;
    
    for(i = 0 ; i < 9 ; i++) {
      double dev = sig[i] - p*id[i] ;
    
      dfsds[i] = (2*p + pc - ps)*id[i]/3 + 3/m2*dev ;
      dgsds[i] = (2*p + pc - ps)*id[i]/3 + 3*beta/m2*dev ;
    }
  }
  
  /* The hardening modulus */
  /* H is defined by: df = (df/dsig_ij)*dsig_ij - dl*H 
   * But df = (df/dsig_ij)*dsig_ij + (df/dpc)*dpc
   * Hence: H = - (df/dpc) * dpc / dl = - (p - ps) * dpc / dl
   * On the other hand 
   * deps_p = dl*(dg/dp) = dl*(2*p + pc - ps)
   * d(ln(pc0)) = - (1 + e0)*v*deps_p = - (1 + e0)*v*dl*(2*p + pc - ps)
   * i.e. d(ln(pc0)) = dl*h with h = - (1 + e0)*v*(2*p + pc - ps)
   * and d(ln(pc)) = lc(s) * d(ln(pc0))
   * dpc = pc * lc(s) * d(ln(pc0))
   * 
   * Hence: H = (1 + e0)*v*(p - ps)*(2*p + pc - ps)*pc*lc(s)
   */
  {
    double v  = 1./(lambda - kappa) ;
    double v1 = (1 + e0)*v ;
    double h = - v1*(2*p + pc - ps) ;
    double dpcda    = pc*lc_s ;
    double dfda     = (p - ps)*dpcda ;
    
    hm[0] = - dfda * h ;
  }
  
  /*
   * Tangent matrix
   */
  {
    double* c = Plasticity_GetTangentStiffnessTensor(plasty) ;
     
    Plasticity_CopyElasticTensor(plasty,c) ;

    /* 
     * Using a = ln(pc0) as hardening variable.
     * So h(p,a) = - v1*(2*p + pc(a) - ps)
     */
    if(dlambda > 0) {
      double v        = 1./(lambda - kappa) ;
      double v1       = (1 + e0)*v ;
      double h        = - v1*(2*p + pc - ps) ;
      double dpcda    = pc*lc_s ;
      double dhda     = - v1*dpcda ;
      double dhdp     = - v1*2 ;
      double dfda     = (p - ps)*dpcda ;
      double ddgdpda  = dpcda ;
      double dlambda1 = dlambda / (1 - dlambda*dhda) ;
      Elasticity_t* elasty = Plasticity_GetElasticity(plasty) ;
      double bulk    = Elasticity_GetBulkModulus(elasty) ;
      double shear   = Elasticity_GetShearModulus(elasty) ;
      double g0 = shear ;
      double k0 = bulk  ;
      double g1 = g0 / (6*g0*dlambda*beta/m2 + 1) ;
      double k1 = k0 / (2*k0*dlambda1 + 1) ;
   
      {
        double mu      = g1 ;
        double lame    = k1 - 2./3.*mu ;
        int    i ;

        for(i = 0 ; i < 81 ; i++) c[i] = 0. ;
    
        for(i = 0 ; i < 3 ; i++) {
          int j ;
      
          #define C(i,j,k,l)  (c[(((i)*3+(j))*3+(k))*3+(l)])
          for(j = 0 ; j < 3 ; j++) {
            C(i,i,j,j) += lame ;
            C(i,j,i,j) += mu ;
            C(i,j,j,i) += mu ;
          }
          #undef C
        }
      }
      
      {
        int    i ;
    
        for(i = 0 ; i < 9 ; i++) {
          dfsds[i] += dlambda1*dfda*dhdp*id[i]/3 ;
          dgsds[i] += dlambda1*h*ddgdpda*id[i]/3 ;
        }
      }
      
      hm[0] /= (1 - dlambda*dhda) ;
    }
      
    {
      int i = Plasticity_UpdateElastoplasticTensor(plasty,c) ;
    }
  }
  
  yield[0] = crit ;
  
  return(yield) ;
}



double* (PlasticityBBM_RM)(Plasticity_t* plasty,double* sig,double* eps_p,double* hardv)
/** Barcelona Basic model criterion.
 * 
 *  Inputs are: 
 *  the slope of the swelling line (kappa),
 *  the slope of the virgin consolidation line (lambda),
 *  the shear modulus (mu),
 *  the slope of the critical state line (M),
 *  the suction cohesion coefficient (k),
 *  the reference consolidation pressure (p_r),
 *  the loading collapse factor curve (lc),
 *  the pre-consolidation pressure at suction=0 (pc0=exp(hardv[0])),
 *  the initial void ratio (e0),
 *  the suction (s=hardv[1]).
 * 
 *  On outputs, the following values are modified:
 *  the stresses (sig), 
 *  the plastic strains (eps_p), 
 *  the log(pre-consolidation pressure) at suction=0 (log(pc0)=hardv[0]).
 * 
 *  Return the value of the yield function. 
 **/
{
  double* yield  = Plasticity_GetCriterionValue(plasty) ;
  Elasticity_t* elasty = Plasticity_GetElasticity(plasty) ;
  double bulk    = Elasticity_GetBulkModulus(elasty) ;
  double mu      = Elasticity_GetShearModulus(elasty) ;
  double m       = Plasticity_GetSlopeCriticalStateLine(plasty) ;
  double kappa   = Plasticity_GetSlopeSwellingLine(plasty) ;
  double lambda  = Plasticity_GetSlopeVirginConsolidationLine(plasty) ;
  double e0      = Plasticity_GetInitialVoidRatio(plasty) ;
  double k       = Plasticity_GetSuctionCohesionCoefficient(plasty) ;
  double p_r     = Plasticity_GetReferenceConsolidationPressure(plasty) ;
  Curve_t* lc    = Plasticity_GetLoadingCollapseFactorCurve(plasty) ;
  double lnpc0   = hardv[0] ;
  double s       = (hardv[1] > 0) ? hardv[1] : 0 ;
  double ps      = k*s ;
  double lc_s    = Curve_ComputeValue(lc,s) ;
  double lnp_r   = log(p_r) ;
  double lnpc    = lnp_r + lc_s * (lnpc0 - lnp_r) ;
  double pc      = exp(lnpc) ;
  double m2      = m*m ;
  double v       = 1./(lambda - kappa) ;
  double beta    = 1 ;
  
  double id[9] = {1,0,0,0,1,0,0,0,1} ;
  double crit ;
  double p,q ;
  double p_t,q_t ;
  double dl ;
  
  /* 
     The criterion
  */
  p    = (sig[0] + sig[4] + sig[8])/3. ;
  q    = sqrt(3*Math_ComputeSecondDeviatoricStressInvariant(sig)) ;
  crit = q*q/m2 + (p - ps)*(p + pc) ;
  
  /*
     Closest point projection algorithm.
   * Only one iterative loop is used to solve
                    q*q/m2 + (p - ps)*(p + pc) = 0
     for p. The other variables (pc,q,dl) are expressed with p.
   */
  dl    = 0. ;
  p_t   = p ;
  q_t   = q ;
  
  if(crit > 0.) {
    double lnpc0_n = lnpc0 ;
    double pc_n  = pc ;
    double fcrit = crit ;
    int nf    = 0 ;
    double tol = 1.e-8 ;
    double v1     = (1+e0)*v ;
    double klub   = 1/bulk ;
    
    while(fabs(fcrit) > tol*pc_n*pc_n) {
      /*
       * Flow rule
       * ---------
       * g(p,q,a)   = q*q*beta/m2 + (p - ps)*(p + pc(a)) = 0
       * dg/dp      = 2*p + pc - ps
       * dg/dq      = (2*beta/m2) q
       * dp/dsig_ij = 1/3 delta_ij
       * dq/dsig_ij = 3/2 dev_ij/q
       * dg/dsig_ij = 1/3 (2*p + pc - ps) delta_ij + (3*beta/m2) dev_ij 
       * deps_p     = dl (2*p + pc - ps)
       * deij_p     = dl (3*beta/m2) dev_ij
       */
      double dfsdp     = 2*p + pc - ps ;
      double dfsdq     = 2*q/m2 ;
      double dfsdpc    = p - ps ;
      double dlnpc0sdp = v1*klub ;
      double dlnpcsdp  = lc_s * dlnpc0sdp ;
      double dpcsdp    = pc * dlnpcsdp ;
      double dgsdp     = 2*p + pc - ps ;
      double d2gsdp2   = 2 + dpcsdp ;
      double ddlsdp    = (-klub - dl*d2gsdp2)/dgsdp ;
      double dqsdp     = -6*mu*beta*ddlsdp*q/(m2 + 6*mu*beta*dl) ;
      double df        = dfsdp + dfsdq*dqsdp + dfsdpc*dpcsdp ;
      
      p     -= fcrit/df ;
      
      /* Variables (pc,dl,q) are explicit in p */
      /* Plastic multiplier (dl):
       * ------------------------
       * deps_e = (p - p_n) / bulk ;
       * deps   = (p_t - p_n) / bulk ;
       * deps_p = (p_t - p) / bulk = dl (2*p + pc - ps)
       * Hence 
       * dl = (p_t - p) / (bulk*(2*p + pc - ps))
       * Pre-consolidation pressures (pc0 and pc):
       * ----------------------------------------
       * deps_p = - (lambda - kappa)/(1+e0) * ln(pc0/pc0_n)
       * Hence using the above relation of deps_p
       * ln(pc0/pc0_n) = (p - p_t) * (1+e0) / (bulk*(lambda-kappa))
       * The loading collapse curve
       * ln(pc/p_r) = lc * ln(pc0/p_r)
       * Deviatoric behavior (q):
       * ------------------------
       * dev_ij = dev_ij_t - 2 mu deij_p 
       *        = dev_ij_t - 6 mu beta / m2 dl dev_ij
       * Hence 
       * dev_ij = dev_ij_t / (1 + 6 mu beta / m2 dl)
       * q      = q_t / (1 + 6 mu beta / m2 dl)
       */

      lnpc0  = lnpc0_n + (p-p_t)*v1*klub ;
      lnpc   = lnp_r + lc_s * (lnpc0 - lnp_r) ;
      pc     = exp(lnpc) ;
      dl     = (p_t - p)*klub/(2*p + pc - ps) ;
      q      = q_t*m2/(m2 + 6*mu*beta*dl) ;
      fcrit  = q*q/m2 + (p - ps)*(p + pc) ;
      
      if(nf++ > 20) {
        Message_FatalError("PlasticityBBM_RM: no convergence") ;
      }
    }
  }
  
  /*
    Stresses and plastic strains
  */
  
  {
    double a = m2/(m2 + 6*mu*beta*dl) ;
    int    i ;
    
    for(i = 0 ; i < 9 ; i++) {
      double dev      = a*(sig[i] - p_t*id[i]) ;
      double dgsds    = (2*p + pc - ps)*id[i]/3 + 3*beta/m2*dev ;
    
      sig[i]    = p*id[i] + dev ;
      eps_p[i] += dl*dgsds ;
    }
  }
  
  /* Consolidation pressure at suction=0 */
  {
    hardv[0] = lnpc0 ;
  }
  
  /* Plastic muliplier */
  Plasticity_GetPlasticMultiplier(plasty)[0] = dl ;
  
  yield[0] = crit ;
  return(yield) ;
}



template<typename T>
T* (PlasticityBBM_YF)(Plasticity_t* plasty,const T* stress,const T* hardv)
/** Return the value of the yield function. 
 **/
{
  size_t SizeNeeded = sizeof(T) ;
  T* yield     = (T*) Plasticity_AllocateInBuffer(plasty,SizeNeeded) ;
  double m     = Plasticity_GetSlopeCriticalStateLine(plasty) ;
  double k     = Plasticity_GetSuctionCohesionCoefficient(plasty) ;
  double p_r   = Plasticity_GetReferenceConsolidationPressure(plasty) ;
  Curve_t* lc  = Plasticity_GetLoadingCollapseFactorCurve(plasty) ;
  T lnpc0      = hardv[0] ;
  T s          = hardv[1] ;
  T ps         = k*s ;
  T lc_s       = Curve_ComputeValue(lc,s) ;
  double lnp_r = log(p_r) ;
  T lnpc       = lnp_r + lc_s * (lnpc0 - lnp_r) ;
  T pc         = exp(lnpc) ;
  double m2    = m*m ;
  T p          = (stress[0] + stress[4] + stress[8])/3. ;
  T q2         = 3*Math_ComputeSecondDeviatoricStressInvariant(stress) ;
  
  yield[0] = q2/m2 + (p - ps)*(p + pc) ;

  return(yield) ;
}



template<typename T>
T* (PlasticityBBM_FR)(Plasticity_t* plasty,const T* stress,const T* hardv)
/** Barcelona Basic model criterion.
 * 
 *  Inputs are: 
 *  the slope of the swelling line (kappa),
 *  the slope of the virgin consolidation line (lambda),
 *  the slope of the critical state line (M),
 *  the suction cohesion coefficient (k),
 *  the reference consolidation pressure (p_r),
 *  the loading collapse factor curve (lc),
 *  the pre-consolidation pressure at suction=0 (pc0=exp(hardv[0])),
 *  the initial void ratio (e0).
 *  the suction (s=hardv[1]),
 * 
 *  Return the direction of the plastic flows based on the flow rules:
 *    - the plastic strain rate (i.e. the potential gradient)
 *    - the rate of log(pre-consolidation pressure) (1/dlambda * d(ln(pc0))/dt)
 **/
{
  size_t SizeNeeded = (9+2)*(sizeof(T)) ;
  T* flow        = (T*) Plasticity_AllocateInBuffer(plasty,SizeNeeded) ;
  double m       = Plasticity_GetSlopeCriticalStateLine(plasty) ;
  double kappa   = Plasticity_GetSlopeSwellingLine(plasty) ;
  double lambda  = Plasticity_GetSlopeVirginConsolidationLine(plasty) ;
  double e0      = Plasticity_GetInitialVoidRatio(plasty) ;
  double k       = Plasticity_GetSuctionCohesionCoefficient(plasty) ;
  double p_r     = Plasticity_GetReferenceConsolidationPressure(plasty) ;
  Curve_t* lc    = Plasticity_GetLoadingCollapseFactorCurve(plasty) ;
  T lnpc0        = hardv[0] ;
  T s            = hardv[1] ;
  T ps           = k*s ;
  T lc_s         = Curve_ComputeValue(lc,s) ;
  double lnp_r   = log(p_r) ;
  T lnpc         = lnp_r + lc_s * (lnpc0 - lnp_r) ;
  T pc           = exp(lnpc) ;
  double m2      = m*m ;
  double beta    = 1 ;
  
  double id[9] = {1,0,0,0,1,0,0,0,1} ;
  T p          = (stress[0] + stress[4] + stress[8])/3. ;
  //double q    = sqrt(3*Math_ComputeSecondDeviatoricStressInvariant(stress)) ;
  
  /*
    Potential function: g = beta*q*q/m2 + (p - ps)*(p + pc)
  */
  
  /*
    Plastic strain: deps^p_ij = dl * dg/dstress_ij
    ----------------------------------------------
    dp/dstress_ij = 1/3 delta_ij
    dq/dstress_ij = 3/2 dev_ij/q 
    dg/dstress_ij = 1/3 (dg/dp) delta_ij + 3/2 (dg/dq) dev_ij/q 
    dg/dp      = 2*p + pc - ps
    dg/dq      = beta*2*q/m2
    
    dg/dstress_ij = 1/3 (2*p + pc - ps) delta_ij + beta*(3/m2) dev_ij 
  */
  {
    Elasticity_t* elasty = Plasticity_GetElasticity(plasty) ;
    double bulk    = Elasticity_GetBulkModulus(elasty) ;
    double pc0 = Plasticity_GetInitialPreconsolidationPressure(plasty) ;
    double N  = 4/(pc0*pc0*bulk) ;
    int    i ;
    
    for(i = 0 ; i < 9 ; i++) {
      T dev = stress[i] - p*id[i] ;

      flow[i] = N*((2*p + pc - ps)*id[i]/3 + 3*beta/m2*dev) ;
    }
  }
  
  /*
    The hardening flow: d(ln(pc0)) = - (1 + e0)*v*deps_p
   * --------------------------------------------------
   * Using a = ln(pc0) as hardening variable.
   * d(a) = - dl * (1 + e0) * v * (dg/dp)
   * So h(p,a) = - (1 + e0) * v * (2*p + pc(a) - ps)
   */
  {
    double v = 1./(lambda - kappa) ;
    T h      = flow[0] + flow[4] + flow[8] ;

    flow[9]  = - (1 + e0)*v*h ;
    flow[10] = 0 ;
  }
  
  return(flow) ;
}



#if 0
double* (PlasticityBBM_YF)(Plasticity_t* plasty,const double* stress,const double* hardv)
/** Return the value of the yield function. 
 **/
{
  size_t SizeNeeded = sizeof(double) ;
  double* yield   = (double*) Plasticity_AllocateInBuffer(plasty,SizeNeeded) ;
  double m     = Plasticity_GetSlopeCriticalStateLine(plasty) ;
  double k     = Plasticity_GetSuctionCohesionCoefficient(plasty) ;
  double p_r   = Plasticity_GetReferenceConsolidationPressure(plasty) ;
  Curve_t* lc  = Plasticity_GetLoadingCollapseFactorCurve(plasty) ;
  double lnpc0 = hardv[0] ;
  double s     = hardv[1] ;
  double ps    = k*s ;
  double lc_s  = Curve_ComputeValue(lc,s) ;
  double lnp_r = log(p_r) ;
  double lnpc  = lnp_r + lc_s * (lnpc0 - lnp_r) ;
  double pc    = exp(lnpc) ;
  double m2    = m*m ;
  double p     = (stress[0] + stress[4] + stress[8])/3. ;
  double q2    = 3*Math_ComputeSecondDeviatoricStressInvariant(stress) ;
  
  yield[0] = q2/m2 + (p - ps)*(p + pc) ;

  return(yield) ;
}



double* (PlasticityBBM_FR)(Plasticity_t* plasty,const double* stress,const double* hardv)
/** Barcelona Basic model criterion.
 * 
 *  Inputs are: 
 *  the slope of the swelling line (kappa),
 *  the slope of the virgin consolidation line (lambda),
 *  the slope of the critical state line (M),
 *  the suction cohesion coefficient (k),
 *  the reference consolidation pressure (p_r),
 *  the loading collapse factor curve (lc),
 *  the pre-consolidation pressure at suction=0 (pc0=exp(hardv[0])),
 *  the initial void ratio (e0).
 *  the suction (s=hardv[1]),
 * 
 *  Return the direction of the plastic flows based on the flow rules:
 *    - the plastic strain rate (i.e. the potential gradient)
 *    - the rate of log(pre-consolidation pressure) (1/dlambda * d(ln(pc0))/dt)
 **/
{
  size_t SizeNeeded = (9+2)*(sizeof(double)) ;
  double* flow   = (double*) Plasticity_AllocateInBuffer(plasty,SizeNeeded) ;
  double m       = Plasticity_GetSlopeCriticalStateLine(plasty) ;
  double kappa   = Plasticity_GetSlopeSwellingLine(plasty) ;
  double lambda  = Plasticity_GetSlopeVirginConsolidationLine(plasty) ;
  double e0      = Plasticity_GetInitialVoidRatio(plasty) ;
  double k       = Plasticity_GetSuctionCohesionCoefficient(plasty) ;
  double p_r     = Plasticity_GetReferenceConsolidationPressure(plasty) ;
  Curve_t* lc    = Plasticity_GetLoadingCollapseFactorCurve(plasty) ;
  double lnpc0   = hardv[0] ;
  double s       = hardv[1] ;
  double ps      = k*s ;
  double lc_s    = Curve_ComputeValue(lc,s) ;
  double lnp_r   = log(p_r) ;
  double lnpc    = lnp_r + lc_s * (lnpc0 - lnp_r) ;
  double pc      = exp(lnpc) ;
  double m2      = m*m ;
  double beta    = 1 ;
  
  double id[9] = {1,0,0,0,1,0,0,0,1} ;
  double p     = (stress[0] + stress[4] + stress[8])/3. ;
  //double q    = sqrt(3*Math_ComputeSecondDeviatoricStressInvariant(stress)) ;
  
  /*
    Potential function: g = beta*q*q/m2 + (p - ps)*(p + pc)
  */
  
  /*
    Plastic strain: deps^p_ij = dl * dg/dstress_ij
    ----------------------------------------------
    dp/dstress_ij = 1/3 delta_ij
    dq/dstress_ij = 3/2 dev_ij/q 
    dg/dstress_ij = 1/3 (dg/dp) delta_ij + 3/2 (dg/dq) dev_ij/q 
    dg/dp      = 2*p + pc - ps
    dg/dq      = beta*2*q/m2
    
    dg/dstress_ij = 1/3 (2*p + pc - ps) delta_ij + beta*(3/m2) dev_ij 
  */
  {
    Elasticity_t* elasty = Plasticity_GetElasticity(plasty) ;
    double bulk    = Elasticity_GetBulkModulus(elasty) ;
    double pc0 = Plasticity_GetInitialPreconsolidationPressure(plasty) ;
    double N  = 4/(pc0*pc0*bulk) ;
    int    i ;
    
    for(i = 0 ; i < 9 ; i++) {
      double dev = stress[i] - p*id[i] ;

      flow[i] = N*((2*p + pc - ps)*id[i]/3 + 3*beta/m2*dev) ;
    }
  }
  
  /*
    The hardening flow: d(ln(pc0)) = - (1 + e0)*v*deps_p
   * --------------------------------------------------
   * Using a = ln(pc0) as hardening variable.
   * d(a) = - dl * (1 + e0) * v * (dg/dp)
   * So h(p,a) = - (1 + e0) * v * (2*p + pc(a) - ps)
   */
  {
    double v = 1./(lambda - kappa) ;
    double h = flow[0] + flow[4] + flow[8] ;

    flow[9]  = - (1 + e0)*v*h ;
    flow[10] = 0 ;
  }
  
  return(flow) ;
}
#endif



#undef Plasticity_GetSlopeSwellingLine
#undef Plasticity_GetSlopeVirginConsolidationLine
#undef Plasticity_GetSlopeCriticalStateLine
#undef Plasticity_GetInitialPreconsolidationPressure
#undef Plasticity_GetInitialVoidRatio
#undef Plasticity_GetSuctionCohesionCoefficient
#undef Plasticity_GetReferenceConsolidationPressure
#undef Plasticity_GetLoadingCollapseFactorCurve
