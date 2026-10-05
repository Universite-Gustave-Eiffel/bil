#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "OutputFiles.h"
#include "OutputFile.h"
#include "Message.h"
#include "Mesh.h"
#include "BilVersion.h"
#include "TextFile.h"
#include "Models.h"
#include "String_.h"
#include "Mry.h"
#include "DistributedMS.h"
#include "Results.h"
#include "Element.h"
#include "Materials.h"
#include "Models.h"
#include "Result.h"
#include "Points.h"
#include "Point.h"


//char   OutputFile_TypeOfCurrentFile ;



OutputFiles_t*   (OutputFiles_Create)(char* filename,size_t n_dates,size_t n_points)
{
  OutputFiles_t* outputfiles = (OutputFiles_t*) Mry_New(OutputFiles_t) ;
  
  
  /* The file name */
  {
    size_t LengthOfName = strlen(filename) + 1 ;
    char* name = (char*) Mry_New(char,LengthOfName) ;
    
    strcpy(name,filename) ;

    OutputFiles_GetDataFileName(outputfiles) = name ;
  }


  /* Date Files */
  OutputFiles_GetNbOfDateFiles(outputfiles) = n_dates ;

  /* Date Output Files */
  {
    int n = ceil(log10((double) n_dates+1)) ;
    size_t LengthOfName = strlen(filename) + 3 + n ;
    char* name = (char*) Mry_New(char,LengthOfName) ;
    
    {
      OutputFile_t* outputfile = (OutputFile_t*) Mry_New(OutputFile_t,n_dates) ;
      
      for(size_t i = 0 ; i < n_dates ; i++) {
        sprintf(name,"%s.t%lu",filename,i) ;
        
        {
          OutputFile_t* opf = OutputFile_Create(name) ;
        
          outputfile[i] = opf[0] ;
          Mry_Free(opf) ;
        }
      }
      //OutputFile_t* outputfile = OutputFile_Create(name,n_dates) ;
    
      OutputFiles_GetDateOutputFile(outputfiles) = outputfile ;
    }
      
    Mry_Free(name) ;
  }
  
  
  /* Point Files */
  OutputFiles_GetNbOfPointFiles(outputfiles) = n_points ;

  /* Point Output Files */
  {
    int n = ceil(log10((double) n_points+1)) ;
    size_t LengthOfName = strlen(filename) + 3 + n ;
    char* name = (char*) Mry_New(char,LengthOfName) ;
    
    {  
      OutputFile_t* outputfile = (OutputFile_t*) Mry_New(OutputFile_t,n_points) ;
      
      for(size_t i = 0 ; i < n_points ; i++) {
        sprintf(name,"%s.p%lu",filename,i+1) ;
        
        {
          OutputFile_t* opf = OutputFile_Create(name) ;
        
          outputfile[i] = opf[0] ;
          Mry_Free(opf) ;
        }
      }
      //OutputFile_t* outputfile = OutputFile_Create(name,n_points) ;
    
      OutputFiles_GetPointOutputFile(outputfiles) = outputfile ;
    }
      
    Mry_Free(name) ;
  }
  
  
  /* Results */
  {
    Results_t* results = Results_Create(OutputFiles_MaxNbOfViews) ;
    
    OutputFiles_GetResults(outputfiles) = results ;
  }
  
  
  /* Text line */
  {
    char* line = (char*) Mry_New(char,OutputFiles_MaxLengthOfTextLine) ;
    
    OutputFiles_GetTextLine(outputfiles) = line ;
  }
  
  
  return(outputfiles) ;
}



