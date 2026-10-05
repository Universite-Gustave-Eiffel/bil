#ifndef DATES_H
#define DATES_H


/* Forward declarations */
struct Dates_t;
struct DataFile_t;
struct Date_t;

#include <vector>

extern Dates_t*  (Dates_New)    (void) ;
extern Dates_t*  (Dates_Create) (DataFile_t*) ;
extern void      (Dates_Scan)   (Dates_t*,DataFile_t*);
extern void      (Dates_Delete) (void*) ;


#define Dates_MaxNbOfDates             (1000)



#define Dates_GetNbOfDates(DATES)       ((DATES)->GetNbOfDates())
#define Dates_GetDate(DATES)            ((DATES)->GetDate())

#define Dates_SetNbOfDates(DATES,NB)    ((DATES)->SetNbOfDates(NB))
#define Dates_SetDate(DATES,DATE)       ((DATES)->SetDate(DATE))

#define Dates_Set(DATES,...)            ((DATES)->Set(__VA_ARGS__))


#include <vector>

struct Dates_t {
  private:
  size_t _nbofdates ;               /* nb of dates */
  Date_t* _date ;              /* date */

  public:
  /* The getters*/
  size_t GetCapacity(){return Dates_MaxNbOfDates;}
  size_t GetNbOfDates(){ return _nbofdates; }
  Date_t* GetDate(){ return _date; }

  /* The setters */
  void SetNbOfDates(size_t nbofdates){ _nbofdates = nbofdates; }
  void SetDate(Date_t* date){ _date = date; }

  void Set(const std::vector<double>& t){Set(t.size(),t.data());}
  void Set(size_t const&,double const*);
} ;


#include "Date.h"

  inline void Dates_t::Set(size_t const& n,double const* t){
    if(n > GetCapacity()) {
      throw std::length_error("Maximum number of dates reached");
    }

    _nbofdates = n;
    for(size_t i = 0 ; i < n ; i++) {
      Date_Set(_date + i,t[i]);
    }
  }

#endif
