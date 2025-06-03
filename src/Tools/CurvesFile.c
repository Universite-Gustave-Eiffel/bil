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
#include "Buffer.h"
#include "Curves.h"
#include "Curve.h"
#include "String_.h"
#include "CurvesFile.h"
#include "InternationalSystemOfUnits.h"
#include "Expression.h"


/* Shorthands of some units */
#define dm    (0.1*InternationalSystemOfUnits_OneMeter)
#define cm    (0.01*InternationalSystemOfUnits_OneMeter)
#define dm3   (dm*dm*dm)
#define cm3   (cm*cm*cm)



typedef double GenericFunction_t(double,va_list) ;

/*
static void  (CurvesFile_StoreFilePosition)(CurvesFile_t*) ;
*/
static void  (CurvesFile_PasteXaxisColumn)(CurvesFile_t*,const char*,double,double) ;
static void  (CurvesFile_PasteYaxisColumn)(CurvesFile_t*,const char*,const char*,int,GenericFunction_t*,...) ;



static GenericFunction_t Langmuir ;
static GenericFunction_t LangmuirN ;
static GenericFunction_t Freundlich ;
static GenericFunction_t MualemVanGenuchten ;
static GenericFunction_t VanGenuchten ;
static GenericFunction_t NavGenuchten ;
static GenericFunction_t Mualem_dry ;
static GenericFunction_t Mualem_gas ;
static GenericFunction_t Millington ;
static GenericFunction_t MonlouisBonnaire ;
static GenericFunction_t CSH3EndMembers ;
static GenericFunction_t CSHLangmuirN ;
static GenericFunction_t RedlichKwongCO2 ;
static GenericFunction_t FenghourCO2 ;
static GenericFunction_t FromCurve ; 
static GenericFunction_t Evaluate ; 
static GenericFunction_t Expressions ; 
static GenericFunction_t MolarDensityOfPerfectGas ;
static GenericFunction_t MolarDensityOfCO2 ;
static GenericFunction_t Affine ;
static GenericFunction_t VanGenuchten_gas ;
static GenericFunction_t PermeabilityCoefficient_KozenyCarman ;
static GenericFunction_t PermeabilityCoefficient_VermaPruess ;
static GenericFunction_t TortuosityToLiquid_OhJang ;
static GenericFunction_t TortuosityToLiquid_BazantNajjar ;
static GenericFunction_t LinLee ;
static GenericFunction_t IdealWaterActivity ;
static GenericFunction_t PartialFugacityOfCO2CH4Mixture ;
static GenericFunction_t MolarVolumeOfCO2CH4Mixture ;


static double (vangenuchten)(double,double) ;
static double (fraction_Silica)(double*,double*,int,double) ;
static double (MolarDensityOfCO2_RedlichKwong)(double,double) ;
static double (ViscosityOfCO2_Fenghour)(double,double) ;
static double (langmuir)(double,double,double,double) ;
static double (MolarVolumeOfCO2CH4Mixture_RedlichKwong)(double,double,double) ;
static double (PartialFugacityOfCO2CH4Mixture_RedlichKwong)(double,double,double,const char*) ;
static double (RedlichKwong)(double,double,double,double) ;



CurvesFile_t*   (CurvesFile_Create)(void)
{
  CurvesFile_t* curvesfile   = (CurvesFile_t*) Mry_New(CurvesFile_t) ;
  

  /* Memory space for textfile */
  {
    TextFile_t* textfile = TextFile_Create(NULL) ;
    
    CurvesFile_GetTextFile(curvesfile) = textfile ;
  }
  
  
  /* Memory space for the file position */
  /*
  {
    fpos_t* pos = (fpos_t*) malloc(sizeof(fpos_t)) ;
    
    if(!pos) {
      arret("CurvesFile_Create(3)") ;
    }
    CurvesFile_GetFilePositionStartingInputData(curvesfile) = pos ;
  }
  */
  
  
  /* Memory space for the text line to be read */
  {
    int n = CurvesFile_MaxLengthOfTextLine ;
    char* line = (char*) Mry_New(char[n]) ;
    
    CurvesFile_GetTextLine(curvesfile) = line ;
  }
  
  
  /* Initialization */
  CurvesFile_GetScaleType(curvesfile) = 'n' ;
  CurvesFile_GetNbOfCurves(curvesfile) = 0 ;
  CurvesFile_GetNbOfPoints(curvesfile) = 0 ;
  CurvesFile_GetCommandLine(curvesfile) = NULL ;
  CurvesFile_GetCurrentPositionInTheCommandLine(curvesfile) = NULL ;
  
  
  return(curvesfile) ;
}



void (CurvesFile_Delete)(void* self)
{
  CurvesFile_t* curvesfile = (CurvesFile_t*) self ;
  
  {
    TextFile_t* textfile = CurvesFile_GetTextFile(curvesfile) ;
    
    TextFile_Delete(textfile) ;
    free(textfile) ;
  }
  
  free(CurvesFile_GetTextLine(curvesfile)) ;
}



#ifdef NOTDEFINED
void (CurvesFile_MoveToFilePositionStartingInputData)(CurvesFile_t* curvesfile)
/** Set the file position of the stream to the beginning of the input data. */
{
  FILE *str  = CurvesFile_GetFileStream(curvesfile) ;
  fpos_t* pos = CurvesFile_GetFilePositionStartingInputData(curvesfile) ;
  
  /* Set the file position of the stream to the stored position */
  if(fsetpos(str,pos)) {
    arret("CurvesFile_MoveToFilePositionStartingInputData") ;
  }
}
#endif


int   (CurvesFile_Initialize)(CurvesFile_t* curvesfile,const char* cmdline)
/** Initialize the curvesfile from the command line "cmdline" 
 *  Return the nb of curves found in the file */
{
  /* The command line */
  CurvesFile_GetCommandLine(curvesfile) = cmdline ;
  
  /* Read the scale */
  {
    char  scale = 'n' ;
    const char* line = cmdline ;
    
    if(strstr(line,"_log")) scale = 'l' ;
    
    CurvesFile_GetScaleType(curvesfile) = scale ;
  }


  /* Read the file name */
  {
    char* filename = CurvesFile_GetFileName(curvesfile) ;

    {
      const char* line = strchr(cmdline,'=') ;
      
      if(line) {
        line += 1 ;
        
        sscanf(line," %s",filename) ;
    
        if(strlen(filename) > CurvesFile_MaxLengthOfFileName) {
          arret("CurvesFile_Initialize(1)") ;
        }
      
        line  = strstr(line,filename) + strlen(filename) ;
        CurvesFile_GetCurrentPositionInTheCommandLine(curvesfile) = line ;
      }
    }
  }
  
  
  /* Does this file exist? If not return 0 */
  {
    if(CurvesFile_DoesNotExist(curvesfile)) {
      return(0) ;
    }
  }
  

  /* Position starting the input data */
  /*
  {
    char *line ;
    
    CurvesFile_OpenFile(curvesfile,"r") ;
    
    do {
      CurvesFile_StoreFilePosition(curvesfile) ;
      
      line = CurvesFile_ReadLineFromCurrentFilePosition(curvesfile) ;
      
    } while((line) && (line[0] == '#')) ;
      
    {
      fpos_t* pos = CurvesFile_GetFilePositionStartingInputData(curvesfile) ;

      *pos = *CurvesFile_GetFilePosition(curvesfile) ;
    }
    
    CurvesFile_CloseFile(curvesfile) ;
  }
  */
  
  

  /* Nb of curves */
  {
    CurvesFile_OpenFile(curvesfile,"r") ;
  
    {
      int n_curves = 0 ;
      char *line ;
      
      /* We skip the commented lines */
      do {
        line = CurvesFile_ReadLineFromCurrentFilePosition(curvesfile) ;
      } while((line) && (line[0] == '#')) ;
      
      /* We count the nb of curves in the first non-commented line */
      if(line) {
        char FS[] = CurvesFile_FieldDelimiters"\n" ;
        
        strtok(line,FS) ;
      
        while((char *) strtok(NULL,FS) != NULL) n_curves++ ;
      }
    
      CurvesFile_GetNbOfCurves(curvesfile) = n_curves ;
    }
    
    CurvesFile_CloseFile(curvesfile) ;
  }


  /* Nb of points */
  {
    CurvesFile_OpenFile(curvesfile,"r") ;
    
    {
      int n_points = 0 ;
      char *line ;
      
      do {
        line = CurvesFile_ReadLineFromCurrentFilePosition(curvesfile) ;
        
        /* We count the nb of non-commented lines */
        if((line) && line[0] != '#') n_points++ ;
        
      } while(line) ;
    
      CurvesFile_GetNbOfPoints(curvesfile) = n_points ;
    }
    
    CurvesFile_CloseFile(curvesfile) ;
  }
  
  return(CurvesFile_GetNbOfCurves(curvesfile)) ;
}




