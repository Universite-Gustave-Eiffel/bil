#ifndef CONTEXT_H
#define CONTEXT_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Context_t; //typedef struct Context_t  Context_t ;
struct CommandLine_t;
struct Options_t;



extern Context_t*    (Context_New)(void);
extern Context_t*    (Context_Create)(int,char**) ;
extern void          (Context_Delete)(void*) ;


#define Context_GetHelpOnline(CTX)         ((CTX)->GetHelpOnline())
#define Context_GetPrintInfo(CTX)          ((CTX)->GetPrintInfo())
#define Context_GetPrintModel(CTX)         ((CTX)->GetPrintModel())
#define Context_GetPrintModule(CTX)        ((CTX)->GetPrintModule())
#define Context_GetPrintUsage(CTX)         ((CTX)->GetPrintUsage())
#define Context_GetReadOnly(CTX)           ((CTX)->GetReadOnly())
#define Context_GetCreateInputFile(CTX)    ((CTX)->GetCreateInputFile())
#define Context_GetInversePermutation(CTX) ((CTX)->GetInversePermutation())
#define Context_GetPostProcessing(CTX)     ((CTX)->GetPostProcessing())
#define Context_GetSolver(CTX)             ((CTX)->GetSolver())
#define Context_GetDebug(CTX)              ((CTX)->GetDebug())
#define Context_GetPrintLevel(CTX)         ((CTX)->GetPrintLevel())
#define Context_GetUseModule(CTX)          ((CTX)->GetUseModule())
#define Context_GetGraph(CTX)              ((CTX)->GetGraph())
#define Context_GetInputFileName(CTX)      ((CTX)->GetInputFileName())
#define Context_GetMiscellaneous(CTX)      ((CTX)->GetMiscellaneous())
#define Context_GetElementOrdering(CTX)    ((CTX)->GetElementOrdering())
#define Context_GetNodalOrdering(CTX)      ((CTX)->GetNodalOrdering())
#define Context_GetCommandLine(CTX)        ((CTX)->GetCommandLine())
#define Context_GetTest(CTX)               ((CTX)->GetTest())
#define Context_GetNbOfThreads(CTX)        ((CTX)->GetNbOfThreads())


#define Context_SetHelpOnline(CTX,A)         ((CTX)->SetHelpOnline(A))
#define Context_SetPrintInfo(CTX,A)          ((CTX)->SetPrintInfo(A))
#define Context_SetPrintModel(CTX,A)         ((CTX)->SetPrintModel(A))
#define Context_SetPrintModule(CTX,A)        ((CTX)->SetPrintModule(A))
#define Context_SetPrintUsage(CTX,A)         ((CTX)->SetPrintUsage(A))
#define Context_SetReadOnly(CTX,A)           ((CTX)->SetReadOnly(A))
#define Context_SetCreateInputFile(CTX,A)    ((CTX)->SetCreateInputFile(A))
#define Context_SetInversePermutation(CTX,A) ((CTX)->SetInversePermutation(A))
#define Context_SetPostProcessing(CTX,A)     ((CTX)->SetPostProcessing(A))
#define Context_SetSolver(CTX,A)             ((CTX)->SetSolver(A))
#define Context_SetDebug(CTX,A)              ((CTX)->SetDebug(A))
#define Context_SetPrintLevel(CTX,A)         ((CTX)->SetPrintLevel(A))
#define Context_SetUseModule(CTX,A)          ((CTX)->SetUseModule(A))
#define Context_SetGraph(CTX,A)              ((CTX)->SetGraph(A))
#define Context_SetInputFileName(CTX,A)      ((CTX)->SetInputFileName(A))
#define Context_SetMiscellaneous(CTX,A)      ((CTX)->SetMiscellaneous(A))
#define Context_SetElementOrdering(CTX,A)    ((CTX)->SetElementOrdering(A))
#define Context_SetNodalOrdering(CTX,A)      ((CTX)->SetNodalOrdering(A))
#define Context_SetCommandLine(CTX,A)        ((CTX)->SetCommandLine(A))
#define Context_SetTest(CTX,A)               ((CTX)->SetTest(A))
#define Context_SetNbOfThreads(CTX,A)        ((CTX)->SetNbOfThreads(A))

