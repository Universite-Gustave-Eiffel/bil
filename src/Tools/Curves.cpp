#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>

#include "Message.h"
#include "Mry.h"
#include "Math_.h"
#include "CurvesFile.h"
#include "Curves.h"
#include "Curve.h"



/* Extern functions */

Curves_t* (Curves_Create)(int n_curves)
{
  Curves_t *curves   = (Curves_t*) Mry_New(Curves_t) ;
  
  Curves_GetNbOfAllocatedCurves(curves) = n_curves ;
  Curves_GetNbOfCurves(curves) = 0 ; /* Important initialization */
  
  {
    Curve_t* cv = (Curve_t*) Mry_New(Curve_t,n_curves) ;
    unsigned int    i ;
    
    for(i = 0 ; i < n_curves ; i++) {
      Curve_t* cvi   = (Curve_t*) Mry_New(Curve_t) ;
      
      cv[i] = cvi[0] ;
      free(cvi) ;
    }
    
    Curves_GetCurve(curves) = cv ;
  }
  
  return(curves) ;
}



void (Curves_Delete)(void* self)
{
  Curves_t* curves   = (Curves_t*) self ;
  
  {
    int n = Curves_GetNbOfAllocatedCurves(curves) ;
    Curve_t* curve = Curves_GetCurve(curves) ;
    
    if(curve) {
      int i ;
    
      for(i = 0 ; i < n ; i++) {
        Curve_t* cv = curve + i ;
      
        Curve_Delete(cv) ;
      }
    
      free(curve) ;
      Curves_GetCurve(curves) = NULL ;
    }
  }
}



int Curves_Append(Curves_t* curves, Curve_t* cv)
/** Append curves with cv 
 *  Return the index of the appended curve */
{
    int NbOfCurves = Curves_GetNbOfCurves(curves) ;
    Curve_t* curve = Curves_GetCurve(curves) ;
    
    if(cv) {
      if(Curves_CannotAppendCurves(curves,1)) {
        arret("Curves_Append") ;
      }
    
      curve[NbOfCurves] = cv[0] ;
    
      Curves_GetNbOfCurves(curves) += 1 ;
    }
    
    return(NbOfCurves) ;
}


int    Curves_FindCurveIndex(Curves_t* curves,const char* yname)
/** Find the curve position index whose y-axis name is pointed to by yname.
 *  Return -1 if the name is not found. */
{
  int n = Curves_GetNbOfCurves(curves) ;
  int    i ;

  if(isdigit(yname[0])) { /* numeric characters */
    i  = atoi(yname) - 1 ;
    
    if(i >= n) i = -1 ;
    
  } else {                /* alphanumeric characters */
    Curve_t* curve = Curves_GetCurve(curves) ;
    
    for(i = 0 ; i < n ; i++) {
      char* yyname = Curve_GetNameOfYAxis(curve + i) ;
      if(!strcmp(yname,yyname)) break ;
    }
    
    if(i == n) i = -1 ;
  }

  //if(i < 0) arret("Curves_FindCurveIndex(2): position not known") ;
  return(i) ;
}



Curve_t* Curves_FindCurve(Curves_t* curves,const char* label)
{
  int i = Curves_FindCurveIndex(curves,label) ;
  Curve_t* curve ;
    
  if(i < 0) {
    curve = NULL ;
  } else {
    curve = Curves_GetCurve(curves) + i ;
  }
  
  return(curve) ;
}



int (Curves_CreateDerivative)(Curves_t* curves,Curve_t* cv)
{
  Curve_t* dcv = Curve_CreateDerivative(cv) ;
  int i = Curves_Append(curves,dcv) ;
  
  free(dcv) ;
  return(i) ;
}



int (Curves_CreateIntegral)(Curves_t* curves,Curve_t* cv)
{
  Curve_t* icv = Curve_CreateIntegral(cv) ;
  int i = Curves_Append(curves,icv) ;
  
  free(icv) ;
  return(i) ;
}



int (Curves_CreateInverse)(Curves_t* curves,Curve_t* cv,const char sc)
{
  Curve_t* icv = Curve_CreateInverse(cv,sc) ;
  int i = Curves_Append(curves,icv) ;
  
  free(icv) ;
  return(i) ;
}




