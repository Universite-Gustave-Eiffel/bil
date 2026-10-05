#ifndef ITERPROCESS_H
#define ITERPROCESS_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct IterProcess_t; //typedef struct IterProcess_t  IterProcess_t ;
struct DataFile_t;
struct ObVals_t;
struct Node_t;
struct Nodes_t;
struct Solver_t;


extern IterProcess_t*  (IterProcess_New)(ObVals_t*) ;
extern IterProcess_t*  (IterProcess_Create)(DataFile_t*,ObVals_t*) ;
extern void            (IterProcess_Scan)(IterProcess_t*,DataFile_t*);
extern void            (IterProcess_Delete)(void*) ;
extern int             (IterProcess_SetCurrentError)(IterProcess_t*,Nodes_t*,Solver_t*) ;
extern void            (IterProcess_PrintCurrentError)(IterProcess_t*) ;


#define IterProcess_GetNbOfIterations(IPR)           ((IPR)->GetNbOfIterations())
#define IterProcess_GetNbOfRepetitions(IPR)          ((IPR)->GetNbOfRepetitions())
#define IterProcess_GetTolerance(IPR)                ((IPR)->GetTolerance())
#define IterProcess_GetRepetitionIndex(IPR)          ((IPR)->GetRepetitionIndex())
#define IterProcess_GetIterationIndex(IPR)           ((IPR)->GetIterationIndex())
#define IterProcess_GetError(IPR)                    ((IPR)->GetError())
#define IterProcess_GetObValIndexOfCurrentError(IPR) ((IPR)->GetObValIndexOfCurrentError())
#define IterProcess_GetNodeIndexOfCurrentError(IPR)  ((IPR)->GetNodeIndexOfCurrentError())
#define IterProcess_GetNodeOfCurrentError(IPR)       ((IPR)->GetNodeOfCurrentError())
#define IterProcess_GetObVals(IPR)                   ((IPR)->GetObVals())

#define IterProcess_SetNbOfIterations(IPR,A)           ((IPR)->SetNbOfIterations(A))
#define IterProcess_SetNbOfRepetitions(IPR,A)          ((IPR)->SetNbOfRepetitions(A))
#define IterProcess_SetTolerance(IPR,A)                ((IPR)->SetTolerance(A))
#define IterProcess_SetRepetitionIndex(IPR,A)          ((IPR)->SetRepetitionIndex(A))
#define IterProcess_SetIterationIndex(IPR,A)           ((IPR)->SetIterationIndex(A))
#define IterProcess_SetError(IPR,A)                    ((IPR)->SetError(A))
#define IterProcess_SetObValIndexOfCurrentError(IPR,A) ((IPR)->SetObValIndexOfCurrentError(A))
#define IterProcess_SetNodeIndexOfCurrentError(IPR,A)  ((IPR)->SetNodeIndexOfCurrentError(A))
#define IterProcess_SetNodeOfCurrentError(IPR,A)       ((IPR)->SetNodeOfCurrentError(A))
#define IterProcess_SetObVals(IPR,A)                   ((IPR)->SetObVals(A))

#define IterProcess_Set(IPR,...)                       ((IPR)->Set(__VA_ARGS__))
        


#define IterProcess_GetObVal(IPR) \
        ObVals_GetObVal(IterProcess_GetObVals(IPR))
        

/* Operations on iterations */
#define IterProcess_IncrementIterationIndex(IPR) \
        (IterProcess_SetIterationIndex(IPR,IterProcess_GetIterationIndex(IPR)+1))

#define IterProcess_LastIterationIsNotReached(IPR) \
        (IterProcess_GetIterationIndex(IPR) < IterProcess_GetNbOfIterations(IPR))

#define IterProcess_InitializeIterations(IPR) \
        (IterProcess_SetIterationIndex(IPR,0))


/* Operations on repetitions */
#define IterProcess_IncrementRepetitionIndex(IPR) \
       (IterProcess_SetRepetitionIndex(IPR,IterProcess_GetRepetitionIndex(IPR)+1))

#define IterProcess_LastRepetitionIsNotReached(IPR) \
        (IterProcess_GetRepetitionIndex(IPR) < IterProcess_GetNbOfRepetitions(IPR))

#define IterProcess_InitializeRepetitions(IPR) \
        (IterProcess_SetRepetitionIndex(IPR,0))


/* Operations on convergence */
#define IterProcess_ConvergenceIsAttained(IPR) \
        (IterProcess_GetError(IPR) < IterProcess_GetTolerance(IPR))

#define IterProcess_ConvergenceIsNotAttained(IPR) \
        (!IterProcess_ConvergenceIsAttained(IPR))


/* Error on which unknown? */
#define IterProcess_GetNameOfTheCurrentError(IPR) \
        (ObVal_GetNameOfUnknown(IterProcess_GetObVal(IPR) + IterProcess_GetObValIndexOfCurrentError(IPR)))


struct IterProcess_t {        /* Iterative process */
  int    _niter ;              /* Max nb of iterations */
  int    _iter ;               /* Current iteration index */
  int    _nrecom ;             /* Max nb of repetitions */
  int    _irecom ;             /* Current repetition index */
  double _tol ;                /* Tolerance */
  double _error ;              /* Current error */
  int    _obvalindex ;         /* Objective value index pertaining to the greatest error */
  size_t _nodeindex ;          /* Node index pertaining to the greatest error */
  Node_t* _node ;              /* Node pertaining to the greatest error */
  ObVals_t* _obvals ;          /* Objective variations */

  int    GetNbOfIterations(){return _niter ;}
  int    GetIterationIndex(){return _iter ;}
  int    GetNbOfRepetitions(){return _nrecom ;}
  int    GetRepetitionIndex(){return _irecom ;}
  double GetTolerance(){return _tol ;}
  double GetError(){return _error ;}
  int    GetObValIndexOfCurrentError(){return _obvalindex ;}
  size_t GetNodeIndexOfCurrentError(){return _nodeindex ;}
  Node_t* GetNodeOfCurrentError(){return _node ;}
  ObVals_t* GetObVals(){return _obvals ;}

  void SetNbOfIterations(int a){_niter = a;}
  void SetIterationIndex(int a){_iter = a;}
  void SetNbOfRepetitions(int a){_nrecom = a;}
  void SetRepetitionIndex(int a){_irecom = a;}
  void SetTolerance(double a){_tol = a;}
  void SetError(double a){_error = a;}
  void SetObValIndexOfCurrentError(int a){_obvalindex = a;}
  void SetNodeIndexOfCurrentError(size_t a){_nodeindex = a;}
  void SetNodeOfCurrentError(Node_t* a){_node = a;}
  void SetObVals(ObVals_t* a){_obvals = a;}

  void Set(int const& iter,double const& tol,int const& rep){
    /* Iterations */
    SetNbOfIterations(iter) ;
    /* Tolerance */
    SetTolerance(tol) ;
    /* Repetitions */
    SetNbOfRepetitions(rep) ;
  }
} ;


#ifdef __CPLUSPLUS
}
#endif

#include "ObVals.h"
#include "ObVal.h"
#endif
