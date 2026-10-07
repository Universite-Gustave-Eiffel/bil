#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
#include "DataFile.h"
#include "Mry.h"
#include "String_.h"
#include "Field.h"

/*
static void   lit_grille(FieldGrid_t* ,int,char*) ;
*/
//static void           Field_ReadGrid(FieldGrid_t*,int,char*) ;
//static double   champaffine(double*,int,FieldAffine_t) ;
//static double   champgrille(double*,int,FieldGrid_t) ;





Field_t* (Field_New)(void)
{
  Field_t* field = (Field_t*) Mry_New(Field_t) ;

  /* Allocation of space for the type of field */
  {
    char* type = (char*) Mry_New(char,Field_MaxLengthOfKeyWord) ;
    
    Field_SetType(field,type) ;
    strcpy(Field_GetType(field),"none") ;
  }

  Field_SetFieldFormat(field,NULL) ;
  
  return(field) ;
}



void (Field_Delete)(void* self)
{
  Field_t* field = (Field_t*) self ;
  
  if(field) {
    void* fieldfmt = Field_GetFieldFormat(field) ;
    char* type = Field_GetType(field) ;
    
    if(fieldfmt) {
      if(String_Is(type,"grid")) {
        FieldGrid_Delete(fieldfmt) ;
      }
      
      Mry_Free(fieldfmt) ;
      Field_SetFieldFormat(field,NULL) ;
    }

    if(type) {
      Mry_Free(type) ;
      Field_SetType(field,NULL) ;
    }
  }
}



void (Field_Scan)(Field_t* field,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  char type[DataFile_MaxLengthOfTextLine] ;

  /* The type (if given )*/
  {
    int n = String_FindAndScanExp(line,"Type",","," = %s",type) ;

    if(n) {
      if(strlen(type) > Field_MaxLengthOfKeyWord - 1) {
        arret("Field_Scan: too long type") ;
      }
    } else {
      /* Default type */
      strcpy(type,"affine");
    }
  }
  
  /* The field data */
  {
    /* Affine field
     * ------------ */
    if(String_Is(type,"affine")) {
      double v;
      double g[3] = {0.,0.,0.};
      double x[3] = {0.,0.,0.};
      
      /* Value */
      {
        int n = String_FindAndScanExp(line,"Val",","," = %lf",&v) ;
        
        if(!n) {
          arret("Field_Scan: no value") ;
        }
      }
      
      /* Gradient */
      {
        int n = String_FindAndScanExp(line,"Grad",","," = ") ;
        
        if(n) {
          char* pline = String_GetAdvancedPosition ;
          
          /* If no conversion can be made, strtod return 0 */
          for(int j = 0 ; j < 3 ; j++) {
            g[j] = strtod(pline,&pline) ;
          }
        } else {
          arret("Field_Scan: no gradient") ;
        }
      }
      
      /* Point */
      {
        int n = String_FindAndScanExp(line,"Poin",","," = ") ;
        
        if(n) {
          char* pline = String_GetAdvancedPosition ;
        
          for(int j = 0 ; j < 3 ; j++) {
            x[j] = strtod(pline,&pline) ;
          }
        } else {
          arret("Field_Scan: no point") ;
        }
      }

      {
        std::string type_str = "affine";

        Field_Set(field,type_str,v,g,x) ;
      }
      
      return ;
    }



    /* Grid field
     * ---------- */
    if(String_Is(type,"grid")) {
      char name[DataFile_MaxLengthOfTextLine] ;

      /* File */
      {
        int n = String_FindAndScanExp(line,"File",","," = %s",name) ;
      
        if(!n) {
          arret("Field_Scan: no file") ;
        }
        
        if(strlen(name) > Field_MaxLengthOfFileName - 1) {
          arret("Field_Scan: too long file name") ;
        }
      }

      {
        std::string type_str = "grid";
        std::string name_str = name;

        Field_Set(field,type_str,name_str);
      }
      
      return ;
    }



    /* Random field
     * ------------ */
    if(String_Is(type,"random")) {
        double v, ranlen;
      
      /* Value */
      {
        int n = String_FindAndScanExp(line,"Val",","," = %lf",&v) ;
        
        if(!n) {
          arret("Field_Scan: no value") ;
        }
      }
      
      /* Random range length */
      {
        int n = String_FindAndScanExp(line,"Ran",","," = %lf",&ranlen) ;
        
        if(!n) {
          ranlen = 0 ;
        } else {
          srand(time(0)) ;
        }
      }

      {
        std::string type_str = "random";

        Field_Set(field,type_str,v,ranlen);
      }
      
      return ;
    }



    {
      arret("Field_Scan: unknown type") ;
    }
  }
  
  return ;
}



