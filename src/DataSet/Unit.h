#ifndef UNIT_H
#define UNIT_H



/* Forward declarations */
struct Unit_t; //typedef struct Unit_t         Unit_t ;
struct DataFile_t;

#include <string>

extern Unit_t* (Unit_New)     (void) ;
extern Unit_t* (Unit_Create)  (std::string&,std::string&);
extern void    (Unit_Delete)  (void*) ;
extern int     (Unit_Scan)    (Unit_t*,DataFile_t*) ;



#define Unit_MaxLengthOfKeyWord   (30)



#define Unit_GetQuantity(U)    ((U)->GetQuantity())
#define Unit_GetName(U)        ((U)->GetName())

#define Unit_SetQuantity(U,A)  ((U)->SetQuantity(A))
#define Unit_SetName(U,A)      ((U)->SetName(A))

#define Unit_Set(U,...)        ((U)->Set(__VA_ARGS__))



#include <string>

struct Unit_t {           /* unit */
  private:
  char* _quantity ;       /* Fundamental quantity: length, mass, time, ... */
  char* _name ;           /* The unit name as a fraction of its SI unit */

  public:
  char* GetQuantity() {return _quantity;}
  char* GetName() {return _name;}

  void SetQuantity(char* quantity) {_quantity = quantity;}
  void SetName(char* name) {_name = name;}
  
  void Set(std::string const&,std::string const&);
} ;


#include "InternationalSystemOfUnits.h"

  inline void Unit_t::Set(std::string const& quantity,std::string const& name){
    strcpy(_quantity,quantity.c_str());
    strcpy(_name,name.c_str());

    /* Length */ 
    if(quantity.compare("Length") == 0) {
      InternationalSystemOfUnits_UseAsLength(name.c_str()) ;
    }
  
    /* Time */ 
    if(quantity.compare("Time") == 0) {
      InternationalSystemOfUnits_UseAsTime(name.c_str()) ;
    }
  
    /* Mass */  
    if(quantity.compare("Mass") == 0) {
      InternationalSystemOfUnits_UseAsMass(name.c_str()) ;
    }
  }


#endif
