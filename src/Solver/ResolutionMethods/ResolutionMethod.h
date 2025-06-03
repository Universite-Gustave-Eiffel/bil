#ifndef RESOLUTIONMETHOD_H
#define RESOLUTIONMETHOD_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


enum ResolutionMethod_e {     /* Type of resolution method */
  ResolutionMethod_CROUT,     /* Crout method */
  ResolutionMethod_SuperLU,   /* SuperLU method */
  ResolutionMethod_MA38,      /* MA38 method */
  ResolutionMethod_SuperLUMT,  /* SuperLUMT method (multi-threaded) */
  ResolutionMethod_SuperLUDist,  /* SuperLUDist method (distributed memory) */
  ResolutionMethod_PetscKSP,    /* KSP method (distributed memory) */
  ResolutionMethod_PetscGMRES,  /* GMRES method (distributed memory) */
  ResolutionMethod_NULL
} ;

typedef enum ResolutionMethod_e  ResolutionMethod_e ;


/* Forward declarations */
struct ResolutionMethod_t; //typedef struct ResolutionMethod_t  ResolutionMethod_t ;
struct Options_t;


extern ResolutionMethod_t* (ResolutionMethod_Create)(Options_t*) ;
extern void                (ResolutionMethod_Delete)(void*) ;



/** The getters */
#define ResolutionMethod_GetType(RM)       ((RM)->type)
#define ResolutionMethod_GetOptions(RM)    ((RM)->options)



#define ResolutionMethod_Is(RM,KEY) \
        (ResolutionMethod_GetType(RM) == Utils_CAT(ResolutionMethod_,KEY))

#define ResolutionMethod_Type(KEY) \
        ((ResolutionMethod_e) Utils_CAT(ResolutionMethod_,KEY))



/* complete the structure types by using the typedef */

struct ResolutionMethod_t {
  ResolutionMethod_e type ;
  Options_t* options ;
} ;


#ifdef __CPLUSPLUS
}
#endif

#include "Utils.h"
#endif
