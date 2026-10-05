#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Message.h"
#include "Context.h"
#include "Mry.h"
#include "Modules.h"
#include "Module.h"



static Modules_t* (Modules_CreateAll)(void) ;



Modules_t* (Modules_New)(void)
{
  Modules_t* modules = (Modules_t*) Mry_New(Modules_t) ;
  
  Modules_SetNbOfModules(modules,0) ;
  
  {
    Module_t* module = Mry_Create(Module_t,Modules_MaxNbOfModules,Module_New());
      
    Modules_SetModule(modules,module) ;
  }
  
  return(modules) ;
}



Modules_t* (Modules_CreateAll)(void)
/** Create the modules found in "ListOfModules.h"  */
{
  Modules_t* modules = Modules_New() ;
  
  {
    size_t n = Module_NbOfListedModules ;
    const char* modulenames[] = {Module_ListOfNames} ;
  
    Modules_SetNbOfModules(modules,n) ;
    
    for(size_t i = 0 ; i < n ; i++) {
      Module_t* module_i = Modules_GetModule(modules) + i ;
      
      Module_Set(module_i,modulenames[i]) ;
    }
  }
  
  return(modules) ;
}





void  (Modules_Delete)(void* self)
{
  Modules_t* modules = (Modules_t*) self ;
  
  if(modules) {
    Module_t* module = Modules_GetModule(modules) ;

    if(module) {
      Mry_Delete(module,Modules_MaxNbOfModules,Module_Delete);
      Mry_Free(module) ;
      Modules_SetModule(modules,nullptr) ;
      Modules_SetNbOfModules(modules,0) ;
    }
  }
}



void (Modules_PrintAll)(char* codename)
{
  Modules_t* modules = Modules_CreateAll() ;
  size_t n_modules = Modules_GetNbOfModules(modules) ;
  Module_t* module = Modules_GetModule(modules) ;

  if(!codename) { /* all */    
    printf("  Module     | Short Title\n") ;
    printf("-------------|------------\n") ;

    for(size_t i = 0 ; i < n_modules ; i++) {
      Module_t* module_i = module + i ;
      
      printf("  %-10s | ",Module_GetCodeNameOfModule(module_i)) ;
      printf("%-s",Module_GetShortTitle(module_i)) ;
      printf("\n") ;
    }
    
  } else {
    Module_t* module_i = Modules_FindModule(modules,codename) ;
    
    if(module_i) {
      printf("Module = %s : ",codename) ;
      printf("%-s",Module_GetShortTitle(module_i)) ;
      printf("\n") ;
    }
  }
  
  Modules_Delete(modules) ;
}
