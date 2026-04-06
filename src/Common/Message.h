#ifndef MESSAGE_H
#define MESSAGE_H


/* Forward declarations */
struct Message_t;

#define Message_Create          Message_t::Create
#define Message_Delete          Message_t::Delete
#define Message_GetInstance     Message_t::GetInstance
#define Message_FatalError0     Message_t::FatalError

#define Message_RuntimeError0 \
        Message_GetInstance()->RuntimeError
        
#define Message_InputError \
        Message_GetInstance()->InputError
        
#define Message_Warning \
        Message_GetInstance()->Warning
        
#define Message_Info \
        Message_GetInstance()->Info
        
#define Message_Direct \
        Message_GetInstance()->Direct

#define Message_LaunchDate \
        Message_GetInstance()->GetLaunchDate
        
#define Message_CPUTime \
        Message_GetInstance()->CPUTime
        
#define Message_CPUTimeInterval \
        Message_GetInstance()->CPUTimeInterval
        
#define Message_SetNewVerbosity \
        Message_GetInstance()->SetNewVerbosity


#define Message_GetLaunchClock(MSG) \
        ((MSG)->GetLaunchClock())
#define Message_GetLaunchTime(MSG) \
        ((MSG)->GetLaunchTime())
#define Message_GetLaunchDate(MSG) \
        ((MSG)->GetLaunchDate())
#define Message_GetVerbosity(MSG) \
        ((MSG)->GetVerbosity())
#define Message_GetSavedClock(MSG) \
        ((MSG)->GetSavedClock())
        
#define Message_SetLaunchClock(MSG,A) \
        ((MSG)->SetLaunchClock(A))
#define Message_SetLaunchTime(MSG,A) \
        ((MSG)->SetLaunchTime(A))
#define Message_SetLaunchDate(MSG,A) \
        ((MSG)->SetLaunchDate(A))
#define Message_SetVerbosity(MSG,A) \
        ((MSG)->SetVerbosity(A))
#define Message_SetSavedClock(MSG,A) \
        ((MSG)->SetSavedClock(A))



#define Message_PrintSourceLocation \
        fprintf(stdout,"\nAt %s, line %d",__FILE__,__LINE__)

#define Message_RuntimeError(...) \
        do { Message_PrintSourceLocation; \
        Message_RuntimeError0(__VA_ARGS__); } while(0)

#define Message_FatalError(...) \
        do { Message_PrintSourceLocation; \
        Message_FatalError0(__VA_ARGS__); } while(0)

/* Old notations which should be eliminated */
#define arret          Message_FatalError
                                           
#define Message_Exit \
        (exit(EXIT_SUCCESS))


#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#include "Mry.h"
#include "DistributedMS.h"

struct Message_t {
  private:
  clock_t  _launchclock;      /* Start up processor clock time */
  clock_t  _savedclock;       /* The last saved processor clock time */
  time_t*  _launchtime;       /* Start up time */
  char*    _launchdate;       /* Start up date */
  /* Verbosity level
   *   0:  silent except fatal error, 
   *   1:  +errors , 
   *   2:  +warnings, 
   *   3:  +infos ,
   *   4:  normal, 
   *   99: debug) */
  int  _verbosity;
  //GenericObject_Delete_t* _delete;
  //void (*_delete)(void*);
  
  public:
  /* The getters */
  clock_t GetLaunchClock(void){return(_launchclock);}
  time_t* GetLaunchTime(void){return(_launchtime);}
  char* GetLaunchDate(void){return(_launchdate);}
  int GetVerbosity(void){return(_verbosity);}
  //GenericObject_Delete_t* GetDelete(void){return(_delete);}
  clock_t GetSavedClock(void){return(_savedclock);}
  
  /* The setters */
  void SetLaunchClock(clock_t x){_launchclock = x;}
  void SetLaunchTime(time_t* x){_launchtime = x;}
  void SetLaunchDate(char* x){_launchdate = x;}
  void SetVerbosity(int x){_verbosity = x;}
  //void SetDelete(GenericObject_Delete_t* x){_delete = x;}
  void SetSavedClock(clock_t x){_savedclock = x;}

  public:
  static Message_t* Create(void){
    Message_t* msg = (Message_t*) Mry_New(Message_t);
    char* date = (char*) Mry_New(char,26);
    time_t* now = (time_t*) Mry_New(time_t);
    clock_t start = clock();
    
    time(now);
    strcpy(date,ctime(now));

    msg->SetLaunchDate(date);
    msg->SetLaunchTime(now);
    msg->SetLaunchClock(start);
    msg->SetSavedClock(start);
    msg->SetVerbosity(4);
    
    //msg->SetDelete(&Delete);
    
    return(msg);
  }
  
