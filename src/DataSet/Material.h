#ifndef MATERIAL_H
#define MATERIAL_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Material_t;
struct Materials_t;
struct DataFile_t;
struct GenericData_t;
struct Curves_t;
struct Curve_t;
struct Fields_t;
struct Functions_t;
struct Models_t;
struct Model_t;

using Model_ComputePropertyIndex_t = int (const char*);


extern Material_t* (Material_New)             (Materials_t*) ;
extern void        (Material_Delete)          (void*) ;
extern void        (Material_Scan)            (Material_t*,DataFile_t*) ;
extern int         (Material_ReadProperties)  (Material_t*,DataFile_t*) ;
extern void        (Material_ScanProperties)  (Material_t*,DataFile_t*,Model_ComputePropertyIndex_t*) ;
//extern void        (Material_ScanProperties1) (Material_t*,FILE*,Model_ComputePropertyIndex_t*,int) ;
//extern void        (Material_ScanProperties2) (Material_t*,FILE*,Model_ComputePropertyIndex_t*,int,int) ;



#define Material_MaxLengthOfKeyWord            (50)
#define Material_MaxLengthOfTextLine           (500)

#define Material_MaxNbOfCurves                 (50)     /* Max nb of curves per mat */
#define Material_MaxNbOfProperties             (200)    /* Max nb of scalar inputs */


/* The getters */
#define Material_GetNbOfProperties(MAT)   ((MAT)->GetNbOfProperties())
#define Material_GetProperty(MAT)         ((MAT)->GetProperty())
#define Material_GetCurves(MAT)           ((MAT)->GetCurves())
#define Material_GetModel(MAT)            ((MAT)->GetModel())
#define Material_GetMethod(MAT)           ((MAT)->GetMethod())
#define Material_GetCodeNameOfModel(MAT)  ((MAT)->GetCodeNameOfModel())
#define Material_GetGenericData(MAT)      ((MAT)->GetGenericData())
#define Material_GetModelIndex(MAT)       ((MAT)->GetModelIndex())
#define Material_GetParentMaterials(MAT)  ((MAT)->GetParentMaterials())

/* The setters */
#define Material_SetNbOfProperties(MAT,A)  ((MAT)->SetNbOfProperties(A))
#define Material_SetProperty(MAT,A)        ((MAT)->SetProperty(A))
#define Material_SetCurves(MAT,A)          ((MAT)->SetCurves(A))
#define Material_SetModel(MAT,A)           ((MAT)->SetModel(A))
#define Material_SetMethod(MAT,A)          ((MAT)->SetMethod(A))
#define Material_SetCodeNameOfModel(MAT,A) ((MAT)->SetCodeNameOfModel(A))
#define Material_SetGenericData(MAT,A)     ((MAT)->SetGenericData(A))
#define Material_SetModelIndex(MAT,A)      ((MAT)->SetModelIndex(A))
#define Material_SetParentMaterials(MAT,A) ((MAT)->SetParentMaterials(A))

#define Material_Set(MAT,...)               ((MAT)->Set(__VA_ARGS__))
#define Material_Finalize(MAT)              ((MAT)->Finalize())
#define Material_Print(MAT)                 ((MAT)->Print())




#define Material_GetFields(MAT) \
          Materials_GetFields(Material_GetParentMaterials(MAT))

#define Material_GetFunctions(MAT) \
          Materials_GetFunctions(Material_GetParentMaterials(MAT))

#define Material_GetUsedModels(MAT) \
        Materials_GetUsedModels(Material_GetParentMaterials(MAT))


/* Material properties */
#define Material_GetNbOfCurves(MAT) \
        Curves_GetNbOfCurves(Material_GetCurves(MAT))

#define Material_GetCurve(MAT) \
        Curves_GetCurve(Material_GetCurves(MAT))

#define Material_GetField(MAT) \
        Fields_GetField(Material_GetFields(MAT))

#define Material_GetNbOfFunctions(MAT) \
        Functions_GetNbOfFunctions(Material_GetFunctions(MAT))

#define Material_GetFunction(MAT) \
        Functions_GetFunction(Material_GetFunctions(MAT))

#define Material_GetDimension(MAT) \
        Geometry_GetDimension(Model_GetGeometry(Material_GetModel(MAT)))

#define Material_FindCurve(MAT,S) \
        Curves_FindCurve(Material_GetCurves(MAT),S)
        
#define Material_GetPropertyValue(MAT,S) \
        (Material_GetProperty(MAT) + Model_GetComputePropertyIndex(Material_GetModel(MAT))(S))[0]

