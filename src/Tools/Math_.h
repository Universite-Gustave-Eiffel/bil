#ifndef MATH_H
#define MATH_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Math_t; //typedef struct Math_t   Math_t;
struct Buffers_t;


/* Constants */
#define Math_Pi           (3.14159265358979323846)
#define Math_Ln10         (2.3025850929940456840179914546843642076011)



#define Math_SolveByGaussEliminationWithoutPartialPivoting(...) \
        Math_SolveByGaussEliminationKIJ(__VA_ARGS__,NULL)

#define Math_SolveByGaussElimination(...) \
        Utils_CAT_NARG(Math_SolveByGaussElimination,__VA_ARGS__)(__VA_ARGS__)

/* Implementation */
#define Math_SolveByGaussElimination3 \
        Math_SolveByGaussEliminationWithPartialPivoting
        
#define Math_SolveByGaussElimination4 \
        Math_SolveByGaussEliminationKIJ


#include "BilConfig.h"

#ifdef HAVE_LAPACK
#if defined(__cplusplus)
  extern "C" {
#endif

extern int dgeev_(const char*,const char*,int*,double*,int*,double*, double*,double*,int*,double*,int*,double*,int*,int*);

#if defined(__cplusplus)
  }
#endif
#endif


#define Math_ComputeSecondDeviatoricStrainInvariant \
        Math_ComputeSecondDeviatoricStressInvariant
        
#define Math_ComputeDeviatoricStrain \
        Math_ComputeDeviatoricStress
        
#define Math_ComputeMatrixInverse(a,n) \
        Math_SolveByGaussJordanElimination(a,NULL,n,0)
        
#define Math_InvertMatrix(A,N)  \
        Math_SolveByGaussJordanElimination(A,NULL,N,0)

#define Math_Max(a,b)          (((a) > (b)) ? (a) : (b))
#define Math_Min(a,b)          (((a) < (b)) ? (a) : (b))

#define Math_Swap(a,b,Type_t) \
        do { \
          Type_t tmp = (a) ; \
          (a) = (b) ; \
          (b) = tmp ; \
        } while(0)
        
#define Math_SwapDouble(a,b)   Math_Swap(a,b,double)
#define Math_SwapInt(a,b)      Math_Swap(a,b,int)

#define Math_Sign(a)           ((a < 0) ? -1 : (a > 0))


#ifndef M_PI
#define M_PI       Math_Pi
#endif

#ifndef MAX
#define MAX        Math_Max
#endif

#ifndef MIN
#define MIN        Math_Min
#endif



#define Math_MaxNbOfMatrices                      (4)
#define Math_MaxSizeOfMatrix                      (9*sizeof(double))
#define Math_SizeOfBuffer                         (Math_MaxNbOfMatrices*Math_MaxSizeOfMatrix)


#define Math_GetOutputs(M)             ((M)->outputs)
#define Math_GetBuffers(M)             ((M)->buffers)
#define Math_GetDelete(M)              ((M)->Delete)



#define Math_GetBuffer(M) \
        Buffers_GetBufferOfCurrentThread(Math_GetBuffers(M))

/* Operations on buffer */
#define Math_AllocateInBuffer(M,sz) \
        (Buffer_Allocate(Math_GetBuffer(M),(sz)))
        
#define Math_FreeBuffer(M) \
        (Buffer_Free(Math_GetBuffer(M)))
        
#define Math_FreeBufferFrom(M,p) \
        (Buffer_FreeFrom(Math_GetBuffer(M),(char*) (p)))



#include "GenericObject.h"

struct Math_t {          /* some math methods */
  void* outputs ;
  Buffers_t*  buffers ;         /* Buffer */
  GenericObject_Delete_t* Delete ;
} ;



/* Old notations that I try to eliminate little by little */
#define j2       Math_ComputeSecondDeviatoricStressInvariant
//#define gausse   Math_SolveByGaussElimination


inline Math_t*  (Math_GetInstance)(void) ;
inline Math_t*  (Math_Create)(void) ;
inline int      (Math_ComputePolynomialEquationRootsOfDegree4)(double*) ;
inline int      (Math_ComputePolynomialEquationRootsOfDegree3)(double*) ;
inline int      (Math_ComputePolynomialEquationRootsOfDegree2)(double*) ;
inline void     (Math_Delete)(void*) ;
inline double   (Math_ComputeFirstStressInvariant)(const double*) ;
inline double   (Math_ComputeSecondStressInvariant)(const double*) ;
inline double   (Math_ComputeSecondDeviatoricStressInvariant)(double const*);
inline double*  (Math_SolveByGaussEliminationWithPartialPivoting)(double*,double*,int) ;
inline double*  (Math_SolveByGaussJordanElimination)(double*,double*,int,int) ;
inline int      (Math_ComputePolynomialEquationRoots)(double*,int const) ;
inline int      (Math_PolishPolynomialEquationRoot)(double*,int,double*,double,int) ;
inline double   (Math_Compute3x3MatrixDeterminant)(const double*) ;
inline double*  (Math_Inverse3x3Matrix)(const double*) ;
inline double*  (Math_ComputePrincipalStresses)(const double*) ;
inline double*  (Math_ComputeRealEigenvaluesAndEigenvectorsOf3x3Matrix)(double*,const char) ;
inline void     (Math_PrintStiffnessTensor)(const double*) ;
inline void     (Math_PrintStressTensor)(const double*) ;
inline void     (Math_PrintMatrix)(const double*,const int) ;
inline void     (Math_PrintVector)(const double*,const int) ;
inline double*  (Math_ComputeDeviatoricStress)(const double*) ;
inline double*  (Math_SolveByGaussEliminationKIJ)(double*,double*,int,int*) ;
inline double*  (Math_SolveByGaussEliminationJIK)(double*,double*,int,int*) ;

template<typename T>
inline T        (Math_ComputeSecondDeviatoricStressInvariant)(T const*);
template<typename T>
inline int      (Math_ComputePolynomialEquationRoots)(T*,int const) ;


/* For the macros */
#include "Utils.h"
#include "Buffers.h"
#include "Buffer.h"

