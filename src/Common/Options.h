#ifndef OPTIONS_H
#define OPTIONS_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Options_t;
struct Context_t;


extern Options_t*  (Options_New)(Context_t* = nullptr) ;
extern void        (Options_Delete)(void*) ;


#define Options_MaxLengthOfKeyWord               (30)

#define Options_GetPrintedInfos(OPT)           ((OPT)->GetPrintedInfos())
#define Options_GetResolutionMethod(OPT)       ((OPT)->GetResolutionMethod())
#define Options_GetPrintLevel(OPT)             ((OPT)->GetPrintLevel())
#define Options_GetModule(OPT)                 ((OPT)->GetModule())
#define Options_GetGraphMethod(OPT)            ((OPT)->GetGraphMethod())
#define Options_GetElementOrderingMethod(OPT)  ((OPT)->GetElementOrderingMethod())
#define Options_GetNodalOrderingMethod(OPT)    ((OPT)->GetNodalOrderingMethod())
#define Options_GetPostProcessingMethod(OPT)   ((OPT)->GetPostProcessingMethod())
#define Options_GetContext(OPT)                ((OPT)->GetContext())
#define Options_GetNbOfThreads(OPT)            ((OPT)->GetNbOfThreads())

#define Options_SetPrintedInfos(OPT,A)           ((OPT)->SetPrintedInfos(A))
#define Options_SetResolutionMethod(OPT,A)       ((OPT)->SetResolutionMethod(A))
#define Options_SetPrintLevel(OPT,A)             ((OPT)->SetPrintLevel(A))
#define Options_SetModule(OPT,A)                 ((OPT)->SetModule(A))
#define Options_SetGraphMethod(OPT,A)            ((OPT)->SetGraphMethod(A))
#define Options_SetElementOrderingMethod(OPT,A)  ((OPT)->SetElementOrderingMethod(A))
#define Options_SetNodalOrderingMethod(OPT,A)    ((OPT)->SetNodalOrderingMethod(A))
#define Options_SetPostProcessingMethod(OPT,A)   ((OPT)->SetPostProcessingMethod(A))
#define Options_SetContext(OPT,A)                ((OPT)->SetContext(A))
#define Options_SetNbOfThreads(OPT,A)            ((OPT)->SetNbOfThreads(A))


#define Options_SetDefault(OPT)                  ((OPT)->SetDefault())
#define Options_Initialize(OPT)                  ((OPT)->Initialize())
#define Options_GetFillFactor(OPT)               ((OPT)->GetFillFactor())
#define Options_GetNbOfSolutions(OPT)            ((OPT)->GetNbOfSolutions())
#define Options_GetNbOfSequences(OPT)            ((OPT)->GetNbOfSequences())

#define Options_Set(OPT,...)                     ((OPT)->Set(__VA_ARGS__))



#define Options_GetDebug(OPT) Options_GetPrintedInfos(OPT)
        

        
#define Options_IsToPrintOutAtEachIteration(OPT) \
        ((OPT) ? !strcmp(Options_GetPrintLevel(OPT),"2") : 0)


/* Resolution method */
#define Options_ResolutionMethodIs(OPT,...) \
        String_Is((OPT) ? Options_GetResolutionMethod(OPT) : NULL,__VA_ARGS__)



/* Module */
#define Options_ModuleIs(OPT,M) \
        String_Is((OPT) ? Options_GetModule(OPT) : NULL,M)


/* Crout method */
#define Options_ResolutionMethodIsCrout(OPT) \
        Options_ResolutionMethodIs(OPT,"crout")


/* SuperLU method */
#define Options_ResolutionMethodIsSuperLU(OPT) \
        (Options_ResolutionMethodIs(OPT,"slu") || \
        (Options_ResolutionMethodIs(OPT,"superlu") && \
        !Options_ResolutionMethodIs(OPT,"superlumt") && \
        !Options_ResolutionMethodIs(OPT,"superludist")))


/* SuperLUMT method */
#define Options_ResolutionMethodIsSuperLUMT(OPT) \
        Options_ResolutionMethodIs(OPT,"superlumt")


/* SuperLUDist method */
#define Options_ResolutionMethodIsSuperLUDist(OPT) \
        Options_ResolutionMethodIs(OPT,"superludist")


/* MA38 method */
#define Options_ResolutionMethodIsMA38(OPT) \
        Options_ResolutionMethodIs(OPT,"ma38")


/* PetscKSP method */
#define Options_ResolutionMethodIsPetscKSP(OPT) \
        Options_ResolutionMethodIs(OPT,"petscksp")
    

