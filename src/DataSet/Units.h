#ifndef UNITS_H
#define UNITS_H


/* Forward declarations */
struct Units_t; //typedef struct Units_t        Units_t ;
struct DataFile_t;
struct Unit_t;
 
 
extern Units_t* (Units_New)     (void) ;
extern Units_t* (Units_Create)  (DataFile_t*) ;
extern void     (Units_Scan)    (Units_t*,DataFile_t*);
extern void     (Units_Delete)  (void*) ;


#define Units_MaxNbOfUnits   (7)


#define Units_GetNbOfUnits(U)         ((U)->GetNbOfUnits())
#define Units_GetUnit(U)              ((U)->GetUnit())
#define Units_GetCapacity(U)          ((U)->GetCapacity())

#define Units_SetNbOfUnits(U,n)      ((U)->SetNbOfUnits(n))
#define Units_SetUnit(U,u)           ((U)->SetUnit(u))

#define Units_EmplaceBack(U,...)     ((U)->EmplaceBack(__VA_ARGS__))


#include <stdexcept>

struct Units_t {
  private:
  size_t _n_units;
  Unit_t*  _unit;

  public:
  Unit_t& operator[](size_t);
  size_t GetCapacity() const {return Units_MaxNbOfUnits;}
  void EmplaceBack(const std::string&, const std::string&);

  public:
  size_t GetNbOfUnits() const {return _n_units;}
  Unit_t* GetUnit() {return _unit;}

  void SetNbOfUnits(size_t n) {
    _n_units = n;
    if(_n_units >= GetCapacity()) {
      throw std::length_error("Maximum number of units reached");
    }
  }
  void SetUnit(Unit_t* u) {_unit = u;}
} ;


#include "Unit.h"

  inline Unit_t& Units_t::operator[](size_t n) {
    if(n >= _n_units) {
      throw std::out_of_range("Index out of bounds");
    }
    return(_unit[n]);
  }

  inline void Units_t::EmplaceBack(const std::string& name, const std::string& value) {
    Unit_t* unit = _unit + _n_units;

    if(_n_units >= GetCapacity()) {
      throw std::length_error("Maximum number of units reached");
    }
    
    Unit_Set(unit,name,value);
    _n_units++;
  }

#endif