void   (OutputFiles_Delete)(void* self)
{
  OutputFiles_t* outputfiles = (OutputFiles_t*) self ;
  
  {
    char* name = OutputFiles_GetDataFileName(outputfiles) ;
    
    if(name) {
      Mry_Free(name) ;
    }
  }

  {
    size_t n_dates = OutputFiles_GetNbOfDateFiles(outputfiles) ;
    OutputFile_t* outputfile = OutputFiles_GetDateOutputFile(outputfiles) ;
    
    if(outputfile) {      
      for(size_t i = 0 ; i < n_dates ; i++) {
        OutputFile_t* opf = outputfile + i ;
      
        OutputFile_Delete(opf) ;
      }
    
      Mry_Free(outputfile) ;
    }
  }

  {
    size_t n_points = OutputFiles_GetNbOfPointFiles(outputfiles) ;
    OutputFile_t* outputfile = OutputFiles_GetPointOutputFile(outputfiles) ;
    
    if(outputfile) {      
      for(size_t i = 0 ; i < n_points ; i++) {
        OutputFile_t* opf = outputfile + i ;
      
        OutputFile_Delete(opf) ;
      }
    
      Mry_Free(outputfile) ;
    }
  }
  
  {
    Results_t* results = OutputFiles_GetResults(outputfiles) ;
    
    if(results) {
      Results_Delete(results) ;
      Mry_Free(results) ;
    }
  }
  
  {
    char* line = OutputFiles_GetTextLine(outputfiles) ;
    
    if(line) {
      Mry_Free(line) ;
    }
  }
}



char* (OutputFiles_Version)(OutputFiles_t* outputfiles)
/** Return the version number of Bil which output files
 *  have been created with.
 */
{
  char* line = OutputFiles_GetTextLine(outputfiles);
  
  /* Read the version in the first line */
  {
    /* Assuming that there is at least one date file */
    OutputFile_t* outputfile = OutputFiles_GetDateOutputFile(outputfiles) ;
    TextFile_t* textfile = OutputFile_GetTextFile(outputfile) ;
    char* c ;
    
    TextFile_OpenFile(textfile,"r") ;
    c = OutputFiles_ReadLineFromCurrentFilePosition(outputfiles,textfile) ;
  
    //if((c = strstr(c,"Version") + strlen("Version"))) {
    if((c = String_FindAndSkipToken(c,"Version"))) {
      sscanf(c,"%s",line) ;
    }
  
    TextFile_CloseFile(textfile) ;
  }
  
  return(line) ;
}




