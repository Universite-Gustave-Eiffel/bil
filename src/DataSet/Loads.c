#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Message.h"
#include "Geometry.h"
#include "DataFile.h"
#include "IntFcts.h"
#include "Functions.h"
#include "Fields.h"
#include "String_.h"
#include "Mry.h"
#include "Loads.h"
#include "Load.h"





Loads_t* (Loads_New)(Fields_t* fields,Functions_t* functions)
{
  Loads_t* loads = (Loads_t*) Mry_New(Loads_t) ;
  
  Loads_SetNbOfLoads(loads,0) ;
  
  {
    Load_t* load = Mry_Create(Load_t,Loads_MaxNbOfLoads,Load_New(fields,functions)) ;

    Loads_SetLoad(loads,load);
  }  

  return(loads) ;
}

#if 0
Loads_t* (Loads_Create)(DataFile_t* datafile,Fields_t* fields,Functions_t* functions)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"CHAR,LOAD,Loads",",") ;
  size_t n_loads = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  Loads_t* loads = Loads_New(fields,functions) ;
  
  {
    int i = String_ToInt(c) ;

    if(i <= 0) return(loads) ;
  }
  
  
  Message_Direct("Enter in %s","Loads") ;
  Message_Direct("\n") ;
  
  /* Scan the datafile */
  {    
    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
    
    /* Read the loads*/
    Loads_SetNbOfLoads(loads,n_loads);
    for(size_t i = 0 ; i < n_loads ; i++) {
      Load_t* load = Loads_GetLoad(loads) + i ;
    
      Message_Direct("Enter in %s %d","Load",i+1) ;
      Message_Direct("\n") ;
    
      Load_Scan(load,datafile) ;
    }
  }
  
  return(loads) ;
}
#else
Loads_t* (Loads_Create)(DataFile_t* datafile,Fields_t* fields,Functions_t* functions)
{
  Loads_t* loads = Loads_New(fields,functions) ;
  
  Loads_Scan(loads,datafile);
  
  return(loads) ;
}
#endif


void (Loads_Scan)(Loads_t* loads,DataFile_t* datafile)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"CHAR,LOAD,Loads",",") ;
  size_t n_loads = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  
  {
    int i = String_ToInt(c) ;

    if(i <= 0) return ;
  }
  
  
  Message_Direct("Enter in %s","Loads") ;
  Message_Direct("\n") ;
  
  /* Scan the datafile */
  {    
    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
    
    /* Read the loads*/
    Loads_SetNbOfLoads(loads,n_loads);
    for(size_t i = 0 ; i < n_loads ; i++) {
      Load_t* load = Loads_GetLoad(loads) + i ;
    
      Message_Direct("Enter in %s %d","Load",i+1) ;
      Message_Direct("\n") ;
    
      Load_Scan(load,datafile) ;
    }
  }
  
  return ;
}



void (Loads_Delete)(void* self)
{
  Loads_t* loads = (Loads_t*) self ;
  
  {
    size_t n_loads = Loads_GetNbOfLoads(loads) ;
    Load_t* load = Loads_GetLoad(loads) ;
    
    Mry_Delete(load,n_loads,Load_Delete) ;
    Mry_Free(load) ;
  }
}
