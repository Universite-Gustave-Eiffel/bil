#ifndef BCOND_H
#define BCOND_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct BCond_t; //typedef struct BCond_t        BCond_t ;
struct DataFile_t;
struct Node_t;
struct Functions_t;
struct Fields_t;


extern BCond_t*  (BCond_New)     (Fields_t*,Functions_t*);
extern void      (BCond_Delete)  (void*) ;
extern void      (BCond_Scan)    (BCond_t*,DataFile_t*) ;
extern void      (BCond_AssignBoundaryConditionsAtOverlappingNodes)(BCond_t*,Node_t*,int,double);



#define BCond_MaxLengthOfKeyWord        (30)
#define BCond_MaxLengthOfRegionName      Region_MaxLengthOfRegionName


#define BCond_GetRegionName(BC)          ((BC)->GetRegionName())
#define BCond_GetNameOfUnknown(BC)       ((BC)->GetNameOfUnknown())
#define BCond_GetNameOfEquation(BC)      ((BC)->GetNameOfEquation())
#define BCond_GetFunction(BC)            ((BC)->GetFunction())
#define BCond_GetField(BC)               ((BC)->GetField())
#define BCond_GetEntryInDataFile(BC)     ((BC)->GetEntryInDataFile())
//#define BCond_GetFunctionIndex(BC)       ((BC)->GetFunctionIndex())
//#define BCond_GetFieldIndex(BC)          ((BC)->GetFieldIndex())
#define BCond_GetFunctions(BC)           ((BC)->GetFunctions())
#define BCond_GetFields(BC)              ((BC)->GetFields())

#define BCond_SetRegionName(BC,A)        ((BC)->SetRegionName(A))
#define BCond_SetNameOfUnknown(BC,A)     ((BC)->SetNameOfUnknown(A))
#define BCond_SetNameOfEquation(BC,A)    ((BC)->SetNameOfEquation(A))
#define BCond_SetFunction(BC,A)          ((BC)->SetFunction(A))
#define BCond_SetField(BC,A)             ((BC)->SetField(A))
#define BCond_SetEntryInDataFile(BC,A)   ((BC)->SetEntryInDataFile(A))
//#define BCond_SetFunctionIndex(BC,A)     ((BC)->SetFunctionIndex(A))
//#define BCond_SetFieldIndex(BC,A)        ((BC)->SetFieldIndex(A))
#define BCond_SetFunctions(BC,A)         ((BC)->SetFunctions(A))
#define BCond_SetFields(BC,A)            ((BC)->SetFields(A))

#define BCond_Set(BC,...)                ((BC)->Set(__VA_ARGS__))


#include <stdexcept>
#include <string>

struct BCond_t {
  private:
  Function_t* _Function ;
  Field_t* _Field ;
  Functions_t* _Functions ;
  Fields_t* _Fields ;
  char*  _EntryInDataFile ;
  char*  _RegionName ;
  char*  _NameOfUnknown ;
  char*  _NameOfEquation ;
  //int    _FunctionIndex ;
  //int    _FieldIndex ;

  public:
  Function_t* GetFunction(){return _Function;}
  Field_t* GetField(){return _Field;}
  Functions_t* GetFunctions(){return _Functions;}
  Fields_t* GetFields(){return _Fields;}
  char*  GetEntryInDataFile(){return _EntryInDataFile;}
  char*  GetRegionName(){return _RegionName;}
  char*  GetNameOfUnknown(){return _NameOfUnknown;}
  char*  GetNameOfEquation(){return _NameOfEquation;}
  //int    GetFunctionIndex(){return _FunctionIndex;}
  //int    GetFieldIndex(){return _FieldIndex;}

  void SetFunction(Function_t* a){_Function = a;}
  void SetField(Field_t* a){_Field = a;}
  void SetFunctions(Functions_t* a){_Functions = a;}
  void SetFields(Fields_t* a){_Fields = a;}
  void SetEntryInDataFile(char* a){_EntryInDataFile = a;}
  void SetRegionName(char* a){_RegionName = a;}
  void SetNameOfUnknown(char* a){_NameOfUnknown = a;}
  void SetNameOfEquation(char* a){_NameOfEquation = a;}
  //void SetFunctionIndex(int a){_FunctionIndex = a;}
  //void SetFieldIndex(int a){_FieldIndex = a;}

  void Set(std::string const& region,std::string const& equation,std::string const& unknown,size_t const& fieldindex,size_t const& functionindex){
    Set(region.c_str(),equation.c_str(),unknown.c_str(),fieldindex,functionindex);
  }
  void Set(std::string const& region,std::string const& unknown,size_t const& fieldindex,size_t const& functionindex){
    Set(region.c_str(),unknown.c_str(),fieldindex,functionindex);
  }
  void Set(char const* region,char const* unknown,size_t const& fieldindex,size_t const& functionindex){
    Set(region," ",unknown,fieldindex,functionindex);
  }
  void Set(char const* region,char const* equation,char const* unknown,size_t const& fieldindex,size_t const& functionindex){
    /* Region */
    if(strlen(region) > BCond_MaxLengthOfKeyWord) {
      throw std::runtime_error("BCond_t::Set: name %s too long") ;
    } else {
      strncpy(_RegionName,region,BCond_MaxLengthOfRegionName)  ;
    }

    /* Unknown */
    if(strlen(unknown) > BCond_MaxLengthOfKeyWord-1)  {
      throw std::runtime_error("BCond_t::Set: too long name of unknown") ;
    } else {
      strcpy(_NameOfUnknown,unknown) ;
    }
    
    if(isdigit(_NameOfUnknown[0])) {
      if(atoi(_NameOfUnknown) < 1) {
        throw std::runtime_error("BCond_t::Set: non positive unknown") ;
      }
    }
      
    /* Field */
    {
      Fields_t* fields = GetFields() ;
      size_t n_fields = Fields_GetNbOfFields(fields) ;

      //SetFieldIndex(-1) ;
      SetField(NULL) ;
      
      if(fieldindex == 0) {
      } else if(fieldindex <= n_fields) {
        Field_t* field = Fields_GetField(fields) ;

        //SetFieldIndex(fieldindex - 1) ;
        
        SetField(field + fieldindex - 1) ;
      } else {
        throw ("BCond_t::Set: field out of range") ;
      }
    }

    /* Function*/
    {
      Functions_t* functions = GetFunctions() ;
      size_t n_functions = Functions_GetNbOfFunctions(functions) ;

      //SetFunctionIndex(-1) ;
      SetFunction(NULL) ;
    
      if(functionindex == 0) {
      } else if(functionindex <= n_functions) {
        Function_t* function = Functions_GetFunction(functions) ;

        //SetFunctionIndex(functionindex - 1) ;
        SetFunction(function + functionindex - 1) ;
      } else {
        throw std::runtime_error("BCond_t::Set: function out of range") ;
      }
    }
    
    /* Equation (not mandatory) */
    if(strlen(equation) > BCond_MaxLengthOfKeyWord-1)  {
      throw std::runtime_error("BCond_t::Set: too long name of equation") ;
    } else {
      strcpy(_NameOfEquation,equation) ;
    }
        
    if(isdigit(_NameOfEquation[0])) {
      if(atoi(_NameOfEquation) < 1) {
        throw std::runtime_error("BCond_t::Set: non positive equation") ;
      }
    }
  }
} ;


/* Old notations which I try to eliminate little by little */
#define cond_t    BCond_t


#ifdef __CPLUSPLUS
}
#endif

#include "Region.h"
#endif
