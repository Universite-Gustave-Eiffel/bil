#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include "Mry.h"
#include "Message.h"
#include "AdjacencyList.h"




AdjacencyList_t* (AdjacencyList_Create)(size_t nvert,unsigned short int* vert_nedges)
{
  AdjacencyList_t* adj = (AdjacencyList_t*) Mry_New(AdjacencyList_t,nvert) ;
  size_t nedges = 0 ;
  
  
  /* The total nb of directed edges */
  for(size_t i = 0 ; i < nvert ; i++) {
    nedges += vert_nedges[i] ;
  }


  /* Allocate memory for the adjacency list */
  {
    size_t* list = (size_t*) Mry_New(size_t,nedges) ;

    AdjacencyList_GetNeighbor(adj) = list ;
  }
  
  /* Initialize the nb of neighbors */
  {    
    for(size_t i = 0 ; i < nvert ; i++) {
      AdjacencyList_t* adji = adj + i ;
        
      AdjacencyList_GetNbOfNeighbors(adji) = 0 ;
      AdjacencyList_GetMaxNbOfNeighbors(adji) = vert_nedges[i] ;
    }
  }

  /* Initialize the pointer to the list of neighbors */
  {      
    for(size_t i = 1 ; i < nvert ; i++) {
      AdjacencyList_t* adji = adj + i ;
      size_t* list = AdjacencyList_GetNeighbor(adji-1);
      unsigned short int n = AdjacencyList_GetMaxNbOfNeighbors(adji-1);

      AdjacencyList_GetNeighbor(adji) = list + n ;
    }
  }

  return(adj) ;
}


void (AdjacencyList_Delete)(void* self)
{
  AdjacencyList_t* adj = (AdjacencyList_t*) self ;
  
  {
    size_t* list = AdjacencyList_GetNeighbor(adj) ;
    
    if(list) {
      Mry_Free(list) ;
      AdjacencyList_GetNeighbor(adj) = NULL ;
    }
  }
}

