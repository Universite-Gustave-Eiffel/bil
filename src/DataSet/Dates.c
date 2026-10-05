#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Message.h"
#include "DataFile.h"
#include "Mry.h"
#include "String_.h"
#include "Dates.h"
#include "Date.h"





Dates_t*  (Dates_New)(void)
{
  Dates_t* dates = (Dates_t*) Mry_New(Dates_t) ;
  
  Dates_SetNbOfDates(dates,0) ;

  {
    Date_t* date = Mry_Create(Date_t,Dates_MaxNbOfDates,Date_New()) ;

    Dates_SetDate(dates, date) ;
  }
  
  return(dates) ;
}




#if 0
Dates_t*  (Dates_Create)(DataFile_t* datafile)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"TEMP,DATE,Dates",",") ;
  size_t n_dates = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  Dates_t* dates = Dates_New() ;
  
  
  Message_Direct("Enter in %s","Dates") ;
  Message_Direct("\n") ;


  {
    double* t = (double*) Mry_New(double,n_dates) ;
    
    c = String_SkipLine(c) ;
    
    String_ScanArray(c,n_dates," %lf",t) ;

    Dates_Set(dates,n_dates,t);
    
    Mry_Free(t) ;
  }
  
  
  return(dates) ;
}
#else
Dates_t*  (Dates_Create)(DataFile_t* datafile)
{
  Dates_t* dates = Dates_New() ;
  
  Dates_Scan(dates,datafile);
  
  return(dates) ;
}
#endif





void  (Dates_Scan)(Dates_t* dates,DataFile_t* datafile)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"TEMP,DATE,Dates",",") ;
  size_t n_dates = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  
  
  Message_Direct("Enter in %s","Dates") ;
  Message_Direct("\n") ;


  {
    double* t = (double*) Mry_New(double,n_dates) ;
    
    c = String_SkipLine(c) ;
    
    String_ScanArray(c,n_dates," %lf",t) ;

    Dates_Set(dates,n_dates,t);
    
    Mry_Free(t) ;
  }
  
  
  return ;
}



void  (Dates_Delete)(void* self)
{
  Dates_t* dates = (Dates_t*) self ;
  
  if(dates) {
    Date_t* date  = Dates_GetDate(dates) ;
    
    if(date) {
      Mry_Delete(date,Dates_MaxNbOfDates,Date_Delete) ;
      Mry_Free(date) ;
      Dates_SetDate(dates,NULL) ;
    }
  }
}
