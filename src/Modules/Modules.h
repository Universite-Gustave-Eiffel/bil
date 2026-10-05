#ifndef MODULES_H
#define MODULES_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Modules_t;
struct Module_t;

#define Modules_MaxNbOfModules               (3)

extern Modules_t* (Modules_New)(void) ;
extern void       (Modules_Delete)(void*) ;
extern void       (Modules_PrintAll)(char*) ;


#define Modules_GetNbOfModules(MODS)    ((MODS)->GetNbOfModules())
#define Modules_GetModule(MODS)         ((MODS)->GetModule())

#define Modules_SetNbOfModules(MODS,A)  ((MODS)->SetNbOfModules(A))
#define Modules_SetModule(MODS,A)       ((MODS)->SetModule(A))

#define Modules_FindModule(MODS,...)    ((MODS)->FindModule(__VA_ARGS__))


struct Modules_t {
  private:
  size_t _n_modules ;
  Module_t* _module ;

  public:
  size_t GetCapacity(){return Modules_MaxNbOfModules;}
  /* The getters */
  size_t GetNbOfModules(){return _n_modules ;}
  Module_t* GetModule(){return _module ;}
  /* The setters */
  void SetNbOfModules(size_t const& a){_n_modules = a;}
  void SetModule(Module_t* a){_module = a;}

  Module_t* FindModule(const char* codename);
} ;

#include "Module.h"

  inline Module_t* Modules_t::FindModule(const char* codename){
    Module_t*  module = GetModule() ;
    size_t n_modules = GetNbOfModules() ;
    size_t j = 0 ;
  
    while(j < n_modules && strcmp(Module_GetCodeNameOfModule(module + j),codename)) j++ ;
  
    if(j < n_modules) {
      Module_t* module_j = module + j ;
      return(module_j) ;
    }

    return(NULL) ;
  }


#ifdef __CPLUSPLUS
}
#endif
#endif
