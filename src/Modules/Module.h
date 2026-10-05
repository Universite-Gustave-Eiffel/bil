#ifndef MODULE_H
#define MODULE_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Module_t;
struct DataSet_t;
struct OutputFiles_t;
struct Solutions_t;
struct Solver_t;

/*  Typedef names of Methods */
using Module_SetModuleProp_t = int (Module_t*);
using Module_ComputeProblem_t = int (DataSet_t*);
using Module_SolveProblem_t = int (DataSet_t*,Solutions_t*,Solver_t*,OutputFiles_t*);
using Module_Increment_t = int (DataSet_t*,Solutions_t*,Solver_t*,OutputFiles_t*,double,double);
using Module_InitializeProblem_t = int (DataSet_t*,Solutions_t*);


#define Module_MaxLengthOfKeyWord        (30)
#define Module_MaxLengthOfFileName       (60)
#define Module_MaxLengthOfShortTitle     (80)
#define Module_MaxLengthOfAuthorNames    (80)

#include "ListOfModules.h"

#define Module_NbOfListedModules         (ListOfModules_Nb)
#define Module_ListOfNames               ListOfModules_Names
#define Module_ListOfSetModuleProp       ListOfModules_Methods(_SetModuleProp)


extern Module_t*  (Module_New)(void) ;
extern void       (Module_Delete)(void*) ;
extern  Module_SetModuleProp_t  Module_ListOfSetModuleProp ;


#define Module_GetCodeNameOfModule(MOD)   ((MOD)->GetCodeNameOfModule())
#define Module_GetShortTitle(MOD)         ((MOD)->GetShortTitle())
#define Module_GetNameOfAuthors(MOD)      ((MOD)->GetNameOfAuthors())
#define Module_GetSetModuleProp(MOD)      ((MOD)->GetSetModuleProp())
#define Module_GetComputeProblem(MOD)     ((MOD)->GetComputeProblem())
#define Module_GetSolveProblem(MOD)       ((MOD)->GetSolveProblem())
#define Module_GetIncrement(MOD)          ((MOD)->GetIncrement())
#define Module_GetInitializeProblem(MOD)  ((MOD)->GetInitializeProblem())
#define Module_GetSequentialIndex(MOD)    ((MOD)->GetSequentialIndex())
#define Module_GetNbOfSequences(MOD)      ((MOD)->GetNbOfSequences())

#define Module_SetCodeNameOfModule(MOD,A)  (MOD)->SetCodeNameOfModule(A)
#define Module_SetShortTitle(MOD,A)        (MOD)->SetShortTitle(A)
#define Module_SetNameOfAuthors(MOD,A)     (MOD)->SetNameOfAuthors(A)
#define Module_SetSetModuleProp(MOD,A)     (MOD)->SetSetModuleProp(A)
#define Module_SetComputeProblem(MOD,A)    (MOD)->SetComputeProblem(A)
#define Module_SetSolveProblem(MOD,A)      (MOD)->SetSolveProblem(A)
#define Module_SetIncrement(MOD,A)         (MOD)->SetIncrement(A)
#define Module_SetInitializeProblem(MOD,A) (MOD)->SetInitializeProblem(A)
#define Module_SetSequentialIndex(MOD,A)   (MOD)->SetSequentialIndex(A)
#define Module_SetNbOfSequences(MOD,A)     (MOD)->SetNbOfSequences(A)


#define Module_Set(MOD,...)                (MOD)->Set(__VA_ARGS__)        


/* Copy operations */
#define Module_CopyCodeNameOfModule(MOD,codename) \
        (strcpy(Module_GetCodeNameOfModule(MOD),codename))

#define Module_CopyShortTitle(MOD,title) \
        (strcpy(Module_GetShortTitle(MOD),title))

#define Module_CopyNameOfAuthors(MOD,authors) \
        (strcpy(Module_GetNameOfAuthors(MOD),authors))


/* Short hands */
#define Module_SetModuleProp(MOD)         (MOD)->SetModuleProp()
#define Module_ComputeProblem(MOD,...)    (MOD)->ComputeProblem(__VA_ARGS__)
#define Module_SolveProblem(MOD,...)      (MOD)->SolveProblem(__VA_ARGS__)
#define Module_Increment(MOD,...)         (MOD)->Increment(__VA_ARGS__)
#define Module_InitializeProblem(MOD,...) (MOD)->InitializeProblem(__VA_ARGS__)