int   (CurvesFile_WriteCurves)(CurvesFile_t* curvesfile)
/** Write curves in the file stored in curvesfile as discrete data
 *  Return the number of curves that has been writen */
{
  const char*  cmdline = CurvesFile_GetCommandLine(curvesfile) ;
  char   models[CurvesFile_MaxNbOfCurves+1][CurvesFile_MaxLengthOfKeyWord] ;
  char   labels[CurvesFile_MaxNbOfCurves+1][CurvesFile_MaxLengthOfKeyWord] ;
  const char*  line  = CurvesFile_GetCurrentPositionInTheCommandLine(curvesfile) ;
  char*  xmodel = models[0] ;
  char*  xlabel = labels[0] ;
  int    acol ;
  int    ycol ;


  /* Write the first column x-axis */
  
  /* Read the range and the nb of points */
  if(line && sscanf(line," %s = %[^({]",xlabel,xmodel) == 2) {
      
    line = strchr(line,'{') ;
      
    if(String_Is(xmodel,"Range")) {
      int    n_points ;
      double x_1,x_2 ;
        
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf , %*[^= ] = %d }",&x_1,&x_2,&n_points) ;
      
      CurvesFile_GetNbOfPoints(curvesfile) = n_points ;

      CurvesFile_PasteXaxisColumn(curvesfile,xlabel,x_1,x_2) ;
        
    } else {
      arret("CurvesFile_WriteCurves(1): The first key-word is not \"Range\"") ;
      return(0) ;
    }
      
    line = strchr(line,'}') + 1 ;
      
  } else {
    char* filename = CurvesFile_GetFileName(curvesfile) ;
    
    arret("CurvesFile_WriteCurves(2): no data to build \"%s\"",filename) ;
    return(0) ;
  }


  /* Write other columns y-axis */
  
#define YLABEL  (labels[ycol - 1])
#define YMODEL  (models[ycol - 1])
#define ALABEL  (labels[acol - 1])
    
  /* We initialize for the first next columns */
  ycol    = 2 ;
  
  /* Read the key-word of the curve */
  while(sscanf(line," %s = %[^(](%d)",YLABEL,YMODEL,&acol) == 3) {
    
#define PasteColumn(a,...) \
        CurvesFile_PasteYaxisColumn(curvesfile,YLABEL,#a,acol,a,__VA_ARGS__)

    
    line = strchr(line,'{') ;

    /* Write a new column depending on the model name */
    if(String_Is(YMODEL,"Freundlich")) {
      double alpha,beta ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf }",&alpha,&beta) ;

      PasteColumn(Freundlich,alpha,beta) ;
      
    } else if(String_Is(YMODEL,"Affine")) {
      double a,b ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf }",&a,&b) ;

      PasteColumn(Affine,a,b) ;
      
    } else if(String_Is(YMODEL,"Langmuir")) {
      double c0,ca_max ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf }",&ca_max,&c0) ;

      PasteColumn(Langmuir,ca_max,c0) ;
      
    } else if(String_Is(YMODEL,"LangmuirN")) {
      double n,x0,y0 ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf , %*[^= ] = %lf }",&y0,&x0,&n) ;

      PasteColumn(LangmuirN,y0,x0,n) ;
      
    } else if(String_Is(YMODEL,"Mualem_wet") || String_Is(YMODEL,"Mualem_liq")){
      double m ;
      
      sscanf(line,"{ %*[^= ] = %lf }",&m) ;
      
      PasteColumn(MualemVanGenuchten,m) ;
      
    } else if(String_Is(YMODEL,"Mualem_dry")){
      double m_w,m_d,a_d,a_w ;
      FILE   *ftmp1 = tmpfile() ;
      double kh_max ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf , %*[^= ] = %lf , %*[^= ] = %lf }",&a_w,&m_w,&a_d,&m_d) ;

      /* Compute kh in the temporary file ftmp1 */
      {
#define MAX_COLX  5
#define FCOLX(i) (frmt + 4*(MAX_COLX + 1 - i))
        char   frmt[] = "%*s %*s %*s %*s %*s %lf" ;
        char   ltmp[CurvesFile_MaxLengthOfTextLine] ;
        double kh = 0. ;
        double p ;
        FILE *ftmp = CurvesFile_FileStreamCopy(curvesfile) ;
        
        do {
          fgets(ltmp,sizeof(ltmp),ftmp) ;
        } while(ltmp[0] == '#') ;
  
        sscanf(ltmp,FCOLX(acol),&p) ;
        fprintf(ftmp1,"%e %e\n",p,kh) ;
  
        while(fgets(ltmp,sizeof(ltmp),ftmp)) {
          if(ltmp[0] != '#') {
            double pn = p ;
            
            sscanf(ltmp,FCOLX(acol),&p) ;
            
            {
              double dp  = p - pn ;
              double s_d = vangenuchten(pn/a_d,m_d) ;
              double s_w = vangenuchten(pn/a_w,m_w) ;
              double hn = (s_d - s_w)/(1. - s_w) ;
              double h,dh ;
        
              s_d = vangenuchten(p/a_d,m_d) ;
              s_w = vangenuchten(p/a_w,m_w) ;
              h   = (s_d - s_w)/(1. - s_w) ;
        
              dh  = h - hn ;
              kh += dh/(p - 0.5*dp) ;
        
              fprintf(ftmp1,"%e %e\n",p,kh) ;
            }
          }
        }
        
        kh_max = kh ;
        fclose(ftmp) ;
#undef FCOLX
#undef MAX_COLX
      }
      
      rewind(ftmp1) ;
      
      PasteColumn(Mualem_dry,a_w,m_w,a_d,m_d,kh_max,ftmp1) ;
      
      fclose(ftmp1) ;
      
    } else if(String_Is(YMODEL,"Mualem_gas")){
      double m ;
      
      sscanf(line,"{ %*[^= ] = %lf }",&m) ;
      
      PasteColumn(Mualem_gas,m) ;
      
    } else if(String_Is(YMODEL,"Van-Genuchten_gas")){
      double m,p,q ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf , %*[^= ] = %lf }",&m,&p,&q) ;
      
      PasteColumn(VanGenuchten_gas,m,p,q) ;
      
    } else if(String_Is(YMODEL,"Van-Genuchten")){
      double a,m ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf }",&a,&m) ;
      
      PasteColumn(VanGenuchten,a,m) ;
      
    } else if(String_Is(YMODEL,"Nav-Genuchten")){
      double a,m ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf }",&a,&m) ;
      
      PasteColumn(NavGenuchten,a,m) ;
      
    } else if(String_Is(YMODEL,"Millington")){
      double b ;
      
      sscanf(line,"{ %*[^= ] = %lf }",&b) ;
      
      PasteColumn(Millington,b) ;
      
    } else if(String_Is(YMODEL,"Monlouis-Bonnaire")){
      double m ;
      
      sscanf(line,"{ %*[^= ] = %lf }",&m) ;
      
      PasteColumn(MonlouisBonnaire,m) ;
      
    } else if(String_Is(YMODEL,"Integral")){
      char   yname[Curve_MaxLengthOfCurveName] ;
      
      sscanf(line,"{ %*[^= ] = %s }",yname) ;
      
      {
        Curves_t* curves = CurvesFile_GetCurves(curvesfile) ;
        /* Curve to be integrated */
        Curve_t* crvi = Curves_FindCurve(curves,yname) ;
        /* We create the integral */
        Curve_t* crvj = Curve_CreateIntegral(crvi) ;
        
        PasteColumn(FromCurve,crvj) ;
        
        /* Free memory */
        Curve_Delete(crvj) ;
        free(crvj) ;
      }
      
    } else if(String_Is(YMODEL,"Evaluate")){
      char    expr[CurvesFile_MaxLengthOfTextLine] ;
      
      {
        const char* c = strchr(line,'{') ;
        
        strcpy(expr,c + 1) ;
      }
      
      {
        char* c = strchr(expr,'}') ;
        
        *c = '\0' ;
      }

      {
        Curves_t* curves = CurvesFile_GetCurves(curvesfile) ;
        
        PasteColumn(Evaluate,curves,expr,ALABEL) ;
      }
      
    } else if(String_Is(YMODEL,"Expressions")){
      char    expr[CurvesFile_MaxLengthOfTextLine] ;
      
      {
        const char* c = strchr(line,'{') ;
        
        strcpy(expr,c + 1) ;
      }
      {
        char* c = strchr(expr,'}') ;
        
        *c = '\0' ;
      }
      
      {
        Curves_t* curves = CurvesFile_GetCurves(curvesfile) ;
        
        PasteColumn(Expressions,curves,expr,YLABEL,ALABEL) ;
      }
      
    } else if(String_Is(YMODEL,"CSH3Poles")){
      double  y_Jen = 0.9 ;
      double  y_Tob = 1.8 ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf }",&y_Tob,&y_Jen) ;
      
      if(!strncmp(YLABEL,"X_CSH",5)) {
        PasteColumn(CSH3EndMembers,y_Tob,y_Jen,"X_CSH") ;
      } else if(!strncmp(YLABEL,"Z_CSH",5)) {
        PasteColumn(CSH3EndMembers,y_Tob,y_Jen,"Z_CSH") ;
      } else if(!strncmp(YLABEL,"V_CSH",5)) {
        PasteColumn(CSH3EndMembers,y_Tob,y_Jen,"V_CSH") ;
      } else if(!strncmp(YLABEL,"S_SH",4)) {
        PasteColumn(CSH3EndMembers,y_Tob,y_Jen,"S_SH") ;
      } else {
        PasteColumn(CSH3EndMembers,y_Tob,y_Jen,"X_CSH") ;
        ycol += 1 ;
        strcpy(YLABEL,"Z_CSH") ;
        PasteColumn(CSH3EndMembers,y_Tob,y_Jen,"Z_CSH") ;
        ycol += 1 ;
        strcpy(YLABEL,"V_CSH") ;
        PasteColumn(CSH3EndMembers,y_Tob,y_Jen,"V_CSH") ;
        ycol += 1 ;
        strcpy(YLABEL,"S_SH") ;
        PasteColumn(CSH3EndMembers,y_Tob,y_Jen,"S_SH") ;
      }
      
    } else if(String_Is(YMODEL,"CSHLangmuirN")){
      double  x1,s1,n1 ;
      double  x2,s2,n2 ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf , %*[^= ] = %lf \
                   , %*[^= ] = %lf , %*[^= ] = %lf , %*[^= ] = %lf }" \
                   , &x1,&s1,&n1,&x2,&s2,&n2) ;
      
      if(!strncmp(YLABEL,"X_CSH",1)) {
        PasteColumn(CSHLangmuirN,x1,s1,n1,x2,s2,n2,"X_CSH") ;
      } else if(!strncmp(YLABEL,"S_SH",1)) {
        PasteColumn(CSHLangmuirN,x1,s1,n1,x2,s2,n2,"S_SH") ;
      }
      
    } else if(String_Is(YMODEL,"Redlich-Kwong_CO2")) {
      double temperature ;
      
      sscanf(line,"{ %*[^= ] = %lf }",&temperature) ;
      
      PasteColumn(RedlichKwongCO2,temperature) ;
      
    } else if(String_Is(YMODEL,"MolarDensityOfCO2")) {
      double temperature ;
      
      sscanf(line,"{ %*[^= ] = %lf }",&temperature) ;
      
      PasteColumn(MolarDensityOfCO2,temperature) ;
      
    } else if(String_Is(YMODEL,"MolarVolumeOfCO2CH4Mixture")) {
      double pressure ;
      double temperature ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf }",&pressure,&temperature) ;
      
      PasteColumn(MolarVolumeOfCO2CH4Mixture,pressure,temperature) ;
      
    } else if(String_Is(YMODEL,"PartialFugacityOfCO2CH4Mixture")) {
      double pressure ;
      double temperature ;
      char gas[4] ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf , %*[^= ] = %s }",&pressure,&temperature,gas) ;
      
      PasteColumn(PartialFugacityOfCO2CH4Mixture,pressure,temperature,gas) ;
      
    } else if(String_Is(YMODEL,"MolarDensityOfPerfectGas")) {
      double temperature ;
      
      sscanf(line,"{ %*[^= ] = %lf }",&temperature) ;
      
      PasteColumn(MolarDensityOfPerfectGas,temperature) ;
      
    } else if(String_Is(YMODEL,"Fenghour_CO2") || \
              String_Is(YMODEL,"ViscosityOfCO2")) {
      double temperature ;
      
      sscanf(line,"{ %*[^= ] = %lf }",&temperature) ;
      
      PasteColumn(FenghourCO2,temperature) ;
      
    } else if(String_Is(YMODEL,"KozenyCarman")) {
      double phi0 ;
      
      sscanf(line,"{ %*[^= ] = %lf }",&phi0) ;
      
      PasteColumn(PermeabilityCoefficient_KozenyCarman,phi0) ;
      
    } else if(String_Is(YMODEL,"VermaPruess")) {
      double phi0 ;
      double frac ;
      double phi_r ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %lf , %*[^= ] = %lf }",&phi0,&frac,&phi_r) ;
      
      PasteColumn(PermeabilityCoefficient_VermaPruess,phi0,frac,phi_r) ;
      
    } else if(String_Is(YMODEL,"OhJang")) {
      
      PasteColumn(TortuosityToLiquid_OhJang,0) ;
      
    } else if(String_Is(YMODEL,"BazantNajjar")) {
      
      PasteColumn(TortuosityToLiquid_BazantNajjar,0) ;
      
    } else if(String_Is(YMODEL,"LinLee")) {
      double tem ;
      char salt[30] ;
      
      sscanf(line,"{ %*[^= ] = %lf , %*[^= ] = %s }",&tem,salt) ;
      
      PasteColumn(LinLee,tem,salt) ;
      
    } else {
      arret("CurvesFile_WriteCurves : fonction non connue") ;
  
    }
    
    line = strchr(line,'}') + 1 ;
    
    /* Increment for the next column */
    ycol += 1 ;
    if(ycol > CurvesFile_MaxNbOfCurves) {
      arret("CurvesFile_WriteCurves: too many curves") ;
    }
    
