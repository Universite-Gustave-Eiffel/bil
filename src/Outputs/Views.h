#ifndef VIEWS_H
#define VIEWS_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct Views_t; //typedef struct Views_t      Views_t ;
struct View_t;


extern Views_t*   (Views_Create)(int) ;
extern void       (Views_Delete)(void*) ;


#define Views_GetNbOfViews(views)      ((views)->nbofviews)
#define Views_GetView(views)           ((views)->view)


#define Views_MaxNbOfViews           (200)


struct Views_t {            /* Views */
  int nbofviews ;  /* nb of views */
  View_t *view ;            /* view */
} ;


#ifdef __CPLUSPLUS
}
#endif
#endif
