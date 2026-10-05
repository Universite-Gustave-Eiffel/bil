/*
** File:
** $Id: Model.h$
**
**
** Purpose:
** Unit specification for model.
**
*/
#ifndef MODEL_H
#define MODEL_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Vacuous declarations and typedef names */

/* Forward declarations */
struct Model_t;
struct Models_t;
struct Element_t;
struct IntFcts_t;
struct ShapeFcts_t;
struct Load_t;
struct Result_t;
struct Results_t;
struct Material_t;
struct DataFile_t;
struct ObVal_t;
struct Views_t;

/*  Typedef names of Methods */
#include <stdio.h>

using Model_SetModelProperties_t = int (Model_t*);
using Model_ComputePropertyIndex_t = int (const char*);
using Model_PrintModelProperties_t = int (Model_t*,FILE*);
using Model_ComputeInitialState_t = int (Element_t*,double);
using Model_ComputeExplicitTerms_t = int (Element_t*,double);
using Model_ComputeImplicitTerms_t = int (Element_t*,double,double);
using Model_ComputeMatrix_t = int (Element_t*,double,double,double*);
using Model_ComputeResidu_t = int (Element_t*,double,double,double*);
using Model_DefineElementProperties_t = int (Element_t*,IntFcts_t*,ShapeFcts_t*);
using Model_ComputeLoads_t = int (Element_t*,double,double,Load_t*,double*);
using Model_ReadMaterialProperties_t = int (Material_t*,DataFile_t*);
using Model_ComputeMaterialProperties_t = void (Element_t*,double);
using Model_ComputeOutputs_t = int (Element_t*,double,double*,Result_t*);


#if 0
using Model_ComputeOutputs_t1 = int (Element_t*,double,double*,Result_t*);
using Model_ComputeOutputs_t2 = int (Element_t*,double,double*,Results_t*);
#include <variant>
using Model_ComputeOutputs_t = std::variant<Model_ComputeOutputs_t1,Model_ComputeOutputs_t2>;
#define Model_GetComputeOutputs(MOD) \
        std::get_if<((MOD)->computeoutputs)->index()>((MOD)->computeoutputs)
#endif



#define Model_MaxLengthOfKeyWord        (30)
#define Model_MaxNbOfEquations          (10)
#define Model_MaxLengthOfShortTitle     (80)
#define Model_MaxLengthOfAuthorNames    (80)
#define Model_MaxNbOfViews              (Views_MaxNbOfViews)

#define Model_MaxNbOfVariables          (100)
#define Model_MaxNbOfVariableFluxes     (Model_MaxNbOfVariables)

#include "ListOfModels.h"

#define Model_NbOfListedModels          (ListOfModels_Nb)
#define Model_ListOfNames               ListOfModels_Names
#define Model_ListOfSetModelProp        ListOfModels_Methods(_SetModelProp)


extern Model_t*  (Model_New)       (Models_t*) ;
extern void      (Model_Delete)    (void*) ;
//extern Model_t*  (Model_Initialize)(Model_t*,const char*) ;
extern void      (Model_Scan)(Model_t*,DataFile_t*) ;
extern  Model_SetModelProperties_t  Model_ListOfSetModelProp ;


/* The getters */
#define Model_GetCodeNameOfModel(MOD)      ((MOD)->GetCodeNameOfModel())
#define Model_GetNbOfEquations(MOD)        ((MOD)->GetNbOfEquations())
#define Model_GetNameOfEquation(MOD)       ((MOD)->GetNameOfEquation())
#define Model_GetNameOfUnknown(MOD)        ((MOD)->GetNameOfUnknown())
#define Model_GetSequentialIndexOfUnknown(MOD) ((MOD)->GetSequentialIndexOfUnknown())
#define Model_GetParentModels(MOD)         ((MOD)->GetParentModels())
#define Model_GetShortTitle(MOD)           ((MOD)->GetShortTitle())
#define Model_GetNameOfAuthors(MOD)        ((MOD)->GetNameOfAuthors())
#define Model_GetNumericalMethod(MOD)      ((MOD)->GetNumericalMethod())
#define Model_GetObjectiveValue(MOD)       ((MOD)->GetObjectiveValue())
#define Model_GetViews(MOD)                ((MOD)->GetViews())

