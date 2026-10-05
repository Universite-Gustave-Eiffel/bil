#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "Message.h"
#include "DataFile.h"
#include "Mesh.h"
#include "Elements.h"
#include "Element.h"
#include "Material.h"
#include "Fields.h"
#include "Functions.h"
#include "BConds.h"
#include "BCond.h"
#include "Mry.h"


static void     BConds_SetDefaultNameOfEquations(BConds_t*,Mesh_t*) ;


BConds_t* (BConds_New)(Fields_t* fields,Functions_t* functions)
{
  BConds_t* bconds  = (BConds_t*) Mry_New(BConds_t) ;
    
  BConds_SetNbOfBConds(bconds,0) ;
  
  /* Allocation of space for the boundary conditions */
  {
    BCond_t* bcond = Mry_Create(BCond_t,BConds_MaxNbOfBConds,BCond_New(fields,functions)) ;

    BConds_SetBCond(bconds,bcond) ;
  }
  
  return(bconds) ;
}


#if 0
BConds_t* (BConds_Create)(DataFile_t* datafile,Fields_t* fields,Functions_t* functions)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"COND,Boundary Conditions",",") ;
  size_t n_bconds = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  BConds_t* bconds = BConds_New(fields,functions) ;
  
  {
    int i = String_ToInt(c) ;

    if(i <= 0) return(bconds) ;
  }
  
  
  Message_Direct("Enter in %s","Boundary Conditions") ;
  Message_Direct("\n") ;

  /* Scan the datafile */
  {    
    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
    
    BConds_SetNbOfBConds(bconds,n_bconds);
    for(size_t ibc = 0 ; ibc < n_bconds ; ibc++) {
      BCond_t* bcond = BConds_GetBCond(bconds) + ibc ;
    
      Message_Direct("Enter in %s %d","Boundary Condition",ibc+1) ;
      Message_Direct("\n") ;
      
      BCond_Scan(bcond,datafile) ;
    }
    
  }
  
  return(bconds) ;
}
#else
BConds_t* (BConds_Create)(DataFile_t* datafile,Fields_t* fields,Functions_t* functions)
{
  BConds_t* bconds = BConds_New(fields,functions) ;
  
  BConds_Scan(bconds,datafile);
  
  return(bconds) ;
}
#endif



void (BConds_Scan)(BConds_t* bconds,DataFile_t* datafile)
{
  char* filecontent = DataFile_GetFileContent(datafile) ;
  char* c  = String_FindToken(filecontent,"COND,Boundary Conditions",",") ;
  size_t n_bconds = (c = String_SkipLine(c)) ? String_ToSize_t(c) : 0 ;
  
  {
    int i = String_ToInt(c) ;

    if(i <= 0) return ;
  }
  
  
  Message_Direct("Enter in %s","Boundary Conditions") ;
  Message_Direct("\n") ;

  /* Scan the datafile */
  {    
    c = String_SkipLine(c) ;
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
    
    BConds_SetNbOfBConds(bconds,n_bconds);
    for(size_t ibc = 0 ; ibc < n_bconds ; ibc++) {
      BCond_t* bcond = BConds_GetBCond(bconds) + ibc ;
    
      Message_Direct("Enter in %s %d","Boundary Condition",ibc+1) ;
      Message_Direct("\n") ;
      
      BCond_Scan(bcond,datafile) ;
    }
  }
  
  return ;
}



void (BConds_Delete)(void* self)
{
  BConds_t* bconds = (BConds_t*) self ;
  
  if(bconds) {
    size_t n_bconds = BConds_GetNbOfBConds(bconds) ;
    BCond_t* bcond  = BConds_GetBCond(bconds) ;
    
    Mry_Delete(bcond,n_bconds,BCond_Delete) ;
    
    Mry_Free(bcond) ;
    BConds_SetBCond(bconds,nullptr);
  }
}



void  (BConds_SetDefaultNameOfEquations)(BConds_t* bconds,Mesh_t* mesh)
/** Set the name of equations to be eliminated to their default names */
{
  size_t n_elts = Mesh_GetNbOfElements(mesh) ;
  Element_t* elt = Mesh_GetElement(mesh) ;
  BCond_t* bcond = BConds_GetBCond(bconds) ;
  size_t n_bconds = BConds_GetNbOfBConds(bconds) ;
  
  
  for(size_t ibc = 0 ; ibc < n_bconds ; ibc++) {
    BCond_t* bcond_i = bcond + ibc ;
    char*    reg_cl = BCond_GetRegionName(bcond_i) ;
    char*  inc_cl = BCond_GetNameOfUnknown(bcond_i) ;
    char*  eqn_cl = BCond_GetNameOfEquation(bcond_i) ;
    
    /* Set the name of equation to be eliminated */
    if(!strcmp(eqn_cl," ")) {
      for(size_t ie = 0 ; ie < n_elts ; ie++) {
        Element_t* elt_i = elt + ie ;
        Material_t* mat = Element_GetMaterial(elt_i) ;
        char* reg_el = Element_GetRegionName(elt_i) ;
      
        if(String_Is(reg_el,reg_cl) && mat != NULL) {
          /* Index of prescribed unknown */
          int jun = Element_FindUnknownPositionIndex(elt_i,inc_cl) ;
          
          if(jun < 0) {
            arret("BConds_SetDefaultNameOfEquations(1):\n"
                  "Unrecognized name of unknown (%s)",inc_cl) ;
          }
          
          {
            /* same j as that of inc_cl */
            int jeq = jun ;
        
            strcpy(eqn_cl,Element_GetNameOfEquation(elt_i)[jeq]) ;
          }
          
          break ;
        }
      }
    }
  }
}



