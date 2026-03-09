#ifndef GENERICDATA_H
#define GENERICDATA_H


/* Forward declarations */
struct GenericData_t; //typedef struct GenericData_t     GenericData_t ;
struct TypeId_t;


#define GenericData_MaxLengthOfKeyWord           (50)



#define GenericData_GetTypeId(GD)                ((GD)->GetTypeId())
#define GenericData_GetName(GD)                  ((GD)->GetName())
#define GenericData_GetNbOfData(GD)              ((GD)->GetNbOfData())
//#define GenericData_GetData(GD)                  ((GD)->GetData())

#define GenericData_GetNextGenericData(GD)       ((GD)->GetNextGenericData())
#define GenericData_GetPreviousGenericData(GD)   ((GD)->GetPreviousGenericData())
#define GenericData_GetDelete(GD)                ((GD)->GetDelete())

#define GenericData_SetTypeId(GD,X)              ((GD)->SetTypeId(X))
#define GenericData_SetName(GD,X)                ((GD)->SetName(X))
#define GenericData_SetNbOfData(GD,X)            ((GD)->SetNbOfData(X))
//#define GenericData_SetData(GD,X)                ((GD)->SetData(X))

#define GenericData_SetNextGenericData(GD,X)     ((GD)->SetNextGenericData(X))
#define GenericData_SetPreviousGenericData(GD,X) ((GD)->SetPreviousGenericData(X))
#define GenericData_SetDelete(GD,X)              ((GD)->SetDelete(X))


#define GenericData_GetData(GD) \
        TypeId_GetData(GenericData_GetTypeId(GD))
        
#define GenericData_Set(GD,X) \
        TypeId_Set(GenericData_GetTypeId(GD),X)


#define GenericData_GetSize(GD) \
        TypeId_GetSize(GenericData_GetTypeId(GD))

#define GenericData_GetIdNumber(GD) \
        TypeId_GetIdNumber(GenericData_GetTypeId(GD))


        
        
#define GenericData_Is(GD,NAME) \
        (!strncmp(GenericData_GetName(GD),NAME,GenericData_MaxLengthOfKeyWord))
        
        
#define GenericData_FindData(GD,NAME) \
        GenericData_GetData_(GenericData_Find(GD,NAME))
        
        
#define GenericData_FindNbOfData(GD,NAME) \
        GenericData_GetNbOfData_(GenericData_Find(GD,NAME))
        
        
#define GenericData_Merge(A,B) \
        GenericData_Append(A,GenericData_First(B))





/* Implementation */
#define GenericData_GetNbOfData_(GD) \
        ((GD) ? GenericData_GetNbOfData(GD) : 0)

#define GenericData_GetData_(GD) \
        ((GD) ? GenericData_GetData(GD) : NULL)



/* Typedef names of methods */
//typedef void* GenericData_Allocate(int,TypedId_t) ;



#define GenericData_Initialize(GD, ...) \
        ((GD)->Initialize(__VA_ARGS__))
        
#define GenericData_Append(GD, ...) \
        ((GD) ? (GD)->Append(__VA_ARGS__) : __VA_ARGS__)
        
#define GenericData_First(GD) \
        ((GD)->First())
        
#define GenericData_Last(GD) \
        ((GD)->Last())
        
#define GenericData_Find(GD, ...) \
        ((GD)->Find(__VA_ARGS__))


#define GenericData_New    GenericData_t::New
#define GenericData_Create GenericData_t::Create
#define GenericData_Delete GenericData_t::Delete




#include "GenericObject.h"
#include <stdio.h>
#include <string.h>

/* Generic data */
struct GenericData_t {
  private:
  TypeId_t* _typ;                /* The type id of data */
  char* _name;                   /* Name of the data */
  size_t _n;                     /* Nb of data */
  //void* _data; /* The data */
  GenericData_t* _prev;          /* Previous generic data */
  GenericData_t* _next;          /* Next generic data */
  GenericObject_Delete_t* _delete;
  
  public:
  /* The getters */
  TypeId_t* GetTypeId(void){return(_typ);}
  char* GetName(void){return(_name);}
  size_t GetNbOfData(void){return(_n);}
  //auto GetData(void){return(_data);}
  GenericData_t* GetNextGenericData(void){return(_next);}
  GenericData_t* GetPreviousGenericData(void){return(_prev);}
  GenericObject_Delete_t* GetDelete(void){return(_delete);}
  
  /* The setters */
  void SetTypeId(TypeId_t* x){_typ = x;}
  void SetName(char* x){_name = x;}
  void SetNbOfData(size_t x){_n = x;}
  //template<typename T> void SetData(T x){_data = x;}
  void SetNextGenericData(GenericData_t* x){_next = x;}
  void SetPreviousGenericData(GenericData_t* x){_prev = x;}
  void SetDelete(void (*x)(void*)){_delete = x;}
  
  /* Other methods */
  private:
  GenericData_t* Last(void){
  /** Return the last generic data or NULL pointer. */
    GenericData_t* gdat = this;
    
    if(gdat) {
      GenericData_t* next;
    
      while((next = gdat->GetNextGenericData())) gdat = next;
    }
  
    return(gdat);
  }

  GenericData_t* First(void){
  /** Return the first generic data or NULL pointer. */
    GenericData_t* gdat = this;
    
    if(gdat) {
      GenericData_t* prev;
    
      while((prev = gdat->GetPreviousGenericData())) gdat = prev;
    }
  
    return(gdat) ;
  }

