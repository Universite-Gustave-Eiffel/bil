#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Message.h"
#include "DataFile.h"
#include "String_.h"
#include "Mry.h"
#include "ObVal.h"




ObVal_t*  (ObVal_New)(void)
{
  ObVal_t* obval = (ObVal_t*) Mry_New(ObVal_t) ;
  
  /* Allocation of space for the name of unknown */
  {
    char* name = (char*) Mry_New(char,ObVal_MaxLengthOfKeyWord) ;
  
    ObVal_SetNameOfUnknown(obval,name) ;
  }
  
  /* Initialization */
  ObVal_SetValue(obval,-1) ; /* arbitrary negative */
  ObVal_SetTypeToAbsolute(obval) ;
  ObVal_SetRelaxationFactor(obval,1) ;
  
  return(obval) ;
}



void (ObVal_Delete)(void* self)
{
  ObVal_t* obval = (ObVal_t*) self ;

  if(obval){
    char* name = ObVal_GetNameOfUnknown(obval);

    if(name){
      Mry_Free(name) ;
      ObVal_SetNameOfUnknown(obval,NULL) ;
    }
  }
}



void  (ObVal_Scan)(ObVal_t* obval,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  char name[ObVal_MaxLengthOfKeyWord] ;
  char type[ObVal_MaxLengthOfKeyWord] = "Absolute" ;
  double v ;
  double r = 0 ;

  /* Unknown and value */
  {
    int n = String_Scan(line," %s = %le",name,&v) ;
        
    if(!n) {
      arret("ObVal_Scan: no unknown") ;
    }
  }
  
  line = String_GetAdvancedPosition ;
  
  /* Absolute or relative? */
  {
    int n = String_Scan(line," %s",type) ;
  }
  
  /* Relaxation factor (if any) */
  {
    int n = String_FindAndScanExp(line,"Relax",","," = %le",&r) ;
  }

  ObVal_Set(obval,name,v,type,r);
}