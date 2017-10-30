#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include "Message.h"
#include "Graph.h"


static AdjacencyList_t* AdjacencyList_Create(int,int*) ;
static void             AdjacencyList_Delete(AdjacencyList_t**) ;


Graph_t*  Graph_Create(int nvert,int* vert_nedges)
{
  Graph_t* graph = (Graph_t*) malloc(sizeof(Graph_t)) ;
  
  if(!graph) {
    arret("Graph_Create(1)") ;
  }
  
  
  Graph_GetNbOfVertices(graph) = nvert ;
  Graph_GetNbOfEdges(graph) = 0 ;
  
  
  {
    AdjacencyList_t* adj = AdjacencyList_Create(nvert,vert_nedges) ;
    
    Graph_GetAdjacencyList(graph) = adj ;
  }

  return(graph) ;
}


void Graph_Delete(Graph_t** graph)
{
  AdjacencyList_Delete(&Graph_GetAdjacencyList(*graph)) ;
  free(*graph) ;
}


AdjacencyList_t* AdjacencyList_Create(int nvert,int* vert_nedges)
{
  int nedges ;
  AdjacencyList_t* adj = (AdjacencyList_t*) malloc(nvert*sizeof(AdjacencyList_t)) ;
    
    
  if(!adj) {
    arret("AdjacencyList_Create(1)") ;
  }
  
  
  /* Nb of directed edges */
  {
    int i ;
    
    nedges = 0 ;
    for(i = 0 ; i < nvert ; i++) {
      nedges += vert_nedges[i] ;
    }
  }
  
  
  /* Allocate memory for the adjacency list */
  {
    int i ;
    int* list = (int*) malloc(nedges*sizeof(int)) ;
      
    if(!list) {
      arret("AdjacencyList_Create(2)") ;
    }
    
    for(i = 0 ; i < nedges ; i++) {
      list[i] = -1 ;
    }
      
    {
      int j = 0 ;
      
      for(i = 0 ; i < nvert ; i++) {
        AdjacencyList_t* adji = adj + i ;
        
        AdjacencyList_GetNbOfNeighbors(adji) = 0 ;
        AdjacencyList_GetNeighbor(adji) = list + j ;
        j += vert_nedges[i] ;
      }
    }
  }

  return(adj) ;
}


void AdjacencyList_Delete(AdjacencyList_t** adj)
{
  free(AdjacencyList_GetNeighbor(*adj)) ;
  free(*adj) ;
}