/* For the inline functions below*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include "Buffers.h"
#include "Buffer.h"
#include "Session.h"
#include "GenericData.h"
#include "Message.h"
#include "Exception.h"
#include "Mry.h"
#include "autodiff.h"

//#include "Math_.h.in"



template<typename T>
inline T (Math_ComputeSecondDeviatoricStressInvariant)(T const* sig)
/** Second invariant of the deviatoric part of a stress tensor:
    J2 = 1/2 tr(dev.dev)  (dev = sig - 1/3 tr(sig) Id) */
{
#define SIG(i,j) (sig[3*(i)+(j)])
  T j2a = (SIG(0,0) - SIG(1,1))*(SIG(0,0) - SIG(1,1))
             + (SIG(1,1) - SIG(2,2))*(SIG(1,1) - SIG(2,2))
             + (SIG(2,2) - SIG(0,0))*(SIG(2,2) - SIG(0,0)) ;
  T j2b = SIG(0,1)*SIG(1,0) + SIG(1,2)*SIG(2,1) + SIG(2,0)*SIG(0,2) ;
  return(j2a/6. + j2b) ;
#undef SIG
}





/*
   Polynomial roots
*/
template<typename T>
inline int (Math_ComputePolynomialEquationRootsOfDegree2)(T* x)
{
  T b = x[1]/x[0] ;
  T c = x[2]/x[0] ;
  T delta = b*b - 4*c ;
  
  if(c == 0) {
    if(b == 0) {
      x[0] = 0 ;
      return(1) ;
    } else {
      x[0] = MAX(-b,0) ;
      x[1] = MIN(-b,0) ;
      return(2) ;
    }
  } else if(delta == 0) {
    T temp1 = - b*0.5 ;
    x[0] = temp1 ;
    return(1) ;
  } else if(delta > 0) {
    T temp1 = - b*0.5;
    T temp2 = sqrt(delta)*0.5 ;
    if(b < 0) {
      x[0] = temp1 + temp2 ;
      x[1] = c/x[0] ;
    } else {
      x[1] = temp1 - temp2 ;
      x[0] = c/x[1] ;
    }
    return(2) ;
  }
  
  return(0) ;
}



template<typename T>
inline int (Math_ComputePolynomialEquationRootsOfDegree3)(T* x)
{
  T b = x[1]/x[0] ;
  T c = x[2]/x[0] ;
  T d = x[3]/x[0] ;
  /* Depressed cubic equation : x3 + px + q = 0 */
  T p  = c - b*b/3 ;
  T q  = d + 2*b*b*b/27 - b*c/3 ;
  T p3 = p*p*p ;
  T q2 = q*q ;
  T r2 = p3/27 + q2/4 ;
  int n = 0 ;
  
  if(q == 0) {
    if(p >= 0) {
      x[0] = 0 ;
      n = 1 ;
    } else {
      T r = sqrt(-p) ;
      x[0] = r ;
      x[1] = 0 ;
      x[2] = - r ;
      n = 3 ;
    }
    
  } else if(r2 == 0) { /* q != 0 and p3/27 = -q2/4 (< 0) */
    T x1 = 3*q/p ;
    T x2 = - 0.5*x1 ; /* double root */
    x[0] = MAX(x1,x2) ;
    x[1] = MIN(x1,x2) ;
    n = 2 ;
    
  } else if(r2 > 0) {
    T r  = sqrt(r2) ;
    T r1 = (q < 0) ? r : -r ;
    T u3 = - 0.5*q + r1 ; /* so u3 is != 0 even for p = 0 */
    T u  = (u3 > 0) ? pow(u3,1/3.) : - pow(-u3,1/3.) ;
    x[0] = u - p/(3*u) ;
    n = 1 ;
    
  /* Trigonometric solution */
  } else { /* q != 0 and p3/27 < -q2/4 (< 0) */
    T srp3 = sqrt(-p/3) ;
    T k    = acos(1.5*q/(p*srp3))/3 ; /* 0 < k < pi/3 */
    T ck   = srp3*cos(k) ;
    T sk   = srp3*sin(k) ;
    T sr3  = sqrt(3.) ;
    T x1   = 2*ck ;
    T x2   = - ck + sr3*sk ;
    T x3   = - ck - sr3*sk ;
    x[0] = x1 ;
    x[1] = x2 ;
    x[2] = x3 ;
    n = 3 ;
  }
  
  {
    int i ;
    for(i = 0 ; i < n ; i++) x[i] -= b/3 ;
  }
  
  return(n) ;
}



template<typename T>
inline int (Math_ComputePolynomialEquationRootsOfDegree4)(T* x)
{
  T b = x[1]/x[0] ;
  T c = x[2]/x[0] ;
  T d = x[3]/x[0] ;
  T e = x[4]/x[0] ;
  /* Depressed quartic equation: x4 + px2 + qx + r = 0 */
  T b2 = b*b ;
  T b3 = b2*b ;
  T b4 = b2*b2 ;
  T p  = - 0.375*b2 + c ;
  T q  =   0.125*b3 - 0.5*b*c + d ;
  T r  = - 0.01171875*b4 + 0.0625*b2*c - 0.25*b*d + e ;
  int n = 0 ;
  
  if(e == 0) {
    int i ;
    n = Math_ComputePolynomialEquationRootsOfDegree3(x) ;
    x[n] = 0 ;
    for(i = n ; i > 0 ; i--) {
      if(x[i - 1] >= 0) break ;
      x[i] = x[i - 1] ;
      x[i - 1] = 0 ;
    }
    return(n + 1) ;
    
  /* Biquadractic equations */
  } else if(q == 0) { 
    T y[3] = {1,0,0} ;
    int i,m ;
    y[1] = p ;
    y[2] = r ;
    m = Math_ComputePolynomialEquationRootsOfDegree2(y) ;
    /* First keep only positive roots */
    for(i = 0 ; i < m ; i++) {
      if(y[i] >= 0) {
        x[i] = sqrt(y[i]) ;
        n++ ;
      }
    }
    /* Second put the negative roots behind */
    for(i = 0 ; i < n ; i++) {
        x[2*n - 1 - i] =  - x[i] ;
    }
    n *= 2 ;
    
  /* Ferrari's solution */
  } else { 
    /* The nested depressed cubic equation: x3 + p1x + q1 */
    T p1 = - p*p/12 - r ;
    T q1 = - p*p*p/108 + p*r/3 - 0.125*q*q ;
    T y[4] = {1,0,0,0} ;
    y[2] = p1 ;
    y[3] = q1 ;
    /* Solving for the nested depressed cubic equation */
    Math_ComputePolynomialEquationRootsOfDegree3(y) ;
    {
      T s = y[0] - 5./6*p ;
      T w2 = p + 2*s ;
      if(w2 <= 0) { /* Theoretically it's never met */
        arret("Math_ComputePolynomialEquationRootsOfDegree4: never met?") ;
        return(0) ;
      } else {
        T w = sqrt(w2) ;
        T h1 = - (3*p + 2*s + 2*q/w) ;
        T h2 = - (3*p + 2*s - 2*q/w) ;
        if(h1 == 0) {
          x[0] = 0.5*w ;
          n = 1 ;
        } else if(h1 > 0) {
          x[0] = 0.5*(w + sqrt(h1)) ;
          x[1] = 0.5*(w - sqrt(h1)) ;
          n = 2 ;
        }
        if(h2 == 0) {
          x[n++] = - 0.5*w ;
        } else if(h2 > 0) {
          x[n++] = 0.5*(- w + sqrt(h2)) ;
          x[n++] = 0.5*(- w - sqrt(h2)) ;
        }
      }
    }
  }
  
  {
    int i ;
    for(i = 0 ; i < n ; i++) x[i] -= 0.25*b ;
  }
  
  return(n) ;
}