#undef PasteColumn
  }

#undef YLABEL
#undef YMODEL
#undef ALABEL
  
  return(CurvesFile_GetNbOfCurves(curvesfile)) ;
}



/* Intern functions */

void (CurvesFile_PasteXaxisColumn)(CurvesFile_t* curvesfile, const char *xlabel,double x_1,double x_2)
{
  FILE *fic = CurvesFile_OpenFile(curvesfile,"w") ;
  
  if(!fic) arret("CurvesFile_PasteXaxisColumn(1)") ;

  /* The first lines must be commented. At least two commented lines:
   * # Models: The name of models used to compute the columns 
   * # Labels: The labels of the columns */
  fprintf(fic,"# Models: X-axis(1)\n") ;
  fprintf(fic,"# Labels: %s(1)\n",xlabel) ;

  {
    char  scale = CurvesFile_GetScaleType(curvesfile) ;
    int n_points = CurvesFile_GetNbOfPoints(curvesfile) ;
    int i ;
    
    if(scale == 'l') {
      if(x_1 <= 0. || x_2 <= 0.) {
        arret("CurvesFile_PasteXaxisColumn(2)") ;
      }
    }
    
    /* Write the column x-axis */
    for(i = 0 ; i < n_points ; i++){
      int    n1 = n_points - 1 ;
      double n  = ((double) i)/n1 ;
      double x ;
      
      if(scale == 'l') {
        double ratio = x_2/x_1 ;
        
        x = x_1*pow(ratio,n) ;
        
      } else {
        double dx = (x_2 - x_1) ;
        
        x = x_1 + n*dx ;
      }
      
      fprintf(fic,"%e\n",x) ;   
    }
  }
  
  CurvesFile_CloseFile(curvesfile) ;
}


