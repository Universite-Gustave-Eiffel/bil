#ifndef PERIODICITIES_H
#define PERIODICITIES_H

#ifdef __CPLUSPLUS
extern "C" {
#endif

/* Forward declaration */
struct Periodicities_t;
struct DataFile_t;
struct Mesh_t;
struct Graph_t;
struct Periodicity_t;


extern Periodicities_t* (Periodicities_New)(void) ;
extern Periodicities_t* (Periodicities_Create)(DataFile_t*) ;
extern void             (Periodicities_Scan)(Periodicities_t*,DataFile_t*);
extern void             (Periodicities_Delete)(void*) ;
extern void             (Periodicities_EliminateMatrixRowColumnIndexes)(Mesh_t*) ;
extern void             (Periodicities_UpdateMatrixRowColumnIndexes)(Mesh_t*) ;
extern void             (Periodicities_UpdateGraph)(Mesh_t*,Graph_t*) ;



#define Periodicities_MaxNbOfPeriodicities             (100)


#define Periodicities_GetNbOfPeriodicities(PS) ((PS)->GetNbOfPeriodicities())
#define Periodicities_GetPeriodicity(PS)       ((PS)->GetPeriodicity())

#define Periodicities_SetNbOfPeriodicities(PS,A) ((PS)->SetNbOfPeriodicities(A))
#define Periodicities_SetPeriodicity(PS,A)       ((PS)->SetPeriodicity(A))


struct Periodicities_t {
  private:
  size_t   _nbperiod ;
  Periodicity_t* _periodicity ;

  public:
  size_t GetCapacity(){return Periodicities_MaxNbOfPeriodicities;}

  /* The getters */
  size_t   GetNbOfPeriodicities(){return _nbperiod ;}
  Periodicity_t* GetPeriodicity(){return _periodicity ;}

  /* The setters */
  void SetNbOfPeriodicities(size_t const& a){_nbperiod = a;}
  void SetPeriodicity(Periodicity_t* a){_periodicity = a;}
} ;



#ifdef __CPLUSPLUS
}
#endif
#endif
