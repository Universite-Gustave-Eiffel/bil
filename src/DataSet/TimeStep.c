#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Message.h"
#include "Mry.h"
#include "TimeStep.h"
#include "DataFile.h"
#include "ObVals.h"
#include "ObVal.h"
#include "Nodes.h"
#include "Node.h"
#include "Solution.h"





TimeStep_t*  (TimeStep_New)(ObVals_t* obvals,DataFile_t* datafile)
{
  TimeStep_t* timestep = (TimeStep_t*) Mry_New(TimeStep_t) ;

  TimeStep_SetObVals(timestep,obvals) ;
  TimeStep_SetDataFile(timestep,datafile) ;

  /* default values */
  TimeStep_SetInitialTimeStep(timestep,0) ;
  TimeStep_SetMaximumTimeStep(timestep,0) ;
  TimeStep_SetMinimumTimeStep(timestep,0) ;
  TimeStep_SetReductionFactor(timestep,0.5) ;
  TimeStep_SetMaximumCommonRatio(timestep,1.5) ;
  TimeStep_SetLocation(timestep,0) ;
  TimeStep_SetLocationAtBegin(timestep) ;

  return(timestep) ;
}



void  (TimeStep_Delete)(void* self)
{
  TimeStep_t* timestep = (TimeStep_t*) self ;
}



#if 0
TimeStep_t*  (TimeStep_Create)(ObVals_t* obvals,DataFile_t* datafile)
{
  TimeStep_t* timestep = TimeStep_New(obvals,datafile) ;
  double dtini ;
  double dtmax ;
  double dtmin ;
  double rfac ;
  double ratio ;
  
  TimeStep_SetDataFile(timestep,datafile) ;
  TimeStep_SetObVals(timestep,obvals) ;

  {
    char* filecontent = DataFile_GetFileContent(datafile) ;
    char* c  = String_FindToken(filecontent,"ALGO,TIME,Time Steps",",") ;
    
    if(!c) {
      Message_FatalError("TimeStep_Create: no Time Steps") ;
    }

    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  
    Message_Direct("Enter in %s","Time Steps") ;
    Message_Direct("\n") ;
  }
  
  
  {
    int cont = 1 ;
    
    do {
      char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
    
      /* Dtini */
      {
        int n = String_FindAndScanExp(line,"Dtini",","," = %lf",&dtini) ;
        
        if(n) {
          TimeStep_SetInitialTimeStep(timestep,dtini) ;
          continue ;
        }
      }
    
      /* Dtmax */
      {
        int n = String_FindAndScanExp(line,"Dtmax",","," = %lf",&dtmax) ;
        
        if(n) {
          TimeStep_SetMaximumTimeStep(timestep,dtmax) ;
          continue ;
        }
      }
    
      /* Dtmin */
      {
        int n = String_FindAndScanExp(line,"Dtmin",","," = %lf",&dtmin) ;
        
        if(n) {
          TimeStep_SetMinimumTimeStep(timestep,dtmin) ;
          continue ;
        }
      }
    
      /* Reduction factor */
      {
        int n = String_FindAndScanExp(line,"Reduction Factor",","," = %lf",&rfac) ;
        
        if(n) {
          TimeStep_SetReductionFactor(timestep,rfac) ;
          continue ;
        }
      }
    
      /* Common ratio */
      {
        int n = String_FindAndScanExp(line,"Common Ratio",","," = %lf",&ratio) ;
        
        if(n) {
          TimeStep_SetMaximumCommonRatio(timestep,ratio) ;
          continue ;
        }
      }
      
      cont = 0 ;
    } while(cont) ;
    
    
    /* Checkings */
    if(TimeStep_GetInitialTimeStep(timestep) <= 0) {
      arret("TimeStep_Create: Dtini is not > 0") ;
    }
    if(TimeStep_GetMaximumTimeStep(timestep) <= 0) {
      arret("TimeStep_Create: Dtmax is not > 0") ;
    }
  }
  
  return(timestep) ;
}
#elseif 0
TimeStep_t*  (TimeStep_Create)(ObVals_t* obvals,DataFile_t* datafile)
{
  TimeStep_t* timestep = TimeStep_New(obvals,datafile) ;
  double dtini ;
  double dtmax ;
  double dtmin = 0 ;
  double rfac = 0.5 ;
  double ratio = 1.5 ;

  {
    char* filecontent = DataFile_GetFileContent(datafile) ;
    char* c  = String_FindToken(filecontent,"ALGO,TIME,Time Steps",",") ;
    
    if(!c) {
      Message_FatalError("TimeStep_Create: no Time Steps") ;
    }

    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  
    Message_Direct("Enter in %s","Time Steps") ;
    Message_Direct("\n") ;
  }
  
  
  {
    int cont = 1 ;
    
    do {
      char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
    
      /* Dtini */
      {
        int n = String_FindAndScanExp(line,"Dtini",","," = %lf",&dtini) ;
        
        if(n) {
          continue ;
        }
      }
    
      /* Dtmax */
      {
        int n = String_FindAndScanExp(line,"Dtmax",","," = %lf",&dtmax) ;
        
        if(n) {
          continue ;
        }
      }
    
      /* Dtmin */
      {
        int n = String_FindAndScanExp(line,"Dtmin",","," = %lf",&dtmin) ;
        
        if(n) {
          continue ;
        }
      }
    
      /* Reduction factor */
      {
        int n = String_FindAndScanExp(line,"Reduction Factor",","," = %lf",&rfac) ;
        
        if(n) {
          continue ;
        }
      }
    
      /* Common ratio */
      {
        int n = String_FindAndScanExp(line,"Common Ratio",","," = %lf",&ratio) ;
        
        if(n) {
          continue ;
        }
      }
      
      cont = 0 ;
    } while(cont) ;

    TimeStep_Set(timestep,dtini,dtmax,dtmin,rfac,ratio);
  }
  
  return(timestep) ;
}
#else
TimeStep_t*  (TimeStep_Create)(ObVals_t* obvals,DataFile_t* datafile)
{
  TimeStep_t* timestep = TimeStep_New(obvals,datafile) ;

  TimeStep_Scan(timestep,datafile);
  
  return(timestep) ;
}
#endif




