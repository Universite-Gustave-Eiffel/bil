#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include "Message.h"
#include "Math_.h"
#include "Geometry.h"
#include "Elements.h"
#include "Element.h"
#include "ShapeFcts.h"
#include "IntFcts.h"
#include "Nodes.h"
#include "Node.h"
#include "Models.h"
#include "Session.h"
#include "GenericData.h"
#include "Mry.h"
#include "SharedMS.h"
#include "GenericObject.h"
#include "Mesh.h"
#include "Load.h"
#include "Model.h"
#include "Mesh.h"
#include "IntFct.h"
#include "Buffers.h"
#include "Function.h"
#include "Field.h"
#include "FEM.h"


static FEM_t*  (FEM_Create)(void) ;


#define _INLINE_

_INLINE_ FEM_t* (FEM_Create)(void)
{
  FEM_t*  fem    = (FEM_t*) Mry_New(FEM_t) ;
  
  
  /* Space allocation for output */
  {
    int const nn = Element_MaxNbOfNodes ;
    int const neq = Model_MaxNbOfEquations ;
    int const ndof = nn*neq ;
    double* output = (double*) Mry_New(double[ndof*ndof]) ;
    
    FEM_SetOutput(fem,output) ;
  }
  
  
  /* Space allocation for input */
  {
    int const np = IntFct_MaxNbOfIntPoints ;
    double* input = (double*) Mry_New(double[np*FEM_MaxShift]) ;
    
    FEM_SetInput(fem,input) ;
  }
  
  
  /* Space allocation for pintfct */
  {
    IntFct_t** pintfct = (IntFct_t**) Mry_New(IntFct_t*[IntFcts_MaxNbOfIntFcts]) ;
    
    FEM_SetPointerToIntFct(fem,pintfct) ;
  }
  
  
  /* Space allocation for buffer */
  {
    Buffers_t* buf = Buffers_Create(FEM_SizeOfBuffer) ;
    
    FEM_SetBuffers(fem,buf) ;
  }
  
  return(fem) ;
}



_INLINE_ void (FEM_Delete)(void* self)
{
  FEM_t* fem = (FEM_t*) self ;
  
  free(FEM_GetOutput(fem)) ;
  
  free(FEM_GetInput(fem)) ;
  
  free(FEM_GetPointerToIntFct(fem)) ;
  
  {
    Buffers_t* buf = FEM_GetBuffers(fem) ;

    if(buf) {
      Buffers_Delete(buf)  ;
      free(buf) ;
      FEM_SetBuffers(fem,NULL) ;
    }
  }
}



_INLINE_ FEM_t*  (FEM_GetInstance)(Element_t* el)
#if SharedMS_APIis(OpenMP)
{
  {
    GenericData_t* gdat = Session_FindGenericData(FEM_t,"FEM") ;
    
    if(!gdat) {
      #pragma omp critical
      if(!Session_FindGenericData(FEM_t,"FEM")) {
        int n = SharedMS_MaxNbOfThreads ;
        FEM_t* fem = Mry_Create(FEM_t,n,FEM_Create()) ;
        
        gdat = GenericData_Create(n,fem,"FEM") ;
    
        Session_AddGenericData(gdat) ;
    
        assert(gdat == Session_FindGenericData(FEM_t,"FEM")) ;
      }
    }
  }
  
  {
    GenericData_t* gdat = Session_FindGenericData(FEM_t,"FEM") ;
    
    if(gdat) {
      FEM_t* fem0 = ((FEM_t*) GenericData_GetData(gdat)) ;
      int id = SharedMS_CurrentThreadId ;
      
      if(id < GenericData_GetNbOfData(gdat)) {
        FEM_t* fem = fem0 + id ;

        FEM_SetElement(fem,el) ;
  
        FEM_FreeBuffer(fem) ;
  
        return(fem) ;
      } else {
        Message_FatalError("FEM_GetInstance:") ;
      }
    } else {
      Message_FatalError("FEM_GetInstance:") ;
    }
  }
}
#else
{
  GenericData_t* gdat = Session_FindGenericData(FEM_t,"FEM") ;
  
  if(!gdat) {
    FEM_t* fem = FEM_Create() ;
    
    gdat = GenericData_Create(1,fem,"FEM") ;
    
    Session_AddGenericData(gdat) ;
    
    assert(gdat == Session_FindGenericData(FEM_t,"FEM")) ;
  }
  
  {
    FEM_t* fem = (FEM_t*) GenericData_GetData(gdat) ;
  
    FEM_SetElement(fem,el) ;
  
    FEM_FreeBuffer(fem) ;
  
    return(fem) ;
  }
}
#endif



_INLINE_ double*  (FEM_ComputeStiffnessMatrix)(FEM_t* fem,IntFct_t* fi,const double* c,const int dec)
/** Return a pointer on a FE stiffness matrix (Ndof*Ndof)
 *  with Ndof = N * dim and N = nb of nodes */
{
#define KR(i,j)     (kr[(i)*ndof + (j)])
#define DH(n,i)     (dh[(n)*3 + (i)])
//#define DH(n,i)     (dh[(n)*dim + (i)])
#define C(i,j,k,l)  (c[(((i)*3 + (j))*3 + (k))*3 + (l)])
#define CAJ(i,j)    (caj[(i)*3 + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  auto dim = Element_GetDimensionOfSpace(el) ;
  auto dim_e = Element_GetDimension(el) ;
  auto nn  = Element_GetNbOfNodes(el) ;
  auto nf  = IntFct_GetNbOfFunctions(fi) ;
  auto np  = IntFct_GetNbOfPoints(fi) ;
  auto dim_h = IntFct_GetDimension(fi) ;
  auto ndof = nn*dim ;
  double* weight = IntFct_GetWeight(fi) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  size_t SizeNeeded = (sizeof(double))*ndof*ndof ;
  double* kr = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  double* x[Element_MaxNbOfNodes] ;
  double zero = 0. ;
  
  
  if(Element_IsSubmanifold(el)) {
    arret("FEM_ComputeStiffnessMatrix") ;
  }
  
  /* Initialization */
  {
    unsigned int i ;
    
    for(i = 0 ; i < ndof*ndof ; i++) kr[i] = zero ;
  }
  
  {    
    for(int i = 0 ; i < nn ; i++) {
      x[i] = Element_GetNodeCoordinate(el,i) ;
    }
  }
  
  /* One node mesh: for this special element the displacement u_i stands for strain_ii */
  if(nn == 1 && dim_e == 3) {    
    for(int i = 0 ; i < dim ; i++) {
      for(int j = 0 ; j < dim ; j++) {
        KR(i,j) = C(i,i,j,j) ;
      }
    }
    
    return(kr) ;
  }


  /* Interface elements */
  if(Element_HasZeroThickness(el)) {
    if(dim_h == dim - 1) {
      /* Assuming that the numbering of the element is formed with the nf first nodes  */
      if(nn > nf) {      
        /* Check the numbering */
        Element_CheckNumberingOfOverlappingNodes(el,nf) ;

        for(int p = 0 ; p < np ; p++ , c += dec) {
          double* h  = IntFct_GetFunctionAtPoint(fi,p) ;
          double* dh = IntFct_GetFunctionGradientAtPoint(fi,p) ;
          double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
          double a   = weight[p]*d ;
          double* norm = Element_ComputeNormalVector(el,dh,nf,dim_h) ;
          double r[9] = {0,0,0,0,0,0,0,0,0} ;
          int i,j,k,l ;

          if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
            double radius = zero ;
        
            for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
        
            a *= 2*M_PI*radius ;
        
            if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
          }
      
          /* */
          #define R(i,k)   r[(i)*3 + (k)]
          #define NORM(i)  (norm[i])
        
          for(i = 0 ; i < 3 ; i++) for(k = 0 ; k < 3 ; k++) {
          
            R(i,k) = 0 ;

            for(j = 0 ; j < 3 ; j++) for(l = 0 ; l < 3 ; l++) {
              double cijkl = C(i,j,k,l) + C(j,i,k,l) + C(i,j,l,k) + C(j,i,l,k) ;
            
              R(i,k) += NORM(j) * cijkl * NORM(l) ;
            }
          
            R(i,k) *= 0.25 ;
          }
        
          #undef NORM

          /* KR(j,i,l,k) = H(j) * R(i,k) * H(l) */
          for(j = 0 ; j < nf ; j++) for(i = 0 ; i < dim ; i++) {
            int jj = Element_OverlappingNode(el,j) ;
          
            for(l = 0 ; l < nf ; l++) for(k = 0 ; k < dim ; k++) {
              double k_jilk = a * h[j] * R(i,k) * h[l] ;
              int ll = Element_OverlappingNode(el,l) ;
          
              KR(j*dim  + i,l*dim  + k) +=   k_jilk ;
              KR(jj*dim + i,l*dim  + k) += - k_jilk ;
              KR(j*dim  + i,ll*dim + k) += - k_jilk ;
              KR(jj*dim + i,ll*dim + k) +=   k_jilk ;
            }
          }

          #undef R
        }
      
        return(kr) ;
      }
    }
  }

  
  /* Loop on integration points */
  {    
    for(int p = 0 ; p < np ; p++ , c += dec) {
      double* h  = IntFct_GetFunctionAtPoint(fi,p) ;
      double* dh = IntFct_GetFunctionGradientAtPoint(fi,p) ;
      double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
      double a   = weight[p]*d ;
      double* caj = Element_ComputeInverseJacobianMatrix(el,dh,nf,dim_h) ;
      double jcj[3][3][3][3],jc[3][3],cj[3][3] ;
      double radius ;
      int    i,j,k,l,r,s ;
    
      /* The radius in axisymmetrical or spherical case */
      if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
        radius = zero ;
        
        for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
        
        a *= 2*M_PI*radius ;
        
        if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
      }
    
      /* JCJ(r,i,k,s) = J(r,j) * C(i,j,k,l) * J(s,l) */
      for(r = 0 ; r < dim_h ; r++) for(i = 0 ; i < dim ; i++) for(k = 0 ; k < dim ; k++) for(s = 0 ; s < dim_h ; s++) {
        jcj[r][i][k][s] = zero ;
        for(j = 0 ; j < dim ; j++) for(l = 0 ; l < dim ; l++) {
          jcj[r][i][k][s] += CAJ(r,j) * C(i,j,k,l) * CAJ(s,l) ;
        }
      }
    
      /* KR(j,i,l,k) = DH(j,r) * JCJ(r,i,k,s) * DH(l,s) */
      for(j = 0 ; j < nn ; j++) for(i = 0 ; i < dim ; i++) for(l = 0 ; l < nn ; l++) for(k = 0 ; k < dim ; k++) {
        for(r = 0 ; r < dim_h ; r++) for(s = 0 ; s < dim_h ; s++) {
          KR(j*dim+i,l*dim+k) += a * DH(j,r) * jcj[r][i][k][s] * DH(l,s) ;
        }
      }
    
      /* Axisymmmetrical case: 3 terms */
      if(Symmetry_IsCylindrical(sym)) {
        /* 1.a JC(r,i) = J(r,j) * C(i,j,theta,theta) */
        for(r = 0 ; r < dim_h ; r++) for(i = 0 ; i < dim ; i++) {
          jc[r][i] = zero ;
          for(j = 0 ; j < dim ; j++) jc[r][i] += CAJ(r,j) * C(i,j,2,2) ;
        }
        
        /* 1.b KR(j,i,l,0) = DH(j,r) * JC(r,i) * H(l)/r */
        for(j = 0 ; j < nn ; j++) for(i = 0 ; i < dim ; i++) for(l = 0 ; l < nn ; l++) for(r = 0 ; r < dim_h ; r++) {
          KR(j*dim+i,l*dim) += a * DH(j,r) * jc[r][i] * h[l]/radius ;
        }
        
        /* 2.a CJ(k,s) = C(theta,theta,k,l) * J(s,l) */
        for(k = 0 ; k < dim ; k++) for(s = 0 ; s < dim_h ; s++) {
          cj[k][s] = zero ; 
          for(l = 0 ; l < dim ; l++) cj[k][s] += C(2,2,k,l) * CAJ(s,l) ;
        }
        
        /* 2.b KR(j,0,l,k) = H(j)/r * CJ(k,s) * DH(l,s) */
        for(j = 0 ; j < nn ; j++) for(l = 0 ; l < nn;l++) for(k = 0 ; k < dim ; k++) for(s = 0 ; s < dim_h ; s++) {
          KR(j*dim,l*dim+k) += a * h[j]/radius * cj[k][s] * DH(l,s) ;
        }
        
        /* 3.  KR(j,0,l,0) = H(j)/r * C(theta,theta,theta,theta) * H(l)/r */
        for(j = 0 ; j < nn ; j++) for(l = 0 ; l < nn ; l++) {
          KR(j*dim,l*dim) += a * h[j]/radius * C(2,2,2,2) * h[l]/radius ;
        }
      
      /* Spherical case: 3 terms */
      } else if(Symmetry_IsSpherical(sym)) {
        
        /* 1.a JC(r,i) = J(r,j) * (C(i,j,theta,theta) + C(i,j,phi,phi)) */
        for(r = 0 ; r < dim_h ; r++) for(i = 0 ; i < dim ; i++) {
          jc[r][i] = zero ;
          for(j = 0 ; j < dim ; j++) jc[r][i] += CAJ(r,j) * (C(i,j,1,1) + C(i,j,2,2)) ;
        }
        
        /* 1.b KR(j,i,l,0) = DH(j,r) * JC(r,i) * H(l)/r */
        for(j = 0 ; j < nn ; j++) for(i = 0 ; i < dim ; i++) for(l = 0 ; l < nn ; l++) for(r = 0 ; r < dim_h ; r++) {
          KR(j*dim+i,l*dim) += a * DH(j,r) * jc[r][i] * h[l]/radius ;
        }
        
        /* 2.a CJ(k,s) = (C(phi,phi,k,l) + C(theta,theta,k,l)) * J(s,l) */
        for(k = 0 ; k < dim ; k++) for(s = 0 ; s < dim_h ; s++) {
          cj[k][s] = zero ; 
          for(l = 0 ; l < dim ; l++) cj[k][s] += (C(1,1,k,l) + C(2,2,k,l)) * CAJ(s,l) ;
        }
        
        /* 2.b KR(j,0,l,k) = H(j)/r * CJ(k,s) * DH(l,s) */
        for(j = 0 ; j < nn ; j++) for(l = 0 ; l < nn ; l++) for(k = 0 ; k < dim ; k++) for(s = 0 ; s < dim_h ; s++) {
          KR(j*dim,l*dim+k) += a * h[j]/radius * cj[k][s] * DH(l,s) ;
        }
        
        /* 3.  KR(j,0,l,0) = H(j)/r * (C(phi,phi,phi,phi) + C(theta,theta,phi,phi) + C(phi,phi,theta,theta) + C(theta,theta,theta,theta)) * H(l)/r */
        {
          double cc = C(1,1,1,1) + C(1,1,2,2) + C(2,2,1,1) + C(2,2,2,2) ;
          
          for(j = 0 ; j < nn ; j++) for(l = 0 ; l < nn ; l++) {
            KR(j*dim,l*dim) += a * h[j]/radius * cc * h[l]/radius ;
          }
        }
      }
    }
  }
  
  return(kr) ;