FieldGrid_t* (FieldGrid_Create)(char const* filename)
{
  FieldGrid_t* grid = (FieldGrid_t*) Mry_New(FieldGrid_t) ;
  size_t n_x = 1,n_y = 1,n_z = 1 ;

  /* Allocation of memory space for the file name */
  {
    char* name = (char*) Mry_New(char,strlen(filename)+1) ;
    
    FieldGrid_GetFileName(grid) = name ;
    
    strcpy(name,filename) ;
  }


  /* Read the numbers */
  {
    DataFile_t* dfile = DataFile_New(filename) ;
    char*   line = DataFile_ReadLineFromCurrentFilePositionInString(dfile) ;
    
    {
      char* c = line ;
      
      if((n_x = strtoul(c,&c,10)) == 0) n_x = 1 ;
      if((n_y = strtoul(c,&c,10)) == 0) n_y = 1 ;
      if((n_z = strtoul(c,&c,10)) == 0) n_z = 1 ;
    }
  
    DataFile_Delete(dfile) ;
    Mry_Free(dfile) ;

    FieldGrid_SetNbOfPointsAlongX(grid,n_x);
    FieldGrid_SetNbOfPointsAlongY(grid,n_y);
    FieldGrid_SetNbOfPointsAlongZ(grid,n_z);
  }
  
  
  /* Allocation of memory space for the coordinates */
  {
    double* x = (double*) Mry_New(double,n_x + n_y + n_z) ;
    double* y = x + n_x ;
    double* z = y + n_y ;

    /* Initialization of the coordinate */
    if(n_x > 0) x[0] = 0. ;
    if(n_y > 0) y[0] = 0. ;
    if(n_z > 0) z[0] = 0. ;

    FieldGrid_SetCoordinateAlongX(grid,x) ;
    FieldGrid_SetCoordinateAlongY(grid,y) ;
    FieldGrid_SetCoordinateAlongZ(grid,z) ;
  }


  /* Allocation of memory space for the values */
  {
    double* v = (double*) Mry_New(double,n_x*n_y*n_z) ;
    
    FieldGrid_SetValue(grid,v) ;
  }

  
  /* Read the grid */
  {
    DataFile_t* dfile = DataFile_New(filename) ;
    char*   line = DataFile_ReadLineFromCurrentFilePositionInString(dfile) ;
    
    line = DataFile_GetCurrentPositionInFileContent(dfile) ;
    
    /* Read the coordinates of the grid */
    {
      size_t n = 0 ;
    
      if(n_x > 1) n += n_x ;
      if(n_y > 1) n += n_y ;
      if(n_z > 1) n += n_z ;
  
      if(n > 1) {
        double* x = FieldGrid_GetCoordinateAlongX(grid) ;
      
        String_ScanArray(line,n," %lf",x) ;
      
        line = String_GetAdvancedPosition ;
      }
    }

    /* Read the values at the grid points */
    {
      size_t n = n_x*n_y*n_z ;
      double* v = FieldGrid_GetValue(grid) ;
      
      String_ScanArray(line,n," %lf",v) ;
    }
  
    DataFile_Delete(dfile) ;
    Mry_Free(dfile) ;
  }

  return(grid) ;
}


#if 0
void (FieldGrid_Delete)(void* self)
{
  FieldGrid_t* field = (FieldGrid_t*) self ;
  
  if(field) {
    {
      char* name = FieldGrid_GetFileName(field);

      if(name) {
        Mry_Free(name);
        FieldGrid_SetFileName(field,NULL);
      }
    }

    {
      double* x = FieldGrid_GetCoordinateAlongX(field);
      
      if(x) {
        Mry_Free(x);
        FieldGrid_SetCoordinateAlongX(field,NULL);
      }
    }

    {
      double* v = FieldGrid_GetValue(field);

      if(v) {
        Mry_Free(v);
        FieldGrid_SetValue(field,NULL);
      }
    }
  }
}
#endif



