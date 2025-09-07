#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include "Math_.h"
#include "Symmetry.h"
#include "Elements.h"
#include "Element.h"
#include "Nodes.h"
#include "Node.h"
#include "Geometry.h"
#include "Message.h"
#include "DataFile.h"
#include "Materials.h"
#include "Models.h"
#include "Periodicities.h"
#include "Graph.h"
#include "Solutions.h"
#include "Context.h"
#include "TextFile.h"
#include "String_.h"
#include "Mry.h"
#include "Mesh.h"
#include "SharedMS.h"
#include "DistributedMS.h"
#include "BConds.h"
#include "Solver.h"
#include "ObVals.h"
#include "ObVal.h"
#include "Matrix.h"
#include "Residu.h"
#include "Loads.h"
#include "Load.h"
#include "Regions.h"
#include "IntFcts.h"


#if defined(__cplusplus)
  extern "C" {
#endif

extern void   mc40ad_(int*,int*,int*,int*,int*,int*,int*,int*,int*,int*) ;
extern void   mc43ad_(int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*) ;

#if defined(__cplusplus)
  }
#endif

static int*      (Mesh_ComputeInversePermutationOfNodes)(Mesh_t*,const char*) ;
static int*      (Mesh_ComputeInversePermutationOfElements)(Mesh_t*,const char*) ;
static Graph_t*  (Mesh_CreateGraph)(Mesh_t*) ;
static void      (Mesh_SetNodeConnectivities)(Mesh_t*) ;


static void   Mesh_OneNode(Mesh_t*) ;

static void   Mesh_ReadInline1d(Mesh_t*,char*) ;


static void   Mesh_ReadFormatGmsh(Mesh_t*,const char*) ;
static void   Mesh_ReadFormatGmsh_1(Mesh_t*,const char*) ;
static void   Mesh_ReadFormatGmsh_2(Mesh_t*,const char*) ;
static void   Mesh_Readm1d(Mesh_t*,const char*) ;
static void   Mesh_ReadFormatCesar(Mesh_t*,const char*) ;

static void   maillage(double*,size_t*,double,int,Node_t*) ;
static size_t mesh1dnew(double*,double*,int,Node_t*) ;
//static void   ecrit_mail_msh_1(Mesh_t*,const char*) ;
//static void   ecrit_mail_msh_2(Mesh_t*,const char*) ;
static int    gmsh_ElmType(unsigned short int,unsigned short int) ;
static unsigned short int    gmsh_NbNodes(int) ;
static unsigned short int    gmsh_DimElement(int) ;

static void     (Mesh_AssembleElementaryMatrices)(Mesh_t*,Matrix_t*) ;
static void     (Mesh_AssembleElementaryResidus)(Mesh_t*,Residu_t*) ;
static void     (Mesh_BroadcastElementaryMatrices)(Mesh_t*) ;
static void     (Mesh_BroadcastElementaryResidus)(Mesh_t*) ;





/* Extern functions */


#if 0
Mesh_t*  (Mesh_New)(void)
{
  Mesh_t* mesh = (Mesh_t*) Mry_New(Mesh_t) ;
  
  Mesh_GetElements(mesh) = (Elements_t*) Mry_New(Elements_t) ;
  
  Mesh_GetNodes(mesh) = (Nodes_t*) Mry_New(Nodes_t) ;
  
  return(mesh) ;
}
#endif



Mesh_t*  (Mesh_Create)(DataFile_t* datafile,Materials_t* materials,Geometry_t* geometry)
{
  //Mesh_t* mesh = Mesh_New() ;
  Mesh_t* mesh = (Mesh_t*) Mry_New(Mesh_t) ;
  
  {
    char* filecontent = DataFile_GetFileContent(datafile) ;
    char* c = String_FindToken(filecontent,"MAIL,MESH,Mesh",",") ;
    
    if(c) {
      c = String_SkipLine(c) ;
    } else {
      arret("Mesh_Create: Mesh not found") ;
    }
      
    DataFile_SetCurrentPositionInFileContent(datafile,c) ;
  }
   
   
  Message_Direct("Enter in %s","Mesh") ;
  Message_Direct("\n") ;
  
  Mesh_GetGeometry(mesh) = geometry ;
  Mesh_GetDataFile(mesh) = datafile ;


  {
    char* line = DataFile_GetCurrentPositionInFileContent(datafile) ;

    /* 1. Allocation memory for the mesh i.e. 
     *    node, coordinates, element and connectivity
     * ----------------------------------------------*/
  
    if(!Mesh_Scan(mesh,line)) {
      Message_FatalError("Mesh_Create: No such file name") ;
    }
    
    /* Set the node connectivities */
    //Mesh_CreateMore(mesh) ;
    Mesh_SetNodeConnectivities(mesh) ;
  }
  
  
  /* 1. Link up elements and materials (no allocation)
   * -------------------------------------------------*/
  Elements_LinkUp(Mesh_GetElements(mesh),materials) ;

  /* 2. Allocation memory space in elements for 
   *   - unknown and equation positions
   * -------------------------------------------*/
  Elements_CreateMore(Mesh_GetElements(mesh)) ;
  
  /* 3. Allocation memory space in nodes for 
   *   - the names of equations and unknowns
   *   - the indexes of matrix rows, matrix columns
   *   - the indexes of objective values
   * -----------------------------------------------*/
  Nodes_CreateMore(Mesh_GetNodes(mesh)) ;

  /* 4. Set the continuity of equations at nodes (no allocation)
   * -----------------------------------------------------------*/
  Mesh_SetEquationContinuity(mesh) ;


  return(mesh) ;
}



void (Mesh_Delete)(void* self)
{
  Mesh_t* mesh = (Mesh_t*) self ;
  
  {
    Mesh_GetDataFile(mesh) = NULL ;
    Mesh_GetGeometry(mesh) = NULL ;
  }
  
  {
    Elements_t* elts = Mesh_GetElements(mesh) ;
    
    if(elts) {
      Elements_Delete(elts) ;
      free(elts) ;
    }
    
    Mesh_GetElements(mesh) = NULL ;
  }
  
  {
    Nodes_t* nodes = Mesh_GetNodes(mesh) ;
    
    if(nodes) {
      Nodes_Delete(nodes) ;
      free(nodes) ;
    }
    
    Mesh_GetNodes(mesh) = NULL ;
  }
}



char*  (Mesh_Scan)(Mesh_t* mesh,char* line)
{
  char  nom_mail[Mesh_MaxLengthOfFileName] ;

  /* Mesh file ? */
  sscanf(line,"%s",nom_mail) ;
  
  /* Treatment after filename extension */
  if(strstr(nom_mail,".msh")) {
    
    Mesh_ReadFormatGmsh(mesh,nom_mail) ;
    //lit_mail_gmsh(mesh,nom_mail) ;
    
  } else if(strstr(nom_mail,".m1d")) {
    
    Mesh_Readm1d(mesh,nom_mail) ;
    //lit_mail_m1d(mesh,nom_mail) ;
    
  } else if(strstr(nom_mail,".ces")) {
    
    Mesh_ReadFormatCesar(mesh,nom_mail) ;
    //lit_mail_cesar(mesh,nom_mail) ;
    
  } else {
    int dim = Mesh_GetDimension(mesh) ;
      
    if(dim == 1) {
      
      /* Read directly in the data file */
      Mesh_ReadInline1d(mesh,line) ;
      //mail1d(mesh,line) ;
      
    } else {
      
      Mesh_OneNode(mesh) ;
      Message_Direct("Mesh_Scan: a mesh with only one node is created!\n") ;
      
      //return(NULL) ;
      
    }
  }
  
  return(line) ;
}



void (Mesh_SetNodeConnectivities)(Mesh_t* mesh)
{
  /* Set the nb of elements per node */
  {
    size_t nno = Mesh_GetNbOfNodes(mesh) ;
    Node_t* node = Mesh_GetNode(mesh) ;
    size_t nel = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    
    {    
      for(size_t jn = 0 ; jn < nno ; jn++) {
        Node_t* node_j = node + jn ;
        
        Node_GetNbOfElements(node_j) = 0 ;
      }
    }
    
    {      
      for(size_t je = 0 ; je < nel ; je++) {
        Element_t* el_j = el + je ;
        int nn = Element_GetNbOfNodes(el_j) ;
      
        for(int k = 0 ; k < nn ; k++) {
          Node_t* node_k = Element_GetNode(el_j,k) ;
          
          Node_GetNbOfElements(node_k) += 1 ;
        }
      }
    }
  }
    
  /* Set the pointers */
  {
    size_t nno = Mesh_GetNbOfNodes(mesh) ;
    Node_t* node = Mesh_GetNode(mesh) ;
    Element_t** pel = Node_GetPointerToElement(node) ;
    size_t nc = 0 ;
    
    {          
      for(size_t jn = 0 ; jn < nno ; jn++) {
        Node_t* node_j = node + jn ;
        
        Node_GetPointerToElement(node_j) = pel + nc ;
        nc += Node_GetNbOfElements(node_j) ;
      }
    }
  }
    
  
  /* Set the node connectivities */
  {
    size_t nno = Mesh_GetNbOfNodes(mesh) ;
    Node_t* node = Mesh_GetNode(mesh) ;
    size_t nel = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;

    {    
      for(size_t jn = 0 ; jn < nno ; jn++) {
        Node_t* node_j = node + jn ;
        
        Node_GetNbOfElements(node_j) = 0 ;
      }
    }
    
    {    
      for(size_t je = 0 ; je < nel ; je++) {
        Element_t* el_j = el + je ;
        int nn = Element_GetNbOfNodes(el_j) ;
      
        for(int k = 0 ; k < nn ; k++) {
          Node_t* node_k = Element_GetNode(el_j,k) ;
          int ne = Node_GetNbOfElements(node_k) ;
          
          Node_GetElement(node_k,ne) = el_j ;

          Node_GetNbOfElements(node_k) += 1 ;
        }
      }
    }
  }
}



Graph_t* (Mesh_CreateGraph)(Mesh_t* mesh)
/** Create the graph matrix of node indexes.
 *  Return a pointer to Graph_t.
 **/
{
  Graph_t* graph ;
  
  
  /* Create a graph */
  {
    size_t    n_no = Mesh_GetNbOfNodes(mesh) ;
    /* Nb of connections per node (useful to size graph) */
    unsigned short int*   nnz_no = (unsigned short int*) calloc(n_no,sizeof(unsigned short int)) ;
  
    if(!nnz_no) {
      arret("Mesh_CreateGraph(1): impossible d\'allouer la memoire") ;
    }
  
  
    /* An overestimation of nnz_no */
    {
      size_t n_el = Mesh_GetNbOfElements(mesh) ;
      Element_t* el = Mesh_GetElement(mesh) ;
    
      for(size_t ie = 0 ; ie < n_el ; ie++) {
        unsigned short int nn = Element_GetNbOfNodes(el + ie);
    
        if(nn > 1) {
          for(unsigned short int i = 0 ; i < nn ; i++) {
            Node_t* node = Element_GetNode(el + ie,i) ;
            size_t k = Node_GetNodeIndex(node) ;
      
            nnz_no[k] += nn - 1 ;
          }
        }
      }
    }
    
    graph = Graph_Create(n_no,nnz_no) ;

    free(nnz_no) ;
  }


  /* Compute the graph */
  {
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;

    for(size_t ie = 0 ; ie < n_el ; ie++) {
      int nn = Element_GetNbOfNodes(el + ie) ;
    
      for(int i = 0 ; i < nn ; i++) {
        Node_t* node_i = Element_GetNode(el + ie,i) ;
        size_t  in = Node_GetNodeIndex(node_i) ;
        size_t* listin = Graph_GetNeighborOfVertex(graph,in) ;
        int  degrin = Graph_GetDegreeOfVertex(graph,in) ;
      
        for(int j = 0 ; j < nn ; j++) {
          Node_t* node_j = Element_GetNode(el + ie,j) ;
          size_t  jn = Node_GetNodeIndex(node_j) ;
          size_t* listjn = Graph_GetNeighborOfVertex(graph,jn) ;
          int  degrjn = Graph_GetDegreeOfVertex(graph,jn) ;
        
          if(in == jn) continue ;
        
          /* Has jn been already met? */
          {
            int met = 0 ;
            int k ;
            
            for(k = 0 ; k < degrin ; k++) {
              if(jn == listin[k]) {met = 1 ; break ;}
            }
        
            if(met) continue ;
          }
        
          /* Not already met. So we increment with jn and in */
          #if 0
          //if(listin[degrin] < 0) {
          if(Graph_MaxDegreeNotAttainedAtVertex(graph,in)) {
            Graph_GetDegreeOfVertex(graph,in) += 1 ;
            listin[degrin] = jn ;
          } else {
            arret("Mesh_CreateGraph(3): not enough space") ;
          }
          
          //if(listjn[degrjn] < 0) {
          if(Graph_MaxDegreeNotAttainedAtVertex(graph,jn)) {
            Graph_GetDegreeOfVertex(graph,jn) += 1 ;
            listjn[degrjn] = in ;
          } else {
            arret("Mesh_CreateGraph(4): not enough space") ;
          }
          #endif
          Graph_AddEdge(graph,in,jn);
          
        }
      }
    }
  }
  
    
  /* Update the graph for periodic mesh/BC */
  {
    Periodicities_t* periodicities = Mesh_GetPeriodicities(mesh) ;
  
    if(periodicities) {
      Periodicities_UpdateGraph(mesh,graph) ;
    }
  }
  
  
  /* Nb of edges */
  Graph_UpdateTheNbOfEdges(graph) ;

  
  return(graph) ;
}



void  (Mesh_SetMatrixRowColumnIndexes)(Mesh_t* mesh,BConds_t* bconds)
/** Set matrix row (column) index which node equation (unknown) points to
 *  by using the inverse permutation vector.
 **/
{
  /* Set indexes to arbitrary >= 0 value */
  Mesh_InitializeMatrixRowColumnIndexes(mesh) ;

  /* Accounting for BC 
   * (set indexes to arbitray < 0 value) */
  BConds_EliminateMatrixRowColumnIndexes(bconds,mesh) ;
  
  /* Set up the system */
  Mesh_UpdateMatrixRowColumnIndexes(mesh) ;

  return ;
}



