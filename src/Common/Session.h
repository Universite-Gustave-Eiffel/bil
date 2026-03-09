#ifndef SESSION_H
#define SESSION_H


/* Forward declarations */
struct GenericData_t;


#define Session_GetCurrentInstance \
        Session_t::GetCurrentInstance

#define Session_Open \
        Session_t::Open

#define Session_Close \
        Session_t::Close
        
#define Session_FindGenericData(T,N) \
        Session_t::FindGenericData(N)
        
#define Session_AddGenericData \
        Session_t::AddGenericData




#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Mry.h"

struct Session_t {
  private:
  int            _index ;
  GenericData_t* _gdat ;
  Session_t*     _prev ;
  inline static Session_t* _cursession = NULL;
  
  private:
  /* The getters */
  int GetIndex(void){return _index;}
  GenericData_t* GetGenericData(void){return _gdat;}
  Session_t* GetPreviousSession(void){return _prev;}
  static Session_t* (GetCurrentInstance)(void){
    assert(_cursession);
    return(_cursession);
  }
  /* The setters */
  void SetIndex(int x){_index = x;}
  void SetGenericData(GenericData_t* x){_gdat = x;}
  void SetPreviousSession(Session_t* x){_prev = x;}
  
  /* Other functions */
  #if 0
  void (Delete)(void) {
    while(_cursession) Close();
  }
  #endif
  
  public:
  static Session_t* Open(void) {
    Session_t* prev  = _cursession;
  
    _cursession = (Session_t*) Mry_New(Session_t);
  
    _cursession->SetPreviousSession(prev);
    
    #if 0
    if(prev) {
      _cursession->SetIndex(prev->GetIndex() + 1);
    } else {
      _cursession->SetIndex(1);
    }
    #endif
  
    return(_cursession);
  }

  inline static Session_t* Close(void);
  inline static GenericData_t* FindGenericData(char const*);
  inline static void AddGenericData(GenericData_t*);
};


  
#include "GenericData.h"
#include "Mry.h"


Session_t* Session_t::Close(void){
  GenericData_t* gdat = _cursession->GetGenericData();

  if(gdat) {
    GenericData_Delete(gdat);
    Mry_Free(gdat);
    _cursession->SetGenericData(NULL);
  }

  {
    Session_t* garbage = _cursession;
    
    _cursession = garbage->GetPreviousSession();

    Mry_Free(garbage);
  }

  return(_cursession) ;
}

GenericData_t* Session_t::FindGenericData(char const* n){
  GenericData_t* gdat = _cursession->GetGenericData();
    
  return(GenericData_Find(gdat,n));
}
  
void Session_t::AddGenericData(GenericData_t* gnew){
  GenericData_t* gdat = _cursession->GetGenericData();

  _cursession->SetGenericData(GenericData_Append(gdat,gnew));
}

#endif