void (CurvesFile_PasteYaxisColumn)(CurvesFile_t* curvesfile,const char* ylabel,const char* model,int acol,GenericFunction_t* fct, ...)
/** Paste a new y-axis column in file "filename" by using the function "fct".
 *  acol   = is the column index to be used as the argument, ie y = fct(x) 
 *  model  = is the name of the model used as "fct"
 *  ylabel = is the label of the y-axis */
{
#define MAX_COLX  5
#define FCOLX(i) (frmt + 4*(MAX_COLX + 1 - i))
  char   frmt[] = "%*s %*s %*s %*s %*s %lf" ;
  int n_curves = CurvesFile_GetNbOfCurves(curvesfile) ;
    
  if(acol > MAX_COLX) {
    arret("CurvesFile_PasteYaxisColumn(1)") ;
  }

  {
    char   ltmp[CurvesFile_MaxLengthOfTextLine] ;
    FILE *ftmp = CurvesFile_FileStreamCopy(curvesfile) ;
    FILE *fic = CurvesFile_OpenFile(curvesfile,"w") ;
    
    if(!fic) {
      arret("CurvesFile_PasteYaxisColumn(2)") ;
    }
  
    while(fgets(ltmp,sizeof(ltmp),ftmp)) {
      
      if(ltmp[0] == '#') {
        /* Write the name of the model: "model" */
        if(!strncmp(ltmp,"# Models",8)) {
          char *c = strrchr(ltmp,'(') ;
          int n ;
          
          /* We read the number of columns */
          sscanf(c,"(%d)",&n) ;
          
          /* There is n-1 existing curves, so we increment */
          n_curves = n ;
    
          if(acol > n_curves) {
            arret("CurvesFile_PasteYaxisColumn(3)") ;
          }
          
          /* The index of this column is n + 1 */
          sprintf(strchr(ltmp,'\n')," %s(%d)\n",model,n + 1) ;
        
        /* Write the label of the y-axis: "ylabel" */
        } else if(!strncmp(ltmp,"# Labels",7)) {
          char *c = strrchr(ltmp,'(') ;
          int n ;
          
          sscanf(c,"(%d)",&n) ;
          
          n_curves = n ;
    
          if(acol > n_curves) {
            arret("CurvesFile_PasteYaxisColumn(4)") ;
          }
          
          sprintf(strchr(ltmp,'\n')," %s(%d)\n",ylabel,n + 1) ;
        }
        
      } else {
        double x,y ;
        va_list args ;

        va_start(args,fct) ;
      
        sscanf(ltmp,FCOLX(acol),&x) ;
        y = fct(x,args) ;
        sprintf(strchr(ltmp,'\n')," %e\n",y) ;
  
        va_end(args) ;
      }
      
      fprintf(fic,"%s",ltmp) ;
    }
    
    CurvesFile_CloseFile(curvesfile) ;
    fclose(ftmp) ;
  }
  
  CurvesFile_GetNbOfCurves(curvesfile) += 1 ;
  
  return ;

#undef FCOLX
#undef MAX_COLX
}






/* Predefined functions */
double (FromCurve)(double x,va_list args)
{
  Curve_t* curve = va_arg(args,Curve_t*) ;
  double y = Curve_ComputeValue(curve,x) ;
  
  return(y) ;
}


double (Evaluate)(double x,va_list args)
{
  Curves_t* curves = va_arg(args,Curves_t*) ;
  char*  expr = va_arg(args,char*) ;
  char*  xlabel = va_arg(args,char*) ;
  int NbOfCurves = Curves_GetNbOfCurves(curves) ;
  double y[10] = {0,0,0,0,0,0,0,0,0,0} ;
  char   line[CurvesFile_MaxLengthOfTextLine] ;
  char *c = expr ;
  
  /* Compute the curve values */
  for(c = expr ; *c ; c++) {
    if(*c == '$') {
      int i = *(c + 1) - '0' ;
      
      if(i > NbOfCurves || i > 10) {
        Message_RuntimeError("Evaluate: not valid curve index") ;
      }
      
      {
        Curve_t* curve = Curves_GetCurve(curves) + i - 1 ;
      
        y[i - 1] = Curve_ComputeValue(curve,x) ;
      }
    }
  }
  
  /* We copy expr in line by replacing 
   * xlabel with "($0)" and "$i" with "($i)" */
  {
    char *p = line ;
    
    c = expr ;
    
    while(*c) {
      
      if(*c == '$') {
        *p++ = '(' ;
        *p++ = '$' ;
        *p++ = *(c + 1) ;
        *p++ = ')' ;
        c += 2 ;
      } else if(!strncmp(c,xlabel,strlen(xlabel))) {
        *p++ = '(' ;
        *p++ = '$' ;
        *p++ = '0' ;
        *p++ = ')' ;
        c += strlen(xlabel) ;
      } else {
        *p++ = *c++ ;
      }
      
    }
    
    *p++ = '\0' ;
  }
  
  /* Substitute $i with the computed values */
  while((c = strchr(line,'$'))) {
    int i = *(c + 1) - '0' ;
      
    if(Curves_CannotAppendCurves(curves,i) || i > 10) {
      Message_RuntimeError("Evaluate") ;
    }
      
    *(c + 0) = '%' ;
    *(c + 1) = 'f' ;
      
    {
      char line1[CurvesFile_MaxLengthOfTextLine] ;
      
      if(i > 0) {
        sprintf(line1,line,y[i - 1]) ;
      } else if(i == 0) {
        sprintf(line1,line,x) ;
      }
      strcpy(line,line1) ;
    }
  }
  
  {
    double val = 0;//Math_EvaluateExpression(line) ;
    Message_RuntimeError("Evaluate: not available") ;
    
    return(val) ;
  }
}


double (Expressions)(double x,va_list args)
{
  Curves_t* curves = va_arg(args,Curves_t*) ;
  char*  expr = va_arg(args,char*) ;
  char*  ylabel = va_arg(args,char*) ;
  char*  xlabel = va_arg(args,char*) ;
  int NbOfCurves = Curves_GetNbOfCurves(curves) ;
  double y[10] = {0,0,0,0,0,0,0,0,0,0} ;
  char   line[CurvesFile_MaxLengthOfTextLine] ;
  char *c = expr ;
  
  /* Compute the curve values */
  for(c = expr ; *c ; c++) {
    if(*c == '$') {
      int i = *(c + 1) - '0' ;
      
      if(i > NbOfCurves || i > 10) {
        Message_RuntimeError("Expressions: not valid curve index") ;
      }
      
      {
        Curve_t* curve = Curves_GetCurve(curves) + i - 1 ;
      
        y[i - 1] = Curve_ComputeValue(curve,x) ;
      }
    }
  }
  
  /* We copy expr in line by 
   * 1. adding the expression "xlabel = x ;" 
   * 2. replacing "$i" with "($i)" 
   */
  {
    char *p = line ;
    
    c = expr ;
    
    /* 1. Add the expression "xlabel = x ;" */
    p += sprintf(p,"%s = %e ; ",xlabel,x) ;
    
    /* 2. Replace "$i" with "($i)" */
    while(*c) {
      
      if(*c == '$') {
        *p++ = '(' ;
        *p++ = '$' ;
        *p++ = *(c + 1) ;
        *p++ = ')' ;
        c += 2 ;
      } else {
        *p++ = *c++ ;
      }
      
    }
    
    *p++ = '\0' ;
  }
  
  /* Substitute $i with the computed values */
  while((c = strchr(line,'$'))) {
    int i = *(c + 1) - '0' ;
      
    if(Curves_CannotAppendCurves(curves,i) || i > 10) {
      Message_RuntimeError("Expressions") ;
    }
      
    *(c + 0) = '%' ;
    *(c + 1) = 'e' ;
      
    {
      char line1[CurvesFile_MaxLengthOfTextLine] ;
      
      if(i > 0) {
        sprintf(line1,line,y[i - 1]) ;
      } else if(i == 0) {
        sprintf(line1,line,x) ;
      }
      strcpy(line,line1) ;
    }
  }
  
  {
    //double val = Math_EvaluateExpressions(ylabel,line) ;
    double val = Expression_Evaluate(ylabel,line) ;
    
    return(val) ;
  }
}


double (Affine)(double x,va_list args)
/** y = a*x + b */
{
  double a = va_arg(args,double) ;
  double b = va_arg(args,double) ;
  double y  = a*x + b ;
  
  return(y) ; 
}


double (Langmuir)(double x,va_list args)
/** y = y0 * x / (x + x0) */
{
  double y0 = va_arg(args,double) ;
  double x0 = va_arg(args,double) ;
  double y  = (x > 0.) ? y0*x/(x0 + x) : 0. ;
  
  return(y) ; 
}


double (LangmuirN)(double x,va_list args)
/** y = y0 * (x/x0)**n / (1 + (x/x0)**n) */
{
  double y0  = va_arg(args,double) ;
  double x0  = va_arg(args,double) ;
  double n   = va_arg(args,double) ;
  double y   = langmuir(x,y0,x0,n) ;
  
  return(y) ; 
}


double (Freundlich)(double x,va_list args)
/** Freundlich: y = a*x**b */
{
  double a = va_arg(args,double) ;
  double b = va_arg(args,double) ;
  double y = (x > 0.) ? a*pow(x,b) : 0. ;
  
  return(y) ;
}


double (MualemVanGenuchten)(double s,va_list args)
/** Mualem-Van Genuchten: y = sqrt(x)*(1 - (1 - x**(1/m))**m)**2 */
{
  double m = va_arg(args,double) ;
  
  if(s <= 0) return(0) ;
  else if(s >= 1) return(1) ;
  else return(sqrt(s)*pow(1 - pow(1 - pow(s,1./m),m),2.)) ;
}


double (VanGenuchten)(double p,va_list args)
/** Van Genuchten: y = (1 + x**(1/(1-m)))**(-m) */
{
  double a = va_arg(args,double) ;
  double m = va_arg(args,double) ;
  
  if(p > 0) {
    double y = pow(1 + pow(p/a,1./(1-m)),-m) ;
    
    return(y) ;
  } else return(1) ;
}


