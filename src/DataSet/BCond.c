#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <iostream>
#include "Message.h"
#include "Mry.h"
#include "String_.h"
#include "Fields.h"
#include "Field.h"
#include "Functions.h"
#include "Function.h"
#include "BCond.h"
#include "Node.h"
#include "DataFile.h"




BCond_t* (BCond_New)(Fields_t* fields,Functions_t* functions)
{
  BCond_t* bcond = (BCond_t*) Mry_New(BCond_t) ;
      
  BCond_SetFields(bcond,fields) ;
  BCond_SetFunctions(bcond,functions) ;

  /* Allocation of space for the name of unknown */
  {
    char* name = (char*) Mry_New(char,BCond_MaxLengthOfKeyWord) ;
  
    BCond_SetNameOfUnknown(bcond,name) ;
  }
    
    
  /* Allocation of space for the name of equation */
  {
    char* name = (char*) Mry_New(char,BCond_MaxLengthOfKeyWord) ;
    
    BCond_SetNameOfEquation(bcond,name) ;
  }
  
  
  /* Allocation of space for the region name */
  {
    char* name = (char*) Mry_New(char,BCond_MaxLengthOfRegionName) ;
    
    BCond_SetRegionName(bcond,name) ;
  }

  return(bcond) ;
}



void (BCond_Delete)(void* self)
{
  BCond_t* bcond = (BCond_t*) self ;
  
  {
    char* name = BCond_GetNameOfUnknown(bcond) ;
    
    if(name) {
      Mry_Free(name) ;
    }
  }
  
  {
    char* name = BCond_GetNameOfEquation(bcond) ;
    
    if(name) {
      Mry_Free(name) ;
    }
  }
  
  {
    char* name = BCond_GetRegionName(bcond) ;
    
    if(name) {
      Mry_Free(name) ;
    }
  }
}


