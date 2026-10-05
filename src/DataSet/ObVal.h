#ifndef OBVAL_H
#define OBVAL_H


/* Forward declarations */
struct ObVal_t; //typedef struct ObVal_t        ObVal_t ;
struct DataFile_t;




extern ObVal_t*  (ObVal_New)    (void) ;
extern void      (ObVal_Delete) (void*) ;
extern void      (ObVal_Scan)   (ObVal_t*,DataFile_t*) ;



#define ObVal_MaxLengthOfKeyWord        (30)


#define ObVal_GetType(OV)             ((OV)->GetType())
#define ObVal_GetNameOfUnknown(OV)    ((OV)->GetNameOfUnknown())
#define ObVal_GetValue(OV)            ((OV)->GetValue())
#define ObVal_GetRelaxationFactor(OV) ((OV)->GetRelaxationFactor())

#define ObVal_SetType(OV,A)             ((OV)->SetType(A))
#define ObVal_SetNameOfUnknown(OV,A)    ((OV)->SetNameOfUnknown(A))
#define ObVal_SetValue(OV,A)            ((OV)->SetValue(A))
#define ObVal_SetRelaxationFactor(OV,A) ((OV)->SetRelaxationFactor(A))

#define ObVal_Set(OV,...)               ((OV)->Set(__VA_ARGS__))



#define ObVal_IsRelativeValue(OV) \
        (ObVal_GetType(OV) == 'r')
        
#define ObVal_SetTypeToRelative(OV) \
        (ObVal_SetType(OV,'r'))


#define ObVal_IsAbsoluteValue(OV) \
        (ObVal_GetType(OV) == 'a')
        
#define ObVal_SetTypeToAbsolute(OV) \
        (ObVal_SetType(OV,'a'))



#define ObVal_GetAbsoluteValue(OV,U) \
        (ObVal_GetValue(OV) * ((ObVal_IsAbsoluteValue(OV)) ? 1 : fabs(U)))

#define ObVal_GetRelativeValue(OV,U) \
        (ObVal_GetValue(OV) / ((ObVal_IsRelativeValue(OV)) ? 1 : fabs(U)))


#include <string>
#include <optional>

struct ObVal_t {              /* Objective variation */
  char    _type ;              /* Type = a(bsolute) or r(elative) */
  char*   _inc ;               /* Name of the unknown */
  double  _val ;               /* Objective variation */
  double  _relaxfactor ;       /* Relaxation factor */

  char    GetType(){return _type ;}
  char*   GetNameOfUnknown(){return _inc ;}
  double  GetValue(){return _val ;}
  double  GetRelaxationFactor(){return _relaxfactor ;}

  void  SetType(char const a){_type = a;}
  void  SetNameOfUnknown(char* a){_inc = a;}
  void  SetValue(double const &a){_val = a;}
  void  SetRelaxationFactor(double const& a){_relaxfactor = a;}

  void Set(std::string const& name,double const& v,std::optional<std::string> const& type = std::nullopt,double const& r = 0){
    if(type) {
      Set(name.c_str(),v,type->c_str(),r);
    } else {
      Set(name.c_str(),v);
    }
  }
  void Set(char const* name,double const& v,char const* type = nullptr,double const& r = 0){
    /* Unknown */
    if(strlen(name) > ObVal_MaxLengthOfKeyWord-1)  {
      throw std::runtime_error("ObVal_t::Set: too long name of unknown") ;
    } else {
      strcpy(GetNameOfUnknown(),name) ;
    }
        
    /* Value */
    if(v < 0.) {
      throw std::runtime_error("ObVal_t::Set: negative value") ;
    } else {
      SetValue(v) ;
    }
  
    /* Type: absolute or relative? */
    if(type) {
      if(strlen(type) > ObVal_MaxLengthOfKeyWord-1)  {
        throw std::runtime_error("ObVal_t::Set: too long type of unknown") ;
      } else {
        /* Set default type */
        SetType('a') ;
          
        /* Read the type "absolute" */
        if(String_CaseIgnoredIs(type,"Absolute",3)) {
          SetType('a') ;
        }
          
        /* Read the type "relative" */
        if(String_CaseIgnoredIs(type,"Relative",3)) {
          SetType('r') ;
        }
      }
    } else {
      SetType('a') ;
    }
  
    /* Relaxation factor (if any) */
    if(r > 0) {
      SetRelaxationFactor(r) ;
    }
  }
} ;


#include <math.h>

#endif