#undef KR
#undef DH
#undef C
#undef CAJ
}



_INLINE_ double*  (FEM_ComputeBiotMatrix)(FEM_t* fem,IntFct_t* fi,const double* c,const int dec)
/** Return a pointer on a Biot-like coupling matrix (Ndof*N) 
 *  with Ndof = N * dim and N = nb of nodes */
{
#define KC(i,j)     (kc[(i)*nn + (j)])
#define DH(n,i)     (dh[(n)*3 + (i)])
//#define DH(n,i)     (dh[(n)*dim + (i)])
#define C(i,j)      (c[(i)*3 + (j)])
#define CAJ(i,j)    (caj[(i)*3 + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int dim_e = Element_GetDimension(el) ;
  int nn  = Element_GetNbOfNodes(el) ;
  int dim_h = IntFct_GetDimension(fi) ;
  int np  = IntFct_GetNbOfPoints(fi) ;
  int nf  = IntFct_GetNbOfFunctions(fi) ;
  double* weight = IntFct_GetWeight(fi) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  int nrow = nn*dim ;
  int ncol = nn ;
  size_t SizeNeeded = nrow*ncol*(sizeof(double)) ;
  double* kc = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  double* x[Element_MaxNbOfNodes] ;
  double zero = 0. ;
  
  if(Element_IsSubmanifold(el)) {
    arret("FEM_ComputeBiotMatrix") ;
  }
  
  /* Initialisation */
  {
    int i ;
    
    for(i = 0 ; i < nrow*ncol ; i++) kc[i] = zero ;
  }
  
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      x[i] = Element_GetNodeCoordinate(el,i) ;
    }
  }
  
  /* One node mesh: for this special element the displacement u_i stands for strain_ii */
  if(nn == 1 && dim_e == 3) {
    int i ;
    
    for(i = 0 ; i < dim ; i++) {
      kc[i] = C(i,i) ;
    }
    
    return(kc) ;
  }
  

  /* Interface elements */
  if(Element_HasZeroThickness(el)) {
    if(dim_h == dim - 1) {
      /* Assuming that the numbering of the element is formed with the nf first nodes  */
      if(nn > nf) {
        int p ;
      
        /* Check the numbering */
        Element_CheckNumberingOfOverlappingNodes(el,nf) ;

        for(p = 0 ; p < np ; p++ , c += dec) {
          double* h  = IntFct_GetFunctionAtPoint(fi,p) ;
          double* dh = IntFct_GetFunctionGradientAtPoint(fi,p) ;
          double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
          double a   = weight[p]*d ;
          double* norm = Element_ComputeNormalVector(el,dh,nf,dim_h) ;
          double r[3] = {0,0,0} ;
          int i,j,l ;

          if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
            double radius = zero ;
            
            for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
            
            a *= 2*M_PI*radius ;
            
            if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
          }
      
          /* 
           * Pay attention to the orientation of the normal:
           * the normal N is oriented from the opposite face
           * to the main face of the element, i.e. from ii to i */
          #define R(i)     (r[i])
          #define NORM(i)  (norm[i])
        
          for(i = 0 ; i < 3 ; i++) {
            R(i) = 0 ;
            for(j = 0 ; j < 3 ; j++) {
              R(i) += C(i,j) * NORM(j) ;
            }
          }
        
          #undef NORM

          /* KR(j,i,l) = H(j) * R(i) * H(l) */
          for(j = 0 ; j < nf ; j++) {
            int jj = Element_OverlappingNode(el,j) ;
            
            for(i = 0 ; i < dim ; i++) {
              for(l = 0 ; l < nf ; l++) {
                double k_jil = a * h[j] * R(i) * h[l] ;
                int ll = Element_OverlappingNode(el,l) ;
                
                /* The continity of pressure is assumed so that p(l) = p(ll).
                 * For generality we consider that p = 0.5 * (p(l) + p(ll))
                 * along the element. */
                KC(j*dim  + i,l ) +=   0.5 * k_jil ;
                KC(jj*dim + i,l ) += - 0.5 * k_jil ;
                KC(j*dim  + i,ll) +=   0.5 * k_jil ;
                KC(jj*dim + i,ll) += - 0.5 * k_jil ;
              }
            }
          }

          #undef R
        }
      
        return(kc) ;
      }
    }
  }
  
  /* boucle sur les points d'integration */
  {
    int p ;
    
    for(p = 0 ; p < np ; p++ , c += dec) {
      double* h  = IntFct_GetFunctionAtPoint(fi,p) ;
      double* dh = IntFct_GetFunctionGradientAtPoint(fi,p) ;
      double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
      double a   = weight[p]*d ;
      double* caj = Element_ComputeInverseJacobianMatrix(el,dh,nf,dim_h) ;
      double jc[3][3] ;
      double radius ;
      int    i,j,k,l ;
    
      /* cas axisymetrique ou spherique */
      if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
        radius = zero ;
        for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
        a *= 2*M_PI*radius ;
        if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
      }
    
      /* JC(k,i) = J(k,j)*C(j,i) */
      for(k = 0 ; k < dim_h ; k++) for(i = 0 ; i < dim ; i++) {
        jc[k][i] = zero ;
        for(j = 0 ; j < dim ; j++) jc[k][i] += CAJ(k,j)*C(j,i) ;
      }
      /* KC(j,i,l) = DH(j,k)*JC(k,i)*H(l) */
      for(j = 0 ; j < nf ; j++) for(i = 0 ; i < dim ; i++) for(l = 0 ; l < nf ; l++) for(k = 0 ; k < dim_h ; k++) {
        KC(j*dim+i,l) += a*DH(j,k)*jc[k][i]*h[l] ;
      }
      /* cas axisymetrique: (r,z,theta) */
      if(Symmetry_IsCylindrical(sym)) {
        /* KC(j,0,l) = H(j)/r*C(theta,theta)*H(l) */
        for(j = 0 ; j < nf ; j++) for(l = 0 ; l < nf ; l++) {
          KC(j*dim,l) += a*h[j]/radius*C(2,2)*h[l] ;
        }
      /* cas spherique: (r,theta,phi) */
      } else if(Symmetry_IsSpherical(sym)) {
        /* KC(j,0,l) = H(j)/r*(C(theta,theta)+C(phi,phi))*H(l) */
        for(j = 0 ; j < nf ; j++) for(l = 0 ; l < nf ; l++) {
          KC(j*dim,l) += a*h[j]/radius*(C(1,1)+C(2,2))*h[l] ;
        }
      }
    }
  }
  
  return(kc) ;
  
#undef KC
#undef DH
#undef C
#undef CAJ
}




_INLINE_ double* (FEM_ComputeMassMatrix)(FEM_t* fem,IntFct_t* fi,const double* c,const int dec)
/** Return a pointer on a FE mass matrix (N*N) 
 *  with N = nb of nodes*/
{
#define KM(i,j)     (km[(i)*nn + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int dim_e = Element_GetDimension(el) ;
  int nn  = Element_GetNbOfNodes(el) ;
  int np  = IntFct_GetNbOfPoints(fi) ;
  int nf  = IntFct_GetNbOfFunctions(fi) ;
  int dim_h = IntFct_GetDimension(fi) ;
  double* weight = IntFct_GetWeight(fi) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  size_t SizeNeeded = nn*nn*(sizeof(double)) ;
  double* km = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  double* x[Element_MaxNbOfNodes] ;
  
  /* Initialization */
  {
    int i ;
    
    for(i = 0 ; i < nn*nn ; i++) {
      km[i] = 0 ;
    }
  }
  
  //if(Element_IsSubmanifold(el)) arret("FEM_ComputeMassMatrix") ;

  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      x[i] = Element_GetNodeCoordinate(el,i) ;
    }
  }
  
  
  /* One node mesh */
  if(nn == 1 && dim_e == 3) {
    KM(0,0) = c[0] ;
      
    return(km) ;
  }


  /* Interface elements */
  if(Element_HasZeroThickness(el)) {
    if(dim_h == dim - 1) {
      /* Assuming that the numbering of the element is formed with the nf first nodes  */
      if(nn > nf) {
        int p ;
      
        /* Check the numbering */
        Element_CheckNumberingOfOverlappingNodes(el,nf) ;
    
        for(p = 0 ; p < np ; p++ , c += dec) {
          double* h  = IntFct_GetFunctionAtPoint(fi,p) ;
          double* dh = IntFct_GetFunctionGradientAtPoint(fi,p) ;
          double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
          double a   = weight[p]*c[0]*d ;
          int    i ;
    
          /* axisymetrical or spherical cases */
          if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
            double radius = 0 ;
        
            for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
            
            a *= 2*M_PI*radius ;
            if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
          }
    
          for(i = 0 ; i < nf ; i++) {
            int ii = Element_OverlappingNode(el,i) ;
            int j ;
        
            for(j = 0 ; j < nf ; j++) {
              int jj = Element_OverlappingNode(el,j) ;
              double kij = a * h[i] * h[j] ;
                
              /* The continity of pressure is assumed so that p(i) = p(ii)
               * (and p(j) = p(jj)).
               * For generality we consider that p = 0.5 * (p(i) + p(ii))
               * along the element. */
              
              KM(i ,j ) += 0.25 * kij ;
              KM(ii,j ) += 0.25 * kij ;
              KM(i ,jj) += 0.25 * kij ;
              KM(ii,jj) += 0.25 * kij ;
            }
          }
        }
      
        return(km) ;
      }
    }
  }
  
  
  /* 0D */
  if(dim_h == 0) {
    if(nn == 1) {
      double radius = x[0][0] ;
      
      KM(0,0) = c[0] ;
      
      if(Symmetry_IsCylindrical(sym)) KM(0,0) *= 2*M_PI*radius ;
      else if(Symmetry_IsSpherical(sym)) KM(0,0) *= 4*M_PI*radius*radius ;
    } else {
      arret("FEM_ComputeMassMatrix: impossible") ;
    }
    return(km) ;
  }

  /* Regular element */
  {
    int p ;
    
    for(p = 0 ; p < np ; p++ , c += dec) {
      double* h  = IntFct_GetFunctionAtPoint(fi,p) ;
      double* dh = IntFct_GetFunctionGradientAtPoint(fi,p) ;
      double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
      double a   = weight[p]*c[0]*d ;
      int    i ;
    
      /* axisymetrical or spherical cases */
      if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
        double radius = 0 ;
        
        for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
        
        a *= 2*M_PI*radius ;
        if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
      }
    
      for(i = 0 ; i < nf ; i++) {
        int j ;
        
        for(j = 0 ; j < nf ; j++) {
          KM(i,j) += a*h[i]*h[j] ;
        }
      }
    }
  }
  
  return(km) ;
  
#undef KM
}



_INLINE_ double* (FEM_ComputeMassMatrix)(FEM_t* fem,IntFct_t* fi,const double* c,const int dec,int const ndif)
{
#define KM(i,j)     (km[(i)*ndof + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  int nn  = Element_GetNbOfNodes(el) ;
  int ndof = nn*ndif;
  size_t SizeNeeded = ndof*ndof*(sizeof(double)) ;
  double* km = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  
  for(int i = 0 ; i < ndof*ndof ; i++) km[i] = 0;
  
  for(int idif = 0 ; idif < ndif ; idif++) {
    for(int jdif = 0 ; jdif < ndif ; jdif++) {
      double const* cij = c + (idif*ndif + jdif);
      double* kij = FEM_ComputeMassMatrix(fem,fi,cij,dec);

      for(int i = 0 ; i < nn ; i++) {      
        for(int j = 0 ; j < nn ; j++) {
          KM(idif + i*ndif,jdif + j*ndif) += kij[i*nn + j];
        }
      }
      
      FEM_FreeBufferFrom(fem,kij);
    }
  }
  
  return(km);

#undef KM
}