double (NavGenuchten)(double s,va_list args)
/** Inverse of Van Genuchten: y = (x**(-1/m) - 1)**(1-m) */
{
  double a = va_arg(args,double) ;
  double m = va_arg(args,double) ;
  
  if(s >= 1) return(0.) ;
  else if(s <= 0) return(HUGE_VAL) ;
  else return(a*pow(pow(s,-1./m) - 1,1-m)) ;
}


double (Mualem_dry)(double p,va_list args)
{
  double a_w = va_arg(args,double) ;
  double m_w = va_arg(args,double) ;
  double a_d = va_arg(args,double) ;
  double m_d = va_arg(args,double) ;
  double kh_max = va_arg(args,double) ;
  FILE*  ftmp1 = va_arg(args,FILE*) ;
  double k_rd ;
  
  char   ltmp1[CurvesFile_MaxLengthOfTextLine] ;
  double kh ;
  double p1 ;
    
      
  do {
    fgets(ltmp1,sizeof(ltmp1),ftmp1) ;
  } while(ltmp1[0] == '#') ;
  
  sscanf(ltmp1,"%lf %lf",&p1,&kh) ;
  
  if(p != p1) arret("Mualem_dry") ;
  
  kh = 1. - kh/kh_max ;
    
  {
    double s_w  = vangenuchten(p/a_w,m_w) ;
    double kl   = 1 - pow(1 - pow(s_w,1./m_w),m_w) ;
      
    double s_d  = vangenuchten(p/a_d,m_d) ;
      
    k_rd = sqrt(s_d)*(kl + (1. - kl)*kh) ;
  }
          
  return(k_rd) ;
}


double (Mualem_gas)(double s,va_list args)
/** Mualem: y = sqrt(1-x)*(1 - x**(1/m))**(2*m) */
{
  double m = va_arg(args,double) ;
  
  if(s <= 0) return(1) ;
  else if(s >= 1) return(0) ;
  else return(sqrt(1 - s)*pow(1 - pow(s,1./m),2*m)) ;
}


double (VanGenuchten_gas)(double s,va_list args)
/** Mualem: y = ((1-x)**p)*(1 - x**(1/m))**(q*m) */
{
  double m = va_arg(args,double) ;
  double p = va_arg(args,double) ;
  double q = va_arg(args,double) ;
  
  if(s <= 0) return(1) ;
  else if(s >= 1) return(0) ;
  else return(pow(1 - s,p)*pow(1 - pow(s,1./m),q*m)) ;
}


double (Millington)(double s,va_list args)
/** Millington: y = (1 - x)**b */
{
  double b = va_arg(args,double) ;
  
  if(s <= 0) return(1) ;
  else if(s >= 1) return(0) ;
  else return(pow(1 - s,b)) ;
}

double (MonlouisBonnaire)(double s,va_list args)
/** Monlouis Bonnaire: y = (1 - x)**5.5*(1 - x**(1/m))**(2*m) */
{
  double m = va_arg(args,double) ;
  
  if(s <= 0) return(0) ;
  else if(s >= 1) return(1) ;
  else return(pow(1 - s,5.5)*pow(1 - pow(s,1./m),2*m)) ;
}


double (RedlichKwongCO2)(double Pa,va_list args)
{
  double T = va_arg(args,double) ;
  double rho = MolarDensityOfCO2_RedlichKwong(Pa,T) ;

  return(1.e-3*rho) ;
}


double (MolarDensityOfCO2)(double Pa,va_list args)
{
  double T = va_arg(args,double) ;
  double rho = MolarDensityOfCO2_RedlichKwong(Pa,T) ;

  return(rho) ;
}


double (MolarVolumeOfCO2CH4Mixture)(double y_co2,va_list args)
{
  double P = va_arg(args,double) ;
  double T = va_arg(args,double) ;
  double V = MolarVolumeOfCO2CH4Mixture_RedlichKwong(P,T,y_co2) ;

  return(V) ;
}


double (PartialFugacityOfCO2CH4Mixture)(double y_co2,va_list args)
{
  double P = va_arg(args,double) ;
  double T = va_arg(args,double) ;
  char* gas = va_arg(args,char*) ;
  double f = PartialFugacityOfCO2CH4Mixture_RedlichKwong(P,T,y_co2,gas) ;

  return(f) ;
}


double (MolarDensityOfPerfectGas)(double Pa,va_list args)
/** Perfect gas law: y = Pa/RT */
{
  double T = va_arg(args,double) ;
  double R  = 8.3143 ;
  double RT = R*T ;
  
  return(Pa/RT) ;
}


double (FenghourCO2)(double Pa,va_list args)  
{
  double T = va_arg(args,double) ;
  
  return(ViscosityOfCO2_Fenghour(Pa,T)) ;
}

      
double (CSH3EndMembers)(double s_CH,va_list args)
{
  double k_CH  = 6.456e-6 ;
  /* Amorpheous Silica */
  double k_SH = 1.93642e-3 ;
  double x_SH = 0 ;
  double y_SH = 1 ;
  double z_SH = 2 ;
  double v_SH = (29. + 7.*z_SH)*cm3 ;
  /* Jennite (y_Jen = 1)         (y_Jen = 0.9)      */
  double k_Jen0 = 4.39e-18  ; /*  k_Jen = 2.39e-16  */
  double x_Jen0 = 1.66666   ; /*  x_Jen = 1.5       */
  double z_Jen0 = 2.66666   ; /*  z_Jen = 2.4       */
  double v_Jen0 = 81.8*cm3  ; /*  v_Jen = 73.6  (cm3/mol) */
  /* Tobermorite (y_Tob = 1)     (y_Tob = 1.8)      */
  double k_Tob0 = 1.684e-12 ; /*  k_Tob = 6.42e-22  */
  double x_Tob0 = 0.83333   ; /*  x_Tob = 1.5       */
  double z_Tob0 = 1.83333   ; /*  z_Tob = 3.3       */
  double v_Tob0 = 54.8*cm3  ; /*  v_Tob = 98.6  (cm3/mol) */
  
  double s_SH = 1. ;
  
  double y_Tob = va_arg(args,double) ;
  double y_Jen = va_arg(args,double) ;
  char *outputtype = va_arg(args,char*) ;
  
  /* Jennite */
  double x_Jen   = x_Jen0*y_Jen ;
  double z_Jen   = z_Jen0*y_Jen ;
  double k_Jen   = pow(k_Jen0,y_Jen) ;
  double v_Jen   = v_Jen0*y_Jen ;
  /* Tobermorite */
  double x_Tob   = x_Tob0*y_Tob ;
  double z_Tob   = z_Tob0*y_Tob ;
  double k_Tob   = pow(k_Tob0,y_Tob) ;
  double v_Tob   = v_Tob0*y_Tob ;
  
  double output ;

  {
    double q_CSH[3] ;
    double a_CSH[3] ;
    double x_CSH[3] ;
    double y_CSH[3] ;
    double z_CSH[3] ;
    double k_CSH[3] ;
    double v_CSH[3] ;
    int    j ;
          
    k_CSH[0] = k_SH   ; 
    x_CSH[0] = x_SH   ; y_CSH[0] = y_SH   ; z_CSH[0] = z_SH ;
    k_CSH[1] = k_Tob  ; 
    x_CSH[1] = x_Tob  ; y_CSH[1] = y_Tob  ; z_CSH[1] = z_Tob ;
    k_CSH[2] = k_Jen  ; 
    x_CSH[2] = x_Jen  ; y_CSH[2] = y_Jen  ; z_CSH[2] = z_Jen ;
    v_CSH[0] = v_SH ;
    v_CSH[1] = v_Tob ;
    v_CSH[2] = v_Jen ;
          
          
    for(j = 0 ; j < 3 ; j++) {
      a_CSH[j] = pow(k_CH*s_CH,x_CSH[j])*pow(k_SH,y_CSH[j])/k_CSH[j] ;
    }
          
    s_SH = fraction_Silica(a_CSH,y_CSH,3,s_SH) ;
          
    for(j = 0 ; j < 3 ; j++) {
      q_CSH[j] = a_CSH[j]*pow(s_SH,y_CSH[j]) ;
    }
          
    {
      double x_m = 0,y_m = 0,z_m = 0 ;
      double v_m = 0 ;
      
      for(j = 0 ; j < 3 ; j++) {
        x_m += q_CSH[j]*x_CSH[j] ;
        y_m += q_CSH[j]*y_CSH[j] ;
        z_m += q_CSH[j]*z_CSH[j] ;
        v_m += q_CSH[j]*v_CSH[j] ;
      }
            
      x_m /= y_m ; z_m /= y_m ; v_m /= y_m ;
      
      if(String_Is(outputtype,"S_SH")) {
        output = s_SH ;
      } else if(String_Is(outputtype,"V_CSH")) {
        output = v_m ;
      } else if(String_Is(outputtype,"Z_CSH")) {
        output = z_m ;
      } else if(String_Is(outputtype,"X_CSH")) {
        output = x_m ;
      } else {
        arret("CSH3EndMembers") ;
      }
    }
  }
      
  return(output) ;
}

      
double (CSHLangmuirN)(double s,va_list args)
{
  double x1 = va_arg(args,double) ;
  double s1 = va_arg(args,double) ;
  double n1 = va_arg(args,double) ;
  double x2 = va_arg(args,double) ;
  double s2 = va_arg(args,double) ;
  double n2 = va_arg(args,double) ;
  char *outputtype = va_arg(args,char*) ;
  double output ;
  
  if(String_Is(outputtype,"S_SH")) {
    double s_SH = pow(1 + pow(s/s1,n1),-x1/n1)*pow(1 + pow(s/s2,n2),-x2/n2) ;
    
    output = s_SH ;
  } else if(String_Is(outputtype,"X_CSH")) {
    double x = langmuir(s,x1,s1,n1) + langmuir(s,x2,s2,n2) ;
    
    output = x ;
  } else {
    arret("CSHLangmuirN") ;
  }
      
  return(output) ;
}