void  (Mesh_UpdateMatrixRowColumnIndexes)(Mesh_t* mesh)
/** Set matrix row (column) index which node equation (unknown) points to
 *  by using the inverse permutation vector.
 **/
{
  /* Accounting for periodic mesh/BC 
   * (set indexes of slave nodes to arbitray < 0 value) */
  Periodicities_EliminateMatrixRowColumnIndexes(mesh) ;
  
  /* Accounting continuous unknowns across zero-thickness elements 
   * (set indexes of overlapping (slave) nodes to arbitray < 0 value) */
  {
    Elements_t* elements = Mesh_GetElements(mesh) ;
    
    Elements_EliminateMatrixRowColumnIndexesOfOverlappingNodes(elements) ;
  }

  /* We set up the system */
  {
    Nodes_t* nodes = Mesh_GetNodes(mesh) ;
    DataFile_t* datafile = Mesh_GetDataFile(mesh) ;
    
    Nodes_SetMatrixRowColumnIndexes(nodes,datafile) ;
  }

  /* Update indexes of slave nodes for periodic mesh */
  Periodicities_UpdateMatrixRowColumnIndexes(mesh) ;
  
  /* Update indexes of overlapping (slave) nodes of zero-thickness elements */
  {
    Elements_t* elements = Mesh_GetElements(mesh) ;
    
    Elements_UpdateMatrixRowColumnIndexesOfOverlappingNodes(elements) ;
  }
  
  return ;
}



void  (Mesh_InitializeMatrixRowColumnIndexes)(Mesh_t* mesh)
/** Initialize the Matrix Row/Column Indexes to >= 0 */
{

  /* Initialization to (arbitrarily) negative value 
   * so as to eliminate dof of isolated nodes or
   * dof of negative position (see below).
   */
  {
    Nodes_t* nodes = Mesh_GetNodes(mesh) ;
    
    Nodes_InitializeMatrixRowColumnIndexes(nodes) ;
  }


  /* Initialization to 0 for nodes belonging to elements */
  {
    Elements_t* elements = Mesh_GetElements(mesh) ;
    
    Elements_InitializeMatrixRowColumnIndexes(elements) ;
  }
  
  /* Set up the system without restrictions */
  {
    Nodes_t* nodes = Mesh_GetNodes(mesh) ;
    
    Nodes_SetMatrixRowColumnIndexes(nodes,NULL) ;
  }
  
  return ;
}



void (Mesh_WriteGraph)(Mesh_t* mesh,const char* nom,const char* format)
/** Create the graph file "nom.graph" in the format "format"
 */
{
  Graph_t*  graph = Mesh_CreateGraph(mesh) ;
  FILE*  fic_graph ;
  
  
  /* Open the file "nom.graph" */
  {
    char   nom_graph[Mesh_MaxLengthOfFileName] ;
    
    if(strlen(nom) + 6 > Mesh_MaxLengthOfFileName) {
      arret("Mesh_WriteGraph(1): too long name") ;
    }
  
    sprintf(nom_graph,"%s.graph",nom) ;
  
    printf("Creation of the graph file %s\n",nom_graph) ;
  
    fic_graph = fopen(nom_graph,"w") ;
  
    if(!fic_graph) {
      arret("Mesh_WriteGraph(2): cannot open %s\n",nom_graph) ;
    }
  }


  /* Write the first line */
  {
    size_t    n_no = Mesh_GetNbOfNodes(mesh) ;
    size_t    nnz  = Graph_GetNbOfEdges(graph) ;
    
    fprintf(fic_graph,"%lu %lu\n",n_no,nnz) ;
  }


  /* Format HSL_MC40 */
  if(String_Is(format,"hsl") || String_Is(format,"hsl_mc40")) {
    size_t    n_no = Mesh_GetNbOfNodes(mesh) ;
    
    for(size_t in = 0 ; in < n_no ; in++) {
      int  degree = Graph_GetDegreeOfVertex(graph,in) ;
      size_t* list   = Graph_GetNeighborOfVertex(graph,in) ;
      
      for(int i = 0 ; i < degree ; i++) {
        size_t jn = list[i] ;
        
        if(jn < in) fprintf(fic_graph,"%lu %lu\n",in+1,jn+1) ;
      }
    }
    
  /* Format METIS */
  } else if(String_Is(format,"metis")) {
    size_t    n_no = Mesh_GetNbOfNodes(mesh) ;
    
    for(size_t in = 0 ; in < n_no ; in++) {
      int  degree = Graph_GetDegreeOfVertex(graph,in) ;
      size_t* list   = Graph_GetNeighborOfVertex(graph,in) ;
      
      for(int i = 0 ; i < degree ; i++) {
        size_t jn = list[i] ;
        
        fprintf(fic_graph,"%lu ",jn+1) ;
      }
      fprintf(fic_graph,"\n") ;
    }
    
  } else {
    arret("Mesh_WriteGraph: non prevu") ;
  }

  /* fermeture du fichier */
  fclose(fic_graph) ;
  
  Graph_Delete(graph) ;
  free(graph) ;
}


void   (Mesh_WriteInversePermutation)(Mesh_t* mesh,const char* nom,const char* format)
/** Create the inverse permutation file "nom.graph.iperm" */
{
  char   nom_iperm[Mesh_MaxLengthOfFileName] ;

  if(strlen(nom) + 12 > Mesh_MaxLengthOfFileName) {
    arret("Mesh_WriteInversePermutation(6): too long name") ;
  }
    
  sprintf(nom_iperm,"%s.graph.iperm",nom) ;
  printf("Creation of the Graph.iperm file = %s\n",nom_iperm) ;
    
  {
    FILE*  fic_iperm = fopen(nom_iperm,"w") ;
  
    if(!fic_iperm) {
      arret("Mesh_WriteInversePermutation(7): can't open the file\n") ;
    }
      
    if(String_Is(format,"hsl") || String_Is(format,"hsl_mc40")) {
      size_t    n_no = Mesh_GetNbOfNodes(mesh) ;
      int*   iperm = Mesh_ComputeInversePermutationOfNodes(mesh,format) ;

      for(size_t i = 0 ; i < n_no ; i++) {
        fprintf(fic_iperm,"%d\n",iperm[i] - 1) ;
      }
        
      free(iperm) ;
    } else if(String_Is(format,"hsl_mc43")) {
      size_t    nelt = Mesh_GetNbOfNodes(mesh) ;
      int*   norder = Mesh_ComputeInversePermutationOfElements(mesh,format) ;

      for(size_t i = 0 ; i < nelt ; i++) {
        fprintf(fic_iperm,"%d\n",norder[i] - 1) ;
      }
      
      free(norder) ;
    }
  
    fclose(fic_iperm) ;
  }
}


int*   (Mesh_ComputeInversePermutationOfNodes)(Mesh_t* mesh,const char* format)
{
  size_t n_no = Mesh_GetNbOfNodes(mesh) ;
  int*   iperm = (int*) malloc(n_no*sizeof(int)) ;
  
  if(!iperm) {
    arret("Mesh_ComputeInversePermutationOfNodes(1): not enough memory") ;
  }
  
  
  if(String_Is(format,"hsl") || String_Is(format,"hsl_mc40")) {
    Graph_t*  graph = Mesh_CreateGraph(mesh) ;
    size_t    nnz  = Graph_GetNbOfEdges(graph) ;
    int*   irn = (int*) malloc(2*nnz*sizeof(int)) ;
    int*   jcn = (int*) malloc(nnz*sizeof(int)) ;
  
    if(!irn || !jcn) {
      arret("Mesh_ComputeInversePermutationOfNodes(2): not enough memory") ;
    }

    /* Compute the row and column indexes: irn and jcn */
    {
      size_t k = 0 ;
  
      for(size_t in = 0 ; in < n_no ; in++) {
        unsigned short int  degrin = Graph_GetDegreeOfVertex(graph,in) ;
        size_t* listin = Graph_GetNeighborOfVertex(graph,in) ;
    
        for(unsigned short int i = 0 ; i < degrin ; i++) {
          size_t jn = listin[i] ;
      
          if(jn < in) {
            irn[k] = in + 1 ;
            jcn[k] = jn + 1 ;
            k++;
          }
        }
      }
    }
  
    Graph_Delete(graph) ;
    free(graph) ;
    
    /* From the package HSL_MC40 */
    {
      int   iflag ;
      int   itype = 1 ;
      int   iprof[2] ;
      int*  icptr = (int*) malloc((n_no + 1)*sizeof(int)) ;
      int*  iw = (int*) malloc((3*n_no + 2)*sizeof(int)) ;
    
      if(!icptr || !iw) {
        arret("Mesh_ComputeInversePermutationOfNodes(4): not enough memory") ;
      }
      
      mc40ad_(&itype,(int*) &n_no,(int*) &nnz,irn,jcn,icptr,iperm,iw,iprof,&iflag) ;
      
      if(iflag < 0) {
        Message_FatalError("Mesh_ComputeInversePermutationOfNodes: something wrong happened in mc40ad\n") ;
        Message_FatalError("iflag = %d\n",iflag) ;
      }
      
      Message_Direct("Profile of the matrix:\n") ;
      Message_Direct("    original ordering  %d\n",iprof[0]) ;
      Message_Direct("    new ordering       %d\n",iprof[1]) ;
      /*
      Message_Direct("The profile of the matrix is the number of coefficients\n") ;
      Message_Direct("of the lower triangle when any zero ahead of the first \n") ;
      Message_Direct("entry in its row is excluded.\n") ;
      */
    
      free(icptr) ;
      free(iw) ;
    }
  
    free(irn) ;
    free(jcn) ;
  } else {
    arret("Mesh_ComputeInversePermutationOfNodes(5): format %s unknown",format) ;
  }
  

  return(iperm) ;
}


int*   (Mesh_ComputeInversePermutationOfElements)(Mesh_t* mesh,const char* format)
{
  Element_t* elt = Mesh_GetElement(mesh) ;
  size_t  nno = Mesh_GetNbOfNodes(mesh) ;
  size_t  nelt = Mesh_GetNbOfElements(mesh) ;
  int*   norder = (int*) malloc(nelt*sizeof(int)) ;
  
  if(!norder) {
    arret("Mesh_ComputeInversePermutationOfElements(1): not enough memory") ;
  }
  
  
  if(String_Is(format,"hsl_mc43")) {
    
    /* From the package HSL_MC43 */
    {
      /* If icntl = 0 the direct element reordering algorithm is employed; 
       * If icntl = 1 the indirect element reordering algorithm is employed
       */
      int  icntl = 1 ;
      int  nz ;
      int* eltptr ;
      int* eltvar ;
      int  liw ;
      int* iw ;
      int  mxwave[2] ;
      int  iflag ;
      
      eltptr = (int*) malloc((nelt + 1)*sizeof(int)) ;
      
      if(!eltptr) {
        arret("Mesh_ComputeInversePermutationOfElements(4): not enough memory") ;
      }
      
      {        
        eltptr[0] = 1 ;
        for(size_t i = 0 ; i < nelt ; i++) {
          int nn = Element_GetNbOfNodes(elt + i) ;
        
          eltptr[i + 1] = eltptr[i] + nn ;
        }
      }
      
      nz = eltptr[nelt] - 1 ;
      
      eltvar = (int*) malloc(nz*sizeof(int)) ;
      
      if(!eltvar) {
        arret("Mesh_ComputeInversePermutationOfElements(4): not enough memory") ;
      }
      
      {
        int k = 0 ;
        
        for(size_t i = 0 ; i < nelt ; i++) {
          int nn = Element_GetNbOfNodes(elt + i) ;
        
          for(int j = 0 ; j < nn ; j++) {
            Node_t* no = Element_GetNode(elt + i,j) ;
            int in = (int) Node_GetNodeIndex(no) ;
            
            eltvar[k] = in + 1 ;
            k++ ;
          }
        }
      }
      
      {
        if(icntl == 1) {
          int nnmax = 0 ;
        
          for(size_t i = 0 ; i < nelt ; i++) {
            int nn = Element_GetNbOfNodes(elt + i) ;
        
            if(nn > nnmax) nnmax = nn ;
          }
          
          liw = 3*(nno + nz + 1) + nelt*(nnmax*nnmax + 1) ;
        
          if(liw < nelt*64) liw = nelt*64 ;
        } else if(icntl == 0) {
          int maxel = 10 ; /* Need to be calculated */
          
          liw = 3*nz + 2*nelt*(maxel + 3) + nno + 3 ;
          
          if(liw < 2*nno) liw = 2*nno ;
        }
        
        iw = (int*) malloc(liw*sizeof(int)) ;
        
        if(!iw) {
          arret("Mesh_ComputeInversePermutationOfElements(4): not enough memory") ;
        }
      }
      
      {
        int ne = (int) nelt;
        int n = (int) nno ;
        
        mc43ad_(&icntl,&n,&ne,&nz,eltvar,eltptr,norder,&liw,iw,mxwave,&iflag) ;
      }
      
      if(iflag < 0) {
        Message_FatalError("Mesh_ComputeInversePermutationOfElements: something wrong happened in mc43ad\n") ;
        Message_FatalError("iflag = %d\n",iflag) ;
      }
      
      Message_Direct("Maximum wavefront of the matrix:\n") ;
      Message_Direct("    original ordering  %d\n",mxwave[0]) ;
      Message_Direct("    new ordering       %d\n",mxwave[1]) ;
      
      free(eltptr) ;
      free(eltvar) ;
      free(iw) ;
    }
  } else {
    arret("Mesh_ComputeInversePermutationOfElements(5): format %s unknown",format) ;
  }
  

  return(norder) ;
}



