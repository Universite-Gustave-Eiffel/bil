#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Message.h"
#include "IterProcess.h"
#include "DataFile.h"
#include "DataSet.h"
#include "ObVals.h"
#include "ObVal.h"
#include "Nodes.h"
#include "Node.h"
#include "Solver.h"
#include "Exception.h"
#include "String_.h"
#include "Mry.h"




IterProcess_t*  (IterProcess_New)(ObVals_t* obvals)
{
  IterProcess_t* iterprocess = (IterProcess_t*) Mry_New(IterProcess_t) ;
  
  IterProcess_SetObVals(iterprocess,obvals) ;

  /* Iterations */
  {
    IterProcess_SetNbOfIterations(iterprocess,5) ;
    IterProcess_SetIterationIndex(iterprocess,0) ;
  }
    
  /* Tolerance */
  {
    IterProcess_SetTolerance(iterprocess,1.e-4) ;
    IterProcess_SetError(iterprocess,0) ;
  }
    
  /* Repetition */
  {
    IterProcess_SetNbOfRepetitions(iterprocess,0) ;
    IterProcess_SetRepetitionIndex(iterprocess,0) ;
  }

  return(iterprocess) ;
}


#if 0
IterProcess_t*  (IterProcess_Create)(DataFile_t* datafile,ObVals_t* obvals)
{
  IterProcess_t* iterprocess = IterProcess_New(obvals) ;
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"ALGO,ITER,Iterative Process",",") ;
  
  
  if(!c) {
    Message_FatalError("No Iterative Process") ;
  }
  
  
  Message_Direct("Enter in %s","Iterative Process") ;
  Message_Direct("\n") ;



  c = String_SkipLine(c) ;
      
  //DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  

  /* Iterations */
  {
    int i ;
    int n = String_FindAndScanExp(c,"Iter",","," = %d",&i) ;
    
    if(n) {
      IterProcess_SetNbOfIterations(iterprocess,i) ;
    } else {
      arret("IterProcess_Create: no Iterations") ;
    }
  }
    
  /* Tolerance */
  {
    double tol ;
    int n = String_FindAndScanExp(c,"Tol",","," = %lf",&tol) ;
    
    if(n) {
      IterProcess_SetTolerance(iterprocess,tol) ;
    } else {
      arret("IterProcess_Create: no Tolerance") ;
    }
  }
    
  /* Repetition */
  {
    int i ;
    int n = String_FindAndScanExp(c,"Rep,Rec",","," = %d",&i) ;
    
    if(n) {
      IterProcess_SetNbOfRepetitions(iterprocess,i) ;
    }
  }


  return(iterprocess) ;
}
#elseif 0
IterProcess_t*  (IterProcess_Create)(DataFile_t* datafile,ObVals_t* obvals)
{
  IterProcess_t* iterprocess = IterProcess_New(obvals) ;
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"ALGO,ITER,Iterative Process",",") ;
  int iter ;
  double tol ;
  int rep = 0 ;
  
  
  if(!c) {
    Message_FatalError("No Iterative Process") ;
  }
  
  
  Message_Direct("Enter in %s","Iterative Process") ;
  Message_Direct("\n") ;


  c = String_SkipLine(c) ;
      
  //DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  

  /* Iterations */
  {
    int n = String_FindAndScanExp(c,"Iter",","," = %d",&iter) ;
    
    if(!n) {
      arret("IterProcess_Create: no Iterations") ;
    }
  }
    
  /* Tolerance */
  {
    int n = String_FindAndScanExp(c,"Tol",","," = %lf",&tol) ;
    
    if(!n) {
      arret("IterProcess_Create: no Tolerance") ;
    }
  }
    
  /* Repetition */
  {
    int n = String_FindAndScanExp(c,"Rep,Rec",","," = %d",&rep) ;
  }

  IterProcess_Set(iterprocess,iter,tol,rep);

  return(iterprocess) ;
}
#else
IterProcess_t*  (IterProcess_Create)(DataFile_t* datafile,ObVals_t* obvals)
{
  IterProcess_t* iterprocess = IterProcess_New(obvals) ;

  IterProcess_Scan(iterprocess,datafile);

  return(iterprocess) ;
}
#endif