#define Model_GetSetModelProperties(MOD)      ((MOD)->GetSetModelProperties())
#define Model_GetReadMaterialProperties(MOD)  ((MOD)->GetReadMaterialProperties())
#define Model_GetPrintModelProperties(MOD)    ((MOD)->GetPrintModelProperties())
#define Model_GetDefineElementProperties(MOD) ((MOD)->GetDefineElementProperties())
#define Model_GetComputeInitialState(MOD)     ((MOD)->GetComputeInitialState())
#define Model_GetComputeExplicitTerms(MOD)    ((MOD)->GetComputeExplicitTerms())
#define Model_GetComputeImplicitTerms(MOD)    ((MOD)->GetComputeImplicitTerms())
#define Model_GetComputeMatrix(MOD)           ((MOD)->GetComputeMatrix())
#define Model_GetComputeResidu(MOD)           ((MOD)->GetComputeResidu())
#define Model_GetComputeLoads(MOD)            ((MOD)->GetComputeLoads())
#define Model_GetComputeOutputs(MOD)          ((MOD)->GetComputeOutputs())
#define Model_GetComputePropertyIndex(MOD)    ((MOD)->GetComputePropertyIndex())
#define Model_GetComputeMaterialProperties(MOD) ((MOD)->GetComputeMaterialProperties())


/* The setters */
#define Model_SetCodeNameOfModel(MOD,A)      ((MOD)->SetCodeNameOfModel(A))
#define Model_SetNbOfEquations(MOD,A)        ((MOD)->SetNbOfEquations(A))
#define Model_SetNameOfEquation(MOD,A)       ((MOD)->SetNameOfEquation(A))
#define Model_SetNameOfUnknown(MOD,A)        ((MOD)->SetNameOfUnknown(A))
#define Model_SetSequentialIndexOfUnknown(MOD,A) ((MOD)->SetSequentialIndexOfUnknown(A))
#define Model_SetParentModels(MOD,A)         ((MOD)->SetParentModels(A))
#define Model_SetShortTitle(MOD,A)           ((MOD)->SetShortTitle(A))
#define Model_SetNameOfAuthors(MOD,A)        ((MOD)->SetNameOfAuthors(A))
#define Model_SetNumericalMethod(MOD,A)      ((MOD)->SetNumericalMethod(A))
#define Model_SetObjectiveValue(MOD,A)       ((MOD)->SetObjectiveValue(A))
#define Model_SetViews(MOD,A)                ((MOD)->SetViews(A))

#define Model_SetSetModelProperties(MOD,A)      ((MOD)->SetSetModelProperties(A))
#define Model_SetReadMaterialProperties(MOD,A)  ((MOD)->SetReadMaterialProperties(A))
#define Model_SetPrintModelProperties(MOD,A)    ((MOD)->SetPrintModelProperties(A))
#define Model_SetDefineElementProperties(MOD,A) ((MOD)->SetDefineElementProperties(A))
#define Model_SetComputeInitialState(MOD,A)     ((MOD)->SetComputeInitialState(A))
#define Model_SetComputeExplicitTerms(MOD,A)    ((MOD)->SetComputeExplicitTerms(A))
#define Model_SetComputeImplicitTerms(MOD,A)    ((MOD)->SetComputeImplicitTerms(A))
#define Model_SetComputeMatrix(MOD,A)           ((MOD)->SetComputeMatrix(A))
#define Model_SetComputeResidu(MOD,A)           ((MOD)->SetComputeResidu(A))
#define Model_SetComputeLoads(MOD,A)            ((MOD)->SetComputeLoads(A))
#define Model_SetComputeOutputs(MOD,A)          ((MOD)->SetComputeOutputs(A))
#define Model_SetComputePropertyIndex(MOD,A)    ((MOD)->SetComputePropertyIndex(A))
#define Model_SetComputeMaterialProperties(MOD,A) ((MOD)->SetComputeMaterialProperties(A))