double (PermeabilityCoefficient_KozenyCarman)(double phi,va_list args)
/* Kozeny-Carman model */
{
  double phi0 = va_arg(args,double) ;
  double coeff_permeability ;
  
  {
    double kozeny_carman  = pow(phi/phi0,3.)*pow(((1 - phi0)/(1 - phi)),2.) ;
  
    coeff_permeability = kozeny_carman ;
  }
  
  return(coeff_permeability) ;
}



double (PermeabilityCoefficient_VermaPruess)(double phi,va_list args)
/* Ref:
 * A. Verma and K. Pruess,
 * Thermohydrological Conditions and Silica Redistribution Near High-Level
 * Nuclear Wastes Emplaced in Saturated Geological Formations,
 * Journal of Geophysical Research, 93(B2) 1159-1173, 1988
 * frac  = fractionnal length of pore bodies (0.8) 
 * phi_r = fraction of initial porosity (phi/phi0) at which permeability is 0 
 */
{
  double phi0 = va_arg(args,double) ;
  double frac = va_arg(args,double) ;
  double phi_r = va_arg(args,double) ;
  double coeff_permeability ;
  
  {
    double S_s =  (phi0 - phi)/phi0    ;
    double w = 1 + (1/frac)/(1/phi_r - 1) ;
    double t = (1 - S_s - phi_r)/(1 - phi_r) ;
    double verma_pruess = (t > 0) ? t*t*(1 - frac + (frac/(w*w)))/(1 - frac + frac*(pow(t/(t + w - 1),2.))) : 0 ;
  
    coeff_permeability = verma_pruess ;
  }
  
  return(coeff_permeability) ;
}




double (TortuosityToLiquid_OhJang)(double phi,va_list args)
/* Ref:
 * Byung Hwan Oh, Seung Yup Jang, 
 * Prediction of diffusivity of concrete based on simple analytic equations, 
 * Cement and Concrete Research 34 (2004) 463 - 480.
 * tau = (m_p + sqrt(m_p**2 + phi_c/(1 - phi_c) * (Ds/D0)**(1/n)))**n
 * m_p = 0.5 * ((phi_cap - phi_c) + (Ds/D0)**(1/n) * (1 - phi_c - phi_cap)) / (1 - phi_c)
 */
{
  double phi_cap = phi/2  ;
  double phi_c   = 0.17 ;   /* Percolation capilar porosity */
  double n     = 2.7 ;          /* OPC n  = 2.7  --------  Fly ash n  = 4.5 */
  double ds    = 1.e-4 ;        /* OPC ds = 1e-4 --------  Fly ash ds = 5e-5 */
  double dsn   = pow(ds,1/n) ;
  double m_phi = 0.5 * ((phi_cap - phi_c) + dsn * (1 - phi_c - phi_cap)) / (1 - phi_c) ;
  double tausat =  pow(m_phi + sqrt(m_phi*m_phi + dsn * phi_c/(1 - phi_c)),n) ;
  
  double tau =  tausat ;
    
  return tau ;
}




double (TortuosityToLiquid_BazantNajjar)(double phi,va_list args)
/* Ref:
 * Z. P. BAZANT, L.J. NAJJAR,
 * Nonlinear water diffusion in nonsaturated concrete,
 * Materiaux et constructions, 5(25), 1972.
 */
{
  double tausat = 0.00029 * exp(9.95 * phi) ;
  double tau    = tausat ;
    
  return tau ;
}


/* Other intern functions */
double (vangenuchten)(double p,double m)
/** Van Genuchten: y = (1 + x**(1/(1-m)))**(-m) */
{
  if(p > 0) return(pow(1 + pow(p,1./(1-m)),-m)) ;
  else return(1) ;
}

double (fraction_Silica)(double *a,double *y,int n,double q0)
/**  */
{
  double err,tol = 1e-8 ;
  double q = q0 ;
  int    i = 0 ;
  
  do {
    double f  = - 1 ;
    double df = 0 ;
    double dq ;
    int    j ;
    
    for(j = 0 ; j < n ; j++) {
      double fj = a[j]*pow(q,y[j]) ;
      
      f  += fj ;
      df += y[j]*fj/q ;
    }
    
    dq = -f/df ;
    err = fabs(dq/q) ;
    q += dq ;
    
    if(i++ > 20) {
      printf("q0 = %e\n",q0) ;
      printf("q  = %e\n",q) ;
      arret("fraction_Silica : non convergence") ;
    }
  } while(err > tol) ;
  
  return(q) ;
}



double (MolarDensityOfCO2_RedlichKwong)(double P,double T)
/** Redlich Kwong EOS for CO2 */
/* From Redlich-Kwong model of EOS
 * Redlich O., Kwong J.N.S 
 * On the thermodynamics of solutions. V. 
 * An equation of state. Fugacities of gaseous solutions.
 * Chemical Reviews, 44(1), 233-244, 1949.
 * P = RT/(V - b) - a/(sqrt(T)*V*(V + b))
 * Input units:
 *   P in Pascal, 
 *   T in Kelvin, 
 * Output units: 
 *   V in mol/m3 
 */
{
  double R         = 8.31451 ;
  double two3      = pow(2,1./3) ;
  double Zc        = 1./3 ;
  double Pc_CO2    = 7.35e6 ;
  double Tc_CO2    = 304.14 ;
  double Vc_CO2    = Zc*R*Tc_CO2/Pc_CO2 ;
  double B_CO2     = Vc_CO2*(two3 - 1) ;
  double A_CO2     = R*pow(Tc_CO2,1.5)*(Vc_CO2/(Vc_CO2-B_CO2)-Zc)*(Vc_CO2+B_CO2) ;
  
  double V         = RedlichKwong(P,T,A_CO2,B_CO2) ;
  
  return(1/V) ;
}



double (MolarVolumeOfCO2CH4Mixture_RedlichKwong)(double P,double T,double y_co2)
/** Redlich Kwong EOS for the mixture of CO2 and CH4 */
/* From Redlich-Kwong model of EOS
 * Redlich O., Kwong J.N.S 
 * On the thermodynamics of solutions. V. 
 * An equation of state. Fugacities of gaseous solutions.
 * Chemical Reviews, 44(1), 233-244, 1949.
 * P = RT/(V - b) - a/(sqrt(T)*V*(V + b))
 * Input units:
 *   P in Pascal, 
 *   T in Kelvin, 
 * Output units: 
 *   V in mol/m3 
 */
{
  double R         = 8.31451 ;
  double two3      = pow(2,1./3) ;
  double Zc        = 1./3 ;
  double Pc_CO2    = 7.35e6 ;
  double Tc_CO2    = 304.14 ;
  double Vc_CO2    = Zc*R*Tc_CO2/Pc_CO2 ;
  double Pc_CH4    = 4.6e6 ;
  double Tc_CH4    = 190 ;
  double Vc_CH4    = Zc*R*Tc_CH4/Pc_CH4 ;
  double B_CO2     = Vc_CO2*(two3 - 1) ;
  double A_CO2     = R*pow(Tc_CO2,1.5)*(Vc_CO2/(Vc_CO2-B_CO2)-Zc)*(Vc_CO2+B_CO2) ;
  double B_CH4     = Vc_CH4*(two3 - 1) ;
  double A_CH4     = R*pow(Tc_CH4,1.5)*(Vc_CH4/(Vc_CH4-B_CH4)-Zc)*(Vc_CH4+B_CH4) ;
  double y_ch4     = 1 - y_co2 ;
  double B         = B_CO2*y_co2 + B_CH4*y_ch4 ;
  double A05       = sqrt(A_CO2)*y_co2 + sqrt(A_CH4)*y_ch4 ;
  double A         = A05*A05 ;
  
  double V         = RedlichKwong(P,T,A,B) ;
  
  return(V) ;
}



