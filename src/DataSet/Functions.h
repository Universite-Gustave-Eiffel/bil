#ifndef FUNCTIONS_H
#define FUNCTIONS_H


/* Forward declarations */
struct Functions_t; //typedef struct Functions_t    Functions_t ;
struct Function_t;
struct DataFile_t;


extern Functions_t* (Functions_New)     (void) ;
extern Functions_t* (Functions_Create)  (DataFile_t*) ;
extern void         (Functions_Scan)(Functions_t*,DataFile_t*);
extern void         (Functions_Delete)  (void*) ;


#define Functions_MaxNbOfFunctions             (100)


#define Functions_GetNbOfFunctions(FCTS)      ((FCTS)->GetNbOfFunctions())
#define Functions_GetFunction(FCTS)           ((FCTS)->GetFunction())
#define Functions_GetCapacity(FCTS)           ((FCTS)->GetCapacity())


#define Functions_SetNbOfFunctions(FCTS,A)      ((FCTS)->SetNbOfFunctions(A))
#define Functions_SetFunction(FCTS,A)           ((FCTS)->SetFunction(A))

#define Functions_EmplaceBack(FCTS,...)         ((FCTS)->EmplaceBack(__VA_ARGS__))

#define Functions_Print(FCTS)                   ((FCTS)->Print())


#include <stdexcept>

struct Functions_t {
  private:
  size_t _n_fn ;
  Function_t* _fn ;

  public:
  size_t GetCapacity() {return Functions_MaxNbOfFunctions;}
  template<typename... Args>
  void EmplaceBack(const std::string&,Args&&...);

  public:
  size_t GetNbOfFunctions() const {return _n_fn;}
  Function_t* GetFunction() {return _fn;}
  void SetNbOfFunctions(size_t n) {
    if(n >= GetCapacity()) {
      throw std::length_error("Maximum number of functions reached");
    }
    _n_fn = n;
  }
  void SetFunction(Function_t* fn) {_fn = fn;}
  void Print();
} ;


#include <utility>
#include "Function.h"

  template<typename... Args>
  inline void Functions_t::EmplaceBack(const std::string& type,Args&&... args) {
    Function_t* function = _fn + _n_fn;
    
    if(_n_fn >= GetCapacity()) {
      throw std::length_error("Maximum number of functions reached");
    }

    Function_Set(function,type,std::forward<Args>(args)...);
    _n_fn++;
  }

  inline void Functions_t::Print() {
    #define PRINT(...)  fprintf(stdout,__VA_ARGS__)
    PRINT("Time functions:\n") ;
    PRINT("\t Nb of functions = %lu\n",_n_fn) ;
    for(size_t i = 0 ; i < _n_fn ; i++) {
      PRINT("Time function(%lu):\n",i) ;
      (_fn+i)->Print();
    }
    #undef PRINT
  }

#endif