#define Context_Set(CTX,...)                 ((CTX)->Set(__VA_ARGS__))


/* All possible contexts that can be read from the command line */
#define Context_IsHelpOnline(CTX)         Context_GetHelpOnline(CTX)
#define Context_IsPrintInfo(CTX)          Context_GetPrintInfo(CTX)
#define Context_IsPrintModel(CTX)         Context_GetPrintModel(CTX)
#define Context_IsPrintModule(CTX)        Context_GetPrintModule(CTX)
#define Context_IsPrintUsage(CTX)         Context_GetPrintUsage(CTX)
#define Context_IsReadOnly(CTX)           Context_GetReadOnly(CTX)
#define Context_IsCreateInputFile(CTX)    Context_GetCreateInputFile(CTX)
#define Context_IsInversePermutation(CTX) Context_GetInversePermutation(CTX)
#define Context_IsPostProcessing(CTX)     Context_GetPostProcessing(CTX)
#define Context_IsGraph(CTX)              Context_GetGraph(CTX)
#define Context_IsMiscellaneous(CTX)      Context_GetMiscellaneous(CTX)
#define Context_IsElementOrdering(CTX)    Context_GetElementOrdering(CTX)
#define Context_IsNodalOrdering(CTX)      Context_GetNodalOrdering(CTX)
#define Context_IsUseModule(CTX)          Context_GetUseModule(CTX)
#define Context_IsTest(CTX)               Context_GetTest(CTX)
#define Context_IsNbOfThreads(CTX)        Context_GetNbOfThreads(CTX)



struct Context_t {        /* Context */
  CommandLine_t* _commandline ;
  void*   _help ;
  void*   _info ;
  void*   _printmodel ;
  void*   _printmodule ;
  void*   _printusage ;
  void*   _readonly ;
  void*   _create ;
  void*   _invperm ;
  void*   _postprocess ;
  void*   _solver ;
  void*   _usemodule ;
  void*   _debug ;
  void*   _printlevel ;
  void*   _graph ;
  void*   _inputfilename ;
  void*   _misc ;
  void*   _eorder ;
  void*   _norder ;
  void*   _nthreads ;
  void*   _test ;

  /* The getters */
  CommandLine_t* GetCommandLine(){return _commandline ;}
  void* GetHelpOnline(){return _help ;}
  void* GetPrintInfo(){return _info ;}
  void* GetPrintModel(){return _printmodel ;}
  void* GetPrintModule(){return _printmodule ;}
  void* GetPrintUsage(){return _printusage ;}
  void* GetReadOnly(){return _readonly ;}
  void* GetCreateInputFile(){return _create ;}
  void* GetInversePermutation(){return _invperm ;}
  void* GetPostProcessing(){return _postprocess ;}
  void* GetSolver(){return _solver ;}
  void* GetUseModule(){return _usemodule ;}
  void* GetDebug(){return _debug ;}
  void* GetPrintLevel(){return _printlevel ;}
  void* GetGraph(){return _graph ;}
  void* GetInputFileName(){return _inputfilename ;}
  void* GetMiscellaneous(){return _misc ;}
  void* GetElementOrdering(){return _eorder ;}
  void* GetNodalOrdering(){return _norder ;}
  void* GetNbOfThreads(){return _nthreads ;}
  void* GetTest(){return _test ;}

  /* The setters */
  void SetCommandLine(CommandLine_t* a){_commandline = a;}
  void SetHelpOnline(void* a){_help = a;}
  void SetPrintInfo(void* a){_info = a;}
  void SetPrintModel(void* a){_printmodel = a;}
  void SetPrintModule(void* a){_printmodule = a;}
  void SetPrintUsage(void* a){_printusage = a;}
  void SetReadOnly(void* a){_readonly = a;}
  void SetCreateInputFile(void* a){_create = a;}
  void SetInversePermutation(void* a){_invperm = a;}
  void SetPostProcessing(void* a){_postprocess = a;}
  void SetSolver(void* a){_solver = a;}
  void SetUseModule(void* a){_usemodule = a;}
  void SetDebug(void* a){_debug = a;}
  void SetPrintLevel(void* a){_printlevel = a;}
  void SetGraph(void* a){_graph = a;}
  void SetInputFileName(void* a){_inputfilename = a;}
  void SetMiscellaneous(void* a){_misc = a;}
  void SetElementOrdering(void* a){_eorder = a;}
  void SetNodalOrdering(void* a){_norder = a;}
  void SetNbOfThreads(void* a){_nthreads = a;}
  void SetTest(void* a){_test = a;}