#define Model_Set(MOD,...)                      ((MOD)->Set(__VA_ARGS__))
#define Model_Initialize(MOD,...)               ((MOD)->Initialize(__VA_ARGS__))
#define Model_Print(MOD)                        ((MOD)->Print())


/* Other getters */
#define Model_GetGeometry(MOD) \
        Models_GetGeometry(Model_GetParentModels(MOD))

#define Model_GetDataFile(MOD) \
        Models_GetDataFile(Model_GetParentModels(MOD))


/* Copy operations */
#define Model_CopyNameOfEquation(MOD,index,name) \
        (strcpy(Model_GetNameOfEquation(MOD)[index],name))

#define Model_CopyNameOfUnknown(MOD,index,name) \
        (strcpy(Model_GetNameOfUnknown(MOD)[index],name))

#define Model_CopyCodeNameOfModel(MOD,codename) \
        (strcpy(Model_GetCodeNameOfModel(MOD),codename))

#define Model_CopyShortTitle(MOD,title) \
        (strcpy(Model_GetShortTitle(MOD),title))

#define Model_CopyNameOfAuthors(MOD,authors) \
        (strcpy(Model_GetNameOfAuthors(MOD),authors))

#define Model_SetDefaultNameOfUnknown(MOD,index,name) \
        do {\
          if(Model_GetNameOfUnknown(MOD)[index][0] == '\0') {\
            Model_CopyNameOfUnknown(MOD,index,name) ;\
          }\
        } while(0)


/* Index of equation*/
#define Model_IndexOfEquation(MOD,name) (MOD)->IndexOfEquation(name)


/* Dimension */
#define Model_GetDimension(MOD) \
        Geometry_GetDimension(Model_GetGeometry(MOD))


/* Short hands */
#define Model_SetModelProperties(MOD) \
        do {\
          if(Model_GetSetModelProperties(MOD)) {\
            Model_GetSetModelProperties(MOD)(MOD);\
          }\
        } while(0)

#define Model_PrintModelProp(MOD,...) \
        do {\
          if(Model_GetPrintModelProperties(MOD)) {\
            Model_GetPrintModelProperties(MOD)(MOD,__VA_ARGS__);\
          }\
        } while(0)

#define Model_ComputeMaterialProperties(MOD,...) \
        do {\
          if(Model_GetComputeMaterialProperties(MOD)) {\
            Model_GetComputeMaterialProperties(MOD)(__VA_ARGS__);\
          }\
        } while(0)


#include <string.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>

struct Model_t {
  Model_SetModelProperties_t*       _setmodelprop ;
  Model_ReadMaterialProperties_t*   _readmatprop ;
  Model_PrintModelProperties_t*     _printmodelprop ;
  Model_DefineElementProperties_t*  _defineelementprop ;
  Model_ComputeInitialState_t*      _computeinitialstate ;
  Model_ComputeExplicitTerms_t*     _computeexplicitterms ;
  Model_ComputeImplicitTerms_t*     _computeimplicitterms ;
  Model_ComputeMatrix_t*            _computematrix ;
  Model_ComputeResidu_t*            _computeresidu ;
  Model_ComputeLoads_t*             _computeloads ;
  Model_ComputeOutputs_t*           _computeoutputs ;
  Model_ComputePropertyIndex_t*     _computepropertyindex ;
  Model_ComputeMaterialProperties_t* _ComputeMaterialProperties ;
    
  char*   _codename ;          /* code name of the model */
  char*   _shorttitle ;        /* Short title of the model */
  char*   _authors ;           /* Authors of the model */
  size_t _nbofequations ;    /* Number of equations */
  char**   _nameofequations ;  /* Names of equations */
  char**   _nameofunknowns ;   /* Names of unknowns */
  int*     _sequentialindex ;  /* Sequential indexes of unknowns/equations */
  Models_t*  _parentmodels ;   /* Models which this model belongs to */
  void*    _numericalmethod ;  /* Numerical method */
  ObVal_t* _obval ;            /* Objective values of unknowns */
  Views_t* _views ;            /* Views */

