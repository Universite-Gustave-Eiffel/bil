#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "Message.h"
#include "Monolithic.h"
#include "Context.h"
#include "CommonModule.h"
#include "Solutions.h"
#include "DataSet.h"
#include "Solvers.h"
#include "Solver.h"
#include "Module.h"
#include "Options.h"
#include "Mesh.h"
#include "Nodes.h"
#include "Loads.h"
#include "IterProcess.h"
#include "Matrix.h"
#include "Residu.h"
#include "BConds.h"
#include "IConds.h"
#include "Dates.h"
#include "Date.h"
#include "TimeStep.h"
#include "OutputFiles.h"
#include "Points.h"


#define AUTHORS  "Dangla"
#define TITLE    "Monolithic approach"

#include "PredefinedModuleMethods.h"

static Module_ComputeProblem_t   calcul ;
static Module_SolveProblem_t     Algorithm ;

#define Iterate     Monolithic_Iterate
#define StepForward Monolithic_StepForward
#define Increment   Monolithic_Increment
#define Initialize  Monolithic_Initialize



/*
  Extern functions
*/

int SetModuleProp(Module_t* module)
{
  Module_CopyShortTitle(module,TITLE) ;
  Module_CopyNameOfAuthors(module,AUTHORS) ;
  Module_SetComputeProblem(module,calcul) ;
  Module_SetSolveProblem(module,Algorithm) ;
  Module_SetIncrement(module,Monolithic_Increment) ;
  Module_SetInitializeProblem(module,Monolithic_Initialize) ;
  return(0) ;
}




int (Monolithic_Iterate)(DataSet_t* dataset,Solutions_t* sols,Solver_t* solver)
/** Iterate until convergence.
 *  On input sols should point to the current solution to be looked for.
 *  On output the current solution is updated after iterating.
 *  Return 0 if convergence has been achieved, -1 otherwise.
 */
{
#define SOL_1     Solutions_GetSolution(sols)

#define T_1       Solution_GetTime(SOL_1)
#define DT_1      Solution_GetTimeStep(SOL_1)

  Options_t*     options     = DataSet_GetOptions(dataset) ;
  Mesh_t*        mesh        = DataSet_GetMesh(dataset) ;
  Loads_t*       loads       = DataSet_GetLoads(dataset) ;
  IterProcess_t* iterprocess = DataSet_GetIterProcess(dataset) ;
  
  Nodes_t*       nodes       = Mesh_GetNodes(mesh) ;

  
  {
      /*
       * 3.1.5 Loop on iterations
       */
      IterProcess_SetIterationIndex(iterprocess,0) ;
      
      while(IterProcess_LastIterationIsNotReached(iterprocess)) {
        IterProcess_IncrementIterationIndex(iterprocess) ;
         
        if(Options_IsToPrintOutAtEachIteration(options)) {
          Message_Direct("Iteration: %d\n",IterProcess_GetIterationIndex(iterprocess));
          Message_CPUTime();
        }
        
        /*
         * 3.1.5.1 The implicit terms (constitutive equations)
         */
        {
          int i = Mesh_ComputeImplicitTerms(mesh,T_1,DT_1) ;
          
          if(Options_IsToPrintOutAtEachIteration(options)) {  
            Message_Direct("Implicit terms:\n");
            Message_Direct("CPU lap time %g seconds\n",Message_CPUTimeInterval());
            Message_CPUTime();
          }
          
          if(i != 0) {
            return(i) ;
          }
        }
        
        /*
         * 3.1.5.2 The residu
         */
        {
          Residu_t*  r = Solver_GetResidu(solver) ;
          
          Mesh_ComputeResidu(mesh,r,loads,T_1,DT_1) ;
          
          {
            char*  debug = Options_GetPrintedInfos(options) ;
            
            if(!strcmp(debug,"residu")) {
              Solver_Print(solver,debug) ;
            }
          }
          
          if(Options_IsToPrintOutAtEachIteration(options)) { 
            Message_Direct("Residu:\n");
            Message_Direct("CPU lap time %g seconds\n",Message_CPUTimeInterval());
            Message_CPUTime();
          }
        }
        
        /*
         * 3.1.5.3 The matrix
         */
        {
          Matrix_t*  a = Solver_GetMatrix(solver) ;
          int i = Mesh_ComputeMatrix(mesh,a,T_1,DT_1) ;
          
          if(i != 0) {
            return(i) ;
          }
          
          {
            char*  debug = Options_GetPrintedInfos(options) ;
            
            if(!strncmp(debug,"matrix",4)) {
              Solver_Print(solver,debug) ;
            }
          }
          
          if(Options_IsToPrintOutAtEachIteration(options)) {
            Message_Direct("Matrix:\n");
            Message_Direct("CPU lap time %g seconds\n",Message_CPUTimeInterval());
            Message_CPUTime();
          }
        }
        
        /*
         * 3.1.5.4 Resolution
         */
        {
          int i = Solver_Solve(solver) ;
          
          if(i != 0) {
            return(i) ;
          }
          
          if(Options_IsToPrintOutAtEachIteration(options)) {
            Message_Direct("Solve:\n");
            Message_Direct("CPU lap time %g seconds\n",Message_CPUTimeInterval());
            Message_CPUTime();
          }
        }
        
        /*
         * 3.1.5.5 Update the unknowns
         */
        Mesh_UpdateCurrentUnknowns(mesh,solver) ;
        
        /*
         * 3.1.5.6 The error
         */
        {
          int i = IterProcess_SetCurrentError(iterprocess,nodes,solver) ;
          
          if(i != 0) {
            return(i) ;
          }
        }
        
        /*
         * 3.1.5.7 We get out if convergence is achieved
         */
        if(IterProcess_ConvergenceIsAttained(iterprocess)) break ;
        
        {
          if(Options_IsToPrintOutAtEachIteration(options)) {
            if(IterProcess_LastIterationIsNotReached(iterprocess)) {
              IterProcess_PrintCurrentError(iterprocess) ;
            }
          }
        }
      }

      {
        IterProcess_PrintCurrentError(iterprocess) ;
      }
  }
      
  
  return(0) ;

#undef T_1
#undef DT_1
#undef SOL_1
}



