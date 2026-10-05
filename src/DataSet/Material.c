#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>
#include "Message.h"
#include "DataFile.h"
#include "Material.h"
#include "Curves.h"
#include "Mry.h"
#include "String_.h"
#include "Models.h"


/* Extern functions */


Material_t* (Material_New)(Materials_t* materials)
{
  Material_t* material   = (Material_t*) Mry_New(Material_t) ;

  Material_SetParentMaterials(material,materials) ;
  
    
  /* Allocation of memory space for the material */
  {
    Material_t* mat = material ;
    
    
    /* Allocation of space for the code name of the model */
    {
      char* name = (char*) Mry_New(char,Material_MaxLengthOfKeyWord) ;
      
      Material_SetCodeNameOfModel(mat,name) ;
    }
    
    /* The generic data */
    {
      Material_SetGenericData(mat,NULL) ;
    }
    
    /* The properties (also part of the generic data) */
    {
      double* pr = (double*) Mry_New(double,Material_MaxNbOfProperties) ;
    
      Material_SetNbOfProperties(mat,0) ;
      Material_SetProperty(mat,pr) ;
      
      Material_AppendData(mat,Material_MaxNbOfProperties,pr,"Parameters") ;
    }

    /* Curves */
    {
      Curves_t* curves = Curves_Create(Material_MaxNbOfCurves) ;
      
      Material_SetCurves(mat,curves) ;
    }
    
    /* for compatibility with former coding of Material_t structure */
    mat->nc = Material_GetNbOfCurves(mat) ;
    mat->cb = Material_GetCurve(mat) ;
    
    /* The method */
    {
      char* meth = (char*) Mry_New(char,Material_MaxLengthOfKeyWord) ;
      
      Material_SetMethod(mat,meth) ;
    }
  }
  
  return(material) ;
}



void (Material_Delete)(void* self)
{
  Material_t* material = (Material_t*) self ;
  
  {
    char* name = Material_GetCodeNameOfModel(material) ;
    
    if(name) {
      Mry_Free(name) ;
      Material_SetCodeNameOfModel(material,NULL) ;
    }
  }
  
  {
    Curves_t* curves = Material_GetCurves(material) ;
    
    if(curves) {
      Curves_Delete(curves) ;
      Mry_Free(curves) ;
      Material_SetCurves(material,NULL) ;
    }
  }
  
  {
    GenericData_t* gdat = Material_GetGenericData(material) ;
    
    if(gdat) {
      GenericData_Delete(gdat) ;
      Mry_Free(gdat) ;
      Material_SetGenericData(material,NULL) ;
    }
  }
  
  {
    char* meth = Material_GetMethod(material) ;

    if(meth) {
      Mry_Free(meth) ;
      Material_SetMethod(material,NULL) ;
    }
  }
}


