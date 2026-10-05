#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "DataFile.h"
#include "String_.h"
#include "Mry.h"
#include "Functions.h"
#include "Function.h"



Functions_t* (Functions_New)(void)
{
  Functions_t* functions   = (Functions_t*) Mry_New(Functions_t) ;
  
  
  Functions_SetNbOfFunctions(functions,0) ;
  Functions_SetFunction(functions,NULL) ;

  {
    Function_t* function = Mry_Create(Function_t,Functions_MaxNbOfFunctions,Function_New()) ;
    
    Functions_SetFunction(functions,function) ;
  }
  
  return(functions) ;
}


#if 0
Functions_t* (Functions_Create)(DataFile_t* datafile)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"FONC,FUNC,Functions",",") ;
  size_t n_fncts = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  Functions_t* functions = Functions_New() ;
  
  
  {
    int i = String_ToInt(c) ;

    if(i <= 0) return(functions) ;
  }
  

  Message_Direct("Enter in %s","Functions") ;
  Message_Direct("\n") ;


  {    
    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
    
    Functions_SetNbOfFunctions(functions,n_fncts);
    for(size_t i = 0 ; i < n_fncts ; i++) {
      Function_t* function = Functions_GetFunction(functions) + i ;
  
      Message_Direct("Enter in %s %lu","Function",i + 1) ;
      Message_Direct("\n") ;
      
      Function_Scan(function,datafile) ;
    }
  }
  
  return(functions) ;
}
#else
Functions_t* (Functions_Create)(DataFile_t* datafile)
{
  Functions_t* functions = Functions_New() ;
  
  Functions_Scan(functions,datafile);
  
  return(functions) ;
}
#endif



void (Functions_Scan)(Functions_t* functions,DataFile_t* datafile)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"FONC,FUNC,Functions",",") ;
  size_t n_fncts = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  
  {
    int i = String_ToInt(c) ;

    if(i <= 0) return ;
  }
  

  Message_Direct("Enter in %s","Functions") ;
  Message_Direct("\n") ;


  {    
    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
    
    Functions_SetNbOfFunctions(functions,n_fncts);
    for(size_t i = 0 ; i < n_fncts ; i++) {
      Function_t* function = Functions_GetFunction(functions) + i ;
  
      Message_Direct("Enter in %s %lu","Function",i + 1) ;
      Message_Direct("\n") ;
      
      Function_Scan(function,datafile) ;
    }
  }
  
  return ;
}



void (Functions_Delete)(void* self)
{
  Functions_t* functions = (Functions_t*) self ;

  if(functions) {
    Function_t* function = Functions_GetFunction(functions) ;

    if(function) {
      size_t n = Functions_GetCapacity(functions) ;

      Mry_Delete(function,n,Function_Delete) ;
      Mry_Free(function) ;
      Functions_SetFunction(functions,NULL) ;
    }
  }
}