double (PartialFugacityOfCO2CH4Mixture_RedlichKwong)(double P,double T,double y_co2,const char* gas)
/** Redlich Kwong EOS for the mixture of CO2 and CH4 */
/* From Redlich-Kwong model of EOS
 * Redlich O., Kwong J.N.S 
 * On the thermodynamics of solutions. V. 
 * An equation of state. Fugacities of gaseous solutions.
 * Chemical Reviews, 44(1), 233-244, 1949.
 * P = RT/(V - b) - a/(sqrt(T)*V*(V + b))
 * Input units:
 *   P in Pascal, 
 *   T in Kelvin, 
 * Output units: 
 *   V in mol/m3 
 */
{
  double R         = 8.31451 ;
  double two3      = pow(2,1./3) ;
  double Zc        = 1./3 ;
  double Pc_CO2    = 7.35e6 ;
  double Tc_CO2    = 304.14 ;
  double Vc_CO2    = Zc*R*Tc_CO2/Pc_CO2 ;
  double Pc_CH4    = 4.6e6 ;
  double Tc_CH4    = 190 ;
  double Vc_CH4    = Zc*R*Tc_CH4/Pc_CH4 ;
  double B_CO2     = Vc_CO2*(two3 - 1) ;
  double A_CO2     = R*pow(Tc_CO2,1.5)*(Vc_CO2/(Vc_CO2-B_CO2)-Zc)*(Vc_CO2+B_CO2) ;
  double B_CH4     = Vc_CH4*(two3 - 1) ;
  double A_CH4     = R*pow(Tc_CH4,1.5)*(Vc_CH4/(Vc_CH4-B_CH4)-Zc)*(Vc_CH4+B_CH4) ;
  double y_ch4     = 1 - y_co2 ;
  double B         = B_CO2*y_co2 + B_CH4*y_ch4 ;
  double A05       = sqrt(A_CO2)*y_co2 + sqrt(A_CH4)*y_ch4 ;
  double A         = A05*A05 ;
  
  double V         = RedlichKwong(P,T,A,B) ;
  double RT        = R*T ;
  double Z         = P*V/RT ;
  double T15       = T*sqrt(T) ;
  double C         = A/(B*R*T15) ;
  double ln1       = log(Z-B*P/RT) ;
  double ln2       = log(1+B*P/(Z*RT)) ;
  
  if(!strcasecmp(gas,"co2")) {
    double lnc_co2 = (Z - 1)*B_CO2/B - ln1 - C*(2*sqrt(A_CO2)/A05 - B_CO2/B)*ln2 ;
    
    return(exp(lnc_co2)*y_co2*P) ;
    
  } else if(!strcasecmp(gas,"ch4")) {
    double lnc_ch4 = (Z - 1)*B_CH4/B - ln1 - C*(2*sqrt(A_CH4)/A05 - B_CH4/B)*ln2 ;
    
    return(exp(lnc_ch4)*y_ch4*P) ;
  }
  
  arret("CO2 or CH4 only") ;
  return(0) ;
}



double (ViscosityOfCO2_Fenghour)(double Pa,double T)  
/** Fenghour viscosity of scCO2: */
/* Implemented by J. Shen 
 * From A. Fenghour (1997)
 * Input units: Pressure in Pa, Temperature in Kelvin
 * Output units: Pa.s 
 * Range of validity:
 *      P [0:300] MPa and T [200-1000] K
 *      P [0-30]  MPa and T [1000-1500] K 
*/
{
  double P        = MAX(Pa,1.e5) ; /* mu_co2 cst for P < 1e5 Pa */
  double gc_co2   = MolarDensityOfCO2_RedlichKwong(P,T) ;
  double M_CO2    = 44.e-3 ;
  double rho      = M_CO2*gc_co2 ;
  double d11      = 0.4071119e-2 ;
  double d21      = 0.7198037e-4 ;
  double d64      = 0.2411697e-16 ;
  double d81      = 0.2971072e-22 ;
  double d82      = -0.1627888e-22 ;
  double T_red    = T/251.196 ;
  double logT_red = log(T_red) ;
  double a0       = 0.235156 ;
  double a1       = -0.491266 ;
  double a2       = 5.211155e-2 ;
  double a3       = 5.347906e-2 ;
  double a4       = -1.537102e-2 ;
  double A_mu     = a0 + a1*logT_red + a2*pow(logT_red,2.) + a3*pow(logT_red,3.)  + a4*pow(logT_red,4.) ;
  double g_mu     = exp(A_mu) ;
  double mu_zero  = 1.00697*pow(T,0.5)/g_mu ; /*zero-density viscosity, uPa.s*/
  double rho2     = rho*rho ;
  double rho6     = rho2*rho2*rho2 ;
  double rho8     = rho6*rho2 ;
  double mu_excess  = d11*rho+ d21*rho2 + d64*rho6/pow(T_red,3.) + d81*rho8 + d82*rho8/T_red;/* in uPa.s*/
  double mu       = mu_zero + mu_excess; /* uPa.s*/
  double mu_co2   = mu*1.e-6 ;           /* Pa.s*/
  return(mu_co2) ;
}



double (langmuir)(double x,double y0,double x0,double n)
/** y = y0 * (x/x0)**n / (1 + (x/x0)**n) */
{
  double a   = (x0 > 0.) ? x/x0 : 0. ;
  double p   = (a  > 0.) ? pow(a,n) : 0. ;
  double y   = (x0 > 0.) ? y0*p/(1 + p) : y0 ;
  
  return(y) ; 
}



static double lna_i(double,double,double,double,double,double) ;


#include "PartialMolarVolumeOfMoleculeInWater.h"