void  (TimeStep_Scan)(TimeStep_t* timestep,DataFile_t* datafile)
{
  double dtini ;
  double dtmax ;
  double dtmin = 0 ;
  double rfac = 0.5 ;
  double ratio = 1.5 ;

  {
    char* filecontent = DataFile_GetFileContent(datafile) ;
    char* c  = String_FindToken(filecontent,"ALGO,TIME,Time Steps",",") ;
    
    if(!c) {
      Message_FatalError("TimeStep_Create: no Time Steps") ;
    }

    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  
    Message_Direct("Enter in %s","Time Steps") ;
    Message_Direct("\n") ;
  }
  
  
  {
    int cont = 1 ;
    
    do {
      char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
    
      /* Dtini */
      {
        int n = String_FindAndScanExp(line,"Dtini",","," = %lf",&dtini) ;
        
        if(n) {
          continue ;
        }
      }
    
      /* Dtmax */
      {
        int n = String_FindAndScanExp(line,"Dtmax",","," = %lf",&dtmax) ;
        
        if(n) {
          continue ;
        }
      }
    
      /* Dtmin */
      {
        int n = String_FindAndScanExp(line,"Dtmin",","," = %lf",&dtmin) ;
        
        if(n) {
          continue ;
        }
      }
    
      /* Reduction factor */
      {
        int n = String_FindAndScanExp(line,"Reduction Factor",","," = %lf",&rfac) ;
        
        if(n) {
          continue ;
        }
      }
    
      /* Common ratio */
      {
        int n = String_FindAndScanExp(line,"Common Ratio",","," = %lf",&ratio) ;
        
        if(n) {
          continue ;
        }
      }
      
      cont = 0 ;
    } while(cont) ;

    TimeStep_Set(timestep,dtini,dtmax,dtmin,rfac,ratio);
  }
  
  return ;
}