template<typename T>
inline int (Math_ComputePolynomialEquationRoots)(T* x,int const n)
/** Real Roots of Polynomial Equation of Degree n
 *  Return the nb of real solutions, sorted 
 *  from max to min */
{
#define MAX_DEGREE  (4)
  //double y[MAX_DEGREE + 1] ;
  int m = 0 ;
  
  if(x[0] != 0) {
    if(n == 0) {
      arret("Math_ComputePolynomialEquationRoots: degree 0") ;
    } else if(n == 1) {
      x[0] = - x[1]/x[0] ;
      return(1) ;
    } else if(n == 2) {
      m = Math_ComputePolynomialEquationRootsOfDegree2(x) ;
      return(m) ;
    } else if(n == 3) {
      m = Math_ComputePolynomialEquationRootsOfDegree3(x) ;
    } else if(n == 4) {
      m = Math_ComputePolynomialEquationRootsOfDegree4(x) ;
    } else {
      arret("Math_ComputePolynomialEquationRoots: degree too big") ;
    }
  } else {
    int i ;
    
    m = Math_ComputePolynomialEquationRoots(x + 1,n - 1) ;
    for(i = 0 ; i < m ; i++) x[i] = x[i + 1] ;
    return(m) ;
  }
  
  return(m) ;
#undef MAX_DEGREE
}




template<typename T>
inline int (Math_PolishPolynomialEquationRoot)(T* x,int n,T* proot,T tol,int iterations)
{
  T root = proot[0] ;
  
  for(int it = 0 ; it < iterations ; it++) {
    T error = x[0] ;
    T derivative = 0 ;
    T droot ;
      
    for(int i = 0 ; i < n ; i++) {
      T a = error ;
      T b = x[i + 1] ;
      
      error = a*root + b ;
    }
      
    for(int i = 0 ; i < n ; i++) {
      T a = derivative ;
      T b = (n - i)*x[i] ;
      
      derivative = a*root + b ;
    }
      
    if(derivative == 0) {
      proot[0] = root ;
      return(0) ;
    }
      
    droot = - error / derivative ;
    root += droot ;
      
    if(fabs(droot) < tol) {
      proot[0] = root ;
      return(0) ;
    }
  }
  
  /* Raise an interrupt signal instead of exit */
  Message_Warning("Math_PolishPolynomialEquationRoot: no convergence") ;
  {    
    Message_Direct("\nthe %dth order polynomial equation:\n",n) ;
    
    for(int i = 0 ; i <= n ; i++) {
      Message_Direct("%d order coefficient: %lf\n",n-i,x[i]) ;
    }
    
    Message_Direct("\nhas not converged for the root %lf\n",root) ;
  }
  //Exception_Interrupt ;
  /*
  arret("Math_PolishPolynomialEquationRoot: not converged ") ;
  */
  return(-1) ;
}


#define _INLINE_ inline

_INLINE_ Math_t*  (Math_Create)(void)
{
  Math_t* math = (Math_t*) Mry_New(Math_t) ;
  
  /* Space allocation for buffer */
  {
    Buffers_t* buf = Buffers_Create(Math_SizeOfBuffer) ;
    
    Math_GetBuffers(math) = buf ;
  }
  
  
  Math_GetDelete(math) = &Math_Delete ;
  
  return(math) ;
}



_INLINE_ Math_t* (Math_GetInstance)(void)
{
  GenericData_t* gdat = Session_FindGenericData(Math_t,"Math") ;
  
  if(!gdat) {
    Math_t* math = Math_Create() ;
    
    gdat = GenericData_Create(1,math,"Math") ;
    
    Session_AddGenericData(gdat) ;
    
    assert(gdat == Session_FindGenericData(Math_t,"Math")) ;
  }
  
  {
    Math_t* math = (Math_t*) GenericData_GetData(gdat) ;
  
    Math_FreeBuffer(math) ;
  
    return(math) ;
  }
}



_INLINE_ void (Math_Delete)(void* self)
{
  Math_t* math = (Math_t*) self ;
  
  {
    Buffers_t* buf = Math_GetBuffers(math) ;
    
    if(buf) {
      Buffers_Delete(buf)  ;
      Mry_Free(buf)  ;
    }
  }
  
  Math_GetDelete(math) = NULL ;
}



_INLINE_ double* (Math_ComputeRealEigenvaluesAndEigenvectorsOf3x3Matrix)(double* a,const char job)
/** Return a pointer to a 3-term array composed of the eigenvalues of "a".
 *  On output the matrix components of "a" are lost and replaced by the
 *  eigenvectors stored in the same order as their eigenvalues.
 *  if job = 'l' left eigenvectors are stored  (i.e. vl * a = lambda vl)
 *  if job = 'r' right eigenvectors are stored (i.e. a * vr = lambda vr)
 */
{
  Math_t* math = Math_GetInstance() ;
  size_t SizeNeeded = 3*(sizeof(double)) ;
  double* eval = (double*) Math_AllocateInBuffer(math,SizeNeeded) ;
  
  #ifdef HAVE_LAPACK
  {
    double wr[3] ; /* real part */
    double wi[3] ; /* imaginary part (not used) */
    double vl[9] ;
    double vr[9] ;
    
    {
      int N = 3 ;
      int info;
      int lwork = 10 * N;
      double work[lwork];
      char  jobvr = (job == 'r') ? 'V' : 'N' ;
      char  jobvl = (job == 'l') ? 'V' : 'N' ;
    
      dgeev_(&jobvl,&jobvr,&N,a,&N,wr,wi,vl,&N,vr,&N,work,&lwork,&info);
    
      if(info) {
        Message_RuntimeError("dgeev of LAPACK has not converged") ;
      }
    }
    
    {
      double* vec = (job == 'l') ? vl : vr ;
      int i ;
      
      for(i = 0 ; i < 9 ; i++) {
        a[i] = vec[i] ;
      }
    
      for(i = 0 ; i < 3 ; i++) {
        eval[i] = wr[i] ;
      }
    }
    
    return(eval) ;
  }
  #endif
  
  Message_RuntimeError("LAPACK is needed") ;
  
  return(eval) ;
}



