#ifndef MESH_H
#define MESH_H

#ifdef __CPLUSPLUS
extern "C" {
#endif



/* Forward declarations */
struct Mesh_t; //typedef struct Mesh_t         Mesh_t ;
struct Materials_t;
struct Geometry_t;
struct BConds_t;
struct DataFile_t;
struct Solver_t;
struct Matrix_t;
struct Residu_t;
struct Loads_t;
struct Elements_t;
struct Nodes_t;

#include <string>

extern Mesh_t*  (Mesh_New)(Geometry_t*,DataFile_t* = nullptr) ;
extern Mesh_t*  (Mesh_Create)(DataFile_t*,Geometry_t*,Materials_t* = nullptr) ;
extern void     (Mesh_Scan)(Mesh_t*,DataFile_t*) ;
extern void     (Mesh_CreateMore)(Mesh_t*,Materials_t*) ;
extern void     (Mesh_Delete)(void*) ;
extern void     (Mesh_Set)(Mesh_t*,std::string const&) ;
extern void     (Mesh_Set)(Mesh_t*,char const*) ;
extern void     (Mesh_SetNodeConnectivities)(Mesh_t*) ;
extern void     (Mesh_SetMatrixRowColumnIndexes)(Mesh_t*,BConds_t*) ;
extern void     (Mesh_UpdateMatrixRowColumnIndexes)(Mesh_t*) ;
extern void     (Mesh_InitializeMatrixRowColumnIndexes)(Mesh_t*) ;
extern void     (Mesh_WriteGraph)(Mesh_t*,const char*,const char*) ;
extern void     (Mesh_WriteInversePermutation)(Mesh_t*,const char*,const char*) ;
extern int      (Mesh_LoadCurrentSolution)(Mesh_t*,DataFile_t*,double*) ;
extern int      (Mesh_StoreCurrentSolution)(Mesh_t*,DataFile_t*,double) ;
extern void     (Mesh_SetCurrentUnknownsWithBoundaryConditions)(Mesh_t*,BConds_t*,double) ;
extern void     (Mesh_UpdateCurrentUnknowns)(Mesh_t*,Solver_t*) ;
extern void     (Mesh_SetEquationContinuity)(Mesh_t*) ;
extern void     (Mesh_PrintData)(Mesh_t*,char*) ;

extern int      (Mesh_ComputeInitialState)(Mesh_t*,double) ;
extern int      (Mesh_ComputeExplicitTerms)(Mesh_t*,double) ;
extern int      (Mesh_ComputeMatrix)(Mesh_t*,Matrix_t*,double,double) ;
extern void     (Mesh_ComputeResidu)(Mesh_t*,Residu_t*,Loads_t*,double,double) ;
extern int      (Mesh_ComputeImplicitTerms)(Mesh_t*,double,double) ;
extern int*     (Mesh_ComputeNbOfMatrixNonzerosPerRowAndColumn)(Mesh_t*,const int) ;
extern int*     (Mesh_ComputeNbOfSubmatrixNonzerosPerRow)(Mesh_t*,const int,const int,const int) ;
extern void     (Mesh_BroadcastConstantTerms)(Mesh_t*) ;
extern void     (Mesh_BroadcastExplicitTerms)(Mesh_t*) ;
extern void     (Mesh_BroadcastImplicitTerms)(Mesh_t*) ;



/* Some constants */
#define Mesh_MaxLengthOfKeyWord        (30)
#define Mesh_MaxLengthOfFileName       (60)
#define Mesh_MaxLengthOfTextLine       (500)



/* Accessors */
#define Mesh_GetDataFile(MSH)               ((MSH)->GetDataFile())
#define Mesh_GetGeometry(MSH)               ((MSH)->GetGeometry())
#define Mesh_GetNodes(MSH)                  ((MSH)->GetNodes())
#define Mesh_GetElements(MSH)               ((MSH)->GetElements())

#define Mesh_SetDataFile(MSH,A)             ((MSH)->SetDataFile(A))
#define Mesh_SetGeometry(MSH,A)             ((MSH)->SetGeometry(A))
#define Mesh_SetNodes(MSH,A)                ((MSH)->SetNodes(A))
#define Mesh_SetElements(MSH,A)             ((MSH)->SetElements(A))



/* Access to the geometry characteristics */
#define Mesh_GetDimension(MSH) \
        Geometry_GetDimension(Mesh_GetGeometry(MSH))

#define Mesh_GetSymmetry(MSH) \
        Geometry_GetSymmetry(Mesh_GetGeometry(MSH))

#define Mesh_GetCoordinateSystem(MSH) \
        Geometry_GetCoordinateSystem(Mesh_GetGeometry(MSH))



/* Access to Nodes */
#define Mesh_GetNbOfNodes(MSH) \
        Nodes_GetNbOfNodes(Mesh_GetNodes(MSH))

#define Mesh_GetNode(MSH) \
        Nodes_GetNode(Mesh_GetNodes(MSH))



/* Access to the elements */
#define Mesh_GetNbOfElements(MSH) \
        Elements_GetNbOfElements(Mesh_GetElements(MSH))

#define Mesh_GetElement(MSH) \
        Elements_GetElement(Mesh_GetElements(MSH))

#define Mesh_GetNbOfConnectivities(MSH) \
        Elements_GetNbOfConnectivities(Mesh_GetElements(MSH))



/* Access to the nb of matrix rows/columns */
#define Mesh_GetNbOfMatrices(MSH) \
        Nodes_GetNbOfMatrices(Mesh_GetNodes(MSH))

#define Mesh_GetNbOfMatrixColumns(MSH) \
        Nodes_GetNbOfMatrixColumns(Mesh_GetNodes(MSH))

#define Mesh_GetNbOfMatrixRows(MSH) \
        Nodes_GetNbOfMatrixRows(Mesh_GetNodes(MSH))



/* Compute the nb of matrix entries */
#define Mesh_ComputeNbOfMatrixEntries(MSH) \
        Elements_ComputeNbOfMatrixEntries(Mesh_GetElements(MSH))

#define Mesh_ComputeNbOfSelectedMatrixEntries(MSH,IND) \
        Elements_ComputeNbOfSelectedMatrixEntries(Mesh_GetElements(MSH),IND)




/* Periodicities */
#define Mesh_GetPeriodicities(MSH) \
        Geometry_GetPeriodicities(Mesh_GetGeometry(MSH))

#define Mesh_IsPeriodic(MSH) \
        Mesh_GetPeriodicities(MSH)

        
 #include <stdexcept>


struct Mesh_t {
  private:
  DataFile_t* _datafile ;
  Geometry_t* _geometry ;
  Elements_t* _elements ;
  Nodes_t*    _nodes ;

  public:
  DataFile_t* GetDataFile(){return _datafile;}
  Geometry_t* GetGeometry(){return _geometry;}
  Elements_t* GetElements(){return _elements;}
  Nodes_t*    GetNodes(){return _nodes;}

  void SetDataFile(DataFile_t* a){_datafile = a;}
  void SetGeometry(Geometry_t* a){_geometry = a;}
  void SetElements(Elements_t* a){_elements = a;}
  void SetNodes(Nodes_t* a){_nodes = a;}
} ;


#ifdef __CPLUSPLUS
}
#endif


#include "Geometry.h"
#include "Nodes.h"
#include "Elements.h"
#endif
