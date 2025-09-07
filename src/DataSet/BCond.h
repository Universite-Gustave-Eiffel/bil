#ifndef BCOND_H
#define BCOND_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct BCond_t; //typedef struct BCond_t        BCond_t ;
struct DataFile_t;
struct Node_t;
struct Functions_t;
struct Fields_t;


extern BCond_t*  (BCond_New)     (void) ;
extern void      (BCond_Delete)  (void*) ;
extern void      (BCond_Scan)    (BCond_t*,DataFile_t*) ;
extern void      (BCond_AssignBoundaryConditionsAtOverlappingNodes)(BCond_t*,Node_t*,int,double);



#define BCond_MaxLengthOfKeyWord        (30)
#define BCond_MaxLengthOfRegionName      Region_MaxLengthOfRegionName


#define BCond_GetRegionName(BC)          ((BC)->RegionName)
#define BCond_GetNameOfUnknown(BC)       ((BC)->NameOfUnknown)
#define BCond_GetNameOfEquation(BC)      ((BC)->NameOfEquation)
#define BCond_GetFunction(BC)            ((BC)->Function)
#define BCond_GetField(BC)               ((BC)->Field)
#define BCond_GetEntryInDataFile(BC)     ((BC)->EntryInDataFile)
#define BCond_GetFunctionIndex(BC)       ((BC)->FunctionIndex)
#define BCond_GetFieldIndex(BC)          ((BC)->FieldIndex)
#define BCond_GetFunctions(BC)           ((BC)->Functions)
#define BCond_GetFields(BC)              ((BC)->Fields)




struct BCond_t {              /* Boundary condition */
  Function_t* Function ;
  Field_t* Field ;
  Functions_t* Functions ;
  Fields_t* Fields ;
  char*  EntryInDataFile ;     /* Entry point in the string of data */
  char*  RegionName ;
  char*  NameOfUnknown ;       /* Name of unknown to be eliminated */
  char*  NameOfEquation ;      /* Name of equation to be eliminated */
  int    FunctionIndex ;
  int    FieldIndex ;
} ;


/* Old notations which I try to eliminate little by little */
#define cond_t    BCond_t


#ifdef __CPLUSPLUS
}
#endif

#include "Region.h"
#endif