  /* The getters */
  Model_SetModelProperties_t*       GetSetModelProperties(){return _setmodelprop ;}
  Model_ReadMaterialProperties_t*   GetReadMaterialProperties(){return _readmatprop ;}
  Model_PrintModelProperties_t*     GetPrintModelProperties(){return _printmodelprop ;}
  Model_DefineElementProperties_t*  GetDefineElementProperties(){return _defineelementprop ;}
  Model_ComputeInitialState_t*      GetComputeInitialState(){return _computeinitialstate ;}
  Model_ComputeExplicitTerms_t*     GetComputeExplicitTerms(){return _computeexplicitterms ;}
  Model_ComputeImplicitTerms_t*     GetComputeImplicitTerms(){return _computeimplicitterms ;}
  Model_ComputeMatrix_t*            GetComputeMatrix(){return _computematrix ;}
  Model_ComputeResidu_t*            GetComputeResidu(){return _computeresidu ;}
  Model_ComputeLoads_t*             GetComputeLoads(){return _computeloads ;}
  Model_ComputeOutputs_t*           GetComputeOutputs(){return _computeoutputs ;}
  Model_ComputePropertyIndex_t*     GetComputePropertyIndex(){return _computepropertyindex ;}
  Model_ComputeMaterialProperties_t* GetComputeMaterialProperties(){return _ComputeMaterialProperties ;}
  char*   GetCodeNameOfModel(){return _codename ;}
  char*   GetShortTitle(){return _shorttitle ;}
  char*   GetNameOfAuthors(){return _authors ;}
  size_t  GetNbOfEquations(){return _nbofequations ;}
  char**  GetNameOfEquation(){return _nameofequations ;}
  char**  GetNameOfUnknown(){return _nameofunknowns ;}
  int*    GetSequentialIndexOfUnknown(){return _sequentialindex ;}
  Models_t*  GetParentModels(){return _parentmodels ;}
  void*    GetNumericalMethod(){return _numericalmethod ;}
  ObVal_t* GetObjectiveValue(){return _obval ;}
  Views_t* GetViews(){return _views ;}


  /* The setters */
  void SetSetModelProperties(Model_SetModelProperties_t* a){_setmodelprop = a;}
  void SetReadMaterialProperties(Model_ReadMaterialProperties_t* a){_readmatprop = a;}
  void SetPrintModelProperties(Model_PrintModelProperties_t* a){_printmodelprop = a;}
  void SetDefineElementProperties(Model_DefineElementProperties_t* a){_defineelementprop = a;}
  void SetComputeInitialState(Model_ComputeInitialState_t* a){_computeinitialstate = a;}
  void SetComputeExplicitTerms(Model_ComputeExplicitTerms_t* a){_computeexplicitterms = a;}
  void SetComputeImplicitTerms(Model_ComputeImplicitTerms_t* a){_computeimplicitterms = a;}
  void SetComputeMatrix(Model_ComputeMatrix_t* a){_computematrix = a;}
  void SetComputeResidu(Model_ComputeResidu_t* a){_computeresidu = a;}
  void SetComputeLoads(Model_ComputeLoads_t* a){_computeloads = a;}
  void SetComputeOutputs(Model_ComputeOutputs_t* a){_computeoutputs = a;}
  void SetComputePropertyIndex(Model_ComputePropertyIndex_t* a){_computepropertyindex = a;}
  void SetComputeMaterialProperties(Model_ComputeMaterialProperties_t* a){_ComputeMaterialProperties = a;}
  void SetCodeNameOfModel(char* a){_codename = a;}
  void SetShortTitle(char* a){_shorttitle = a;}
  void SetNameOfAuthors(char* a){_authors = a;}
  void SetNbOfEquations(size_t a){_nbofequations = a;}
  void SetNameOfEquation(char** a){_nameofequations = a;}
  void SetNameOfUnknown(char** a){_nameofunknowns = a;}
  void SetSequentialIndexOfUnknown(int* a){_sequentialindex = a;}
  void SetParentModels(Models_t* a){_parentmodels = a;}
  void SetNumericalMethod(void* a){_numericalmethod = a;}
  void SetObjectiveValue(ObVal_t* a){_obval = a;}
  void SetViews(Views_t* a){_views = a;}

