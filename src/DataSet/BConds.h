#ifndef BCONDS_H
#define BCONDS_H


/* Forward declarations */
struct BConds_t; //typedef struct BConds_t       BConds_t ;
struct DataFile_t;
struct Functions_t;
struct Fields_t;
struct Mesh_t;
struct BCond_t;


extern BConds_t* (BConds_New)(Fields_t*,Functions_t*);
extern BConds_t* (BConds_Create)(DataFile_t*,Fields_t*,Functions_t*) ;
extern void      (BConds_Scan)(BConds_t*,DataFile_t*);
extern void      (BConds_Delete)(void*) ;
extern void      (BConds_EliminateMatrixRowColumnIndexes)(BConds_t*,Mesh_t*) ;
extern void      (BConds_AssignBoundaryConditions)(BConds_t*,Mesh_t*,double) ;


#define BConds_MaxNbOfBConds             (100)


#define BConds_GetNbOfBConds(BCS)        ((BCS)->GetNbOfBConds())
#define BConds_GetBCond(BCS)             ((BCS)->GetBCond())

#define BConds_SetNbOfBConds(BCS,A)      ((BCS)->SetNbOfBConds(A))
#define BConds_SetBCond(BCS,A)           ((BCS)->SetBCond(A))

#define BConds_EmplaceBack(BCS,...)      ((BCS)->EmplaceBack(__VA_ARGS__))


#include <string>

struct BConds_t {
  private:
  size_t _n_cl ;
  BCond_t* _cl ;

  public:
  size_t GetCapacity(){return BConds_MaxNbOfBConds;}
  void EmplaceBack(std::string const& region,std::string const& equation,std::string const& unknown,size_t const& fieldindex,size_t const& functionindex){
    EmplaceBack(region.c_str(),equation.c_str(),unknown.c_str(),fieldindex,functionindex);
  }
  void EmplaceBack(std::string const& region,std::string const& unknown,size_t const& fieldindex,size_t const& functionindex){
    EmplaceBack(region.c_str(),unknown.c_str(),fieldindex,functionindex);
  }
  void EmplaceBack(char const*,char const*,char const*,size_t const&,size_t const&);
  void EmplaceBack(char const*,char const*,size_t const&,size_t const&);

  size_t GetNbOfBConds() const {return _n_cl;}
  BCond_t* GetBCond(){return _cl;}

  void SetNbOfBConds(size_t n){
    if(n >= GetCapacity()) {
      throw std::length_error("Maximum number of bconds reached");
    }
    _n_cl = n;
  }
  void SetBCond(BCond_t* a){_cl = a;}
} ;


#include "BCond.h"

  inline void BConds_t::EmplaceBack(char const* region,char const* unknown,size_t const& fieldindex,size_t const& functionindex){
    BCond_t* bcond = _cl + _n_cl;
    
    if(_n_cl >= GetCapacity()) {
      throw std::length_error("Maximum number of boundary conditions reached");
    }

    bcond->Set(region,unknown,fieldindex,functionindex);
    _n_cl++;
  }

  inline void BConds_t::EmplaceBack(char const* region,char const* equation,char const* unknown,size_t const& fieldindex,size_t const& functionindex){
    BCond_t* bcond = _cl + _n_cl;
    
    if(_n_cl >= GetCapacity()) {
      throw std::length_error("Maximum number of boundary conditions reached");
    }

    bcond->Set(region,equation,unknown,fieldindex,functionindex);
    _n_cl++;
  }

#endif