int   (Monolithic_StepForward)(DataSet_t* dataset,Solutions_t* sols,Solver_t* solver,double t1,double t2)
/** Increment time and find a solution.
 *  On input sols should point to a valid solution at a time tn >= t1.
 *  On output, 2 possibilities:
 *  - return  0: the time is incremented to t = tn + dt <= t2, sols steps forward and
 *    points to the solution at time t whatever convergence has been achieved or not.
 *  - return -1: the time is not incremented, sols is not modified 
 *    (something went wrong).
 */
{
#define SOL_1     Solutions_GetSolution(sols)
#define SOL_n     Solution_GetPreviousSolution(SOL_1)

#define T_n       Solution_GetTime(SOL_n)
#define DT_n      Solution_GetTimeStep(SOL_n)
#define STEP_n    Solution_GetStepIndex(SOL_n)

#define T_1       Solution_GetTime(SOL_1)
#define DT_1      Solution_GetTimeStep(SOL_1)
#define STEP_1    Solution_GetStepIndex(SOL_1)

  Mesh_t*        mesh        = DataSet_GetMesh(dataset) ;
  BConds_t*      bconds      = DataSet_GetBConds(dataset) ;
  TimeStep_t*    timestep    = DataSet_GetTimeStep(dataset) ;
  IterProcess_t* iterprocess = DataSet_GetIterProcess(dataset) ;

  {
    {
      /*
       * 3.1.1 Looking for a new solution at t + dt
       * We step forward (point to the next solution) 
       */
      Solutions_StepForward(sols) ;
      
      /*
       * 3.1.1b Save the environment. 
       * That means that this is where the environment
       * is restored after a nonlocal jump.
       */
      Exception_SaveEnvironment ;
      
      /*
       * 3.1.1c Backup the previous solution:
       * if the saved environment was restored after a nonlocal jump
       * and 
       * if the exception mechanism orders to do it.
       */
      {
        if(Exception_OrderToBackupAndTerminate) {
          backupandreturn :
          Solutions_StepBackward(sols) ;
          return(-1) ;
        }
      }
      
      /*
       * 3.1.2 Compute the explicit terms with the previous solution
       */
      {
        int i = Mesh_ComputeExplicitTerms(mesh,T_n) ;
        
        if(i != 0) {
          Message_Direct("\n") ;
          Message_Direct("Monolithic_StepForward(1): undefined explicit terms\n") ;
          /* Backup the previous solution */
          goto backupandreturn ;
        }
      }
        
      /*
       * 3.1.3 Compute and set the time step
       */
      {
        double dt = TimeStep_ComputeTimeStep(timestep,SOL_n,t1,t2) ;
        
        DT_1 = dt ;
        STEP_1 = STEP_n + 1 ;
      }
      
      /*
       * 3.1.3b Initialize the repetition index
       */
      IterProcess_SetRepetitionIndex(iterprocess,0) ;
      
      
      /*
       * 3.1.3c Reduce the time step 
       * if the exception mechanism orders to do it.
       */
      {
        if(Exception_OrderToReiterateWithSmallerTimeStep) {
          repeatwithreducedtimestep :
          
          IterProcess_IncrementRepetitionIndex(iterprocess) ;
          DT_1 *= TimeStep_GetReductionFactor(timestep) ;
          
        } else if(Exception_OrderToReiterateWithInitialTimeStep) {
          repeatwithinitialtimestep :
          
          IterProcess_IncrementRepetitionIndex(iterprocess) ;
          DT_1 *= TimeStep_GetReductionFactor(timestep) ;
          {
            double t_ini = TimeStep_GetInitialTimeStep(timestep) ;
              
            if(DT_1 > t_ini) DT_1 = t_ini ;
          }
        }
      }

      
      /*
       * 3.1.3d The time at which we compute
       */
      {
        int irecom = IterProcess_GetRepetitionIndex(iterprocess) ;
        
        if(irecom > 0) Message_Direct("Repetition no %d\n",irecom) ;
      }
      T_1 = T_n + DT_1 ;
      Message_Direct("Step %d  t = %e (dt = %4.2e)",STEP_1,T_1,DT_1) ;
      
      /*
       * 3.1.4 Initialize the unknowns
       */
      Mesh_SetCurrentUnknownsWithBoundaryConditions(mesh,bconds,T_1) ;
      
      /*
       * 3.1.5 Iterate to converge to the solution at this time
       */
      {
        int i = Iterate(dataset,sols,solver) ;
          
        if(i != 0) {
          if(IterProcess_LastRepetitionIsNotReached(iterprocess)) {
            goto repeatwithinitialtimestep ;
          } else {
            int iter = IterProcess_GetIterationIndex(iterprocess) ;
              
            Message_Direct("\n") ;
            Message_Direct("Monolithic_StepForward(2): somethong wrong has happened at iteration %d\n",iter) ;
            goto backupandreturn ;
          }
        }
      }
      
      /*
       * 3.1.6 Back to 3.1.3 with a smaller time step
       */
      if(IterProcess_ConvergenceIsNotAttained(iterprocess)) {
        if(IterProcess_LastRepetitionIsNotReached(iterprocess)) {
          goto repeatwithreducedtimestep ;
        }
      }
    }
  }
  
  return(0) ;

#undef T_n
#undef DT_n
#undef STEP_n
#undef T_1
#undef DT_1
#undef STEP_1
#undef SOL_n
#undef SOL_1
}