void (OutputFiles_BackupSolutionAtTime_)(OutputFiles_t* outputfiles,DataSet_t* dataset,double t,int idate1)
/* Backup solutions at a given time in the appropriate output file */
{
  Mesh_t* mesh = DataSet_GetMesh(dataset) ;
  int dim = Mesh_GetDimension(mesh) ;
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  Materials_t* materials = DataSet_GetMaterials(dataset) ;
  Models_t* usedmodels = Materials_GetUsedModels(materials) ;
  size_t n_usedmodels = Models_GetNbOfModels(usedmodels) ;
  
  OutputFile_t* outputfile = OutputFiles_GetDateOutputFile(outputfiles) ;
  
  Result_t* r_s = Results_GetResult(OutputFiles_GetResults(outputfiles)) ;
  
  /* Open the date file for writing */
  TextFile_t* textfile = OutputFile_GetTextFile(outputfile + idate1) ; 
  FILE *fict = TextFile_OpenFile(textfile,"w") ;
  
  OutputFile_TypeOfCurrentFile = 't' ;
  
  int size = DistributedMS_NbOfProcessors ;
  int rank = DistributedMS_RankOfCallingProcess ;
  
  if(size > 1) {
    Mesh_BroadcastExplicitTerms(mesh) ;
    Mesh_BroadcastImplicitTerms(mesh) ;
  }
  
  if(rank > 0) return ;
  
  
  /* First lines: version and date */
  {
    time_t date ;
    time(&date) ;
    fprintf(fict,"# Version " BIL_VERSION ", %s",ctime(&date)) ;
    fprintf(fict,"# Time = %e\n",t) ;
  }
  
  
  {
    unsigned int headings[10] = {0,0,0,0,0,0,0,0,0,0} ;
    
    if(n_usedmodels > 10) {
      arret("OutputFiles_BackupSolutionAtTime (1): too many used models") ;
    }
  

    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Element_t* elt = el + ie ;
      Material_t* mat = Element_GetMaterial(elt) ;
      char*  codename = (mat) ? Material_GetCodeNameOfModel(mat) : NULL ;
      size_t usedmodelindex = Models_FindModelIndex(usedmodels,codename) ;
      unsigned int entete = headings[usedmodelindex] ;
      
      /* if(!strcmp(codename,ucodename)) { */
      if(mat) {
        int  nn = Element_GetNbOfNodes(elt) ;
        int  in ;
        
        /* We skip elements of the boundaries */
        if(Element_IsSubmanifold(elt)) continue ;
        
        for(in = 0 ; in < nn ; in++) {
          double *x_s = Element_GetNodeCoordinate(elt,in) ;
          int    nso ;
          int    j,k ;
        
          Element_FreeBuffer(elt) ;
          nso = Element_ComputeOutputs(elt,t,x_s,r_s) ;
          
          if(nso > OutputFiles_MaxNbOfViews) {
            arret("OutputFiles_BackupSolutionAtTime (1) : trop de valeurs") ;
          }

          if(nso == 0) continue ;
          
          /* Headings: Model and views */
          if(entete == 0) {
            entete = 1 ;
            headings[usedmodelindex] = 1 ;
            fprintf(fict,"# Model = %s\n",codename) ;
            fprintf(fict,"# Number of views = %d\n",nso) ;
            fprintf(fict,"# Numbers of components per view =") ;
            for(k = 0 ; k < nso ; k++) {
              int n = Result_GetNbOfValues(r_s + k) ;
              
              fprintf(fict," %d",n) ;
            }
            fprintf(fict,"\n") ;
            j = 1 ;
            fprintf(fict,"# Coordinates(%d)",j) ;
            j += 3 ;
            for(k = 0 ; k < nso ; k++) {
              char* name = Result_GetNameOfView(r_s + k) ;
              
              fprintf(fict," %s(%d)",name,j) ;
              j += Result_GetNbOfValues(r_s + k) ;
            }
            fprintf(fict,"\n") ;
          }
          
          /* Results per line */
          /* 1. Coordinates of node */
          for(j = 0 ; j < dim ; j++) {
            fprintf(fict,OutputFiles_RecordNumberFormat,x_s[j]) ;
          }
          for(j = dim ; j < 3 ; j++) {
            fprintf(fict,OutputFiles_RecordNumberFormat,0.) ;
          }
          
          /* 2. Components of views */
          for(k = 0 ; k < nso ; k++) {
            int n_r = Result_GetNbOfValues(r_s + k) ;
            
            for(j = 0 ; j < n_r ; j++) {
              fprintf(fict,OutputFiles_RecordNumberFormat,Result_GetValue(r_s + k)[j]) ;
            }
          }
          
          /* 3. End of line */
          fprintf(fict,"\n") ;
        }
      }
    }

  }
  
  
  /* Close the date file */
  TextFile_CloseFile(textfile) ;
}



