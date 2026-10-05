#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Message.h"
#include "DataFile.h"
#include "Functions.h"
#include "Function.h"
#include "Fields.h"
#include "Field.h"
#include "String_.h"
#include "Mry.h"
#include "Load.h"




Load_t* (Load_New)(Fields_t* fields,Functions_t* functions)
{
  Load_t* load = (Load_t*) Mry_New(Load_t) ;
  
  Load_SetFields(load,fields);
  Load_SetFunctions(load,functions);
  
  /* Allocation of space for the name of type */
  {
    char* type =  (char*) Mry_New(char,Load_MaxLengthOfKeyWord) ;
  
    Load_SetType(load,type) ;
  }
  
  
  /* Allocation of space for the name of equation */
  {
    char* name = (char*) Mry_New(char,Load_MaxLengthOfKeyWord) ;
  
    Load_SetNameOfEquation(load,name) ;
  }
  
  
  /* Allocation of space for the region name */
  {
    char* name = (char*) Mry_New(char,Load_MaxLengthOfRegionName) ;
    
    Load_SetRegionName(load,name) ;
  }
  
  return(load) ;
}



void (Load_Delete)(void* self)
{
  Load_t* load = (Load_t*) self ;
  
  {
    char* type = Load_GetType(load) ;
    
    if(type) {
      Mry_Free(type) ;
    }
  }
  
  {
    char* name = Load_GetNameOfEquation(load) ;
    
    if(name) {
      Mry_Free(name) ;
    }
  }
  
  {
    char* name = Load_GetRegionName(load) ;
    
    if(name) {
      Mry_Free(name) ;
    }
  }
}


#if 0
void Load_Scan(Load_t* load,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  

  /* Region */
  {
    char name[Load_MaxLengthOfRegionName] ;
    int n = String_FindAndScanExp(line,"Reg",","," = %s",name) ;
    //int i ;
    //int n = String_FindAndScanExp(line,"Reg",","," = %d",&i) ;
    
    if(n) {
      strncpy(Load_GetRegionName(load),name,Load_MaxLengthOfRegionName)  ;
    } else {
      arret("Load_Scan: no region") ;
    }
  }


  /* Equation */
  {
    char name[Load_MaxLengthOfKeyWord] ;
    int n = String_FindAndScanExp(line,"Equ",","," = %s",name) ;
    
    if(n) {
      strncpy(Load_GetNameOfEquation(load),name,Load_MaxLengthOfKeyWord) ;
      
      if(strlen(Load_GetNameOfEquation(load)) > Load_MaxLengthOfKeyWord) {
        arret("Load_Scan: name %s too long",name) ;
      }
      
      if(isdigit(Load_GetNameOfEquation(load)[0])) {
        if(atoi(Load_GetNameOfEquation(load)) < 1) {
          arret("Load_Scan: not positive number") ;
        }
      }
    } else {
      arret("Load_Scan: no equation") ;
    }
  }


  /* Type */
  {
    char name[Load_MaxLengthOfKeyWord] ;
    int n = String_FindAndScanExp(line,"Type",","," = %s",name) ;
    
    if(n) {
      strncpy(Load_GetType(load),name,Load_MaxLengthOfKeyWord) ;
      
      if(strlen(Load_GetType(load)) > Load_MaxLengthOfKeyWord) {
        arret("Load_Scan: name %s too long",name) ;
      }
      
      if(String_CaseIgnoredIs(name,"flux")) {
        Message_Warning("Load_Scan:\n"\
        "since June 2022, the definition of the type \"flux\" has changed\n"\
        "the old definition of this type is renamed  \"cumulflux\"\n"\
        "so use the types:\n"\
        "- \"flux\" for a real flux (e.g. kg/m2/s)\n"\
        "- \"cumulflux\" for a cumulative flux (e.g. kg/m2)\n") ;
      }
      
    } else {
      arret("Load_Scan: no Type") ;
    }
  }


  /* Field */
  {
    int i ;
    int n = String_FindAndScanExp(line,"Field,Champ",","," = %d",&i) ;
    
    //Load_SetFieldIndex(load,-1) ;
    Load_SetField(load,NULL) ;
    
    if(n) {
      Fields_t* fields = Load_GetFields(load) ;
      size_t n_fields = Fields_GetNbOfFields(fields) ;
      int ifld = i ;

      //Load_SetFieldIndex(load,ifld) ;
      
      if(ifld <= 0) {

        Load_SetField(load,NULL) ;
        
      } else if(ifld <= n_fields) {
        Field_t* field = Fields_GetField(fields) ;
        
        Load_SetField(load,field + ifld - 1) ;

      } else {

        arret("Load_Scan: field out of range") ;

      }
    } else {
      arret("Load_Scan: no field") ;
    }
  }


  /* Function */
  {
    int i ;
    int n = String_FindAndScanExp(line,"Func,Fonc",","," = %d",&i) ;
    
    //Load_SetFunctionIndex(load,-1) ;
    Load_SetFunction(load,NULL) ;
    
    if(n) {
      Functions_t* functions = Load_GetFunctions(load) ;
      size_t n_functions = Functions_GetNbOfFunctions(functions) ;
      int ifct = i ;
      
      //Load_SetFunctionIndex(load,ifct) ;
      
      if(ifct <= 0) {
        
        Load_SetFunction(load,NULL) ;
        
      } else if(ifct <= n_functions) {
        Function_t* function = Functions_GetFunction(functions) ;
        
        Load_SetFunction(load,function + ifct -1) ;
        
      } else {

        arret("Load_Scan: function out of range") ;

      }
    } else {
      arret("Load_Scan: no function") ;
    }
  }
}
#else
void Load_Scan(Load_t* load,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  char region[Load_MaxLengthOfRegionName] ;
  char equation[Load_MaxLengthOfKeyWord] ;
  char type[Load_MaxLengthOfKeyWord] ;
  size_t ifld ;
  size_t ifct ;
  

  /* Region */
  {
    int n = String_FindAndScanExp(line,"Reg",","," = %s",region) ;
    
    if(!n) {
      arret("Load_Scan: no region") ;
    }
  }


  /* Equation */
  {
    int n = String_FindAndScanExp(line,"Equ",","," = %s",equation) ;
    
    if(!n) {
      arret("Load_Scan: no equation") ;
    }
  }


  /* Type */
  {
    int n = String_FindAndScanExp(line,"Type",","," = %s",type) ;
    
    if(!n) {
      arret("Load_Scan: no Type") ;
    }
  }


  /* Field */
  {
    int n = String_FindAndScanExp(line,"Field,Champ",","," = %lu",&ifld) ;
        
    if(!n) {
      arret("Load_Scan: no field") ;
    }
  }


  /* Function */
  {
    int n = String_FindAndScanExp(line,"Func,Fonc",","," = %lu",&ifct) ;
        
    if(!n) {
      arret("Load_Scan: no function") ;
    }
  }

  Load_Set(load,region,equation,type,ifld,ifct);
}
#endif