  public:
  GenericData_t* Append(GenericData_t* b){
  /** Append "b" to "this". 
   *  Return the appended generic data (ie "this" or "b") or NULL pointer. */
    /* Add b at the end of this */
    GenericData_t* lasta  = Last() ;
    
    /* The condition is useless since "lasta" must not be NULL.  */
    if(lasta) lasta->SetNextGenericData(b) ;
    if(b) b->SetPreviousGenericData(lasta) ;
    
    return(this) ;
  }

  GenericData_t* Find(const char* name){
  /** Return the generic data named as "name" or NULL pointer. */
    {
      GenericData_t* next = this ;
    
      while(next) {
        if(GenericData_Is(next,name)) return(next) ;
        next = next->GetNextGenericData() ;
      }
    }
  
    {
      GenericData_t* prev = this ;
    
      while(prev) {
        if(GenericData_Is(prev,name)) return(prev) ;
        prev = prev->GetPreviousGenericData() ;
      }
    }
  
    return(NULL) ;
  }
  #if 1
  template<typename T>
  void Initialize(size_t,T*,const char*);
  inline static GenericData_t* New(void);
  template<typename T>
  inline static GenericData_t* Create(size_t,T*,const char*);
  inline static void Delete(void*);
  #endif
} ;


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "TypeId.h"
#include "Mry.h"

#if 1
template<typename T>
void GenericData_t::Initialize(size_t n,T* data,const char* name){
    TypeId_Set(GetTypeId(),data);
    SetNbOfData(n);
    
    {
      #define GenericData_MIN(a,b)   (((a) < (b)) ? (a) : (b))
      size_t len = GenericData_MIN(strlen(name),GenericData_MaxLengthOfKeyWord) ;
      #undef GenericData_MIN
      
      strncpy(GetName(),name,len) ;
      GetName()[len] = '\0' ;
    }
}


GenericData_t* GenericData_t::New(void)
{
  GenericData_t* gdat = (GenericData_t*) Mry_New(GenericData_t) ;
  
  /* Allocation for the name */
  {
    char* name = (char*) Mry_New(char,GenericData_MaxLengthOfKeyWord + 1) ;
    
    gdat->SetName(name) ;
    strcpy(gdat->GetName(),"\0") ;
  }
  
  {
    TypeId_t* typ = TypeId_Create();
    
    gdat->SetTypeId(typ);
    gdat->SetNbOfData(0);
  }
  
  {
    gdat->SetNextGenericData(NULL);
    gdat->SetPreviousGenericData(NULL);
  }
  
  gdat->SetDelete(&GenericData_Delete);
  
  return(gdat) ;
}


template<typename T>
GenericData_t* GenericData_t::Create(size_t n,T* data,const char* name)
{
  GenericData_t* gdat = New();
  
  gdat->Initialize(n,data,name) ;
  
  return(gdat) ;
}


void GenericData_t::Delete(void* self)
{
  GenericData_t* gdat = (GenericData_t*) self;
  
  if(gdat) {
    {
      char* name = gdat->GetName();

      if(name) {
        Mry_Free(name);
        gdat->SetName(NULL);
      }
    }
    
    {
      TypeId_t* typ = gdat->GetTypeId();
    
      if(typ) {
        size_t n = gdat->GetNbOfData();
        void* data = TypeId_GetData(typ);

        if(data) {
          size_t sz = TypeId_GetSize(typ);
          
          for(size_t i = 0 ; i < n ; i++) {
            void* data_i = ((char*) data) + i*sz;
            
            TypeId_DeleteData(typ,data_i);
          }

          Mry_Free(data);
        }

        TypeId_Delete(typ); // Do nothing!
        Mry_Free(typ);
        
        gdat->SetTypeId(NULL);
      }
    }
  
    /* Delete also the linked generic data  */
    {
      GenericData_t*   prev = gdat->GetPreviousGenericData();
      GenericData_t*   next = gdat->GetNextGenericData();
      
      /* Connect prev and next */
      if(prev) prev->SetNextGenericData(next);
      if(next) next->SetPreviousGenericData(prev);
    
      if(prev) {
        Delete(prev);
        Mry_Free(prev);
        gdat->SetPreviousGenericData(NULL);
      }
  
      if(next) {
        Delete(next);
        Mry_Free(next);
        gdat->SetNextGenericData(NULL);
      }
    }
  }
}
#endif



#if 0
inline GenericData_t* (GenericData_New)       (void) ;
template<typename T>
inline GenericData_t* (GenericData_Create)    (size_t,T*,const char*) ;
template<typename T>
inline void           (GenericData_Initialize)(GenericData_t*,size_t,T*,const char*) ;
inline GenericData_t* (GenericData_Append)    (GenericData_t*,GenericData_t*) ;
inline GenericData_t* (GenericData_First)     (GenericData_t*) ;
inline GenericData_t* (GenericData_Last)      (GenericData_t*) ;
inline GenericData_t* (GenericData_Find)      (GenericData_t*,const char*) ;

//inline void           (GenericData_DeleteData)   (TypeId_t*,void*) ;
inline void           (GenericData_Delete)       (void*) ;


#include "GenericData.h.in"
#endif

/* For the macros */
#include "TypeId.h"

#endif
