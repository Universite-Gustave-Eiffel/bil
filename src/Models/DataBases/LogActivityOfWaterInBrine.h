#ifndef LOGACTIVITYOFWATERINBRINE_H
#define LOGACTIVITYOFWATERINBRINE_H



/* Forward declarations */
struct LogActivityOfWaterInBrine_t; //typedef struct LogActivityOfWaterInBrine_t     LogActivityOfWaterInBrine_t ;
struct Curves_t;



extern LogActivityOfWaterInBrine_t* (LogActivityOfWaterInBrine_Create)(void) ;
extern double (LogActivityOfWaterInBrine)(double,double,const char*) ;


/* 
 * Dependance on the temperature
 * log(a_w(T)) = log(a_w(T0)) + h_w/R (1/T - 1/T0)
 */




//#define LogActivityOfWaterInBrine_GetTemperature(LAW) ((LAW)->temperature)
#define LogActivityOfWaterInBrine_GetCurves(LAW)      ((LAW)->curves)
#define LogActivityOfWaterInBrine_GetNbOfSalt(LAW)    ((LAW)->nbofsalt)



#define LogActivityOfWaterInBrine_GetCurve(LAW) \
        Curves_GetCurve(LogActivityOfWaterInBrine_GetCurves(LAW))


/* Macros for the room temperature
 * -------------------------------*/
#define LogActivityOfWaterInBrine_GetRoomTemperature(LAW) \
        Temperature_GetRoomValue(LogActivityOfWaterInBrine_GetTemperature(LAW))
        

#define LogActivityOfWaterInBrine_SetRoomTemperature(LAW,T) \
        Temperature_SetRoomTemperature(LogActivityOfWaterInBrine_GetTemperature(LAW),T)



struct LogActivityOfWaterInBrine_t {
  //Temperature_t* temperature ;
  int nbofsalt ;
  Curves_t* curves ;
} ;


#include "Curves.h"
#include "Temperature.h"


#endif
