#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include "Mry.h"
#include "Solver.h"
#include "Message.h"
#include "CommandLine.h"
#include "Context.h"
#include "BilConfig.h"
#include "ResolutionMethod.h"
#include "CroutMethod.h"
#include "DistributedMS.h"
#include "Mesh.h"
#include "Options.h"
#include "Residu.h"
#include "Matrix.h"
#include "GenericData.h"

#ifdef HAVE_SUPERLU
  #include "SuperLUMethod.h"
#endif

#ifdef HAVE_SUPERLUMT
  #include "SuperLUMTMethod.h"
  #include "superlu.h"
#endif

#ifdef HAVE_SUPERLUDIST
  #include "SuperLUDistMethod.h"
  #include "superlu.h"
#endif

#if defined (HAVE_BLAS) && defined (HAVE_LAPACK)
  #include "MA38Method.h"
#endif

#if defined (HAVE_PETSC)
  #include "PetscKSPMethod.h"
  #include "PetscAIJFormat.h"
  #include <petsc.h>
#endif




/*
  Extern functions
*/

Solver_t*  (Solver_Create)(Mesh_t* mesh,Options_t* options,unsigned short int const n_res,unsigned short int const imatrix)
{
  Solver_t* solver = (Solver_t*) Mry_New(Solver_t) ;
  
  
  /* Nb of rows/columns */
  {
    size_t n_col = Mesh_GetNbOfMatrixColumns(mesh)[imatrix] ;
    
    Solver_GetNbOfColumns(solver) = n_col ;
  }
  
  
  /* Allocation of space for the residu */
  {
    Residu_t* residu = Residu_Create(mesh,options,n_res,imatrix) ;
    
    Solver_GetResidu(solver) = residu ;
  }
  
  
  /* Allocation of space for the matrix */
  {
    Matrix_t* matrix = Matrix_Create(mesh,options,imatrix) ;
    
    Solver_GetMatrix(solver) = matrix ;
  }
  
  
  /* Resolution method */
  {
    Solver_GetResolutionMethod(solver) = ResolutionMethod_Create(options) ;
  }
  
  
  /* The solver */
  {
    if(Solver_ResolutionMethodIs(solver,CROUT)) {
      Solver_GetSolve(solver) = CroutMethod_Solve ;
      
    #ifdef HAVE_SUPERLU
    } else if(Solver_ResolutionMethodIs(solver,SuperLU)) {
      Solver_GetSolve(solver) = SuperLUMethod_Solve ;
      
      /* Allocate work spaces for dgssvx */
      {
        int n = Solver_GetNbOfColumns(solver) ;
        void* etree  = Mry_New(int,n) ;
        void* R      = Mry_New(double,n) ;
        void* C      = Mry_New(double,n) ;
        void* err    = Mry_New(double,2) ;
        GenericData_t* getree = GenericData_Create(n,etree,"etree") ;
        GenericData_t* gR     = GenericData_Create(n,R,"R") ;
        GenericData_t* gC     = GenericData_Create(n,C,"C") ;
        GenericData_t* gerr   = GenericData_Create(2,err,"err") ;
        /* The FILL factor: FILL = 30 set by sp_ienv of superlu. */
        /* If lwork = 0 allocate space internally by system malloc */
        double fill = Options_GetFillFactor(options) ;
        Matrix_t* matrix = Solver_GetMatrix(solver) ;
        int nnz = Matrix_GetNbOfNonZeroValues(matrix) ;
        int lwork = (int) floor(fill*nnz) ;

        Solver_AppendGenericWorkSpace(solver,getree) ;
        Solver_AppendGenericWorkSpace(solver,gR) ;
        Solver_AppendGenericWorkSpace(solver,gC) ;
        Solver_AppendGenericWorkSpace(solver,gerr) ;
      
        if(lwork) {
          void* work = Mry_New(double,lwork) ;
          GenericData_t* gwork = GenericData_Create(lwork,work,"work") ;
        
          Solver_AppendGenericWorkSpace(solver,gwork) ;
        }
      }
    #endif
      
    #ifdef HAVE_SUPERLUMT
    } else if(Solver_ResolutionMethodIs(solver,SuperLUMT)) {
      Solver_GetSolve(solver) = SuperLUMTMethod_Solve ;
      
      /* Allocate work spaces for dgssvx */
      {
        int n = Solver_GetNbOfColumns(solver) ;
        void* etree  = Mry_New(int,n) ;
        void* colcnt_h = Mry_New(int,n) ;
        void* part_super_h = Mry_New(int,n) ;
        void* R      = Mry_New(double,n) ;
        void* C      = Mry_New(double,n) ;
        void* err    = Mry_New(double,2) ;
        GenericData_t* getree = GenericData_Create(n,etree,"etree") ;
        GenericData_t* gcolcnt_h = GenericData_Create(n,colcnt_h,"colcnt_h") ;
        GenericData_t* gpart_super_h = GenericData_Create(n,part_super_h,"part_super_h") ;
        GenericData_t* gR     = GenericData_Create(n,R,"R") ;
        GenericData_t* gC     = GenericData_Create(n,C,"C") ;
        GenericData_t* gerr   = GenericData_Create(2,err,"err") ;
        Matrix_t* matrix = Solver_GetMatrix(solver) ;
        int nnz = Matrix_GetNbOfNonZeroValues(matrix) ;
        /* The FILL factor of superlumt from the command line*/
        /* If lwork = 0 allocate space internally by system malloc */
        double fill = Options_GetFillFactor(options) ;
        /* If not given in the command line (=0) it is */
        double fill6 = sp_ienv(6);
        double fill7 = sp_ienv(7);
        double fill8 = sp_ienv(8);
        int lwork6 = (fill6 < 0) ? (int) floor(-fill6*nnz) : fill6 ;
        int lwork7 = (fill7 < 0) ? (int) floor(-fill7*nnz) : fill7 ;
        int lwork8 = (fill8 < 0) ? (int) floor(-fill8*nnz) : fill8 ;
        int lwork678 = lwork6 + lwork7 + lwork8 ;
        int lwork = (fill > 0) ? (int) floor(fill*nnz) : lwork678 ;

        Solver_AppendGenericWorkSpace(solver,getree) ;
        Solver_AppendGenericWorkSpace(solver,gcolcnt_h) ;
        Solver_AppendGenericWorkSpace(solver,gpart_super_h) ;
        Solver_AppendGenericWorkSpace(solver,gR) ;
        Solver_AppendGenericWorkSpace(solver,gC) ;
        Solver_AppendGenericWorkSpace(solver,gerr) ;
      
        if(lwork) {
          void* work = Mry_New(double,lwork) ;
          GenericData_t* gwork = GenericData_Create(lwork,work,"work") ;
        
          Solver_AppendGenericWorkSpace(solver,gwork) ;
        }
      }
    #endif
      
    #ifdef HAVE_SUPERLUDIST
    } else if(Solver_ResolutionMethodIs(solver,SuperLUDist)) {
      Solver_GetSolve(solver) = SuperLUDistMethod_Solve ;
      
      /* Allocate work spaces for pdgssvx_ABglobal */
      {
        dScalePermstruct_t* scalepermstruct = Mry_New(dScalePermstruct_t) ;
        dLUstruct_t* lustruct = Mry_New(dLUstruct_t) ;
        gridinfo_t* grid = Mry_New(gridinfo_t) ;
        GenericData_t* gscalepermstruct = GenericData_Create(1,scalepermstruct,"ScalePermstruct") ;
        GenericData_t* glustruct = GenericData_Create(1,lustruct,"LUstruct") ;
        GenericData_t* ggrid = GenericData_Create(1,grid,"grid") ;

        Solver_AppendGenericWorkSpace(solver,gscalepermstruct) ;
        Solver_AppendGenericWorkSpace(solver,glustruct) ;
        Solver_AppendGenericWorkSpace(solver,ggrid) ;
      
        /* Initialization */
        {
          int n = Solver_GetNbOfColumns(solver) ;
          //int nprocs = Options_NbOfProcessorsInDistributedMemorySolver(options) ;
          int nprocs = DistributedMS_NbOfProcessors ;
          int nprow = sqrt(nprocs) ;
          int npcol = nprocs/nprow ;
          //Context_t* ctx = Options_GetContext(options) ;
          //CommandLine_t* cmd = Context_GetCommandLine(ctx) ;
          //int argc = CommandLine_GetNbOfArgs(cmd) ;
          //char** argv = CommandLine_GetArg(cmd) ;
        
          /* Initialize the superlu process grid */
          superlu_gridinit(MPI_COMM_WORLD,nprow,npcol,grid);
          
          /* Leave if not belong in the grid */
          {
            int iam = (*grid).iam ;
            
            if(iam > nprocs) {
              superlu_gridexit(grid) ;
              Message_FatalError("Solver_Create: too many ") ;
            }
          }

          /* Initialize scalepermstruct
           * scalepermstruct->DiagScale = NOEQUIL
           */
          dScalePermstructInit(n,n,scalepermstruct);
          
          {
            int i ;
            
            for(i = 0 ; i < n ; i++) {
              scalepermstruct->perm_r[i] = i ;
              scalepermstruct->perm_c[i] = i ;
            }
          }
          /* Initialize lustruct 
           * lustruct->Llu->inv = 0
           */
          dLUstructInit(n,lustruct);
        }
      }
    #endif
    
    #if defined (HAVE_BLAS) && defined (HAVE_LAPACK)
    } else if(Solver_ResolutionMethodIs(solver,MA38)) {
      Solver_GetSolve(solver) = MA38Method_Solve ;

      /*  Allocate work space for ma38cd */
      {
        int n_col = Solver_GetNbOfColumns(solver) ;
        int lwork = 4 * n_col ;
        void* work = Mry_New(double,lwork) ;
        GenericData_t* gwork = GenericData_Create(lwork,work,"work") ;
      
        Solver_AppendGenericWorkSpace(solver,gwork) ;
      }
    #endif
    
    #if defined (HAVE_PETSC)
    } else if(Solver_ResolutionMethodIs(solver,PetscKSP)) {
      Solver_GetSolve(solver) = PetscKSPMethod_Solve ;
      
      /* Initialization */
      {
        Context_t* ctx = Options_GetContext(options) ;
        CommandLine_t* cmd = Context_GetCommandLine(ctx) ;
        int argc = CommandLine_GetNbOfArgs(cmd) ;
        char** argv = CommandLine_GetArg(cmd) ;
        const char help[] = "Solver KSP\n\n" ;
        
        PetscInitialize(&argc,&argv,NULL,help) ;
      }
      
      /*  Create the solver KSP */
      {
        KSP* ksp = (KSP*) Mry_New(KSP) ;
        GenericData_t* gksp = GenericData_Create(1,ksp,"ksp") ;
        
        KSPCreate(PETSC_COMM_WORLD,ksp) ;
        
        /* Set the method from the command line "-ksp_type <method>" 
         * where "method" is one of the following options: 
         * richardson, chebyshev, cg, gmres, tcqmr, bcgs, cgs, tfqmr, 
         * cr, lsqr, bicg, preonly ...
         * with gmres by default. 
         * Set the relative, absolute, divergence tolerance from the
         * command line: 
         * "-ksp_rtol <rtol>", "-ksp_atol <atol>", "-ksp_dtol <dtol>
         **/
        {
          size_t n = Solver_GetNbOfColumns(solver) ;
          PetscReal rtol = 1.e-4/n ;
          
          KSPSetTolerances(*ksp,rtol,PETSC_DEFAULT,PETSC_DEFAULT,PETSC_DEFAULT) ;
          //KSPSetTolerances(*ksp,PETSC_DEFAULT,PETSC_DEFAULT,PETSC_DEFAULT,PETSC_DEFAULT) ;
          KSPSetFromOptions(*ksp) ;
          //KSPSetUp(*ksp) ;
        }
        
        Solver_AppendGenericWorkSpace(solver,gksp) ;
      }
      
      /* Sets the preconditioner options */
      #if 1
      {
        GenericData_t* gw = Solver_GetGenericWorkSpace(solver) ;
        KSP* ksp = GenericData_FindData(gw,"ksp") ;
        PC*  pc = (PC*) Mry_New(PC) ;
        GenericData_t* gpc = GenericData_Create(1,pc,"pc") ;

        /* Set the method from the command line "-pc_type <method>"
         * wher "method" is on eof the following options:
         * none, jacobi, sor, ilu, icc ...
         **/
        {
          KSPGetPC(*ksp,pc) ;
          PCSetFromOptions(*pc) ;
          //PCSetUp(*pc) ;
        }
        
        //Solver_AppendGenericWorkSpace(solver,gpc) ;
      }
      #endif
    #endif
    
    } else {
      arret("Solver_Create: method not available") ;
    }
  }
  
  return(solver) ;
}