int   Curves_ReadCurves(Curves_t* curves,const char* dline)
/** Read new curves as defined in the filename found in dline 
 *  and append "curves" accordingly.
 *  Return the nb of curves that has been read */
{
  int    n_curves ;
  int    n_points ;
  CurvesFile_t* curvesfile = CurvesFile_Create() ;

  /* Is there a file name? */
  if(!CurvesFile_Initialize(curvesfile,dline)) {
    /* Create it if needed. */
    CurvesFile_GetCurves(curvesfile) = curves ;
    CurvesFile_WriteCurves(curvesfile) ;
    /* We reinitialize again for the file position starting input data*/
    /* CurvesFile_Initialize(curvesfile,dline) ; */
  }


  /* Nb of curves */
  n_curves = CurvesFile_GetNbOfCurves(curvesfile) ;


  /* Nb of points */
  n_points = CurvesFile_GetNbOfPoints(curvesfile) ;


  {    
    if(Curves_CannotAppendCurves(curves,n_curves)) {
      arret("Curves_ReadCurves(4): too many curves!") ;
    }
  }
  
  
  /* Allocate memory for the curves */
  {
    int i ;
    
    for(i = 0 ; i < n_curves ; i++) {
      Curve_t* cb_i = Curves_GetCurve(curves) + Curves_GetNbOfCurves(curves) + i ;
      Curve_t* cb   = Curve_Create(n_points) ;
    
      cb_i[0] = cb[0] ;
      free(cb) ;
    }
  }
  
  
  /* Read and store the names of x-axis and y-axis */
  {
    char *line ;
    
    CurvesFile_OpenFile(curvesfile,"r") ;
    
    do {
      
      line = CurvesFile_ReadLineFromCurrentFilePosition(curvesfile) ;
      
    } while((line) && (line[0] == '#') && (strncmp(line,"# Labels:",9) != 0)) ;
    
    if(strncmp(line,"# Labels:",9) == 0) {
      
      line += 9 ;
      
      /* Read the label of x-axis */
      line = strtok(line," ") ;
      
      if(line) {
        char xlabel[Curve_MaxLengthOfCurveName] ;
        char* c ;
        int i ;
        
        /* Save the label of x-axis */
        strcpy(xlabel,line) ;
            
        if((c = strchr(xlabel,'('))) c[0] = '\0' ;
      
        for(i = 0 ; i < n_curves ; i++) {
          Curve_t* cb_i = Curves_GetCurve(curves) + Curves_GetNbOfCurves(curves) + i ;
          char* xname = Curve_GetNameOfXAxis(cb_i) ;
          char* yname = Curve_GetNameOfYAxis(cb_i) ;
        
          /* Store the label of x-axis */
          strcpy(xname,xlabel) ;
        
          /* Read the label of y-axis */
          line = strtok(NULL," ") ;
        
          /* Store the label of y-axis */
          if(line) {
            strcpy(yname,line) ;
            
            if((c = strchr(yname,'('))) c[0] = '\0' ;
          }
        }
      }
    }
    
    CurvesFile_CloseFile(curvesfile) ;
  }


  /* Read the x-values and y-values */
  {
    double a_1,a_2 ;
    char *line ;
    FILE   *fict = CurvesFile_OpenFile(curvesfile,"r") ;
    int i ;
    
    do {
      CurvesFile_StoreFilePosition(curvesfile) ;
      
      line = CurvesFile_ReadLineFromCurrentFilePosition(curvesfile) ;
      
    } while((line) && (line[0] == '#')) ;
      
    CurvesFile_MoveToStoredFilePosition(curvesfile) ;
  
    /* Set position in the file */
    /* CurvesFile_MoveToFilePositionStartingInputData(curvesfile) ; */

  
    /* Read x and y */
    for(i = 0 ; i < n_points ; i++) {
      double x ;
      int j ;
      
      fscanf(fict,"%le",&x) ;
    
      if(i == 0) {
        a_1 = x ;
      } else {
        a_2 = x ;
      }
    
      for(j = 0 ; j < n_curves ; j++) {
        Curve_t* cb_j = Curves_GetCurve(curves) + Curves_GetNbOfCurves(curves) + j ;
        double* y = Curve_GetYValue(cb_j) + i ;
        char fmt[] = "%*[" CurvesFile_FieldDelimiters "] %le" ;
      
        fscanf(fict,fmt,y) ;
      }
    }
  
    /* The same first and last points for all the curves */
    for(i = 0 ; i < n_curves ; i++) {
      Curve_t *cb_i = Curves_GetCurve(curves) + Curves_GetNbOfCurves(curves) + i ;
    
      Curve_GetXRange(cb_i)[0] = a_1 ;
      Curve_GetXRange(cb_i)[1] = a_2 ;
    }
  
    CurvesFile_CloseFile(curvesfile) ;
  }


  /* Scale */
  {
    char   scale = CurvesFile_GetScaleType(curvesfile) ;
    int i ;

    for(i = 0 ; i < n_curves ; i++) {
      Curve_t *cb_i = Curves_GetCurve(curves) + Curves_GetNbOfCurves(curves) + i ;
      
      Curve_GetScaleType(cb_i) = scale ;
    }
  }
  
  Curves_GetNbOfCurves(curves) += n_curves ;
  
  CurvesFile_Delete(curvesfile) ;
  free(curvesfile) ;

  return(n_curves) ;
}
