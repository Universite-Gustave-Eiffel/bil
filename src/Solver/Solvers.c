#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include "Mry.h"
#include "Options.h"
#include "Solvers.h"
#include "Solver.h"
#include "Message.h"
#include "Mesh.h"



Solvers_t*  (Solvers_Create)(Mesh_t* mesh,Options_t* options,unsigned short int const n)
{
  unsigned short int NbOfMatrices = Mesh_GetNbOfMatrices(mesh) ;
  Solvers_t* solvers = (Solvers_t*) Mry_New(Solvers_t) ;
  
  Solvers_GetNbOfSolvers(solvers) = NbOfMatrices ;
  
  {
    Solver_t* solver = (Solver_t*) Mry_New(Solver_t,NbOfMatrices) ;
    
    for(unsigned short int i = 0 ; i < NbOfMatrices ; i++) {
      Solver_t* solver_i = Solver_Create(mesh,options,n,i) ;
      
      solver[i] = solver_i[0] ;
      Mry_Free(solver_i) ;
    }
    
    Solvers_GetSolver(solvers) = solver ;
  }
  
  return(solvers) ;
}



void  (Solvers_Delete)(void* self)
{
  Solvers_t* solvers = (Solvers_t*) self ;
  
  {
    int n = Solvers_GetNbOfSolvers(solvers) ;
    Solver_t* solver = Solvers_GetSolver(solvers) ;
    
    if(solver) {
      Mry_Delete(solver,n,Solver_Delete) ;
      Mry_Free(solver) ;
      Solvers_GetSolver(solvers) = NULL ;
    }
  }
}