int   (Monolithic_Increment)(DataSet_t* dataset,Solutions_t* sols,Solver_t* solver,OutputFiles_t* outputfiles,double t1,double t2)
/** Increment time, find the solution. Repeat until reaching the time t2.
 *  On input sols should point to a solution at a time >= t1.
 *  On output sols points to the last converged solution at a time <= t2.
 *  Return 0 if convergence was achieved at time t2, -1 otherwise.
 */
{
#define SOL_1     Solutions_GetSolution(sols)

#define T_1       Solution_GetTime(SOL_1)

  IterProcess_t* iterprocess = DataSet_GetIterProcess(dataset) ;

  {
    /*
     * 3.1 Loop on time steps
     */
    do {
      /*
       * 3.1.1 Looking for a new solution at t + dt
       * We step forward (point to the next solution) 
       */
      {
        int i = StepForward(dataset,sols,solver,t1,t2) ;
        
        if(i != 0) return(i) ;
      }
      
      /*
       * 3.1.7 Backup for specific points
       */
      OutputFiles_BackupSolutionAtPoint(outputfiles,dataset,T_1) ;
      /*
       * 3.1.8 Go out if convergence was not achieved
       */
      if(IterProcess_ConvergenceIsNotAttained(iterprocess)) break ;
      
    } while(T_1 < t2) ;
  }
  
  return(0) ;

#undef T_1
#undef SOL_1
}