_INLINE_ double*  (FEM_ComputeConductionMatrix)(FEM_t* fem,IntFct_t* fi,const double* c,const int dec)
/** Return a pointer on a FE conduction matrix (N*N) 
 *  with N = nb of nodes */
{
#define KC(i,j)     (kc[(i)*nn + (j)])
#define DH(n,i)     (dh[(n)*3 + (i)])
//#define DH(n,i)     (dh[(n)*dim_h + (i)])
#define C(i,j)      (c[(i)*3 + (j)])
#define CAJ(i,j)    (caj[(i)*3 + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int dim_e = Element_GetDimension(el) ;
  int nn  = Element_GetNbOfNodes(el) ;
  int np  = IntFct_GetNbOfPoints(fi) ;
  int nf  = IntFct_GetNbOfFunctions(fi) ;
  int dim_h = IntFct_GetDimension(fi) ;
  double* weight = IntFct_GetWeight(fi) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  size_t SizeNeeded = nn*nn*(sizeof(double)) ;
  double* kc = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  double* x[Element_MaxNbOfNodes] ;
  
  /* Initialization */
  {
    int i ;
    
    for(i = 0 ; i < nn*nn ; i++) {
      kc[i] = 0 ;
    }
  }
  
  //if(Element_IsSubmanifold(el)) arret("FEM_ComputeConductionMatrix") ;
  
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      x[i] = Element_GetNodeCoordinate(el,i) ;
    }
  }
  
  
  
  /* one node mesh: the unknown stands for the gradient along x-axis */
  if(nn == 1 && dim_e == 3) {
    KC(0,0) = C(0,0) ;
      
    return(kc) ;
  }
  


  /* Interface elements */
  if(Element_HasZeroThickness(el)) {
    if(dim_h == dim - 1) {
      /* Assuming that the numbering of the element is formed with the nf first nodes  */
      if(nn > nf) {
        int p ;
      
        /* Check the numbering */
        Element_CheckNumberingOfOverlappingNodes(el,nf) ;
    
        for(p = 0 ; p < np ; p++ , c += dec) {
          double* h  = IntFct_GetFunctionAtPoint(fi,p) ;
          double* dh = IntFct_GetFunctionGradientAtPoint(fi,p) ;
          double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
          double a   = weight[p]*d ;
          double* caj = Element_ComputeInverseJacobianMatrix(el,dh,nf,dim_h) ;
          double jcj[3][3] ;
          int    i,j,k,l ;
    
          /* axisymetrical or spherical cases */
          if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
            double radius = 0 ;
        
            for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
            
            a *= 2*M_PI*radius ;
            
            if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
          }
    
          /* jcj = J(i,k)*C(k,l)*J(j,l) */
          for(i = 0 ; i < dim_h ; i++) for(j = 0 ; j < dim_h ; j++) {
            jcj[i][j] = 0 ;
            for(k = 0 ; k < dim ; k++) for(l = 0 ; l < dim ; l++)  {
              jcj[i][j] += CAJ(i,k)*C(k,l)*CAJ(j,l) ;
            }
          }
      
          /* KC(i,j) = DH(i,k)*JCJ(k,l)*DH(j,l) */
          for(i = 0 ; i < nf ; i++) {
            int ii = Element_OverlappingNode(el,i) ;
            
            for(j = 0 ; j < nf ; j++) {
              int jj = Element_OverlappingNode(el,j) ;
            
              for(k = 0 ; k < dim_h ; k++) for(l = 0 ; l < dim_h ; l++) {
                double kcij = a * DH(i,k) * jcj[k][l] * DH(j,l) ;
                
                KC(i ,j ) += 0.25 * kcij ;
                KC(ii,j ) += 0.25 * kcij ;
                KC(i ,jj) += 0.25 * kcij ;
                KC(ii,jj) += 0.25 * kcij ;
              }
            }
          }
        }
      
        return(kc) ;
      }
    }
  }
  
  /* Loop on integration points */
  {
    int p ;
    
    for(p = 0 ; p < np ; p++ , c += dec) {
      double* h  = IntFct_GetFunctionAtPoint(fi,p) ;
      double* dh = IntFct_GetFunctionGradientAtPoint(fi,p) ;
      double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
      double a   = weight[p]*d ;
      double* caj = Element_ComputeInverseJacobianMatrix(el,dh,nf,dim_h) ;
      double jcj[3][3] ;
      int    i,j,k,l ;
    
      /* axisymetrical or spherical cases */
      if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
        double radius = 0 ;
        
        for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
        
        a *= 2*M_PI*radius ;
        if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
      }
    
      /* jcj = J(i,k)*C(k,l)*J(j,l) */
      for(i = 0 ; i < dim_h ; i++) for(j = 0 ; j < dim_h ; j++) {
        jcj[i][j] = 0 ;
        for(k = 0 ; k < dim ; k++) for(l = 0 ; l < dim ; l++)  {
          jcj[i][j] += CAJ(i,k)*C(k,l)*CAJ(j,l) ;
        }
      }
      
      /* KC(i,j) = DH(i,k)*JCJ(k,l)*DH(j,l) */
      for(i = 0 ; i < nf ; i++) for(j = 0 ; j < nf ; j++) {
        for(k = 0 ; k < dim_h ; k++) for(l = 0 ; l < dim_h ; l++) {
          KC(i,j) += a * DH(i,k) * jcj[k][l] * DH(j,l) ;
        }
      }
    }
  }
  
  return(kc) ;

#undef KC
#undef DH
#undef C
#undef CAJ
}



_INLINE_ double* (FEM_ComputeConductionMatrix)(FEM_t* fem,IntFct_t* fi,const double* c,const int dec,int const ndif)
{
#define KC(i,j)     (kc[(i)*ndof + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  int nn  = Element_GetNbOfNodes(el) ;
  int ndof = nn*ndif;
  size_t SizeNeeded = ndof*ndof*(sizeof(double)) ;
  double* kc = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  
  for(int i = 0 ; i < ndof*ndof ; i++) kc[i] = 0;
  
  for(int idif = 0 ; idif < ndif ; idif++) {
    for(int jdif = 0 ; jdif < ndif ; jdif++) {
      double const* cij = c + 9*(idif*ndif + jdif);
      double* kij = FEM_ComputeConductionMatrix(fem,fi,cij,dec);

      for(int i = 0 ; i < nn ; i++) {      
        for(int j = 0 ; j < nn ; j++) {
          KC(idif + i*ndif,jdif + j*ndif) += kij[i*nn + j];
        }
      }
      
      FEM_FreeBufferFrom(fem,kij);
    }
  }
  
  return(kc);

#undef KC
}




_INLINE_ double*  (FEM_ComputePoroelasticMatrix6)(FEM_t* fem,IntFct_t* fi,const double* c,const int dec,const int n_dif,const int idis)
/** Return a pointer on a FE poroelastic matrix (Ndof x Ndof).
 * 
 *  Ndof = nb of degrees of freedom (= NN*Neq)
 *  NN   = nb of nodes 
 *  Neq  = nb of equations (= Dim + n_dif)
 *  Dim  = dimension of the problem
 * 
 *  On inputs:
 * 
 *  n_dif = nb of Biot-like coupling terms (pressure, temperature, etc...)
 * 
 *  idis = position index:
 *         of the first component of the displacement in the output matrix k.
 * 
 *  c = the entry matrix which should be given in the following order:
 * 
 *  K0 to Kn then A0 to An etc.. with n = n_dif:
 * 
 *  | K0(9x9) K1(9x1) K2(9x1) ... | 
 *  | A0(1x9) A1(1x1) A2(1x1) ... | 
 *  | B0(1x9) B1(1x1) B2(1x1) ... | 
 *  | ........................... |
 * 
 *   K0    = Stiffness matrix
 *   Kn    = Mechanic-hydraulic coupling terms
 *   A0,B0 = Hydraulic-mechanic coupling terms
 *   Ai,Bj = Hydraulic terms
 * 
 *  On outputs:
 *  the FE matrix k is provided in an order depending on "idis".
 *  The first displacement unknown U1 will be positionned at "idis".
 * 
 *  Below: I=idis , J=idis+D , N=n_dif+D , D=Dim
 * 
 *           0      ...  I      ...  J     ...   N
 *           |           |           |           |
 *           v           v           v           v
 *        | A0(1x1) ... AI(1xD) ... AJ(1x1) ... AN(1x1) | P1
 *        | ........................................... | .
 *  Knm = | K0(Dx1) ... KI(DxD) ... KJ(Dx1) ... KN(Dx1) | U(D)
 *        | B0(1x1) ... BI(1xD) ... BJ(1x1) ... BN(1x1) | PJ
 *        | ..........................................  | .
 *        | Z0(1x1) ... ZI(1xD) ... ZJ(1x1) ... ZN(1x1) | PN
 * 
 *        | K11 ... K1n |
 *  k   = | ........... |  n = NN = nb of nodes
 *        | Kn1 ... Knn |
 */
{
#define K(i,j)      (k[(i)*ndof + (j)])
#define E_Mec       indmec
#define I_U         E_Mec
#define E_Hyd(i)    (((i) < E_Mec) ? (i) : dim + (i))
#define I_H(i)      E_Hyd(i)
  int indmec = ((idis < 0) ? 0 : ((idis > n_dif) ? n_dif : idis)) ;
  Element_t* el = FEM_GetElement(fem) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int nn  = Element_GetNbOfNodes(el) ;
  int neq = dim + n_dif ;
  int ndof = nn*neq ;
  size_t SizeNeeded = ndof*ndof*(sizeof(double)) ;
  double* k = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  int    i,n,m ;
  double zero = 0. ;
  
  /* Initialization */
  for(i = 0 ; i < ndof*ndof ; i++) k[i] = zero ;
  

  /* 
  ** 1.  Mechanics
  */
  /* 1.1 Elasticity */
  {
    double* kr = FEM_ComputeStiffnessMatrix(fem,fi,c,dec) ;
    
    #define KR(i,j)     (kr[(i)*nn*dim + (j)])
    for(n = 0 ; n < nn ; n++) for(m = 0 ; m < nn ; m++) {
      int j ;
      
      for(i = 0 ; i < dim ; i++) for(j = 0 ; j < dim ; j++) {
        K(E_Mec + i + n*neq,I_U + j + m*neq) = KR(i + n*dim,j + m*dim) ;
      }
    }
    #undef KR
    
    FEM_FreeBufferFrom(fem,kr);
  }
  
  /* 1.2 Biot-like coupling terms */
  {
    for(i = 0 ; i < n_dif ; i++) {
      int I_h = I_H(i) ;
      const double* c1 = c + 81 + i*9 ;
      double* kb = FEM_ComputeBiotMatrix(fem,fi,c1,dec) ;
    
      #define KB(i,j)     (kb[(i)*nn + (j)])
      for(n = 0 ; n < nn ; n++) for(m = 0 ; m < nn ; m++) {
        int j ;
      
        for(j = 0 ; j < dim ; j++) {
          K(E_Mec + j + n*neq,I_h + m*neq) = KB(j + n*dim,m) ;
        }
      }
      #undef KB
      
      FEM_FreeBufferFrom(fem,kb);
    }
  }
  

  /* 2. Hydraulic equations */
  {
    for(i = 0 ; i < n_dif ; i++) {
      int E_h = E_Hyd(i) ;
      int j ;
    
      /* 2.1 Coupling terms */
      {
        const double* c1 = c + 81 + n_dif*9 + i*(9 + n_dif) ;
        double* kb = FEM_ComputeBiotMatrix(fem,fi,c1,dec) ;
    
        #define KB(i,j)     (kb[(i)*nn + (j)])
        for(n = 0 ; n < nn ; n++) for(m = 0 ; m < nn ; m++) {
          for(j = 0 ; j < dim ; j++) {
            K(E_h + m*neq,I_U + j + n*neq) = KB(j + n*dim,m) ;
          }
        }
        #undef KB
      
        FEM_FreeBufferFrom(fem,kb);
      }
    
      /* 2.2 Accumulations */
      {
        for(j = 0 ; j < n_dif ; j++) {
          int I_h = I_H(j) ;
          const double* c2 = c + 81 + n_dif*9 + i*(9 + n_dif) + 9 + j ;
          double* ka = FEM_ComputeMassMatrix(fem,fi,c2,dec) ;
    
          #define KA(i,j)     (ka[(i)*nn + (j)])
          for(n = 0 ; n < nn ; n++) for(m = 0 ; m < nn ; m++) {
            K(E_h + n*neq,I_h + m*neq) = KA(n,m) ;
          }
          #undef KA
      
          FEM_FreeBufferFrom(fem,ka);
        }
      }
    }
  }
  
  return(k) ;

#undef K
#undef E_Mec
#undef I_U
#undef E_Hyd
#undef I_H
}



