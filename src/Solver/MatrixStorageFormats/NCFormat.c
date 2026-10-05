#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "Options.h"
#include "Mesh.h"
#include "Element.h"
#include "Node.h"
#include "Message.h"
#include "BilConfig.h"
#include "Mry.h"
#include "NCFormat.h"



/* Extern functions */



NCFormat_t* (NCFormat_Create)(Mesh_t* mesh,const int imatrix)
/** Create a matrix in NCFormat */
{
  NCFormat_t* ncformat = (NCFormat_t*) Mry_New(NCFormat_t) ;
  size_t n_col = Mesh_GetNbOfMatrixColumns(mesh)[imatrix] ;
  /* Working memory */
  size_t* colptr0 = (size_t*) Mry_New(size_t,n_col + 1) ;
  size_t nnz_max;
  
  if(imatrix >= Mesh_GetNbOfMatrices(mesh)) {
    arret("NCFormat_Create") ;
  }


  /* We compute in colptr0 an over-estimated nb of terms 
   * in each column of the matrix */
  {
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    
    for(size_t i = 0 ; i < n_col + 1 ; i++) colptr0[i] = 0 ;

    /* Max nb of terms per column */
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      int neq = Element_GetNbOfEquations(el + ie) ;
    
      for(int i = 0 ; i < Element_GetNbOfNodes(el + ie) ; i++) {
        Node_t* node_i = Element_GetNode(el + ie,i) ;
        int ieq ;
      
        for(ieq = 0 ; ieq < neq ; ieq++) {
          int ij = i*neq + ieq ;
          int ii = Element_GetUnknownPosition(el + ie)[ij] ;
          int jcol ;
        
          if(ii < 0) continue ;
        
          //jcol = Node_GetMatrixColumnIndex(node_i)[ii] ;
          jcol = Node_GetSelectedMatrixColumnIndexOf(node_i,ii,imatrix) ;
        
          if(jcol < 0) continue ;
          colptr0[jcol+1] += Element_GetNbOfNodes(el + ie)*neq ;
        }
      }
    }

    colptr0[0] = 0 ; /* imposed to 0 ! */
    
    /* This is the cumulative nb of terms i.e. this is where to start
     * the column in stored non-zero terms of the matrix. So an
     * over-estimated nb of terms for the matrix is colptr0[n_col] */
    for(size_t i = 0 ; i < n_col ; i++) colptr0[i+1] += colptr0[i] ;

    nnz_max = colptr0[n_col];
  }


  /* Allocation of space for colptr */
  {
    size_t* colptr = (size_t*) Mry_New(size_t,n_col+1) ;
    
    NCFormat_GetFirstNonZeroValueIndexOfColumn(ncformat) = colptr ;
  }
    
  /* Allocation of space for rowind */
  {
    size_t* rowind = (size_t*) Mry_New(size_t,nnz_max) ;
    
    NCFormat_GetRowIndexOfNonZeroValue(ncformat) = rowind ;
  }


  /* Initialize colptr and rowind */
  {
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    size_t* colptr = NCFormat_GetFirstNonZeroValueIndexOfColumn(ncformat) ;
    size_t* rowind = NCFormat_GetRowIndexOfNonZeroValue(ncformat) ;
    
    for(size_t i = 0 ; i < n_col + 1 ; i++) colptr[i] = 0 ;

    for(size_t ie = 0 ; ie < n_el ; ie++) {
      int neq = Element_GetNbOfEquations(el + ie) ;
    
      for(int i = 0 ; i < Element_GetNbOfNodes(el + ie) ; i++) {
        Node_t* node_i = Element_GetNode(el + ie,i) ;
        int ieq ;
      
        for(ieq = 0 ; ieq < neq ; ieq++) {
          int iieq = i*neq + ieq ;
          int ii = Element_GetUnknownPosition(el + ie)[iieq] ;
          int irow ;
          int j ;
        
          if(ii < 0) continue ;
        
          //irow = Node_GetMatrixColumnIndex(node_i)[ii] ;
          irow = Node_GetSelectedMatrixColumnIndexOf(node_i,ii,imatrix) ;
          if(irow < 0) continue ;
        
          for(j = 0 ; j < Element_GetNbOfNodes(el + ie) ; j++) {
            Node_t* node_j = Element_GetNode(el + ie,j) ;
            int jeq ;
          
            for(jeq = 0 ; jeq < neq ; jeq++) {
              int jjeq = j*neq + jeq ;
              int jj = Element_GetUnknownPosition(el + ie)[jjeq] ;
              int jcol ;
              size_t k ;
            
              if(jj < 0) continue ;
            
              //jcol = Node_GetMatrixColumnIndex(node_j)[jj] ;
              jcol = Node_GetSelectedMatrixColumnIndexOf(node_j,jj,imatrix) ;
              if(jcol < 0) continue ;
            
              /* on verifie que irow n'est pas deja enregistre */
              for(k = 0 ; k < colptr[jcol + 1] ; k++) {
                if(irow == rowind[colptr0[jcol] + k]) break ;
              }
            
              if(k < colptr[jcol + 1]) continue ;
            
              rowind[colptr0[jcol] + colptr[jcol + 1]] = irow ;
              colptr[jcol + 1] += 1 ;
            
              if(irow == jcol) continue ;
            
              rowind[colptr0[irow] + colptr[irow + 1]] = jcol ;
              colptr[irow + 1] += 1 ;
            }
          }
        }
      }
    }
  }


  /* compression of rowind */
  {
    size_t* colptr = NCFormat_GetFirstNonZeroValueIndexOfColumn(ncformat) ;
    size_t* rowind = NCFormat_GetRowIndexOfNonZeroValue(ncformat) ;
    size_t j = 0 ;
    
    colptr[0] = 0 ;
    
    for(size_t i = 0 ; i < n_col ; i++) {      
      for(size_t k = 0 ; k < colptr[i + 1] ; k++) {
        rowind[j++] = rowind[colptr0[i] + k] ;
      }
      
      colptr[i + 1] += colptr[i] ;
    }

    if(colptr0[n_col] < colptr[n_col]) {
      arret("NCFormat_Create: memory issue") ;
    }
  }

  Mry_Free(colptr0) ;


  {
    size_t* colptr = NCFormat_GetFirstNonZeroValueIndexOfColumn(ncformat) ;
  
    NCFormat_GetNbOfNonZeroValues(ncformat) = colptr[n_col] ;
  }


  /* reallocate the memory space */
  {
    size_t* rowind = NCFormat_GetRowIndexOfNonZeroValue(ncformat) ;
    size_t nnz = NCFormat_GetNbOfNonZeroValues(ncformat) ;
    size_t* rowind1 = (size_t*) Mry_Realloc(rowind,nnz_max*sizeof(size_t),nnz*sizeof(size_t)) ;
    
    if(rowind1 != rowind) {
      NCFormat_GetRowIndexOfNonZeroValue(ncformat) = rowind1 ;
      Message_Warning("NCFormat_Create: new memory allocation") ;
    }
  }
  

  /* Allocation of space for the matrix */
  {
    size_t nnz = NCFormat_GetNbOfNonZeroValues(ncformat) ;
    double* nzval = (double*) Mry_New(double,nnz) ;
    
    NCFormat_GetNonZeroValue(ncformat) = nzval ;
  }
  
  return(ncformat) ;
}