int (Monolithic_Initialize)(DataSet_t* dataset,Solutions_t* sols)
/** The solution pointed to by sols, is initialized if the context tells
 *  that initialization should be performed otherwise nothing is done.
 *  Return an int, idate, so that the initial time is between
 *  date[idate] and date[idate+1] (0 by default).
 */
{
#define SOL_1     Solutions_GetSolution(sols)

#define T_1       Solution_GetTime(SOL_1)

  Mesh_t* mesh = DataSet_GetMesh(dataset) ;

  int idate = 0 ;


  Solutions_InitializeMeshPointers(sols,mesh) ;

  
  {
    DataFile_t* datafile = DataSet_GetDataFile(dataset) ;
    int i = Mesh_LoadCurrentSolution(mesh,datafile,&T_1) ;
    
    idate = 0 ;
    
    if(i) {
      Dates_t*     dates     = DataSet_GetDates(dataset) ;
      int          nbofdates = Dates_GetNbOfDates(dates) ;
      Date_t*      date      = Dates_GetDate(dates) ;
      
      while(idate + 1 < nbofdates && T_1 >= Date_GetTime(date + idate + 1)) idate++ ;
      
      Message_Direct("Continuation ") ;
      
      if(DataFile_ContextIsFullInitialization(datafile)) {
        Message_Direct("(full initialization) ") ;
      } else if(DataFile_ContextIsPartialInitialization(datafile)) {
        Message_Direct("(partial initialization) ") ;
      } else if(DataFile_ContextIsNoInitialization(datafile)) {
        Message_Direct("(no initialization) ") ;
      }
      
      Message_Direct("at t = %e (between steps %d and %d)\n",T_1,idate,idate+1) ;
    }
    
    if(DataFile_ContextIsInitialization(datafile)) {
      IConds_t* iconds = DataSet_GetIConds(dataset) ;
    
      IConds_AssignInitialConditions(iconds,mesh,T_1) ;

      Mesh_ComputeInitialState(mesh,T_1) ;
    }
  }
  
  return(idate) ;

#undef T_1
#undef SOL_1
}



/*
  Intern functions
*/

