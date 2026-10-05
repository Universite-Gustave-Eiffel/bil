#include <stdio.h>
#include "ElementSol.h"
#include "Message.h"
#include "Mry.h"
#include "GenericData.h"



/* Extern functions */

ElementSol_t*   (ElementSol_New)(void)
{
  ElementSol_t* elementsol = (ElementSol_t*) Mry_New(ElementSol_t) ;
  
  #if 1
  ElementSol_GetImplicitGenericData(elementsol) = GenericData_New() ;
  ElementSol_GetExplicitGenericData(elementsol) = GenericData_New() ;
  ElementSol_GetConstantGenericData(elementsol) = GenericData_New() ;
  #endif
  
  #if 0
  ElementSol_GetImplicitGenericData(elementsol) = NULL ;
  ElementSol_GetExplicitGenericData(elementsol) = NULL ;
  ElementSol_GetConstantGenericData(elementsol) = NULL ;
  #endif
  
  //ElementSol_GetPreviousElementSol(elementsol)  = NULL ;
  //ElementSol_GetNextElementSol(elementsol)      = NULL ;
  
  return(elementsol) ;
}



void   (ElementSol_Delete)(void* self)
{
  ElementSol_t*   elementsol = (ElementSol_t*) self ;
  
  {
    GenericData_t* gdat = ElementSol_GetImplicitGenericData(elementsol) ;
    
    if(gdat) {
      GenericData_Delete(gdat) ;
      Mry_Free(gdat) ;
      ElementSol_GetImplicitGenericData(elementsol) = NULL ;
    }
  }
  
  {
    GenericData_t* gdat = ElementSol_GetExplicitGenericData(elementsol) ;
    
    if(gdat) {
      GenericData_Delete(gdat) ;
      Mry_Free(gdat) ;
      ElementSol_GetExplicitGenericData(elementsol) = NULL ;
    }
  }
  
  {
    GenericData_t* gdat = ElementSol_GetConstantGenericData(elementsol) ;
    
    if(gdat) {
      GenericData_Delete(gdat) ;
      Mry_Free(gdat) ;
      ElementSol_GetConstantGenericData(elementsol) = NULL ;
    }
  }
}



void (ElementSol_AllocateMemoryForImplicitTerms)(ElementSol_t* elementsol)
/**  Allocate the memory space for the implicit terms
 */
{
  GenericData_t* gdat = ElementSol_GetImplicitGenericData(elementsol) ;
  size_t ni = ElementSol_GetNbOfImplicitTerms(elementsol) ;
  double* vi = (double*) Mry_New(double,ni) ;
        
  GenericData_Initialize(gdat,ni,vi,"implicit terms") ;
}



void (ElementSol_AllocateMemoryForExplicitTerms)(ElementSol_t* elementsol)
/**  Allocate the memory space for the explicit terms
 */
{
  GenericData_t* gdat = ElementSol_GetExplicitGenericData(elementsol) ;
  size_t ne = ElementSol_GetNbOfExplicitTerms(elementsol) ;
  double* ve = (double*) Mry_New(double,ne) ;
        
  GenericData_Initialize(gdat,ne,ve,"explicit terms") ;
}



void (ElementSol_AllocateMemoryForConstantTerms)(ElementSol_t* elementsol)
/**  Allocate the memory space for the constant terms
 */
{
  GenericData_t* gdat = ElementSol_GetConstantGenericData(elementsol) ;
  size_t n0 = ElementSol_GetNbOfConstantTerms(elementsol) ;
  double* v0 = (double*) Mry_New(double,n0) ;
        
  GenericData_Initialize(gdat,n0,v0,"constant terms") ;
}




void (ElementSol_Copy)(ElementSol_t* elementsol_d,ElementSol_t* elementsol_s)
/** Copy the (im/ex)plicit and constant terms 
 *  from elementsol_src to elementsol_dest */
{
  
  /* Implicit terms */
  {
    double* vi_s = (double*) ElementSol_GetImplicitTerm(elementsol_s) ;
    double* vi_d = (double*) ElementSol_GetImplicitTerm(elementsol_d) ;
        
    if(vi_d != vi_s) {
      size_t nvi = ElementSol_GetNbOfImplicitTerms(elementsol_s) ;
      
      for(size_t i = 0 ; i < nvi ; i++) {
        vi_d[i] = vi_s[i] ;
      }
    }
  }

  /* Explicit terms */
  {
    double* ve_s = (double*) ElementSol_GetExplicitTerm(elementsol_s) ;
    double* ve_d = (double*) ElementSol_GetExplicitTerm(elementsol_d) ;
      
    if(ve_d != ve_s) {
      size_t nve = ElementSol_GetNbOfExplicitTerms(elementsol_s) ;
      
      for(size_t i = 0 ; i < nve ; i++) {
        ve_d[i] = ve_s[i] ;
      }
    }
  }

  /* Constant terms */
  {
    double* vc_s = (double*) ElementSol_GetConstantTerm(elementsol_s) ;
    double* vc_d = (double*) ElementSol_GetConstantTerm(elementsol_d) ;
        
    if(vc_d != vc_s) {
      size_t nvc = ElementSol_GetNbOfConstantTerms(elementsol_s) ;
      
      for(size_t i = 0 ; i < nvc ; i++) {
        vc_d[i] = vc_s[i] ;
      }
    }
  }
}