void (NCFormat_Delete)(void* self)
{
  NCFormat_t* a = (NCFormat_t*) self ;
  
  Mry_Free(NCFormat_GetFirstNonZeroValueIndexOfColumn(a)) ;
  Mry_Free(NCFormat_GetRowIndexOfNonZeroValue(a)) ;
  Mry_Free(NCFormat_GetNonZeroValue(a)) ;
}




size_t (NCFormat_AssembleElementMatrix)(NCFormat_t* a,double* ke,int* col,int* row,int ndof,int* rowptr,int n_row)
/** Assemble the local matrix ke into the global matrix a 
 *  Return the nb of entries */
{
#define KE(i,j) (ke[(i)*ndof+(j)])
  double* nzval  = (double*) NCFormat_GetNonZeroValue(a) ;
  size_t*    colptr = NCFormat_GetFirstNonZeroValueIndexOfColumn(a) ;
  size_t*    rowind = NCFormat_GetRowIndexOfNonZeroValue(a) ;
  int    je ;
  size_t len = 0 ;

  if(rowptr) {
    int i ;
    
    for(i = 0 ; i < n_row ; i++) {
      rowptr[i] = -1 ;
    }
  }
  
  for(je = 0 ; je < ndof ; je++) {
    int jcol = col[je] ;
    int ie ;
    
    if(jcol < 0) continue ;

    if(rowptr) {
      int i ;
      
      for(i = colptr[jcol] ; i < colptr[jcol+1] ; i++) {
        rowptr[rowind[i]] = i ;
      }
    }

    for(ie = 0 ; ie < ndof ; ie++) {
      int irow = row[ie] ;
      
      if(irow < 0) continue ;
      
      if(rowptr) {
        if(rowptr[irow] < 0) {
          arret("NCFormat_AssembleElementMatrix: assembling not possible") ;
        }

        if(ke) {
          nzval[rowptr[irow]] += KE(ie,je) ;
        }
      
        len += 1 ;
      }
    }

    if(rowptr) {      
      for(size_t i = colptr[jcol] ; i < colptr[jcol+1] ; i++) {
        rowptr[rowind[i]] = -1 ;
      }
    }
  }
  
  return(len) ;

#undef KE
}




void (NCFormat_PrintMatrix)(NCFormat_t* a,size_t n_col,const char* keyword)
{
  double* nzval  = (double*) NCFormat_GetNonZeroValue(a) ;
  size_t*   colptr = NCFormat_GetFirstNonZeroValueIndexOfColumn(a) ;
  size_t*   rowind = NCFormat_GetRowIndexOfNonZeroValue(a) ;
  size_t    nnz = NCFormat_GetNbOfNonZeroValues(a) ;

  fprintf(stdout,"\n") ;
  fprintf(stdout,"Matrix in compressed column format:\n") ;
  fprintf(stdout,"n_col = %lu nnz = %lu\n",n_col,nnz) ;

  fprintf(stdout,"\n") ;
  fprintf(stdout,"\"col\" col: (lig)val ...\n") ;
  
  for(size_t jcol = 0 ; jcol < n_col ; jcol++) {
    
    fprintf(stdout,"col %lu:",jcol) ;
    
    for(size_t i = colptr[jcol] ; i < colptr[jcol+1] ; i++) {
      size_t irow = rowind[i] ;
      
      fprintf(stdout," (%lu)% e",irow,nzval[i]) ;
    }
    
    fprintf(stdout,"\n") ;
  }
}

