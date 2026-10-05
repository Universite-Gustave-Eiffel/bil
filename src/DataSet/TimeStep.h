#ifndef TIMESTEP_H
#define TIMESTEP_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct TimeStep_t;
struct DataFile_t;
struct ObVals_t;
struct Nodes_t;
struct Solution_t;


extern TimeStep_t*  (TimeStep_New)(ObVals_t*,DataFile_t* = nullptr) ;
extern TimeStep_t*  (TimeStep_Create)(ObVals_t*,DataFile_t*) ;
extern void         (TimeStep_Scan)(TimeStep_t*,DataFile_t*);
extern void         (TimeStep_Delete)(void*) ;
extern double       (TimeStep_ComputeTimeStep)(TimeStep_t*,Solution_t*,double,double) ;


#define TimeStep_GetInitialTimeStep(TS)       ((TS)->GetInitialTimeStep())
#define TimeStep_GetMaximumTimeStep(TS)       ((TS)->GetMaximumTimeStep())
#define TimeStep_GetMinimumTimeStep(TS)       ((TS)->GetMinimumTimeStep())
#define TimeStep_GetMaximumCommonRatio(TS)    ((TS)->GetMaximumCommonRatio())
#define TimeStep_GetReductionFactor(TS)       ((TS)->GetReductionFactor())
#define TimeStep_GetObVals(TS)                ((TS)->GetObVals())
#define TimeStep_GetLocation(TS)              ((TS)->GetLocation())
#define TimeStep_GetDataFile(TS)              ((TS)->GetDataFile())

#define TimeStep_SetInitialTimeStep(TS,A)     ((TS)->SetInitialTimeStep(A))
#define TimeStep_SetMaximumTimeStep(TS,A)     ((TS)->SetMaximumTimeStep(A))
#define TimeStep_SetMinimumTimeStep(TS,A)     ((TS)->SetMinimumTimeStep(A))
#define TimeStep_SetMaximumCommonRatio(TS,A)  ((TS)->SetMaximumCommonRatio(A))
#define TimeStep_SetReductionFactor(TS,A)     ((TS)->SetReductionFactor(A))
#define TimeStep_SetObVals(TS,A)              ((TS)->SetObVals(A))
#define TimeStep_SetLocation(TS,A)            ((TS)->SetLocation(A))
#define TimeStep_SetDataFile(TS,A)            ((TS)->SetDataFile(A))

#define TimeStep_Set(TS,...)                  ((TS)->Set(__VA_ARGS__))



/* Accesss to Objective Variations */
#define TimeStep_GetObVal(TS) \
        ObVals_GetObVal(TimeStep_GetObVals(TS))


/* Time location management */
#define TimeStep_SetLocationAtBegin(TS) TimeStep_SetLocation(TS,0)
        
#define TimeStep_SetLocationInBetween(TS) TimeStep_SetLocation(TS,1)
        
#define TimeStep_SetLocationAtEnd(TS) TimeStep_SetLocation(TS,2)
        
#define TimeStep_IsLocatedAtBegin(TS) (TimeStep_GetLocation(TS) == 0)
        
#define TimeStep_IsLocatedInBetween(TS) (TimeStep_GetLocation(TS) == 1)
        
#define TimeStep_IsLocatedAtEnd(TS) (TimeStep_GetLocation(TS) == 2)
        
        

/* The sequential index */
#define TimeStep_GetSequentialIndex(TS) \
        DataFile_GetSequentialIndex(TimeStep_GetDataFile(TS))



struct TimeStep_t {           /* Time step management */
  double _dtini ;              /* Initial time step */
  double _dtmax ;              /* Maximum time step */
  double _dtmin ;              /* Minimum time step */
  double _raison ;             /* Maximum common ratio */
  double _fr ;                 /* Factor reducing the time step */
  ObVals_t* _obvals ;          /* Objective variations */
  char   _loc ;                /* Time location */
  DataFile_t*    _datafile ;   /* data file */

  double GetInitialTimeStep(){return _dtini ;}
  double GetMaximumTimeStep(){return _dtmax ;}
  double GetMinimumTimeStep(){return _dtmin ;}
  double GetMaximumCommonRatio(){return _raison ;}
  double GetReductionFactor(){return _fr ;}
  ObVals_t* GetObVals(){return _obvals ;}
  char   GetLocation(){return _loc ;}
  DataFile_t* GetDataFile(){return _datafile ;}

  void SetInitialTimeStep(double const& a){_dtini = a;}
  void SetMaximumTimeStep(double const& a){_dtmax = a;}
  void SetMinimumTimeStep(double const& a){_dtmin = a;}
  void SetMaximumCommonRatio(double const& a){_raison = a;}
  void SetReductionFactor(double const& a){_fr = a;}
  void SetObVals(ObVals_t* a){_obvals = a;}
  void SetLocation(char const a){_loc = a;}
  void SetDataFile(DataFile_t* a){_datafile = a;}

  void Set(double const& dtini,double const& dtmax,double const& dtmin = 0,double const& rfac = 0.5,double const& ratio = 1.5){
    /* Dtini */
    SetInitialTimeStep(dtini) ;
    /* Dtmax */
    SetMaximumTimeStep(dtmax) ;
    /* Dtmin */
    SetMinimumTimeStep(dtmin) ;
    /* Reduction factor */
    SetReductionFactor(rfac) ;
    /* Common ratio */
    SetMaximumCommonRatio(ratio) ;
    
    /* Checkings */
    if(GetInitialTimeStep() <= 0) {
      arret("TimeStep_t::Set: Dtini is not > 0") ;
    }
    if(GetMaximumTimeStep() <= 0) {
      arret("TimeStep__::Set: Dtmax is not > 0") ;
    }
  }
} ;


#ifdef __CPLUSPLUS
}
#endif

/* Need for the macros */
#include "ObVals.h"
#endif