#define Material_SetPropertiesToZero(MAT,N) \
        do { \
          int Material_i ; \
          for(Material_i = 0 ; Material_i < N ; Material_i++) { \
            Material_GetProperty(MAT)[Material_i] = 0 ; \
          } \
        } while(0)

/*
** #define Material_ReadProperties(MAT,datafile) \
*          Model_GetReadMaterialProperties(Material_GetModel(MAT))(MAT,datafile)
*/

/* Equations/unknowns */
#define Material_GetNbOfEquations(MAT) \
        Model_GetNbOfEquations(Material_GetModel(MAT))

#define Material_SetNbOfEquations(MAT,A) \
        Model_SetNbOfEquations(Material_GetModel(MAT),A)

#define Material_GetNameOfEquation(MAT) \
        Model_GetNameOfEquation(Material_GetModel(MAT))

#define Material_GetNameOfUnknown(MAT) \
        Model_GetNameOfUnknown(Material_GetModel(MAT))
        
#define Material_CopyNameOfEquation(MAT,index,name) \
        (strcpy(Material_GetNameOfEquation(MAT)[index],name))

#define Material_CopyNameOfUnknown(MAT,index,name) \
        (strcpy(Material_GetNameOfUnknown(MAT)[index],name))

#define Material_GetObjectiveValue(MAT) \
        Model_GetObjectiveValue(Material_GetModel(MAT))

#define Material_GetSequentialIndexOfUnknown(MAT) \
        Model_GetSequentialIndexOfUnknown(Material_GetModel(MAT))
        


/* GenericData */
#define Material_AppendGenericData(MAT,GD) \
        do { \
          if(Material_GetGenericData(MAT)) { \
            GenericData_Append(Material_GetGenericData(MAT),GD) ; \
          } else { \
            Material_SetGenericData(MAT,GD) ; \
          } \
        } while(0)
        
#define Material_FindGenericData(MAT,...) \
        GenericData_Find(Material_GetGenericData(MAT),__VA_ARGS__)
        
#define Material_FindData(MAT,...) \
        GenericData_FindData(Material_GetGenericData(MAT),__VA_ARGS__)
        
#define Material_FindNbOfData(MAT,...) \
        GenericData_FindNbOfData(Material_GetGenericData(MAT),__VA_ARGS__)

#define Material_AppendData(MAT,...) \
        Material_AppendGenericData(MAT,GenericData_Create(__VA_ARGS__))


/* We use a C extension provided by GNU C:
 * A compound statement enclosed in parentheses may appear 
 * as an expression in GNU C.
 * (https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html#Statement-Exprs) */
#define Material_PropertyIndex(PAR,V) \
        CustomValues_Index(PAR,V,double)


#include <string>

struct Material_t {           /* material */
  char*   _codenameofmodel ;   /**< Code name of the model */
  char*   _method ;            /**< Characterize a method */
  int     _n ;                 /**< Nb of properties */
  double* _pr ;                /**< The properties */
  GenericData_t* _genericdata ;
  Curves_t* _curves ;          /**< Curves */
  Model_t* _model ;            /**< Model */
  Materials_t* _parentmaterials ;  /**< Materials which this material belongs to */
  size_t _modelindex ;            /**< Model index */
  
  /* for compatibility with former version (should be eliminated) */
  unsigned short int neq ;    /**< nombre d'equations du modele */
  char**   eqn ;              /**< nom des equations */
  char**   inc ;              /**< nom des inconnues */
  int      nc ;               /**< nb of curves */
  Curve_t* cb ;               /**< curves */

  /* The getters */
  char*   GetCodeNameOfModel(){return _codenameofmodel ;}
  char*   GetMethod(){return _method ;}
  int     GetNbOfProperties(){return _n ;}
  double* GetProperty(){return _pr ;}
  GenericData_t* GetGenericData(){return _genericdata ;}
  Curves_t* GetCurves(){return _curves ;}
  Model_t* GetModel(){return _model ;}
  Materials_t* GetParentMaterials(){return _parentmaterials ;}
  size_t GetModelIndex(){return _modelindex ;}

  /* The setters */
  void SetCodeNameOfModel(char* a){_codenameofmodel = a ;}
  void SetMethod(char* a){_method = a ;}
  void SetNbOfProperties(int a){_n = a ;}
  void SetProperty(double* a){_pr = a ;}
  void SetGenericData(GenericData_t* a){_genericdata = a ;}
  void SetCurves(Curves_t* a){_curves = a ;}
  void SetModel(Model_t* a){_model = a ;}
  void SetParentMaterials(Materials_t* a){_parentmaterials = a ;}
  void SetModelIndex(size_t const& a){_modelindex = a ;}

