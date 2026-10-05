#ifndef LOADS_H
#define LOADS_H


/* Forward declarations */
struct Loads_t; //typedef struct Loads_t        Loads_t ;
struct DataFile_t;
struct Fields_t;
struct Functions_t;
struct Load_t;


extern Loads_t* (Loads_New)   (Fields_t*,Functions_t*);
extern Loads_t* (Loads_Create)(DataFile_t*,Fields_t*,Functions_t*) ;
extern void     (Loads_Scan)(Loads_t*,DataFile_t*);
extern void     (Loads_Delete)(void*) ;


#define Loads_MaxNbOfLoads             (100)

#define Loads_GetNbOfLoads(LOADS)        ((LOADS)->GetNbOfLoads())
#define Loads_GetLoad(LOADS)             ((LOADS)->GetLoad())

#define Loads_SetNbOfLoads(LOADS,A)      ((LOADS)->SetNbOfLoads(A))
#define Loads_SetLoad(LOADS,A)           ((LOADS)->SetLoad(A))

#define Loads_EmplaceBack(LOADS,...)     ((LOADS)->EmplaceBack(__VA_ARGS__))


#include <string>
#include <stdexcept>

struct Loads_t {
  private:
  size_t _n_cg ;
  Load_t* _cg ;

  public:
  size_t GetCapacity(){return Loads_MaxNbOfLoads;}
  void EmplaceBack(std::string const& region,std::string const& equation,std::string const& type,size_t const& fieldindex,size_t const& functionindex){
    EmplaceBack(region.c_str(),equation.c_str(),type.c_str(),fieldindex,functionindex);
  }
  void EmplaceBack(char const*,char const*,char const*,size_t const&,size_t const&);

  size_t GetNbOfLoads() const {return _n_cg;}
  Load_t* GetLoad(){return _cg;}

  void SetNbOfLoads(size_t n){
    if(n >= GetCapacity()) {
      throw std::length_error("Maximum number of loads reached");
    }
    _n_cg = n;
  }
  void SetLoad(Load_t* a){_cg = a;}
} ;


#include "Load.h"

  inline void Loads_t::EmplaceBack(char const* region,char const* equation,char const* type,size_t const& fieldindex,size_t const& functionindex){
    Load_t* load = _cg + _n_cg;
    
    if(_n_cg >= GetCapacity()) {
      throw std::length_error("Maximum number of loadings reached");
    }

    load->Set(region,equation,type,fieldindex,functionindex);
    _n_cg++;
  }


#endif
