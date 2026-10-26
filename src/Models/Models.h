/*
** File:
** $Id: Models.h$
**
**
** Purpose:
** Unit specification for models.
**
*/
#ifndef MODELS_H
#define MODELS_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Models_t;
struct Model_t;
struct Geometry_t;
struct DataFile_t;


#include <stdio.h>

#define Models_MaxNbOfModels             (100)

extern Models_t* (Models_New)(Geometry_t*,DataFile_t* = nullptr) ;
extern Models_t* (Models_Create)(Geometry_t*,DataFile_t*) ;
extern void      (Models_Scan)(Models_t*,DataFile_t*);
extern void      (Models_Delete)(void*) ;
extern void      (Models_PrintAll)(char*,FILE *) ;


#define Models_GetMaxNbOfModels(MODS) ((MODS)->GetMaxNbOfModels())
#define Models_GetNbOfModels(MODS)    ((MODS)->GetNbOfModels())
#define Models_GetModel(MODS)         ((MODS)->GetModel())
#define Models_GetGeometry(MODS)      ((MODS)->GetGeometry())
#define Models_GetDataFile(MODS)      ((MODS)->GetDataFile())
#define Models_GetVectorOfPointersToModels(MODS) ((MODS)->GetVectorOfPointersToModels())
#define Models_GetModelAt(MODS,I)        ((MODS)->GetModelAt(I))

#define Models_SetMaxNbOfModels(MODS,A) ((MODS)->SetMaxNbOfModels(A))
#define Models_SetNbOfModels(MODS,A)    ((MODS)->SetNbOfModels(A))
#define Models_SetModel(MODS,A)         ((MODS)->SetModel(A))
#define Models_SetGeometry(MODS,A)      ((MODS)->SetGeometry(A))
#define Models_SetDataFile(MODS,A)      ((MODS)->SetDataFile(A))

#define Models_FindModel(MODS,A)         ((MODS)->FindModel(A))
#define Models_FindModelIndex(MODS,A)    ((MODS)->FindModelIndex(A))
#define Models_FindOrAppendModel(MODS,A) ((MODS)->FindOrAppendModel(A))

#define Models_EmplaceBack(MODS,...)     ((MODS)->EmplaceBack(__VA_ARGS__))
#define Models_Print(MODS)               ((MODS)->Print())



#include <string>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <vector>

struct Models_t {
  private:
  size_t _maxn_model ;
  size_t _n_model ;
  /* Non-contiguous, non-owing container */
  std::vector<Model_t*> _pmodel_vec;
  /* Contiguous, owing container */
  Model_t* _model ;
  Geometry_t* _geometry  ;
  DataFile_t* _datafile ;

  public:
  size_t GetCapacity(){return _pmodel_vec.capacity();}
  Model_t* EmplaceBack();
  template<typename... Args>
  Model_t* EmplaceBack(char const*,Args&&...);
  Model_t* EmplaceBack(Model_t&);

  size_t GetMaxNbOfModels(){return _pmodel_vec.capacity();}
  size_t GetNbOfModels(){return _pmodel_vec.size();}
  std::vector<Model_t*>& GetVectorOfPointersToModels(){return _pmodel_vec;}
  Model_t* GetModel(){return _model;}
  Model_t* GetModelAt(size_t&);
  Geometry_t* GetGeometry(){return _geometry;}
  DataFile_t* GetDataFile(){return _datafile;}

  void SetMaxNbOfModels(size_t const& a){_maxn_model = a;}
  void SetNbOfModels(size_t const& a){_n_model = a;}
  void SetModel(Model_t* a){_model = a;}
  void SetGeometry(Geometry_t* a){_geometry = a;}
  void SetDataFile(DataFile_t* a){_datafile = a;}

  size_t FindModelIndex(const char*);
  Model_t* FindModel(const char*);
  Model_t* FindOrAppendModel(const char*);
  void Print();
} ;


#include "Model.h"

  inline Model_t* Models_t::GetModelAt(size_t& i){
    //return GetModel() + i;
    return _pmodel_vec.at(i);
  }

  inline size_t Models_t::FindModelIndex(const char* codename){
    size_t n_models = GetNbOfModels() ;
    size_t j = 0 ;
  
    while(j < n_models && strcmp(Model_GetCodeNameOfModel(GetModelAt(j)),codename)) j++ ;
  
    if(j < n_models) {
      return(j) ;
    }

    return(GetCapacity()+1) ;
  }

  inline Model_t* Models_t::FindModel(const char* codename){
    size_t j = FindModelIndex(codename) ;
  
    if(j < GetNbOfModels()) {
      return(GetModelAt(j));
    }

    return(NULL) ;
  }

  inline Model_t* Models_t::FindOrAppendModel(const char* codename){
    Model_t* model = FindModel(codename) ;
      
    if(!model) {
      model = EmplaceBack(codename) ;
    }

    return(model) ;
  }

  inline Model_t* Models_t::EmplaceBack() {
    Model_t* model = _model + GetNbOfModels();

     _pmodel_vec.push_back(model);
    
    Model_SetParentModels(model,this);

    return(model);
  }

  template<typename... Args>
  inline Model_t* Models_t::EmplaceBack(char const* name,Args&&... args) {
    Model_t* model = _model + GetNbOfModels();

     _pmodel_vec.push_back(model);

    Model_SetParentModels(model,this);
    Model_Set(model,name,std::forward<Args>(args)...);

    return(model);
  }

  inline Model_t* Models_t::EmplaceBack(Model_t& m) {
    Model_t* model;

    _pmodel_vec.push_back(&m);
    model = _pmodel_vec.back();

    Model_SetParentModels(model,this);
    Model_SetModelProperties(model);

    return(model);
  }

  inline void Models_t::Print(){
    #define PRINT(...) fprintf(stdout,__VA_ARGS__)
    size_t n_models = GetNbOfModels() ;

    PRINT("Models:\n") ;
    PRINT("\t Nb of models = %lu\n",n_models) ;
    PRINT("\n") ;
    
    for(size_t i = 0 ; i < n_models ; i++) {
      Model_Print(GetModelAt(i));
      PRINT("\n") ;
    }
    #undef PRINT
  }

#ifdef __CPLUSPLUS
}
#endif
#endif
