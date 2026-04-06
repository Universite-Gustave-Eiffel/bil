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
struct Model_t; //typedef struct Model_t        Model_t ;
struct Element_t;
struct IntFcts_t;
struct ShapeFcts_t;
struct Load_t;
struct Result_t;
struct Results_t;
struct Material_t;
struct DataFile_t;
struct Geometry_t;
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


extern Model_t*  (Model_New)       (void) ;
extern void      (Model_Delete)    (void*) ;
extern Model_t*  (Model_Initialize)(Model_t*,const char*,Geometry_t*,DataFile_t*) ;
extern void      (Model_Scan)(Model_t*,DataFile_t*,Geometry_t*) ;



#define Model_MaxLengthOfKeyWord        (30)
#define Model_MaxNbOfEquations          (10)
#define Model_MaxLengthOfShortTitle     (80)
#define Model_MaxLengthOfAuthorNames    (80)
#define Model_MaxNbOfViews              (Views_MaxNbOfViews)

#define Model_MaxNbOfVariables          (100)
#define Model_MaxNbOfVariableFluxes     (Model_MaxNbOfVariables)


/* Accessors */
#define Model_GetCodeNameOfModel(MOD)      ((MOD)->codename)
#define Model_GetNbOfEquations(MOD)        ((MOD)->nbofequations)
#define Model_GetNameOfEquation(MOD)       ((MOD)->nameofequations)
#define Model_GetNameOfUnknown(MOD)        ((MOD)->nameofunknowns)
#define Model_GetSequentialIndexOfUnknown(MOD)        ((MOD)->sequentialindex)
#define Model_GetGeometry(MOD)             ((MOD)->geometry)
#define Model_GetDataFile(MOD)             ((MOD)->datafile)
#define Model_GetShortTitle(MOD)           ((MOD)->shorttitle)
#define Model_GetNameOfAuthors(MOD)        ((MOD)->authors)
#define Model_GetNumericalMethod(MOD)      ((MOD)->numericalmethod)
#define Model_GetObjectiveValue(MOD)       ((MOD)->obval)
#define Model_GetViews(MOD)                ((MOD)->views)


#define Model_GetSetModelProperties(MOD)      ((MOD)->setmodelprop)
#define Model_GetReadMaterialProperties(MOD)  ((MOD)->readmatprop)
#define Model_GetPrintModelProperties(MOD)    ((MOD)->printmodelprop)
#define Model_GetDefineElementProperties(MOD) ((MOD)->defineelementprop)
#define Model_GetComputeInitialState(MOD)     ((MOD)->computeinitialstate)
#define Model_GetComputeExplicitTerms(MOD)    ((MOD)->computeexplicitterms)
#define Model_GetComputeImplicitTerms(MOD)    ((MOD)->computeimplicitterms)
#define Model_GetComputeMatrix(MOD)           ((MOD)->computematrix)
#define Model_GetComputeResidu(MOD)           ((MOD)->computeresidu)
#define Model_GetComputeLoads(MOD)            ((MOD)->computeloads)
#define Model_GetComputeOutputs(MOD)          ((MOD)->computeoutputs)
#define Model_GetComputePropertyIndex(MOD)    ((MOD)->computepropertyindex)
#define Model_GetComputeMaterialProperties(MOD)   ((MOD)->ComputeMaterialProperties)



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



struct Model_t {              /* model */
  Model_SetModelProperties_t*       setmodelprop ;
  Model_ReadMaterialProperties_t*   readmatprop ;
  Model_PrintModelProperties_t*     printmodelprop ;
  Model_DefineElementProperties_t*  defineelementprop ;
  Model_ComputeInitialState_t*      computeinitialstate ;
  Model_ComputeExplicitTerms_t*     computeexplicitterms ;
  Model_ComputeImplicitTerms_t*     computeimplicitterms ;
  Model_ComputeMatrix_t*            computematrix ;
  Model_ComputeResidu_t*            computeresidu ;
  Model_ComputeLoads_t*             computeloads ;
  Model_ComputeOutputs_t*           computeoutputs ;
  Model_ComputePropertyIndex_t*     computepropertyindex ;
  Model_ComputeMaterialProperties_t* ComputeMaterialProperties ;
    
  char*   codename ;          /* code name of the model */
  char*   shorttitle ;        /* Short title of the model */
  char*   authors ;           /* Authors of the model */
  
  unsigned short int nbofequations ;    /* Number of equations */
  char**   nameofequations ;  /* Names of equations */
  char**   nameofunknowns ;   /* Names of unknowns */
  int*     sequentialindex ;  /* Sequential indexes of unknowns/equations */
  
  Geometry_t* geometry  ;     /* Geometry of the problem being dealt with */
  DataFile_t* datafile ;      /* Datafile being dealt with */
  
  void*    numericalmethod ;  /* Numerical method */
  ObVal_t* obval ;            /* Objective values of unknowns */
  Views_t* views ;            /* Views */
} ;


/* Old notations which should be eliminated */
#define MAX_EQUATIONS     Model_MaxNbOfEquations


#ifdef __CPLUSPLUS
}
#endif

/* Need for the macros */
#include "Views.h"
#include "Geometry.h"
#endif
