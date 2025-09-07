#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "Options.h"
#include "Mesh.h"
#include "Message.h"
#include "BilConfig.h"
#include "Mry.h"
#include "DistributedMS.h"
#include "CoordinateFormat.h"



CoordinateFormat_t* (CoordinateFormat_Create)(Mesh_t* mesh,Options_t* options,const int imatrix)
/** Create a matrix in CoordinateFormat format with duplicate entries */
{
  CoordinateFormat_t* cfmt = (CoordinateFormat_t*) Mry_New(CoordinateFormat_t) ;
  
  {
    /* Nb of entries */
    {
      size_t nnz = Mesh_ComputeNbOfSelectedMatrixEntries(mesh,imatrix) ;
      
      CoordinateFormat_GetNbOfNonZeroValues(cfmt) = nnz ;
    }
    
    /* Allocate memory space for the values */
    {
      size_t nnz = CoordinateFormat_GetNbOfNonZeroValues(cfmt) ;
      /* The length required by ma38 must not be lower than 2*nnz */
      size_t ff = Options_GetFillFactor(options) ;
      size_t lv = ff*2*nnz ;
      double* v = (double*) Mry_New(double,lv) ;
      
      CoordinateFormat_GetLengthOfArrayValue(cfmt) = lv ;
      CoordinateFormat_GetNonZeroValue(cfmt) = v ;
    }
    
    /* Allocate memory space for the indices, namely:
     * the row and column indices;
     * some other informations provided by ma38 */
    {
      size_t nnz = CoordinateFormat_GetNbOfNonZeroValues(cfmt) ;
      size_t n = Mesh_GetNbOfMatrixColumns(mesh)[imatrix] ;
      /* The length required by ma38 must not be lower than 3*nnz+2*n+1 */
      size_t ff = Options_GetFillFactor(options) ;
      size_t lindex = ff*(3*nnz + 2*n + 1) ;
      int* index = (int*) Mry_New(int,lindex) ;
      
      CoordinateFormat_GetLengthOfArrayIndex(cfmt) = lindex ;
      CoordinateFormat_GetIndex(cfmt)              = index ;
    }
  }
  
  return(cfmt) ;
}



void (CoordinateFormat_Delete)(void* self)
{
  CoordinateFormat_t* ac = (CoordinateFormat_t*) self ;
  
  free(CoordinateFormat_GetNonZeroValue(ac)) ;
  free(CoordinateFormat_GetIndex(ac)) ;
}




size_t (CoordinateFormat_AssembleElementMatrix)(CoordinateFormat_t* a,double* ke,int* col,int* row,int ndof,size_t nnz)
/** Assemble the local matrix ke into the global matrix a 
 *  Return the nb of entries */
{
#define KE(i,j) (ke[(i)*ndof+(j)])
  double*  val = CoordinateFormat_GetNonZeroValue(a) ;
  int*  colind = CoordinateFormat_GetColumnIndexOfValue(a) ;
  int*  rowind = CoordinateFormat_GetRowIndexOfValue(a) ;
  size_t len = 0 ;
  int rank = DistributedMS_RankOfCallingProcess ;
  
  if(rank > 0) return(len) ;
  
  
  {
    for(int je = 0 ; je < ndof ; je++) {
      int jcol = col[je] ;
    
      if(jcol < 0) continue ;

      for(int ie = 0 ; ie < ndof ; ie++) {
        int irow = row[ie] ;
      
        if(irow < 0) continue ;
      
        /* Row (and column) indices outside the range 1,nnz are ignored in ma38 */
        if(ke) {
          if(KE(ie,je) == 0.) irow = -1 ;
        
          val[nnz]    = KE(ie,je) ;
          colind[nnz] = jcol + 1 ;
          rowind[nnz] = irow + 1 ;
          nnz++ ;
        }
        
        len += 1 ;
      }
    }
  }
  
  return(len) ;

#undef KE
}




void (CoordinateFormat_PrintMatrix)(CoordinateFormat_t* a,size_t n_col,const char* keyword)
{
  double* val  = CoordinateFormat_GetNonZeroValue(a) ;
  int*  colind = CoordinateFormat_GetColumnIndexOfValue(a) ;
  int*  rowind = CoordinateFormat_GetRowIndexOfValue(a) ;
  size_t   nnz    = CoordinateFormat_GetNbOfNonZeroValues(a) ;

  fprintf(stdout,"\n") ;
  fprintf(stdout,"Matrix in coordinate format with duplicate entries:\n") ;
  fprintf(stdout,"order = %lu ; nb of entries = %lu\n",n_col,nnz) ;
  fprintf(stdout,"\n") ;
  
  {    
    for(size_t k = 0 ; k < nnz ; k++) {
    
      fprintf(stdout,"%lu: K(%d,%d) = %e",k,rowind[k],colind[k],val[k]) ;
    
      fprintf(stdout,"\n") ;
    }
  }
}