_INLINE_ double (Math_Compute3x3MatrixDeterminant)(const double* a)
/** Return the determinant of a 3x3 matrix */
{
  double det ;
  
  {
    det  = a[0] * a[4] * a[8] - a[0] * a[7] * a[5] \
         + a[3] * a[7] * a[2] - a[3] * a[1] * a[8] \
         + a[6] * a[1] * a[5] - a[6] * a[4] * a[2] ;
  }
  
  return(det) ;
}



_INLINE_ double* (Math_Inverse3x3Matrix)(const double* a)
/** Return a pointer to the inverse of a 3x3 matrix 
 *  or NULL if not invertible. */
{
  double det = Math_Compute3x3MatrixDeterminant(a) ;
  
  if(det == 0) return(NULL) ;

  {
      Math_t* math = Math_GetInstance() ;
      size_t SizeNeeded = 9*(sizeof(double)) ;
      double* b = (double*) Math_AllocateInBuffer(math,SizeNeeded) ;
        
      b[0] = a[4] * a[8] - a[7] * a[5] ;
      b[1] = a[7] * a[2] - a[1] * a[8] ;
      b[2] = a[1] * a[5] - a[4] * a[2] ;
      b[3] = a[5] * a[6] - a[8] * a[3] ;
      b[4] = a[8] * a[0] - a[2] * a[6] ;
      b[5] = a[2] * a[3] - a[5] * a[0] ;
      b[6] = a[3] * a[7] - a[6] * a[4] ;
      b[7] = a[6] * a[1] - a[0] * a[7] ;
      b[8] = a[0] * a[4] - a[3] * a[1] ;
  
      {
        double ted = 1./det ;
        int i ;
    
        for(i = 0 ; i < 9 ; i++) {
          b[i] *= ted ;
        }
      }

      return(b) ;
  }
}



_INLINE_ double* (Math_ComputePrincipalStresses)(const double* sig)
/** Return a pointer to a 3-term vector composed of the 
 *  principal stresses of "sig", i.e. as roots of the cubic equation:
 *  x^3 - I1*x^2 + I2*x - I3 = 0
 *  where I1, I2, I3 are the invariants of "sig":
 *  I1 = tr(sig) = sig11 + sig22 + sig33
 *  I2 = 1/2 * (tr(sig)^2 - tr(sig.sig)) 
 *     = sig11 * sig22 + sig22 * sig33 + sig33 * sig11
 *     - sig12 * sig21 - sig23 * sig32 - sig31 * sig13
 *  I3 = det(sig)
 */
{
  Math_t* math = Math_GetInstance() ;
  size_t SizeNeeded = 3*(sizeof(double)) ;
  double* sigp = (double*) Math_AllocateInBuffer(math,SizeNeeded) ;
    
  double I1 = sig[0] + sig[4] + sig[8] ;
  double I3 = Math_Compute3x3MatrixDeterminant(sig) ;
  double I2 = sig[0]*sig[4] + sig[4]*sig[8] + sig[8]*sig[0] \
            - sig[1]*sig[3] - sig[5]*sig[7] - sig[6]*sig[2] ;

  double p = I2 / 3 - I1 * I1 / 9 ; /* p < 0 provided that sig is symmetric */
  double q = 0.5 * I3 + I1 * I1 * I1 / 27 - I1 * I2 / 6 ;
  
  assert(p < 0) ;
  
  {
    double sqrp = sqrt(-p) ;
    double t = acos(q/(p*sqrp)) / 3 ; /* 0 < t < Pi/3 */
    double ct = sqrp * cos(t) ; /* ct > 0 */
    double st = sqrp * sin(t) ; /* st > 0 */
    double sqr3 = sqrt(3.) ;
    double b1 = 2 * ct             + I1 / 3 ;
    double b2 =    -ct + sqr3 * st + I1 / 3 ;
    double b3 =    -ct - sqr3 * st + I1 / 3 ;
      
    sigp[0] = b1 ;
    sigp[1] = b2 ;
    sigp[2] = b3 ;
  }

  return(sigp) ;
}



_INLINE_ double (Math_ComputeFirstStressInvariant)(const double* sig)
/** First invariant of a stress tensor: I1 = tr(sig) */
{
  return(sig[0] + sig[4] + sig[8]);
}



_INLINE_ double (Math_ComputeSecondStressInvariant)(const double* sig)
/** Second invariant of a symmetric stress tensor:
    I2 = 1/2 tr(sig.sig) = 1/2 sig_ik sig_ki */
{
  double i2a = sig[0]*sig[0] + sig[4]*sig[4] + sig[8]*sig[8] ;
  double i2b = sig[1]*sig[3] + sig[2]*sig[6] + sig[5]*sig[7] ;
  return(0.5*i2a + i2b) ;
}



_INLINE_ double (Math_ComputeSecondDeviatoricStressInvariant)(double const* sig)
/** Second invariant of the deviatoric part of a stress tensor:
    J2 = 1/2 tr(dev.dev)  (dev = sig - 1/3 tr(sig) Id) */
{
#define SIG(i,j) (sig[3*(i)+(j)])
  double j2a = (SIG(0,0) - SIG(1,1))*(SIG(0,0) - SIG(1,1))
             + (SIG(1,1) - SIG(2,2))*(SIG(1,1) - SIG(2,2))
             + (SIG(2,2) - SIG(0,0))*(SIG(2,2) - SIG(0,0)) ;
  double j2b = SIG(0,1)*SIG(1,0) + SIG(1,2)*SIG(2,1) + SIG(2,0)*SIG(0,2) ;
  return(j2a/6. + j2b) ;
#undef SIG
}



_INLINE_ double* (Math_ComputeDeviatoricStress)(const double* sig)
/** Return a pointer to the deviator of sig */
{
  Math_t* math = Math_GetInstance() ;
  size_t SizeNeeded = 9*(sizeof(double)) ;
  double* sigd = (double*) Math_AllocateInBuffer(math,SizeNeeded) ;
  double  sigm = Math_ComputeFirstStressInvariant(sig)/3 ;
  
  {
    int i ;
    
    for(i = 0 ; i < 9 ; i++) {
      sigd[i] = sig[i] ;
    }
        
    sigd[0] -= sigm ;
    sigd[4] -= sigm ;
    sigd[8] -= sigm ;
  }

  return(sigd) ;
}