/* Fill factor */
#define Options_DefaultFillFactor(OPT) \
        (Options_ResolutionMethodIsMA38(OPT) ? 2 : 0)

#if 0
#define Options_GetFillFactor(OPT) \
        ((Options_GetContext(OPT) && \
        String_Is(((char**) Context_GetSolver(Options_GetContext(OPT)))[2],"-ff") && \
        strtoul(((char**) Context_GetSolver(Options_GetContext(OPT)))[3],NULL,10)) ? \
        strtoul(((char**) Context_GetSolver(Options_GetContext(OPT)))[3],NULL,10) : Options_DefaultFillFactor(OPT))
        
        
/* Number of solutions for the monolithic approach */
#define Options_GetNbOfSolutions(OPT) \
        ((Options_GetContext(OPT) && \
        Options_ModuleIs(OPT,"Monolithic") && \
        Context_IsUseModule(Options_GetContext(OPT))) ? \
        atoi(((char**) Context_GetUseModule(Options_GetContext(OPT)))[2]) : 2)


/* Number of sequences for the sequential non iterative approach */
#define Options_GetNbOfSequences(OPT) \
        ((Options_GetContext(OPT) && \
        Options_ModuleIs(OPT,"SNIA") && \
        Context_IsUseModule(Options_GetContext(OPT))) ? \
        atoi(((char**) Context_GetUseModule(Options_GetContext(OPT)))[2]) : 1)
#endif


/* Number of threads for multi-threaded calculations */
#define Options_NbOfThreads(OPT) \
        atoi(Options_GetNbOfThreads(OPT))


#include <string>

struct Options_t {
  private:
  char*   _debug ;             /* what to be printed */
  char*   _method ;            /* resolution method */
  /* Print level
   *   0:  minimal, 
   *   1:  normal at each step , 
   *   2:  normal at each iteration, 
   *   3-: not used) */
  char*   _level ;             /* print level */
  char*   _module ;            /* module */
  char*   _graph ;             /* Graph method */
  char*   _eordering ;         /* Element ordering method */
  char*   _nordering ;         /* Nodal ordering method */
  char*   _postprocess ;       /* Post-processing method */
  char*   _nthreads ;          /* Nb of requested threads */
  Context_t* _context ;

  public:
  /* The getters */
  char*   GetPrintedInfos(){return _debug ;}
  char*   GetResolutionMethod(){return _method ;}
  char*   GetPrintLevel(){return _level ;}
  char*   GetModule(){return _module ;}
  char*   GetGraphMethod(){return _graph ;}
  char*   GetElementOrderingMethod(){return _eordering ;}
  char*   GetNodalOrderingMethod(){return _nordering ;}
  char*   GetPostProcessingMethod(){return _postprocess ;}
  char*   GetNbOfThreads(){return _nthreads ;}
  Context_t* GetContext(){return _context ;}

  /* The setters */
  void SetPrintedInfos(char* a){_debug = a;}
  void SetResolutionMethod(char* a){_method = a;}
  void SetPrintLevel(char* a){_level = a;}
  void SetModule(char* a){_module = a;}
  void SetGraphMethod(char* a){_graph = a;}
  void SetElementOrderingMethod(char* a){_eordering = a;}
  void SetNodalOrderingMethod(char* a){_nordering = a;}
  void SetPostProcessingMethod(char* a){_postprocess = a;}
  void SetNbOfThreads(char* a){_nthreads = a;}
  void SetContext(Context_t* a){_context = a;}

  void SetDefault(void);
  void Initialize(void);

  void Set(char const*);

  #if 0
  void Set(std::string const& type,std::string const& value) {
    Set(type.c_str(),value.c_str());
  }
  void Set(char const* type,char const* value) {
    if(strcmp(type,"printlevel") == 0) {
      strcpy(GetPrintLevel(),value) ;
    }
    if(strcmp(type,"debug") == 0) {
      strcpy(GetPrintedInfos(),value) ;
    }
    if(strcmp(type,"solver") == 0) {
      strcpy(GetResolutionMethod(),value) ;
    }
    if(strcmp(type,"module") == 0) {
      strcpy(GetModule(),value) ;
    }
    if(strcmp(type,"nbofthreads") == 0) {
      strcpy(GetNbOfThreads(),value) ;
    }
    if(strcmp(type,"graphmethod") == 0) {
      strcpy(GetGraphMethod(),value);
    }
    if(strcmp(type,"eordering") == 0) {
      strcpy(GetElementOrderingMethod(),value);
    }
    if(strcmp(type,"nordering") == 0) {
      strcpy(GetNodalOrderingMethod(),value);
    }
    if(strcmp(type,"postproc") == 0) {
      strcpy(GetPostProcessingMethod(),value);
    }
  }
  #endif