void  (Solver_Delete)(void* self)
{
  Solver_t* solver = (Solver_t*) self ;
  
  {
    Matrix_t* a = Solver_GetMatrix(solver) ;
    
    if(a) {
      Matrix_Delete(a) ;
      Mry_Free(a) ;
      Solver_GetMatrix(solver) = NULL ;
    }
  }
  
  {
    Residu_t* rs = Solver_GetResidu(solver) ;

    if(rs) {
      Residu_Delete(rs) ;
      Mry_Free(rs) ;
      Solver_GetResidu(solver) = NULL ;
    }
  }
  
  {
    GenericData_t* genericwork = Solver_GetGenericWorkSpace(solver) ;
    
    if(genericwork) {
      GenericData_Delete(genericwork) ;
      Mry_Free(genericwork) ;
      Solver_GetGenericWorkSpace(solver) = NULL ;
    }
  }
  
  #if defined (HAVE_PETSC)
  {
    if(Solver_ResolutionMethodIs(solver,PetscKSP)) {
      PetscBool isfinalized ;
      
      PetscFinalized(&isfinalized) ;
      
      if(!isfinalized) {
        PetscInitialize(NULL,NULL,NULL,NULL) ;
        PetscFinalize() ;
      }
    }
  }
  #endif
  
  {
    ResolutionMethod_t* rm = Solver_GetResolutionMethod(solver) ;
    
    if(rm) {
      ResolutionMethod_Delete(rm) ;
      Mry_Free(rm) ;
      Solver_GetResolutionMethod(solver) = NULL ;
    }
  }
}



void Solver_Print(Solver_t* solver,char* keyword)
{
  static int i_debug=0 ;

  fprintf(stdout,"\n") ;
  fprintf(stdout,"debug(%d)\n",i_debug++) ;
  fprintf(stdout,"-----\n") ;
            
  if(!strcmp(keyword,"residu")) {
    Residu_t* residu = Solver_GetResidu(solver) ;
    
    Residu_PrintResidu(residu,keyword) ;
  }
  
  if(!strncmp(keyword,"matrix",6)) {
    Matrix_t*  a = Solver_GetMatrix(solver) ;
    
    Matrix_PrintMatrix(a,keyword) ;
  }

}

