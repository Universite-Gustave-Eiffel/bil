#ifndef MRY_H
#define MRY_H



#define Mry_New(...) \
        Utils_CAT_NARG(Mry_New,__VA_ARGS__)(__VA_ARGS__)


/* Mry_New(T[N]) is also valid */
#define Mry_New1(T) \
        Mry_AllocateZeroed(1,sizeof(T))

#define Mry_New2(T,N) \
        Mry_AllocateZeroed((size_t) (N),sizeof(T))


/* We use a C extension provided by GNU C:
 * A compound statement enclosed in parentheses may appear 
 * as an expression in GNU C.
 * (https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html#Statement-Exprs) */
#define Mry_Create(T,N,CREATE) \
        ({ \
          T* Mry_v = (T*) Mry_New(T,N) ; \
          do { \
            for(std::remove_const_t<decltype(N)> Mry_i = 0 ; Mry_i < N ; Mry_i++) { \
              T* Mry_v0 = CREATE ; \
              Mry_v[Mry_i] = Mry_v0[0] ; \
              Mry_Free(Mry_v0) ; \
            } \
          } while(0); \
          Mry_v ; \
        })


#define Mry_Delete(OBJ,N,DELETE) \
        do { \
          if(OBJ) { \
            for(std::remove_const_t<decltype(N)> Mry_i = 0 ; Mry_i < N ; Mry_i++) { \
              DELETE(OBJ + Mry_i) ; \
            } \
          } \
        } while(0)
        


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define _INLINE_ inline

_INLINE_ void* Mry_Allocate(size_t size)
{
  void* ptr ;

  if(!size) return (NULL);
  
  ptr = malloc(size) ;
  
  assert(ptr) ;
  
  return (ptr);
}



_INLINE_ void* Mry_AllocateZeroed(size_t num, size_t size)
{
  void* ptr;

  if(!size) return (NULL);
  
  ptr = calloc(num, size);
  
  assert(ptr) ;
  
  return (ptr);
}



_INLINE_ void* Mry_Realloc(void* ptr, size_t size)
{
  if(!size) return (NULL);
  
  ptr = realloc(ptr, size);
  
  assert(ptr) ;
  
  return (ptr);
}



_INLINE_ void Mry_Free(void* ptr)
{
  if(ptr == NULL) return;
  free(ptr);
}

#undef _INLINE_


/* For the macros */
#include "Utils.h"
#include <type_traits>

#endif
