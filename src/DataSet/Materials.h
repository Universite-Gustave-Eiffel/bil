#ifndef MATERIALS_H
#define MATERIALS_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Materials_t; //typedef struct Materials_t    Materials_t ;
struct DataFile_t;
struct Fields_t;
struct Functions_t;
struct Models_t;
struct Material_t;
struct ObVals_t;


extern Materials_t* (Materials_New)   (Models_t*,Fields_t*,Functions_t*);
extern Materials_t* (Materials_Create)(DataFile_t*,Fields_t*,Functions_t*,Models_t*);
extern void         (Materials_Scan)(Materials_t*,DataFile_t*);
extern void         (Materials_Delete)(void*);
extern void         (Materials_LinkUpToObVals)(Materials_t*,ObVals_t*);

#define Materials_MaxNbOfMaterials  (100)

/* The getters */
#define Materials_GetNbOfMaterials(MATS)  ((MATS)->GetNbOfMaterials())
#define Materials_GetMaterial(MATS)       ((MATS)->GetMaterial())
#define Materials_GetUsedModels(MATS)     ((MATS)->GetUsedModels())
#define Materials_GetCapacity(MATS)       ((MATS)->GetCapacity())
#define Materials_GetFields(MATS)         ((MATS)->GetFields())
#define Materials_GetFunctions(MATS)      ((MATS)->GetFunctions())

/* The setters */
#define Materials_SetNbOfMaterials(MATS,A) ((MATS)->SetNbOfMaterials(A))
#define Materials_SetMaterial(MATS,A)      ((MATS)->SetMaterial(A))
#define Materials_SetUsedModels(MATS,A)    ((MATS)->SetUsedModels(A))
#define Materials_SetFields(MATS,A)        ((MATS)->SetFields(A))
#define Materials_SetFunctions(MATS,A)     ((MATS)->SetFunctions(A))

#define Materials_EmplaceBack(MATS,...)    ((MATS)->EmplaceBack(__VA_ARGS__))
#define Materials_Print(MATS)              ((MATS)->Print())


#define Materials_GetNbOfUsedModels(MATS) \
        Models_GetNbOfModels(Materials_GetUsedModels(MATS))
        
#define Materials_GetUsedModel(MATS) \
        Models_GetModel(Materials_GetUsedModels(MATS))

#include <string>

struct Materials_t {
  private:
  size_t _n_mat ;        /**< Nb of materials */
  Material_t* _mat ;           /**< Material */
  Models_t* _models ;          /**< Used models */
  Fields_t* _fields ;          /**< Fields */
  Functions_t* _functions ;    /**< Time functions */

  public:
  size_t GetCapacity() {return Materials_MaxNbOfMaterials;}
  Material_t* EmplaceBack(std::string const& name){
    return(EmplaceBack(name.c_str()));
  }
  Material_t* EmplaceBack(char const*);

  public:
  size_t GetNbOfMaterials() const {return _n_mat;}
  Material_t* GetMaterial() {return _mat;}
  Models_t* GetUsedModels() {return _models;}
  Fields_t* GetFields() {return _fields;}
  Functions_t* GetFunctions() {return _functions;}

  void SetNbOfMaterials(size_t n) {
    if(_n_mat >= Materials_MaxNbOfMaterials) {
      throw std::length_error("Maximum number of materials reached");
    }
    _n_mat = n;
  }
  void SetMaterial(Material_t* a) {_mat = a;}
  void SetUsedModels(Models_t* a) {_models = a;}
  void SetFields(Fields_t* a) {_fields = a;}
  void SetFunctions(Functions_t* a) {_functions = a;}
  void Print();
} ;


#include "Material.h"
#include "Models.h"

  inline Material_t* Materials_t::EmplaceBack(char const* modelname){
    Material_t* material = _mat + _n_mat;
    Models_t* usedmodels = GetUsedModels() ;
    Model_t* model = Models_FindOrAppendModel(usedmodels,modelname) ;
    size_t modind  = Models_FindModelIndex(usedmodels,modelname) ;
    
    if(_n_mat >= GetCapacity()) {
      throw std::length_error("Maximum number of materials reached");
    }

    Material_Set(material,model,modind) ;

    #if 0
    /* Input material data */
    /* A model pointing to a null pointer serves to build curves only */
    {
      DataFile_t* datafile = Models_GetDataFile(usedmodels) ;

      if(model) {
        Model_ReadMaterialProperties_t* readmatprop = Model_GetReadMaterialProperties(model) ;
    
        if(readmatprop) {
          int n = readmatprop(this,datafile) ;
    
          if(n > Material_MaxNbOfProperties) {
            Message_RuntimeError("Material_t::Set: too many properties") ;
          }
    
          SetNbOfProperties(n) ;
        }
      } else {
        Material_Set();
      }
    }
    
    /* for compatibility with old version */
    if(GetModel()) {
      if(Material_GetNbOfEquations(this) == 0) {
        Material_SetNbOfEquations(this,neq) ;
      }
    }
    
    nc = Material_GetNbOfCurves(this) ;
    
    if(!GetModel()) {
      throw std::runtime_error("Material_t::Set: Model not known") ;
    }
    #endif

    _n_mat++;
    return material;
  }

  inline void Materials_t::Print(){
    #define PRINT(...) fprintf(stdout,__VA_ARGS__)
    {
      size_t n_mats = GetNbOfMaterials() ;
      Material_t* mat = GetMaterial() ;

      PRINT("Materials:\n") ;
      PRINT("\t Nb of materials = %lu\n",n_mats) ;
      PRINT("\n") ;
    
      for(size_t i = 0 ; i < n_mats ; i++) {
        PRINT("Material(%lu):\n",i) ;
        Material_Print(mat+i);
      }
    }
    PRINT("\n") ;
    {
      Models_t* usedmodels = GetUsedModels() ;
      size_t n_usedmodels = Models_GetNbOfModels(usedmodels) ;
      Model_t* usedmodel = Models_GetModel(usedmodels) ;
      
      PRINT("Nb of used models = %lu\n",n_usedmodels) ;
    
      for(size_t i = 0 ; i < n_usedmodels ; i++) {
        PRINT("\t Used model(%lu): %s\n",i,Model_GetCodeNameOfModel(usedmodel+i)) ;
      }
    }
    #undef PRINT
  }


#ifdef __CPLUSPLUS
}
#endif

#include "Models.h"
#endif