_INLINE_ double* (Math_SolveByGaussEliminationWithPartialPivoting)(double* a,double* b,int n)
/** Return a pointer to the solution x of the system a.x = b
 *  by Gaussian elimination with backsubstitution and partial 
 *  pivoting (interchange of rows). The implementation 
 *  corresponds to the kij ordering where
 *    j = column index
 *    i = row index
 *    k = dummy summation index
 *  On input:
 *  - a is the matrix stored by rows
 *  - n is the dimension of the system (nb of rows/columns)
 *  - b is the right hand side vector
 *  On output:
 *  - a is replaced by the LU decomposition
 *      of a rowwise permutation of itself.
 *  - b is replaced by the solution x.
 * 
 *  Ref:
 *  W. Press, S.A. Teukolky, W.T. Vetterling, B.P. Flannery
 *  Numerical recipes, the art of scientific computing, 3rd ed.
 *  Cambridge university press, 2007.
 **/
{
#define  A(i,j)  (a[(i)*n+(j)])
#define  SWAP(a,b) Math_SwapDouble(a,b)
/* For the record but actually not returned: 
 * d is output +1 or -1 depending on whether the number of row
 * interchanges was even or odd, respectively. */
double d = 1 ;

  {
    int    k ;

    /* This is the outermost kij loop */
    for(k = 0 ; k < n ; k++) {
      int i ;
    
      {
        double big = 0. ;
        int imax = k ;
      
        /* Search for largest pivot */
        for(i = k ; i < n ; i++) {
          if(fabs(A(i,k)) > big) {
            big  = fabs(A(i,k)) ;
            imax = i ;
          }
        }
    
        /* Do we need to interchange rows? */
        if(k != imax) {
          int j ;
          
          for(j = 0 ; j < n ; j++) {
            SWAP(A(imax,j),A(k,j)) ;
          }
        
          SWAP(b[imax],b[k]) ;
          
          d *= -1 ;
        }
      }
    
      if(A(k,k) == 0.) {
        arret("Math_SolveByGaussEliminationWithPartialPivoting") ;
      }
    
      for(i = k + 1 ; i < n ; i++) {
        int j ;
      
        /* Divide by the pivot */
        A(i,k) /= A(k,k) ;
      
        /* Innermost loop: reduce remaining submatrix  */
        for(j = k + 1 ; j < n ; j++) {
          A(i,j) -= A(i,k) * A(k,j) ;
        }
      
        /* Forward substitution */
        b[i] -= A(i,k) * b[k] ;
      }
    }
  }
  
  /* Backsubstitution */
  {
    int i ;
    
    for(i = n - 1 ; i >= 0 ; i--) {
      int j ;
      
      for(j = i + 1 ; j < n ; j++) {
        b[i] -= A(i,j) * b[j] ;
      }
      
      b[i] /= A(i,i) ;
    }
  }
  
  return(b) ;
#undef A
#undef SWAP
}



_INLINE_ double* (Math_SolveByGaussEliminationJIK)(double* a,double* b,int n,int* indx)
/** Return a pointer to the solution x of the system a.x = b
 *  by Gaussian elimination with backsubstitution and partial 
 *  pivoting (interchange of rows). The implementation 
 *  corresponds to the jik ordering where
 *    j = column index
 *    i = row index
 *    k = dummy summation index
 *  On input:
 *  - a is the matrix stored by rows
 *  - n is the dimension of the system (nb of rows/columns)
 *  - b should point to the right hand side vector or to NULL
 *  - indx is a valid pointer to a free space of n*int or NULL
 *  if indx = NULL no pivoting is performed.
 *  On output:
 *  - a is replaced by the LU decomposition
 *      of a rowwise permutation of itself.
 *  - b is replaced by the solution x if b is not NULL.
 * 
 *  Ref:
 *  W. Press, S.A. Teukolky, W.T. Vetterling, B.P. Flannery
 *  Numerical recipes, the art of scientific computing, 2nd ed.
 *  Cambridge university press, 2007.
 **/
{
#define  A(i,j)  (a[(i)*n+(j)])
#define  SWAP(a,b) Math_SwapDouble(a,b)
/* For the record but actually not returned: 
 * d is output +1 or -1 depending on whether the number of row
 * interchanges was even or odd, respectively. */
double d = 1 ;

  {
    int j ;
    
    /* Loop over columns */
    for(j = 0 ; j < n ; j++) {
      int i ;
      
      /* Loop over rows */
      for(i = 0 ; i < j ; i++) {
        int k ;
        
        for(k = 0 ; k < i ; k++) {
          A(i,j) -= A(i,k) * A(k,j) ;
        }
      }
    
      for(i = j ; i < n ; i++) {
        int k ;
        
        for(k = 0 ; k < j ; k++) {
          A(i,j) -= A(i,k) * A(k,j) ;
        }
      }
      
      if(indx) {
        double big = 0 ;
        int imax = j ;
    
        /* Search for largest pivot */
        for(i = j ; i < n ; i++) {
          if(fabs(A(i,j)) > big) {
            big = fabs(A(i,j)) ;
            imax = i ;
          }
        }
        
        /* Do we need to interchange rows? */
        if(j != imax) {
          int k ;
        
          for(k = 0 ; k < n ; k++) {
            SWAP(A(imax,k),A(j,k)) ;
          }
          
          d *= -1 ;
        }
      
        indx[j] = imax ;
      }
    
      if(A(j,j) == 0.) {
        arret("Math_SolveByGaussEliminationJIK") ;
      }
    
      /* Divide by the pivot */
      for(i = j + 1 ; i < n ; i++) {
        A(i,j) /= A(j,j) ;
      }
    }
  }
  
  if(b) {
    int ii = 0 ;
    int i ;
    
    /* Forward substitution */
    for(i = 0 ; i < n ; i++) {
      int ip = (indx) ? indx[i] : i ;

      if(ip != i) SWAP(b[ip],b[i]) ;
      
      if(ii) {
        int j ;
        
        for(j = ii - 1 ; j < i ; j++) {
          b[i] -= A(i,j) * b[j] ;
        }
      } else {
        if(b[i] != 0) ii = i + 1 ;
      }
    }
  
    /* Backsubstitution */
    for(i = n - 1 ; i >= 0 ; i--) {
      int j ;
      
      for(j = i + 1 ; j < n ; j++) {
        b[i] -= A(i,j) * b[j] ;
      }
      
      b[i] /= A(i,i) ;
    }
  }
  
  return(b) ;
#undef A
#undef SWAP
}



