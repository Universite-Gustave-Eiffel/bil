#ifndef ICOND_H
#define ICOND_H


/* Forward declarations */
struct ICond_t; //typedef struct ICond_t        ICond_t ;
struct DataFile_t;
struct Functions_t;
struct Function_t;
struct Fields_t;
struct Field_t;


extern ICond_t* (ICond_New)    (Fields_t*,Functions_t*);
extern void     (ICond_Delete) (void*) ;
extern void     (ICond_Scan)   (ICond_t*,DataFile_t*) ;



#define ICond_MaxLengthOfRegionName      Region_MaxLengthOfRegionName
#define ICond_MaxLengthOfKeyWord        (30)
#define ICond_MaxLengthOfFileName       (60)


#define ICond_GetRegionName(IC)              ((IC)->GetRegionName())
#define ICond_GetNameOfUnknown(IC)           ((IC)->GetNameOfUnknown())
#define ICond_GetFunction(IC)                ((IC)->GetFunction())
#define ICond_GetField(IC)                   ((IC)->GetField())
#define ICond_GetFileNameOfNodalValues(IC)   ((IC)->GetFileNameOfNodalValues())
#define ICond_GetEntryInDataFile(IC)         ((IC)->GetEntryInDataFile())
#define ICond_GetFunctions(IC)               ((IC)->GetFunctions())
#define ICond_GetFields(IC)                  ((IC)->GetFields())

#define ICond_SetRegionName(IC,A)            ((IC)->SetRegionName(A))
#define ICond_SetNameOfUnknown(IC,A)         ((IC)->SetNameOfUnknown(A))
#define ICond_SetFunction(IC,A)              ((IC)->SetFunction(A))
#define ICond_SetField(IC,A)                 ((IC)->SetField(A))
#define ICond_SetFileNameOfNodalValues(IC,A) ((IC)->SetFileNameOfNodalValues(A))
#define ICond_SetEntryInDataFile(IC,A)       ((IC)->SetEntryInDataFile(A))
#define ICond_SetFunctions(IC,A)             ((IC)->SetFunctions(A))
#define ICond_SetFields(IC,A)                ((IC)->SetFields(A))

#define ICond_Set(IC,...)                    ((IC)->Set(__VA_ARGS__))
#define ICond_Print(IC)                      ((IC)->Print())


#include <string>

struct ICond_t {
  private:
  Function_t* _Function ;
  Field_t* _Field ;
  Functions_t* _Functions ;
  Fields_t* _Fields ;
  char*  _EntryInDataFile ;
  char*  _RegionName ;
  char*  _NameOfUnknown ;
  char*  _FileNameOfNodalValues ;

  public:
  Function_t* GetFunction(){return _Function ;}
  Field_t* GetField(){return _Field ;}
  Functions_t* GetFunctions(){return _Functions ;}
  Fields_t* GetFields(){return _Fields ;}
  char*  GetEntryInDataFile(){return _EntryInDataFile ;}
  char*  GetRegionName(){return _RegionName ;}
  char*  GetNameOfUnknown(){return _NameOfUnknown ;}
  char*  GetFileNameOfNodalValues(){return _FileNameOfNodalValues ;}

  void SetFunction(Function_t* a){_Function = a;}
  void SetField(Field_t* a){_Field = a;}
  void SetFunctions(Functions_t* a){_Functions = a;}
  void SetFields(Fields_t* a){_Fields = a;}
  void SetEntryInDataFile(char* a){_EntryInDataFile = a;}
  void SetRegionName(char* a){_RegionName = a;}
  void SetNameOfUnknown(char* a){_NameOfUnknown = a;}
  void SetFileNameOfNodalValues(char* a){_FileNameOfNodalValues = a;}

