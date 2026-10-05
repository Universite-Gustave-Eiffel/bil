#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "Mry.h"
#include "String_.h"
#include "DataFile.h"
#include "Curves.h"
#include "Curve.h"
#include "Function.h"



static int FunctionPiecewiseAffine_ReadInFile(FunctionPiecewiseAffine_t*,char*) ;


Function_t*  (Function_New)(void)
{
  Function_t* function = (Function_t*) Mry_New(Function_t) ;

  /* Allocation of space for the type of function */
  {
    char* type = (char*) Mry_New(char,Function_MaxLengthOfKeyWord) ;
    
    Function_SetType(function,type) ;
    strcpy(Field_GetType(function),"none") ;
  }

  Function_SetFunctionFormat(function,NULL) ;
  
  return(function) ;
}



void (Function_Delete)(void* self)
{
  Function_t* function = (Function_t*) self ;
  
  if(function) {
    void* functionfmt = Function_GetFunctionFormat(function) ;
    char* type = Function_GetType(function) ;
    
    if(functionfmt) {
      if(String_Is(type,"piecewiseaffine")) {
        FunctionPiecewiseAffine_Delete(functionfmt) ;
      }
      
      Mry_Free(functionfmt) ;
      Function_SetFunctionFormat(function,NULL) ;
    }

    if(type) {
      Mry_Free(type) ;
      Function_SetType(function,NULL) ;
    }
  }
}


FunctionPiecewiseAffine_t*  (FunctionPiecewiseAffine_New)(const size_t n)
{
  FunctionPiecewiseAffine_t* function = (FunctionPiecewiseAffine_t*) Mry_New(FunctionPiecewiseAffine_t) ;
    
  FunctionPiecewiseAffine_SetNbOfPoints(function,n) ;

  {
    double* x = (double*) Mry_New(double,n) ;
    
    FunctionPiecewiseAffine_SetXValue(function,x) ;
  }

  {
    double* f = (double*) Mry_New(double,n) ;
    
    FunctionPiecewiseAffine_SetFValue(function,f) ;
  }
  
  return(function) ;
}


#if 0
void (FunctionPiecewiseAffine_Delete)(void* self)
{
  FunctionPiecewiseAffine_t* function = (FunctionPiecewiseAffine_t*) self ;
  
  {
    double* x = FunctionPiecewiseAffine_GetXValue(function) ;
    
    if(x) {
      Mry_Free(x) ;
      FunctionPiecewiseAffine_SetXValue(function,NULL) ;
    }
  }
  
  {
    double* f = FunctionPiecewiseAffine_GetFValue(function) ;
    
    if(f) {
      Mry_Free(f) ;
      FunctionPiecewiseAffine_SetFValue(function,NULL) ;
    }
  }
}
#endif



void (Function_Scan)(Function_t* function,DataFile_t* datafile)
{
  char* cur = DataFile_GetCurrentPositionInFileContent(datafile) ;
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  char type[DataFile_MaxLengthOfTextLine] ;

  line = String_SkipBlankChars(line) ;

  /* The type (if given )*/
  {
    int n = String_FindAndScanExp(line,"Type",","," = %s",type) ;

    if(n) {
      if(strlen(type) > Function_MaxLengthOfKeyWord - 1) {
        arret("Function_Scan: too long type") ;
      }
    } else {
      /* Default type */
      strcpy(type,"piecewiseaffine");
    }
  }
  
  
  /* PiecewiseAffine function
   * ------------------------ */
  if(String_Is(type,"piecewiseaffine")) {
    std::string type_str = "piecewiseaffine";
    char* pline = String_FindToken(line,"N");

    if(!pline) pline = String_FindToken(line,"F");

    /* Read direct */
    if(String_Is(pline,"Ntimes",1)) {
      size_t n_tm = 0 ;
      int n = String_FindAndScanExp(line,"N",","," = %lu",&n_tm) ;
      std::vector<double> t_vec(n_tm);
      std::vector<double> f_vec(n_tm);
      
      /* Read the F(T) */
      if(n_tm > 0) {
        double* t = t_vec.data();
        double* f = f_vec.data();
        char* c = cur ;
        
        c = String_FindToken(c,"F") ;
        
        if(c) {
          
          String_ScanArrays(c,n_tm," F( %lf ) = %lf",t,f) ;
          
          c = String_GetAdvancedPosition ;
          
          DataFile_SetCurrentPositionInFileContent(datafile,c) ;
          
        } else {
          arret("Function_Scan: no key F() found") ;
        }
      }

      Function_Set(function,type_str,t_vec,f_vec) ;
      return;
    }

    /* Read in a file */
    if(String_Is(pline,"File",2)) {
      char name[Function_MaxLengthOfFileName] ;
      
      String_FindAndScanExp(line,"Fi",","," = %s",name) ;
      
      if(strlen(name) > Function_MaxLengthOfFileName) {
        arret("Function_Scan: too long file name") ;
      }
      
      {
        char* c = String_FindAndSkipToken(line,"=") ;

        Function_Set(function,type_str,c) ;
      }
      return ;
    }

    arret("Function_Scan: keyword not known") ;
  }

  arret("Function_Scan: type not known") ;
  return;
}



