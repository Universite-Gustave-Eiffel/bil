#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Message.h"
#include "DataFile.h"
#include "Mesh.h"
#include "Models.h"
#include "Model.h"
#include "String_.h"
#include "Mry.h"
#include "ObVals.h"
#include "ObVal.h"
#include "Materials.h"
#include "Material.h"
#include "Nodes.h"






ObVals_t*  (ObVals_New)(void)
{
  ObVals_t* obvals = (ObVals_t*) Mry_New(ObVals_t) ;
  
  ObVals_SetNbOfObVals(obvals,0);
  
  /* Allocation of space for the objective values */
  {
    ObVal_t* obval = Mry_Create(ObVal_t,ObVals_MaxNbOfObVals,ObVal_New()) ;

    ObVals_SetObVal(obvals,obval) ;
  }
  
  return(obvals) ;
}



void  (ObVals_Delete)(void* self)
{
  ObVals_t* obvals = (ObVals_t*) self ;
  
  if(obvals) {
    ObVal_t* obval = ObVals_GetObVal(obvals) ;
    
    if(obval) {
      Mry_Delete(obval,ObVals_MaxNbOfObVals,ObVal_Delete) ;
      Mry_Free(obval) ;
      ObVals_SetObVal(obvals,nullptr);
    }
  }
}




void  (ObVals_Scan)(ObVals_t* obvals,DataFile_t* datafile)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"OBJE,Objective Variations",",") ;
  
  
  if(!c) {
    Message_FatalError("No Objective Variations") ;
  }
  
  
  Message_Direct("Enter in %s","Objective Variations") ;
  Message_Direct("\n") ;



  c = String_SkipLine(c) ;

  DataFile_SetCurrentPositionInFileContent(datafile,c) ;



  /* Scan the datafile for objective values */
  {
    ObVal_t* obval = ObVals_GetObVal(obvals) ;
    size_t n_obvals = 0;
  
    #if 0
    for(size_t i = 0 ; i < n_obvals ; i++) {
      /* Check if a keyword is given twice */
      {
        char* line = DataFile_GetCurrentPositionInFileContent(datafile) ;
        char  name[ObVal_MaxLengthOfKeyWord] ;
        
        String_Scan(line," %s",name) ;
    
        if(strlen(name) > ObVal_MaxLengthOfKeyWord) {
          arret("ObVals_Scan: too long keyword") ;
        }
    
        for(size_t j = 0 ; j < i ; j++) {
          if(!strcmp(name,ObVal_GetNameOfUnknown(obval + j))) {
            arret("ObVals_Scan: keyword %s given twice",name) ;
          }
        }
      }
      
      ObVal_Scan(obval+i,datafile) ;
    }
    #else
    {
      char* line = DataFile_GetCurrentPositionInFileContent(datafile) ;

      while(String_FindChar(String_CopyLine(line),'=')) {
        char name[ObVal_MaxLengthOfKeyWord] ;
        
        String_Scan(line," %s",name) ;
    
        if(strlen(name) > ObVal_MaxLengthOfKeyWord) {
          arret("ObVals_Scan: too long keyword") ;
        }
    
        /* Check if a keyword is given twice */
        for(size_t j = 0 ; j < n_obvals ; j++) {
          if(!strcmp(name,ObVal_GetNameOfUnknown(obval + j))) {
            arret("ObVals_Scan: keyword %s given twice",name) ;
          }
        }
      
        ObVal_Scan(obval+n_obvals,datafile) ;
        n_obvals++;
        ObVals_SetNbOfObVals(obvals,n_obvals);
        line = DataFile_GetCurrentPositionInFileContent(datafile) ;
      }
    }
    #endif
  }
  
  return ;
}



int (ObVals_FindObValIndex)(ObVals_t* obvals,char* name)
{
  size_t n_obvals = ObVals_GetNbOfObVals(obvals) ;
  ObVal_t* obval = ObVals_GetObVal(obvals) ;
  
  {      
    for(size_t i = 0 ; i < n_obvals ; i++) {
      ObVal_t* obval_i = obval + i ;
      char* name_obval = ObVal_GetNameOfUnknown(obval_i) ;
          
      if(!strcmp(name,name_obval)) {
        return(i) ;
      }
    }
  }
    
  {
    arret("ObVals_FindObValIndex: unknown %s not found",name) ;
  }
      
  return(-1) ;
}