void (OutputFiles_BackupSolutionAtPoint_)(OutputFiles_t* outputfiles,DataSet_t* dataset,double t,const char* mode)
/* Backup solutions at given points in the approriate output files */
{
  Mesh_t* mesh = DataSet_GetMesh(dataset) ;
  Points_t* points = DataSet_GetPoints(dataset) ;
  int dim = Mesh_GetDimension(mesh) ;
  size_t npt = Points_GetNbOfPoints(points) ;
  OutputFile_t* outputfile = OutputFiles_GetPointOutputFile(outputfiles) ;
  Result_t* r_s = Results_GetResult(OutputFiles_GetResults(outputfiles)) ;
  
  OutputFile_TypeOfCurrentFile = 'p' ;
  
  int rank = DistributedMS_RankOfCallingProcess ;
  
  
  /* Open point files for writing */
  //if(t == t_0) {
  if(String_Is(mode,"o")) {
    for(size_t p = 0 ; p < npt ; p++) {
      TextFile_t* textfile = OutputFile_GetTextFile(outputfile + p) ;
      Point_t* point = Points_GetPoint(points) + p ;
      double *xp = Point_GetCoordinate(point) ;
      Element_t* elt = Point_GetEnclosingElement(point) ;

      if(!elt) {
        Point_EnclosingElement(point,mesh) ;
        elt = Point_GetEnclosingElement(point) ;
      }

      int rankofelt = Element_RankOfSupportingProcessor(elt) ;
      
      if(rank == rankofelt) TextFile_OpenFile(textfile,"w") ;
    }
  }
  
  
  for(size_t p = 0 ; p < npt ; p++) {
    TextFile_t* textfile = OutputFile_GetTextFile(outputfile + p) ;
    Point_t* point = Points_GetPoint(points) + p ;
    double *xp = Point_GetCoordinate(point) ;
    Element_t* elt = Point_GetEnclosingElement(point) ;
    int rankofelt = Element_RankOfSupportingProcessor(elt) ;
    FILE *ficp = TextFile_GetFileStream(textfile) ;
    
    if(rank != rankofelt) continue ;
    
    /* First lines: version and point coordinates */
    //if(t == t_0) {
    if(String_Is(mode,"o")) {
      int    j ;
      time_t date ;
      time(&date) ;
    
      fprintf(ficp,"# Version " BIL_VERSION ", %s",ctime(&date)) ;
    
      fprintf(ficp,"# Point =") ;
      for(j = 0 ; j < dim ; j++) fprintf(ficp," %e",xp[j]) ;
      for(j = dim ; j < 3 ; j++) fprintf(ficp," 0") ;
      fprintf(ficp,"\n") ;
    }
    
    
    if(elt) {
      Material_t* mat = Element_GetMaterial(elt) ;
      
      if(mat) {
        char*  codename = Material_GetCodeNameOfModel(mat) ;
        int    nso ;
        int    i ;
        
        Element_FreeBuffer(elt) ;
        nso = Element_ComputeOutputs(elt,t,xp,r_s) ;
        
        if(nso > OutputFiles_MaxNbOfViews) {
          arret("OutputFiles_BackupSolutionAtPoint: too much values") ;
        }
  
        /* Headings: Model and views */
        //if(t == t_0) {
        if(String_Is(mode,"o")) {
          int    j ;
          
          fprintf(ficp,"# Model = %s\n",codename) ;
          fprintf(ficp,"# Number of views = %d\n",nso) ;
          fprintf(ficp,"# Numbers of components per view =") ;
          for(i = 0 ; i < nso ; i++) {
            int n = Result_GetNbOfValues(r_s + i) ;
              
            fprintf(ficp," %d",n) ;
          }
          fprintf(ficp,"\n") ;
          j = 1 ;
          fprintf(ficp,"# Time(%d)",j) ;
          j += 1 ;
          for(i = 0 ; i < nso ; i++) {
            char* name = Result_GetNameOfView(r_s + i) ;
            
            fprintf(ficp," %s(%d)",name,j) ;
            j += Result_GetNbOfValues(r_s + i) ;
          }
          fprintf(ficp,"\n") ;
        }
  
        /* Results per line */
        /* 1. Time */
        fprintf(ficp,OutputFiles_RecordNumberFormat,t) ;
        
        /* 2. Components of views */
        for(i = 0 ; i < nso ; i++) {
          int n_r = Result_GetNbOfValues(r_s + i) ;
          int    j ;
          
          for(j = 0 ; j < n_r ; j++) {
            fprintf(ficp,OutputFiles_RecordNumberFormat,Result_GetValue(r_s + i)[j]) ;
          }
        }
        
        /* 3. End of line */
        fprintf(ficp,"\n") ;
      }
    }
    
    /* We clean the stream */
    TextFile_CleanTheStream(textfile) ;
  }
}