void  (IterProcess_Scan)(IterProcess_t* iterprocess,DataFile_t* datafile)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"ALGO,ITER,Iterative Process",",") ;
  int iter ;
  double tol ;
  int rep = 0 ;
  
  
  if(!c) {
    Message_FatalError("No Iterative Process") ;
  }
  
  
  Message_Direct("Enter in %s","Iterative Process") ;
  Message_Direct("\n") ;


  c = String_SkipLine(c) ;
      
  //DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  

  /* Iterations */
  {
    int n = String_FindAndScanExp(c,"Iter",","," = %d",&iter) ;
    
    if(!n) {
      arret("IterProcess_Create: no Iterations") ;
    }
  }
    
  /* Tolerance */
  {
    int n = String_FindAndScanExp(c,"Tol",","," = %lf",&tol) ;
    
    if(!n) {
      arret("IterProcess_Create: no Tolerance") ;
    }
  }
    
  /* Repetition */
  {
    int n = String_FindAndScanExp(c,"Rep,Rec",","," = %d",&rep) ;
  }

  IterProcess_Set(iterprocess,iter,tol,rep);

  return ;
}



void  (IterProcess_Delete)(void* self)
{
  IterProcess_t* iterprocess = (IterProcess_t*) self ;
}



int (IterProcess_SetCurrentError)(IterProcess_t* iterprocess,Nodes_t* nodes,Solver_t* solver)
{
  int imatrix = Solver_GetMatrixIndex(solver) ;
  double*   x     = Solver_GetSolution(solver) ;
  size_t    nrows = Solver_GetNbOfRows(solver) ;
  ObVal_t*  obval = IterProcess_GetObVal(iterprocess) ;
  Node_t*   node  = Nodes_GetNode(nodes) ;
  size_t nb_nodes = Nodes_GetNbOfNodes(nodes) ;
  double err = 0. ;
  int    obvalindex = 0 ;
  Node_t* nodecrit = NULL;
  
  
  if(nrows > 0) {          
    for(size_t i = 0 ; i < nb_nodes ; i++) {
      Node_t* nodi = node + i ;
      int nin = Node_GetNbOfUnknowns(nodi) ;
      int j ;
            
      for(j = 0 ; j < nin ; j++) {
        //int   k = Node_GetMatrixColumnIndex(nodi)[j] ;
        int   k = Node_GetSelectedMatrixColumnIndexOf(nodi,j,imatrix) ;
              
        if(k >= 0) {
          ObVal_t* obval_j = obval + Node_GetObValIndex(nodi)[j] ;
          double val = ObVal_GetValue(obval_j) ;
          double re = fabs(x[k])/val ;
                
          if(ObVal_IsRelativeValue(obval_j)) {
            double* u_n = Node_GetPreviousUnknown(nodi) ;
                  
            if(fabs(u_n[j]) > 0.) re /= fabs(u_n[j]) ;
          }

          /* Sometimes re is strictly equal to zero hence the >= */
          if(re >= err) {
            err = re ;
            obvalindex = Node_GetObValIndex(nodi)[j] ;
            nodecrit = nodi ;
          }
        }
      }
    }
  
    if(nrows > 0 && nodecrit == NULL) {
      /* Raise an interrupt signal instead of exit */
      Message_Warning("IterProcess_SetCurrentError: can't compute error!") ;
      return(1) ;
      //Exception_Interrupt ;
    }
  }
          
  IterProcess_SetError(iterprocess,err) ;
  IterProcess_SetObValIndexOfCurrentError(iterprocess,obvalindex) ;
  IterProcess_SetNodeOfCurrentError(iterprocess,nodecrit) ;
  {
    size_t nodeindex  = Node_GetNodeIndex(nodecrit) ;
    
    IterProcess_SetNodeIndexOfCurrentError(iterprocess,nodeindex) ;
  }
  
  return(0) ;
}



void (IterProcess_PrintCurrentError)(IterProcess_t* iterprocess)
{
  char*  name = IterProcess_GetNameOfTheCurrentError(iterprocess) ;
  double err  = IterProcess_GetError(iterprocess) ;
  int    iter = IterProcess_GetIterationIndex(iterprocess) ;
  Node_t* node = IterProcess_GetNodeOfCurrentError(iterprocess) ;

  if(node) {
    size_t inode = Node_GetNodeIndex(node) ;
    
    Message_Direct("  (%s[%lu])Error = %4.2e (%d iters)\n",name,inode,err,iter) ;
    //Message_Direct("  (%s)Error = %4.2e (%d iters)\n",name,err,iter) ;
  } else {
    Message_Direct("  (none[-])Error = %4.2e (%d iters)\n",err,iter) ;
  }
}