_INLINE_ double*  (FEM_ComputePoroelasticMatrixBis)(FEM_t* fem,IntFct_t* fi,const double* c,const int dec,const int n_dif,const int idis)
/** Return a pointer on a FE poroelastic matrix (Ndof x Ndof).
 * 
 *  Ndof = nb of degrees of freedom (= NN*Neq)
 *  NN   = nb of nodes 
 *  Neq  = nb of equations (= Dim + n_dif)
 *  Dim  = dimension of the problem
 * 
 *  On inputs:
 * 
 *  n_dif = nb of Biot-like coupling terms (pressure, temperature, etc...)
 * 
 *  idis = position index:
 *         of the first component of the displacement in the output matrix k.
 *         and
 *         of the first component of the strain tensor in the entry matrix c.
 * 
 *  c = the entry matrix which should be given in the order depending on idis:
 * 
 *  Below: I=idis , J=idis+9 , N=n_dif+9
 * 
 *         0      ...  I      ...  J     ...   N
 *         |           |           |           |
 *         v           v           v           v
 *      | A0(1x1) ... AI(1x9) ... AJ(1x1) ... AN(1x1) | P1
 *      | ........................................... | .
 *  c = | K0(9x1) ... KI(9x9) ... KJ(9x1) ... KN(9x1) | Strain tensor(9)
 *      | B0(1x1) ... BI(1x9) ... BJ(1x1) ... BN(1x1) | PJ
 *      | ..........................................  | .
 *      | Z0(1x1) ... ZI(1x9) ... ZJ(1x1) ... ZN(1x1) | PN
 * 
 *   KI    = Stiffness matrix
 *   Kn    = Mechanic-hydraulic coupling terms
 *   AI,BI,ZI = Hydraulic-mechanic coupling terms
 *   An,Bn,Zn = Hydraulic terms
 * 
 *  On outputs (same as in FEM_ComputePoroelasticMatrix6):
 *  the FE matrix k is provided in an order depending on "idis".
 *  The first displacement unknown U1 will be positionned at "idis".
 * 
 *  Below: I=idis , J=idis+D , N=n_dif+D , D=Dim
 * 
 *           0      ...  I      ...  J     ...   N
 *           |           |           |           |
 *           v           v           v           v
 *        | A0(1x1) ... AI(1xD) ... AJ(1x1) ... AN(1x1) | P1
 *        | ........................................... | .
 *  Knm = | K0(Dx1) ... KI(DxD) ... KJ(Dx1) ... KN(Dx1) | U(D)
 *        | B0(1x1) ... BI(1xD) ... BJ(1x1) ... BN(1x1) | PJ
 *        | ..........................................  | .
 *        | Z0(1x1) ... ZI(1xD) ... ZJ(1x1) ... ZN(1x1) | PN
 * 
 *        | K11 ... K1n |
 *  k   = | ........... |  n = NN = nb of nodes
 *        | Kn1 ... Knn |
 */
{
#define K(i,j)      (k[(i)*ndof + (j)])
#define E_Mec       indmec
#define I_U         E_Mec
#define E_Hyd(i)    (((i) < E_Mec) ? (i) : dim + (i))
#define I_H(i)      E_Hyd(i)
  int indmec = ((idis < 0) ? 0 : ((idis > n_dif) ? n_dif : idis)) ;
  Element_t* el = FEM_GetElement(fem) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int nn  = Element_GetNbOfNodes(el) ;
  int neq = dim + n_dif ;
  int ndof = nn*neq ;
  size_t SizeNeeded = ndof*ndof*(sizeof(double)) ;
  double* k = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  int    i,n,m ;
  double zero = 0. ;
  
  /* Initialization */
  for(i = 0 ; i < ndof*ndof ; i++) k[i] = zero ;
  

  /* 
  ** 1.  Mechanics
  */
  /* 1.1 Elasticity: KI */
  {
    const double* c0 = c + idis*(n_dif + 9) + idis*9 ;
    double* kr = FEM_ComputeStiffnessMatrix(fem,fi,c0,dec) ;
    
    #define KR(i,j)     (kr[(i)*nn*dim + (j)])
    for(n = 0 ; n < nn ; n++) for(m = 0 ; m < nn ; m++) {
      int j ;
      
      for(i = 0 ; i < dim ; i++) for(j = 0 ; j < dim ; j++) {
        K(E_Mec + i + n*neq,I_U + j + m*neq) = KR(i + n*dim,j + m*dim) ;
      }
    }
    #undef KR
    
    FEM_FreeBufferFrom(fem,kr);
  }
  
  /* 1.2 Mechanic-hydraulic coupling terms (Biot-like): Kn */
  {
    for(i = 0 ; i < n_dif ; i++) {
      int I_h = I_H(i) ;
      int ki = (i < idis) ? 0 : 81 ; 
      const double* c1 = c + idis*(n_dif + 9) + i*9 + ki ;
      //const double* c1 = c + 81 + i*9 ;
      double* kb = FEM_ComputeBiotMatrix(fem,fi,c1,dec) ;
    
      #define KB(i,j)     (kb[(i)*nn + (j)])
      for(n = 0 ; n < nn ; n++) for(m = 0 ; m < nn ; m++) {
        int j ;
      
        for(j = 0 ; j < dim ; j++) {
          K(E_Mec + j + n*neq,I_h + m*neq) = KB(j + n*dim,m) ;
        }
      }
      #undef KB
    
      FEM_FreeBufferFrom(fem,kb);
    }
  }
  

  /* 2. Hydraulic equations */
  {
    for(i = 0 ; i < n_dif ; i++) {
      int E_h = E_Hyd(i) ;
      int j ;
    
      /* 2.1 Hydraulic-mechanics coupling terms: AI..BI..ZI */
      {
        int ki = (i < idis) ? 0 : 81 + n_dif*9 ;
        const double* c1 = c + ki + i*(9 + n_dif) + idis ;
        //const double* c1 = c + 81 + n_dif*9 + i*(9 + n_dif) ;
        double* kb = FEM_ComputeBiotMatrix(fem,fi,c1,dec) ;
    
        #define KB(i,j)     (kb[(i)*nn + (j)])
        for(n = 0 ; n < nn ; n++) for(m = 0 ; m < nn ; m++) {
          for(j = 0 ; j < dim ; j++) {
            K(E_h + m*neq,I_U + j + n*neq) = KB(j + n*dim,m) ;
          }
        }
        #undef KB
    
        FEM_FreeBufferFrom(fem,kb);
      }
    
      /* 2.2 Accumulations */
      {
        for(j = 0 ; j < n_dif ; j++) {
          int I_h = I_H(j) ;
          int ki = (i < idis) ? 0 : 81 + n_dif*9 ;
          int kj = (j < idis) ? 0 : 9 ;
          const double* c2 = c + ki + i*(9 + n_dif) + kj + j ;
          //const double* c2 = c + 81 + n_dif*9 + i*(9 + n_dif) + 9 + j ;
          double* ka = FEM_ComputeMassMatrix(fem,fi,c2,dec) ;
    
          #define KA(i,j)     (ka[(i)*nn + (j)])
          for(n = 0 ; n < nn ; n++) for(m = 0 ; m < nn ; m++) {
            K(E_h + n*neq,I_h + m*neq) = KA(n,m) ;
          }
          #undef KA
    
          FEM_FreeBufferFrom(fem,ka);
        }
      }
    }
  }
  
  return(k) ;

#undef K
#undef E_Mec
#undef I_U
#undef E_Hyd
#undef I_H
}



_INLINE_ double* FEM_AssemblePoroelasticAndConductionMatrices(FEM_t* fem,int const ndif,int const idis,double const* kc,double* k) {
  Element_t* el = FEM_GetElement(fem) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int nn  = Element_GetNbOfNodes(el) ;
  int neq = dim + ndif ;
  int ndof = nn*neq ;
    
  #define K(i,j)    (k[(i)*ndof + (j)])
  #define KC(i,j)   (kc[(i)*nn*ndif + (j)])    
  {
    int e_mech = ((idis < 0) ? 0 : ((idis > ndif) ? ndif : idis)) ;
      
    #define E_Hyd(i)    (((i) < e_mech) ? (i) : dim + (i))
    #define I_Hyd(i)    E_Hyd(i)
    for(int i = 0 ; i < ndif ; i++) {
      int e_i = E_Hyd(i) ;
        
      for(int j = 0 ; j < ndif ; j++) {
        int u_j = I_Hyd(j) ;
          
        for(int n = 0 ; n < nn ; n++) {      
          for(int m = 0 ; m < nn ; m++) {
              
            K(e_i + n*neq,u_j + m*neq) += KC(i + n*ndif,j + m*ndif) ;
          }
        }
      }
    }
    #undef E_Hyd
    #undef I_Hyd
  }
  #undef KC
  #undef K
    
  return(k);
}



_INLINE_ void (FEM_TransformMatrixFromDegree2IntoDegree1)(FEM_t* fem,const int inc,const int equ,double* k)
{
#define K(i,j)     (k[(i)*nn*neq+(j)])
  Element_t* el = FEM_GetElement(fem) ;
  int    nn = Element_GetNbOfNodes(el) ;
  int    dim = Element_GetDimension(el) ;
  int    neq = Element_GetNbOfEquations(el) ;
  int    n_vertices = 0 ;
  int    edge_vertices_line[2]       = {0,1} ;
  int    edge_vertices_triangle[6]   = {0,1,1,2,2,0} ;
  int    edge_vertices_quadrangle[8] = {0,1,1,2,2,3,3,0} ;
  int    *edge_vertices[Element_MaxNbOfNodes] ;
  int    ilin,icol ;
  int    n ;

  if(nn > Element_MaxNbOfNodes) arret("FEM_TransformMatrixFromDegree2IntoDegree1") ;

  if(dim == 1) {
    if(nn == 3) {
      n_vertices = 2 ;
      edge_vertices[n_vertices] = edge_vertices_line ;
    } else return ;
  } else if(dim == 2) {
    if(nn == 6) {
      n_vertices = 3 ;
      edge_vertices[n_vertices] = edge_vertices_triangle ;
    } else if(nn == 8) {
      n_vertices = 4 ;
      edge_vertices[n_vertices] = edge_vertices_quadrangle ;
    } else return ;
  } else if(dim == 3) {
    arret("FEM_TransformMatrixFromDegree2IntoDegree1") ;
  } else {
    arret("FEM_TransformMatrixFromDegree2IntoDegree1") ;
  }

  for(n = n_vertices + 1 ; n < nn ; n++) {
    edge_vertices[n] = edge_vertices[n - 1] + 2 ;
  }

  /* Transformation de la matrice de degre 2 en degre 1 */
  for(ilin = 0 ; ilin < nn*neq ; ilin++) { /* boucle sur les lignes */
    for(n = n_vertices ; n < nn ; n++) {
      int i ;
      icol  = n*neq + inc ;
      for(i = 0 ; i < 2 ; i++) {
        int    m    = edge_vertices[n][i] ;
        int    jcol = m*neq + inc ;
        K(ilin,jcol) += 0.5*K(ilin,icol) ;
      }
      K(ilin,icol) = 0. ;
    }
  }
  
  for(icol = 0 ; icol < nn*neq ; icol++) { /* boucle sur les colonnes */
    for(n = n_vertices ; n < nn ; n++) {
      int i ;
      ilin  = n*neq + equ ;
      for(i = 0 ; i < 2 ; i++) {
        int    m     = edge_vertices[n][i] ;
        int    jlin  = m*neq + equ ;
        K(jlin,icol) += 0.5*K(ilin,icol) ;
      }
      K(ilin,icol) = 0. ;
    }
  }

  /* Relation lineaire aux noeuds milieux des aretes */
  for(n = n_vertices ; n < nn ; n++) {
    int i ;
    ilin  = n*neq + equ ;
    K(ilin,ilin) = -1 ;
    for(i = 0 ; i < 2 ; i++) {
      int    m     = edge_vertices[n][i] ;
      icol  = m*neq + inc ;
      K(ilin,icol) = 0.5 ;
    }
  }
#undef K
}



_INLINE_ double*   (FEM_ComputeBodyForceResidu)(FEM_t* fem,IntFct_t* intfct,const double* f,const int dec)
/* Compute the residu due to a force */
{
  Element_t* el = FEM_GetElement(fem) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int dim_e = Element_GetDimension(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  int nf = IntFct_GetNbOfFunctions(intfct) ;
  int np = IntFct_GetNbOfPoints(intfct) ;
  int dim_h = IntFct_GetDimension(intfct) ;
  double* weight = IntFct_GetWeight(intfct) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  size_t SizeNeeded = nn*(sizeof(double)) ;
  double* r = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  double* x[Element_MaxNbOfNodes] ;
  double zero = 0. ;

  /* Initialization */
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) r[i] = zero ;
  }
  
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      x[i] = Element_GetNodeCoordinate(el,i) ;
    }
  }
  
  
  /* One node mesh */
  if(nn == 1 && dim_e == 3) {
    r[0] = f[0] ;

    return(r) ;
  }
  
  
  /* Interface elements */
  if(Element_HasZeroThickness(el)) {
    if(dim_h == dim - 1) {
      /* Assuming that the numbering of the element is formed with the nf first nodes  */
      if(nn > nf) {
        int p ;

        /* Check the numbering */
        Element_CheckNumberingOfOverlappingNodes(el,nf) ;

        /* 1D, 2D, 3D */
        for(p = 0 ; p < np ; p++) {
          double* h  = IntFct_GetFunctionAtPoint(intfct,p) ;
          double* dh = IntFct_GetFunctionGradientAtPoint(intfct,p) ;
          double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
          double a   = weight[p]*d ;
          int i ;
    
          /* cas axisymetrique ou shperique */
          if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
            double radius = zero ;
      
            for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
            
            a *= 2*M_PI*radius ;
            
            if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
          }
    
          /* R(i) = F*H(i) */
          for(i = 0 ; i < nf ; i++) {
            int ii = Element_OverlappingNode(el,i) ;
            double ri = a * f[p*dec] * h[i] ;
            
            r[i ] += 0.5 * ri ;
            r[ii] += 0.5 * ri ;
          }
        }

        return(r) ;
      }
    }
  }


  /* 0D */
  if(dim_h == 0) {
    if(nn == 1) {
      double radius = x[0][0] ;
      
      r[0] = f[0] ;
      
      if(Symmetry_IsCylindrical(sym)) r[0] *= 2*M_PI*radius ;
      else if(Symmetry_IsSpherical(sym)) r[0] *= 4*M_PI*radius*radius ;
      
    } else {
      arret("FEM_ComputeBodyForceResidu: impossible") ;
    }
    
    return(r) ;
  }

  /* 1D, 2D, 3D */
  {
    int p ;
    
    for(p = 0 ; p < np ; p++) {
      double* h  = IntFct_GetFunctionAtPoint(intfct,p) ;
      double* dh = IntFct_GetFunctionGradientAtPoint(intfct,p) ;
      double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
      double a   = weight[p]*d ;
      int i ;
    
      /* cas axisymetrique ou shperique */
      if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
        double radius = zero ;
      
        for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
        
        a *= 2*M_PI*radius ;
        if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
      }
    
      /* R(i) = F*H(i) */
      for(i = 0 ; i < nf ; i++) {
        r[i] += a * f[p*dec] * h[i] ;
      }
    }
  }
  
  return(r) ;
}