#if 0
void (Material_Scan)(Material_t* mat,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  char  modelname[Material_MaxLengthOfKeyWord] ;
        
  /* Read and store the code name of the model */
  {
    char  codename[Material_MaxLengthOfKeyWord] ;
    char*  code = codename + 1 ;
    //int n = String_FindAndScanExp(line,"Model =,",","," %s",codename+1) ;
      
    if(String_Is(line,"Model",5)) {
      char* c = String_FindAndSkipToken(line,"=") ;
          
      String_ScanStringUntil(c,codename + 1,"(" String_SpaceChars) ;
      //String_Scan(line,"%*[^= ] = %s",codename + 1) ;
    } else {
      String_Scan(line,"%s",codename + 1) ;
    }
      
    if(isdigit(codename[1])) {
      codename[0] = 'm' ;
      code = codename ;
    }
      
    /* Code name of the model */
    strcpy(modelname,code) ;
    strcpy(Material_GetCodeNameOfModel(mat),code) ;
  }
      
      
  /* Find or append a model and point to it */
  {
    char*  code = Material_GetCodeNameOfModel(mat) ;
    Models_t* usedmodels = Material_GetUsedModels(mat) ;
    Model_t* matmodel = Models_FindOrAppendModel(usedmodels,code) ;
    size_t modind  = Models_FindModelIndex(usedmodels,code) ;
        
    Material_SetModel(mat,matmodel) ;
    Material_SetModelIndex(mat,modind) ;
  }
      
      
  /* Read and store the user-defined name of unknowns */
  {
    char* c = String_FindChar(line,'(') ;
        
    if(c) {
      Model_t* matmodel = Material_GetModel(mat) ;
      size_t neq = Model_GetNbOfEquations(matmodel) ;
          
      c[0] = ' ' ;
      for(size_t i = 0 ; i < neq ; i++) {
        char name[Material_MaxLengthOfKeyWord] ;
        char* c1 = String_FindAnyChar(c,",)\n") ;
            
        if(c1) {
          c1[0] = ' ' ;
          String_Scan(c,"%s",name) ;
          Model_CopyNameOfUnknown(matmodel,i,name) ;
          c = c1 ;
        } else {
          Message_FatalError("Material_Scan") ;
        }
      }
    }
  }
  /* Read and store the user-defined name of equations */
  {
    char* c = String_FindChar(line,'(') ;
    c = String_FindChar(c,'(') ;
        
    if(c) {
      Model_t* matmodel = Material_GetModel(mat) ;
      size_t neq = Model_GetNbOfEquations(matmodel) ;
          
      c[0] = ' ' ;
      for(size_t i = 0 ; i < neq ; i++) {
        char name[Material_MaxLengthOfKeyWord] ;
        char* c1 = String_FindAnyChar(c,",)\n") ;
            
        if(c1) {
          c1[0] = ' ' ;
          String_Scan(c,"%s",name) ;
          Model_CopyNameOfEquation(matmodel,i,name) ;
          c = c1 ;
        } else {
          Message_FatalError("Material_Scan") ;
        }
      }
    }
  }


  /* for compatibility with old version */
  {
    if(Material_GetModel(mat)) {
      mat->eqn = Material_GetNameOfEquation(mat) ;
      mat->inc = Material_GetNameOfUnknown(mat) ;
    }
  }


  /* Input material data */
  /* A model pointing to a null pointer serves to build curves only */
  {
    int n = Material_ReadProperties(mat,datafile) ;
    
    Material_SetNbOfProperties(mat,n) ;
  }
    
    
  /* for compatibility with old version */
  {
    if(Material_GetModel(mat)) {
      if(Material_GetNbOfEquations(mat) == 0) {
        Material_SetNbOfEquations(mat,mat->neq) ;
      }
    }
  }
  mat->nc = Material_GetNbOfCurves(mat) ;
    
    
  if(!Material_GetModel(mat)) {
    Message_FatalError("Material_Scan: Model not known") ;
  }
}
#else
void (Material_Scan)(Material_t* mat,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  char  modelname[Material_MaxLengthOfKeyWord] ;
  Model_t* matmodel ;
  int modind ;
        
  /* Read and store the code name of the model */
  {
    char  codename[Material_MaxLengthOfKeyWord] ;
    char*  code = codename + 1 ;
      
    if(String_Is(line,"Model",5)) {
      char* c = String_FindAndSkipToken(line,"=") ;
          
      String_ScanStringUntil(c,codename + 1,"(" String_SpaceChars) ;
      //String_Scan(line,"%*[^= ] = %s",codename + 1) ;
    } else {
      String_Scan(line,"%s",codename + 1) ;
    }
      
    if(isdigit(codename[1])) {
      codename[0] = 'm' ;
      code = codename ;
    }
      
    /* Code name of the model */
    strcpy(modelname,code) ;
  }

  /* Find or append a model and point to it */
  {
    Models_t* usedmodels = Material_GetUsedModels(mat) ;
    
    matmodel = Models_FindOrAppendModel(usedmodels,modelname) ;
    modind  = Models_FindModelIndex(usedmodels,modelname) ;
  }

  strcpy(Material_GetCodeNameOfModel(mat),modelname) ;
  Material_SetModel(mat,matmodel) ;
  Material_SetModelIndex(mat,modind) ;


  /* for compatibility with old version */
  if(Material_GetModel(mat)) {
    mat->eqn = Material_GetNameOfEquation(mat) ;
    mat->inc = Material_GetNameOfUnknown(mat) ;
  }


  /* Input material data */
  /* A model pointing to a null pointer serves to build curves only */
  {
    int n = Material_ReadProperties(mat,datafile) ;
    
    Material_SetNbOfProperties(mat,n) ;
  }
    
    
  /* for compatibility with old version */
  if(Material_GetModel(mat)) {
    if(Material_GetNbOfEquations(mat) == 0) {
      Material_SetNbOfEquations(mat,mat->neq) ;
    }
  }
  mat->nc = Material_GetNbOfCurves(mat) ;
    
    
  if(!Material_GetModel(mat)) {
    Message_FatalError("Material_Scan: Model not known") ;
  }
}
#endif