void (Mesh_SetEquationContinuity)(Mesh_t* mesh)
/** Set the continuity of unknowns/equations at nodes, i.e.:
 *  - the number of unknowns/equations at nodes
 *  - the names of unknowns/equations at nodes
 *  - the position of unknowns/equations at the nodes of elements
 *  - the sequential index of unknowns at nodes
 *    (limited in the range between 0 and NbOfMatrices-1)
 */
{
   
  /* Initialize the nb of unknowns/equations at nodes */
  {
    Node_t* no = Mesh_GetNode(mesh) ;
    size_t n_no = Mesh_GetNbOfNodes(mesh) ;
    
    for(size_t in = 0 ; in < n_no ; in++) {
      Node_GetNbOfUnknowns(no + in) = 0 ;
      Node_GetNbOfEquations(no + in) = 0 ;
    }
  }
  
  /* Compute
   *  - the number of equations per node: Node_GetNbOfEquations(no)
   *  - the number of unknowns  per node: Node_GetNbOfUnknowns(no)
   *  - the name of equations per node: Node_GetNameOfEquation(no)
   *  - the name of unknowns  per node: Node_GetNameOfUnknown(no)
   *  - the position of equations at nodes of each element: Element_GetEquationPosition(el)
   *  - the position of unknowns  at nodes of each element: Element_GetUnknownPosition(el)
   *  - the sequential index of unknowns per nodes: Node_GetSequentialIndexOfUnknown(no)
   */
  {
    int NbOfMatrices = Mesh_GetNbOfMatrices(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Element_t* el_i = el + ie ;
      Material_t* mat = Element_GetMaterial(el_i) ;
    
      if(mat) {
        int nn = Element_GetNbOfNodes(el_i) ;
        int neq = Element_GetNbOfEquations(el_i) ;
        char** mat_name_unk = Material_GetNameOfUnknown(mat) ;
        char** mat_name_eqn = Material_GetNameOfEquation(mat) ;
        int*   mat_seq_ind = Material_GetSequentialIndexOfUnknown(mat) ;
        short int*  unk_pos = Element_GetUnknownPosition(el_i) ;
        short int*  eqn_pos = Element_GetEquationPosition(el_i) ;
        int in ;
      
        for(in = 0 ; in < nn ; in++) {
          Node_t* node_i = Element_GetNode(el_i,in) ;
          char** node_name_unk = Node_GetNameOfUnknown(node_i) ;
          char** node_name_eqn = Node_GetNameOfEquation(node_i) ;
          int*   node_seq_ind = Node_GetSequentialIndexOfUnknown(node_i) ;
          int ieq ;
        
          for(ieq = 0 ; ieq < neq ; ieq++) {
            int ij = in*neq + ieq ;

            /* Continuity of unknowns: 
             * - number of unknowns: Node_GetNbOfUnknowns(node_i), 
             * - name of unknowns:   Node_GetNameOfUnknown(node_i) 
             * - position of unknowns at nodes: Element_GetUnknownPosition(el_i)
             */
            if(unk_pos[ij] >= 0) { /* if < 0 no unknown at this node */
              short int jun ;

              for(jun = 0 ; jun < Node_GetNbOfUnknowns(node_i) ; jun++) {
                if(String_Is(node_name_unk[jun],mat_name_unk[ieq])) break ;
              }
          
              {
                /* Set the position of unknown ij at the node i */
                unk_pos[ij] = jun ;
                /* Skip if this unknown has already been met? */
                if(jun == Node_GetNbOfUnknowns(node_i)) {
                  /* Increment the number of unknowns at node i */
                  Node_GetNbOfUnknowns(node_i) += 1 ;
                  /* Set the name of the unknown ij */
                  node_name_unk[jun] = mat_name_unk[ieq] ;
                  /* Set the sequentialindex of unknown ij */
                  //node_seq_ind[jun] = mat_seq_ind[ieq] ;
                  node_seq_ind[jun] = Math_Min(mat_seq_ind[ieq],NbOfMatrices-1) ;
                }
              }
            }

            /* Continuity of equations:
             * - number of equations: Node_GetNbOfEquations(node_i)
             * - name of equations:   Node_GetNameOfEquation(node_i)
             * - position of equations: Element_GetEquationPosition(el_i)
             */
            if(eqn_pos[ij] >= 0) { /* if < 0 no equation at this node */
              short int jeq ;
            
              for(jeq = 0 ; jeq < Node_GetNbOfEquations(node_i) ; jeq++) {
                if(String_Is(node_name_eqn[jeq],mat_name_eqn[ieq])) break ;
              }
          
              {
                /* Set the position of equation ij at the node i */
                eqn_pos[ij] = jeq ;
                /* Skip if this equation has already been met? */
                if(jeq == Node_GetNbOfEquations(node_i)) {
                  /* Increment the number of equations */
                  Node_GetNbOfEquations(node_i) += 1 ;
                  /* Set the name of the equation ij */
                  node_name_eqn[jeq] = mat_name_eqn[ieq] ;
                }
              }
            }
          }
        }
      }
    }
  }
}



int (Mesh_LoadCurrentSolution)(Mesh_t* mesh,DataFile_t* datafile,double* t)
/** Load the solution from a continuous file (suffix "cont" or "conti"). 
 ** Return either i > 0 if a continuous file was found and a solution
 ** was loaded from it, or 0 if no continuous file was found. */
{
  int ires = 0 ;
  FILE* fic_cont ;

  /* ires = 0 no continuous file is found
   * ires > 0 a continuous file is found */
  {
    char* nom = DataFile_GetFileName(datafile) ;
    char   nom_cont[Mesh_MaxLengthOfFileName] ;

    if(strlen(nom) + 6 > Mesh_MaxLengthOfFileName) {
      arret("Mesh_LoadCurrentSolution(1): too long name") ;
    }
    
    /* From the highest to the lowest priority */
    if(ires == 0) {
      sprintf(nom_cont,"%s.cont",nom) ;
      {
        fic_cont = fopen(nom_cont,"rb") ;
    
        if(fic_cont) {
          ires = 2 ;
          /* Set initialization context to no initialization */
          DataFile_ContextSetToNoInitialization(datafile) ;
        }
      }
    }
    
    if(ires == 0) {
      sprintf(nom_cont,"%s.conti",nom) ;
      {
        fic_cont = fopen(nom_cont,"rb") ;
    
        if(fic_cont) {
          ires = 1 ;
          /* Set initialization context to partial initialization */
          DataFile_ContextSetToPartialInitialization(datafile) ;
        }
      }
    }
  }
  

  if(ires == 0) return(ires) ;

  /* Time */
  fscanf(fic_cont,"%lf",t) ;
  
  /* Unknowns */
  {
    size_t n_no = Mesh_GetNbOfNodes(mesh) ;
    Node_t* no = Mesh_GetNode(mesh) ;
    
    for(size_t i = 0 ; i < n_no ; i++) {
      double* u = Node_GetCurrentUnknown(no + i) ;
      int    nb_unk = Node_GetNbOfUnknowns(no + i) ;
      int j ;
    
      for(j = 0 ; j < nb_unk ; j++) {
        fscanf(fic_cont,"%lf",u + j) ;
      }
    }
  }
  
  
  {
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    
    /* Implicit terms */
    for(size_t i = 0 ; i < n_el ; i++) {
      double* vi = Element_GetCurrentImplicitTerm(el + i) ;
      size_t   n_vi = Element_GetNbOfImplicitTerms(el + i) ;
      
      for(size_t j = 0 ; j < n_vi ; j++) {
        fscanf(fic_cont,"%lf",vi + j) ;
      }
    }
  
    /* Explicit terms */
    for(size_t i = 0 ; i < n_el ; i++) {
      double* ve = Element_GetCurrentExplicitTerm(el + i) ;
      size_t   n_ve = Element_GetNbOfExplicitTerms(el + i) ;
      
      for(size_t j = 0 ; j < n_ve ; j++) {
        fscanf(fic_cont,"%lf",ve + j) ;
      }
    }
  
    /* Constant terms */
    for(size_t i = 0 ; i < n_el ; i++) {
      double* v0 = Element_GetConstantTerm(el + i) ;
      size_t   n_v0 = Element_GetNbOfConstantTerms(el + i) ;
      
      for(size_t j = 0 ; j < n_v0 ; j++) {
        fscanf(fic_cont,"%lf",v0 + j) ;
      }
    }
  }

  fclose(fic_cont) ;
  
  return(ires) ;
}



int (Mesh_StoreCurrentSolution)(Mesh_t* mesh,DataFile_t* datafile,double t)
/** Store the solution to a storage file (suffix "sto"). */
{
  FILE* fic_sto ;


  {
    char* nom = DataFile_GetFileName(datafile) ;
    char   nom_sto[Mesh_MaxLengthOfFileName] ;

    if(strlen(nom) + 4 > Mesh_MaxLengthOfFileName) {
      arret("Mesh_StoreCurrentSolution(1): too long name") ;
    }
    
    sprintf(nom_sto,"%s.sto",nom) ;
    
    fic_sto = fopen(nom_sto,"rb") ;
    if(fic_sto) {
      Message_Direct("%s has been replaced\n",nom_sto) ;
      fclose(fic_sto) ;
    }
    
    fic_sto = fopen(nom_sto,"wb") ;
    if(!fic_sto) {
      Message_FatalError("error while opening %s\n",nom_sto) ;
    }
  }

  /* Time */
  fprintf(fic_sto,"%e ",t) ;
  fprintf(fic_sto,"\n") ;
  
  
  /* Unknowns */
  {
    size_t n_no = Mesh_GetNbOfNodes(mesh) ;
    Node_t* no = Mesh_GetNode(mesh) ;
    
    for(size_t i = 0 ; i < n_no ; i++) {
      double* u = Node_GetCurrentUnknown(no + i) ;
      int    nb_unk = Node_GetNbOfUnknowns(no + i) ;
      int j ;
      
      for(j = 0 ; j < nb_unk ; j++) {
        fprintf(fic_sto,"%.12e ",u[j]) ;
      }
      
      if(nb_unk) fprintf(fic_sto,"\n") ;
    }
  }
  
  
  {
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    
    /* Implicit terms */
    for(size_t i = 0 ; i < n_el ; i++) {
      double* vi = Element_GetCurrentImplicitTerm(el + i) ;
      size_t   n_vi = Element_GetNbOfImplicitTerms(el + i) ;
      
      for(size_t j = 0 ; j < n_vi ; j++) {
        fprintf(fic_sto,"%.12e ",vi[j]) ;
      }
      
      if(n_vi) fprintf(fic_sto,"\n") ;
    }
    
    /* Explicit terms */
    for(size_t i = 0 ; i < n_el ; i++) {
      double* ve = Element_GetCurrentExplicitTerm(el + i) ;
      size_t   n_ve = Element_GetNbOfExplicitTerms(el + i) ;
      
      for(size_t j = 0 ; j < n_ve ; j++) {
        fprintf(fic_sto,"%.12e ",ve[j]) ;
      }
      
      if(n_ve) fprintf(fic_sto,"\n") ;
    }
    
    /* Constant terms */
    for(size_t i = 0 ; i < n_el ; i++) {
      double* v0 = Element_GetConstantTerm(el + i) ;
      size_t   n_v0 = Element_GetNbOfConstantTerms(el + i) ;
      
      for(size_t j = 0 ; j < n_v0 ; j++) {
        fprintf(fic_sto,"%.12e ",v0[j]) ;
      }
      
      if(n_v0) fprintf(fic_sto,"\n") ;
    }
  }

  fclose(fic_sto) ;
  
  return(0) ;
}



void (Mesh_SetCurrentUnknownsWithBoundaryConditions)(Mesh_t* mesh,BConds_t* bconds,double t)
/* Set the current values.. */
{
  /*
    1. .. with previous ones
  */
  {
    size_t   nb_nodes = Mesh_GetNbOfNodes(mesh) ;
    Node_t*        node = Mesh_GetNode(mesh) ;
  
    for(size_t i = 0 ; i < nb_nodes ; i++) {
      int  nb_unk = Node_GetNbOfUnknowns(node + i) ;
      double* u_n = Node_GetPreviousUnknown(node + i) ;
      double* u_1 = Node_GetCurrentUnknown(node + i) ;
      int j ;
        
      for(j = 0 ; j < nb_unk ; j++) {
        u_1[j] = u_n[j] ;
      }
    }
  }
      
  /*
    2. .. and with the boundary conditions
  */
  BConds_AssignBoundaryConditions(bconds,mesh,t) ;
}



void (Mesh_UpdateCurrentUnknowns)(Mesh_t* mesh,Solver_t* solver)
{
  double* x = Solver_GetSolution(solver) ;
  Nodes_t* nodes = Mesh_GetNodes(mesh) ;
  Node_t* node = Mesh_GetNode(mesh) ;
  ObVals_t* obvals = Nodes_GetObjectiveValues(nodes) ;
  ObVal_t* obval = ObVals_GetObVal(obvals) ;
  size_t nb_nodes = Mesh_GetNbOfNodes(mesh) ;
  int imatrix = Solver_GetMatrixIndex(solver) ;
          
  for(size_t i = 0 ; i < nb_nodes ; i++) {
    int nin = Node_GetNbOfUnknowns(node + i) ;
    double* u_1 = Node_GetCurrentUnknown(node + i) ;
    int j ;
            
    for(j = 0 ; j < nin ; j++) {
      //int   k = Node_GetMatrixColumnIndex(node + i)[j] ;
      int   k = Node_GetSelectedMatrixColumnIndexOf(node + i,j,imatrix) ;
      int  iobval = Node_GetObValIndex(node + i)[j] ;
      double rfac = ObVal_GetRelaxationFactor(obval + iobval) ;
              
      if(k >= 0) u_1[j] += rfac * x[k] ;
    }
  }
  
  #if 0
  {
    int n = Solver_GetNbOfRows(solver) ;
    
    printf("\n") ;
    printf("rank = %d\n",DistributedMS_RankOfCallingProcess) ;
    Math_PrintVector(x,n) ;
    printf("\n") ;
  }
  #endif
}



/* This implementation is skipped because in some models we initialize
 * the nodal unknowns which are shared by several elements. */
#if 0 //DistributedMS_APIis(MPI)
int (Mesh_ComputeInitialState)(Mesh_t* mesh,double t)
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int size = DistributedMS_NbOfProcessors ;
  int flag = 0 ;
  
  {
    int rank = DistributedMS_RankOfCallingProcess ;

    for(size_t ie = rank ; ie < n_el ; ie += size) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        int i ;
      
        Element_FreeBuffer(el + ie) ;
        i = Element_ComputeInitialState(el + ie,t) ;
        if(i != 0) {
          flag = i ;
          break ;
        }
      }
    }
  }
    
  if(flag) return(flag) ;
  
  /* Broadcast to other processors */
  if(size > 1) {
    Mesh_BroadcastConstantTerms(mesh) ;
    Mesh_BroadcastExplicitTerms(mesh) ;
    Mesh_BroadcastImplicitTerms(mesh) ;
  }
  
  return(flag) ;
}

#else