  static void Delete(void* self){
    Message_t* msg = (Message_t*) self;
    
    Mry_Free(msg->GetLaunchDate());
    Mry_Free(msg->GetLaunchTime());
    //msg->SetDelete(NULL);
  }
  
  static void FatalError(const char* fmt, ...){
    fflush(stdout);
    
    fprintf(stderr,"\nBil fatal error...\n");
    
    {
      va_list args;
      va_start(args,fmt);
      vfprintf(stderr,fmt,args);
      va_end(args);
    }
    
    fprintf(stderr,"\n...stop\n");
    fflush(stderr);
    
    exit(EXIT_SUCCESS);
  }
  
  void RuntimeError(const char* fmt, ...){
    if(GetVerbosity() < 1) {
      exit(EXIT_SUCCESS);
      return;
    }
    
    fflush(stdout);
    
    fprintf(stderr,"\nBil runtime error...\n");
    
    {
      va_list args;
      va_start(args,fmt);
      vfprintf(stderr,fmt,args);
      va_end(args);
    }
    
    fprintf(stderr,"\n...stop\n");
    fflush(stderr);
    
    exit(EXIT_SUCCESS);
  }
  
  void InputError(const char* name,const int i){
    if(GetVerbosity() < 1) {
      exit(EXIT_SUCCESS);
      return;
    }
    
    fflush(stdout);
    
    fprintf(stderr,"\nBil input error...\n");
    
    {
      fprintf(stderr,"** On entry to %s, parameter number %d had an illegal value",name,i);
    }
    
    fprintf(stderr,"\n...stop\n");
    fflush(stderr);
    
    exit(EXIT_SUCCESS);
  }

  void Warning(const char* fmt, ...){
    if(DistributedMS_RankOfCallingProcess) return;
    
    if(GetVerbosity() < 2) return;
    
    fflush(stdout);
    
    fprintf(stderr,"\n");
    fprintf(stderr,"Warning: ");
    
    {
      va_list args;
      va_start(args,fmt);
      vfprintf(stderr,fmt,args);
      va_end(args);
    }
    
    fprintf(stderr,"\n");
    fflush(stderr);
  }

  void Info(const char* fmt, ...){
    if(DistributedMS_RankOfCallingProcess) return;
    
    if(GetVerbosity() < 3) return;
    
    fflush(stdout);
    
    fprintf(stdout,"\n");
    fprintf(stdout,"Info: ");
    
    {
      va_list args;
      va_start(args,fmt);
      vfprintf(stdout,fmt,args);
      va_end(args);
    }
    
    fprintf(stdout,"\n");
    fflush(stdout);
  }

  int Direct(const char* fmt, ...){
    int n;
    
    if(DistributedMS_RankOfCallingProcess) return(0);
    
    if(GetVerbosity() < 4) return(0);
    
    fflush(stdout);
    
    {
      va_list args;
      va_start(args,fmt);
      n = vfprintf(stdout,fmt,args);
      va_end(args);
    }
    
    fflush(stdout);
    return(n);
  }
  
  double CPUTime(void){
    double start = (double) GetLaunchClock();
    double end   = (double) clock();
    double t_cpu = end - start;
    double elapsed = (t_cpu) / CLOCKS_PER_SEC;
    
    SetSavedClock(clock());
    
    return(elapsed);
  }
  
  double CPUTimeInterval(void){
    double start = (double) GetSavedClock();
    double end   = (double) clock();
    double t_cpu = end - start;
    double elapsed = (t_cpu) / CLOCKS_PER_SEC;
    
    return(elapsed);
  }
  
  int SetNewVerbosity(const int newverb){
    int oldverb = GetVerbosity();
    
    SetVerbosity(newverb);
    
    return(oldverb);
  }
  
  inline static Message_t* GetInstance(void);
};


#include <assert.h>
#include "Session.h"
#include "GenericData.h"


Message_t* Message_t::GetInstance(void){
    GenericData_t* gdat = Session_FindGenericData(Message_t,"Message");
    
    if(!gdat) {
      Message_t* msg = Create();
      
      gdat = GenericData_Create(1,msg,"Message");
      
      Session_AddGenericData(gdat);
      
      assert(gdat == Session_FindGenericData(Message_t,"Message"));
    }
    
    return((Message_t*) GenericData_GetData(gdat));
}

#endif
