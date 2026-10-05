#include <stdio.h>
#include "ElementsSol.h"
#include "ElementSol.h"
#include "Mesh.h"
#include "Message.h"
#include "Mry.h"


/* Extern functions */

ElementsSol_t*   (ElementsSol_Create)(Mesh_t* mesh)
{
  ElementsSol_t* elementssol = (ElementsSol_t*) Mry_New(ElementsSol_t) ;
  
  
  {
    size_t NbOfElements = Mesh_GetNbOfElements(mesh) ;
    ElementSol_t* elementsol = (ElementSol_t*) Mry_New(ElementSol_t,NbOfElements) ;

    ElementsSol_SetElementSol(elementssol,elementsol) ;
    ElementsSol_SetNbOfElements(elementssol,NbOfElements) ;
    
    
    /* Initialization */
    {      
      for(size_t i = 0 ; i < NbOfElements ; i++) {
        ElementSol_t* elementsol_i = ElementSol_New() ;
        
        elementsol[i] = elementsol_i[0] ;
        Mry_Free(elementsol_i) ;
      }
    }
  }
  
  return(elementssol) ;
}



void (ElementsSol_Delete)(void* self)
{
  ElementsSol_t* elementssol = (ElementsSol_t*) self ;
  
  {
    size_t NbOfElements = ElementsSol_GetNbOfElements(elementssol) ;
    ElementSol_t* elementsol = ElementsSol_GetElementSol(elementssol) ;
    
    if(elementsol) {      
      for(size_t i = 0 ; i < NbOfElements ; i++) {
        ElementSol_t* elementsol_i = elementsol + i ;
      
        ElementSol_Delete(elementsol_i) ;
      }
    
      Mry_Free(elementsol) ;
      ElementsSol_SetElementSol(elementssol,NULL) ;
    }
  }
}



void (ElementsSol_AllocateMemoryForImplicitTerms)(ElementsSol_t* elementssol)
/**  Allocate the memory space for the implicit terms
 *   assuming that the numbers of these terms have been initialized
 *   in "elementssol".
 */
{
  size_t NbOfElements = ElementsSol_GetNbOfElements(elementssol) ;
  ElementSol_t* elementsol = ElementsSol_GetElementSol(elementssol) ;
  
  {
    for(size_t i = 0 ; i < NbOfElements ; i++) {
      ElementSol_AllocateMemoryForImplicitTerms(elementsol + i) ;
    }
  }
}



void (ElementsSol_AllocateMemoryForExplicitTerms)(ElementsSol_t* elementssol)
/**  Allocate the memory space for the explicit terms
 *   assuming that the numbers of these terms have been initialized
 *   in "elementssol".
 */
{
  size_t NbOfElements = ElementsSol_GetNbOfElements(elementssol) ;
  ElementSol_t* elementsol = ElementsSol_GetElementSol(elementssol) ;
  
    {
      for(size_t i = 0 ; i < NbOfElements ; i++) {
        ElementSol_AllocateMemoryForExplicitTerms(elementsol + i) ;
      }
    }
}



void (ElementsSol_AllocateMemoryForConstantTerms)(ElementsSol_t* elementssol)
/**  Allocate the memory space for the constant terms
 *   assuming that the numbers of these terms have been initialized
 *   in "elementssol".
 */
{
  size_t NbOfElements = ElementsSol_GetNbOfElements(elementssol) ;
  ElementSol_t* elementsol = ElementsSol_GetElementSol(elementssol) ;
  
    {
      for(size_t i = 0 ; i < NbOfElements ; i++) {
        ElementSol_AllocateMemoryForConstantTerms(elementsol + i) ;
      }
    }
}



void (ElementsSol_Copy)(ElementsSol_t* elementssol_dest,ElementsSol_t* elementssol_src)
/** Copy the (im/ex)plicit and constant terms 
 *  from elementssol_src to elementssol_dest */
{
  size_t nelts = ElementsSol_GetNbOfElements(elementssol_src) ;
  ElementSol_t* elementsol_s = ElementsSol_GetElementSol(elementssol_src) ;
  ElementSol_t* elementsol_d = ElementsSol_GetElementSol(elementssol_dest) ;
  
  ElementsSol_SetNbOfElements(elementssol_dest,nelts) ;

  for(size_t ie = 0 ; ie < nelts ; ie++) {
    ElementSol_t* elementsoli_s = elementsol_s + ie ;
    ElementSol_t* elementsoli_d = elementsol_d + ie ;
        
    ElementSol_Copy(elementsoli_d,elementsoli_s);
  }
}
