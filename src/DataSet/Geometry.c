#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include "Geometry.h"
#include "DataFile.h"
#include "Message.h"
#include "Periodicities.h"
#include "Mry.h"
#include "String_.h"

#include <algorithm>
#include <cctype>


/* Global functions */

Geometry_t*  (Geometry_New)(void)
{
  Geometry_t* geom = (Geometry_t*) Mry_New(Geometry_t) ;
  
  Geometry_SetDimension(geom,3);
  Geometry_SetNoSymmetry(geom);

  {
    Periodicities_t* periodicities = Periodicities_New();

    Geometry_SetPeriodicities(geom,periodicities);
  }
  
  return(geom) ;
}




void (Geometry_Delete)(void* self)
{
  Geometry_t* geom = (Geometry_t*) self ;
  
  if(geom) {
    Periodicities_t* periodicities = Geometry_GetPeriodicities(geom) ;
    
    if(periodicities) {
      Periodicities_Delete(periodicities) ;
      Mry_Free(periodicities) ;
      Geometry_SetPeriodicities(geom,nullptr) ;
    }
  }
}


#if 0
Geometry_t*  (Geometry_Create)(DataFile_t* datafile)
{
  Geometry_t* geom = Geometry_New() ;
  
  
  Message_Direct("Enter in %s","Geometry") ;
  Message_Direct("\n") ;
  
  {
    unsigned short int dim = 3 ;
    char* filecontent = DataFile_GetFileContent(datafile) ;
    char* c  = String_FindToken(filecontent,"DIME,GEOM,Geometry",",") ;
    char* line = String_SkipLine(c) ;
  
    if(line) line = String_FindAnyChar(line,"0123") ;
  
    if(line) {
      dim  = (unsigned short int) strtoul(line,NULL,10) ;
    }
  
    Geometry_SetDimension(geom,dim);
    
    if(line) line = String_SkipAnyChars(line,"0123456789 ") ;
  
    /* The symmetry */

    if(dim > 0 && dim < 3) {
      char* pline = line ;
      char sym[TextFile_MaxLengthOfTextLine] ;
      int n = String_Scan(line,"%s",sym) ;
      std::string sym_str(sym);

      if(n) {
        Geometry_Set(geom,dim,sym_str) ;
      }
    
      #if 0
      if(String_CaseIgnoredIs(pline,"plane",4))  {
        
        Geometry_SetPlaneSymmetry(geom) ;
        
      } else if(String_CaseIgnoredIs(pline,"axis",4))  {
        
        Geometry_SetCylindricalSymmetry(geom) ;
        
      } else if(String_CaseIgnoredIs(pline,"sphe",4))  {
        
        Geometry_SetSphericalSymmetry(geom) ;
        
      } else {
        
        Geometry_SetPlaneSymmetry(geom) ;
        
        Message_Warning("Geometry_Create: by default the symmetry is set to plane") ;
      }
      #endif
    }
  }
  
  Geometry_SetPeriodicities(geom,Periodicities_Create(datafile));
  
  return(geom) ;
}
#else
Geometry_t*  (Geometry_Create)(DataFile_t* datafile)
{
  Geometry_t* geom = Geometry_New() ;
  
  Geometry_Scan(geom,datafile);
  
  return(geom) ;
}
#endif



void  (Geometry_Scan)(Geometry_t* geom,DataFile_t* datafile)
{
  Message_Direct("Enter in %s","Geometry") ;
  Message_Direct("\n") ;
  
  {
    unsigned short int dim = 3 ;
    char* filecontent = DataFile_GetFileContent(datafile) ;
    char* c  = String_FindToken(filecontent,"DIME,GEOM,Geometry",",") ;
    char* line = String_SkipLine(c) ;
  
    if(line) line = String_FindAnyChar(line,"0123") ;
  
    if(line) {
      dim  = (unsigned short int) strtoul(line,NULL,10) ;
    }
  
    Geometry_SetDimension(geom,dim);
    
    if(line) line = String_SkipAnyChars(line,"0123456789 ") ;
  
    /* The symmetry */
    if(dim > 0 && dim < 3) {
      char* pline = line ;
      char sym[TextFile_MaxLengthOfTextLine] ;
      int n = String_Scan(line,"%s",sym) ;
      std::string sym_str(sym);

      if(n) {
        Geometry_Set(geom,dim,sym_str) ;
      }
    }
  }
  
  {
    Periodicities_t* periodicities = Geometry_GetPeriodicities(geom) ;

    Periodicities_Scan(periodicities,datafile);
  }
  
  return(geom) ;
}