int  (Material_ReadProperties)(Material_t* material,DataFile_t* datafile)
{
  Model_t* model = Material_GetModel(material) ;
  
  if(model) {
    Model_ReadMaterialProperties_t* readmatprop = Model_GetReadMaterialProperties(model) ;
    
    if(readmatprop) {
      int n = readmatprop(material,datafile) ;
    
      if(n > Material_MaxNbOfProperties) {
        Message_RuntimeError("Material_ReadProperties: too many properties") ;
      }
    
      return(n) ;
    }
  }
  
  Material_ScanProperties(material,datafile,NULL) ;
  
  return(0) ;
}



void (Material_ScanProperties)(Material_t* mat,DataFile_t* datafile,int (*pm)(const char*))
/** Read the material properties in the string of the file content */
{
  int    nd = Material_GetNbOfProperties(mat) ;
  short int    cont = 1 ;
  
  if(!datafile) return;

  while(cont) {
    char   mot[Material_MaxLengthOfKeyWord] = {'\n'} ;
    char*  line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
    
    if(!line) break ;
    
    {
      //String_ScanStringUntil(line,mot,"=") ;
      //String_Scan(line,"%*[ ]%[^=]",mot) ;
      //sscanf(line," %*[ ]%[^=]",mot) ;
      int n = String_Scan(line," %[^= ]",mot) ;
      //String_Scan(line,"%*[ ]%[^=]",mot) ;
      
      if(n > Material_MaxLengthOfKeyWord) {
        arret("Material_ScanProperties: too many characters") ;
      }
    }

    /* Reading some curves */
    if(String_Is(mot,"Courbes",6) || String_Is(mot,"Curves",5)) {
      Curves_t* curves = Material_GetCurves(mat) ;
      
      Curves_ReadCurves(curves,line) ;
      
      if(Curves_GetNbOfCurves(curves) > Material_MaxNbOfCurves) {
        arret("Material_ScanProperties: too many curves") ;
      }

    /* Reading the method */
    } else if(String_Is(mot,"Method",6)) {
      char* p = String_FindChar(line,'=') ;
      
      if(p) {
        char* cr = String_FindChar(p,'\n') ;
        
        if(cr) *cr = '\0' ;
        
        p = String_FindAndSkipToken(p,"=") ;
        p = String_SkipBlankChars(p) ;
        //sscanf(p,"%s",Material_GetMethod(mat)) ;
        strcpy(Material_GetMethod(mat),p) ;
      }
      
    /* Reading the material properties and storing through pm */
    } else if(pm) {
      char* p = String_FindChar(line,'=') ;

      /* We assume that this is a property as long as "=" is found */
      if(p) {
        int i = (*pm)(mot) ;
        
        if(i >= 0) {
        
          String_Scan(p+1,"%lf",Material_GetProperty(mat) + i) ;
          nd = (nd > i + 1) ? nd : i + 1 ;
        
        } else {
        
          Message_RuntimeError("%s is not known",mot) ;
          
        }
      
      /* ... otherwise we stop reading */
      } else {
        
        /* go out */
        cont = 0 ;
      }
      
    } else {
      break ;
    }
    
  }

  Material_SetNbOfProperties(mat,nd) ;
  return ;
}