  size_t GetFillFactor();
  size_t GetNbOfSolutions();
  size_t GetNbOfSequences();
} ;


#include "Module.h"
#include "Context.h"
#include "String_.h"

  inline void Options_t::SetDefault(void){
    const char*  modulenames[] = {Module_ListOfNames} ;
    const char*  defaultmodule = modulenames[0] ;
  
    strcpy(GetPrintedInfos(),"\0") ;
    strcpy(GetResolutionMethod(),"crout") ;
    strcpy(GetPrintLevel(),"1") ;
    strcpy(GetModule(),defaultmodule) ;
    strcpy(GetNbOfThreads(),"1") ;
    strcpy(GetGraphMethod(),"\0");
    strcpy(GetElementOrderingMethod(),"\0");
    strcpy(GetNodalOrderingMethod(),"\0");
    strcpy(GetPostProcessingMethod(),"\0");
  }

  inline void Options_t::Initialize(void){
    Context_t* ctx = GetContext() ;
    size_t len = Options_MaxLengthOfKeyWord-1;

    if(!ctx) return;
  
    if(Context_GetSolver(ctx)) {
      char* a = ((char**) Context_GetSolver(ctx))[1] ;
    
      strncpy(GetResolutionMethod(),a,len);
    }
  
    if(Context_GetDebug(ctx)) {
      char* a = ((char**) Context_GetDebug(ctx))[1] ;
    
      strncpy(GetPrintedInfos(),a,len);
    }
  
    if(Context_GetGraph(ctx)) {
      char* a = ((char**) Context_GetGraph(ctx))[1] ;

      strncpy(GetGraphMethod(),a,len);
    }
  
    if(Context_GetElementOrdering(ctx)) {
      char* a = ((char**) Context_GetElementOrdering(ctx))[1] ;

      strncpy(GetElementOrderingMethod(),a,len);
    }
  
    if(Context_GetNodalOrdering(ctx)) {
      char* a = ((char**) Context_GetNodalOrdering(ctx))[1] ;

      strncpy(GetNodalOrderingMethod(),a,len);
    }
  
    if(Context_GetPrintLevel(ctx)) {
      char* a = ((char**) Context_GetPrintLevel(ctx))[1] ;
    
      strncpy(GetPrintLevel(),a,len);
    }
  
    if(Context_GetNbOfThreads(ctx)) {
      char* a = ((char**) Context_GetNbOfThreads(ctx))[1] ;
    
      strncpy(GetNbOfThreads(),a,len);
    }
  
    if(Context_GetUseModule(ctx)) {
      char* a = ((char**) Context_GetUseModule(ctx))[1] ;
    
      strncpy(GetModule(),a,len);
    }
  
    if(Context_GetPostProcessing(ctx)) {
      char* a = ((char**) Context_GetPostProcessing(ctx))[1] ;

      strncpy(GetPostProcessingMethod(),a,len);
    }
  }

  inline size_t Options_t::GetFillFactor() {
    Context_t* ctx = GetContext();
    size_t ff = Options_ResolutionMethodIsMA38(this) ? 2 : 0;

    if(ctx) {
      char** s = Context_GetSolver(ctx);

      if(s && String_Is(s[2],"-ff")) {
        size_t ff1 = strtoul(s[3],NULL,10);

        if(ff1) ff = ff1;
      }
    }

    return(ff);
  }

  inline size_t Options_t::GetNbOfSolutions(){
    Context_t* ctx = GetContext();
    size_t n = 2;

    if(Options_ModuleIs(this,"Monolithic") && ctx) {
      if(Context_IsUseModule(ctx)) {
        char** s = (char**) Context_GetUseModule(ctx);

        n = strtoul(s[2],NULL,10);
      }
    }

    return(n);
  }

  inline size_t Options_t::GetNbOfSequences() {
    Context_t* ctx = GetContext();
    size_t n = 1;

    if(Options_ModuleIs(this,"SNIA") && ctx) {
      if(Context_IsUseModule(ctx)) {
        char** s = (char**) Context_GetUseModule(ctx);

         n = strtoul(s[2],NULL,10);
      }
    }

    return(n);
  }

  inline void Options_t::Set(char const* args) {
    Context_t* ctx = GetContext();

    Context_Set(ctx,args);
    Initialize();
  }



#ifdef __CPLUSPLUS
}
#endif

/* For the macros */
#include <string.h>
#include <stdlib.h>
#include "String_.h"
#include "Context.h"
#endif