double LinLee(double c_s1,va_list args)
/** Activity of water in brine from
 *  H. Lin and L. Lee, 
 *  Estimations of activity coefficients of constituent ions in aqueous
 *  electrolyte solutions with the two-ionic-parameter approach, 
 *  Fluid Phase Equilibria, 237 (2005) 1-8.
 */
{
  double tem1 = va_arg(args,double) ;
  const char* s = va_arg(args,const char*) ;
  double c_s = (c_s1 > 0) ? c_s1 : 0 ;
  double tem = (tem1 > 200) ? tem1 : 200 ;
  double T_0  = 273.15 ;
  /* eps_r: electric permittivity of the solution */
  double eps_r = 0.0007*(tem - T_0)*(tem - T_0) - 0.3918*(tem - T_0) + 87.663 ;
  /* A_phi: the Debye-Huckel constant */
  double A_phi   = 1398779.816/pow(eps_r*tem,1.5) ;
  
  /* References */
  #define M_H2O   (18.e-3)
  double b0   = sqrt(M_H2O) ;
  double S0   = pow(M_H2O,1.29) ;
  #undef M_H2O

  double A    = A_phi/b0 ;
  
  double b_cat ;
  double b_ani ;
  double S_cat ;
  double S_ani ;
  double z_cat ;
  double z_ani ;
  double nu_cat ;
  double nu_ani ;
  double v_cat ;
  double v_ani ;

  
  if(String_Is(s,"NaCl")) {
    b_cat  = 4.352/b0 ;
    b_ani  = 1.827/b0 ;
    S_cat  = 26.448/S0 ;
    S_ani  = 19.245/S0 ;
    z_cat  = 1 ;
    z_ani  = -1 ;
    nu_cat = 1 ;
    nu_ani = 1 ;
    v_cat  = PartialMolarVolumeOfMoleculeInWater(Na) ;
    v_ani  = PartialMolarVolumeOfMoleculeInWater(Cl) ;
  } else if(String_Is(s,"CaCl2")) {
    b_cat  = 3.908/b0 ;
    b_ani  = 2.085/b0 ;
    S_cat  = 18.321/S0 ;
    S_ani  = 10.745/S0 ;
    z_cat  = 2 ;
    z_ani  = -1 ;
    nu_cat = 1 ;
    nu_ani = 2 ;
    v_cat = PartialMolarVolumeOfMoleculeInWater(Ca) ;
    v_ani = PartialMolarVolumeOfMoleculeInWater(Cl) ;
  } else if(String_Is(s,"Na2SO4")) {
    b_cat  = 1.552/b0 ;
    b_ani  = 1.662/b0 ;
    S_cat  = 3.464/S0 ;
    S_ani  = 0.022/S0 ;
    z_cat  = 1 ;
    z_ani  = -2 ;
    nu_cat = 2 ;
    nu_ani = 1 ;
    v_cat = PartialMolarVolumeOfMoleculeInWater(Na) ;
    v_ani = PartialMolarVolumeOfMoleculeInWater(SO4) ;
  } else if(String_Is(s,"K2SO4")) {
    b_cat  = 1.655/b0 ;
    b_ani  = 1.570/b0 ;
    S_cat  = 0.017/S0 ;
    S_ani  = 2.273/S0 ;
    z_cat  = 1 ;
    z_ani  = -2 ;
    nu_cat = 2 ;
    nu_ani = 1 ;
    v_cat = PartialMolarVolumeOfMoleculeInWater(K) ;
    v_ani = PartialMolarVolumeOfMoleculeInWater(SO4) ;
  } else if(String_Is(s,"KCl")) {
    b_cat  = 1.243/b0 ;
    b_ani  = 3.235/b0 ;
    S_cat  = 13.296/S0 ;
    S_ani  = 11.158/S0 ;
    z_cat  = 1 ;
    z_ani  = -1 ;
    nu_cat = 1 ;
    nu_ani = 1 ;
    v_cat = PartialMolarVolumeOfMoleculeInWater(K) ;
    v_ani = PartialMolarVolumeOfMoleculeInWater(Cl) ;
  } else {
    arret("LinLee: salt not available") ;
    return(-1) ;
  }
  
  {
    /* concentration of ions */
    double c_ani  = nu_ani*c_s ;
    double c_cat  = nu_cat*c_s ;
    /* Molar volume of liquid water (m3/mol) */
    #define V_H2O  (18.e-6)
    /* concentration of solvent */
    double c_h2o  = (1 - (c_ani*v_ani + c_cat*v_cat))/V_H2O ;
    #undef V_H2O
    
    /* molalities * M_H2O */
    double m_ani  = c_ani/c_h2o ;
    double m_cat  = c_cat/c_h2o ;

    /* ionic strength */
    double I      =  0.5*(z_ani*z_ani*m_ani + z_cat*z_cat*m_cat);
  
    /* activity of water */
    double II_ani = lna_i(tem,I,z_ani,b_ani,S_ani,A) ;
    double II_cat = lna_i(tem,I,z_cat,b_cat,S_cat,A) ;
    
    /* Expression from Lin and Lee, 2005.
     * This expression is consistent with the thermodynamic
     * (Nguyen T.Q., Modélisations physico-chimiques de la pénétration
     * des ions chlorures dans les matériaux cimentaires, 
     * phd thesis ENPS ParisTech, 2007.)
     */
    {
      double lna_w  = m_ani*II_ani + m_cat*II_cat ;
      /* linearized activity of water */
      //double lna_w = - (m_ani + m_cat) ;

      return(lna_w) ;
    }
  }
}


double lna_i(double T,double I,double z,double b,double S,double A)
/* Contribution de chaque ion au log de l'activite du solvant 
   lna_w = sum_i ( m_i*lna_i ) (T.Q Nguyen) */ 
{
  double alpha = 1.29 ;
  double a1 = alpha/(1+alpha) ;
  double II = sqrt(I) ;
  double lna = A*II/(1 + b*II) - a1*S*pow(I,alpha)/T ;
  double lna_i = -1 + lna*z*z ;
  
  return(lna_i) ;
}




double IdealWaterActivity(double c_s,va_list args)
/* Natural log of water activity in ideal mixture 
 * Inputs:
 * - c_s: concentration in mol/m3
 * - nu_cat: cation stoichiometry of salt
 * - nu_ani: anion stoichiometry of salt
 */
{
  int nu_cat = va_arg(args,int) ;
  int nu_ani = va_arg(args,int) ;
  double c_cat = nu_cat*c_s ;
  double c_ani = nu_ani*c_s ;
  
  /* linearized term */
  /* Molar volume of liquid water (m3/mol) */
  #define V_H2O  (18.e-6)
  double lna_w = - (c_cat + c_ani)*V_H2O ;
  #undef V_H2O
  
  return(lna_w) ;
}



double (RedlichKwong)(double P,double T,double A,double B)
/** Solve Redlich-Kwong EOS for the molar volume */
/* From Redlich-Kwong model of EOS
 * Redlich O., Kwong J.N.S 
 * On the thermodynamics of solutions. V. 
 * An equation of state. Fugacities of gaseous solutions.
 * Chemical Reviews, 44(1), 233-244, 1949.
 * P = RT/(V - B) - A/(sqrt(T)*V*(V + B))
 * Input units:
 *   P in Pascal, 
 *   T in Kelvin, 
 *   B in m3/mol, 
 *   A/sqrt(T) in Pascal*m6/mol2
 * Output units: 
 *   V in mol/m3 
 **/
{
  double R         = 8.31451 ;
  double RT        = R*T ;
  double T05       = sqrt(T) ;
  double AB        = A*B ;
  double B2        = B*B ;
  double P0        = 1.e5 ;
  
  if(P > P0) {
    double RToP      = RT/P ;
    double PT05      = P*T05 ;
    double a         = 1. ;
    double b         = - RToP ;
    double c         = - (B*RToP - A/(PT05) + B2) ;
    double d         = - (AB/(PT05));
    double x[4]      ;
    
    x[0]  = a  ; x[1]  = b  ; x[2]  = c  ; x[3]  = d  ;
    Math_ComputePolynomialEquationRoots(x,3) ;
    
    {
      double V = x[0] ; /* the volume of gas phase is the largest root */
      return(V) ;
    }
    
  } else {
    double RToP0     = RT/P0 ;
    double P0T05     = P0*T05 ;
    double a0        = 1. ;
    double b0        = - (RToP0) ;
    double c0        = - (B*RToP0 - A/(P0T05) + B2) ;
    double d0        = - (AB/(P0T05)) ;
    double x0[4]     ;
    
    x0[0] = a0 ; x0[1] = b0 ; x0[2] = c0 ; x0[3] = d0 ;
    Math_ComputePolynomialEquationRoots(x0,3) ;
    
    {
      double V0 = x0[0] ;
      return(V0*P0/P) ;
    }
  }
}



#if 0
double (MolarDensityOfCO2_RedlichKwong)(double Pa,double T)
/** Redlich Kwong EOS for CO2 */
/* From Redlich-Kwong model of EOS
 * Redlich O., Kwong J.N.S 
 * On the thermodynamics of solutions. V. 
 * An equation of state. Fugacities of gaseous solutions.
 * Chemical Reviews, 44(1), 233-244, 1949.
 * P = RT/(V - b) - a/(sqrt(T)*V*(V + b))
 * Input units:
 *   P in Pascal, 
 *   T in Kelvin, 
 * Output units: 
 *   V in mol/L 
 */
{
  double R         = 83.14472 ;   /* cm3*bar/(K*mol) */
  double RT        = R*T ;
  double T05       = sqrt(T) ;
  double A_CO2     = 7.54e7 - 4.13e4*T ; /* bar*cm6*K0.5/mol2 */
  double B_CO2     = 27.80 ; /* cm3/mol */
  double AB_CO2    = A_CO2*B_CO2 ;
  double B2_CO2    = B_CO2*B_CO2 ;
  double P         = Pa/1.e5 ; /* unit of bar */
  double P0        = 1. ;

  
  double gc_co2 ;
  
  if(P > P0) {
    double RToP      = RT/P ;
    double PT05      = P*T05 ;
    double a         = 1. ;
    double b         = - RToP ;
    double c         = - (B_CO2*RToP - A_CO2/(PT05) + B2_CO2) ;
    double d         = - (AB_CO2/(PT05));
    double x[4]      ;
    
    x[0]  = a  ; x[1]  = b  ; x[2]  = c  ; x[3]  = d  ;
    Math_ComputePolynomialEquationRoots(x,3) ;
    
    {
      double V = x[0] ;    /* cm3/mol */
      gc_co2 = 1.e3/V ;    /* mol/dm3 */
    }
    
  } else {
    double RToP0     = RT/P0 ;
    double P0T05     = P0*T05 ;
    double a0        = 1. ;
    double b0        = - (RToP0) ;
    double c0        = - (B_CO2*RToP0 - A_CO2/(P0T05) + B2_CO2) ;
    double d0        = - (AB_CO2/(P0T05)) ;
    double x0[4]     ;
    
    x0[0] = a0 ; x0[1] = b0 ; x0[2] = c0 ; x0[3] = d0 ;
    Math_ComputePolynomialEquationRoots(x0,3) ;
    
    {
      double V0 = x0[0] ;       /* cm3/mol */
      gc_co2 = P/P0*1.e3/V0  ;  /* mol/dm3 */
    }
  }
  
  return(gc_co2) ;
}
#endif

