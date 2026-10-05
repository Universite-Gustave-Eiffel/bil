#ifndef ADJACENCYLIST_H
#define ADJACENCYLIST_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct AdjacencyList_t; //typedef struct AdjacencyList_t AdjacencyList_t ;


#include <stdio.h>

extern AdjacencyList_t* (AdjacencyList_Create)(size_t,unsigned short int*) ;
extern void             (AdjacencyList_Delete)(void*) ;


#define AdjacencyList_GetMaxNbOfNeighbors(A)       ((A)->MaxNbOfNeighbors)
#define AdjacencyList_GetNbOfNeighbors(A)          ((A)->NbOfNeighbors)
#define AdjacencyList_GetNeighbor(A)               ((A)->Neighbor)


#define AdjacencyList_AddNeighbor(A,I) \
        do {\
          if(AdjacencyList_MaxNbOfNeighborsIsNotAttained(A)) {\
            unsigned short int AdjacencyList_n = AdjacencyList_GetNbOfNeighbors(A);\
            AdjacencyList_GetNeighbor(A)[AdjacencyList_n] = I;\
            AdjacencyList_GetNbOfNeighbors(A) += 1;\
          } else {\
            Message_RuntimeError("AdjacencyList_Add: not enough space");\
          }\
        } while(0)


#define AdjacencyList_MaxNbOfNeighborsIsNotAttained(A) \
        (AdjacencyList_GetNbOfNeighbors(A) < AdjacencyList_GetMaxNbOfNeighbors(A))



struct AdjacencyList_t {
  unsigned short int  MaxNbOfNeighbors;
  unsigned short int  NbOfNeighbors;
  size_t* Neighbor;
} ;

#include "Message.h"


#ifdef __CPLUSPLUS
}
#endif
#endif