_INLINE_ double*   (FEM_ComputeStrainWorkResidu)(FEM_t* fem,IntFct_t* intfct,const double* sig,const int dec)
/* Compute the residu due to strain work */
{
#define DH(n,i)     (dh[(n)*3 + (i)])
//#define DH(n,i)     (dh[(n)*dim_h + (i)])
#define SIG(i,j)    (sig[(i)*3 + (j)])
#define CAJ(i,j)    (caj[(i)*3 + (j)])
#define R(n,i)      (r[(n)*dim + (i)])
  Element_t* el = FEM_GetElement(fem) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int dim_e = Element_GetDimension(el) ;
  int nn = Element_GetNbOfNodes(el) ;
  int nf = IntFct_GetNbOfFunctions(intfct) ;
  int np = IntFct_GetNbOfPoints(intfct) ;
  int dim_h = IntFct_GetDimension(intfct) ;
  int ndof = nn*dim ;
  double* weight = IntFct_GetWeight(intfct) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  size_t SizeNeeded = ndof*(sizeof(double)) ;
  double* r = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  double* x[Element_MaxNbOfNodes] ;
  double zero = 0. ;
  
  /* Initialization */
  {
    int i ;
    
    for(i = 0 ; i < ndof ; i++) r[i] = zero ;
  }
  
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      x[i] = Element_GetNodeCoordinate(el,i) ;
    }
  }
  
  
  /* One node mesh: for this special element the displacement u_i stands for strain_ii */
  if(nn == 1 && dim_e == 3) {
    int i ;
    
    for(i = 0 ; i < dim ; i++) {
      r[i] = SIG(i,i) ;
    }

    return(r) ;
  }
  
  
  /* Interface elements */
  if(Element_HasZeroThickness(el)) {
    if(dim_h == dim - 1) {
      /* Assuming that the numbering of the element is formed with the nf first nodes  */
      if(nn > nf) {
        int p ;

        /* Check the numbering */
        Element_CheckNumberingOfOverlappingNodes(el,nf) ;

        for(p = 0 ; p < np ; p++ , sig += dec) {
          double* h  = IntFct_GetFunctionAtPoint(intfct,p) ;
          double* dh = IntFct_GetFunctionGradientAtPoint(intfct,p) ;
          double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
          double a   = weight[p]*d ;
          double* norm = Element_ComputeNormalVector(el,dh,nf,dim_h) ;
          double sign[3] = {0,0,0} ;
          int i,j ;

          if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
            double radius = 0. ;
        
            for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
        
            a *= 2*M_PI*radius ;
        
            if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
          }

          /* Compute the vector stress: SIG.N 
           * Pay attention to the orientation of the normal:
           * the normal N is oriented from the opposite face
           * to the main face of the element, i.e. from ii to i */
          #define NORM(i)  (norm[i])
          for(i = 0 ; i < dim ; i++) {
            for(j = 0 ; j < dim ; j++) {
              sign[i] += SIG(i,j) * NORM(j) ;
            }
          }
          #undef NORM

          /* R(i,j) = H(i) * SIGN(j) */
          for(i = 0 ; i < nf ; i++) {
            int ii = Element_OverlappingNode(el,i) ;

            for(j = 0 ; j < dim ; j++) {
              R(i,j)  +=   a * h[i] * sign[j] ;
              R(ii,j) += - a * h[i] * sign[j] ;
            }
          }
        }

        return(r) ;
      }
    }
  }
  
  /* Regular element */
  {
    int p ;
    
    for(p = 0 ; p < np ; p++ , sig += dec) {
      double* h  = IntFct_GetFunctionAtPoint(intfct,p) ;
      double* dh = IntFct_GetFunctionGradientAtPoint(intfct,p) ;
      double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
      double a   = weight[p]*d ;
      double* caj = Element_ComputeInverseJacobianMatrix(el,dh,nf,dim_h) ;
      double radius ;
      int i,j ;
    
      /* The radius in axisymmetrical or spherical case */
      if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
        radius = 0. ;
        
        for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
        
        a *= 2*M_PI*radius ;
        
        if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
      }
    
      /* R(i,j) = DH(i,k) * J(k,l) * S(l,j) */
      for(i = 0 ; i < nf ; i++) for(j = 0 ; j < dim ; j++) {
        int    k,l ;
        
        for(k = 0 ; k < dim_h ; k++) for(l = 0 ; l < dim ; l++) {
          R(i,j) += a * DH(i,k) * CAJ(k,l) * SIG(l,j) ;
        }
      }
    
      /* Axisymmetrical or spherical case */
      if(Symmetry_IsCylindrical(sym)) {
        /* R(i,0) = H(i)/r * S(theta,theta) */
        for(i = 0 ; i < nf ; i++) {
          R(i,0) += a * h[i]/radius * SIG(2,2) ;
        }
      } else if(Symmetry_IsSpherical(sym)) {
        /* R(i,0) = H(i)/r * (SIG(theta,theta) + SIG(phi,phi)) */
        for(i = 0 ; i < nf ; i++) {
          R(i,0) += a * h[i]/radius * (SIG(1,1) + SIG(2,2)) ;
        }
      }
    }
  }
  
  return(r) ;
  
#undef DH
#undef SIG
#undef CAJ
#undef R
}



_INLINE_ double*   (FEM_ComputeFluxResidu)(FEM_t* fem,IntFct_t* intfct,const double* f,const int dec)
/* Compute the residu due to flux */
{
#define DH(n,i)     (dh[(n)*3+(i)])
//#define DH(n,i)     (dh[(n)*dim_h+(i)])
#define CAJ(i,j)    (caj[(i)*3 + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int dim_e = Element_GetDimension(el) ;
  int nn  = Element_GetNbOfNodes(el) ;
  int nf = IntFct_GetNbOfFunctions(intfct) ;
  int np = IntFct_GetNbOfPoints(intfct) ;
  int dim_h = IntFct_GetDimension(intfct) ;
  double* weight = IntFct_GetWeight(intfct) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  size_t SizeNeeded = nn*(sizeof(double)) ;
  double* r = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  double* x[Element_MaxNbOfNodes] ;
  double zero = 0. ;
  
  /* Initialization */
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) r[i] = zero ;
  }
  
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      x[i] = Element_GetNodeCoordinate(el,i) ;
    }
  }
  
  
  /* one node mesh: the unknown stands for the gradient along x-axis */
  if(nn == 1 && dim_e == 3) {
    r[0] = f[0] ;
    
    return(r) ;
  }
  
  
  /* Interface elements */
  if(Element_HasZeroThickness(el)) {
    if(dim_h == dim - 1) {
      /* Assuming that the numbering of the element is formed with the nf first nodes  */
      if(nn > nf) {
        int p ;

        /* Check the numbering */
        Element_CheckNumberingOfOverlappingNodes(el,nf) ;
    
        for(p = 0 ; p < np ; p++ , f +=dec) {
          double* h  = IntFct_GetFunctionAtPoint(intfct,p) ;
          double* dh = IntFct_GetFunctionGradientAtPoint(intfct,p) ;
          double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
          double a   = weight[p]*d ;
          double* caj = Element_ComputeInverseJacobianMatrix(el,dh,nf,dim_h) ;
          int i ;
    
          /* cas axisymetrique ou shperique */
          if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
            double radius = 0. ;
            
            for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
            
            a *= 2*M_PI*radius ;
            if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
          }
    
          /* R(i) = DH(i,k)*J(k,j)*F(j) */
          for(i = 0 ; i < nf ; i++) {
            int ii = Element_OverlappingNode(el,i) ;
            int    j,k ;
      
            for(j = 0 ; j < dim ; j++) for(k = 0 ; k < dim_h ; k++) {
              double ri = a * DH(i,k) * CAJ(k,j) * f[j] ;
                
              /* The continity of pressure is assumed so that p(i) = p(ii).
               * For generality we consider that p = 0.5 * (p(i) + p(ii))
               * along the element. */
              
              r[i]  += 0.5 * ri ;
              r[ii] += 0.5 * ri ;
            }
          }
        }

        return(r) ;
      }
    }
  }
  
  /* Regular element */
  {
    int p ;
    
    for(p = 0 ; p < np ; p++ , f +=dec) {
      double* h  = IntFct_GetFunctionAtPoint(intfct,p) ;
      double* dh = IntFct_GetFunctionGradientAtPoint(intfct,p) ;
      double d   = Element_ComputeJacobianDeterminant(el,dh,nf,dim_h) ;
      double a   = weight[p]*d ;
      double* caj = Element_ComputeInverseJacobianMatrix(el,dh,nf,dim_h) ;
      int i ;
    
      /* cas axisymetrique ou shperique */
      if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
        double radius = 0. ;
        
        for(i = 0 ; i < nf ; i++) radius += h[i]*x[i][0] ;
        
        a *= 2*M_PI*radius ;
        if(Symmetry_IsSpherical(sym)) a *= 2*radius ;
      }
    
      /* R(i) = DH(i,k)*J(k,j)*F(j) */
      for(i = 0 ; i < nf ; i++) {
        int    j,k ;
      
        for(j = 0 ; j < dim ; j++) for(k = 0 ; k < dim_h ; k++) {
          r[i] += a * DH(i,k) * CAJ(k,j) * f[j] ;
        }
      }
    }
  }
  
  return(r) ;
  
#undef DH
#undef CAJ
}



_INLINE_ double* (FEM_ComputeMassResidu)(FEM_t* fem,IntFct_t* intfct,const double* f,const double* f_n,const int dec)
/** Return a residu due to mass increment */
{
  int np = IntFct_GetNbOfPoints(intfct) ;
  double g1[IntFct_MaxNbOfIntPoints] ;
  
  {
    int i ;
    
    for(i = 0 ; i < np ; i++) {
      g1[i] = f[i*dec] - f_n[i*dec] ;
    }
  }
    
  return(FEM_ComputeBodyForceResidu(fem,intfct,g1,1)) ;
}



_INLINE_ double* (FEM_ComputeMassBalanceEquationResidu)(FEM_t* fem,IntFct_t* intfct,const double* f,const double* f_n,const double* w,const double dt,const int dec)
/** Return a mass balance equation residu */
{
  Element_t* el = FEM_GetElement(fem) ;
  int nn  = Element_GetNbOfNodes(el) ;
  double* rm = FEM_ComputeMassResidu(fem,intfct,f,f_n,dec) ;
  double* rw = FEM_ComputeFluxResidu(fem,intfct,w,dec) ;
  
  {
    int i ;
    
    for(i = 0 ; i < nn ; i++) {
      rm[i] += -dt*rw[i] ;
    }
  }
  
  return(rm) ;
}



