#ifndef LOAD_H
#define LOAD_H


/* Forward declarations */
struct Load_t; //typedef struct Load_t         Load_t ;
struct DataFile_t;
struct Function_t;
struct Functions_t;
struct Fields_t;
struct Field_t;


extern Load_t* (Load_New)    (Fields_t*,Functions_t*);
extern void    (Load_Delete) (void*) ;
extern void    (Load_Scan)   (Load_t*,DataFile_t*) ;




#define Load_MaxLengthOfRegionName      Region_MaxLengthOfRegionName
#define Load_MaxLengthOfKeyWord               (30)


#define Load_GetRegionName(LOAD)         ((LOAD)->GetRegionName())
#define Load_GetType(LOAD)               ((LOAD)->GetType())
#define Load_GetNameOfEquation(LOAD)     ((LOAD)->GetNameOfEquation())
#define Load_GetFunctionIndex(LOAD)      ((LOAD)->GetFunctionIndex())
#define Load_GetFieldIndex(LOAD)         ((LOAD)->GetFieldIndex())
#define Load_GetFunction(LOAD)           ((LOAD)->GetFunction())
#define Load_GetField(LOAD)              ((LOAD)->GetField())
#define Load_GetFunctions(LOAD)          ((LOAD)->GetFunctions())
#define Load_GetFields(LOAD)             ((LOAD)->GetFields())

#define Load_SetRegionName(LOAD,A)       ((LOAD)->SetRegionName(A))
#define Load_SetType(LOAD,A)             ((LOAD)->SetType(A))
#define Load_SetNameOfEquation(LOAD,A)   ((LOAD)->SetNameOfEquation(A))
#define Load_SetFunctionIndex(LOAD,A)    ((LOAD)->SetFunctionIndex(A))
#define Load_SetFieldIndex(LOAD,A)       ((LOAD)->SetFieldIndex(A))
#define Load_SetFunction(LOAD,A)         ((LOAD)->SetFunction(A))
#define Load_SetField(LOAD,A)            ((LOAD)->SetField(A))
#define Load_SetFunctions(LOAD,A)        ((LOAD)->SetFunctions(A))
#define Load_SetFields(LOAD,A)           ((LOAD)->SetFields(A))


#define Load_Set(LOAD,...)               ((LOAD)->Set(__VA_ARGS__))


#define Load_TypeIs(LOAD,TYPE)           (!strcmp(Load_GetType(LOAD),TYPE))

#include <string>
#include <stdexcept>

struct Load_t {
  private:
  Function_t*  _Function ;
  Field_t*     _Field ;
  Functions_t* _Functions ;
  Fields_t*    _Fields ;
  char*  _RegionName ;
  char*  _Type ;
  char*  _NameOfEquation ;
  //int    _FunctionIndex ;
  //int    _FieldIndex ;

  public:
  char* GetRegionName(){return _RegionName;}
  char* GetType(){return _Type;}
  char* GetNameOfEquation(){return _NameOfEquation;}
  //int GetFunctionIndex(){return _FunctionIndex;}
  //int GetFieldIndex(){return _FieldIndex;}
  Function_t* GetFunction(){return _Function;}
  Field_t* GetField(){return _Field;}
  Functions_t* GetFunctions(){return _Functions;}
  Fields_t* GetFields(){return _Fields;}

  void SetRegionName(char* a){_RegionName = a;}
  void SetType(char* a){_Type = a;}
  void SetNameOfEquation(char* a){_NameOfEquation = a;}
  //void SetFunctionIndex(int a){_FunctionIndex = a;}
  //void SetFieldIndex(int a){_FieldIndex = a;}
  void SetFunction(Function_t* a){_Function = a;}
  void SetField(Field_t* a){_Field = a;}
  void SetFunctions(Functions_t* a){_Functions = a;}
  void SetFields(Fields_t* a){_Fields = a;}

  void Set(std::string const& region,std::string const& equation,std::string const& type,size_t const& fieldindex,size_t const& functionindex){
    Set(region.c_str(),equation.c_str(),type.c_str(),fieldindex,functionindex);
  }
  void Set(char const* region,char const* equation,char const* type,size_t const& fieldindex,size_t const& functionindex){
    /* Region */
    if(strlen(region) > Load_MaxLengthOfKeyWord) {
      throw std::runtime_error("Load_t::Set: region name too long") ;
    } else {
      strncpy(_RegionName,region,Load_MaxLengthOfRegionName);
    }

    /* Equation*/
    if(strlen(equation) > Load_MaxLengthOfKeyWord) {
      throw std::runtime_error("Load_t::Set: equation name too long") ;
    } else {
      strncpy(_NameOfEquation,equation,Load_MaxLengthOfKeyWord) ;
    }
      
    if(isdigit(_NameOfEquation[0])) {
      if(atoi(_NameOfEquation) < 1) {
        throw std::runtime_error("Load_t::Set: not positive number") ;
      }
    }

    /* Type */
    if(strlen(type) > Load_MaxLengthOfKeyWord) {
      throw std::runtime_error("Load_t::Set: type name too long") ;
    } else {
      strncpy(_Type,type,Load_MaxLengthOfKeyWord) ;
    }
      
    if(String_CaseIgnoredIs(type,"flux")) {
      Message_Warning("Load_t::Set:\n"\
      "since June 2022, the definition of the type \"flux\" has changed\n"\
      "the old definition of this type is renamed  \"cumulflux\"\n"\
      "so use the types:\n"\
      "- \"flux\" for a real flux (e.g. kg/m2/s)\n"\
      "- \"cumulflux\" for a cumulative flux (e.g. kg/m2)\n") ;
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
        throw std::runtime_error("Load_t::Set: field out of range") ;
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
        throw std::runtime_error("Load_t::Set: function out of range") ;
      }
    }
  }
} ;


/* Old notations which I try to eliminate little by little */
#define char_t    Load_t

#include "Region.h"
#endif