int (FunctionPiecewiseAffine_ReadInFile)(FunctionPiecewiseAffine_t* fn,char* line1)
/* Lecture des fonctions du temps dans le fichier "nom"
   retourne le nb de fonctions lues */
{
  int    n_points,n_fonctions ;
  char   line[Function_MaxLengthOfTextLine],*c ;
  FILE   *fict ;
  int long pos ;
  char nom[Function_MaxLengthOfFileName] ;
  
  String_Scan(line1," %s",nom) ;
      
  if(strlen(nom) > Function_MaxLengthOfFileName) {
    arret("FunctionPiecewiseAffine_ReadInFile: too long file name") ;
  }

  fict = fopen(nom,"r") ;
  
  if(fict) {
    /* nb de fonctions */
    n_fonctions = 0 ;
    
    do {
      fgets(line,sizeof(line),fict) ;
      c = (char*) strtok(line," \n") ;
      /* } while(c != NULL && *c == '#' && !feof(fict)) ; */
    } while((c == NULL || *c == '#') && !feof(fict)) ;
    
    while((char*) strtok(NULL," \n") != NULL) n_fonctions++ ;

    /* nb de points */
    n_points = 1 ;
    
    while(!feof(fict) && fgets(line,sizeof(line),fict)) {
      c = (char*) strtok(line," \n") ;
      if(c != NULL && *c != '#') n_points++ ;
    }

    /* reservation de la memoire */
    {    
      for(int i = 0 ; i < n_fonctions ;i++) {
        FunctionPiecewiseAffine_t* fct = FunctionPiecewiseAffine_New(n_points) ;
        
        fn[i] = fct[0] ;
        Mry_Free(fct) ;
      }
    }
  
    {
      /* positionnement dans le fichier */
      rewind(fict) ;
    
      do {
        pos = ftell(fict) ;
        fgets(line,sizeof(line),fict) ;
        c = (char*) strtok(line," \n") ;
        /* } while(c != NULL && *c == '#' && !feof(fict)) ; */
      } while((c == NULL || *c == '#') && !feof(fict)) ;
    
      fseek(fict,pos,SEEK_SET) ;
  
  
      /* Read time and function values */
      {
        int i ;
        
        for(i = 0 ; i < n_points ; i++) {
          double x ;
          int    j ;
    
          fscanf(fict,"%le",&x) ;
    
          for(j = 0 ; j < n_fonctions ; j++) {
            double* t = FunctionPiecewiseAffine_GetXValue(fn + j) ;
            double* f = FunctionPiecewiseAffine_GetFValue(fn + j) ;
            
            t[i] = x ;
            fscanf(fict,"%le",f + i) ;
          }
        }
      }
    }
    
  } else {
    int nbofcurves = 10 ;
    Curves_t* curves = Curves_Create(nbofcurves) ;
    Curve_t*  curve  = Curves_GetCurve(curves) ;
    
    Curves_ReadCurves(curves,line1) ;
    
    n_fonctions = Curves_GetNbOfCurves(curves) ;
    
    n_points = Curve_GetNbOfPoints(curve) ;
    
    //arret("FunctionPiecewiseAffine_ReadInFile: unable to open the file") ;

    /* reservation de la memoire */
    {
      int i ;
    
      for(i = 0 ; i < n_fonctions ;i++) {
        FunctionPiecewiseAffine_t* fct = FunctionPiecewiseAffine_New(n_points) ;
        
        fn[i] = fct[0] ;
        Mry_Free(fct) ;
      }
    }
  
    {
      double* t = Curve_CreateSamplingOfX(curve) ;
      int i ;
      
      /* Read time and function values */
      for(i = 0 ; i < n_points ; i++) {
        int    j ;
    
        for(j = 0 ; j < n_fonctions ; j++) {
          Curve_t* curve_j = curve + j ;
          double* y = Curve_GetYValue(curve_j) ;
          double* x = FunctionPiecewiseAffine_GetXValue(fn + j) ;
          double* f = FunctionPiecewiseAffine_GetFValue(fn + j) ;
          
          x[i] = t[i] ;
          f[i] = y[i] ;
        }
      }
      
      Mry_Free(t) ;
    }
    
    Curves_Delete(curves) ;
    Mry_Free(curves) ;
  }

  fclose(fict) ;

  return(n_fonctions) ;
}