_INLINE_ double* (Math_SolveByGaussEliminationKIJ)(double* a,double* b,int n,int* indx)
/** Return a pointer to the solution x of the system a.x = b
 *  by Gaussian elimination with backsubstitution and partial 
 *  pivoting (interchange of rows). The implementation 
 *  corresponds to the kij ordering where
 *    j = column index
 *    i = row index
 *    k = dummy summation index
 *  On input:
 *  - a is the matrix stored by rows
 *  - n is the dimension of the system (nb of rows/columns)
 *  - b should point to the right hand side vector or to NULL.
 *  - indx is a valid pointer to a free space of n*int or NULL
 *  if indx = NULL no pivoting is performed.
 *  On output:
 *  - a is replaced by the LU decomposition
 *      of a rowwise permutation of itself.
 *  - b is replaced by the solution x if b is not NULL.
 * 
 *  Ref:
 *  W. Press, S.A. Teukolky, W.T. Vetterling, B.P. Flannery
 *  Numerical recipes, the art of scientific computing, 3rd ed.
 *  Cambridge university press, 2007.
 **/
{
#define  A(i,j)  (a[(i)*n+(j)])
#define  SWAP(a,b) Math_SwapDouble(a,b)
/* For the record but actually not returned: 
 * d is output +1 or -1 depending on whether the number of row
 * interchanges was even or odd, respectively. */
double d = 1 ;

  {
    int k ;
    
    /* This is the outermost kij loop */
    for(k = 0 ; k < n ; k++) {
      int i ;

      if(indx) {
        double big = 0. ;
        int imax = k ;
        
        /* Search for largest pivot */
        for(i = k ; i < n ; i++) {
          if(fabs(A(i,k)) > big) {
            big  = fabs(A(i,k)) ;
            imax = i ;
          }
        }
    
        /* Do we need to interchange rows? */
        if(k != imax) {
          int j ;
        
          for(j = 0 ; j < n ; j++) {
            SWAP(A(imax,j),A(k,j)) ;
          }
          
          d *= -1 ;
        }
      
        indx[k] = imax ;
      }
    
      if(A(k,k) == 0.) {
        arret("Math_SolveByGaussEliminationKIJ") ;
      }
    
      for(i = k + 1 ; i < n ; i++) {
        int j ;
        
        /* Divide by the pivot */
        A(i,k) /= A(k,k) ;
        
        /* Innermost loop: reduce remaining submatrix  */
        for(j = k + 1 ; j < n ; j++) {
          A(i,j) -= A(i,k) * A(k,j) ;
        }
      }
    }
  }
  
  if(b) {
    int ii = 0 ;
    int i ;
    
    /* Forward substitution */
    for(i = 0 ; i < n ; i++) {
      int ip = (indx) ? indx[i] : i ;

      if(ip != i) SWAP(b[ip],b[i]) ;
      
      if(ii) {
        int j ;
        
        for(j = ii - 1 ; j < i ; j++) {
          b[i] -= A(i,j) * b[j] ;
        }
      } else {
        if(b[i] != 0) ii = i + 1 ;
      }
    }
  
    /* Backsubstitution */
    for(i = n - 1 ; i >= 0 ; i--) {
      int j ;
      
      for(j = i + 1 ; j < n ; j++) {
        b[i] -= A(i,j) * b[j] ;
      }
      
      b[i] /= A(i,i) ;
    }
  }
  
  return(b) ;
#undef A
#undef SWAP
}




_INLINE_ void (Math_PrintStiffnessTensor)(const double* c)
/** Print a 4th rank tensor.
 **/
{
  {
    int i ;
    
    for(i = 0 ; i < 9 ; i++) {
      int j = i - (i/3)*3 ;
        
      printf("C%d%d--:",i/3 + 1,j + 1) ;
        
      for (j = 0 ; j < 9 ; j++) {
        printf(" % e",c[i*9 + j]) ;
      }
        
      printf("\n") ;
    }
  }
}




_INLINE_ void (Math_PrintMatrix)(const double* c,const int n)
/** Print a matrix of rank n.
 **/
{
  if(n) {
    int i ;
    
    for(i = 0 ; i < n ; i++) {
      int j ;
        
      printf("Row(%d)-Col(1-%d): (",i + 1,n) ;
        
      for (j = 0 ; j < n ; j++) {
        printf(" % e",c[i*n + j]) ;
      }
        
      printf(")\n") ;
    }
  }
}




_INLINE_ void (Math_PrintVector)(const double* c,const int n)
/** Print a vector of rank n.
 **/
{
    if(n) {
      int j ;
        
      printf("V(1-%d): (",n) ;
        
      for (j = 0 ; j < n ; j++) {
        printf(" % e",c[j]) ;
      }
        
      printf(")\n") ;
    }
}




_INLINE_ void (Math_PrintStressTensor)(const double* c)
/** Print a 2nd rank tensor.
 **/
{
  {
    int i ;
    
    for(i = 0 ; i < 3 ; i++) {
      int j ;
        
      printf("S%d-:",i + 1) ;
        
      for (j = 0 ; j < 3 ; j++) {
        printf(" % e",c[i*3 + j]) ;
      }
        
      printf("\n") ;
    }
  }
}