  void Set(std::string const& region,std::string const& unknown,size_t const& ifld,size_t const& ifct) {
    Set(region.c_str(),unknown.c_str(),ifld,ifct);
  }
  void Set(char const* region,char const* unknown,size_t const& ifld,size_t const& ifct) {
    /* Region */
    {
      strncpy(GetRegionName(),region,ICond_MaxLengthOfRegionName)  ;
    }
    
    /* Unknown */
    if(strlen(unknown) > ICond_MaxLengthOfKeyWord-1)  {
      throw std::runtime_error("ICond_t::Set: too long name of unknown") ;
    }
        
    strcpy(GetNameOfUnknown(),unknown) ;
    
    if(isdigit(GetNameOfUnknown()[0])) {
      if(GetNameOfUnknown()[0] < '1') {
        throw std::runtime_error("ICond_t::Set: non positive unknown") ;
      }
    }
    
    /* File */
    {
      strcpy(GetFileNameOfNodalValues(),"\0") ;
    }
    
    /* Field */
    {
      Fields_t* fields = GetFields() ;
      size_t  n_fields = Fields_GetNbOfFields(fields) ;
      
      if(ifld == 0) {
        SetField(NULL) ;
      } else if(ifld <= n_fields) {
        Field_t*  field = Fields_GetField(fields) ;

        SetField(field + ifld - 1) ;
      } else {
        throw std::runtime_error("ICond_t::Set: field out of range") ; 
      }
    }
    
    /* Function */
    {
      Functions_t* functions = GetFunctions() ;
      size_t n_functions = Functions_GetNbOfFunctions(functions) ;
      
      if(ifct == 0) {
        SetFunction(NULL) ;
      } else if(ifct <= n_functions) {
        Function_t* fct = Functions_GetFunction(functions) ;
        
        SetFunction(fct + ifct - 1) ;
      } else {
        throw std::runtime_error("ICond_t::Set: function out of range") ;
      }
    }
  }

  void Set(std::string const& region,std::string const& unknown,std::string const& filename,size_t const& ifct) {
    Set(region.c_str(),unknown.c_str(),filename.c_str(),ifct);
  }
  void Set(char const* region,char const* unknown,char const* filename,size_t const& ifct) {
    /* Region */
    {
      strncpy(GetRegionName(),region,ICond_MaxLengthOfRegionName)  ;
    }
    
    /* Unknown */
    if(strlen(unknown) > ICond_MaxLengthOfKeyWord-1)  {
      throw std::runtime_error("ICond_t::Set: too long name of unknown") ;
    }
        
    strcpy(GetNameOfUnknown(),unknown) ;
    
    if(isdigit(GetNameOfUnknown()[0])) {
      if(GetNameOfUnknown()[0] < '1') {
        throw std::runtime_error("ICond_t::Set: non positive unknown") ;
      }
    }
    
    /* File */
    if(filename) {
      if(strlen(filename) > ICond_MaxLengthOfFileName-1)  {
        throw std::runtime_error("ICond_Scan: name too long") ;
      }
      
      strcpy(GetFileNameOfNodalValues(),filename) ;
    }
    
    /* Field */
    {
      SetField(NULL) ;
    }
    
    /* Function */
    {
      Functions_t* functions = GetFunctions() ;
      size_t n_functions = Functions_GetNbOfFunctions(functions) ;
      
      if(ifct == 0) {
        SetFunction(NULL) ;
      } else if(ifct <= n_functions) {
        Function_t* fct = Functions_GetFunction(functions) ;
        
        SetFunction(fct + ifct - 1) ;
      } else {
        throw std::runtime_error("ICond_t::Set: function out of range") ;
      }
    }
  }

  void Print(void){
    #define PRINT(...) fprintf(stdout,__VA_ARGS__)
    char* reg = GetRegionName() ;
    char* name_unk = GetNameOfUnknown() ;
    Field_t* ch = GetField() ;
    Function_t* fn = GetFunction() ;
      
    PRINT("\t Region  = %s\n",reg) ;
    PRINT("\t Unknown = %s\n",name_unk) ;
      
    if(ch) {
      Fields_t* chs = GetFields() ;
      ptrdiff_t n = ch - Fields_GetField(chs);
        
      PRINT("\t Field = %td (type %s)\n",n,Field_GetType(ch)) ;
    } else {
      PRINT("\t Natural initial condition (null)\n") ;
    }
      
    if(fn) {
      Functions_t* fns = GetFunctions() ;
      ptrdiff_t n = fn - Functions_GetFunction(fns) ;
        
      PRINT("\t Function = %td\n",n) ;
    } else {
      PRINT("\t Function unity (f(t) = 1)\n") ;
    }
    #undef PRINT
  }
} ;


#include "Region.h"
#endif
