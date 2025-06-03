#ifndef ELEMENTSOL_H
#define ELEMENTSOL_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct ElementSol_t; //typedef struct ElementSol_t   ElementSol_t ;
struct GenericData_t;


extern ElementSol_t* (ElementSol_New)(void) ;
extern void          (ElementSol_Delete)(void*) ;
extern void          (ElementSol_AllocateMemoryForImplicitTerms)(ElementSol_t*) ;
extern void          (ElementSol_AllocateMemoryForExplicitTerms)(ElementSol_t*) ;
extern void          (ElementSol_AllocateMemoryForConstantTerms)(ElementSol_t*) ;
extern void          (ElementSol_Copy)(ElementSol_t*,ElementSol_t*) ;


//#define ElementSol_GetPreviousElementSol(ES)    ((ES)->prev)
//#define ElementSol_GetNextElementSol(ES)        ((ES)->next)
#define ElementSol_GetImplicitGenericData(ES)   ((ES)->impgdat)
#define ElementSol_GetExplicitGenericData(ES)   ((ES)->expgdat)
#define ElementSol_GetConstantGenericData(ES)   ((ES)->cstgdat)




/* Nb of (im/ex)plicit and constant terms */
#define ElementSol_GetNbOfImplicitTerms(ES) \
        GenericData_GetNbOfData(ElementSol_GetImplicitGenericData(ES))

#define ElementSol_GetNbOfExplicitTerms(ES) \
        GenericData_GetNbOfData(ElementSol_GetExplicitGenericData(ES))

#define ElementSol_GetNbOfConstantTerms(ES) \
        GenericData_GetNbOfData(ElementSol_GetConstantGenericData(ES))
        
#define ElementSol_SetNbOfImplicitTerms(ES,X) \
        GenericData_SetNbOfData(ElementSol_GetImplicitGenericData(ES),X)

#define ElementSol_SetNbOfExplicitTerms(ES,X) \
        GenericData_SetNbOfData(ElementSol_GetExplicitGenericData(ES),X)

#define ElementSol_SetNbOfConstantTerms(ES,X) \
        GenericData_SetNbOfData(ElementSol_GetConstantGenericData(ES),X)




/* Access to (im/ex)plicit and constant terms */
#define ElementSol_GetImplicitTerm(ES) \
        GenericData_GetData(ElementSol_GetImplicitGenericData(ES))

#define ElementSol_GetExplicitTerm(ES) \
        GenericData_GetData(ElementSol_GetExplicitGenericData(ES))

#define ElementSol_GetConstantTerm(ES) \
        GenericData_GetData(ElementSol_GetConstantGenericData(ES))


/* Add (im/ex)plicit and constant generic data */
#define ElementSol_AddImplicitGenericData(ES,GD) \
        do { \
          ElementSol_GetImplicitGenericData(ES) = GenericData_Append(ElementSol_GetImplicitGenericData(ES),GD) ; \
        } while(0)
        
#define ElementSol_AddExplicitGenericData(ES,GD) \
        do { \
          ElementSol_GetExplicitGenericData(ES) = GenericData_Append(ElementSol_GetExplicitGenericData(ES),GD) ; \
        } while(0)
        
#define ElementSol_AddConstantGenericData(ES,GD) \
        do { \
          ElementSol_GetConstantGenericData(ES) = GenericData_Append(ElementSol_GetConstantGenericData(ES),GD) ; \
        } while(0)



/* Find (im/ex)plicit and constant data */
#define ElementSol_FindImplicitData(ES,...) \
        GenericData_FindData(ElementSol_GetImplicitGenericData(ES),__VA_ARGS__)
        
#define ElementSol_FindExplicitData(ES,...) \
        GenericData_FindData(ElementSol_GetExplicitGenericData(ES),__VA_ARGS__)
        
#define ElementSol_FindConstantData(ES,...) \
        GenericData_FindData(ElementSol_GetConstantGenericData(ES),__VA_ARGS__)






#define ElementSol_DeleteExplicitGenericData(ES) \
        do { \
          GenericData_t* gdat = ElementSol_GetExplicitGenericData(ES) ; \
          GenericData_Delete(gdat) ; \
          free(gdat) ; \
          ElementSol_GetExplicitGenericData(ES) = NULL ; \
        } while(0)
        
        
#define ElementSol_DeleteConstantGenericData(ES) \
        do { \
          GenericData_t* gdat = ElementSol_GetConstantGenericData(ES) ; \
          GenericData_Delete(gdat) ; \
          free(gdat) ; \
          ElementSol_GetConstantGenericData(ES) = NULL ; \
        } while(0)


struct ElementSol_t {              /* Element Solutions */
  GenericData_t* impgdat ;         /* Implicit generic data */
  GenericData_t* expgdat ;         /* Explicit generic data */
  GenericData_t* cstgdat ;         /* Constant generic data */
  //ElementSol_t*  prev ;            /* Previous Element Solutions */
  //ElementSol_t*  next ;            /* Next Element Solutions */
} ;


/* For the macros */
#include "GenericData.h"

#ifdef __CPLUSPLUS
}
#endif
#endif