Views_t* (OutputFiles_CreateGlobalViews)(OutputFiles_t* outputfiles,Models_t* usedmodels,TextFile_t* textfile)
/* We use informations found in the date file textfile to build the global views and 
 * initialize the local views per used model including the index in the global views.
 * Return a pointer to Views_t.
 */
{
  Model_t*  usedmodel  = Models_GetModel(usedmodels) ;
  size_t n_usedmodels = Models_GetNbOfModels(usedmodels) ;
  
  int    nbofviews ;
  char   nameofview[OutputFiles_MaxNbOfViews][OutputFiles_MaxLengthOfViewName] ;
  int    nbcompofview[OutputFiles_MaxNbOfViews] ;


  /* Open the date file for reading */
  TextFile_OpenFile(textfile,"r") ;
      
  {
    char*  pline ;
    int    imodel = -1 ;
    int    usedmodelindex ;
    
    nbofviews = 0 ;
      
    while((pline = OutputFiles_ReadLineFromCurrentFilePosition(outputfiles,textfile))) {
      
      if(pline[0] == '#') {
        
        /* Model */
        if(strstr(pline,"Model")) {
          char name[Model_MaxLengthOfKeyWord] ;
          
          pline = strchr(pline,'=') + 1 ;
          
          sscanf(pline,"%s",name) ;
          
          usedmodelindex = Models_FindModelIndex(usedmodels,name) ;
          
          imodel += 1 ;
            
        /* Nb of views */
        } else if(strstr(pline,"nombre de vues") || \
                  strstr(pline,"Number of views")) {
          int n ;
          
          pline = strchr(pline,'=') + 1 ;
          
          sscanf(pline,"%d",&n) ;
          
          /* We store the number of views in the current used model */
          {
            Model_t* model = usedmodel + usedmodelindex ;
            Views_t*  views = Model_GetViews(model) ;
            
            Views_GetNbOfViews(views) = n ;
          }
          
        /* Nb of components per view */
        } else if(strstr(pline,"nombres de composantes/vue") || \
                  strstr(pline,"Numbers of components per view")) {
          Model_t* model = usedmodel + usedmodelindex ;
          Views_t*  views = Model_GetViews(model) ;
          View_t*   view  = Views_GetView(views) ;
          int nviews = Views_GetNbOfViews(views) ;
          int i ;
          
          pline = strchr(pline,'=') + 1 ;
                    
          /* We store the number of components in the current used model */
          for(i = 0 ; i < nviews ; i++) {
            int n ;
            
            sscanf(pline,"%d",&n) ;
            
            View_GetNbOfComponents(view + i) = n ;
            
            pline = strpbrk(pline,"139") + 1 ;
          }
          
        /* Name of views */
        } else if(strstr(pline,"coordonnees") || \
                  strstr(pline,"Coordinates")) {
          Model_t* model = usedmodel + usedmodelindex ;
          Views_t*  views = Model_GetViews(model) ;
          View_t*   view  = Views_GetView(views) ;
          int nviews = Views_GetNbOfViews(views) ;
          int i ;
          
          pline = strstr(pline,"(1)") + strlen("(1)") ;
          
          /* We store the view names in the current used model */
          for(i = 0 ; i < nviews ; i++) {
            int    n = View_GetNbOfComponents(view + i) ;
            char*  name = View_GetNameOfView(view + i) ;
            char*  p ;
            int j ;
            
            //sscanf(pline,"%s",name) ;
            //sscanf(pline,"%*[ ]%[^(]",name) ;
            String_ScanStringUntil(pline,name,"(") ;
            
            if((p = strrchr(name,'('))) *p = '\0' ;
            
            /* Does this view have already been met? */
            for(j = 0 ; j < nbofviews ; j++) {
              if(!strcmp(nameofview[j],name) && (nbcompofview[j] == n)) break ;
            }
            
            /* If never met, we increment the nb of views */
            if(j == nbofviews) {
              strcpy(nameofview[j],name) ;
              nbcompofview[j] = n ;
              nbofviews += 1 ;
            }
            
            /* Here we store the index in the global views */
            View_GetGlobalIndex(view + i) = j ;
            
            pline = strchr(pline,')') + 1 ;
          }
          
          if(imodel == n_usedmodels - 1) break ;
        }
      }
    }
    
    if(imodel != n_usedmodels - 1) {
      Message_Direct("The nb of models doesn't fit that in the datafile\n") ;
    }
    
  }
  
  TextFile_CloseFile(textfile) ;
  
  /* We build global views */
  {
    Views_t* views = Views_Create(nbofviews) ;
    View_t*  view  = Views_GetView(views) ;
    int i ;
    
    for(i = 0 ; i < nbofviews ; i++) {
      View_t* vi = view + i ;
      char* name = View_GetNameOfView(vi) ;
      
      strcpy(name,nameofview[i]) ;
      View_GetNbOfComponents(vi) = nbcompofview[i] ;
    }
    
    return(views) ;
  }
  
}