int (Mesh_ComputeInitialState)(Mesh_t* mesh,double t)
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int flag = 0 ;
  
  {
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        int i ;
      
        Element_FreeBuffer(el + ie) ;
        i = Element_ComputeInitialState(el + ie,t) ;
        if(i != 0) {
          flag = i ;
          break ;
        }
      }
    }
  }
  
  return(flag) ;
}
#endif




int (Mesh_ComputeExplicitTerms)(Mesh_t* mesh,double t)
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int size = DistributedMS_NbOfProcessors ;
  int flag = 0 ;
  
  {
    int rank = DistributedMS_RankOfCallingProcess ;

    #if SharedMS_APIis(OpenMP)
      #pragma omp parallel for
    #endif
    for(size_t ie = rank ; ie < n_el ; ie += size) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        int    i ;
      
        Element_FreeBuffer(el + ie) ;
        i = Element_ComputeExplicitTerms(el + ie,t) ;
        if(i != 0) {
          #if SharedMS_APIis(OpenMP)
            #pragma omp critical
          #endif
          {
            flag = i ;
            n_el = 0 ;
          }
        }
      }
    }
  }
  
  return(flag) ;
}



int (Mesh_ComputeImplicitTerms)(Mesh_t* mesh,double t,double dt)
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int size = DistributedMS_NbOfProcessors ;
  int flag = 0 ;
  
  {
    int rank = DistributedMS_RankOfCallingProcess ;
    
    #if SharedMS_APIis(OpenMP)
      #pragma omp parallel for
    #endif
    for(size_t ie = rank ; ie < n_el ; ie += size) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        int    i ;
      
        Element_FreeBuffer(el + ie) ;
        i = Element_ComputeImplicitTerms(el + ie,t,dt) ;
        if(i != 0) {
          #if SharedMS_APIis(OpenMP)
            #pragma omp critical
          #endif
          {
            flag = i ;
            n_el = 0 ;
          }
        }
      }
    }
  }
  
  return(flag) ;
}





int (Mesh_ComputeMatrix)(Mesh_t* mesh,Matrix_t* a,double t,double dt)
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int size = DistributedMS_NbOfProcessors ;
  int flag = 0 ;

  Matrix_SetValuesToZero(a) ;
  
  {
    int rank = DistributedMS_RankOfCallingProcess ;

    #if SharedMS_APIis(OpenMP)
      #pragma omp parallel for
    #endif
    for(size_t ie = rank ; ie < n_el ; ie += size) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        double* ke = Element_GetMatrix(el + ie) ;
        int    i ;
      
        Element_FreeBuffer(el + ie) ;
        i = Element_ComputeMatrix(el + ie,t,dt,ke) ;
        if(i != 0) {
          #if SharedMS_APIis(OpenMP)
            #pragma omp critical
          #endif
          {
            flag = i ;
            n_el = 0 ;
          }
        }
      }
    }
  }

  if(flag) return(flag) ;
  
  /* Broadcast to other processors */
  if(size > 1) {
    Mesh_BroadcastElementaryMatrices(mesh) ;
  }
  
  /* Assembling the elementary matrices */
  Mesh_AssembleElementaryMatrices(mesh,a) ;
  
  return(flag) ;
}



void (Mesh_ComputeResidu)(Mesh_t* mesh,Residu_t* r,Loads_t* loads,double t,double dt)
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int size = DistributedMS_NbOfProcessors ;
  int rank = DistributedMS_RankOfCallingProcess ;
  
  Residu_SetValuesToZero(r) ;
  
  /* Residu */
  {  
    #if SharedMS_APIis(OpenMP)
      #pragma omp parallel for
    #endif
    for(size_t ie = rank ; ie < n_el ; ie += size) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        double* re = Element_GetResidu(el + ie) ;
      
        Element_FreeBuffer(el + ie) ;
        Element_ComputeResidu(el + ie,t,dt,re) ;
      }
    }
  }
  
  /* Loads */
  {
    int n_cg = Loads_GetNbOfLoads(loads) ;
    Load_t* cg = Loads_GetLoad(loads) ;
    
    {
      int i_cg ;
    
      for(i_cg = 0 ; i_cg < n_cg ; i_cg++) {
        char* reg_cg = Load_GetRegionName(cg + i_cg) ;
    
        #if SharedMS_APIis(OpenMP)
          #pragma omp parallel for
        #endif
        for(size_t ie = rank ; ie < n_el ; ie += size) {
          if(String_Is(Element_GetRegionName(el + ie),reg_cg)) {
            Material_t* mat = Element_GetMaterial(el + ie) ;
    
            if(mat) {
              #define NE (Element_MaxNbOfNodes*Model_MaxNbOfEquations)
              double re_cg[NE] ;
              #undef NE
                
              Element_FreeBuffer(el + ie) ;
              Element_ComputeLoads(el + ie,t,dt,cg + i_cg,re_cg) ;
      
              {
                double* re = Element_GetResidu(el + ie) ;
                int ndof = Element_GetNbOfDOF(el + ie) ;
                int i ;
                
                for(i = 0 ; i < ndof ; i++) {
                  re[i] += re_cg[i] ;
                }
              }
            }
          }
        }
      }
    }
  }
  
  /* Broadcast to other processors */
  if(size > 1) {
    Mesh_BroadcastElementaryResidus(mesh) ;
  }
  
  /* Assembling the elementary residus */
  Mesh_AssembleElementaryResidus(mesh,r) ;
}


void (Mesh_BroadcastConstantTerms)(Mesh_t* mesh)
/** Broadcast constant terms to other processors */
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int size = DistributedMS_NbOfProcessors ;

  if(size > 1) {
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        int ranksender = Element_RankOfSupportingProcessor(el + ie) ;
        
        {
          double* vc = Element_GetConstantTerm(el+ie) ;
          size_t nvc = Element_GetNbOfConstantTerms(el+ie) ;
          
          #if DistributedMS_APIis(MPI)
            MPI_Bcast(vc,nvc,MPI_DOUBLE,ranksender,MPI_COMM_WORLD) ;
          #else
            Message_FatalError("Distributed MS not available") ;
          #endif
        }
      }
    }
  }
  
  return ;
}


void (Mesh_BroadcastExplicitTerms)(Mesh_t* mesh)
/** Broadcast explicit terms to other processors */
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int size = DistributedMS_NbOfProcessors ;

  if(size > 1) {
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        int ranksender = Element_RankOfSupportingProcessor(el + ie) ;
        
        {
          double* ve = Element_GetExplicitTerm(el+ie) ;
          size_t nve = Element_GetNbOfExplicitTerms(el+ie) ;
          
          #if DistributedMS_APIis(MPI)
            MPI_Bcast(ve,nve,MPI_DOUBLE,ranksender,MPI_COMM_WORLD) ;
          #else
            Message_FatalError("Distributed MS not available") ;
          #endif
        }
      }
    }
  }
  
  return ;
}


void (Mesh_BroadcastImplicitTerms)(Mesh_t* mesh)
/** Broadcast implicit terms to other processors */
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int size = DistributedMS_NbOfProcessors ;
  
  if(size > 1) {    
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        int ranksender = Element_RankOfSupportingProcessor(el + ie) ;
        
        {
          double* vi = Element_GetImplicitTerm(el+ie) ;
          size_t nvi = Element_GetNbOfImplicitTerms(el+ie) ;
          
          #if DistributedMS_APIis(MPI)
            MPI_Bcast(vi,nvi,MPI_DOUBLE,ranksender,MPI_COMM_WORLD) ;
          #else
            Message_FatalError("Distributed MS not available") ;
          #endif
        }
      }
    }
  }
  
  return ;
}



int* (Mesh_ComputeNbOfMatrixNonzerosPerRowAndColumn)(Mesh_t* mesh,const int imatrix)
/** Return an array of int containing the number of nonzeros 
 *  per row and per column in the matrix of index imatrix.  */
{
  size_t n_col = Mesh_GetNbOfMatrixColumns(mesh)[imatrix] ;
  int*  nnzrow = (int*) Mry_New(int,2*n_col) ;
  int*  nnzcol = nnzrow + n_col ;
  
  if(imatrix >= Mesh_GetNbOfMatrices(mesh)) {
    arret("Mesh_ComputeNbOfMatrixNonzerosPerRowAndColumn") ;
  }
  
  
  {    
    for(size_t i = 0 ; i < 2*n_col ; i++) nnzrow[i] = 0 ;
  }


  {
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el0 = Mesh_GetElement(mesh) ;

    /* Max nb of terms per row */
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Element_t* el = el0 + ie ;
      int   ndof = Element_GetNbOfDOF(el) ;
      int*  row  = Element_ComputeSelectedMatrixRowAndColumnIndices(el,imatrix) ;
      int*  col  = row + ndof ;
  
      {
        int   jdof ;

        for(jdof = 0 ; jdof < ndof ; jdof++) {
          int jcol = col[jdof] ;
    
          if(jcol < 0) continue ;

          {
            int idof ;
            
            for(idof = 0 ; idof < ndof ; idof++) {
              int irow = row[idof] ;
      
              if(irow < 0) continue ;
              
              nnzrow[irow] += 1 ;
              nnzcol[jcol] += 1 ;
            }
          }
        }
      }
  
      Element_FreeBufferFrom(el,row) ;
    }
  }
  
  /* Checks */
  {
    size_t nnz = Mesh_ComputeNbOfSelectedMatrixEntries(mesh,imatrix) ;
    int nnzr = 0 ;
    int nnzc = 0 ;
    
    for(size_t i = 0 ; i < n_col ; i++) {
      nnzr += nnzrow[i] ;
      nnzc += nnzcol[i] ;
    }
    
    if((nnzr != nnzc) || (nnzr != nnz)) {
      arret("Mesh_ComputeNbOfMatrixNonzerosPerRowAndColumn") ;
    }
  }

  return(nnzrow) ;
}



int* (Mesh_ComputeNbOfSubmatrixNonzerosPerRow)(Mesh_t* mesh,const int imatrix,const int Istart,const int Iend)
/** Compute the number of the non zeros in the two submatrices of the 
 *  global matrix of index imatrix. The two submatrices are the diagonal
 *  submatrix and the off-diagonal submatrix defined by the global rows
 *  of index ranging between Istart and Iend-1.
 *  Return an array of 2*(Iend-Istart) int containing the number of 
 *  the diagonal non zeros per local row followed by 
 *  the off-diagonal non zeros per local row */
{
  int   nlocalrows = Iend - Istart ;
  int*  d_nnzrow = (int*) Mry_New(int,2*nlocalrows) ;
  int*  o_nnzrow = d_nnzrow + nlocalrows ;
  
  if(imatrix >= Mesh_GetNbOfMatrices(mesh)) {
    arret("Mesh_ComputeNbOfSubmatrixNonzerosPerRow") ;
  }
  
  
  {    
    for(int i = 0 ; i < 2*nlocalrows ; i++) d_nnzrow[i] = 0 ;
  }


  {
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el0 = Mesh_GetElement(mesh) ;

    /* Max nb of terms per row */
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Element_t* el = el0 + ie ;
      int   ndof = Element_GetNbOfDOF(el) ;
      int*  row  = Element_ComputeSelectedMatrixRowAndColumnIndices(el,imatrix) ;
      int*  col  = row + ndof ;
  
      {
        int   jdof ;

        for(jdof = 0 ; jdof < ndof ; jdof++) {
          int jcol = col[jdof] ;
    
          if(jcol < 0) continue ;

          {
            int idof ;
            
            for(idof = 0 ; idof < ndof ; idof++) {
              int irow = row[idof] ;
      
              if(irow < 0) continue ;
              
              if(irow >= Istart && irow < Iend) {
                if(jcol >= Istart && jcol < Iend) {
                  d_nnzrow[irow - Istart] += 1 ;
                } else {
                  o_nnzrow[irow - Istart] += 1 ;
                }
              }
            }
          }
        }
      }
  
      Element_FreeBufferFrom(el,row) ;
    }
  }

  return(d_nnzrow) ;
}




/* Intern functions */


void (Mesh_OneNode)(Mesh_t* mesh)
{
  size_t n_el = 1 ;
  size_t n_no = 1 ;
  
  {
    int dim = 3 ;
    size_t n_c = 1 ;
    
    Mesh_GetNodes(mesh) = Nodes_New(n_no,dim,n_c) ;
    Mesh_GetElements(mesh) = Elements_New(n_el,n_c) ;
  }

  {
    Elements_t* elts = Mesh_GetElements(mesh) ;
    Regions_t* regions = Elements_GetRegions(elts) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    Node_t* no = Mesh_GetNode(mesh) ;
    
    Element_GetElementIndex(el) = 0 ;
    Element_GetMaterialIndex(el) = 0 ;
    Element_SetDefaultRegion(el,regions,1) ;

    Element_GetDimension(el) = 3 ;

    /* Numbering */
    Element_GetNbOfNodes(el) = 1 ;
    Element_GetNode(el,0) = no ;
  }
}



