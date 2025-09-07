#ifndef VIEW_H
#define VIEW_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct View_t; //typedef struct View_t       View_t ;


extern View_t*    (View_Create)(void) ;
extern void       (View_Delete)(void*) ;


#define View_MaxLengthOfViewName    (50)      /* Max length of view name */


#define View_GetNbOfComponents(view)  ((view)->n)
#define View_GetNameOfView(view)      ((view)->name)
#define View_GetGlobalIndex(view)     ((view)->index)



struct View_t {               /* View (scalar, vector, tensor) */
  int n ;               /* Nb of components (1,3,9) */
  char*   name ;              /* Name of the view */
  int     index ;             /* Index of the view in the global set of views */
} ;



#ifdef __CPLUSPLUS
}
#endif
#endif
