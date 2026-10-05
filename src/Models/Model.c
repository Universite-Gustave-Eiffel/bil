#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Model.h"
#include "Models.h"

#include "Message.h"
#include "Geometry.h"
#include "DataFile.h"
#include "ObVal.h"
#include "Views.h"
#include "Mry.h"




Model_t* (Model_New)(Models_t* models)
{
  Model_t* model = (Model_t*) Mry_New(Model_t) ;
  
  Model_SetParentModels(model,models);
  
  {
    /* Allocation of space for name of equations */
    {
      char** names = (char**) Mry_New(char*,Model_MaxNbOfEquations) ;
      
      Model_SetNameOfEquation(model,names) ;
    }
      
    {
      char* name = (char*) Mry_New(char,Model_MaxLengthOfKeyWord*Model_MaxNbOfEquations) ;
    
      {
        for(int j = 0 ; j < Model_MaxNbOfEquations ; j++) {
          Model_GetNameOfEquation(model)[j] = name + j*Model_MaxLengthOfKeyWord;
          Model_CopyNameOfEquation(model,j,"\0");
        }
      }
    }
    
    
    /* Allocation of space for name of unknowns */
    {
      char** names = (char**) Mry_New(char*,Model_MaxNbOfEquations) ;
    
      Model_SetNameOfUnknown(model,names) ;
    }
      
    {
      char* name = (char*) Mry_New(char,Model_MaxLengthOfKeyWord*Model_MaxNbOfEquations) ;
    
      {
        for(int j = 0 ; j < Model_MaxNbOfEquations ; j++) {
          Model_GetNameOfUnknown(model)[j]  = name + j*Model_MaxLengthOfKeyWord;
          Model_CopyNameOfUnknown(model,j,"\0");
        }
      }
    }
    
    
    /* Allocation of space for the sequential indexes of unknowns/equations */
    {
      int* ind = (int*) Mry_New(int,Model_MaxNbOfEquations) ;
    
      Model_SetSequentialIndexOfUnknown(model,ind) ;
    }
    
    
    /* Allocation of space for code name of the model */
    {
      char* name = (char*) Mry_New(char,Model_MaxLengthOfKeyWord) ;
      
      Model_SetCodeNameOfModel(model,name) ;
    }
  
    
    /* Allocation of space for short title of the model */
    {
      char* name = (char*) Mry_New(char,Model_MaxLengthOfShortTitle) ;
      
      Model_SetShortTitle(model,name) ;
      Model_CopyShortTitle(model,"\0") ;
    }
  
    
    /* Allocation of space for name of authors */
    {
      char* name = (char*) Mry_New(char,Model_MaxLengthOfAuthorNames) ;
    
      Model_SetNameOfAuthors(model,name) ;
      Model_CopyNameOfAuthors(model,"\0") ;
    }
    
    
    /* Allocation of space for objective values */
    {
      ObVal_t* obval = (ObVal_t*) Mry_New(ObVal_t,Model_MaxNbOfEquations) ;
    
      Model_SetObjectiveValue(model,obval) ;
    }
    
    
    /* Allocation of space for views */
    {
      Views_t* views = Views_Create(Model_MaxNbOfViews) ;
      
      Model_SetViews(model,views) ;
    }
  }
  
  return(model) ;
}



void  (Model_Delete)(void* self)
{
  Model_t* model = (Model_t*) self ;
  
  if(model) {
    {
      char** names = Model_GetNameOfEquation(model) ;
      
      if(names) {
        char* name = Model_GetNameOfEquation(model)[0] ;
    
        if(name) {
          Mry_Free(name) ;
        }

        Mry_Free(names) ;
        Model_SetNameOfEquation(model,nullptr) ;
      }
    }
    
    {
      char** names = Model_GetNameOfUnknown(model) ;
      
      if(names) {
        char* name = Model_GetNameOfUnknown(model)[0] ;
    
        if(name) {
          Mry_Free(name) ;
        }

        Mry_Free(names) ;
        Model_SetNameOfUnknown(model,nullptr) ;
      }
    }
    
    {
      int* ind = Model_GetSequentialIndexOfUnknown(model) ;
    
      if(ind) {
        Mry_Free(ind) ;
        Model_SetSequentialIndexOfUnknown(model,nullptr) ;
      }
    }
    
    {
      char* name = Model_GetCodeNameOfModel(model) ;
      
      if(name) {
        Mry_Free(name) ;
        Model_SetCodeNameOfModel(model,nullptr) ;
      }
    }
    
    {
      char* name = Model_GetShortTitle(model) ;
      
      if(name) {
        Mry_Free(name) ;
        Model_SetShortTitle(model,nullptr) ;
      }
    }
    
    {
      char* name = Model_GetNameOfAuthors(model) ;
      
      if(name) {
        Mry_Free(name) ;
        Model_SetNameOfAuthors(model,nullptr) ;
      }
    }
    
    {
      ObVal_t* obval = Model_GetObjectiveValue(model) ;
      
      if(obval) {
        Mry_Free(obval) ;
        Model_SetObjectiveValue(model,nullptr) ;
      }
    }
    
    {
      Views_t* views = Model_GetViews(model) ;
      
      if(views) {
        Views_Delete(views) ;
        Mry_Free(views) ;
        Model_SetViews(model,nullptr) ;
      }
    }
  }
}



void (Model_Scan)(Model_t* model,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  std::string codename;
  std::vector<std::string> equ_vec;
  std::vector<std::string> unk_vec;
  
  /* Code name of the model */
  {
    int n = String_FindAndScanExp(line,"Name",","," = %s",codename.data()) ;
    
    if(!n) {
      arret("Model_Scan") ;
    }
  }
      
  {
    /* Name of equations */
    if(String_FindAndScanExp(line,"Equations",","," = ")) {
      char* pline_equ = String_GetAdvancedPosition ;

      /* Name of unknowns */
      if(String_FindAndScanExp(line,"Unknowns",","," = ")) {
        char* pline_unk = String_GetAdvancedPosition ;
        size_t n_equ = equ_vec.size() ;
          
        pline_equ = String_SkipBlankChars(pline_equ) ;
        while(strncmp(pline_equ,"Unknowns",7) && n_equ < Model_MaxNbOfEquations) {
          char equ[Model_MaxLengthOfKeyWord];
          char unk[Model_MaxLengthOfKeyWord];

          pline_equ += String_Scan(pline_equ,"%s",equ) ;
          pline_unk += String_Scan(pline_unk,"%s",unk) ; 
          pline_equ = String_SkipBlankChars(pline_equ) ;

          equ_vec.push_back(equ);
          unk_vec.push_back(unk);
        }
      }
    }
  }

  Model_Set(model,codename,equ_vec,unk_vec) ;
}