void (Mesh_ReadInline1d)(Mesh_t* mesh,char* str)
/* 1D Mesh */
{
  int     npt ;
  double* pt ;
  size_t*    ne ;
  double  dx_ini ;

  /* Nb of points */
  str += String_Scan(str,"%d",&npt) ;

  pt = (double*) Mry_New(double,npt) ;

  ne = (size_t*) Mry_New(size_t,npt) ;

  /* Points */
  {    
    for(int i = 0 ; i < npt ; i++) {
      str += String_Scan(str,"%le",pt + i) ;
    }
  }
  
  /* Length of the first element */
  {
    str += String_Scan(str,"%lf",&dx_ini) ;
  }
  
  /* Nb of elements */
  {    
    for(int i = 0 ; i < npt - 1 ; i++) {
      str += String_Scan(str,"%lu",ne + i) ;
    }
  }
  
  /* Allocate memory */
  {
    unsigned short int dim = Mesh_GetDimension(mesh) ;
    size_t n_no ;
    size_t n_el = 0 ;
    size_t n_c ;
  
    /* Nb of elements */
    for(int i = 0 ; i < npt - 1 ; i++) {
      n_el += ne[i] ;
    }

    /* Nb of nodes */
    n_no = n_el + 1 ;
    
    n_c = 2*n_el ;
    
    Mesh_GetNodes(mesh) = Nodes_New(n_no,dim,n_c) ;
    Mesh_GetElements(mesh) = Elements_New(n_el,n_c) ;
  }

  /* The region and material indexes */
  {
    Elements_t* elts = Mesh_GetElements(mesh) ;
    Regions_t* regions = Elements_GetRegions(elts) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    size_t k = 0 ;
    
    for(int i = 0 ; i < npt - 1 ; i++)  {
      int imat ;
      
      str += String_Scan(str,"%d",&imat) ;
    
      for(size_t j = k ; j < k + ne[i] ; j++) {
        Element_GetMaterialIndex(el + j) = imat - 1 ;
        Element_SetDefaultRegion(el + j,regions,i+1) ;
      }
      
      k += ne[i] ;
    }
  }

  /* Set the node coordinates */
  {
    Node_t* no = Mesh_GetNode(mesh) ;
    
    maillage(pt,ne,dx_ini,npt,no) ;
  }

  free(pt) ;
  free(ne) ;


  /* Set the remaining attributes of elements and nodes */
  {
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    Node_t* no = Mesh_GetNode(mesh) ;
    Node_t** pno = Element_GetPointerToNode(el) ;
    int n_c = 0 ;
    
    for(size_t i = 0 ; i < n_el ; i++) {
      unsigned short int nn = 2 ;
      
      Element_GetNbOfNodes(el + i) = nn ;
      Element_GetDimension(el + i) = 1 ;
      Element_GetPointerToNode(el + i) = pno + n_c ;
      n_c += Element_GetNbOfNodes(el + i) ;
      
      for(int j = 0 ; j < nn ; j++) {
        Element_GetNode(el + i,j) = no + i + j ;
      }
    }

    /* Special case of elements at the surface i=0 et i=n_el-1 */
    if(n_el > 0) {
      if(Node_GetCoordinate(no)[0] == Node_GetCoordinate(no + 1)[0]) {
        Element_GetNbOfNodes(el) = 1 ;
        Element_GetDimension(el) = 0 ;
        Element_GetNode(el,0) = no + 1 ;
      }
      
      if(n_el > 1) {
        if(Node_GetCoordinate(no + n_el - 1)[0] == Node_GetCoordinate(no + n_el)[0]) {
          Element_GetNbOfNodes(el + n_el - 1) = 1 ;
          Element_GetDimension(el + n_el - 1) = 0 ;
          Element_GetNode(el + n_el - 1,0) = no + n_el - 1 ;
        }
      }
    }
  }
    
  {
    Nodes_t* nodes = Mesh_GetNodes(mesh) ;
      
    Nodes_InitializePointerToElements(nodes) ;
  }
}






void (Mesh_ReadFormatGmsh)(Mesh_t* mesh,const char* nom_msh)
/* Read a mesh in a file under the format GMSH */
{
  char   line[Mesh_MaxLengthOfTextLine] ;
  FILE*  fic_msh ;

  /* ouverture du fichier */
  fic_msh = fopen(nom_msh,"r") ;
  if(!fic_msh) {
    arret("lit_mail_gmsh (1) :impossible d\'ouvrir le fichier") ;
  }

  do {
    fgets(line,sizeof(line),fic_msh) ;
  } while(line[0] != '$') ;

  /* fermeture du fichier */
  fclose(fic_msh) ;

  if(String_Is(&line[1],"NOD",3)) { /* Version 1.0 */
    Mesh_ReadFormatGmsh_1(mesh,nom_msh) ;
    return ;
  } else if(String_Is(&line[1],"MeshFormat",10)) { /* Version 2.0 */
    Mesh_ReadFormatGmsh_2(mesh,nom_msh) ;
    return ;
  }
  arret("Mesh_ReadFormatGmsh: not available") ;
}



void (Mesh_ReadFormatGmsh_1)(Mesh_t* mesh,const char* name)
/* Read a mesh in a file under the format GMSH version 1.0 */
{
  TextFile_t* textfile = TextFile_Create(name) ;
  FILE*  strfile = TextFile_OpenFile(textfile,"r") ;
  char   mot[Mesh_MaxLengthOfKeyWord] ;
  char   line[Mesh_MaxLengthOfTextLine] ;
  size_t n_no ;
  size_t n_el ;
  size_t n_c ;

  /* The file should begin by $NOD */
  fscanf(strfile,"%s",mot) ;
  
  if(strcmp(mot,"$NOD") != 0) {
    arret("Mesh_ReadFormatGmsh_1: no $NOD") ;
  }

  /* Read the nb of nodes */
  //TextFile_ReadLineFromCurrentFilePosition(textfile,line,sizeof(line)) ;
  {
    size_t n_no_lu ;
    
    fscanf(strfile,"%lu",&n_no_lu) ;
    
    n_no = 0 ;
    for(size_t i = 0 ; i < n_no_lu ; i++) {
      size_t n ;
      
      /* le numero du noeud */
      fscanf(strfile,"%lu",&n) ;
      if(n_no < n) n_no = n ;
      /* on lit le reste de la ligne */
      if(fgets(line,sizeof(line),strfile) == NULL) {
        fprintf(stdout,"erreur ou fin de fichier\n") ;
      }
    }
    
    //Mesh_GetNbOfNodes(mesh) = n_no ;
  }
  
  fscanf(strfile,"%s",mot) ;
  
  if(strcmp(mot,"$ENDNOD") != 0) {
    arret("Mesh_ReadFormatGmsh_1 (2) : pas de $ENDNOD") ;
  }


  /* Read the nb of elements */
  fscanf(strfile,"%s",mot) ;
  
  if(strcmp(mot,"$ELM") != 0) {
    arret("Mesh_ReadFormatGmsh_1 (3) : pas de $ELM") ;
  }
  
  
  /* Read the nb of elements and the cumulative nb of nodes */
  {
    size_t n_el_lu ;

    /* nombre d'elements lus */
    fscanf(strfile,"%lu",&n_el_lu) ;
    
    n_el = 0 ;
    n_c = 0 ;
    for(size_t i = 0 ; i < n_el_lu ; i++) {
      size_t n ;
      size_t nn ;
      
      /* le numero d'element */
      fscanf(strfile,"%lu",&n) ;
      if(n_el < n) n_el = n ;
      /* on lit le reste de la ligne */
      if(fgets(line,sizeof(line),strfile) == NULL) {
        fprintf(stdout,"erreur ou fin de fichier\n") ;
      }
      /* elm_type, identificateurs et nb de noeuds */
      sscanf(line,"%*d %*d %*d %lu",&nn) ;
      n_c += nn ;
    }
    
    //Mesh_GetNbOfElements(mesh) = n_el ;
    //Mesh_GetNbOfConnectivities(mesh) = n_c ;
  }
  
  fscanf(strfile,"%s",mot) ;
  
  if(strcmp(mot,"$ENDELM") != 0) {
    arret("Mesh_ReadFormatGmsh_1 (4) : pas de $ENDELM") ;
  }
  
  
  
  /* Allocation of space for "nodes" and "elements" */
  {
    //int n_no = Mesh_GetNbOfNodes(mesh) ;
    //int n_el = Mesh_GetNbOfElements(mesh) ;
    //int n_c  = Mesh_GetNbOfConnectivities(mesh) ;
    unsigned short int dim  = Mesh_GetDimension(mesh) ;
    Nodes_t* nodes = Nodes_New(n_no,dim,n_c) ;
    Elements_t* elements = Elements_New(n_el,n_c) ;
  
    Mesh_GetNodes(mesh) = nodes ;
    Mesh_GetElements(mesh) = elements ;
  }


  /* Re-start */
  rewind(strfile) ;


  /* Nodes */
  {
    Node_t* no = Mesh_GetNode(mesh) ;
    unsigned short int dim  = Mesh_GetDimension(mesh) ;
    size_t n_no_lu ;

    fscanf(strfile,"%s",mot) ;
    
    /* nombre de noeuds */
    fscanf(strfile,"%lu",&n_no_lu) ;
    
    for(size_t i = 0 ; i < n_no_lu ; i++) {
      int n ;
      
      /* le numero du noeud */
      fscanf(strfile,"%d",&n) ;
      n -= 1 ;
      
      /* les coordonnees*/
      for(int j = 0 ; j < dim ; j++) {
        fscanf(strfile,"%le",Node_GetCoordinate(no + n) + j) ;
      }
      /* on lit le reste de la ligne */
      if(fgets(line,sizeof(line),strfile) == NULL) {
        fprintf(stdout,"erreur ou fin de fichier\n") ;
      }
    }
    
    fscanf(strfile,"%s",mot) ;
  }
  

  /* Elements */
  {
    Node_t* no = Mesh_GetNode(mesh) ;
    Elements_t* elts = Mesh_GetElements(mesh) ;
    Regions_t* regions = Elements_GetRegions(elts) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    Node_t** p_node = Element_GetPointerToNode(el) ;
    size_t n_el_lu ;
    
    fscanf(strfile,"%s",mot) ;
    
    /* nombre d'elements */
    fscanf(strfile,"%lu",&n_el_lu) ;
    
    /* les elements */
    n_c = 0 ;
    for(size_t i = 0 ; i < n_el_lu ; i++) {
      size_t n ;
      int imat ;
      int reg ;
      unsigned short int nn ;
      int type ;
      
      /* le numero d'element */
      fscanf(strfile,"%lu",&n) ; n -= 1 ;
      /* elm_type */
      fscanf(strfile,"%d",&type) ;
      /* identificateurs et nb de noeuds */
      fscanf(strfile,"%d %d %hu",&imat,&reg,&nn) ;
    
      Element_GetElementIndex(el + n) = n ;
      Element_GetMaterialIndex(el + n) = imat - 1 ;
      Element_GetNbOfNodes(el + n) = nn ;
      Element_SetDefaultRegion(el + n,regions,reg) ;
      Element_GetDimension(el + n) = gmsh_DimElement(type) ;
      if(Element_GetNbOfNodes(el + n) > Element_MaxNbOfNodes) arret("trop de noeuds") ;
      /* numerotation */
      Element_GetPointerToNode(el + n) = p_node + n_c ;
      
      n_c += nn ;
      
      for(int j = 0 ; j < nn ; j++) {
        int nodeindex ;
        
        fscanf(strfile,"%d",&nodeindex) ;
        Element_GetNode(el + n,j) = no + nodeindex - 1 ;
      }
    }
  
    fscanf(strfile,"%s",mot) ;
  }
    
  {
    Nodes_t* nodes = Mesh_GetNodes(mesh) ;
      
    Nodes_InitializePointerToElements(nodes) ;
  }
  
  TextFile_Delete(textfile) ;
  free(textfile) ;
}




