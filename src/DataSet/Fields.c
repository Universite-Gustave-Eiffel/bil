#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "DataFile.h"
#include "Mry.h"
#include "Fields.h"
#include "Field.h"

/*
static void   lit_grille(FieldGrid_t* ,int,char*) ;
*/
//static void           Field_ReadGrid(FieldGrid_t*,int,char*) ;



Fields_t* (Fields_New)(void)
{
  Fields_t* fields = (Fields_t*) Mry_New(Fields_t) ;
  
  Fields_SetNbOfFields(fields,0) ;
    
  {
    Field_t* field = Mry_Create(Field_t,Fields_MaxNbOfFields,Field_New()) ;
      
    Fields_SetField(fields,field) ;
  }
  
  return(fields) ;
}


#if 0
Fields_t* (Fields_Create)(DataFile_t* datafile)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"CHMP,FLDS,Fields",",") ;
  size_t n_fields = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  Fields_t* fields  = Fields_New() ;
    
  {
    int i = String_ToInt(c) ;

    if(i <= 0) return(fields) ;
  }
  
  Message_Direct("Enter in %s","Fields") ;
  Message_Direct("\n") ;

  {    
    c = String_SkipLine(c) ;
    
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
    
    /* Read the fields */
    Fields_SetNbOfFields(fields,n_fields) ;
    for(size_t i = 0 ; i < n_fields ; i++) {
      Field_t* field = Fields_GetField(fields) + i ;
      
      Message_Direct("Enter in %s %lu","Field",i+1) ;
      Message_Direct("\n") ;
      
      Field_Scan(field,datafile) ;
    }
  }
  
  return(fields) ;
}
#else
Fields_t* (Fields_Create)(DataFile_t* datafile)
{
  Fields_t* fields  = Fields_New() ;

  Fields_Scan(fields,datafile);
  
  return(fields) ;
}
#endif


void (Fields_Scan)(Fields_t* fields,DataFile_t* datafile)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"CHMP,FLDS,Fields",",") ;
  size_t n_fields = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
    
  {
    int i = String_ToInt(c) ;

    if(i <= 0) return ;
  }
  
  Message_Direct("Enter in %s","Fields") ;
  Message_Direct("\n") ;

  {    
    c = String_SkipLine(c) ;
    
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
    
    /* Read the fields */
    Fields_SetNbOfFields(fields,n_fields) ;
    for(size_t i = 0 ; i < n_fields ; i++) {
      Field_t* field = Fields_GetField(fields) + i ;
      
      Message_Direct("Enter in %s %lu","Field",i+1) ;
      Message_Direct("\n") ;
      
      Field_Scan(field,datafile) ;
    }
  }
  
  return ;
}



void (Fields_Delete)(void* self)
{
  Fields_t* fields = (Fields_t*) self ;

  if(fields) {
    Field_t* field = Fields_GetField(fields) ;

    if(field) {
      size_t n = Fields_GetCapacity(fields) ;

      Mry_Delete(field,n,Field_Delete) ;
      Mry_Free(field) ;
      Fields_SetField(fields,NULL) ;
    }
  }
}