_INLINE_ double* (FEM_ComputeSurfaceLoadResidu)(FEM_t* fem,IntFct_t* intfct,Load_t* load,const double t,const double dt)
/* Compute the residu force due to surface loads (r) */
{
  Element_t* el = FEM_GetElement(fem) ;
  double** u = Element_ComputePointerToCurrentNodalUnknowns(el) ;
  Geometry_t* geom = Element_GetGeometry(el) ;
  int nn  = Element_GetNbOfNodes(el) ;
  int dim = Geometry_GetDimension(geom) ;
  int dim_e = Element_GetDimension(el) ;
  Symmetry_t sym = Geometry_GetSymmetry(geom) ;
  Node_t** no = Element_GetPointerToNode(el) ;
  Field_t* field = Load_GetField(load) ;
  char*    load_eqn = Load_GetNameOfEquation(load) ;
  char*    load_type = Load_GetType(load) ;
  Function_t* function = Load_GetFunction(load) ;
  int dim_h  = IntFct_GetDimension(intfct) ;
  int    nf  = IntFct_GetNbOfFunctions(intfct) ;
  int    np  = IntFct_GetNbOfPoints(intfct) ;
  int    neq = Element_GetNbOfEquations(el) ;
  int    ieq = Element_FindEquationPositionIndex(el,load_eqn) ;
  int    ndof = nn*neq ;
  size_t SizeNeeded = ndof*(sizeof(double)) ;
  double* r = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  double* x[Element_MaxNbOfNodes] ;
  double zero = 0. ;
  int    i ;
  
  for(i = 0 ; i < nn ; i++) {
    x[i] = Node_GetCoordinate(no[i]) ;
  }

  /* initialization */
  for(i = 0 ; i < ndof ; i++) r[i] = zero ;

  if(field == NULL) return(r) ;

  /* Position index of the equation */
  if(ieq < 0) {
    arret("FEM_ComputeSurfaceLoadResidu:\n"\
          "  illicit position index of equation: %d",ieq) ;
  }


  /* flux or cumulative flux*/
  if(strncmp(load_type,"flux",4) == 0 || strncmp(load_type,"cumulflux",4) == 0) {
    double ft = dt ;

    if(function) {
      if(strncmp(load_type,"flux",4) == 0) {
        ft = 0.5 * dt * (Function_ComputeValue(function,t) + Function_ComputeValue(function,t - dt)) ;
      }
    
      if(strncmp(load_type,"cumulflux",4) == 0) {
        ft = Function_ComputeValue(function,t) - Function_ComputeValue(function,t - dt) ;
      }
    }
    
    if(dim == 1 && nn == 1) {
      double radius = x[0][0] ;
      
      r[ieq] = ft*Field_ComputeValueAtPoint(field,x[0],dim) ;
      
      if(Symmetry_IsCylindrical(sym)) r[ieq] *= 2*M_PI*radius ;
      else if(Symmetry_IsSpherical(sym)) r[ieq] *= 4*M_PI*radius*radius ;
      return(r) ;
    }
    
    if(dim >= 2) {
      double* rb ;
      double f[3*IntFct_MaxNbOfIntPoints] ;
      int p ;
      
      for(p = 0 ; p < np ; p++) {
        double* h = IntFct_GetFunctionAtPoint(intfct,p) ;
        double y[3] = {0.,0.,0.,} ;
        for(i = 0 ; i < dim ; i++) {
          int j ;
          for(j = 0 ; j < nf ; j++) y[i] += h[j]*x[j][i] ;
        }
        f[p] = ft*Field_ComputeValueAtPoint(field,y,dim) ;
      }
      
      rb = FEM_ComputeBodyForceResidu(fem,intfct,f,1) ;
      
      for(i = 0 ; i < nn ; i++) r[i*neq+ieq] = rb[i] ;
      
      FEM_FreeBufferFrom(fem,rb) ;
      return(r) ;
    }
  }


  /* linear dependent flux: F = A*U */
  if(strncmp(load_type,"linearflux",4) == 0) {
    double ft = 1 ;
    
    if(function != NULL) {
      ft = Function_ComputeValue(function,t) ;
    }
    
    if(dim == 1 && nn == 1) {
      double v = u[0][ieq] ;
      double radius = x[0][0] ;
      
      r[ieq] = ft*Field_ComputeValueAtPoint(field,x[0],dim)*v ;
      
      if(Symmetry_IsCylindrical(sym)) r[ieq] *= 2*M_PI*radius ;
      else if(Symmetry_IsSpherical(sym)) r[ieq] *= 4*M_PI*radius*radius ;
      return(r) ;
    }
    
    if(dim >= 2) {
      double* rb ;
      double f[3*IntFct_MaxNbOfIntPoints] ;
      int p ;
      
      for(p = 0 ; p < np ; p++) {
        double v = FEM_ComputeUnknown(fem,u,intfct,p,ieq) ;
        double* h = IntFct_GetFunctionAtPoint(intfct,p) ;
        double y[3] = {0.,0.,0.,} ;
        for(i = 0 ; i < dim ; i++) {
          int j ;
          for(j = 0 ; j < nf ; j++) y[i] += h[j]*x[j][i] ;
        }
        f[p] = ft*Field_ComputeValueAtPoint(field,y,dim)*v ;
      }
      
      rb = FEM_ComputeBodyForceResidu(fem,intfct,f,1) ;
      
      for(i = 0 ; i < nn ; i++) r[i*neq+ieq] = rb[i] ;
      
      FEM_FreeBufferFrom(fem,rb) ;
      return(r) ;
    }
  }
  
  
  /* force */
  if(strncmp(load_type,"force",5) == 0) {
    double ft = 1. ;
    
    if(function != NULL) {
      ft = Function_ComputeValue(function,t) ;
    }
    
    if(dim == 1 && nn == 1) {
      double radius = x[0][0] ;
      
      r[ieq] = ft*Field_ComputeValueAtPoint(field,x[0],dim) ;
      
      if(Symmetry_IsCylindrical(sym)) r[ieq] *= 2*M_PI*radius ;
      else if(Symmetry_IsSpherical(sym)) r[ieq] *= 4*M_PI*radius*radius ;
      return(r) ;
    }
    
    if(dim >= 2) {
      double* rb ;
      double f[3*IntFct_MaxNbOfIntPoints] ;
      int p ;
      
      for(p = 0 ; p < np ; p++) {
        double* h = IntFct_GetFunctionAtPoint(intfct,p) ;
        double y[3] = {0.,0.,0.,} ;
        for(i = 0 ; i < dim ; i++) {
          int j ;
          for(j = 0 ; j < nf ; j++) y[i] += h[j]*x[j][i] ;
        }
        f[p] = ft*Field_ComputeValueAtPoint(field,y,dim) ;
      }
      
      rb = FEM_ComputeBodyForceResidu(fem,intfct,f,1) ;
      
      for(i = 0 ; i < nn ; i++) r[i*neq+ieq] = rb[i] ;
      
      FEM_FreeBufferFrom(fem,rb) ;
      return(r) ;
    }
  }
  
  
  /* pressure */
  if(strncmp(load_type,"press",5) == 0) {
    double ft = 1. ;
    
    if(function != NULL) {
      ft = Function_ComputeValue(function,t) ;
    }
    
    if(dim >= 2) {
      double f[3*IntFct_MaxNbOfIntPoints] ;
      int p ;
      
      for(p = 0 ; p < np ; p++) {
        double* h = IntFct_GetFunctionAtPoint(intfct,p) ;
        double* dh = IntFct_GetFunctionGradientAtPoint(intfct,p) ;
        double* n = Element_ComputeNormalVector(el,dh,nf,dim_h) ;
        double y[3] = {0.,0.,0.,} ;
        double f0 ;
        for(i = 0 ; i < dim ; i++) {
          int j ;
          for(j = 0 ; j < nf ; j++) y[i] += h[j]*x[j][i] ;
        }
        f0 = ft*Field_ComputeValueAtPoint(field,y,dim) ;
        for(i = 0 ; i < dim ; i++) f[p*dim+i] = -f0*n[i] ;
      }
      
      for(i = 0 ; i < dim ; i++) {
        int j ;
        double* rb = FEM_ComputeBodyForceResidu(fem,intfct,f+i,dim) ;
        
        for(j = 0 ; j < nn ; j++) r[j*neq+ieq+i] = rb[j] ;
        
        FEM_FreeBufferFrom(fem,rb) ;
      }
      
      return(r) ;
    }
  }
  
  
  /* tensor component */
  if(strncmp(load_type,"sig_",4) == 0) {
    int ii = load_type[4] - '1' ;
    int jj = load_type[5] - '1' ;
    double ft = 1. ;
    
    if(ii < 0 || ii >= dim) {
      arret("FEM_ComputeSurfaceLoadResidu:\n"\
            "  first index must range between 1 and %d",dim) ;
    }
    
    if(jj < 0 || jj >= dim) {
      arret("FEM_ComputeSurfaceLoadResidu:\n"\
            "  second index must range between 1 and %d",dim) ;
    }
    
    if(function != NULL) {
      ft = Function_ComputeValue(function,t) ;
    }
    
    /* One node mesh: for this special element the displacement u_i stands for strain_ii */
    if(nn == 1 && dim_e == 3) {
      double* h = IntFct_GetFunctionAtPoint(intfct,0) ;
      double n[3] = {0,0,0} ;
      double y[3] = {0,0,0} ;
      
      if(ii != jj) {
        arret("FEM_ComputeSurfaceLoadResidu: different indices not allowed") ;
      }
        
      for(i = 0 ; i < dim ; i++) {
        int j ;
        
        for(j = 0 ; j < nf ; j++) y[i] += h[j]*x[j][i] ;
      }
      
      n[ii] = 1 ;
      r[ieq+ii] = ft*Field_ComputeValueAtPoint(field,y,dim)*n[jj] ;
      
      return(r) ;
    }
    
    if(dim >= 2) {
      double* rb ;
      double f[3*IntFct_MaxNbOfIntPoints] ;
      int p ;
      int j ;
      
      for(p = 0 ; p < np ; p++) {
        double* h = IntFct_GetFunctionAtPoint(intfct,p) ;
        double* dh = IntFct_GetFunctionGradientAtPoint(intfct,p) ;
        double* n = Element_ComputeNormalVector(el,dh,nf,dim_h) ;
        double y[3] = {0,0,0} ;
        
        for(i = 0 ; i < dim ; i++) {
          for(j = 0 ; j < nf ; j++) y[i] += h[j]*x[j][i] ;
        }
        
        f[p] = ft*Field_ComputeValueAtPoint(field,y,dim)*n[jj] ;
      }
      
      rb = FEM_ComputeBodyForceResidu(fem,intfct,f,1) ;
      
      for(j = 0 ; j < nn ; j++) r[j*neq+ieq+ii] = rb[j] ;
      
      FEM_FreeBufferFrom(fem,rb) ;
      return(r) ;
    }
  }
  
  arret("FEM_ComputeSurfaceLoadResidu: illicit load type.\n"\
        "  Available types are:\n"\
        "  flux, cumulflux, linearflux, force, pressure\n"\
        "  sig_11,sig_12,...,sig_33") ;
  return(r) ;
}




_INLINE_ void (FEM_TransformResiduFromDegree2IntoDegree1)(FEM_t* fem,const int inc,const int equ,double* r)
{
  Element_t* el = FEM_GetElement(fem) ;
  int    nn = Element_GetNbOfNodes(el) ;
  int    dim = Element_GetDimension(el) ;
  int    neq = Element_GetNbOfEquations(el) ;
  int    n_vertices = 0 ;
  int    edge_vertices_line[2]       = {0,1} ;
  int    edge_vertices_triangle[6]   = {0,1,1,2,2,0} ;
  int    edge_vertices_quadrangle[8] = {0,1,1,2,2,3,3,0} ;
  int    *edge_vertices[Element_MaxNbOfNodes] ;
  int    n ;

  if(nn > Element_MaxNbOfNodes) arret("FEM_TransformResiduFromDegree2IntoDegree1") ;

  if(dim == 1) {
    if(nn == 3) {
      n_vertices = 2 ;
      edge_vertices[n_vertices] = edge_vertices_line ;
    } else return ;
  } else if(dim == 2) {
    if(nn == 6) {
      n_vertices = 3 ;
      edge_vertices[n_vertices] = edge_vertices_triangle ;
    } else if(nn == 8) {
      n_vertices = 4 ;
      edge_vertices[n_vertices] = edge_vertices_quadrangle ;
    } else return ;
  } else if(dim == 3) {
    arret("FEM_TransformResiduFromDegree2IntoDegree1") ;
  } else {
    arret("FEM_TransformResiduFromDegree2IntoDegree1") ;
  }

  for(n = n_vertices + 1 ; n < nn ; n++) {
    edge_vertices[n] = edge_vertices[n - 1] + 2 ;
  }

  for(n = n_vertices ; n < nn ; n++) {
    int ilin  = n*neq + equ ;
    int i ;
    
    for(i = 0 ; i < 2 ; i++) {
      int    m     = edge_vertices[n][i] ;
      int    jlin  = m*neq + equ ;
      
      r[jlin] += 0.5*r[ilin] ;
    }
    
    //r[ilin] = 0. ;
    #if 1
    {
      double u_n = Element_GetValueOfCurrentNodalUnknown(el,n,inc) ;
      
      r[ilin] = u_n ;
    
      for(i = 0 ; i < 2 ; i++) {
        int    m     = edge_vertices[n][i] ;
        double u_m = Element_GetValueOfCurrentNodalUnknown(el,m,inc) ;
      
        r[ilin] -= 0.5*u_m ;
      }
    }
    #endif
  }
}



_INLINE_ void   (FEM_AverageStresses)(Mesh_t* mesh,double* stress)
{
  unsigned int nel = Mesh_GetNbOfElements(mesh) ;
  Element_t* el0 = Mesh_GetElement(mesh) ;
  double vol = FEM_ComputeVolume(mesh) ;
  
  /* Stress integration */
  {
    #define N_MODELS (4)
    int n_models = N_MODELS ;
    const char* modelswithstresses[N_MODELS] = {"Elast","Plast","MechaMic","Plastold"} ;
    const int   stressindex[N_MODELS] = {0,12,12,0} ;
    #undef N_MODELS
    int i ;
    
    for(i = 0 ; i < 9 ; i++) {
      double sig = 0 ;
      unsigned int ie ;
    
      for(ie = 0 ; ie < nel ; ie++) {
        Element_t* el = el0 + ie ;
        double* vim = Element_GetCurrentImplicitTerm(el) ;
        IntFct_t* intfct = Element_GetIntFct(el) ;
        int nbofintpoints = IntFct_GetNbOfPoints(intfct) ;
        int ni = Element_GetNbOfImplicitTerms(el) ;
        int nvi = ni/nbofintpoints ;
    
        if(Element_IsSubmanifold(el)) continue ;
        
        {
          Model_t* model = Element_GetModel(el) ;
          char* codename = Model_GetCodeNameOfModel(model) ;
          int j = 0 ;
  
          while(j < n_models && strcmp(modelswithstresses[j],codename)) j++ ;
  
          if(j == n_models) {
            arret("FEM_AverageStresses: model not implemented") ;
          }
          
          vim += stressindex[j] ;
        }
    
        sig +=  Element_IntegrateOverElement(el,intfct,vim + i,nvi) ;
      }
    
      /* Stress average */
      stress[i] = sig/vol ;
    }
  }
}