  template<typename... Args>
  void Set(Args... args) {
    throw std::runtime_error("Model_t::Set: Not implemented");
  }
  void Set(char const* codename){
    Initialize(codename) ;
  }
  void Set(std::string const& codename){
    Initialize(codename.c_str()) ;
  }
  void Set(char const* codename,size_t n,char const* const* name_equ,char const* const* name_unk){
    std::string name_str{codename};
    std::vector<std::string> name_equ_vec(name_equ,name_equ+n);
    std::vector<std::string> name_unk_vec(name_unk,name_unk+n);

    Set(name_str,name_equ_vec,name_unk_vec) ;
  }
  void Set(std::string const& codename,std::string const& name_equ,std::string const& name_unk){
    auto split = [](const std::string& s) {
      std::vector<std::string> v;
      std::stringstream ss(s);
      std::string token;
      while(std::getline(ss,token,',')) v.push_back(token);
      return v;
    };

    Set(codename,split(name_equ),split(name_unk)) ;
  }
  void Set(std::string const& codename,std::vector<std::string> const& name_equ,std::vector<std::string> const& name_unk){
    /* Code name of the model */
    Initialize(codename.c_str()) ;
      
    {
      size_t neq = GetNbOfEquations() ;
      size_t n = std::min(name_equ.size(),name_unk.size()) ;
      
      for(size_t i = 0 ; i < n ; i++) {
        size_t j = IndexOfEquation(name_equ[i].c_str()) ;

        if(j < neq) {
          strcpy(_nameofunknowns[j],name_unk[i].c_str());
        } else {
          break;
        }
      }
    }

    /* To account for the new unknown and equation names */ 
    if(GetSetModelProperties()) {
      GetSetModelProperties()(this);
    } else {
      throw std::runtime_error("Model_t::Set: No SetModelProperties method defined");
    }
  }

  void Initialize(char const* codename){
    size_t n_models = Model_NbOfListedModels ;
    const char* modelnames[] = {Model_ListOfNames} ;
    Model_SetModelProperties_t* xModel_SetModelProperties[] = {Model_ListOfSetModelProp} ;
    size_t i = 0 ;
  
    while(i < n_models && strcmp(modelnames[i],codename)) i++ ;
    
    if(i < n_models) {
      strcpy(GetCodeNameOfModel(),modelnames[i]);
      SetSetModelProperties(xModel_SetModelProperties[i]) ;
      /* Call to SetModelProperties */
      if(GetSetModelProperties()) {
        GetSetModelProperties()(this);
      } else {
        throw std::runtime_error("Model_t::Initialize: No SetModelProperties method defined");
      }
    } else {
      printf("No model named %s",codename) ;
    }
  }

  /* Methods */
  size_t IndexOfEquation(char const* name) {
    for(size_t i = 0 ; i < _nbofequations ; i++) {
      if(strcmp(_nameofequations[i],name) == 0) return(i);
    }
    return(Model_MaxNbOfEquations+1);
  }

  void Print(void) {
    #define PRINT(...) fprintf(stdout,__VA_ARGS__)
    int c2 = 40 ;
    size_t nb_eqn = GetNbOfEquations() ;
    char* codename = GetCodeNameOfModel() ;
    char** name_eqn = GetNameOfEquation() ;
    char** name_unk = GetNameOfUnknown() ;

    PRINT("\t Model = %s\n",codename) ;
    PRINT("\t Equations:\n") ;
    PRINT("\t Nb of equations = %lu\n",nb_eqn) ;
      
    for(size_t j = 0 ; j < nb_eqn ; j++) {
      int n = PRINT("\t equation(%lu): (%s)",j + 1,name_eqn[j]) ;
      
      while(n < c2) n += PRINT(" ") ;
        
      n += PRINT("unknown(%lu): (%s)",j + 1,name_unk[j]) ;
        
      PRINT("\n") ;
    }
    #undef PRINT
  }
} ;


/* Old notations which should be eliminated */
#define MAX_EQUATIONS     Model_MaxNbOfEquations


#ifdef __CPLUSPLUS
}
#endif

/* Need for the macros */
#include "Views.h"
#include "Geometry.h"
#include "Models.h"
#endif
