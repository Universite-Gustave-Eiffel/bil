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




Load_t* Load_New(void)
{
  Load_t* load = (Load_t*) Mry_New(Load_t) ;
  
  
  /* Allocation of space for the name of type */
  {
    char* type =  (char*) Mry_New(char[Load_MaxLengthOfKeyWord]) ;
  
    Load_GetType(load) = type ;
  }
  
  
  /* Allocation of space for the name of equation */
  {
    char* name = (char*) Mry_New(char[Load_MaxLengthOfKeyWord]) ;
  
    Load_GetNameOfEquation(load) = name ;
  }
  
  
  /* Allocation of space for the region name */
  {
    char* name = (char*) Mry_New(char[Load_MaxLengthOfRegionName]) ;
    
    Load_GetRegionName(load) = name ;
  }
  
  return(load) ;
}



void (Load_Delete)(void* self)
{
  Load_t* load = (Load_t*) self ;
  
  {
    char* type = Load_GetType(load) ;
    
    if(type) {
      free(type) ;
    }
  }
  
  {
    char* name = Load_GetNameOfEquation(load) ;
    
    if(name) {
      free(name) ;
    }
  }
  
  {
    char* name = Load_GetRegionName(load) ;
    
    if(name) {
      free(name) ;
    }
  }
}



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
      Load_GetRegionTag(load) = atoi(name) ;
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
    
    Load_GetFieldIndex(load) = -1 ;
    Load_GetField(load) = NULL ;
    
    if(n) {
      Fields_t* fields = Load_GetFields(load) ;
      int n_fields = Fields_GetNbOfFields(fields) ;
      int ifld = i - 1 ;

      Load_GetFieldIndex(load) = ifld ;
      
      if(ifld < 0) {

        Load_GetField(load) = NULL ;
        
      } else if(ifld < n_fields) {
        Field_t* field = Fields_GetField(fields) ;
        
        Load_GetField(load) = field + ifld ;

      } else {

        arret("Load_Scan: field out of range") ;

      }
    }
  }


  /* Function */
  {
    int i ;
    int n = String_FindAndScanExp(line,"Func,Fonc",","," = %d",&i) ;
    
    Load_GetFunctionIndex(load) = -1 ;
    Load_GetFunction(load) = NULL ;
    
    if(n) {
      Functions_t* functions = Load_GetFunctions(load) ;
      int n_functions = Functions_GetNbOfFunctions(functions) ;
      int ifct = i - 1 ;
      
      Load_GetFunctionIndex(load) = ifct ;
      
      if(ifct < 0) {
        
        Load_GetFunction(load) = NULL ;
        
      } else if(ifct < n_functions) {
        Function_t* function = Functions_GetFunction(functions) ;
        
        Load_GetFunction(load) = function + ifct ;
        
      } else {

        arret("Load_Scan: function out of range") ;

      }
    }
  }
}
