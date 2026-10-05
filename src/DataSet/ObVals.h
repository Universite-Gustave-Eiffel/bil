#ifndef OBVALS_H
#define OBVALS_H


/* Forward declarations */
struct ObVals_t; //typedef struct ObVals_t       ObVals_t ;
struct DataFile_t;
struct Mesh_t;
struct Materials_t;
struct ObVal_t;


extern ObVals_t*  (ObVals_New)(void) ;
extern ObVals_t*  (ObVals_Create)(DataFile_t*,Mesh_t*,Materials_t*) ;
extern void       (ObVals_Scan)(ObVals_t*,DataFile_t*);
extern void       (ObVals_Delete)(void*) ;
extern int        (ObVals_FindObValIndex)(ObVals_t*,char*) ;


#define ObVals_MaxNbOfObVals             (100)


#define ObVals_GetNbOfObVals(OVS)    ((OVS)->GetNbOfObVals())
#define ObVals_GetObVal(OVS)         ((OVS)->GetObVal())


#define ObVals_SetNbOfObVals(OVS,A)    ((OVS)->SetNbOfObVals(A))
#define ObVals_SetObVal(OVS,A)         ((OVS)->SetObVal(A))

#define ObVals_EmplaceBack(OVS,...)    ((OVS)->EmplaceBack(__VA_ARGS__))


#include <string>
#include <stdexcept>
#include <optional>

struct ObVals_t {
  private:
  size_t _n_obj ;
  ObVal_t* _obj ;

  public:
  size_t GetCapacity(){return ObVals_MaxNbOfObVals;}
  void EmplaceBack(std::string const& name,double const& v,std::optional<std::string> const& type = std::nullopt,double const& r = 0){
    if(type) {
      EmplaceBack(name.c_str(),v,type->c_str(),r);
    } else {
      EmplaceBack(name.c_str(),v);
    }
  }
  void EmplaceBack(char const*,double const&,char const* = nullptr,double const& = 0);
  size_t GetNbOfObVals(){return _n_obj ;}
  ObVal_t* GetObVal(){return _obj ;}

  void SetNbOfObVals(size_t a){
    if(a >= GetCapacity()) {
      throw std::length_error("Maximum number of obvals reached");
    }
    _n_obj = a;
  }
  void SetObVal(ObVal_t* a){_obj = a;}
} ;


#include "ObVal.h"

  inline void ObVals_t::EmplaceBack(char const* name,double const& v,char const* type,double const& r){
    ObVal_t* obval = _obj + _n_obj;
    
    if(_n_obj >= GetCapacity()) {
      throw std::length_error("Maximum number of objective values reached");
    }

    ObVal_Set(obval,name,v,type,r);
    _n_obj++;
  }


#endif
