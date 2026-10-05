#ifndef DATE_H
#define DATE_H


/* Forward declarations */
struct Date_t; //typedef struct Date_t        Date_t ;




extern Date_t*  (Date_New)    (void) ;
extern void     (Date_Delete) (void*) ;




#define Date_GetTime(DATE)        ((DATE)->GetTime())

#define Date_SetTime(DATE,A)      ((DATE)->SetTime(A))

#define Date_Set(DATE,...)        ((DATE)->Set(__VA_ARGS__))




struct Date_t {
  private:
  double _time ;

  public:
  Date_t(double time = 0) : _time(time) {}

  /* The getters */
  double GetTime(){ return _time; }

  /* The setters */
  void SetTime(double time){ _time = time; }
  void Set(double const& t){
    _time = t;
  }
} ;

#endif