int   Algorithm(DataSet_t* dataset,Solutions_t* sols,Solver_t* solver,OutputFiles_t* outputfiles)
/** On input sols should point to the initial solution except
 *  if the context tells that initialization should be performed.
 *  On output sols points to the last converged solution.
 *  Return 0 if convergence has been achieved, -1 otherwise.
 */
{
#define SOL_1     Solutions_GetSolution(sols)

#define T_1       Solution_GetTime(SOL_1)

  Dates_t*       dates       = DataSet_GetDates(dataset) ;
  IterProcess_t* iterprocess = DataSet_GetIterProcess(dataset) ;
  
  int            nbofdates   = Dates_GetNbOfDates(dates) ;
  Date_t*        date        = Dates_GetDate(dates) ;

  int            idate = Initialize(dataset,sols) ;
  
  
  /*
   * 2. Backup
   */
  OutputFiles_BackupSolutionAtPoint(outputfiles,dataset,T_1,"o") ;
  OutputFiles_BackupSolutionAtTime(outputfiles,dataset,T_1,idate) ;
  
  
  /*
   * 3. Loop on dates
   */
  for(; idate < nbofdates - 1 ; idate++) {
    Date_t* date_i = date + idate ;
    
    /*
     * 3.1 Loop on time steps
     */
    {
      double t1 = Date_GetTime(date_i) ;
      double t2 = Date_GetTime(date_i + 1) ;
      int i = Increment(dataset,sols,solver,outputfiles,t1,t2) ;
        
      if(i != 0) {
        OutputFiles_BackupSolutionAtTime(outputfiles,dataset,T_1,idate+1) ;
        return(-1) ;
      }
    }
    
    /*
     * 3.2 Backup for this time
     */
    OutputFiles_BackupSolutionAtTime(outputfiles,dataset,T_1,idate+1) ;
    
    /*
     * 3.3 Go to 4. if convergence was not achieved
     */
    if(IterProcess_ConvergenceIsNotAttained(iterprocess)) break ;
  }
  
  /*
   * 4. Step backward if convergence was not achieved
   */
  if(IterProcess_ConvergenceIsNotAttained(iterprocess)) {
    Solutions_StepBackward(sols) ;
    return(-1) ;
  }
  
  return(0) ;

#undef T_1
#undef SOL_1
}



int calcul(DataSet_t* dataset)
{
  Mesh_t* mesh = DataSet_GetMesh(dataset) ;
  Options_t* options = DataSet_GetOptions(dataset) ;
  int n_sol = Options_GetNbOfSolutions(options) ;
  //const int n_sol = 2 ; /* Must be 2 at minimum but works with more */
  Solutions_t* sols = Solutions_Create(mesh,n_sol) ;
  
  if(n_sol < 2) {
    Message_RuntimeError("Nb of solutions should not be less than 2.") ;
  }

  /* Execute this line to set only one allocation of space for explicit terms. */
  /* This is not mandatory except in some models where constant terms are saved as 
   * explicit terms and updated only once during initialization. 
   * It is then necessary to merge explicit terms. Otherwise it is not mandatory.
   * Should be eliminated in the future. */
  //Solutions_MergeExplicitTerms(sols) ;
  /* This is done 11/05/2015 */
  Message_Warning("Explicit terms are not merged anymore in this version.") ;
  
  
  {
    DataFile_t* datafile = DataSet_GetDataFile(dataset) ;
    int i = 0 ;

  /* Print */
    {
      char*   debug  = Options_GetPrintedInfos(options) ;
    
      if(!strcmp(debug,"numbering")) DataSet_PrintData(dataset,debug) ;
    }
    
  /* 1. Initial time */
    {
      Solution_t* sol = Solutions_GetSolution(sols) ;
      Dates_t*  dates = DataSet_GetDates(dataset) ;
      Date_t* date    = Dates_GetDate(dates) ;
      double t0       = Date_GetTime(date) ;
      
      Solution_GetTime(sol) = t0 ;
    }
    
  /* 2. Calculation */
    {
      char*   filename = DataFile_GetFileName(datafile) ;
      Dates_t*  dates    = DataSet_GetDates(dataset) ;
      int     nbofdates  = Dates_GetNbOfDates(dates) ;
      Points_t* points   = DataSet_GetPoints(dataset) ;
      int     n_points   = Points_GetNbOfPoints(points) ;
      OutputFiles_t* outputfiles = OutputFiles_Create(filename,nbofdates,n_points) ;
      Solvers_t* solvers = Solvers_Create(mesh,options,1) ;
      Solver_t* solver = Solvers_GetSolver(solvers) ;
      
      i = Algorithm(dataset,sols,solver,outputfiles) ;
      
      Solvers_Delete(solvers) ;
      free(solvers) ;
      OutputFiles_Delete(outputfiles) ;
      free(outputfiles) ;
    }
      
  /* 3. Store for future resumption */
    {
      Solution_t* sol = Solutions_GetSolution(sols) ;
      double t =  Solution_GetTime(sol) ;
      
      Mesh_StoreCurrentSolution(mesh,datafile,t) ;
    }
    
    Solutions_Delete(sols) ;
    free(sols) ;
    return(i) ;
  }
}
