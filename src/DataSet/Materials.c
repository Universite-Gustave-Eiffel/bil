#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>
#include "Message.h"
#include "DataFile.h"
#include "Materials.h"
#include "Material.h"
#include "Models.h"
#include "Model.h"
#include "Curves.h"
#include "Mry.h"
#include "Geometry.h"
#include "Fields.h"
#include "Functions.h"
#include "ObVals.h"


/* Extern functions */

Materials_t* (Materials_New)(Models_t* models,Fields_t* fields,Functions_t* functions)
{
  Materials_t* materials   = (Materials_t*) Mry_New(Materials_t) ;

  Materials_SetFields(materials,fields) ;
  Materials_SetFunctions(materials,functions) ;
  Materials_SetUsedModels(materials,models) ;

  Materials_SetNbOfMaterials(materials,0);

  /* Allocate the materials */
  {
    Material_t* material = Mry_Create(Material_t,Materials_MaxNbOfMaterials,Material_New(materials)) ;

    Materials_SetMaterial(materials,material) ;
  } 
  
  return(materials) ;
}



void (Materials_Delete)(void* self)
{
  Materials_t* materials = (Materials_t*) self ;
  
  if(materials) {
    {
      Material_t* material = Materials_GetMaterial(materials) ;
    
      if(material) {
        size_t n = Materials_GetCapacity(materials) ;

        Mry_Delete(material,n,Material_Delete) ;
        Mry_Free(material) ;
        Materials_SetMaterial(materials,NULL) ;
      }
    }
  
    #if 0
    {
      Models_t* usedmodels = Materials_GetUsedModels(materials) ;
    
      if(usedmodels) {
        Models_Delete(usedmodels) ;
        Mry_Free(usedmodels) ;
        Materials_SetUsedModels(materials,NULL) ;
      }
    }
    #endif
  }
}


#if 0
Materials_t* (Materials_Create)(DataFile_t* datafile,Fields_t* fields,Functions_t* functions,Models_t* models)
{
  size_t n_mats = DataFile_CountTokens(datafile,"MATE,Material",",") ;
  Materials_t* materials = Materials_New(models,fields,functions) ;
  
  
  Message_Direct("Enter in %s","Materials") ;
  Message_Direct("\n") ;

  Materials_SetNbOfMaterials(materials,n_mats) ;
  

  /* Scan the datafile */
  {    
    for(size_t i = 0 ; i < n_mats ; i++) {
      Material_t* mat = Materials_GetMaterial(materials) + i ;
      char* c = DataFile_FindNthToken(datafile,"MATE,Material",",",i + 1) ;
      
      c = String_SkipLine(c) ;
      
      DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  
      Message_Direct("Enter in %s %lu","Material",i+1) ;
      Message_Direct("\n") ;
      
      Material_Scan(mat,datafile) ;
    }
  }
  
  DataFile_CloseFile(datafile) ;
  
  return(materials) ;
}
#else
Materials_t* (Materials_Create)(DataFile_t* datafile,Fields_t* fields,Functions_t* functions,Models_t* models)
{
  Materials_t* materials = Materials_New(models,fields,functions) ;
  
  Materials_Scan(materials,datafile);
  
  return(materials) ;
}
#endif



void (Materials_Scan)(Materials_t* materials,DataFile_t* datafile)
{
  size_t n_mats = DataFile_CountTokens(datafile,"MATE,Material",",") ;
  
  
  Message_Direct("Enter in %s","Materials") ;
  Message_Direct("\n") ;

  Materials_SetNbOfMaterials(materials,n_mats) ;
  

  /* Scan the datafile */
  {    
    for(size_t i = 0 ; i < n_mats ; i++) {
      Material_t* mat = Materials_GetMaterial(materials) + i ;
      char* c = DataFile_FindNthToken(datafile,"MATE,Material",",",i + 1) ;
      
      c = String_SkipLine(c) ;
      
      DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  
      Message_Direct("Enter in %s %lu","Material",i+1) ;
      Message_Direct("\n") ;
      
      Material_Scan(mat,datafile) ;
    }
  }
  
  DataFile_CloseFile(datafile) ;
  
  return ;
}


void  (Materials_LinkUpToObVals)(Materials_t* mats,ObVals_t* obvals)
/* Copy objective values in those of models */
{
  ObVal_t* obval = ObVals_GetObVal(obvals) ;
  size_t n_mats = Materials_GetNbOfMaterials(mats) ;
  Material_t* mat = Materials_GetMaterial(mats) ;
  
  for(size_t i = 0 ; i < n_mats ; i++) {
    Model_t* model = Material_GetModel(mat + i) ;
    ObVal_t* model_obval = Model_GetObjectiveValue(model) ;
    char** name_unk = Model_GetNameOfUnknown(model) ;
    size_t nb_equ = Model_GetNbOfEquations(model) ;
      
    for(size_t j = 0 ; j < nb_equ ; j++) {
      int k = ObVals_FindObValIndex(obvals,name_unk[j]) ;
        
      if(k >= 0) {
        model_obval[j] = obval[k] ;
      } else {
        arret("Materials_LinkUpToObVals: unknown %s not known",name_unk[j]) ;  
      }
    }
  }
}