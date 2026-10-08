#ifndef PERIODICITY_H
#define PERIODICITY_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Periodicity_t;         //typedef struct Periodicity_t        Periodicity_t ;
struct DataFile_t;

extern Periodicity_t* (Periodicity_New)(void) ;
extern void           (Periodicity_Delete)(void*) ;
extern void           (Periodicity_Scan)(Periodicity_t*,DataFile_t*) ;


#define Periodicity_MaxLengthOfRegionName      Region_MaxLengthOfRegionName

#define Periodicity_GetMasterRegion(P)         ((P)->GetMasterRegion())
#define Periodicity_GetSlaveRegion(P)          ((P)->GetSlaveRegion())
#define Periodicity_GetMasterRegionName(P)     ((P)->GetMasterRegionName())
#define Periodicity_GetSlaveRegionName(P)      ((P)->GetSlaveRegionName())
#define Periodicity_GetPeriodVector(P)         ((P)->GetPeriodVector())

#define Periodicity_SetMasterRegion(P,A)       ((P)->SetMasterRegion(A))
#define Periodicity_SetSlaveRegion(P,A)        ((P)->SetSlaveRegion(A))
#define Periodicity_SetMasterRegionName(P,A)   ((P)->SetMasterRegionName(A))
#define Periodicity_SetSlaveRegionName(P,A)    ((P)->SetSlaveRegionName(A))
#define Periodicity_SetPeriodVector(P,A)       ((P)->SetPeriodVector(A))


#define Periodicity_Set(P,...)                 ((P)->Set(__VA_ARGS__))


struct Periodicity_t {
  int     _MasterRegion ;
  int     _SlaveRegion ;
  char*   _MasterRegionName ;
  char*   _SlaveRegionName ;
  double* _PeriodVector ;

  /* The getters */
  int     GetMasterRegion(){return _MasterRegion ;}
  int     GetSlaveRegion(){return _SlaveRegion ;}
  char*   GetMasterRegionName(){return _MasterRegionName ;}
  char*   GetSlaveRegionName(){return _SlaveRegionName ;}
  double* GetPeriodVector(){return _PeriodVector ;}

  /* The setters */
  void SetMasterRegion(int const& a){_MasterRegion = a;}
  void SetSlaveRegion(int const& a){_SlaveRegion = a;}
  void SetMasterRegionName(char* a){_MasterRegionName = a;}
  void SetSlaveRegionName(char* a){_SlaveRegionName = a;}
  void SetPeriodVector(double* a){_PeriodVector = a;}

  Set(char const* masterreg,char const* slavereg,double const* periodvector) {
    strcpy(_MasterRegionName,masterreg) ;
    strcpy(_SlaveRegionName,slavereg) ;
    for(int i = 0 ; i < 3 ; i++) {
      _PeriodVector[i] = periodvector[i] ;
    }
  }
} ;


#ifdef __CPLUSPLUS
}
#endif

/* Needs for the macros */
#include "Region.h"
#endif
