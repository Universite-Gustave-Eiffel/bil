#ifndef ICONDS_H
#define ICONDS_H


/* Forward declarations */
struct IConds_t; //typedef struct IConds_t       IConds_t ;
struct DataFile_t;
struct Functions_t;
struct Mesh_t;
struct Fields_t;
struct ICond_t;


extern IConds_t* (IConds_New)(Fields_t*,Functions_t*);
extern IConds_t* (IConds_Create)(DataFile_t*,Fields_t*,Functions_t*) ;
extern void      (IConds_Scan)(IConds_t*,DataFile_t*);
extern void      (IConds_Delete)(void*) ;
extern void      (IConds_AssignInitialConditions)(IConds_t*,Mesh_t*,double) ;


#define IConds_MaxNbOfIConds             (100)
#define IConds_MaxLengthOfKeyWord        (30)
#define IConds_MaxLengthOfFileName       (60)


#define IConds_GetNbOfIConds(ICS)              ((ICS)->GetNbOfIConds())
#define IConds_GetICond(ICS)                   ((ICS)->GetICond())
#define IConds_GetFileNameOfNodalValues(ICS)   ((ICS)->GetFileNameOfNodalValues())

#define IConds_SetNbOfIConds(ICS,A)            ((ICS)->SetNbOfIConds(A))
#define IConds_SetICond(ICS,A)                 ((ICS)->SetICond(A))
#define IConds_SetFileNameOfNodalValues(ICS,A) ((ICS)->SetFileNameOfNodalValues(A))

#define IConds_EmplaceBack(ICS,...)            ((ICS)->EmplaceBack(__VA_ARGS__))

#define IConds_Print(ICS)                      ((ICS)->Print())


#include <string>
#include <stdexcept>

struct IConds_t {
  private:
  char*  _file ;
  size_t _n_ic ;
  ICond_t* _ic ;

  public:
  size_t GetCapacity(){return IConds_MaxNbOfIConds;}
  void EmplaceBack(std::string const& region,std::string const& unknown,size_t const& ifld,size_t const& ifct){
    EmplaceBack(region.c_str(),unknown.c_str(),ifld,ifct);
  }
  void EmplaceBack(std::string const& region,std::string const& unknown,std::string const& filename,size_t const& ifct){
    EmplaceBack(region.c_str(),unknown.c_str(),filename.c_str(),ifct);
  }
  void EmplaceBack(char const*,char const*,size_t const&,size_t const&);
  void EmplaceBack(char const*,char const*,char const*,size_t const&);

  char*  GetFileNameOfNodalValues(){return _file;}
  size_t GetNbOfIConds(){return _n_ic;}
  ICond_t* GetICond(){return _ic;}

  void SetFileNameOfNodalValues(char* a){_file = a;}
  void SetNbOfIConds(size_t a){_n_ic = a;}
  void SetICond(ICond_t* a){_ic = a;}

  void Print();
} ;


#include "ICond.h"

  inline void IConds_t::EmplaceBack(char const* region,char const* unknown,size_t const& ifld,size_t const& ifct) {
    ICond_t* icond = _ic + _n_ic;
    
    if(_n_ic >= GetCapacity()) {
      throw std::length_error("Maximum number of initial conditions reached");
    }

    icond->Set(region,unknown,ifld,ifct);
    _n_ic++;
  }

  inline void IConds_t::EmplaceBack(char const* region,char const* unknown,char const* filename,size_t const& ifct) {
    ICond_t* icond = _ic + _n_ic;
    
    if(_n_ic >= GetCapacity()) {
      throw std::length_error("Maximum number of initial conditions reached");
    }

    icond->Set(region,unknown,filename,ifct);
    _n_ic++;
  }
  
  inline void IConds_t::Print(){
    #define PRINT(...) fprintf(stdout,__VA_ARGS__)
    for(size_t i = 0 ; i < _n_ic ; i++) {
      PRINT("Initial Condition(%lu):\n",i) ;
      ICond_Print(_ic + i);
    }
    #undef PRINT
  }


#endif
