#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Models.h"
#include "Model.h"

#include "Message.h"
#include "Geometry.h"
#include "DataFile.h"
#include "Mry.h"


static Models_t* (Models_CreateAll)(Geometry_t*) ;


Models_t* (Models_New)(Geometry_t* geom,DataFile_t* datafile)
{
  Models_t* models = (Models_t*) Mry_New(Models_t) ;
  
  Models_SetMaxNbOfModels(models,Models_MaxNbOfModels) ;
  Models_SetNbOfModels(models,0) ;
  Models_SetModel(models,NULL) ;
  Models_SetGeometry(models,geom) ;
  Models_SetDataFile(models,datafile) ;
    
  {
    Model_t* model = Mry_Create(Model_t,Models_MaxNbOfModels,Model_New(models)) ;

    Models_SetModel(models,model) ;
  }

  return(models) ;
}



void  (Models_Delete)(void* self)
{
  Models_t* models = (Models_t*) self ;
  
  if(models) {
    Model_t* model = Models_GetModel(models) ;
    
    if(model) {
      Mry_Delete(model,Models_MaxNbOfModels,Model_Delete);    
      Mry_Free(model) ;
      Models_SetModel(models,NULL) ;
      Models_SetMaxNbOfModels(models,0) ;
      Models_SetNbOfModels(models,0) ;
    }
  }
}


#if 0
Models_t* (Models_Create)(Geometry_t* geom,DataFile_t* datafile)
{
  size_t n_models = DataFile_CountMainTokens(datafile,"Models",",") ;
  Models_t* models  = Models_New(geom,datafile) ;
    
  if(n_models <= 0) {
    return(models) ;
  }
  
  
  Message_Direct("Enter in %s","Models") ;
  Message_Direct("\n") ;


  {    
    Models_SetNbOfModels(models,n_models);

    /* Read the models */
    for(size_t i = 0 ; i < n_models ; i++) {
      Model_t* model = Models_GetModel(models) + i ;
      char* c = DataFile_FindNthToken(datafile,"Model",",",i + 1) ;
      
      c = String_SkipLine(c) ;
      
      DataFile_SetCurrentPositionInFileContent(datafile,c) ;
      
      Message_Direct("Enter in %s %lu","Model",i+1) ;
      Message_Direct("\n") ;
      
      Model_Scan(model,datafile) ;
    }
  }
  
  return(models) ;
}
#else
Models_t* (Models_Create)(Geometry_t* geom,DataFile_t* datafile)
{
  Models_t* models  = Models_New(geom,datafile) ;

  Models_Scan(models,datafile);
  
  return(models) ;
}
#endif



void (Models_Scan)(Models_t* models,DataFile_t* datafile)
{
  size_t n_models = DataFile_CountMainTokens(datafile,"Models",",") ;
    
  if(n_models <= 0) {
    return ;
  }
  
  
  Message_Direct("Enter in %s","Models") ;
  Message_Direct("\n") ;


  {    
    Models_SetNbOfModels(models,n_models);

    /* Read the models */
    for(size_t i = 0 ; i < n_models ; i++) {
      Model_t* model = Models_GetModel(models) + i ;
      char* c = DataFile_FindNthToken(datafile,"Model",",",i + 1) ;
      
      c = String_SkipLine(c) ;
      
      DataFile_SetCurrentPositionInFileContent(datafile,c) ;
      
      Message_Direct("Enter in %s %lu","Model",i+1) ;
      Message_Direct("\n") ;
      
      Model_Scan(model,datafile) ;
    }
  }
  
  return ;
}



Models_t* (Models_CreateAll)(Geometry_t* geom)
/** Create the models found in "ListOfModels.h"  */
{
  Models_t* models = Models_New(geom) ;
  
  {
    size_t n = Model_NbOfListedModels ;
    const char* modelnames[] = {Model_ListOfNames} ;
    
    Models_SetNbOfModels(models,n);
  
    for(size_t i = 0 ; i < n ; i++) {
      Model_t* model_i = Models_GetModel(models) + i ;
      
      Model_Initialize(model_i,modelnames[i]) ;
    }
  }
  
  return(models) ;
}



void (Models_PrintAll)(char* codename,FILE* ficd)
{
  //Geometry_t geom = {3} ;
  Geometry_t* geom = Geometry_New() ;
  Models_t* models = Models_CreateAll(geom) ;
  size_t n_models = Models_GetNbOfModels(models) ;
  Model_t* model = Models_GetModel(models) ;

  if(!codename) { /* all */    
    if(!ficd) {
      printf("  Model         |  Authors       |  Short Title\n") ;
      /*     "  14c...........|  14c...........|  ...........\n" */
      printf("----------------|----------------|-------------\n") ;
    }

    for(size_t i = 0 ; i < n_models ; i++) {
      Model_t* model_i = model + i ;
      
      printf("  %-14.14s|",Model_GetCodeNameOfModel(model_i)) ;
      printf("  %-14.14s|",Model_GetNameOfAuthors(model_i)) ;
      printf("  %-s",Model_GetShortTitle(model_i)) ;
      printf("\n") ;
      
      if(ficd) {
        printf("Model = %s : ",Model_GetCodeNameOfModel(model_i)) ;
        Model_PrintModelProp(model_i,ficd) ;
        printf("=============================================") ;
        printf("\n\n") ;
      }
    }
    
  } else {
    Model_t* model_i = Models_FindModel(models,codename) ;
    
    if(model_i) {
      printf("Model = %s : ",codename) ;
      Model_PrintModelProp(model_i,ficd) ;
    }
  }
  
  Geometry_Delete(geom);
  Models_Delete(models) ;
  Mry_Free(models) ;
}