double (TimeStep_ComputeTimeStep)(TimeStep_t* timestep,Solution_t* soln,double t1,double t2)
/** Return the current time increment dt so that the current time is t = tn + dt.
 *  The two times tn and t should lie within the range between t1 and t2.
 *  Also set the location of the time step dt within the range between t1 and t2 
 *  as being either at the beginning, in between or at the end of the  range [t1:t2].
 **/
{
  Nodes_t* nodes = Solution_GetNodes(soln) ;
  int nbofsequences = Nodes_GetNbOfMatrices(nodes) ;
  int sequentialindex = TimeStep_GetSequentialIndex(timestep) ;
  double tn = Solution_GetSequentialTime(soln)[sequentialindex] ;
  double dtn = Solution_GetSequentialTimeStep(soln)[sequentialindex] ;
  double dt ;
  double zero = 0. ;
  //static unsigned int step ;
  //unsigned int debut=0,entre=1,fin=2 ;
  
  if(tn < t1) {
    Message_RuntimeError("TimeStep_ComputeTimeStep: tn=%g outside the range t1=%g-t2=%g",tn,t1,t2) ;
  }
  

  if(nbofsequences > 1) {
    Message_Direct("S%d-",sequentialindex) ;
  }
  
  
  /* Compute the time increment dt */
  if(dtn == zero) { /* for the first step */
    //step = debut ;
    TimeStep_SetLocationAtBegin(timestep) ;
    dt = TimeStep_GetInitialTimeStep(timestep) ;
    
//  } else if(tn == t1 && step == fin) { /* beginning of a new date */
  } else if(tn == t1 && TimeStep_IsLocatedAtEnd(timestep)) { /* beginning of a new date */
    TimeStep_SetLocationAtBegin(timestep) ;
    /* To get around the fact that for very tiny dt: (t1 - tn) = 0 ! */
    dt = TimeStep_GetInitialTimeStep(timestep) ;
    
  } else {
    size_t n_no = Nodes_GetNbOfNodes(nodes) ;
    Node_t* no = Nodes_GetNode(nodes) ;
    ObVal_t* obval = TimeStep_GetObVal(timestep) ;
    int    obvalindex = 0 ;
    Node_t* nodecrit = NULL;
    double varmax = zero ;
    double rs ;
    
    //step = entre ;
    TimeStep_SetLocationInBetween(timestep) ;
    
    for(size_t i = 0 ; i < n_no ; i++) {
      Node_t* nodi = no + i ;
      int neq = Node_GetNbOfEquations(nodi) ;
      //double* u_1 = Node_GetCurrentUnknown(nodi) ;
      //double* u_n = Node_GetPreviousUnknown(nodi) ;
      double* u_n2 = Node_GetUnknownInDistantPast(nodi,2) ;
      double* u_n = Node_GetPreviousUnknown(nodi) ;
      int*    node_seq_ind = Node_GetSequentialIndexOfUnknown(nodi) ;
      int j ;
      
      for(j = 0 ; j < neq ; j++) {
        int k = node_seq_ind[j] ;
        
        if(k == sequentialindex) {
          ObVal_t* obval_j = obval + Node_GetObValIndex(nodi)[j] ;
          double val = ObVal_GetValue(obval_j) ;
          double varrel = fabs(u_n[j] - u_n2[j])/val ;
        
          if(ObVal_IsRelativeValue(obval_j)) {
            if(fabs(u_n2[j]) > 0.) varrel /= fabs(u_n2[j]) ;
          }

          if(nodecrit == NULL || varrel >= varmax) {
            varmax = varrel ;
            obvalindex = Node_GetObValIndex(nodi)[j] ;
            nodecrit = nodi;
          }
        }
      }
    }
    
    rs = TimeStep_GetMaximumCommonRatio(timestep) ;
    if(varmax > zero) rs = 1./varmax ;
    if(rs > TimeStep_GetMaximumCommonRatio(timestep)) {
      rs = TimeStep_GetMaximumCommonRatio(timestep) ;
    }
    
    dt = dtn * rs ;

    if(dt > TimeStep_GetMaximumTimeStep(timestep)) {
      dt = TimeStep_GetMaximumTimeStep(timestep) ;
    }
    
    if(dt < TimeStep_GetMinimumTimeStep(timestep) && dt > zero) {
      dt = TimeStep_GetMinimumTimeStep(timestep) ;
    }
    
    if(nodecrit) {
      size_t nodeindex = Node_GetNodeIndex(nodecrit);
      
      Message_Direct("(%s[%lu])",ObVal_GetNameOfUnknown(TimeStep_GetObVal(timestep) + obvalindex),nodeindex) ;
    } else {
      Message_Direct("(none[-])") ;
    }
  }
  
  /* tn + dt should not be greater than the next date */
  if(TimeStep_IsLocatedAtBegin(timestep)) {
    if(tn + dt > t2) {
      dt = t2 - tn ;
    }
  } else {
  /* The factor c>1 ensures that the last time step be not too small
   * since it will lie in between (1-c)*dt and c*dt.
   */
    double c = 1.1 ;
    
    if(tn + c*dt > t2) {
      dt = t2 - tn ;
    }
  }

  //if(tn + dt == t2) step = fin ; 
  /* End of the current date step */
  if(tn + dt == t2) {
    TimeStep_SetLocationAtEnd(timestep) ;
  }

  if(dt < zero) {
    arret("TimeStep_ComputeTimeStep: dt < 0") ;
  }
  
  return(dt) ;
}