#if 0
double (Field_ComputeValueAtPoint)(Field_t* ch,double* x,int dim)
{
  char*   type = Field_GetType(ch) ;
  double v ;

  if(!strcmp(type,"affine")) {
    FieldAffine_t* affine = (FieldAffine_t*) Field_GetFieldFormat(ch) ;
    v = champaffine(x,dim,*affine) ;
  } else if(!strcmp(type,"grid")) {
    FieldGrid_t* grille = (FieldGrid_t*) Field_GetFieldFormat(ch) ;
    v = champgrille(x,dim,*grille) ;
  } else if(!strcmp(type,"random")) {
    FieldRandom_t* cst = (FieldRandom_t*) Field_GetFieldFormat(ch) ;
    double value = FieldConstant_GetValue(cst) ;
    double ranlen = FieldConstant_GetRandomRangeLength(cst) ;

    {
      double rmax = (double)RAND_MAX ;
      double r = (double)rand() / rmax - 0.5 ;
      
      v = value + ranlen*r ;
    }
  } else {
    arret("Field_ComputeValueAtPoint : type non prevu") ;
    return(0.) ;
  }

  return(v) ;
}


double champaffine(double* x,int dim,FieldAffine_t ch)
{
  double v = ch._v,*x0 = ch._x,*grd = ch._g ;
  int    i ;
  
  for(i=0;i<dim;i++) v += grd[i]*(x[i] - x0[i]) ;
  
  return(v) ;
}


double champgrille(double* p,int dim,FieldGrid_t ch)
{
#define V(i,j,k) (ch._v[(i) + (j)*n_x + (k)*n_x*n_y])
  unsigned long int n_x = ch._n_x,n_y = ch._n_y,n_z = ch._n_z ;
  double* x = ch._x,*y = ch._y,*z = ch._z ;
  double x0 = p[0],y0 = p[1],z0 = p[2] ;
  double v ;
  unsigned long int ix1,iy1,iz1,ix2,iy2,iz2 ;

  if(dim > 0) {
    if(x0 <= x[0]) {
      ix1 = 0 ;
      ix2 = ix1 ;
    } else if(x0 >= x[n_x - 1]) {
      ix1 = n_x - 1 ;
      ix2 = ix1 ;
    } else {
      for(ix1=0;ix1<n_x-1;ix1++)  if(x0 < x[ix1+1]) break  ;
      ix2 = ix1 + 1 ;
    }
  }

  if(dim > 1) {
    if(y0 <= y[0]) {
      iy1 = 0 ;
      iy2 = iy1 ;
    } else if(y0 >= y[n_y - 1]) {
      iy1 = n_y - 1 ;
      iy2 = iy1 ;
    } else {
      for(iy1=0;iy1<n_y-1;iy1++)  if(y0 < y[iy1+1]) break  ;
      iy2 = iy1 + 1 ;
    }
  }

  if(dim > 2) {
    if(z0 <= z[0]) {
      iz1 = 0 ;
      iz2 = iz1 ;
    } else if(z0 >= z[n_z - 1]) {
      iz1 = n_z - 1 ;
      iz2 = iz1 ;
    } else {
      for(iz1=0;iz1<n_z-1;iz1++)  if(z0 < z[iz1+1]) break  ;
      iz2 = iz1 + 1 ;
    }
  }

  if(dim == 1) {
    double x1 = x[ix1],x2 = x[ix2] ;
    double v1 = V(ix1,0,0) ;
    double v2 = V(ix2,0,0) ;
    
    if(ix1 == ix2) v = v1 ;
    else v = v1 + (v2 - v1)*(x0 - x1)/(x2 - x1) ;

  } else if(dim == 2) {
    double x1 = x[ix1],x2 = x[ix2] ;
    double y1 = y[iy1],y2 = y[iy2] ;
    double d_x = x2 - x1,d_y = y2 - y1 ;
    double v11 = V(ix1,iy1,0) ;
    double v21 = V(ix2,iy1,0) ;
    double v12 = V(ix1,iy2,0) ;
    double v22 = V(ix2,iy2,0) ;
    
    if(ix1 == ix2 && iy1 == iy2) {
      v = v11 ;
      
    } else if(ix2 == ix1 + 1 && iy2 == iy1 + 1) {
      
      v = v11 + (v21 - v11)*(x0 - x1)/d_x + (v12 - v11)*(y0 - y1)/d_y \
      + (v22 + v11 - v21 - v12)*(x0 - x1)*(y0 - y1)/(d_x*d_y) ;
      
    } else if(ix1 == ix2) {
      
      v = v11 + (v12 - v11)*(y0 - y1)/d_y ;
      
    } else if(iy1 == iy2) {
      
      v = v11 + (v21 - v11)*(x0 - x1)/d_x ;
      
    } else {
      arret("champgrille : non prevu") ;
    }
    
    arret("champgrille : non prevu") ;
    
  } else if(dim == 3) {
    
    arret("champgrille : non prevu") ;
    
  } else {
    
    arret("champgrille : non prevu") ;
    
  }

  return(v) ;
#undef V
}
#endif
