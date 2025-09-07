#ifndef CURVE_H
#define CURVE_H

#ifdef __CPLUSPLUS
extern "C" {
#endif



/* Forward declarations */
struct Curve_t        ; typedef struct Curve_t        Curve_t ;



extern Curve_t* (Curve_Create)(int) ;
extern void     (Curve_Delete)(void*) ;
extern Curve_t* (Curve_CreateDerivative)(Curve_t const*) ;
extern Curve_t* (Curve_CreateIntegral)(Curve_t const*) ;
extern Curve_t* (Curve_CreateInverse)(Curve_t const*,const char) ;
extern double*  (Curve_CreateSamplingOfX)(Curve_t const*) ;

template<typename T>
extern T        (Curve_ComputeValue)(Curve_t const*,const T&) ;

/* Explicit specialization of template must appear before instantiation.
 * So we declare the specialization here. */
template<>
double   (Curve_ComputeValue<double>)(Curve_t const*,const double&) ;
//extern real     (Curve_ComputeValue)(Curve_t*,const real&);

extern double   (Curve_ComputeDerivative)(Curve_t const*,const double&) ;
extern double   (Curve_ComputeIntegral)(Curve_t const*,const double&) ;
//extern char*    (Curve_PrintInFile)(Curve_t*) ;


#define Curve_MaxLengthOfKeyWord        (30)
#define Curve_MaxLengthOfFileName       (60)
#define Curve_MaxLengthOfTextLine       (500)
#define Curve_MaxLengthOfCurveName      (30)


#define Curve_GetNbOfPoints(curve)      ((curve)->n)
#define Curve_GetXRange(curve)          ((curve)->a)
#define Curve_GetYValue(curve)          ((curve)->f)
#define Curve_GetScaleType(curve)       ((curve)->echelle)
#define Curve_GetNameOfXAxis(curve)     ((curve)->xname)
#define Curve_GetNameOfYAxis(curve)     ((curve)->yname)



/* Set the names of x-axis and y-axis */
#define Curve_SetNameOfXAxis(CV,NAME) \
        strcpy(Curve_GetNameOfXAxis(CV),NAME)

#define Curve_SetNameOfYAxis(CV,NAME) \
        strcpy(Curve_GetNameOfYAxis(CV),NAME)



struct Curve_t {              /* courbe */
  char*  xname ;              /* Name of the x-axis */
  char*  yname ;              /* Name of the y-axis */
  char   echelle ;            /* echelle = n(ormale) ou l(ogarithmique) */
  int    n ;                  /* nombre de points */
  double* a ;                 /* abscisses */
  double* f ;                 /* valeurs f(a) */
} ;



/* Old notations which should be eliminated */
#define crbe_t                 Curve_t
#define courbe(a,b)            Curve_ComputeValue(&(b),(a))
#define dcourbe(a,b)           Curve_ComputeDerivative(&(b),(a))


#ifdef __CPLUSPLUS
}
#endif
#endif