void  BConds_EliminateMatrixRowColumnIndexes(BConds_t* bconds,Mesh_t* mesh)
/** Set to a negative value the matrix row/column 
 *  indexes which are prescribed by the BC */
{
  size_t n_elts = Mesh_GetNbOfElements(mesh) ;
  Element_t* elt = Mesh_GetElement(mesh) ;
  BCond_t* bcond = BConds_GetBCond(bconds) ;
  size_t n_bconds = BConds_GetNbOfBConds(bconds) ;
  
  
  BConds_SetDefaultNameOfEquations(bconds,mesh) ;

  
  /* Set to arbitrary negative value the matrix row/column indexes
   * so as to eliminate rows and columns due to boundary conditions */
  for(size_t ibc = 0 ; ibc < n_bconds ; ibc++) {
    BCond_t* bcond_i = bcond + ibc ;
    char*    reg_cl = BCond_GetRegionName(bcond_i) ;
    char*  inc_cl = BCond_GetNameOfUnknown(bcond_i) ;
    char*  eqn_cl = BCond_GetNameOfEquation(bcond_i) ;
    int    regionwasfound = 0 ;

    for(size_t ie = 0 ; ie < n_elts ; ie++) {
      Element_t* elt_i = elt + ie ;
      Material_t* mat = Element_GetMaterial(elt_i) ;
      char* reg_el = Element_GetRegionName(elt_i) ;
      
      if(String_Is(reg_el,reg_cl) && mat != NULL) {
        //int    neq = Element_GetNbOfEquations(elt_i) ;
        int    nn  = Element_GetNbOfNodes(elt_i) ;
        int    i ;

        /* Index of prescribed unknown */
        {
          int jun = Element_FindUnknownPositionIndex(elt_i,inc_cl) ;
          
          if(jun < 0) {
            arret("BConds_EliminateMatrixRowColumnIndexes(1)") ;
          }
        }

        /* We eliminate the associated column */
        for(i = 0 ; i < nn ; i++) {
          Node_t* node_i = Element_GetNode(elt_i,i) ;
          //int ij = i*neq + jun ;
          //int ii = Element_GetUnknownPosition(elt_i)[ij] ;
          
          /* Set to a arbitrary negative value */
          Node_EliminateMatrixColumnIndexForBCond(node_i,inc_cl) ;
        }

        /* Index of equation to be eliminated */
        #if 0
        if(!strcmp(eqn_cl," ")) { /* same j as inc_cl */
          jeq = jun ;
        } else {
          jeq = Element_FindEquationPositionIndex(elt_i,eqn_cl) ;
        }
        #endif
        {
          int jeq = Element_FindEquationPositionIndex(elt_i,eqn_cl) ;
          
          if(jeq < 0) arret("BConds_EliminateMatrixRowColumnIndexes(2)") ;
        }

        /* We eliminate the associated row */
        for(i = 0 ; i < nn ; i++) {
          Node_t* node_i = Element_GetNode(elt_i,i) ;
          //int ij = i*neq + jeq ;
          //int ii = Element_GetEquationPosition(elt_i)[ij] ;
          
          /* Set to a arbitrarily negative value */
          Node_EliminateMatrixRowIndexForBCond(node_i,eqn_cl) ;
        }
        
        regionwasfound = 1 ;
      }
    }
    
    if(!regionwasfound) {
      arret("BConds_EliminateMatrixRowColumnIndexes(3):\n" \
            "the region %d was not found",reg_cl) ;
    }
  }

}




void   BConds_AssignBoundaryConditions(BConds_t* bconds,Mesh_t* mesh,double t)
/** Assign the boundary conditions */
{
  int dim = Mesh_GetDimension(mesh) ;
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  size_t n_bconds = BConds_GetNbOfBConds(bconds) ;
  BCond_t* bcond = BConds_GetBCond(bconds) ;


  for(size_t ibc = 0 ; ibc < n_bconds ; ibc++) {
    BCond_t* bcond_i = bcond + ibc ;
    char*    reg_cl = BCond_GetRegionName(bcond_i) ;
    char*  inc_cl = BCond_GetNameOfUnknown(bcond_i) ;

    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Element_t* el_i = el + ie ;
      Material_t* mat = Element_GetMaterial(el_i) ;
      char* reg_el = Element_GetRegionName(el_i) ;
      
      if(String_Is(reg_el,reg_cl) && mat != NULL) {
        int  nn = Element_GetNbOfNodes(el_i) ;
        int    in ;

        /* Index of prescribed unknown */
        {
          int j = Element_FindUnknownPositionIndex(el_i,inc_cl) ;
          
          if(j < 0) arret("BConds_AssignBoundaryConditions(1)") ;
        }

        /* We assign the prescribed value to the unknown */
        for(in = 0 ; in < nn ; in++) {
          Node_t* node_i = Element_GetNode(el_i,in) ;
          
          BCond_AssignBoundaryConditionsAtOverlappingNodes(bcond_i,node_i,dim,t) ;
        }
      }
    }
  }
}