void (Mesh_ReadFormatGmsh_2)(Mesh_t* mesh,const char* nom_msh)
/* Read a mesh in a file under the format GMSH version 2.0 */
{
  TextFile_t* textfile = TextFile_Create(nom_msh) ;
  FILE*  fic_msh = TextFile_OpenFile(textfile,"r") ;
  double version ;
  int    file_type,data_size ;
  char   mot[Mesh_MaxLengthOfKeyWord] ;
  char   line[Mesh_MaxLengthOfTextLine] ;
  size_t n_no = 0 ;
  size_t n_el = 0 ;
  size_t n_c = 0 ;

  /* Which version? */
  fscanf(fic_msh,"%s",mot) ;
  if(strcmp(mot,"$MeshFormat")) arret("lit_mail_gmsh_2 (2) : pas de $MeshFormat") ;
  fscanf(fic_msh, "%lf %d %d\n",&version,&file_type,&data_size) ;
  if(floor(version) != 2) arret("Error: Wrong msh file version") ;
  fscanf(fic_msh,"%s",mot) ;
  if(strcmp(mot,"$EndMeshFormat")) arret("lit_mail_gmsh_2 (2) : pas de $EndFormat") ;


  /* The file should begin by $Nodes */
  fscanf(fic_msh,"%s",mot) ;
  if(strcmp(mot,"$Nodes")) {
    arret("lit_mail_gmsh_2 (2) : pas de $Nodes") ;
  }
  
  /* Read the nb of nodes */
  {
    size_t  nb_nodes ;
    
    fscanf(fic_msh,"%lu",&nb_nodes) ;
    
    n_no = 0 ;
    for(size_t i = 0 ; i < nb_nodes ; i++) {
      size_t n ;
      
      /* le numero du noeud */
      fscanf(fic_msh,"%lu",&n) ;
      
      if(n_no < n) n_no = n ;
      
      /* on lit le reste de la ligne */
      if(!fgets(line,sizeof(line),fic_msh)) {
        arret("lit_mail_gmsh_2 (3) : erreur ou fin de fichier") ;
      }
    }
    
    //Mesh_GetNbOfNodes(mesh) = n_no ;
  }

  fscanf(fic_msh,"%s",mot) ;
  if(strcmp(mot,"$EndNodes")) arret("lit_mail_gmsh_2 (4) : pas de $EndNodes") ;



  /* Elements */
  fscanf(fic_msh,"%s",mot) ;
  if(strcmp(mot,"$Elements")) {
    arret("lit_mail_gmsh_2 (5) : pas de $Elements") ;
  }

  /* Read the nb of elements */
  {
    size_t nb_elements ;
    
    fscanf(fic_msh,"%lu",&nb_elements) ;
    
    /* calcul de n_el et de n_c */
    n_el = 0 ;
    n_c = 0 ;
    for(size_t i = 0 ; i < nb_elements ; i++) {
      size_t n;
      int elm_type ;
      
      /* le numero d'element */
      fscanf(fic_msh,"%lu",&n) ;
      
      if(n_el < n) n_el = n ;
      
      /* on lit le reste de la ligne */
      if(!fgets(line,sizeof(line),fic_msh)) {
        arret("lit_mail_gmsh_2 (6) : erreur ou fin de fichier") ;
      }
      
      /* elm_type */
      sscanf(line,"%d",&elm_type) ;
      
      n_c += gmsh_NbNodes(elm_type) ;
    }
    
    //Mesh_GetNbOfElements(mesh) = n_el ;
    //Mesh_GetNbOfConnectivities(mesh) = n_c ;
  }
  
  fscanf(fic_msh,"%s",mot) ;
  if(strcmp(mot,"$EndElements")) {
    arret("lit_mail_gmsh_2 (7) : pas de $EndElements") ;
  }
  
  
  
  /* Allocation of space for "nodes" and "elements" */
  {
    //int n_no = Mesh_GetNbOfNodes(mesh) ;
    //int n_el = Mesh_GetNbOfElements(mesh) ;
    //int n_c  = Mesh_GetNbOfConnectivities(mesh) ;
    int dim  = Mesh_GetDimension(mesh) ;
    Nodes_t* nodes = Nodes_New(n_no,dim,n_c) ;
    Elements_t* elements = Elements_New(n_el,n_c) ;
  
    Mesh_GetNodes(mesh) = nodes ;
    Mesh_GetElements(mesh) = elements ;
  }
  

  /* Re-start */
  rewind(fic_msh) ;

  /* FORMAT */
  fscanf(fic_msh,"%s",mot) ;
  fscanf(fic_msh, "%lf %d %d\n",&version,&file_type,&data_size) ;
  fscanf(fic_msh,"%s",mot) ;
  

  /* Nodes */
  {
    Node_t* no = Mesh_GetNode(mesh) ;
    int dim  = Mesh_GetDimension(mesh) ;
    size_t nb_nodes ;
    
    fscanf(fic_msh,"%s",mot) ;
    /* nombre de noeuds */
    fscanf(fic_msh,"%lu",&nb_nodes) ;
  

    for(size_t i = 0 ; i < nb_nodes ; i++) {
      int n,j ;
      
      /* le numero du noeud */
      fscanf(fic_msh,"%d",&n) ; n -= 1 ;
      
      /* les coordonnees*/
      for(j = 0 ; j < dim ; j++) {
        fscanf(fic_msh,"%le",Node_GetCoordinate(no + n) + j) ;
      }
      
      /* on lit le reste de la ligne */
      if(!fgets(line,sizeof(line),fic_msh)) {
        arret("lit_mail_gmsh_2 (10) : erreur ou fin de fichier") ;
      }
    }
    
    fscanf(fic_msh,"%s",mot) ;
  }
  

  /* Elements */
  {
    Node_t* no = Mesh_GetNode(mesh) ;
    Elements_t* elts = Mesh_GetElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    Node_t** p_node = Element_GetPointerToNode(el) ;
    Regions_t* regions = Elements_GetRegions(elts) ;
    size_t nb_elements ;
    
    fscanf(fic_msh,"%s",mot) ;
    /* nombre d'elements */
    fscanf(fic_msh,"%lu",&nb_elements) ;


    n_c = 0 ;
    for(size_t i = 0 ; i < nb_elements ; i++) {
      size_t n ;
      int elm_type,nb_tags ;
      int physical,elementary,partition ;
      char regionname[Region_MaxLengthOfRegionName] ;
      
      /* le numero d'element */
      fscanf(fic_msh,"%lu",&n) ;
      if(n > 0) n -= 1 ;
      
      /* elm_type */
      fscanf(fic_msh,"%d %d",&elm_type,&nb_tags) ;
      
      elementary = physical = partition = 1;
      
      for(int j = 0; j < nb_tags; j++){
        char tag[Region_MaxLengthOfRegionName] ;
        
        fscanf(fic_msh, "%s", tag);
          
        if(j == 0) {
          physical   = atoi(tag) ;
        } else if(j == 1) {
          elementary = atoi(tag) ;
          strcpy(regionname,tag) ;
        } else if(j == 2) {
          partition  = atoi(tag) ;
        }
        /* ignore any other tags for now */
      }
      
      Element_GetElementIndex(el + n) = n ;
      Element_GetMaterialIndex(el + n) = physical - 1 ;
      Element_GetRegion(el + n) = Regions_FindRegion(regions,regionname) ;
      Element_GetNbOfNodes(el + n) = gmsh_NbNodes(elm_type) ;
      Element_GetDimension(el + n) = gmsh_DimElement(elm_type) ;
      
      if(!Element_GetNbOfNodes(el + n)){
        arret("lit_mail_gmsh_2 (13) : Error: Unknown type for element"); 
      }
      
      if(Element_GetNbOfNodes(el + n) > Element_MaxNbOfNodes) {
        arret("lit_mail_gmsh_2 (14) : trop de noeuds") ;
      }
      
      /* numerotation */
      Element_GetPointerToNode(el + n) = p_node + n_c ;
      
      n_c += Element_GetNbOfNodes(el + n) ;
      
      for(int j = 0; j < Element_GetNbOfNodes(el + n) ; j++) {
        int nodeindex ;
        
        fscanf(fic_msh,"%d",&nodeindex) ;
        
        Element_GetNode(el + n,j) = no + nodeindex - 1 ;
      }
    }
  
    fscanf(fic_msh,"%s",mot) ;
  }
    
  {
    Nodes_t* nodes = Mesh_GetNodes(mesh) ;
      
    Nodes_InitializePointerToElements(nodes) ;
  }
  
  TextFile_Delete(textfile) ;
  free(textfile) ;
}



void (Mesh_Readm1d)(Mesh_t* mesh,const char* nom_m1d)
/* Read a 1D mesh under the format m1d */
{
  TextFile_t* textfile = TextFile_Create(nom_m1d) ;
  FILE*  fic_m1d = TextFile_OpenFile(textfile,"r") ;
  int    nreg ;
  double* pt ;
  double* lc ;

  /* Read the points and the characteristic lengths */
  {
    int npt ;
    int i ;

    /* nombre de points */
    fscanf(fic_m1d,"%d",&npt) ;

    pt = (double*) Mry_New(double,2*npt) ;

    lc = pt + npt ;
    
    for(i = 0 ; i < npt ; i++) {
      fscanf(fic_m1d,"%le %le",pt + i,lc + i) ;
    }
    
    nreg = npt - 1 ;
  }
  

  /* Allocate memory */
  {
    int dim = 1 ;
    size_t n_el = mesh1dnew(pt,lc,nreg,NULL) ;
    size_t n_no = n_el + 1 ;
    size_t n_c  = 2*n_el ;
    
    Mesh_GetNodes(mesh) = Nodes_New(n_no,dim,n_c) ;
    Mesh_GetElements(mesh) = Elements_New(n_el,n_c) ;
  }
  

  /* Nodes */
  {
    Node_t* no = Mesh_GetNode(mesh) ;
    
    mesh1dnew(pt,lc,nreg,no) ;
  }

  /* Elements */
  {
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Elements_t* elts = Mesh_GetElements(mesh) ;
    Regions_t* regions = Elements_GetRegions(elts) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    Node_t* no = Mesh_GetNode(mesh) ;
    size_t k = 0 ;

    /* les materiaux et les regions */
    for(int ireg = 0 ; ireg < nreg ; ireg++)  {
      int imat ;
      
      fscanf(fic_m1d,"%d",&imat) ; /* materiau */
      
      while(k < n_el) {
        Element_GetMaterialIndex(el + k) = imat - 1 ;
        Element_SetDefaultRegion(el + k,regions,ireg+1) ;
        k++ ;
        if(fabs(pt[ireg + 1] - Node_GetCoordinate(no + k)[0]) < lc[ireg + 1]*0.1) break ;
      }
    }
  }

  free(pt) ;
  
  /* Set the remaining attributes of element */
  {
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    Node_t* no = Mesh_GetNode(mesh) ;
    Node_t** pno = Element_GetPointerToNode(el) ;
    size_t j = 0 ;
    
    for(size_t i = 0 ; i < n_el ; i++) {
      unsigned short int nn = 2 ;
      
      Element_GetNbOfNodes(el + i) = nn ;
      Element_GetDimension(el + i) = 1 ;
      Element_GetPointerToNode(el + i) = pno + j ;
      
      for(int k = 0 ; k < nn ; k++) {
        Element_GetNode(el + i,k) = no + i + k ;
      }
      
      j += nn ;
    }
  
    /* cas d'elements de surface i=0 et i=n_el-1 */
    if(n_el > 0) {
      if(Node_GetCoordinate(no)[0] == Node_GetCoordinate(no + 1)[0]) {
        Element_GetNbOfNodes(el) = 1 ;
        Element_GetDimension(el) = 0 ;
        Element_GetNode(el,0) = no + 1 ;
      }
      if(n_el > 1) {
        if(Node_GetCoordinate(no + n_el - 1)[0] == Node_GetCoordinate(no + n_el)[0]) {
          Element_GetNbOfNodes(el + n_el - 1) = 1 ;
          Element_GetDimension(el + n_el - 1) = 0 ;
          Element_GetNode(el + n_el - 1,0) = no + n_el - 1 ;
        }
      }
    }
  }
    
  {
    Nodes_t* nodes = Mesh_GetNodes(mesh) ;
      
    Nodes_InitializePointerToElements(nodes) ;
  }
  
  TextFile_Delete(textfile) ;
  free(textfile) ;
}



void (Mesh_ReadFormatCesar)(Mesh_t* mesh,const char* nom)
/* Read a mesh in a file under the format CESAR */
{
  TextFile_t* textfile = TextFile_Create(nom) ;
  FILE*  fic_ces = TextFile_OpenFile(textfile,"r") ;
  int    dim_el[3][8]  = {{0,1,1,-1,-1,-1,-1,-1},{0,1,2,2,-1,2,-1,2},{0,1,2,3,-1,-1,-1,3}} ;
  /*
    char   type_el[3][8] = {{'P','L'},{'P','L','T','Q'},{'P','L','T','S','Y','I',' ','H'}} ; 
  */
  char   mot[Mesh_MaxLengthOfKeyWord] ;


  /* Nb of nodes */
  {
    size_t n_no ;
    
    fscanf(fic_ces,"%s",mot) ;
  
    if(strcmp(mot,"COOR") != 0) {
      arret("pas de COOR !\n") ;
    }
    
    fscanf(fic_ces,"%*d %*d %lu %*d",&n_no) ;
    
    Mesh_GetNbOfNodes(mesh) = n_no ;
    
    {
      unsigned short int dim  = Mesh_GetDimension(mesh) ;

      for(size_t i = 0 ; i < n_no*dim ; i++) {
        fscanf(fic_ces,"%*e") ;
      }
    }
  }

  
  /* Nb of elements */
  {
    size_t n_el ;
    size_t n_c ;
    
    fscanf(fic_ces,"%s",mot) ;
  
    if(strcmp(mot,"ELEM") != 0) {
      arret("pas de ELEM !\n") ;
    }
    
    fscanf(fic_ces,"%*d %*d %lu %*d",&n_el) ;
    
    /* The nb of connectivities is the last integer */
    {      
      for(size_t i = 0 ; i < n_el + 1 ; i++) {
        fscanf(fic_ces,"%lu",&n_c) ;
      }
    }
    
    Mesh_GetNbOfElements(mesh) = n_el ;
    Mesh_GetNbOfConnectivities(mesh) = n_c ;
  }
  
  
  /* Allocation of space for "nodes" and "elements" */
  {
    size_t n_no = Mesh_GetNbOfNodes(mesh) ;
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    size_t n_c  = Mesh_GetNbOfConnectivities(mesh) ;
    int dim  = Mesh_GetDimension(mesh) ;
    Nodes_t* nodes = Nodes_New(n_no,dim,n_c) ;
    Elements_t* elements = Elements_New(n_el,n_c) ;
  
    Mesh_GetNodes(mesh) = nodes ;
    Mesh_GetElements(mesh) = elements ;
  }
  
  
  /* Re-start */
  rewind(fic_ces) ;


  /* Nodes */
  {
    fscanf(fic_ces,"%s",mot) ;
  
    if(strcmp(mot,"COOR") != 0) {
      arret("pas de COOR !\n") ;
    }
  
    /* Coordinates */
    {
      int dim  = Mesh_GetDimension(mesh) ;
      size_t n_no = Mesh_GetNbOfNodes(mesh) ;
      Node_t* no = Mesh_GetNode(mesh) ;
    
      fscanf(fic_ces,"%*d %*d %*d %*d") ;

      for(size_t i = 0 ; i < n_no ; i++) {
        Node_t* node_i = no + i ;
        double* x = Node_GetCoordinate(node_i) ;
        int j ;
        
        for(j = 0 ; j < dim ; j++) {
          fscanf(fic_ces,"%le",x + j) ;
        }
      }
    }
  }
  

  /* Elements */
  {
    int dim  = Mesh_GetDimension(mesh) ;
    size_t n_no = Mesh_GetNbOfNodes(mesh) ;
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    size_t n_c  = Mesh_GetNbOfConnectivities(mesh) ;
    Elements_t* elts = Mesh_GetElements(mesh) ;
    Regions_t* regions = Elements_GetRegions(elts) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    Node_t* no = Mesh_GetNode(mesh) ;
    Node_t** p_node = Element_GetPointerToNode(el) ;
    int* reg_i ;
    
    fscanf(fic_ces,"%s",mot) ;
  
    if(strcmp(mot,"ELEM") != 0) {
      arret("pas de ELEM !\n") ;
    }
    
    fscanf(fic_ces,"%*d %*d %*d %*d") ;

    n_c = 0 ;
    
    for(size_t i = 0 ; i < n_el ; i++) {
      size_t j = n_c ;
      unsigned short int nn ;
      
      Element_GetPointerToNode(el + i) = p_node + n_c ;
      
      fscanf(fic_ces,"%lu",&n_c) ;
      
      nn = n_c - j ;
      
      if(nn > Element_MaxNbOfNodes) {
        arret("trop de noeuds\n") ;
      }
      
      Element_GetElementIndex(el + i) = i ;
      Element_GetNbOfNodes(el + i) = nn ;
      Element_GetDimension(el + i) = dim_el[dim - 1][nn - 1] ;
    }
  
    /* Connectivity */
    for(size_t i = 0 ; i < n_el ; i++) {
      int nn = Element_GetNbOfNodes(el + i) ;
      int j ;
      
      for(j = 0 ; j < nn ; j++) {
        int nodeindex ;
        
        fscanf(fic_ces,"%d",&nodeindex) ;
        
        Element_GetNode(el + i,j) = no + nodeindex - 1 ;
      }
    }

    /* Regions */
    reg_i = (int*) Mry_New(int,n_el) ;
    
    for(size_t i = 0 ; i < n_el ; i++) {
      int    n_type = 4 ;
      char   nom_el[][5] = {"OBS2","OBS3","OBT3","OBQ4"} ;
      
      fscanf(fic_ces,"%s",mot) ;
      
      reg_i[i] = 0 ;
      
      for(int j = 0 ; j < n_type ; j++) {
        if(String_Is(mot,nom_el[j],4)) {
          reg_i[i] = j + 1 ;
        }
      }
      
      if(reg_i[i]  == 0) {
        arret("Mesh_ReadFormatCesar: pas de region") ;
      }
    }

    /* Material indexes */
    for(size_t i = 0 ; i < n_el ; i++) {
      int imat ;
      
      fscanf(fic_ces,"%d",&imat) ;
      
      imat -= 1 ;
      Element_GetMaterialIndex(el + i) = imat ;
      /* Region index is not defined in CESAR so we assume the following formula */
      Element_SetDefaultRegion(el + i,regions,10*reg_i[i] + imat) ;
    }
    
    free(reg_i) ;
  }
    
  {
    Nodes_t* nodes = Mesh_GetNodes(mesh) ;
      
    Nodes_InitializePointerToElements(nodes) ;
  }

  TextFile_Delete(textfile) ;
  free(textfile) ;
}





