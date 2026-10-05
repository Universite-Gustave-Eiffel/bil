#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "DataFile.h"
#include "InternationalSystemOfUnits.h"
#include "Mry.h"
#include "Units.h"
#include "Unit.h"


Units_t* (Units_New)(void)
{
  Units_t* units = (Units_t*) Mry_New(Units_t) ;
  
  
  Units_SetNbOfUnits(units,0) ;
  
  {
    Unit_t* unit = Mry_Create(Unit_t,Units_MaxNbOfUnits,Unit_New()) ;
    
    Units_SetUnit(units,unit) ;
  }
  
  return(units) ;
}



void (Units_Delete)(void* self)
{
  Units_t* units = (Units_t*) self ;

  {
    Unit_t* unit = Units_GetUnit(units) ;
    
    if(unit) {
      size_t n = Units_GetCapacity(units) ;
      
      Mry_Delete(unit,n,Unit_Delete) ;
      Mry_Free(unit) ;
      Units_SetUnit(units,NULL) ;
    }
  }
}



#if 0
Units_t* (Units_Create)(DataFile_t* datafile)
{
  Units_t* units = (Units_t*) Units_New() ;
  
  
  {
    char* filecontent = DataFile_GetFileContent(datafile) ;
    char* c  = String_FindToken(filecontent,"UNITS,Units",",") ;
    
    if(!c) return(units) ;
  
      
    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  
    Message_Direct("Enter in %s","Units") ;
    Message_Direct("\n") ;
    
    {
      Unit_t* unit = Units_GetUnit(units) ;
      int n = 0 ;
      
      while(Unit_Scan(unit + n,datafile)) n++ ;

      Units_SetNbOfUnits(units,n) ;
      
      if(n > Units_MaxNbOfUnits) {
        arret("Units_Create: too many units") ;
      }
    }
  
  }
  
  return(units) ;
}
#else
Units_t* (Units_Create)(DataFile_t* datafile)
{
  Units_t* units = (Units_t*) Units_New() ;

  Units_Scan(units,datafile);
  
  return(units) ;
}
#endif




void (Units_Scan)(Units_t* units,DataFile_t* datafile)
{
  {
    char* filecontent = DataFile_GetFileContent(datafile) ;
    char* c  = String_FindToken(filecontent,"UNITS,Units",",") ;
    
    if(!c) return ;
  
      
    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  
    Message_Direct("Enter in %s","Units") ;
    Message_Direct("\n") ;
    
    {
      Unit_t* unit = Units_GetUnit(units) ;
      int n = 0 ;
      
      while(Unit_Scan(unit + n,datafile)) n++ ;

      Units_SetNbOfUnits(units,n) ;
      
      if(n > Units_MaxNbOfUnits) {
        arret("Units_Create: too many units") ;
      }
    }
  
  }
  
  return ;
}

