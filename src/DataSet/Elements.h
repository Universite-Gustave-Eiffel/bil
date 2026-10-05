#ifndef ELEMENTS_H
#define ELEMENTS_H

#ifdef __CPLUSPLUS
extern "C" {
#endif



/* Forward declarations */
struct Elements_t; //typedef struct Elements_t     Elements_t ;
struct IntFcts_t;
struct ShapeFcts_t;
struct Buffers_t;
struct Element_t;
struct Node_t;
struct Regions_t;
struct Materials_t;


#include <stdio.h>
extern Elements_t*  (Elements_New)                      (const size_t,const size_t) ;
extern void         (Elements_Delete)                   (void*) ;
extern void         (Elements_CreateMore)               (Elements_t*) ;
extern void         (Elements_LinkUpToMaterials)        (Elements_t*,Materials_t*) ;
extern void         (Elements_DefineProperties)         (Elements_t*) ;
extern size_t       (Elements_ComputeNbOfMatrixEntries) (Elements_t*) ;
extern size_t       (Elements_ComputeNbOfSelectedMatrixEntries)(Elements_t*,const int) ;
extern void         (Elements_UpdateMatrixRowColumnIndexes)(Elements_t*) ;
extern void         (Elements_EliminateMatrixRowColumnIndexesOfOverlappingNodes)(Elements_t*) ;
extern void         (Elements_UpdateMatrixRowColumnIndexesOfOverlappingNodes)(Elements_t*) ;
extern void         (Elements_InitializeMatrixRowColumnIndexes)(Elements_t*) ;


/* Accessors */
#define Elements_GetNbOfElements(ELTS)           ((ELTS)->NbOfElements)
#define Elements_GetNbOfConnectivities(ELTS)     ((ELTS)->NbOfConnectivities)
#define Elements_GetElement(ELTS)                ((ELTS)->Element)
#define Elements_GetPointerToNode(ELTS)          ((ELTS)->PointerToNode)
#define Elements_GetRegions(ELTS)                ((ELTS)->Regions)
#define Elements_GetShapeFcts(ELTS)              ((ELTS)->ShapeFcts)
#define Elements_GetIntFcts(ELTS)                ((ELTS)->IntFcts)
#define Elements_GetBuffers(ELTS)                ((ELTS)->Buffers)
#define Elements_GetMaximumSizeOfElements(ELTS)  ((ELTS)->MaximumSizeOfElements)
#define Elements_GetMinimumSizeOfElements(ELTS)  ((ELTS)->MinimumSizeOfElements)




typedef Node_t*   Node_tt ;

struct Elements_t {
  Element_t* Element ;
  Node_tt* PointerToNode ;
  Regions_t*   Regions ;
  ShapeFcts_t* ShapeFcts ;    /* Shape functions */
  IntFcts_t*   IntFcts ;      /* Interpolation functions */
  Buffers_t*  Buffers ;
  double      MaximumSizeOfElements ;
  double      MinimumSizeOfElements ;
  size_t NbOfElements ;
  size_t NbOfConnectivities ;
} ;


#ifdef __CPLUSPLUS
}
#endif
#endif
