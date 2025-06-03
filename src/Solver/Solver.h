#ifndef SOLVER_H
#define SOLVER_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Solver_t; //typedef struct Solver_t       Solver_t ;
struct Mesh_t;
struct Options_t;
struct Solver_t;
struct Matrix_t;
struct Residu_t;
struct GenericData_t;
struct ResolutionMethod_t;


//extern Solver_t*  (Solver_Create)(Mesh_t*,Options_t*,const int) ;
extern Solver_t*  (Solver_Create)(Mesh_t*,Options_t*,const int,const int) ;
extern void       (Solver_Delete)(void*) ;
extern void       (Solver_Print)(Solver_t*,char*) ;


#define Solver_GetResolutionMethod(SV)  ((SV)->mth)
#define Solver_GetNbOfRows(SV)          ((SV)->n)
#define Solver_GetNbOfColumns(SV)       ((SV)->n)
#define Solver_GetMatrix(SV)            ((SV)->a)
#define Solver_GetResidu(SV)            ((SV)->residu)
#define Solver_GetSolve(SV)             ((SV)->solve)
#define Solver_GetGenericWorkSpace(SV)  ((SV)->genericwork)
//#define Solver_GetRHS(SV)               ((SV)->b)
//#define Solver_GetSolution(SV)          ((SV)->x)


#if 1
#define Solver_GetRHS(SV) \
        Residu_GetRHS(Solver_GetResidu(SV))
              
#define Solver_GetSolution(SV) \
        Residu_GetSolution(Solver_GetResidu(SV))
#endif


#define Solver_GetMatrixIndex(SV) \
        Matrix_GetMatrixIndex(Solver_GetMatrix(SV))

#define Solver_GetResiduIndex(SV) \
        Residu_GetResiduIndex(Solver_GetResidu(SV))

#define Solver_Solve(SV) \
        (Solver_GetSolve(SV)(SV))



#define Solver_GetResolutionMethodType(SV) \
        ResolutionMethod_GetType(Solver_GetResolutionMethod(SV))

#define Solver_ResolutionMethodIs(SV,KEY) \
        ResolutionMethod_Is(Solver_GetResolutionMethod(SV),KEY)
        

#define Solver_AppendGenericWorkSpace(SV,GD) \
        Solver_GetGenericWorkSpace(SV) = GenericData_Append(Solver_GetGenericWorkSpace(SV),GD)


#define Solver_GetOptions(SV) \
        ResolutionMethod_GetOptions(Solver_GetResolutionMethod(SV))

#define Solver_NbOfThreads(SV) \
        Options_NbOfThreads(Solver_GetOptions(SV)) ;



/*  Typedef names of Methods */
typedef int  Solver_Solve_t(Solver_t*) ;


/* complete the structure types by using the typedef */
struct Solver_t {             /* System solver */
  Solver_Solve_t* solve ;
  ResolutionMethod_t* mth ;   /* Method */
  unsigned int    n ;         /* Nb of rows/columns */
  Matrix_t* a ;               /* Matrix */
  Residu_t* residu ;          /* Residu */
  GenericData_t* genericwork ; /* Working space */
  //double* b ;                 /* RHS */
  //double* x ;                 /* Solution */
} ;


#ifdef __CPLUSPLUS
}
#endif

#include "Matrix.h"
#include "Residu.h"
#include "GenericData.h"
#include "ResolutionMethod.h"
#include "Options.h"
#endif