#if 1
_INLINE_ double* (Math_SolveByGaussJordanElimination)(double* a,double* b,int n,int m)
/** Return matrix inverse and replace rhs vectors by solution vectors.
 *  We use Gauss-Jordan elimination with full pivoting.
 *  On input:
 *  - the matrix a[n*n] is arranged sequentially rwo by row.
 *  - the r.h.s. b[n*m] contain the m right-hand side vectors. 
 *  On output:
 *  - a is replaced by its matrix inverse, and 
 *  - b is replaced by the corresponding set of solution vectors.
 *  Used with m = 0, it only replaces a by its inverse.
 *  Ref:
 *  W.H. Press, S.A. Teukolsky, W.T. Vetterling, B.P. Flannery. 
 *  Numerical Recipes, Cambridge University Press, 2007.
 */
{
#define  A(i,j)  (a[(i)*n+(j)])
#define  B(i,j)  (b[(i)*m+(j)])
#define  SWAP(a,b) Math_SwapDouble(a,b)
#define  MaxNbOfRows (100)
  /* These integer arrays are used for bookkeeping on the pivoting. */
  int    indxc[MaxNbOfRows],indxr[MaxNbOfRows],ipiv[MaxNbOfRows] ;
  int    icol,irow ;
  int    i ;

  if(n > MaxNbOfRows) {
    arret("Math_SolveByGaussJordanElimination") ;
  }

  for(i = 0 ; i < n ; i++) ipiv[i] = 0 ;
  
  /* This is the main loop over the columns to be reduced */
  for(i = 0 ; i < n ; i++) { 
    double big = 0. ;
    int j,ll ;
    
    /* This is the outer loop of the search for a pivot element */
    for(j = 0 ; j < n ; j++) {
      if(ipiv[j] != 1) {
        int k ;
        
        for(k = 0 ; k < n ; k++) {
          if(ipiv[k] == 0) {
            if(fabs(A(j,k)) >= big) {
              big  = fabs(A(j,k)) ;
              irow = j ;
              icol = k ;
            }
          }
        }
      }
    }
    
    ++(ipiv[icol]) ;
    
    /* We now have the pivot element, so we interchange rows, if needed, to put the pivot element on the diagonal. The columns are not physically interchanged, only relabeled: indxc[i], the column of the (i+1)th pivot element, is the (i+1)th column that is reduced, while indxr[i] is the row in which that pivot element was originally located. If indxr[i] != indxc[i], there is an implied column interchange. With this form of bookkeeping, the solution b’s will end up in the correct order, and the inverse matrix will be scrambled by columns. */
    
    if(irow != icol) {
      int l ;
      
      for(l = 0 ; l < n ; l++) SWAP(A(irow,l),A(icol,l)) ;
      for(l = 0 ; l < m ; l++) SWAP(B(irow,l),B(icol,l)) ;
    }
    
    /* We are now ready to divide the pivot row by the pivot element, 
     * located at irow and icol. */
     
    indxr[i] = irow ; 
    indxc[i] = icol ;
    
    if(A(icol,icol) == 0.) {
      arret("Math_SolveByGaussJordanElimination: singular matrix") ;
    }
    
    {
      double pivinv = 1./A(icol,icol) ;
      int l ;
      
      A(icol,icol) = 1. ;
    
      for(l = 0 ; l < n ; l++) A(icol,l) *= pivinv ;
      for(l = 0 ; l < m ; l++) B(icol,l) *= pivinv ;
    }
    
    /* Next, we reduce the rows...
     * ...except for the pivot one, of course. */
    for(ll = 0 ; ll < n ; ll++) {
      if(ll != icol) {
        double dum = A(ll,icol) ;
        int l ;
        
        A(ll,icol) = 0. ;
        
        for(l = 0 ; l < n ; l++) A(ll,l) -= A(icol,l)*dum ;
        for(l = 0 ; l < m ; l++) B(ll,l) -= B(icol,l)*dum ;
      }
    }
  }
    
  /* This is the end of the main loop over columns of the reduction. It only remains to unscramble the solution in view of the column interchanges. We do this by interchanging pairs of columns in the reverse order that the permutation was built up. */

  {
    int l ;
      
    for(l = n-1 ; l >= 0 ; l--) {
      if(indxr[l] != indxc[l]) {
        int k ;
        
        for(k = 0 ; k < n ; k++) {
          SWAP(A(k,indxr[l]),A(k,indxc[l])) ;
        }
      }
    }
  }
  
  return(a) ;
#undef A
#undef B
#undef SWAP
}
#endif



/*
   Polynomial roots
*/
_INLINE_ int (Math_ComputePolynomialEquationRootsOfDegree2)(double* x)
{
  double b = x[1]/x[0] ;
  double c = x[2]/x[0] ;
  double delta = b*b - 4*c ;
  
  if(c == 0) {
    if(b == 0) {
      x[0] = 0 ;
      return(1) ;
    } else {
      x[0] = MAX(-b,0) ;
      x[1] = MIN(-b,0) ;
      return(2) ;
    }
  } else if(delta == 0) {
    double temp1 = - b*0.5 ;
    x[0] = temp1 ;
    return(1) ;
  } else if(delta > 0) {
    double temp1 = - b*0.5;
    double temp2 = sqrt(delta)*0.5 ;
    if(b < 0) {
      x[0] = temp1 + temp2 ;
      x[1] = c/x[0] ;
    } else {
      x[1] = temp1 - temp2 ;
      x[0] = c/x[1] ;
    }
    return(2) ;
  }
  
  return(0) ;
}



_INLINE_ int (Math_ComputePolynomialEquationRootsOfDegree3)(double* x)
{
  double b = x[1]/x[0] ;
  double c = x[2]/x[0] ;
  double d = x[3]/x[0] ;
  /* Depressed cubic equation : x3 + px + q = 0 */
  double p  = c - b*b/3 ;
  double q  = d + 2*b*b*b/27 - b*c/3 ;
  double p3 = p*p*p ;
  double q2 = q*q ;
  double r2 = p3/27 + q2/4 ;
  int n = 0 ;
  
  if(q == 0) {
    if(p >= 0) {
      x[0] = 0 ;
      n = 1 ;
    } else {
      double r = sqrt(-p) ;
      x[0] = r ;
      x[1] = 0 ;
      x[2] = - r ;
      n = 3 ;
    }
    
  } else if(r2 == 0) { /* q != 0 and p3/27 = -q2/4 (< 0) */
    double x1 = 3*q/p ;
    double x2 = - 0.5*x1 ; /* double root */
    x[0] = MAX(x1,x2) ;
    x[1] = MIN(x1,x2) ;
    n = 2 ;
    
  } else if(r2 > 0) {
    double r  = sqrt(r2) ;
    double r1 = (q < 0) ? r : -r ;
    double u3 = - 0.5*q + r1 ; /* so u3 is != 0 even for p = 0 */
    double u  = (u3 > 0) ? pow(u3,1/3.) : - pow(-u3,1/3.) ;
    x[0] = u - p/(3*u) ;
    n = 1 ;
    
  /* Trigonometric solution */
  } else { /* q != 0 and p3/27 < -q2/4 (< 0) */
    double srp3 = sqrt(-p/3) ;
    double k    = acos(1.5*q/(p*srp3))/3 ; /* 0 < k < pi/3 */
    double ck   = srp3*cos(k) ;
    double sk   = srp3*sin(k) ;
    double sr3  = sqrt(3.) ;
    double x1   = 2*ck ;
    double x2   = - ck + sr3*sk ;
    double x3   = - ck - sr3*sk ;
    x[0] = x1 ;
    x[1] = x2 ;
    x[2] = x3 ;
    n = 3 ;
  }
  
  {
    int i ;
    for(i = 0 ; i < n ; i++) x[i] -= b/3 ;
  }
  
  return(n) ;
}