void (maillage)(double* pt,size_t* ne,double dx_ini,int npt,Node_t* no)
/* Calcul des coordonnees (no) d'un maillage 1D */
{
  int    npt1 = npt - 1 ;
  double dx = dx_ini ;

  Node_GetCoordinate(no)[0] = pt[0] ;
  
  for(int i = 0 ; i < npt1 ; i++) {
    double e ;
    
    if(ne[i] > 1) {
      e = log(fabs(pt[i + 1] - pt[i])/dx)/log((double) ne[i]) ;
    } else {
      e = 1. ;
    }
    
    for(size_t j = 1 ; j <= ne[i] ; j++) {
      double a = ((double) j)/((double) ne[i]) ;
      
      no += 1 ;
      
      Node_GetCoordinate(no)[0] = pt[i] + (pt[i + 1] - pt[i])*pow(a,e) ;
    }
    
    {
      double a = fabs(Node_GetCoordinate(no)[0] - Node_GetCoordinate(no - 1)[0]) ;
    
      if(a > 0.) dx = a ;
    }
  }
}



size_t (mesh1dnew)(double* point, double* l_c, int n_reg, Node_t* no)
/* Calcul des coordonnees (no) d'un maillage 1D
   et retourne le nombre d'elements */
{
#define MAX_NREG 100
  size_t    n_el ;
  size_t    ne[MAX_NREG] ;

  if(n_reg > MAX_NREG) {
    arret("mesh1d : trop de regions") ;
  }
#undef MAX_NREG

  {
    n_el = 0 ;
    
    for(int reg = 0 ; reg < n_reg ; reg++) {
      double l = fabs(point[reg + 1] - point[reg]) ;
      double b = l_c[reg + 1]/l_c[reg] ;
      double a = (l - l_c[reg])/(l - l_c[reg + 1]) ;
      size_t n ;

      if(l <= l_c[reg] || l <= l_c[reg + 1]) {
        n = 1 ;
      } else {
        if(a == 1.) n = (size_t) floor(l/l_c[reg] + 0.5) ;
        else n = (size_t) floor(log(b)/log(a) + 0.5) + 1 ;
      }

      ne[reg] = n ;

      n_el += n ;
    }
  }
  
  if(!no) return(n_el) ;


  {
    Node_t* p = no ;
    
    for(int reg = 0 ; reg < n_reg ; reg++) {
      double l = fabs(point[reg + 1] - point[reg]) ;
      double b = l_c[reg + 1]/l_c[reg] ;
      double a ;
      double l_i ;
      size_t n = ne[reg] ;

      /* a = (l - l_c[reg])/(l - l_c[reg+1]) ; */
      if(l_c[reg + 1] == l_c[reg]) a = 1. ;
      else if(n > 1) a = exp(log(b)/(n - 1)) ;
      else a = 0 ;
    
      /* l_i  = l_c[reg] ; */
      if(a == 1.) l_i = l/n ;
      else l_i = l*(1. - a)/(1. - a*b) ;

      Node_GetCoordinate(p)[0] = point[reg] ;
      Node_GetCoordinate(p + n)[0] = point[reg + 1] ;
      
      for(size_t i = 1 ; i < n ; i++) {
        Node_GetCoordinate(p + i)[0] = Node_GetCoordinate(p + i - 1)[0] + l_i ;
        l_i *= a ;
      }
      p   += n ;
    }
  }

  return(n_el) ;
}



int gmsh_ElmType(unsigned short int dim,unsigned short int nn)
/* N = i(Vertices) + j(Edges) + k(Faces) + l(Volume) */
{
  if(dim == 0) {
    switch (nn) {
    case 1 : return 15;             /* point */
    }
  } else if(dim == 1) {
    switch (nn) {
    case 2 : return 1;              /* line 1 */
    case 3 : return 8;              /* line 2 */
    }
  } else if(dim == 2) {
    switch (nn) {
    case 3 : return 2;              /* triangle 1 */
    case 6 : return 9;              /* triangle 2 */
    case 4 : return 3;              /* quadrangle 1 */
    case 9 : return 10;             /* quadrangle 2 */
    case 8 : return 16;             /* quadrangle 2 */
    }
  } else if(dim == 3) {
    switch (nn) {
    case 4 : return 4;              /* tetrahedron 1 */
    case 10: return 11;             /* tetrahedron 2 */
    case 8 : return 5;              /* hexahedron 1 */
    case 27: return 12;             /* hexahedron 2 */
    case 20: return 17;             /* hexahedron 2 */
    case 6 : return 6;              /* prism 1 */
    case 18: return 13;             /* prism 2 */
    case 15: return 18;             /* prism 2 */
    case 5 : return 7;              /* pyramid 1 */
    case 14: return 14;             /* pyramid 2 */
    case 13: return 19;             /* pyramid 2 */
    }
  }
  
  arret("gmsh_ElmType : dimension incorrect") ;
  return(0);
}



unsigned short int gmsh_NbNodes(int type)
/* N = i(Vertices) + j(Edges) + k(Faces) + l(Volume) */
{
  switch (type) {
  case 15: return 1;              /* point */
  case 1 : return 2;              /* line 1 */
  case 8 : return 2 + 1;          /* line 2 */
  case 2 : return 3;              /* triangle 1 */
  case 9 : return 3 + 3;          /* triangle 2 */
  case 3 : return 4;              /* quadrangle 1 */
  case 10: return 4 + 4 + 1;      /* quadrangle 2 */
  case 16: return 4 + 4;          /* quadrangle 2 */
  case 4 : return 4;              /* tetrahedron 1 */
  case 11: return 4 + 6;          /* tetrahedron 2 */
  case 5 : return 8;              /* hexahedron 1 */
  case 12: return 8 + 12 + 6 + 1; /* hexahedron 2 */
  case 17: return 8 + 12;         /* hexahedron 2 */
  case 6 : return 6;              /* prism 1 */
  case 13: return 6 + 9 + 3;      /* prism 2 */
  case 18: return 6 + 9;          /* prism 2 */
  case 7 : return 5;              /* pyramid 1 */
  case 14: return 5 + 8 + 1;      /* pyramid 2 */
  case 19: return 5 + 8;          /* pyramid 2 */
  }
  
  arret("gmsh_NbNodes");
  return(0);
}



unsigned short int gmsh_DimElement(int type)
{
  switch (type) {
  case 15: return 0;              /* point */
  case 1 : return 1;              /* line 1 */
  case 8 : return 1;              /* line 2 */
  case 2 : return 2;              /* triangle 1 */
  case 9 : return 2;              /* triangle 2 */
  case 3 : return 2;              /* quadrangle 1 */
  case 10: return 2;              /* quadrangle 2 */
  case 16: return 2;              /* quadrangle 2 */
  case 4 : return 3;              /* tetrahedron 1 */
  case 11: return 3;              /* tetrahedron 2 */
  case 5 : return 3;              /* hexahedron 1 */
  case 12: return 3;              /* hexahedron 2 */
  case 17: return 3;              /* hexahedron 2 */
  case 6 : return 3;              /* prism 1 */
  case 13: return 3;              /* prism 2 */
  case 18: return 3;              /* prism 2 */
  case 7 : return 3;              /* pyramid 1 */
  case 14: return 3;              /* pyramid 2 */
  case 19: return 3;              /* pyramid 2 */
  }
  
  arret("gmsh_DimElement");
  return(0);
}




/* Not used anymore */


#if 0
void ecrit_mail_msh_1(Mesh_t* mesh,const char* nom)
/* fichier de maillage au format GMSH "nom.msh" */
{
  int    i ;
  char   nom_msh[Mesh_MaxLengthOfFileName] ;
  FILE*  fic_msh ;
  
  sprintf(nom_msh,"%s.msh",nom) ;
  fic_msh = fopen(nom_msh,"w") ;
  if(!fic_msh) {
    arret("erreur a l'ouverture du fichier %s",nom_msh) ;
  }

  /* Les noeuds */
  fprintf(fic_msh,"$NOD\n") ;
  /* nombre de noeuds */
  fprintf(fic_msh,"%d\n",N_NO) ;
  for(i=0;i<(int) N_NO;i++) {
    int    j ;
    /* le numero du noeud */
    fprintf(fic_msh,"%d",i+1) ;
    /* les coordonnees*/
    for(j=0;j<(int) DIM;j++) fprintf(fic_msh," %e",Node_GetCoordinate(NO + i)[j]) ;
    for(j=(int) DIM;j<3;j++) fprintf(fic_msh," 0") ;
    fprintf (fic_msh,"\n") ;
  }
  fprintf(fic_msh,"$ENDNOD\n") ;

  /* Les elements */
  fprintf(fic_msh,"$ELM\n") ;
  /* nombre d'elements lus */
  fprintf(fic_msh,"%d\n",N_EL) ;
  for(i=0;i<(int) N_EL;i++) {
    int    nn = Element_GetNbOfNodes(EL + i) ;
    int    imat = Element_GetMaterialIndex(EL + i) ;
    char*   reg = Element_GetRegionName(EL + i) ;
    int    dim_el = Element_GetDimension(EL + i) ;
    int    j ;
    /* le numero d'element */
    fprintf(fic_msh,"%d",i+1) ;
    /* groupe et nb de noeuds par elements */
    fprintf(fic_msh," %d %d %s %d",gmsh_ElmType(dim_el,nn),imat+1,reg,nn) ;
    /* numerotation */
    for(j=0;j<nn;j++) {
      Node_t* node_j = Element_GetNode(EL + i,j) ;
      fprintf(fic_msh," %lu",Node_GetNodeIndex(node_j) + 1) ;
    }
    fprintf(fic_msh,"\n") ;
  }
  fprintf(fic_msh,"$ENDELM\n") ;

  /* fermeture du ficher */
  fclose(fic_msh) ;
}
#endif



#if 0
void ecrit_mail_msh_2(Mesh_t* mesh,const char* nom)
/* fichier de maillage au format GMSH "nom.msh" */
{
  int    i ;
  char   nom_msh[Mesh_MaxLengthOfFileName] ;
  FILE*  fic_msh ;
  
  /* Ouverture de fichier */
  sprintf(nom_msh,"%s.msh",nom) ;
  fic_msh = fopen(nom_msh,"w") ;
  if(!fic_msh) {
    arret("erreur a l'ouverture du fichier %s",nom_msh) ;
  }

  /* Le format */
  fprintf(fic_msh,"$MeshFormat\n") ;
  fprintf(fic_msh,"2 0 %lu\n",sizeof(double)) ;
  fprintf(fic_msh,"$EndMeshFormat\n") ;

  /* Les noeuds */
  fprintf(fic_msh,"$Nodes\n") ;
  /* nombre de noeuds */
  fprintf(fic_msh,"%d\n",N_NO) ;
  for(i=0;i<(int) N_NO;i++) {
    int    j ;
    double* x = Node_GetCoordinate(NO + i) ;
    /* le numero du noeud */
    fprintf(fic_msh,"%d",i+1) ;
    /* les coordonnees*/
    for(j=0;j<(int) DIM;j++) fprintf(fic_msh," %e",x[j]) ;
    for(j=(int) DIM;j<3;j++) fprintf(fic_msh," 0") ;
    fprintf (fic_msh,"\n") ;
  }
  fprintf(fic_msh,"$EndNodes\n") ;

  /* Les elements */
  fprintf(fic_msh,"$Elements\n") ;
  /* nombre d'elements lus */
  fprintf(fic_msh,"%d\n",N_EL) ;
  for(i=0;i<(int) N_EL;i++) {
    int    nn = Element_GetNbOfNodes(EL + i) ;
    int    imat = Element_GetMaterialIndex(EL + i) ;
    char*   reg = Element_GetRegionName(EL + i) ;
    int    dim_el = Element_GetDimension(EL + i) ;
    int    j ;
    if(nn > 8) arret("ecrit_mail_msh_2 : trop de noeud") ;
    /* le numero d'element */
    fprintf(fic_msh,"%d",i+1) ;
    /* le type */
    fprintf(fic_msh," %d",gmsh_ElmType(dim_el,nn)) ;
    /* 3 tags */
    fprintf(fic_msh," 3") ;
    /* physical, elementary, partition */
    fprintf(fic_msh," %d %s 0",imat+1,reg) ;
    /* numerotation */
    for(j=0;j<nn;j++) {
      Node_t* node_j = Element_GetNode(EL + i,j) ;
      fprintf(fic_msh," %lu",Node_GetNodeIndex(node_j) + 1) ;
    }
    fprintf(fic_msh,"\n") ;
  }
  fprintf(fic_msh,"$EndElements\n") ;

  /* fermeture du ficher */
  fclose(fic_msh) ;
}
#endif




#define PRINT(...) \
        fprintf(stdout,__VA_ARGS__)
        //Message_Direct(__VA_ARGS__)


