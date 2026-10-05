#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Message.h"
#include "DataFile.h"
#include "Mesh.h"
#include "Mry.h"
#include "String_.h"
#include "Points.h"
#include "Point.h"



Points_t*  (Points_New)(void)
{
  Points_t* points = (Points_t*) Mry_New(Points_t) ;

  /* Nb of points */
  Points_SetNbOfPoints(points,0) ;
  
  /* Pointer to point */
  {
    Point_t* point = Mry_Create(Point_t,Points_MaxNbOfPoints,Point_New()) ;

    Points_SetPoint(points,point) ;
  }
  
  return(points) ;
}



void  (Points_Delete)(void* self)
{
  Points_t* points = (Points_t*) self ;
  
  if(points) {
    Point_t* point = Points_GetPoint(points) ;
    
    if(point){
      Mry_Delete(point,Points_MaxNbOfPoints,Point_Delete) ;
      Mry_Free(point) ;
      Points_SetPoint(points,NULL) ;
    }
  }
}



#if 0
Points_t*  (Points_Create)(DataFile_t* datafile,Mesh_t* mesh)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"POIN,Points",",") ;
  size_t n_points = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  Points_t* points = Points_New() ;
  
  
  Message_Direct("Enter in %s","Points") ;
  Message_Direct("\n") ;
  
  
  c = String_SkipLine(c) ;

   
  Points_SetNbOfPoints(points,n_points) ;
  
  /* Read in the input data file */
  {
    Point_t* point = Points_GetPoint(points) ;
    char* c1 = String_CopyLine(c) ;
    
    
    /* If no token "Reg" is found in the line c1 */
    if(!String_FindToken(c1,"Reg")) {
      int dim = Mesh_GetDimension(mesh) ;
      double* x = (double*) Mry_New(double,3*n_points) ;
    
      String_ScanArray(c,dim*n_points," %lf",x) ;
      
      /* The coordinates */
      {     
        for(size_t i = 0 ; i < n_points ; i++) {
          double* coor = Point_GetCoordinate(point + i) ;
      
          for(int j = 0 ; j < dim ; j++) {
            coor[j] = x[dim*i + j] ;
          }
        }
      }
    
      Mry_Free(x) ;

    /* If a token "Reg" is found in the line */
    } else {    
      DataFile_SetCurrentPositionInFileContent(datafile,c) ;
    
      for(size_t i = 0 ; i < n_points ; i++) {
        char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  
        Message_Direct("Enter in %s %d","Point",i+1) ;
        Message_Direct("\n") ;

        Point_EnclosingElement(point + i,mesh) ;
        
        Point_Scan(point + i,line) ;
      }
    }
  }
  
  return(points) ;
}
#else
Points_t*  (Points_Create)(DataFile_t* datafile,Mesh_t* mesh)
{
  Points_t* points = Points_New() ;

  Points_Scan(points,datafile,mesh);
  
  return(points) ;
}
#endif


void  (Points_Scan)(Points_t* points,DataFile_t* datafile,Mesh_t* mesh)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"POIN,Points",",") ;
  size_t n_points = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  
  
  Message_Direct("Enter in %s","Points") ;
  Message_Direct("\n") ;
  
  
  c = String_SkipLine(c) ;

   
  Points_SetNbOfPoints(points,n_points) ;
  
  /* Read in the input data file */
  {
    Point_t* point = Points_GetPoint(points) ;
    char* c1 = String_CopyLine(c) ;
    
    
    /* If no token "Reg" is found in the line c1 */
    if(!String_FindToken(c1,"Reg")) {
      int dim = Mesh_GetDimension(mesh) ;
      double* x = (double*) Mry_New(double,3*n_points) ;
    
      String_ScanArray(c,dim*n_points," %lf",x) ;
      
      /* The coordinates */
      {     
        for(size_t i = 0 ; i < n_points ; i++) {
          double* coor = Point_GetCoordinate(point + i) ;
      
          for(int j = 0 ; j < dim ; j++) {
            coor[j] = x[dim*i + j] ;
          }
        }
      }
    
      Mry_Free(x) ;

    /* If a token "Reg" is found in the line */
    } else {    
      DataFile_SetCurrentPositionInFileContent(datafile,c) ;
    
      for(size_t i = 0 ; i < n_points ; i++) {
        char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  
        Message_Direct("Enter in %s %d","Point",i+1) ;
        Message_Direct("\n") ;

        Point_EnclosingElement(point + i,mesh) ;
        
        Point_Scan(point + i,line) ;
      }
    }
  }
  
  return ;
}