_INLINE_ double   (FEM_AverageCurrentImplicitTerm)(Mesh_t* mesh,const char* modelname,const int index,const int nvi)
/** Average the value stored at the position "index" in the current 
 *  implicit terms of the model "modelname".
 *  Return the volume average over the elements of the model "modelname".
 */
{
  unsigned int nel = Mesh_GetNbOfElements(mesh) ;
  Element_t* el0 = Mesh_GetElement(mesh) ;
  double sum = 0 ;
  double vol = 0 ;
        
  if(index >= nvi) {
    arret("FEM_AverageCurrentImplicitTerm:") ;
  }
  
  /* Integration */
  {
    unsigned int ie ;
    
    for(ie = 0 ; ie < nel ; ie++) {
      Element_t* el = el0 + ie ;
      Model_t* model = Element_GetModel(el) ;
      char* codename = Model_GetCodeNameOfModel(model) ;
    
      if(Element_IsSubmanifold(el)) continue ;
      
      if(String_Is(codename,modelname)) {
        double* vi = Element_GetCurrentImplicitTerm(el) ;
        IntFct_t* intfct = Element_GetIntFct(el) ;
        double one = 1 ;
    
        vol +=  Element_IntegrateOverElement(el,intfct,&one,0) ;
        sum +=  Element_IntegrateOverElement(el,intfct,vi + index,nvi) ;
      }
    }
    
    if(vol <= 0) {
      arret("FEM_AverageCurrentImplicitTerm:") ;
    }
  }

  return(sum/vol) ;
}



_INLINE_ double   (FEM_AveragePreviousImplicitTerm)(Mesh_t* mesh,const char* modelname,const int index,const int nvi)
/** Average the value stored at the position "index" in the previous 
 *  implicit terms of the model "modelname".
 *  Return the volume average over the elements of the model "modelname".
 */
{
  unsigned int nel = Mesh_GetNbOfElements(mesh) ;
  Element_t* el0 = Mesh_GetElement(mesh) ;
  double sum = 0 ;
  double vol = 0 ;
        
  if(index >= nvi) {
    arret("FEM_AveragePreviousImplicitTerm:") ;
  }
  
  /* Integration */
  {
    unsigned int ie ;
    
    for(ie = 0 ; ie < nel ; ie++) {
      Element_t* el = el0 + ie ;
      Model_t* model = Element_GetModel(el) ;
      char* codename = Model_GetCodeNameOfModel(model) ;
    
      if(Element_IsSubmanifold(el)) continue ;
      
      if(String_Is(codename,modelname)) {
        double* vi = Element_GetPreviousImplicitTerm(el) ;
        IntFct_t* intfct = Element_GetIntFct(el) ;
        double one = 1 ;
    
        vol +=  Element_IntegrateOverElement(el,intfct,&one,0) ;
        sum +=  Element_IntegrateOverElement(el,intfct,vi + index,nvi) ;
      }
    }
    
    if(vol <= 0) {
      arret("FEM_AveragePreviousImplicitTerm:") ;
    }
  }

  return(sum/vol) ;
}



_INLINE_ double   (FEM_ComputeVolume)(Mesh_t* mesh)
{
  unsigned int nel = Mesh_GetNbOfElements(mesh) ;
  Element_t* el0 = Mesh_GetElement(mesh) ;
  double vol = 0 ;
  
  /* The volume */
  {
    unsigned int ie ;
    
    for(ie = 0 ; ie < nel ; ie++) {
      Element_t* el = el0 + ie ;
      IntFct_t* intfct = Element_GetIntFct(el) ;
      double one = 1 ;
    
      if(Element_IsSubmanifold(el)) continue ;
      
      vol +=  Element_IntegrateOverElement(el,intfct,&one,0) ;
    }
  }
  
  return(vol) ;
}



_INLINE_ void (FEM_AddAverageTerms)(FEM_t* fem,const int nvi,const int nve,const int nvc)
/** Compute the volume average of the (im/ex)plicit and constant terms
 *  over the element and add these values in the corresponding arrays 
 *  after the interpolated points. Enough memory should have been allocated. 
 *  So if np is the nb of interpolation points at least (np+1)*nvi, (np+1)*nve
 *  and (np+1)*nvc doubles should have been allocated for the implicit, explicit
 *  and constant terms.
 */
{
  Element_t* el = FEM_GetElement(fem) ;
  IntFct_t*  intfct = Element_GetIntFct(el) ;
  int np = IntFct_GetNbOfPoints(intfct) ;
  double* ve0  = Element_GetExplicitTerm(el) ;
  double* vi0  = Element_GetImplicitTerm(el) ;
  double* vc0  = Element_GetConstantTerm(el) ;
  double one = 1 ;
  double vol = Element_IntegrateOverElement(el,intfct,&one,0) ;
    
  {
    double* vi = vi0 + np*nvi ;
    int i ;
    
    for(i = 0 ; i < nvi ; i++) {
      double v =  Element_IntegrateOverElement(el,intfct,vi0 + i,nvi) ;
      
      vi[i] =  v/vol ;
    }
  }
    
  {
    double* ve = ve0 + np*nve ;
    int i ;
    
    for(i = 0 ; i < nve ; i++) {
      double v =  Element_IntegrateOverElement(el,intfct,ve0 + i,nve) ;
      
      ve[i] =  v/vol ;
    }
  }
    
  {
    double* vc = vc0 + np*nvc ;
    int i ;
    
    for(i = 0 ; i < nvc ; i++) {
      double v =  Element_IntegrateOverElement(el,intfct,vc0 + i,nvc) ;
      
      vc[i] =  v/vol ;
    }
  }

}



/* FEM_Compute functions in the form (FEM_t*,double**,IntFct_t*,int,int) */


_INLINE_ double (FEM_ComputeUnknown)(FEM_t* fem,double const* const* u,IntFct_t* intfct,int p,int inc)
/** Compute the unknown located at "inc" at the interpolation point "p" */
{
#define U(n)   (u[n][Element_GetNodalUnknownPosition(el,n,inc)])
  Element_t* el = FEM_GetElement(fem) ;
  int nn = IntFct_GetNbOfFunctions(intfct) ;
  double* h  = IntFct_GetFunctionAtPoint(intfct,p) ;
  double par = 0. ;
  int i ;
        
  for(i = 0 ; i < nn ; i++) {
    par += h[i]*U(i) ;
  }
        
  return(par) ;
#undef U
}



_INLINE_ double* (FEM_ComputeDisplacementVector)(FEM_t* fem,double const* const* u,IntFct_t* intfct,int p,int inc)
/** Compute the displacement vector located at 
 *  "inc,inc+1,inc+2" at the interpolation point "p" */
{
  size_t SizeNeeded = 3*sizeof(double) ;
  double* dis = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;

  {
    Element_t* el = FEM_GetElement(fem) ;
    int dim = Element_GetDimensionOfSpace(el) ;
    int i ;
    
    /* In case of a zero-thickness element (fracture) 
     * we compute the discontinuous displacement vector */
    #if 0
    if(Element_HasZeroThickness(el)) {
      int nf = IntFct_GetNbOfFunctions(intfct) ;
      double* du[Element_MaxNbOfNodes] ;
      double  du1[Element_MaxNbOfNodes*Element_MaxNbOfDOF] ;
      
      for(i = 0 ; i < nf ; i++) {
        int ii = Element_OverlappingNode(el,i) ;
        int j ;
        
        du[i] = du1 + i*Element_MaxNbOfDOF ;
        
        for(j = 0 ; j < dim ; j++) {
          du[i][inc + j] = u[i][inc + j] - u[ii][inc + j] ;
        }
      }
    
      for(i = 0 ; i < dim ; i++) {
        dis[i] = FEM_ComputeUnknown(fem,du,intfct,p,inc+i) ;
      }
    } else 
    #endif
    {
      for(i = 0 ; i < dim ; i++) {
        dis[i] = FEM_ComputeUnknown(fem,u,intfct,p,inc+i) ;
      }
    }
    
    for(i = dim ; i < 3 ; i++) dis[i] = 0 ;
  }
        
  return(dis) ;
}



_INLINE_ double* (FEM_ComputeUnknownGradient)(FEM_t* fem,double const* const* u,IntFct_t* intfct,int p,int inc)
/** Compute the unknown gradient located at "inc" at the interpolation point "p" */
{
#define U(n)   (u[n][Element_GetNodalUnknownPosition(el,n,inc)])
#define DH(n,i)  (dh[(n)*3 + (i)])
//#define DH(n,i)  (dh[(n)*dim_h + (i)])
#define CJ(i,j)  (cj[(i)*3 + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int dim_e = Element_GetDimension(el) ;
  size_t SizeNeeded = 3*sizeof(double) ;
  double* grad = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  int nn = IntFct_GetNbOfFunctions(intfct) ;
  int dim_h = IntFct_GetDimension(intfct) ;
  
  
  /* One node mesh: for this special element the unknown stands for the gradient along x-axis */
  if(nn == 1 && dim_e == 3){
    int i ;
    
    for(i = 0 ; i < 3 ; i++) {
      grad[i] = 0 ;
    }

    grad[0] = U(0) ;
    
    return(grad) ;
  }
  

  {
    /* interpolation functions */
    double* dh = IntFct_GetFunctionGradientAtPoint(intfct,p) ;
    double* gu = grad ;
    double grf[3] = {0,0,0} ;
  
    /* the gradient in the reference frame */
    {
      int l ;
        
      for(l = 0 ; l < dim_h ; l++) {
        int k ;
      
        for(k = 0 ; k < nn ; k++) {
          grf[l] += U(k) * DH(k,l) ;
        }
      }
    }
  
    /* the parameter gradient (gu) */
    {
      /* inverse jacobian matrix (cj) */
      double* cj = Element_ComputeInverseJacobianMatrix(el,dh,nn,dim_h) ;
      int i ;
      
      for(i = 0 ; i < 3 ; i++)  gu[i] = 0. ;
  
      for(i = 0 ; i < dim ; i++) {
        int l ;
        
        for(l = 0 ; l < dim_h ; l++) {
          gu[i] += grf[l] * CJ(l,i) ;
        }
      }
      
      Element_FreeBufferFrom(el,cj) ;
    }
  }
  
  return(grad) ;
  
#undef U
#undef DH
#undef CJ
}



_INLINE_ double* (FEM_ComputeLinearStrainTensor)(FEM_t* fem,double const* const* u,IntFct_t* intfct,int p,int inc)
/** Compute the 3D linearized strain tensor for the displacement vector 
 *  located at "inc" and at the interpolation point "p" */
{
#define U(n,i)   (u[n][Element_GetNodalUnknownPosition(el,n,inc + (i))])
#define DH(n,i)  (dh[(n)*3 + (i)])
//#define DH(n,i)  (dh[(n)*dim_h + (i)])
#define STRAIN(i,j) (strain[(i)*3 + (j)])
#define CJ(i,j)  (cj[(i)*3 + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int dim_e = Element_GetDimension(el) ;
  int dim_h = IntFct_GetDimension(intfct) ;
  size_t SizeNeeded = 9*sizeof(double) ;
  double* strain = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  int nn = IntFct_GetNbOfFunctions(intfct) ;
  
  
  /* One node mesh: for this special element the displacement u_i stands for strain_ii */
  if(nn == 1 && dim_e == 3){
    int i ;
    
    for(i = 0 ; i < 9 ; i++) {
      strain[i] = 0 ;
    }

    for(i = 0 ; i < dim ; i++) {
      STRAIN(i,i) = U(0,i) ;
    }
    
    return(strain) ;
  }


  {
    /* interpolation functions */
    double* h  = IntFct_GetFunctionAtPoint(intfct,p) ;
    double* dh = IntFct_GetFunctionGradientAtPoint(intfct,p) ;
    /* inverse jacobian matrix (cj) */
    double* cj = Element_ComputeInverseJacobianMatrix(el,dh,nn,dim_h) ;
    double  gu[3][3] ;
    int     i ;
  
  
    /* initialisation of gu */
    for(i = 0 ; i < 3 ; i++) {
      int j ;
        
      for(j = 0 ; j < 3 ; j++)  {
        gu[i][j] = 0. ;
      }
    }
  
    /* displacement gradient (gu) */
    if(Element_HasZeroThickness(el)) {
      double* norm = Element_ComputeNormalVector(el,dh,nn,dim_h) ;
      double du[3] = {0,0,0} ;
      
      /* the displacement discontinuity */
      for(i = 0 ; i < dim ; i++) {
        int k ;
          
        for(k = 0 ; k < nn ; k++) {
          int kk = Element_OverlappingNode(el,k) ;
            
          du[i] += (U(k,i) - U(kk,i)) * h[k] ;
        }
      }
      
      #define NORM(i)  (norm[i])
      for(i = 0 ; i < dim ; i++) {
        int j ;
        
        for(j = 0 ; j < dim ; j++) {
          gu[i][j] = du[i] * NORM(j) ;
        }
      }
      #undef NORM
      
    } else if(dim_h > 0) {
      double  grf[3][3] = {{0,0,0},{0,0,0},{0,0,0}} ;
      
      /* the gradient in the reference frame */
      for(i = 0 ; i < dim ; i++) {
        int l ;
          
        for(l = 0 ; l < dim_h ; l++) {
          int k ;
          
          for(k = 0 ; k < nn ; k++) {
            grf[i][l] += U(k,i) * DH(k,l) ;
          }
        }
      }
      
      /* the gradient in the current frame */
      for(i = 0 ; i < dim ; i++) {
        int j ;
        
        for(j = 0 ; j < dim ; j++) {
          int l ;
          
          for(l = 0 ; l < dim_h ; l++) {
            gu[i][j] += grf[i][l] * CJ(l,j) ;
          }
        }
      }
    } else {
      Message_FatalError("FEM_ComputeLinearStrainTensor: dim_h <= 0!") ;
    }
      
    {  
      /* Linearized strain tensor */
      for(i = 0 ; i < 3 ; i++) {
        int j ;
        
        for(j = 0 ; j < 3 ; j++) {
          STRAIN(i,j) = (gu[i][j] + gu[j][i])*0.5 ;
        }
      }
  
      /* symmetric cases: axisymmetrical or spherical */
      if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
        
        if(Element_HasZeroThickness(el)) {
          /* No additif terms */
        } else {
          double radius = 0. ;
          double u_r   = 0. ;
          
          for(i = 0 ; i < nn ; i++) {
            double* x = Element_GetNodeCoordinate(el,i) ;
          
            radius += h[i]*x[0] ;
            u_r    += h[i]*U(i,0) ;
          }
          
          if(radius > 0) {
            STRAIN(2,2) += u_r/radius ;
          
            if(Symmetry_IsSpherical(sym)) STRAIN(1,1) += u_r/radius ;
          }
        }
      }
    }
      
    Element_FreeBufferFrom(el,cj) ;
  }
  
  return(strain) ;
