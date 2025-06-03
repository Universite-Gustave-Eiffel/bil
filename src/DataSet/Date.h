#ifndef DATE_H
#define DATE_H


/* Forward declarations */
struct Date_t; //typedef struct Date_t        Date_t ;




extern Date_t*  (Date_New)    (void) ;
extern void     (Date_Delete) (void*) ;



#define Date_GetTime(DATE)            ((DATE)->time)




struct Date_t {
  double time ;
} ;

#endif
