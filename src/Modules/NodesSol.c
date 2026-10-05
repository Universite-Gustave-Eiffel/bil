#include <stdio.h>
#include "Mry.h"
#include "Mesh.h"
#include "NodesSol.h"
#include "NodeSol.h"
#include "Message.h"
#include "Nodes.h"
#include "Node.h"




/* Extern functions */

NodesSol_t* (NodesSol_Create)(Mesh_t* mesh)
{
  NodesSol_t* nodessol = (NodesSol_t*) Mry_New(NodesSol_t) ;

  {
    /* Allocation of memory for the nodesol */
    {
      size_t  NbOfNodes = Mesh_GetNbOfNodes(mesh) ;
      Nodes_t* nodes = Mesh_GetNodes(mesh) ;
      NodeSol_t* nodesol = (NodeSol_t*) Mry_New(NodeSol_t,NbOfNodes) ;
    
      NodesSol_SetNodeSol(nodessol,nodesol) ;
      NodesSol_SetNbOfNodes(nodessol,NbOfNodes) ;
      NodesSol_SetNodes(nodessol,nodes) ;
      
      
      {
        Node_t* node = Mesh_GetNode(mesh) ;
        
        for(size_t i = 0 ; i < NbOfNodes ; i++) {
          unsigned short int nu = Node_GetNbOfUnknowns(node + i) ;
          NodeSol_t* nsol = NodeSol_Create(nu) ;
          
          nodesol[i] = nsol[0] ;
          Mry_Free(nsol) ;
        }
      }
    }
  }
  
  return(nodessol) ;
}



void (NodesSol_Delete)(void* self)
{
  NodesSol_t* nodessol = (NodesSol_t*) self ;
  
  {
    size_t  NbOfNodes = NodesSol_GetNbOfNodes(nodessol) ;
    NodeSol_t* nodesol = NodesSol_GetNodeSol(nodessol) ;
    
    Mry_Delete(nodesol,NbOfNodes,NodeSol_Delete) ;
    Mry_Free(nodesol) ;
  }
}



void (NodesSol_Copy)(NodesSol_t* nodessol_dest,NodesSol_t* nodessol_src)
/** Copy the nodal unknowns from nodessol_src to nodessol_dest */
{
  Nodes_t* nodes = NodesSol_GetNodes(nodessol_src) ;
  size_t nno = NodesSol_GetNbOfNodes(nodessol_src) ;
  
  NodesSol_SetNodes(nodessol_dest,nodes) ;
  NodesSol_SetNbOfNodes(nodessol_dest,nno) ;
  
  /* Nodal values */
    {
      NodeSol_t* nodesol_s = NodesSol_GetNodeSol(nodessol_src) ;
      NodeSol_t* nodesol_d = NodesSol_GetNodeSol(nodessol_dest) ;
      
      for(size_t in = 0 ; in < nno ; in++) {
        NodeSol_t* nodesoli_s = nodesol_s + in ;
        NodeSol_t* nodesoli_d = nodesol_d + in ;
        double* u_s = NodeSol_GetUnknown(nodesoli_s) ;
        double* u_d = NodeSol_GetUnknown(nodesoli_d) ;
        unsigned short int nu = NodeSol_GetNbOfUnknowns(nodesoli_s) ;
        
        for(unsigned short int i = 0 ; i < nu ; i++) {
          u_d[i] = u_s[i] ;
        }
      }
    }
}



void (NodesSol_CopySelectedSequentialUnknowns)(NodesSol_t* nodessol_dest,NodesSol_t* nodessol_src,const int sequentialindex)
/** Copy the nodal unknowns of given sequential index 
 * ("sequentialindex") from nodessol_src to nodessol_dest. */
{
  Nodes_t* nodes = NodesSol_GetNodes(nodessol_src) ;
  size_t nb_nodes = Nodes_GetNbOfNodes(nodes) ;
  Node_t* node = Nodes_GetNode(nodes) ;
  NodeSol_t* nodesol_src = NodesSol_GetNodeSol(nodessol_src) ;
  NodeSol_t* nodesol_dest = NodesSol_GetNodeSol(nodessol_dest) ;

  {  
      for(size_t i = 0 ; i < nb_nodes ; i++) {
        Node_t* nodi = node + i ;
        int*    node_seq_ind = Node_GetSequentialIndexOfUnknown(nodi) ;
        unsigned short int  nb_unk = Node_GetNbOfUnknowns(nodi) ;
        double* u_src = NodeSol_GetUnknown(nodesol_src + i) ;
        double* u_dest = NodeSol_GetUnknown(nodesol_dest + i) ;
        
        {
          for(unsigned short int j = 0 ; j < nb_unk ; j++) {
            int k = node_seq_ind[j] ;
          
            if(k == sequentialindex) {
              u_dest[j] = u_src[j] ;
            }
          }
        }
      }
  }
}