#if 0
void BCond_Scan(BCond_t* bcond,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  
  /* Region */
  {
    char name[BCond_MaxLengthOfRegionName] ;
    int n = String_FindAndScanExp(line,"Reg",","," = %s",name) ;
    //int i ;
    //int n = String_FindAndScanExp(line,"Reg",","," = %d",&i) ;
    
    if(n) {
      strncpy(BCond_GetRegionName(bcond),name,BCond_MaxLengthOfRegionName)  ;
    } else {
      arret("BCond_Scan: no region") ;
    }
  }
    
    
  /* Unknown */
  {
    char name[BCond_MaxLengthOfKeyWord] ;
    int n = String_FindAndScanExp(line,"Unk,Inc",","," = %s",name) ;
        
    if(n) {
      strcpy(BCond_GetNameOfUnknown(bcond),name) ;
    } else {
      arret("BCond_Scan: no unknown") ;
    }
      
    if(strlen(name) > BCond_MaxLengthOfKeyWord-1)  {
      arret("BCond_Scan: too long name of unknown") ;
    }
    
    if(isdigit(BCond_GetNameOfUnknown(bcond)[0])) {
      if(BCond_GetNameOfUnknown(bcond)[0] < '1') {
        arret("BCond_Scan: non positive unknown") ;
      }
    }
  }
    
    
  /* Field */
  {
    int i ;
    int n = String_FindAndScanExp(line,"Field,Champ",","," = %d",&i) ;
    
    //BCond_SetFieldIndex(bcond,-1) ;
    BCond_SetField(bcond,NULL) ;
        
    if(n) {
      Fields_t* fields = BCond_GetFields(bcond) ;
      size_t n_fields = Fields_GetNbOfFields(fields) ;
      int ifld = i - 1 ;
      
      //BCond_SetFieldIndex(bcond,ifld) ;
      
      if(ifld < 0) {
        
        BCond_SetField(bcond,NULL) ;
        
      } else if(ifld < n_fields) {
        Field_t* field = Fields_GetField(fields) ;
        
        BCond_SetField(bcond,field + ifld) ;
        
      } else {
        
        arret("BCond_Scan: field out of range") ;
        
      }
    }
  }
    
    
  /* Function */
  {
    int i ;
    int n = String_FindAndScanExp(line,"Func,Fonc",","," = %d",&i) ;
    
    BCond_SetFunction(bcond,NULL) ;
        
    if(n) {
      Functions_t* functions = BCond_GetFunctions(bcond) ;
      size_t n_functions = Functions_GetNbOfFunctions(functions) ;
      int ifct = i - 1 ;
      
      
      if(ifct < 0) {
        
        BCond_SetFunction(bcond,NULL) ;
        
      } else if(ifct < n_functions) {
        Function_t* fct = Functions_GetFunction(functions) ;
        
        BCond_SetFunction(bcond,fct + ifct) ;
        
      } else {
        
        arret("BCond_Scan: function out of range") ;
        
      }
    }
  }
    
    
  /* Equation (not mandatory) */
  {
    char name[BCond_MaxLengthOfKeyWord] ;
    int n = String_FindAndScanExp(line,"Equ",","," = %s",name) ;
      
    if(n) {
      strcpy(BCond_GetNameOfEquation(bcond),name) ;
    } else {
      strcpy(BCond_GetNameOfEquation(bcond)," ") ;
    }
      
    if(strlen(name) > BCond_MaxLengthOfKeyWord-1)  {
      arret("BCond_Scan: too long name of equation") ;
    }
        
    if(isdigit(BCond_GetNameOfEquation(bcond)[0])) {
      if(BCond_GetNameOfEquation(bcond)[0] < '1') {
        arret("BCond_Scan: non positive equation") ;
      }
    }
  }
}
#else
void BCond_Scan(BCond_t* bcond,DataFile_t* datafile)
{
  char* line = DataFile_ReadLineFromCurrentFilePositionInString(datafile) ;
  char region[BCond_MaxLengthOfRegionName] ;
  char unknown[BCond_MaxLengthOfKeyWord] ;
  char equation[BCond_MaxLengthOfKeyWord] ;
  size_t ifld ;
  size_t ifct ;
  
  /* Region */
  {
    int n = String_FindAndScanExp(line,"Reg",","," = %s",region) ;
    
    if(!n) {
      arret("BCond_Scan: no region") ;
    }
  }
    
    
  /* Unknown */
  {
    int n = String_FindAndScanExp(line,"Unk,Inc",","," = %s",unknown) ;
        
    if(!n) {
      arret("BCond_Scan: no unknown") ;
    }
  }
    
    
  /* Field */
  {
    int n = String_FindAndScanExp(line,"Field,Champ",","," = %lu",&ifld) ;
    
    BCond_SetField(bcond,NULL) ;
        
    if(!n) {
      arret("BCond_Scan: no field") ;
    }
  }
    
    
  /* Function */
  {
    int n = String_FindAndScanExp(line,"Func,Fonc",","," = %lu",&ifct) ;
    
    BCond_SetFunction(bcond,NULL) ;
        
    if(!n) {
      arret("BCond_Scan: no function") ;
    }
  }
    
    
  /* Equation (not mandatory) */
  {
    int n = String_FindAndScanExp(line,"Equ",","," = %s",equation) ;
      
    if(!n) {
      strcpy(equation," ") ;
    }
  }

  //std::cout << "equation = " << equation << std::endl ;
  BCond_Set(bcond,region,equation,unknown,ifld,ifct);
}
#endif





void   BCond_AssignBoundaryConditionsAtOverlappingNodes(BCond_t* bcond,Node_t* node0,int dim,double t)
/** Assign the boundary conditions at nodes */
{
  Function_t* function = BCond_GetFunction(bcond) ;
  double ft = (function) ? Function_ComputeValue(function,t) : 1. ;
  char*  unk = BCond_GetNameOfUnknown(bcond) ;
  Field_t* field = BCond_GetField(bcond) ;
  int n_nodes ;
  Node_t* node = Node_OverlappingNodes(node0,&n_nodes) ;

  /* We assign the prescribed value to the unknown */
  {
    int i ;
        
    for(i = 0 ; i < n_nodes ; i++) {
      Node_t* node_i = node + i ;
      int jj = Node_FindUnknownPositionIndex(node_i,unk) ;
          
      if(jj >= 0) {
        double* u = Node_GetCurrentUnknown(node_i) ;
            
        if(field) {
          double* x = Node_GetCoordinate(node_i) ;
              
          u[jj] = ft*Field_ComputeValueAtPoint(field,x,dim) ;
              
        } else {
              
          u[jj] = 0. ;
              
        }
      }
    }
  }
  
  Node_FreeBufferFrom(node0,node) ;
}