_INLINE_ int (Math_ComputePolynomialEquationRootsOfDegree4)(double* x)
{
  double b = x[1]/x[0] ;
  double c = x[2]/x[0] ;
  double d = x[3]/x[0] ;
  double e = x[4]/x[0] ;
  /* Depressed quartic equation: x4 + px2 + qx + r = 0 */
  double b2 = b*b ;
  double b3 = b2*b ;
  double b4 = b2*b2 ;
  double p  = - 0.375*b2 + c ;
  double q  =   0.125*b3 - 0.5*b*c + d ;
  double r  = - 0.01171875*b4 + 0.0625*b2*c - 0.25*b*d + e ;
  int n = 0 ;
  
  if(e == 0) {
    int i ;
    n = Math_ComputePolynomialEquationRootsOfDegree3(x) ;
    x[n] = 0 ;
    for(i = n ; i > 0 ; i--) {
      if(x[i - 1] >= 0) break ;
      x[i] = x[i - 1] ;
      x[i - 1] = 0 ;
    }
    return(n + 1) ;
    
  /* Biquadractic equations */
  } else if(q == 0) { 
    double y[3] = {1,0,0} ;
    int i,m ;
    y[1] = p ;
    y[2] = r ;
    m = Math_ComputePolynomialEquationRootsOfDegree2(y) ;
    /* First keep only positive roots */
    for(i = 0 ; i < m ; i++) {
      if(y[i] >= 0) {
        x[i] = sqrt(y[i]) ;
        n++ ;
      }
    }
    /* Second put the negative roots behind */
    for(i = 0 ; i < n ; i++) {
        x[2*n - 1 - i] =  - x[i] ;
    }
    n *= 2 ;
    
  /* Ferrari's solution */
  } else { 
    /* The nested depressed cubic equation: x3 + p1x + q1 */
    double p1 = - p*p/12 - r ;
    double q1 = - p*p*p/108 + p*r/3 - 0.125*q*q ;
    double y[4] = {1,0,0,0} ;
    y[2] = p1 ;
    y[3] = q1 ;
    /* Solving for the nested depressed cubic equation */
    Math_ComputePolynomialEquationRootsOfDegree3(y) ;
    {
      double s = y[0] - 5./6*p ;
      double w2 = p + 2*s ;
      if(w2 <= 0) { /* Theoretically it's never met */
        arret("Math_ComputePolynomialEquationRootsOfDegree4: never met?") ;
        return(0) ;
      } else {
        double w = sqrt(w2) ;
        double h1 = - (3*p + 2*s + 2*q/w) ;
        double h2 = - (3*p + 2*s - 2*q/w) ;
        if(h1 == 0) {
          x[0] = 0.5*w ;
          n = 1 ;
        } else if(h1 > 0) {
          x[0] = 0.5*(w + sqrt(h1)) ;
          x[1] = 0.5*(w - sqrt(h1)) ;
          n = 2 ;
        }
        if(h2 == 0) {
          x[n++] = - 0.5*w ;
        } else if(h2 > 0) {
          x[n++] = 0.5*(- w + sqrt(h2)) ;
          x[n++] = 0.5*(- w - sqrt(h2)) ;
        }
      }
    }
  }
  
  {
    int i ;
    for(i = 0 ; i < n ; i++) x[i] -= 0.25*b ;
  }
  
  return(n) ;
}



/*
 * Solve polynomial equations of degrees 1 to 4
 */
_INLINE_ int (Math_ComputePolynomialEquationRoots)(double* x,int const n)
/** Real Roots of Polynomial Equation of Degree n
 *  Return the nb of real solutions, sorted 
 *  from max to min */
{
#define MAX_DEGREE  (4)
  //double y[MAX_DEGREE + 1] ;
  int m = 0 ;
  
  /*
  {
    int i ;
    
    for(i = 0 ; i < n + 1 ; i++)  y[i] = x[i] ;
  }
  */
  
  if(x[0] != 0) {
    if(n == 0) {
      arret("Math_ComputePolynomialEquationRoots: degree 0") ;
    } else if(n == 1) {
      x[0] = - x[1]/x[0] ;
      return(1) ;
    } else if(n == 2) {
      m = Math_ComputePolynomialEquationRootsOfDegree2(x) ;
      return(m) ;
    } else if(n == 3) {
      m = Math_ComputePolynomialEquationRootsOfDegree3(x) ;
    } else if(n == 4) {
      m = Math_ComputePolynomialEquationRootsOfDegree4(x) ;
    } else {
      arret("Math_ComputePolynomialEquationRoots: degree too big") ;
    }
  } else {
    int i ;
    
    m = Math_ComputePolynomialEquationRoots(x + 1,n - 1) ;
    for(i = 0 ; i < m ; i++) x[i] = x[i + 1] ;
    return(m) ;
  }
  
  /* Polish the solutions */
  /*
  {
    double tol = 1.e-10 ;
    int i ;

    for(i = 0 ; i < m ; i++) {
      int k = Math_PolishPolynomialEquationRoot(y,n,x+i,fabs(z)*tol,10) ;
    }
  }
  */
  
  return(m) ;
#undef MAX_DEGREE
}



_INLINE_ int (Math_PolishPolynomialEquationRoot)(double* x,int n,double* proot,double tol,int iterations)
{
  double root = proot[0] ;
  int it ;
  
  for(it = 0 ; it < iterations ; it++) {
    int i ;
    double error = x[0] ;
    double derivative = 0 ;
    double droot ;
      
    for(i = 0 ; i < n ; i++) {
      double a = error ;
      double b = x[i + 1] ;
      
      error = a*root + b ;
    }
      
    for(i = 0 ; i < n ; i++) {
      double a = derivative ;
      double b = (n - i)*x[i] ;
      
      derivative = a*root + b ;
    }
      
    if(derivative == 0) {
      proot[0] = root ;
      return(0) ;
    }
      
    droot = - error / derivative ;
    root += droot ;
      
    if(fabs(droot) < tol) {
      proot[0] = root ;
      return(0) ;
    }
  }
  
  /* Raise an interrupt signal instead of exit */
  Message_Warning("Math_PolishPolynomialEquationRoot: no convergence") ;
  {
    int i ;
    
    Message_Direct("\nthe %dth order polynomial equation:\n",n) ;
    
    for(i = 0 ; i <= n ; i++) {
      Message_Direct("%d order coefficient: %lf\n",n-i,x[i]) ;
    }
    
    Message_Direct("\nhas not converged for the root %lf\n",root) ;
  }
  //Exception_Interrupt ;
  /*
  arret("Math_PolishPolynomialEquationRoot: not converged ") ;
  */
  return(-1) ;
}

#undef _INLINE_



#ifdef __CPLUSPLUS
}
#endif
#endif
