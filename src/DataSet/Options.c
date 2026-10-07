#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Mry.h"
#include "Context.h"
#include "Message.h"
#include "Options.h"
#include "Module.h"


/* Global functions */
//static void   Options_SetDefault(Options_t*) ;
//static void   Options_Initialize(Options_t*) ;



Options_t*  (Options_New)(Context_t* ctx)
{
  Options_t* options = (Options_t*) Mry_New(Options_t) ;

  {
    size_t n = Options_MaxLengthOfKeyWord ;
    char* c = (char*) Mry_New(char,9*n) ;

    Options_SetPrintedInfos(options,c) ;
    Options_SetResolutionMethod(options,c + n) ;
    Options_SetPrintLevel(options,c + 2*n) ;
    Options_SetModule(options,c + 3*n) ;
    Options_SetNbOfThreads(options,c + 4*n) ;
    Options_SetGraphMethod(options,c + 5*n);
    Options_SetElementOrderingMethod(options,c + 6*n);
    Options_SetNodalOrderingMethod(options,c + 7*n);
    Options_SetPostProcessingMethod(options,c + 8*n);
  }
  
  Options_SetDefault(options) ;

  {
    Context_t* ctx0 = Context_New() ;

    Options_SetContext(options,ctx0) ;
  }

  if(ctx) {
    CommandLine_t* cmd = Context_GetCommandLine(ctx) ;
    int argc = CommandLine_GetNbOfArgs(cmd) ;
    char** argv = CommandLine_GetArg(cmd) ;
    Context_t* ctx0 = Options_GetContext(options) ;
    
    Context_Set(ctx0,argc,argv);
    Options_Initialize(options) ;
  }
  
  return(options) ;
}



void (Options_Delete)(void* self)
{
  Options_t* options = (Options_t*) self ;
  
  if(options){
    {
      char* c = Options_GetPrintedInfos(options) ;
    
      if(c) {
        Mry_Free(c) ;
        Options_SetPrintedInfos(options,NULL) ;
      }
    }

    {
      Context_t* ctx = Options_GetContext(options) ;
    
      if(ctx) {
        Context_Delete(ctx) ;
        Mry_Free(ctx) ;
        Options_SetContext(options,NULL) ;
      }
    }
  }
}