void Mesh_PrintData(Mesh_t* mesh,char* mot)
{
  static int i_debug=0 ;
  
  if(String_Is(mot,"\0")) return ;

  PRINT("\n") ;
  PRINT("debug(%d)\n",i_debug++) ;
  PRINT("-----\n") ;
  

  /* Geometry
   * -------- */
  if(Mesh_GetGeometry(mesh) && (String_Is(mot,"geometry",4) || String_Is(mot,"all",3))) {
    int dim = Mesh_GetDimension(mesh) ;
    Symmetry_t sym = Mesh_GetSymmetry(mesh) ;
    
    PRINT("\n") ;
    PRINT("Geometry:\n") ;
    
    PRINT("\t Dimension = %dD\n",dim) ;
    PRINT("\t Symmetry = ") ;
    
    if(0) {
      
    } else if(Symmetry_IsCylindrical(sym)) {
      PRINT("Axisymmetrical\n") ;
      
    } else if(Symmetry_IsSpherical(sym)) {
      PRINT("Spherical\n") ;

    } else if(Symmetry_IsPlane(sym)) {
      PRINT("Plane\n") ;

    } else {
      PRINT("No symmetry\n") ;
    }
  }

  /* Mesh
   * ---- */
  if(mesh && (String_Is(mot,"mesh",4) || String_Is(mot,"all",3))) {
    int dim = Mesh_GetDimension(mesh) ;
    size_t n_no = Mesh_GetNbOfNodes(mesh) ;
    Node_t* no = Mesh_GetNode(mesh) ;
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    int c1 = 14 ;
    int c2 = 30 ;
    int c3 = 45 ;
    
    PRINT("\n") ;
    PRINT("Mesh:\n") ;
    
    PRINT("\t Nodes:\n") ;
    PRINT("\t Nb of nodes = %lu\n",n_no) ;
    
    for(size_t i = 0 ; i < n_no ; i++) {
      Node_t* node_i = no + i ;
      int ne = Node_GetNbOfElements(node_i) ;
      int n = PRINT("\t no(%lu)",i) ;
      int j ;
      
      while(n < c1) n += PRINT(" ") ;
      
      n += PRINT(":") ;
      
      for(j = 0 ; j < dim ; j++) {
        n += PRINT(" % e",Node_GetCoordinate(node_i)[j]) ;
      }
      
      while(n < c3) n += PRINT(" ") ;
      
      if(ne) n += PRINT("  el(") ;
      
      for(j = 0 ; j < ne ; j++) {
        n += PRINT("%lu",Element_GetElementIndex(Node_GetElement(node_i,j))) ;
        n += PRINT(((j < ne - 1) ? "," : ")")) ;
      }
      
      PRINT("\n") ;
    }
    
    PRINT("\n") ;
    PRINT("\t Elements:\n") ;
    PRINT("\t Nb of elements = %lu\n",n_el) ;
    
    for(size_t i = 0 ; i < n_el ; i++) {
      Element_t* elt_i = el + i ;
      int nn = Element_GetNbOfNodes(elt_i) ;
      int n = PRINT("\t el(%lu)",i) ;
      int j ;
      
      while(n < c1) n += PRINT(" ") ;
      
      n += PRINT(":") ;
      
      n += PRINT("  reg(%s)",Region_GetRegionName(Element_GetRegion(elt_i))) ;
      
      while(n < c2) n += PRINT(" ") ;
      
      n += PRINT("  mat(%d)",Element_GetMaterialIndex(elt_i)) ;
      
      while(n < c3) n += PRINT(" ") ;
      
      n += PRINT("  no(") ;
      
      for(j = 0 ; j < nn ; j++) {
        n += PRINT("%lu",Node_GetNodeIndex(Element_GetNode(elt_i,j))) ;
        n += PRINT(((j < nn - 1) ? "," : ")")) ;
      }
      
      
      
      PRINT("\n") ;
    }
  }
  

  /* Continuity
   * ---------- */
  if(mesh && (String_Is(mot,"continuity",3))) {
    size_t n_no = Mesh_GetNbOfNodes(mesh) ;
    Node_t* no = Mesh_GetNode(mesh) ;
    size_t n_el = Mesh_GetNbOfElements(mesh) ;
    Element_t* el = Mesh_GetElement(mesh) ;
    
    PRINT("\n") ;
    PRINT("Continuity:\n") ;
    
    PRINT("\t Positions of unknowns and equations at nodes of elements\n") ;
    
    for(size_t i = 0 ; i < n_el ; i++) {
      Element_t* elt_i = el + i ;
      int nn = Element_GetNbOfNodes(elt_i) ;
      int neq = Element_GetNbOfEquations(elt_i) ;
      char** name_unk = Element_GetNameOfUnknown(elt_i) ;
      char** name_eqn = Element_GetNameOfEquation(elt_i) ;
      int j ;
      
      PRINT("\t el(%lu): %d nodes\n",i,nn) ;
      
      PRINT("\t    %d unknowns\n",neq) ;
      
      for(j = 0 ; j < nn ; j++) {
        int k ;
        
        PRINT("\t    no(%d):",j) ;
        
        for(k = 0 ; k < neq ; k++) {
          PRINT(" %s(%d)",name_unk[k],Element_GetUnknownPosition(elt_i)[j*neq + k]) ;
        }
        
        PRINT("\n") ;
      }
      
      PRINT("\t    %d equations\n",neq) ;
      
      for(j = 0 ; j < nn ; j++) {
        int k ;
        
        PRINT("\t    no(%d):",j) ;
        
        for(k = 0 ; k < neq ; k++) {
          PRINT(" %s(%d)",name_eqn[k],Element_GetEquationPosition(elt_i)[j*neq + k]) ;
        }
        
        PRINT("\n") ;
      }
    }
    
    PRINT("\n") ;
    PRINT("\t Equations and unknowns at nodes:\n") ;
    
    for(size_t i = 0 ; i < n_no ; i++) {
      Node_t* node_i = no + i ;
      int nb_unk = Node_GetNbOfUnknowns(node_i) ;
      int nb_eqn = Node_GetNbOfEquations(node_i) ;
      int j ;
      
      PRINT("\t no(%lu):\n",i) ;
      PRINT("\t    %d unknowns:",nb_unk) ;
      
      for(j = 0 ; j < nb_unk ; j++) {
        char* name = Node_GetNameOfUnknown(node_i)[j] ;
        
        PRINT(" %s",name) ;
      }
      
      PRINT("\n") ;
      PRINT("\t    %d equations:",nb_eqn) ;
      
      for(j = 0 ; j < nb_eqn ; j++) {
        char* name = Node_GetNameOfEquation(node_i)[j] ;
        
        PRINT(" %s",name) ;
      }
      PRINT("\n") ;
    }
  }

  /* Matrix numbering
   * ---------------- */
  if(mesh && String_Is(mot,"numbering",3)) {
    size_t n_no = Mesh_GetNbOfNodes(mesh) ;
    Node_t* no = Mesh_GetNode(mesh) ;
    
    PRINT("\n") ;
    PRINT("Matrix numbering:\n") ;
    
    PRINT("\n") ;
    PRINT("\t Matrix indexes of equations and unknowns at nodes:\n") ;
    
    for(size_t i = 0 ; i < n_no ; i++) {
      Node_t* node_i = no + i ;
      int nb_unk = Node_GetNbOfUnknowns(node_i) ;
      int nb_eqn = Node_GetNbOfEquations(node_i) ;
      int j ;
      
      PRINT("\t node(%lu):\n",i) ;
      PRINT("\t    %d unknowns(col):",nb_unk) ;
      
      for(j = 0 ; j < nb_unk ; j++) {
        char* name = Node_GetNameOfUnknown(node_i)[j] ;
        int icol = Node_GetMatrixColumnIndex(node_i)[j] ;
        
        PRINT(" %s(%d)",name,icol) ;
      }
      
      PRINT("\n") ;
      PRINT("\t    %d equations(row):",nb_eqn) ;
      
      for(j = 0 ; j < nb_eqn ; j++) {
        char* name = Node_GetNameOfEquation(node_i)[j] ;
        int irow = Node_GetMatrixRowIndex(node_i)[j] ;
        
        PRINT(" %s(%d)",name,irow) ;
      }
      PRINT("\n") ;
    }
  }


  /* Interpolation functions
   * ----------------------- */
  if(mesh && (String_Is(mot,"interpolation",4))) {
    Elements_t* elts = Mesh_GetElements(mesh) ;
    IntFcts_t* intfcts = Elements_GetIntFcts(elts) ;
    int n_fi = IntFcts_GetNbOfIntFcts(intfcts) ;
    IntFct_t* intfct = IntFcts_GetIntFct(intfcts) ;
    int i ;
    
    PRINT("\n") ;
    PRINT("Interpolation:\n") ;
    
    PRINT("\t Nb of interpolation functions = %d\n",n_fi) ;
    
    for(i = 0 ; i < n_fi ; i++) {
      int np = IntFct_GetNbOfPoints(intfct + i) ;
      int nn = IntFct_GetNbOfFunctions(intfct + i) ;
      int dim = IntFct_GetDimension(intfct + i) ;
      
      PRINT("\n") ;
      
      PRINT("\t Interpolation function %d\n",i) ;
      
      PRINT("\t Nb of integration points = %d",np) ;
      
      PRINT(", Dimension = %d\n",dim) ;
      
      if(np <= 0) continue ;
      
      
      PRINT("\t Point Coordinates:\n") ;
      
      {
        char axis[3] = {'x','y','z'} ;
        int k ;
        
        for(k = 0 ; k < dim ; k++) {
          int p ;
      
          PRINT("\t %c = ",axis[k]) ;
        
          for(p = 0 ; p < np ; p++) {
            double* ap = IntFct_GetCoordinatesAtPoint(intfct + i,p) ;
            
            PRINT("% e ",ap[k]) ;
          }
        
          PRINT("\n") ;
        }
      }
      
      
      PRINT("\t Weights = ") ;
      
      {
        double* w = IntFct_GetWeight(intfct + i) ;
        int p ;
        
        for(p = 0 ; p < np ; p++) {
          PRINT("%e ",w[p]) ;
        }
        
        PRINT("\n") ;
      }
      
      
      PRINT("\t Nb of functions = %d\n",nn) ;
      
      
      PRINT("\t Functions:\n") ;
      
      {
        int k ;
        
        for(k = 0 ; k < nn ; k++) {
          int p ;
          
          if(k == 0) {
            
            PRINT("\t hi = ") ;
            
            for(p = 0 ; p < np ; p++) {
              PRINT(" hi(pt %d)     ",p) ;
            }
            
            PRINT("\n") ;
          }
        
          PRINT("\t h%d = ",k) ;
        
          for(p = 0 ; p < np ; p++) {
            double* hp = IntFct_GetFunctionAtPoint(intfct + i,p) ;
            
            PRINT("% e ",hp[k]) ;
          }
        
          PRINT("\n") ;
        }
      }
      
      
      
      PRINT("\t Function derivatives:\n") ;
      
      
#define DHP(n,i)  (dhp[(n)*dim+(i)])
      {
        int l ;
        
        for(l = 0 ; l < nn ; l++) {
          int k ;
          char axis[3] = {'x','y','z'} ;
          
          if(l == 0) {
            int p ;
            
            PRINT("\t hi,j = ") ;
            
            for(p = 0 ; p < np ; p++) {
              PRINT(" hi,j(pt %d)   ",p) ;
            }
            
            PRINT("\n") ;
          }
        
          for(k = 0 ; k < dim ; k++) {
            int p ;
        
            PRINT("\t h%d,%c = ",l,axis[k]) ;
          
            for(p = 0 ; p < np ; p++) {
              double* dhp = IntFct_GetFunctionGradientAtPoint(intfct + i,p) ;
              
              PRINT("% e ",DHP(l,k)) ;
            }
        
            PRINT("\n") ;
          }
        }
      }
#undef DHJ
    }
  }

  fflush(stdout) ;
}





void (Mesh_AssembleElementaryMatrices)(Mesh_t* mesh,Matrix_t* a)
/** Assembling the elementary matrices */
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  
  
  /* Assembling the elementary matrices */
  {    
    #if SharedMS_APIis(OpenMP)
      #pragma omp parallel for
    #endif
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        size_t len = Matrix_AssembleElementMatrix(a,el+ie,NULL) ;
      
        if(len > 0) {
          double* ke = Element_GetMatrix(el + ie) ;
          
          #if SharedMS_APIis(OpenMP)
            #pragma omp critical
          #endif
          Matrix_AssembleElementMatrix(a,el+ie,ke) ;
        }
      }
    }
  }
  
  return ;
}



void (Mesh_AssembleElementaryResidus)(Mesh_t* mesh,Residu_t* r)
/** Assembling the elementary residus */
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;

  {  
    #if SharedMS_APIis(OpenMP)
      #pragma omp parallel for
    #endif
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        int len = Residu_AssembleElementResidu(r,el + ie,NULL) ;
      
        if(len > 0) {
          double* re = Element_GetResidu(el + ie) ;
          
          #if SharedMS_APIis(OpenMP)
            #pragma omp critical
          #endif
          Residu_AssembleElementResidu(r,el + ie,re) ;
        }
      }
    }
  }
  
   return ;
}



void (Mesh_BroadcastElementaryMatrices)(Mesh_t* mesh)
/** Broadcast elementary matrices to other processors */
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int size = DistributedMS_NbOfProcessors ;
  
  if(size > 1) {    
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        int ranksender = Element_RankOfSupportingProcessor(el + ie) ;
        
        {
          double* ke = Element_GetMatrix(el+ie) ;
          int ndof = Element_GetNbOfDOF(el+ie) ;
          int n = ndof*ndof ;
          
          #if DistributedMS_APIis(MPI)
            MPI_Bcast(ke,n,MPI_DOUBLE,ranksender,MPI_COMM_WORLD) ;
          #else
            Message_FatalError("Distributed MS not available") ;
          #endif
        }
      }
    }
  }
  
  return ;
}


void (Mesh_BroadcastElementaryResidus)(Mesh_t* mesh)
/** Broadcast elementary residus to other processors */
{
  size_t n_el = Mesh_GetNbOfElements(mesh) ;
  Element_t* el = Mesh_GetElement(mesh) ;
  int size = DistributedMS_NbOfProcessors ;
  
  if(size > 1) {    
    for(size_t ie = 0 ; ie < n_el ; ie++) {
      Material_t* mat = Element_GetMaterial(el + ie) ;
    
      if(mat) {
        int ranksender = Element_RankOfSupportingProcessor(el + ie) ;
        
        {
          double* re = Element_GetResidu(el+ie) ;
          int ndof = Element_GetNbOfDOF(el+ie) ;
          
          #if DistributedMS_APIis(MPI)
            MPI_Bcast(re,ndof,MPI_DOUBLE,ranksender,MPI_COMM_WORLD) ;
          #else
            Message_FatalError("Distributed MS not available") ;
          #endif
        }
      }
    }
  }
  
  return ;
}
