#ifndef MRY_H
#define MRY_H



#define Mry_New(...) \
        Utils_CAT_NARG(Mry_New,__VA_ARGS__)(__VA_ARGS__)


/* Mry_New(T[N]) is also valid */
#define Mry_New1(T) \
        Mry_AllocateZeroed(sizeof(T),1)

#define Mry_New2(T,N) \
        Mry_AllocateZeroed(sizeof(T),(size_t) (N))


/* Using a placement new */
#if 0
#define Mry_New1(T) \
        ({\
          T* Mry_v = Mry_AllocateZeroed(sizeof(T),1);\
          new (&Mry_v[0]) T() ; \
          Mry_v;\
        })

#define Mry_New2(T,N) \
        ({\
          T* Mry_v = Mry_AllocateZeroed(sizeof(T),(size_t) (N));\
          do { \
            for(size_t Mry_i = 0 ; Mry_i < N ; Mry_i++) { \
              new (&Mry_v[Mry_i]) T() ; \
            } \
          } while(0); \
          Mry_v;\
        })
#endif


/* We use a C extension provided by GNU C:
 * A compound statement enclosed in parentheses may appear 
 * as an expression in GNU C.
 * (https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html#Statement-Exprs) */
#define Mry_Create(T,N,CREATE) \
        ({ \
          T* Mry_v = static_cast<T*>(Mry_New(T,N)) ; \
          do { \
            for(size_t Mry_i = 0 ; Mry_i < N ; Mry_i++) { \
              T* Mry_v0 = CREATE ; \
              std::memcpy(&Mry_v[Mry_i],Mry_v0,sizeof(T)); \
              Mry_Free(Mry_v0) ; \
            } \
          } while(0); \
          Mry_v ; \
        })

            /*for(std::remove_const_t<decltype(N)> Mry_i = 0 ; Mry_i < N ; Mry_i++) { \*/

#define Mry_Delete(OBJ,N,DELETE) \
        do { \
          if(OBJ) { \
            for(size_t Mry_i = 0 ; Mry_i < N ; Mry_i++) { \
              DELETE(OBJ + Mry_i) ; \
            } \
          } \
        } while(0)


            /*for(std::remove_const_t<decltype(N)> Mry_i = 0 ; Mry_i < N ; Mry_i++) { \*/
        


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cstring>


#if 0
inline void* Mry_Allocate(size_t size)
{
  void* ptr ;

  if(!size) return (NULL);
  
  ptr = malloc(size) ;
  
  assert(ptr) ;
  
  return(ptr);
}

inline void* Mry_AllocateZeroed(size_t size,size_t num = 1)
{
  void* ptr;

  if(!size) return (NULL);
  
  ptr = calloc(num, size);
  
  assert(ptr) ;
  
  return(ptr);
}

inline void* Mry_Realloc(void* old_ptr,size_t old_size,size_t new_size)
{
  void* new_ptr;

  if(!new_size) return (NULL);
  
  new_ptr = realloc(old_ptr,new_size);
  
  assert(new_ptr) ;
  
  return(new_ptr);
}

inline void Mry_Free(void* ptr)
{
  if(ptr) free(ptr);
}
#else

#include <algorithm>
#include <new>

inline void* Mry_Allocate(size_t size)
{
  void* ptr ;

  if(!size) return (NULL);
  
  ptr = ::operator new(size,std::nothrow);
  
  assert(ptr) ;
  
  return(ptr);
}

inline void* Mry_AllocateZeroed(size_t size,size_t num = 1)
{
  void* ptr ;

  if(!size) return (NULL);
  
  ptr = ::operator new(num*size,std::nothrow);
  
  assert(ptr) ;

  if(ptr) {
    std::memset(ptr,0,num*size);
  }
  
  return(ptr);
}


#if 0
template<typename T,typename... Args>
inline T* Mry_Create_(size_t N, T* (*CREATE)(Args...),Args&&... args)
{
  T* v = static_cast<T*>(Mry_AllocateZeroed(sizeof(T),N));

  for(size_t i = 0 ; i < N ; i++) {
    T* v0 = CREATE(std::forward<Args>(args)...);

    v[i] = v0[0] ;
    Mry_Free(v0) ;
  }

  return(v);
}
#endif


inline void* Mry_Realloc(void* old_ptr,size_t old_size,size_t new_size)
{
  void* new_ptr;

  if(!new_size) return (NULL);
  
  new_ptr = ::operator new(new_size);

  std::memcpy(new_ptr,old_ptr,std::min(old_size,new_size));

  ::operator delete(old_ptr);

  return(new_ptr);
}


inline void Mry_Free(void* ptr)
{
  if(ptr) ::operator delete(ptr);
}
#endif


/* For the macros */
#include "Utils.h"
#include <type_traits>

#endif
