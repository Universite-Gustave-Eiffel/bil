#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Message.h"
#include "Context.h"
#include "Exception.h"
#include "Mry.h"
#include "Module.h"
#include "Modules.h"


Module_t* (Module_New)(void)
{
  Module_t* module = (Module_t*) Mry_New(Module_t) ;
  
  {
    /* Allocation of space for the code name */
    {
      char* code = (char*) Mry_New(char,Module_MaxLengthOfKeyWord) ;
      
      Module_SetCodeNameOfModule(module,code) ;
    }
    
    
    /* Allocation of space for the short title */
    {
      char* title = (char*) Mry_New(char,Module_MaxLengthOfShortTitle) ;
      
      Module_SetShortTitle(module,title) ;
    
      Module_CopyShortTitle(module,"\0") ;
    }
    
    
    /* Allocation of space for the author names */
    {
      char* names = (char*) Mry_New(char,Module_MaxLengthOfAuthorNames) ;
      
      Module_SetNameOfAuthors(module,names) ;
      
      Module_CopyNameOfAuthors(module,"\0") ;
    }
    
    /* Default values */
    {
      const char*  modulenames[] = {Module_ListOfNames} ;
      const char*  defaultmodule = modulenames[0] ;

      Module_Set(module,defaultmodule);
      Module_SetNbOfSequences(module,1) ;
      Module_SetSequentialIndex(module,0) ;
    }
  }
  
  return(module) ;
}



void  (Module_Delete)(void* self)
{
  Module_t* module = (Module_t*) self ;
  
  if(module) {
    {
      char* name = Module_GetCodeNameOfModule(module);

      if(name) {
        Mry_Free(name) ;
        Module_SetCodeNameOfModule(module,nullptr);
      }
    }

    {
      char* name = Module_GetShortTitle(module);

      if(name) {
        Mry_Free(name) ;
        Module_SetShortTitle(module,nullptr);
      }
    }

    {
      char* name = Module_GetNameOfAuthors(module);

      if(name) {
        Mry_Free(name) ;
        Module_SetNameOfAuthors(module,nullptr);
      }
    }
  }
}
