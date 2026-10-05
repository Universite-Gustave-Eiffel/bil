#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Message.h"
#include "DataFile.h"
#include "Mesh.h"
#include "Mry.h"
#include "Point.h"






Point_t*  (Point_New)(void)
{
  Point_t* point = (Point_t*) Mry_New(Point_t) ;
  
  
  /* Allocation of space for the region name */
  {
    char* name = (char*) Mry_New(char,Point_MaxLengthOfRegionName) ;
    
    Point_SetRegionName(point,name) ;
  }
  
  strcpy(Point_GetRegionName(point),"\0") ;
  
  return(point) ;
}



void (Point_Delete)(void* self)
{
  Point_t* point = (Point_t*) self ;
  
  if(point) {
    {
      char* name = Point_GetRegionName(point) ;
    
      if(name) {
        Mry_Free(name) ;
        Point_SetRegionName(point,NULL);
      }
    }
  }
}



void (Point_Scan)(Point_t* point,char* line)
{
  char name[Point_MaxLengthOfRegionName] ;
  char* region;
  double coor[3] = {0,0,0} ;
  
  /* Region */
  {
    int n = String_FindAndScanExp(line,"Reg",","," = %s",name) ;
    
    if(n) {
      region = name;
    } else {
      region = nullptr;
    }
  }

  /* Coordinates */
  {
    int n = String_FindAndScanExp(line,"Coor",","," = ") ;
    char* c;
    
    if(n) {
      c = String_GetAdvancedPosition ;
    } else if(!region) {
      c = line;
    } else {
      arret("Point_Scan: no coordinates") ;
    }
    
    String_ScanArray(c,3," %lf",coor) ;
  }

  Point_Set(point,coor,region);
}



void (Point_EnclosingElement)(Point_t* point,Mesh_t* mesh)
/** Set a pointer to the element which encloses the point */
{
  int dim = Mesh_GetDimension(mesh) ;
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  double* pt = Point_GetCoordinate(point) ;
  char* reg = Point_GetRegionName(point) ;
  double d0 = 0. ;
  Element_t* enclosel = NULL;

  for(size_t i = 0 ; i < n_el ; i++) {
    int  nn = Element_GetNbOfNodes(el + i) ;
    char* reg_el = Element_GetRegionName(el + i) ;
    double x_s[3] = {0.,0.,0.} ;
    double d = 0. ;
    
    /* Select the element whose center is the closest to the point */
    if(String_Is(reg,"\0") || String_Is(reg,reg_el)) {      
      for(int j = 0 ; j < dim ; j++) {      
        x_s[j] = 0. ;
      
        for(int in = 0 ; in < nn ; in++) {
          x_s[j] += Element_GetNodeCoordinate(el + i,in)[j] ;
        }
      
        x_s[j] /= nn ;
        x_s[j] -= pt[j] ;
      }
    
      for(int j = 0 ; j < dim ; j++) {
        d += x_s[j]*x_s[j] ;
      }
    
      if(enclosel == NULL || d < d0) {
        d0 = d ;
        enclosel = el + i;
      }
    }
  }
  
  Point_SetEnclosingElement(point,enclosel);
}
