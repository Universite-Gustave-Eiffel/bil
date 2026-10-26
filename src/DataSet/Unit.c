#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "DataFile.h"
#include "InternationalSystemOfUnits.h"
#include "Mry.h"
#include "String_.h"
#include "Unit.h"




Unit_t* (Unit_New)(void)
{
  Unit_t* unit = (Unit_t*) Mry_New(Unit_t) ;
    
    
  /* Allocation of space for the name of unit */
  {
    char* name = (char*) Mry_New(char,Unit_MaxLengthOfKeyWord) ;
  
    Unit_SetQuantity(unit,name) ;
  }
  {
    char* name = (char*) Mry_New(char,Unit_MaxLengthOfKeyWord) ;
  
    Unit_SetName(unit,name) ;
  }
  
  return(unit) ;
}



void (Unit_Delete)(void* self)
{
  Unit_t* unit = (Unit_t*) self ;
  
  {
    char* name = Unit_GetQuantity(unit) ;
    
    if(name) {
      Mry_Free(name) ;
      Unit_SetQuantity(unit,NULL) ;
    }
  }
  
  {
    char* name = Unit_GetName(unit) ;
    
    if(name) {
      Mry_Free(name) ;
      Unit_SetName(unit,NULL) ;
    }
  }
}



int (Unit_Scan)(Unit_t* unit,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  char value[Unit_MaxLengthOfKeyWord] ;

  /* Length */
  {
    int n = String_FindAndScanExp(line,"Length",","," = %s",value) ;
        
    if(n) {
      Unit_Set(unit,"Length",value) ;
      return(1);
    }
  }
  
  /* Time */
  {
    int n = String_FindAndScanExp(line,"Time",","," = %s",value) ;
        
    if(n) {
      Unit_Set(unit,"Time",value) ;
      return(1);
    }
  }
  
  /* Mass */
  {
    int n = String_FindAndScanExp(line,"Mass",","," = %s",value) ;
        
    if(n) {
      Unit_Set(unit,"Mass",value) ;
      return(1);
    }
  }
  
  return(0) ;
}



Unit_t* (Unit_Create)(char const* name,char const* value)
{
  Unit_t* unit = Unit_New() ;

  Unit_Set(unit,name,value);
  
  return(unit) ;
}