  template<typename... Args>
  void Set(Args&&...);
  void Initialize(void);
} ;



#include <string.h>
#include <string>
#include <utility>
#include "String_.h"
#include "CommandLine.h"
#include "Message.h"

  template<typename... Args>
  inline void Context_t::Set(Args&&... args) {
    CommandLine_t* cmd = GetCommandLine();

    CommandLine_Set(cmd,std::forward<Args>(args)...);
    Initialize();
  }

  inline void Context_t::Initialize(void){
    CommandLine_t* cmd = GetCommandLine();
    int    argc = CommandLine_GetNbOfArgs(cmd) ;
    char** argv = CommandLine_GetArg(cmd) ;
    
    #if 0
    if(argc == 1) {
      SetPrintUsage((char**) argv) ;
      return ;
    }
    #endif
    
    
    /* Get line options */
    //for(int i = 1 ; i < argc ; i++) {
    for(int i = 0 ; i < argc ; i++) {
      
      if(argv[i][0] != '-') { /* File name */
        SetInputFileName((char**) argv + i) ;
        
      } else if(String_Is(argv[i],"-info",5)) {
        SetPrintInfo((char**) argv + i) ;
    
      } else if(String_Is(argv[i],"-help",5)) {
        SetHelpOnline((char**) argv + i) ;
        
      } else if(String_Is(argv[i],"-solver",strlen(argv[i]))) {
        SetSolver((char**) argv + i) ;
        if(i + 1 < argc) {
          i++ ;
        } else {
          Message_FatalError("Missing solver") ;
        }
        
        /* 
         * Skip two more entries if the following entry is either:
         * "-ff <factor>": the fill factor for multi-frontal methods
         **/
        if(i + 1 < argc) {
          if(String_Is(argv[i + 1],"-ff")) {
            if(i + 2 < argc) {
              i += 2 ;
            } else {
              Message_FatalError("Missing fill factor") ;
            }
          }
        }
        
        /* 
         * Skip some more entries for PetscKSP methods
         **/
        if(String_Is(argv[i],"petscksp",strlen(argv[i]))) {
        
          /* 
           * Skip two more entries if the following entry is either:
           * "-ksp_XXX" <YYY>: option for PetscKSP methods
           **/
          while(i + 1 < argc && String_Is(argv[i + 1],"-ksp_",5)) {
            if(i + 2 < argc) {
              i += 2 ;
            } else {
              Message_FatalError("Missing value for ksp option") ;
            }
          }
        
        
          /* 
           * Skip two more entries if the following entry is either:
           * "-pc_XXX" <YYY>: option for PetscKSP methods
           **/
        
          while(i + 1 < argc && String_Is(argv[i + 1],"-pc_",4)) {
            if(i + 2 < argc) {
              i += 2 ;
            } else {
              Message_FatalError("Missing value for pc option") ;
            }
          }
        
        
          /* 
           * Skip two more entries if the following entry is either:
           * "-mat_XXX" <YYY>: option for PetscKSP methods
           **/
        
          while(i + 1 < argc && String_Is(argv[i + 1],"-mat_",5)) {
            if(i + 2 < argc) {
              i += 2 ;
            } else {
              Message_FatalError("Missing value for mat option") ;
            }
          }
        
        
          /* 
           * Skip two more entries if the following entry is either:
           * "-log_XXX" <YYY>: option for PetscKSP methods
           **/
        
          while(i + 1 < argc && String_Is(argv[i + 1],"-log_",5)) {
            if(i + 2 < argc) {
              i += 2 ;
            } else {
              Message_FatalError("Missing value for log option") ;
            }
          }
        
        
          /* 
           * Skip one more entry if the following entry is:
           * "-mpi_linear_solver_server": option for PetscKSP methods
           **/
        
          while(i + 1 < argc && String_Is(argv[i + 1],"-mpi_linear_solver_server",5)) {
            i += 1 ;
          }
        }
      
      } else if(String_Is(argv[i],"-debug",strlen(argv[i]))) {
        SetDebug((char**) argv + i) ;
        if(i + 1 < argc) {
          i++ ;
        } else {
          Message_FatalError("Missing name of data to be printed") ;
        }
  
      } else if(String_Is(argv[i],"-level",strlen(argv[i]))) {
        SetPrintLevel((char**) argv + i) ;
        if(i + 1 < argc) {
          i++ ;
        } else {
          Message_FatalError("Missing level") ;
        }
  
      } else if(String_Is(argv[i],"-nthreads",strlen(argv[i]))) {
        SetNbOfThreads((char**) argv + i) ;
        if(i + 1 < argc) {
          i++ ;
        } else {
          Message_FatalError("Missing nb of requested threads") ;
        }
  
      } else if(String_Is(argv[i],"-with",strlen(argv[i]))) {
        SetUseModule((char**) argv + i) ;
        if(i + 1 < argc) {
          i++ ;
        } else {
          Message_FatalError("Missing module") ;
        }
        
        /* If the module is "Monolithic" we skip the next entry
         * which should be the nb of solutions requested. */
        {
          if(String_Is(argv[i],"Monolithic")) {
            if(i + 1 < argc) {
              i += 1 ;
            } else {
              Message_FatalError("Missing nb of solutions") ;
            }
          }
        }
        
        /* If the module is "SNIA" we skip the next entry
         * which should be the nb of sequences requested. */
        {
          if(String_Is(argv[i],"SNIA")) {
            if(i + 1 < argc) {
              i += 1 ;
            } else {
              Message_FatalError("Missing nb of sequences") ;
            }
          }
        }
  
      } else if(String_Is(argv[i],"-models",strlen(argv[i]))) {
        SetPrintModel((char**) argv + i) ;
  
      } else if(String_Is(argv[i],"-modules",strlen(argv[i]))) {
        SetPrintModule((char**) argv + i) ;
  
      } else if(String_Is(argv[i],"-readonly",strlen(argv[i]))) {
        SetReadOnly((char**) argv + i) ;
  
      } else if(String_Is(argv[i],"-create",strlen(argv[i]))) {
        SetCreateInputFile((char**) argv + i) ;
  
      } else if(String_Is(argv[i],"-graph",strlen(argv[i]))) {
        SetGraph((char**) argv + i) ;
        if(i + 1 < argc) {
          i++ ;
        } else {
          Message_FatalError("Missing graph method") ;
        }
  
      } else if(String_Is(argv[i],"-iperm",strlen(argv[i]))) {
        SetInversePermutation((char**) argv + i) ;
  
      } else if(String_Is(argv[i],"-eordering",strlen(argv[i]))) {
        SetElementOrdering((char**) argv + i) ;
        if(i + 1 < argc) {
          i++ ;
        } else {
          Message_FatalError("Missing element ordering method") ;
        }
  
      } else if(String_Is(argv[i],"-nordering",strlen(argv[i]))) {
        SetNodalOrdering((char**) argv + i) ;
        if(i + 1 < argc) {
          i++ ;
        } else {
          Message_FatalError("Missing nodal ordering method") ;
        }
  
      } else if(String_Is(argv[i],"-postprocessing",strlen(argv[i]))) {
        SetPostProcessing((char**) argv + i) ;
        if(i + 1 < argc) {
          i++ ;
        } else {
          Message_FatalError("Missing post-processing method") ;
        }
  
      } else if(String_Is(argv[i],"-miscellaneous",strlen(argv[i]))) {
        SetMiscellaneous((char**) argv + i) ;
  
      } else if(String_Is(argv[i],"-test",strlen(argv[i]))) {
        SetTest((char**) argv + i) ;
        
      } else {
        Message_FatalError("Unknown option") ;
      }
      
    }
    
  }



#ifdef __CPLUSPLUS
}
#endif
#endif