#undef U
#undef DH
#undef STRAIN
#undef CJ
}

/* End */



/* FEM_Compute functions in the form (FEM_t*,double*,int,int) */

_INLINE_ double (FEM_ComputeCurrentUnknown)(FEM_t* fem,double* h,int nn,int inc)
/** Compute the unknown at the current time located at "inc" */
{
#define U(n)   (Element_GetValueOfCurrentNodalUnknown(el,n,inc))
  Element_t* el = FEM_GetElement(fem) ;
  int    i ;
  double par = 0. ;
  
  for(i = 0 ; i < nn ; i++) {
    par += h[i]*U(i) ;
  }
  
  return (par) ;
#undef U
}


_INLINE_ double (FEM_ComputePreviousUnknown)(FEM_t* fem,double* h,int nn,int inc)
/** Compute the unknown at the previous time located at "inc" */
{
#define U(n)   (Element_GetValueOfPreviousNodalUnknown(el,n,inc))
  Element_t* el = FEM_GetElement(fem) ;
  int    i ;
  double par = 0. ;
  
  for(i = 0 ; i < nn ; i++) {
    par += h[i]*U(i) ;
  }
  
  return (par) ;
#undef U
}



_INLINE_ double* (FEM_ComputeCurrentUnknownGradient)(FEM_t* fem,double* dh,int nn,int inc)
/** Compute the current unknown gradient for the unknown located at "inc" */
{
#define U(n)   (Element_GetValueOfCurrentNodalUnknown(el,n,inc))
#define DH(n,i)  (dh[(n)*3 + (i)])
//#define DH(n,i)  (dh[(n)*dim_h + (i)])
#define CJ(i,j)  (cj[(i)*3 + (j)]) 
  Element_t* el = FEM_GetElement(fem) ;
  int    dim_h  = Element_GetDimension(el) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  size_t SizeNeeded = 3*sizeof(double) ;
  double* grad = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  
  
  {
    /* inverse jacobian matrix (cj) */
    double* cj = Element_ComputeInverseJacobianMatrix(el,dh,nn,dim_h) ;
    double* gu = grad ;
    double grf[3] = {0,0,0} ;
    int    i ;
  
    /* initialisation of gu */
    for(i = 0 ; i < 3 ; i++)  gu[i] = 0. ;
  
    /* the gradient in the reference frame */
    {
      int l ;
        
      for(l = 0 ; l < dim_h ; l++) {
        int k ;
      
        for(k = 0 ; k < nn ; k++) {
          grf[l] += U(k)*DH(k,l) ;
        }
      }
    }
  
    /* the parameter gradient (gu) */
    for(i = 0 ; i < dim ; i++) {
      int l ;
        
      for(l = 0 ; l < dim_h ; l++) {
        gu[i] += grf[l]*CJ(l,i) ;
      }
    }
  
    Element_FreeBufferFrom(el,cj) ;
  }
  
  return(grad) ;
  
#undef U
#undef DH
#undef CJ
}



_INLINE_ double* (FEM_ComputePreviousUnknownGradient)(FEM_t* fem,double* dh,int nn,int inc)
/** Compute the previous unknown gradient for the unknown located at "inc" */
{
#define U(n)   (Element_GetValueOfPreviousNodalUnknown(el,n,inc))
#define DH(n,i)  (dh[(n)*3 + (i)])
//#define DH(n,i)  (dh[(n)*dim_h + (i)])
#define CJ(i,j)  (cj[(i)*3 + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  int    dim_h  = Element_GetDimension(el) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  size_t SizeNeeded = 3*sizeof(double) ;
  double* grad = (double*) FEM_AllocateInBuffer(fem,SizeNeeded) ;
  
  
  {
    /* inverse jacobian matrix (cj) */
    double* cj = Element_ComputeInverseJacobianMatrix(el,dh,nn,dim_h) ;
    double* gu = grad ;
    double grf[3] = {0,0,0} ;
    int    i ;
  
    /* initialisation of gu */
    for(i = 0 ; i < 3 ; i++)  gu[i] = 0. ;
  
    /* the gradient in the reference frame */
    {
      int l ;
        
      for(l = 0 ; l < dim_h ; l++) {
        int k ;
      
        for(k = 0 ; k < nn ; k++) {
          grf[l] += U(k)*DH(k,l) ;
        }
      }
    }
  
    /* the parameter gradient (gu) */
    for(i = 0 ; i < dim ; i++) {
      int l ;
        
      for(l = 0 ; l < dim_h ; l++) {
        gu[i] += grf[l]*CJ(l,i) ;
      }
    }
  
    Element_FreeBufferFrom(el,cj) ;
  }

  return(grad) ;
  
#undef U
#undef DH
#undef CJ
}
/* End */



/* FEM_Compute functions in the form (FEM_t*,double*,double*,int,int) */

_INLINE_ double* (FEM_ComputeCurrentLinearStrainTensor)(FEM_t* fem,double* h,double* dh,int nn,int inc)
/** Compute the 3D linearized strain tensor for a displacement vector located at "inc" */
{
#define U(n,i)   (Element_GetValueOfCurrentNodalUnknown(el,n,inc + (i)))
#define DH(n,i)  (dh[(n)*3 + (i)])
//#define DH(n,i)  (dh[(n)*dim_h + (i)])
#define EPS(i,j) (eps[(i)*3 + (j)])
#define CJ(i,j)  (cj[(i)*3 + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  size_t SizeNeeded = 9*sizeof(double) ;
  double* eps = (double*) Element_AllocateInBuffer(el,SizeNeeded) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  int    dim_h  = Element_GetDimension(el) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int    i,j ;
  double gu[3][3], *cj ;
  
  
  /* initialisation of gu */
  for(i = 0 ; i < 3 ; i++) for(j = 0 ; j < 3 ; j++)  gu[i][j] = 0. ;
  
  /* inverse jacobian matrix (cj) */
  cj = Element_ComputeInverseJacobianMatrix(el,dh,nn,dim_h) ;
  
  /* displacement gradient (gu) */
  for(i = 0 ; i < dim ; i++) for(j = 0 ; j < dim ; j++) {
    int    k,l ;
    for(k = 0 ; k < nn ; k++) for(l = 0 ; l < dim_h ; l++) {
      gu[i][j] += U(k,i)*DH(k,l)*CJ(l,j) ;
    }
  }
  
  /* Linearized strain tensor */
  for(i = 0 ; i < 3 ; i++) for(j = 0 ; j < 3 ; j++) {
    EPS(i,j) = (gu[i][j] + gu[j][i])*0.5 ;
  }
  
  /* symmetric cases: axisymmetrical or spherical */
  if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
    double radius = 0. ;
    double u_r   = 0. ;
    
    for(i = 0 ; i < nn ; i++) {
      double* x = Element_GetNodeCoordinate(el,i) ;
      
      radius += h[i]*x[0] ;
      u_r   += h[i]*U(i,0) ;
    }
    
    EPS(2,2) += u_r/radius ;
    
    if(Symmetry_IsSpherical(sym)) EPS(1,1) += u_r/radius ;
  }
  
  return(eps) ;
#undef U
#undef DH
#undef EPS
#undef CJ
}



_INLINE_ double* (FEM_ComputeIncrementalLinearStrainTensor)(FEM_t* fem,double* h,double* dh,int nn,int inc)
/** Compute the 3D incremental linearized strain tensor for a displacement vector located at "inc" */
{
#define U(n,i)   (Element_GetValueOfCurrentNodalUnknown(el,n,inc + (i)))
#define U_n(n,i) (Element_GetValueOfPreviousNodalUnknown(el,n,inc + (i)))
#define DU(n,i)  (U(n,i) - U_n(n,i))
#define DH(n,i)  (dh[(n)*3 + (i)])
//#define DH(n,i)  (dh[(n)*dim_h + (i)])
#define EPS(i,j) (eps[(i)*3 + (j)])
#define CJ(i,j)  (cj[(i)*3 + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  size_t SizeNeeded = 9*sizeof(double) ;
  double* eps = (double*) Element_AllocateInBuffer(el,SizeNeeded) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  int    dim_h  = Element_GetDimension(el) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int    i,j ;
  double gu[3][3], *cj ;
  
  
  /* initialisation of gu */
  for(i = 0 ; i < 3 ; i++) for(j = 0 ; j < 3 ; j++)  gu[i][j] = 0. ;
  
  /* inverse jacobian matrix (cj) */
  cj = Element_ComputeInverseJacobianMatrix(el,dh,nn,dim_h) ;
  
  /* displacement gradient (gu) */
  for(i = 0 ; i < dim ; i++) for(j = 0 ; j < dim ; j++) {
    int    k,l ;
    for(k = 0 ; k < nn ; k++) for(l = 0 ; l < dim_h ; l++) {
      gu[i][j] += DU(k,i)*DH(k,l)*CJ(l,j) ;
    }
  }
  
  /* Linearized strain tensor */
  for(i = 0 ; i < 3 ; i++) for(j = 0 ; j < 3 ; j++) {
    EPS(i,j) = (gu[i][j] + gu[j][i])*0.5 ;
  }
  
  /* symmetric cases: axisymmetrical or spherical */
  if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
    double radius = 0. ;
    double u_r   = 0. ;
    
    for(i = 0 ; i < nn ; i++) {
      double* x = Element_GetNodeCoordinate(el,i) ;
      
      radius += h[i]*x[0] ;
      u_r   += h[i]*DU(i,0) ;
    }
    
    EPS(2,2) += u_r/radius ;
    if(Symmetry_IsSpherical(sym)) EPS(1,1) += u_r/radius ;
  }
  
  return(eps) ;
#undef U
#undef U_n
#undef DU
#undef DH
#undef EPS
#undef CJ
}



_INLINE_ double* (FEM_ComputePreviousLinearStrainTensor)(FEM_t* fem,double* h,double* dh,int nn,int inc)
/** Compute the 3D linearized strain tensor for a displacement vector located at "inc" */
{
#define U(n,i)   (Element_GetValueOfPreviousNodalUnknown(el,n,inc + (i)))
#define DH(n,i)  (dh[(n)*3 + (i)])
//#define DH(n,i)  (dh[(n)*dim_h + (i)])
#define EPS(i,j) (eps[(i)*3 + (j)])
#define CJ(i,j)  (cj[(i)*3 + (j)])
  Element_t* el = FEM_GetElement(fem) ;
  size_t SizeNeeded = 9*sizeof(double) ;
  double* eps = (double*) Element_AllocateInBuffer(el,SizeNeeded) ;
  Symmetry_t sym = Element_GetSymmetry(el) ;
  int    dim_h  = Element_GetDimension(el) ;
  int dim = Element_GetDimensionOfSpace(el) ;
  int    i,j ;
  double gu[3][3], *cj ;
  
  /* initialisation of gu */
  for(i = 0 ; i < 3 ; i++) for(j = 0 ; j < 3 ; j++)  gu[i][j] = 0. ;
  
  /* inverse jacobian matrix (cj) */
  cj = Element_ComputeInverseJacobianMatrix(el,dh,nn,dim_h) ;
  
  /* displacement gradient (gu) */
  for(i = 0 ; i < dim ; i++) for(j = 0 ; j < dim ; j++) {
    int    k,l ;
    for(k = 0 ; k < nn ; k++) for(l = 0 ; l < dim_h ; l++) {
      gu[i][j] += U(k,i)*DH(k,l)*CJ(l,j) ;
    }
  }
  
  /* Linearized strain tensor */
  for(i = 0 ; i < 3 ; i++) for(j = 0 ; j < 3 ; j++) {
    EPS(i,j) = (gu[i][j] + gu[j][i])*0.5 ;
  }
  
  /* symmetric cases: axisymmetrical or spherical */
  if(Symmetry_IsCylindrical(sym) || Symmetry_IsSpherical(sym)) {
    double radius = 0. ;
    double u_r   = 0. ;
    
    for(i = 0 ; i < nn ; i++) {
      double* x = Element_GetNodeCoordinate(el,i) ;
      
      radius += h[i]*x[0] ;
      u_r   += h[i]*U(i,0) ;
    }
    
    EPS(2,2) += u_r/radius ;
    if(Symmetry_IsSpherical(sym)) EPS(1,1) += u_r/radius ;
  }
  
  return(eps) ;
#undef U
#undef DH
#undef EPS
#undef CJ
}
/* End */

#undef _INLINE_
