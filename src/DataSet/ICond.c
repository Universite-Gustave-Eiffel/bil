#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Message.h"
#include "Mry.h"
#include "String_.h"
#include "Fields.h"
#include "Field.h"
#include "Functions.h"
#include "Function.h"
#include "ICond.h"
#include "DataFile.h"



ICond_t* (ICond_New)(Fields_t* fields,Functions_t* functions)
{
  ICond_t* icond = (ICond_t*) Mry_New(ICond_t) ;
      
  ICond_SetFields(icond,fields) ;
  ICond_SetFunctions(icond,functions) ;
    
  /* Allocation of space for the name of unknowns */
  {
    char* name = (char*) Mry_New(char,ICond_MaxLengthOfKeyWord) ;
  
    ICond_SetNameOfUnknown(icond,name) ;
  }
    
    
  /* Allocation of space for the name of files of nodal values */
  {
    char* filename = (char*) Mry_New(char,ICond_MaxLengthOfFileName) ;
  
    ICond_SetFileNameOfNodalValues(icond,filename) ;
    ICond_GetFileNameOfNodalValues(icond)[0] = '\0' ;
  }
  
  
  /* Allocation of space for the region name */
  {
    char* name = (char*) Mry_New(char,ICond_MaxLengthOfRegionName) ;
    
    ICond_SetRegionName(icond,name) ;
  }
  
  return(icond) ;
}



void (ICond_Delete)(void* self)
{
  ICond_t* icond = (ICond_t*) self ;
  
  {
    char* name = ICond_GetNameOfUnknown(icond) ;
    
    if(name) {
      Mry_Free(name) ;
    }
  }
  
  {
    char* name = ICond_GetFileNameOfNodalValues(icond) ;
    
    if(name) {
      Mry_Free(name) ;
    }
  }
  
  {
    char* name = ICond_GetRegionName(icond) ;
    
    if(name) {
      Mry_Free(name) ;
    }
  }
}



void (ICond_Scan)(ICond_t* icond,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  char region[ICond_MaxLengthOfRegionName] ;
  char unknown[ICond_MaxLengthOfKeyWord] ;
  char name[ICond_MaxLengthOfFileName] ;
  char* filename ;
  size_t ifld ;
  size_t ifct ;
  
  /* Region */
  {
    int n = String_FindAndScanExp(line,"Reg",","," = %s",region) ;
    
    if(!n) {
      arret("ICond_Scan: no region") ;
    }
  }
    
    
  /* Unknown */
  {
    int n = String_FindAndScanExp(line,"Unk,Inc",","," = %s",unknown) ;
        
    if(!n) {
      arret("ICond_Scan: no unknown") ;
    }
  }
    
    
  /* File */
  {
    int n = String_FindAndScanExp(line,"File,Fichier",","," = %s",name) ;
        
    if(n) {
      filename = name;
    } else {
      filename = nullptr;
    }
  }
    
    
  /* Field */
  {
    int n = String_FindAndScanExp(line,"Field,Champ",","," = %lu",&ifld) ;
        
    if(!n) {
      ifld = 0;
    }
  }
    
    
  /* Function (not mandatory) */
  {
    int n = String_FindAndScanExp(line,"Func,Fonc",","," = %lu",&ifct) ;
        
    if(!n) {
      ifct = 0;
    }
  }

  if(filename) {
    ICond_Set(icond,region,unknown,filename,ifct);
  } else {
    ICond_Set(icond,region,unknown,ifld,ifct);
  }
}