  void Set(std::string const& name){Set(name.c_str());}
  void Set(char const*);
  void Set(Model_t*,size_t const&);
  void Set(std::string const& name,double const& v){Set(name.c_str(),v);}
  void Set(char const*,double const&);
  void Set(std::string const& name,std::string const& line){Set(name.c_str(),line.c_str());}
  void Set(char const*,char const*);
  void Finalize(void);

  void Print(void);
} ;


#include "Materials.h"
#include "Model.h"
#include "Models.h"
#include "Curves.h"
#include "Curve.h"
#include "String_.h"

  inline void Material_t::Set(char const* modelname){
    /* Find or append a model and point to it */
    Materials_t* materials = GetParentMaterials() ;
    Models_t* usedmodels = Materials_GetUsedModels(materials) ;
    Model_t* matmodel = Models_FindOrAppendModel(usedmodels,modelname) ;
    size_t modind  = Models_FindModelIndex(usedmodels,modelname) ;

    strcpy(GetCodeNameOfModel(),modelname) ;
    SetModel(matmodel) ;
    SetModelIndex(modind) ;


    /* for compatibility with old version */
    if(GetModel()) {
      eqn = Material_GetNameOfEquation(this) ;
      inc = Material_GetNameOfUnknown(this) ;
    }


    /* Input material data */
    /* A model pointing to a null pointer serves to build curves only */
    #if 0
    {
      int n = Material_ReadProperties(mat,datafile) ;
    
      Material_SetNbOfProperties(mat,n) ;
    }
    #endif
    
    
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
  }

  inline void Material_t::Set(Model_t* matmodel,size_t const& modind){
    char* modelname = Model_GetCodeNameOfModel(matmodel) ;

    strcpy(GetCodeNameOfModel(),modelname) ;
    SetModel(matmodel) ;
    SetModelIndex(modind) ;
      
    /* for compatibility with old version */
    if(matmodel) {
      eqn = Material_GetNameOfEquation(this) ;
      inc = Material_GetNameOfUnknown(this) ;
    }

    #if 0
    /* Input material data */
    /* A model pointing to a null pointer serves to build curves only */
    {
      DataFile_t* datafile = Models_GetDataFile(usedmodels) ;
      Model_t* model = GetModel();

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
        Set();
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
  }

  inline void Material_t::Set(char const* mot,double const& value){
    Model_t* model = GetModel() ;
    Model_ComputePropertyIndex_t* pm = Model_GetComputePropertyIndex(model) ;
    int  nd = GetNbOfProperties() ;
    
    if(strlen(mot) > Material_MaxLengthOfKeyWord) {
      throw std::runtime_error("Material_t::Set: too many characters") ;
    }

    /* Reading some curves */
    if(String_Is(mot,"Courbes",6) || String_Is(mot,"Curves",5)) {
      throw std::runtime_error("Material_t::Set: reserved keyword") ;

    /* Reading the method */
    } else if(String_Is(mot,"Method",6)) {
      throw std::runtime_error("Material_t::Set: reserved keyword") ;
      
    /* Reading the material properties and storing through pm */
    } else if(pm) {
      int i = (*pm)(mot) ;
        
      if(i >= 0) {  
        GetProperty()[i] = value ;
        nd = (nd > i + 1) ? nd : i + 1 ;
      } else {
        throw std::runtime_error("Material_t::Set: property is not known") ;
      }
    } else {
      throw std::runtime_error("Material_t::Set: pm is not known") ;
    }

    SetNbOfProperties(nd) ;
  }

  inline void Material_t::Set(char const* mot,char const* line){
    if(strlen(mot) > Material_MaxLengthOfKeyWord) {
      throw std::runtime_error("Material_t::Set: too many characters") ;
    }

    /* Reading some curves */
    if(String_Is(mot,"Courbes",6) || String_Is(mot,"Curves",5)) {
      char cline[Curve_MaxLengthOfTextLine] ;
      Curves_t* curves = GetCurves() ;

      strcpy(cline,mot);
      strcpy(cline + strlen(cline)," = ");
      strcpy(cline + strlen(cline),line) ;
      
      Curves_ReadCurves(curves,cline) ;
      
      if(Curves_GetNbOfCurves(curves) > Material_MaxNbOfCurves) {
        throw std::runtime_error("Material_t::Set: too many curves") ;
      }

    /* Reading the method */
    } else if(String_Is(mot,"Method",6)) {
      char const* p = String_FindChar(line,'=') ;
      
      if(p) {
        char* cline = String_CopyLine(line);
        char* cr = String_FindAndSkipToken(cline,"=") ;
        
        cr = String_SkipBlankChars(cr) ;
        strcpy(GetMethod(),cr) ;
      } else {
        strcpy(GetMethod(),line) ;
      }
      
    } else {
      throw std::runtime_error("Material_t::Set: property is not known") ;
    }
  }
  #if 0
  inline void Material_t::Set(DataFile_t* datafile){
    /* Input material data */
    /* A model pointing to a null pointer serves to build curves only */
    Models_t* usedmodels = GetUsedModels() ;
    DataFile_t* datafile = Models_GetDataFile(usedmodels) ;
    Model_t* model = GetModel();
    ptrdiff_t nth = this - Materials_GetMaterial(GetParentMaterials()) ;
    char* c = DataFile_FindNthToken(datafile,"MATE,Material",",",nth + 1) ;
      
    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;

    if(model) {
      Model_ReadMaterialProperties_t* readmatprop = Model_GetReadMaterialProperties(model) ;
    
      if(readmatprop) {
        int n = readmatprop(this,datafile) ;
    
        if(n > Material_MaxNbOfProperties) {
          throw std::runtime_error("Material_t::Set: too many properties") ;
        }
    
        SetNbOfProperties(n) ;
      }
    } else {
      throw std::runtime_error("Material_t::Set: Model not known") ;
    }
  }
  #endif

  inline void Material_t::Finalize(void){
    Model_t* model = GetModel();

    if(model) {
      Model_ReadMaterialProperties_t* rmp = Model_GetReadMaterialProperties(model) ;
    
      if(rmp) {
        int n = rmp(this,nullptr) ;
    
        if(n > Material_MaxNbOfProperties) {
          throw std::runtime_error("Material_t::Finalize: too many properties") ;
        }
    
        SetNbOfProperties(n) ;
      }
    } else {
      throw std::runtime_error("Material_t::Finalize: Model not known") ;
    }
  }

  inline void Material_t::Print(void){
    #define PRINT(...) fprintf(stdout,__VA_ARGS__)
    int c2 = 40 ;
    int nb_pr = GetNbOfProperties() ;
    size_t nb_eqn = Material_GetNbOfEquations(this) ;
    char** name_eqn = Material_GetNameOfEquation(this) ;
    char** name_unk = Material_GetNameOfUnknown(this) ;
    int nb_cv = Material_GetNbOfCurves(this) ;
    Curve_t* cv = Material_GetCurve(this) ;
      
    PRINT("\t Model = %s\n",GetCodeNameOfModel()) ;
      
    PRINT("\n") ;
      
    PRINT("\t Equations:\n") ;
    PRINT("\t Nb of equations = %lu\n",nb_eqn) ;
      
    for(size_t j = 0 ; j < nb_eqn ; j++) {
      int n = PRINT("\t equation(%lu): (%s)",j + 1,name_eqn[j]) ;
      
      while(n < c2) n += PRINT(" ") ;
        
      n += PRINT("unknown(%lu): (%s)",j + 1,name_unk[j]) ;
        
      PRINT("\n") ;
    }
      
    PRINT("\n") ;
      
    PRINT("\t Properties:\n") ;
    PRINT("\t Nb of properties = %d\n",nb_pr) ;
      
    for(int j = 0 ; j < nb_pr ; j++) {
      PRINT("\t prop(%d) = %e\n",j,GetProperty()[j]) ;
    }
      
    PRINT("\n") ;
      
    PRINT("\t Curves:\n") ;
    PRINT("\t Nb of curves = %d\n",nb_cv) ;
      
    for(int j = 0 ; j < nb_cv ; j++) {
      PRINT("\t curve(%d): np = %d\n",j + 1,Curve_GetNbOfPoints(cv + j)) ;
    }
    #undef PRINT
  }


/* Old notations which should be eliminated */
#define mate_t                 Material_t
#define dmat                   Material_ScanProperties1
#define lit_mate               Material_ScanProperties2


#ifdef __CPLUSPLUS
}
#endif

/* For the macros */
#include <stdlib.h>
#include "Fields.h"
#include "Functions.h"
#include "Curves.h"
#include "Geometry.h"
#include "Model.h"
#include "GenericData.h"
#include "Materials.h"
#include "Models.h"


#endif
