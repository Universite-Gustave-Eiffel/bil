#ifndef UNIT_H
#define UNIT_H



/* Forward declarations */
struct Unit_t; //typedef struct Unit_t         Unit_t ;
struct DataFile_t;


extern Unit_t* (Unit_New)     (void) ;
extern Unit_t* (Unit_Create)  (char const*,char const*);
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
  
  void Set(char const*,char const*);
} ;


#include "InternationalSystemOfUnits.h"

  inline void Unit_t::Set(char const* quantity,char const* name){
    strcpy(_quantity,quantity);
    strcpy(_name,name);

    /* Length */ 
    if(strcmp(quantity,"Length") == 0) {
      InternationalSystemOfUnits_UseAsLength(name) ;
    }
  
    /* Time */ 
    if(strcmp(quantity,"Time") == 0) {
      InternationalSystemOfUnits_UseAsTime(name) ;
    }
  
    /* Mass */  
    if(strcmp(quantity,"Mass") == 0) {
      InternationalSystemOfUnits_UseAsMass(name) ;
    }
  }


#endif