#include <stdexcept>

struct Module_t {              /* module */
  private:
  Module_SetModuleProp_t*      _setmoduleprop ;
  Module_ComputeProblem_t*     _computeproblem ;
  Module_SolveProblem_t*       _solveproblem ;
  Module_Increment_t*          _increment ;
  Module_InitializeProblem_t*  _initializeproblem ;
  int _nbofsequences ;
  int _sequentialindex ;
  
  char*  _codename ;          /* Code name of the module */
  char*  _authors ;           /* Authors of this module */
  char*  _shorttitle ;        /* Short title of the module */

  public:
  /* The getters */
  Module_SetModuleProp_t* GetSetModuleProp(){return _setmoduleprop;}
  Module_ComputeProblem_t* GetComputeProblem(){return _computeproblem;}
  Module_SolveProblem_t* GetSolveProblem(){return _solveproblem;}
  Module_Increment_t* GetIncrement(){return _increment;}
  Module_InitializeProblem_t* GetInitializeProblem(){return _initializeproblem;}
  char* GetCodeNameOfModule(){return _codename;}
  char* GetShortTitle(){return _shorttitle;}
  char* GetNameOfAuthors(){return _authors;}
  int   GetSequentialIndex(){return _sequentialindex;}
  int   GetNbOfSequences(){return _nbofsequences;}

  /* The setters */
  void SetSetModuleProp(Module_SetModuleProp_t* A){_setmoduleprop = A;}
  void SetComputeProblem(Module_ComputeProblem_t* A){_computeproblem = A;}
  void SetSolveProblem(Module_SolveProblem_t* A){_solveproblem = A;}
  void SetIncrement(Module_Increment_t* A){_increment = A;}
  void SetInitializeProblem(Module_InitializeProblem_t* A){_initializeproblem = A;}
  void SetCodeNameOfModule(char* A){_codename = A;}
  void SetShortTitle(char* A){_shorttitle = A;}
  void SetNameOfAuthors(char* A){_authors = A;}
  void SetSequentialIndex(int const& A){_sequentialindex = A;}
  void SetNbOfSequences(int const& A){_nbofsequences = A;}

  void Set(std::string const& codestr){Set(codestr.c_str());}
  void Set(const char* codename){
    size_t NbOfModules = Module_NbOfListedModules ;
    const char* modulenames[] = {Module_ListOfNames} ;
    Module_SetModuleProp_t* xModule_SetModuleProp[] = {Module_ListOfSetModuleProp} ;
    size_t i = 0 ;
  
    while(i < NbOfModules && strcmp(modulenames[i],codename)) i++ ;
    
    if(i < NbOfModules) {
      strcpy(GetCodeNameOfModule(),modulenames[i]);
      SetSetModuleProp(xModule_SetModuleProp[i]) ;
      /* Call to SetModuleProp */
      if(SetModuleProp() < 0) {
        throw std::runtime_error("Module_t::Set: something wrong in SetModuleProp");
      }
    } else {
      printf("No module named %s",codename) ;
      throw std::runtime_error("Module_t::Set: no module found");
    }
    
    /* by default */
    SetNbOfSequences(1);
  }

  /* The methods */
  int SetModuleProp(){
    return((_setmoduleprop) ? _setmoduleprop(this) : -1);
  }

  int ComputeProblem(DataSet_t* dataset) {
    return((_computeproblem) ? _computeproblem(dataset) : -1);
  }

  int SolveProblem(DataSet_t* dataset,Solutions_t* solutions,Solver_t* solver,OutputFiles_t* outputfiles) {
    return((_solveproblem) ? _solveproblem(dataset,solutions,solver,outputfiles) : -1);
  }

  int Increment(DataSet_t* dataset,Solutions_t* solutions,Solver_t* solver,OutputFiles_t* outputfiles,double t1,double t2) {
    return((_increment) ? _increment(dataset,solutions,solver,outputfiles,t1,t2) : -1);
  }

  int InitializeProblem(DataSet_t* dataset,Solutions_t* solutions) {
    return((_initializeproblem) ? _initializeproblem(dataset,solutions) : -1);
  }
} ;







#ifdef __CPLUSPLUS
}
#endif
#endif
