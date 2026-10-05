#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Help.h"
#include "Message.h"
#include "DataFile.h"
#include "Geometry.h"
#include "Fields.h"
#include "Field.h"
#include "Functions.h"
#include "Function.h"
#include "Dates.h"
#include "Date.h"
#include "Points.h"
#include "Point.h"
#include "ObVals.h"
#include "ObVal.h"
#include "TimeStep.h"
#include "IterProcess.h"
#include "Options.h"
#include "Module.h"
#include "Materials.h"
#include "Models.h"
#include "Mesh.h"
#include "IConds.h"
#include "ICond.h"
#include "Loads.h"
#include "Load.h"
#include "BConds.h"
#include "BCond.h"
#include "DataSet.h"
#include "IntFcts.h"
#include "Units.h"
#include "String_.h"
#include "Curve.h"
//#include "Parser.h"



/* Extern functions */
DataSet_t*  (DataSet_New)(std::string const& filestr,Context_t* ctx){
  return(DataSet_New(filestr.c_str(),ctx));
}

DataSet_t*  (DataSet_New)(char const* filename,Context_t* ctx)
{
  DataSet_t* dataset = (DataSet_t*) Mry_New(DataSet_t) ;
  Options_t*     options = Options_New(ctx);
  DataFile_t*    datafile = DataFile_New(filename) ;
  Units_t*       units = Units_New() ;
  Geometry_t*    geometry = Geometry_New() ;
  Models_t*      models = Models_New(geometry,datafile) ;
  Functions_t*   functions = Functions_New() ;
  Fields_t*      fields = Fields_New() ;
  Materials_t*   materials = Materials_New(models,fields,functions) ;
  Dates_t*       dates = Dates_New() ;
  Points_t*      points = Points_New() ;
  IConds_t*      iconds = IConds_New(fields,functions) ;
  BConds_t*      bconds = BConds_New(fields,functions) ;
  Loads_t*       loads = Loads_New(fields,functions) ;
  ObVals_t*      obvals = ObVals_New() ;
  TimeStep_t*    timestep = TimeStep_New(obvals,datafile) ;
  IterProcess_t* iterprocess = IterProcess_New(obvals) ;
  Module_t*      module = Module_New() ;
  Mesh_t*        mesh = Mesh_New(geometry,datafile);

  DataSet_SetOptions(dataset,options) ;
  DataSet_SetDataFile(dataset,datafile) ;
  DataSet_SetUnits(dataset,units) ;
  DataSet_SetGeometry(dataset,geometry) ;
  DataSet_SetFields(dataset,fields) ;
  DataSet_SetFunctions(dataset,functions) ;
  DataSet_SetModels(dataset,models) ;
  DataSet_SetMaterials(dataset,materials) ;
  DataSet_SetMesh(dataset,mesh) ;
  DataSet_SetIConds(dataset,iconds) ;
  DataSet_SetLoads(dataset,loads) ;
  DataSet_SetBConds(dataset,bconds) ;
  DataSet_SetPoints(dataset,points) ;
  DataSet_SetDates(dataset,dates) ;
  DataSet_SetObVals(dataset,obvals) ;
  DataSet_SetTimeStep(dataset,timestep) ;
  DataSet_SetIterProcess(dataset,iterprocess) ;
  DataSet_SetModule(dataset,module) ;

  DataFile_SetParent(datafile,dataset) ;

  if(options) {
    char* codename = Options_GetModule(options) ;

    if(codename) Module_Set(module,codename) ;
    Module_SetNbOfSequences(module,Options_GetNbOfSequences(options)) ;
  }

  return(dataset);
}



DataSet_t*  (DataSet_Create)(char const* filename,Context_t* ctx)
{
  DataSet_t*  dataset = DataSet_New(filename,ctx) ;
  DataFile_t* datafile = DataSet_GetDataFile(dataset) ;
  Options_t* options = DataSet_GetOptions(dataset);
  char*   debug  = Options_GetPrintedInfos(options) ;

  DataSet_Scan(dataset,datafile);
  DataSet_Finalize(dataset);
  
  /* Other printings in debug mode */
  if(!strcmp(debug,"continuity")) DataSet_PrintData(dataset,debug) ;
  //if(!strcmp(debug,"numbering")) DataSet_PrintData(dataset,debug) ;
  //if(!strcmp(debug,"inter")) DataSet_PrintData(dataset,debug) ;
  if(!strcmp(debug,"all")) DataSet_PrintData(dataset,debug) ;
  
  return(dataset) ;
}


void  (DataSet_Scan)(DataSet_t* dataset,DataFile_t* datafile)
{
  char* filename = DataFile_GetFileName(datafile);
  Options_t* opt = DataSet_GetOptions(dataset);
  char* debug  = Options_GetPrintedInfos(opt) ;

  /* DataFile */
  {
    if(DataFile_DoesNotExist(datafile)) {
      Message_Info("File %s not found\n",filename) ;
      Message_Exit ;
    }

    DataFile_RemoveComments(datafile) ;
  }
  if(!strcmp(debug,"data")) DataSet_PrintData(dataset,debug) ;

  Message_Direct("Reading %s\n",filename) ;


  /* Units */
  {
    Units_t* units = DataSet_GetUnits(dataset) ;

    Units_Scan(units,datafile);
  }
  
  
  /* Geometry */
  {
    Geometry_t* geometry = DataSet_GetGeometry(dataset) ;

    Geometry_Scan(geometry,datafile);
  }
  if(!strcmp(debug,"geom")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Fields */
  {
    Fields_t* fields = DataSet_GetFields(dataset) ;
  
    Fields_Scan(fields,datafile);
  }
  if(!strcmp(debug,"field")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Functions */
  {
    Functions_t* functions = DataSet_GetFunctions(dataset) ;
  
    Functions_Scan(functions,datafile);
  }
  if(!strcmp(debug,"func")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Models */
  {
    Models_t* models   = DataSet_GetModels(dataset) ;

    Models_Scan(models,datafile);
  }
  if(!strcmp(debug,"model")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Materials */
  {
    Materials_t* materials = DataSet_GetMaterials(dataset) ;
  
    Materials_Scan(materials,datafile);
  }
  if(!strcmp(debug,"mate")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Mesh */
  {
    Mesh_t*      mesh = DataSet_GetMesh(dataset) ;
    Materials_t* materials = DataSet_GetMaterials(dataset) ;
  
    Mesh_Scan(mesh,datafile);
  }
  if(!strcmp(debug,"mesh")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Initial conditions */
  {
    IConds_t* iconds = DataSet_GetIConds(dataset) ;
  
    IConds_Scan(iconds,datafile);
  }
  if(!strcmp(debug,"init")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Loads */
  {
    Loads_t* loads = DataSet_GetLoads(dataset) ;
  
    Loads_Scan(loads,datafile);
  }
  if(!strcmp(debug,"load")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Boundary conditions */
  {
    BConds_t* bconds = DataSet_GetBConds(dataset) ;
  
    BConds_Scan(bconds,datafile);
  }
  if(!strcmp(debug,"bcond")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Points */
  {
    Points_t* points = DataSet_GetPoints(dataset) ;
    Mesh_t*   mesh = DataSet_GetMesh(dataset) ;
  
    Points_Scan(points,datafile,mesh);
  }
  if(!strcmp(debug,"points")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Dates */
  {
    Dates_t* dates = DataSet_GetDates(dataset) ;
    
    Dates_Scan(dates,datafile);
  }
  if(!strcmp(debug,"dates")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Objective variations */
  {
    ObVals_t* obvals = DataSet_GetObVals(dataset) ;
  
    ObVals_Scan(obvals,datafile);
  }
  if(!strcmp(debug,"obval")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Time steps */
  {
    TimeStep_t*    timestep = DataSet_GetTimeStep(dataset) ;
    
    TimeStep_Scan(timestep,datafile);
  }
  if(!strcmp(debug,"time")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Iterative process */
  {
    IterProcess_t* iterprocess = DataSet_GetIterProcess(dataset) ;
    
    IterProcess_Scan(iterprocess,datafile);
  }
  if(!strcmp(debug,"iter")) DataSet_PrintData(dataset,debug) ;
  
  
  /* Module */
  if(!strcmp(debug,"module")) DataSet_PrintData(dataset,debug) ;

  Message_Direct("End of reading %s\n",filename) ;
  Message_Direct("\n") ;
  
  return ;
}



void (DataSet_Delete)(void* self)
{
  DataSet_t* dataset = (DataSet_t*) self ;
  
  {
    Options_t* options = DataSet_GetOptions(dataset);

    if(options) {
      Options_Delete(options);
      Mry_Free(options);
      DataSet_SetOptions(dataset,NULL);
    }
  }
  
  {
    Units_t* units = DataSet_GetUnits(dataset) ;
    
    if(units) {
      Units_Delete(units) ;
      Mry_Free(units) ;
      DataSet_SetUnits(dataset,NULL) ;
    }
  }
  
  {
    DataFile_t* datafile = DataSet_GetDataFile(dataset) ;
    
    if(datafile) {
      DataFile_Delete(datafile) ;
      Mry_Free(datafile) ;
      DataSet_SetDataFile(dataset,NULL) ;
    }
  }
  
  {
    Geometry_t* geometry = DataSet_GetGeometry(dataset) ;
    
    if(geometry) {
      Geometry_Delete(geometry) ;
      Mry_Free(geometry) ;
      DataSet_SetGeometry(dataset,NULL) ;
    }
  }
  
  {
    Mesh_t* mesh = DataSet_GetMesh(dataset) ;
    
    if(mesh) {
      Mesh_Delete(mesh) ;
      Mry_Free(mesh) ;
      DataSet_SetMesh(dataset,NULL) ;
    }
  }

  {
    Models_t* models = DataSet_GetModels(dataset) ;

    if(models){
      Models_Delete(models);
      Mry_Free(models);
      DataSet_SetModels(dataset,NULL) ;
    }
  }
  
  {
    Materials_t* materials = DataSet_GetMaterials(dataset) ;
    
    if(materials) {
      Materials_Delete(materials) ;
      Mry_Free(materials) ;
      DataSet_SetMaterials(dataset,NULL) ;
    } 
  }
  
  {
    Dates_t* dates = DataSet_GetDates(dataset) ;
    
    if(dates) {
      Dates_Delete(dates) ;
      Mry_Free(dates) ;
      DataSet_SetDates(dataset,NULL) ;
    }
  }
  
  {
    Points_t* points = DataSet_GetPoints(dataset) ;
    
    if(points) {
      Points_Delete(points) ;
      Mry_Free(points) ;
      DataSet_SetPoints(dataset,NULL) ;
    }
  }
  
  {
    IConds_t* iconds = DataSet_GetIConds(dataset) ;
    
    if(iconds) {
      IConds_Delete(iconds) ;
      Mry_Free(iconds) ;
      DataSet_SetIConds(dataset,NULL) ;
    }
  }
  
  {
    BConds_t* bconds = DataSet_GetBConds(dataset) ;
    
    if(bconds) {
      BConds_Delete(bconds) ;
      Mry_Free(bconds) ;
      DataSet_SetBConds(dataset,NULL) ;
    }
  }
  
  {
    Loads_t* loads = DataSet_GetLoads(dataset) ;
    
    if(loads) {
      Loads_Delete(loads) ;
      Mry_Free(loads) ;
      DataSet_SetLoads(dataset,NULL) ;
    }
  }
  
  {
    Functions_t* functions = DataSet_GetFunctions(dataset) ;
    
    if(functions) {
      Functions_Delete(functions) ;
      Mry_Free(functions) ;
      DataSet_SetFunctions(dataset,NULL) ;
    }
  }
  
  {
    Fields_t* fields = DataSet_GetFields(dataset) ;
    
    if(fields) {
      Fields_Delete(fields) ;
      Mry_Free(fields) ;
      DataSet_SetFields(dataset,NULL) ;
    }
  }
  
  //IntFcts_t*     intfcts ; */
  
  {
    ObVals_t* obvals = DataSet_GetObVals(dataset) ;
    
    if(obvals) {
      ObVals_Delete(obvals) ;
      Mry_Free(obvals) ;
      DataSet_SetObVals(dataset,NULL) ;
    }
  }
  
  {
    TimeStep_t* timestep = DataSet_GetTimeStep(dataset) ;
    
    if(timestep) {
      TimeStep_Delete(timestep) ;
      Mry_Free(timestep) ;
      DataSet_SetTimeStep(dataset,NULL) ;
    }
  }
  
  {
    IterProcess_t* iterprocess = DataSet_GetIterProcess(dataset) ;
    
    if(iterprocess) {
      IterProcess_Delete(iterprocess) ;
      Mry_Free(iterprocess) ;
      DataSet_SetIterProcess(dataset,NULL) ;
    }
  }
  
  {
    Module_t* module = DataSet_GetModule(dataset) ;
    
    if(module) {
      Module_Delete(module) ;
      Mry_Free(module) ;
      DataSet_SetModule(dataset,NULL) ;
    }
  }
}



#if 0
DataSet_t*  (DataSet_Create1)(char* filename,Options_t* opt)
{
  DataSet_t* dataset = (DataSet_t*) Mry_New(DataSet_t) ;
  
  DataSet_SetOptions(dataset,opt) ;
  
  {
    DataFile_t* datafile = DataFile_New(filename) ;
  
    DataSet_SetDataFile(dataset,datafile) ;
  
    if(DataFile_DoesNotExist(datafile)) {
      Help_WriteData(filename) ;
      Message_Info("To start the computation, type bil %s\n",filename) ;
      Message_Exit ;
    }
  }
  
  Parser_ParseFile(dataset) ;
  
  {
    char*   debug  = Options_GetPrintedInfos(opt) ;
    
    DataSet_PrintData(dataset,debug) ;
  }
  
  return(dataset) ;
}
#endif



/* Local functions */


#define DATASET       (dataset)
#define DATAFILE      DataSet_GetDataFile(DATASET)
#define GEOMETRY      DataSet_GetGeometry(DATASET)
#define MESH          DataSet_GetMesh(DATASET)
#define MODELS        DataSet_GetModels(DATASET)
#define MATERIALS     DataSet_GetMaterials(DATASET)
#define FIELDS        DataSet_GetFields(DATASET)
#define ICONDS        DataSet_GetIConds(DATASET)
#define BCONDS        DataSet_GetBConds(DATASET)
#define FUNCTIONS     DataSet_GetFunctions(DATASET)
#define LOADS         DataSet_GetLoads(DATASET)
#define POINTS        DataSet_GetPoints(DATASET)
#define DATES         DataSet_GetDates(DATASET)
#define OBVALS        DataSet_GetObVals(DATASET)
#define ITERPROCESS   DataSet_GetIterProcess(DATASET)
#define TIMESTEP      DataSet_GetTimeStep(DATASET)

#define N_EL          Mesh_GetNbOfElements(MESH)
#define EL            Mesh_GetElement(MESH)
#define ELTS          Mesh_GetElements(MESH)

#define INTFCTS       Elements_GetIntFcts(ELTS)

#define NOM           DataFile_GetFileName(DATAFILE)

//#define DIM           Mesh_GetDimension(MESH)
//#define SYMMETRY      Mesh_GetSymmetry(MESH) 
//#define COORSYS       Mesh_GetCoordinateSystem(MESH)

#define DIM           Geometry_GetDimension(GEOMETRY)
#define SYMMETRY      Geometry_GetSymmetry(GEOMETRY) 
#define COORSYS       Geometry_GetCoordinateSystem(GEOMETRY)

#define N_NO          Mesh_GetNbOfNodes(MESH)
#define NO            Mesh_GetNode(MESH)

#define N_MODELS      Models_GetNbOfModels(MODELS)
#define MODEL         Models_GetModel(MODELS)

#define N_MAT         Materials_GetNbOfMaterials(MATERIALS)
#define MAT           Materials_GetMaterial(MATERIALS)

#define N_CH          Fields_GetNbOfFields(FIELDS)
#define CH            Fields_GetField(FIELDS)

#define N_IC          IConds_GetNbOfIConds(ICONDS)
#define IC            IConds_GetICond(ICONDS)

#define N_FN          Functions_GetNbOfFunctions(FUNCTIONS)
#define FN            Functions_GetFunction(FUNCTIONS)

#define N_CL          BConds_GetNbOfBConds(BCONDS)
#define CL            BConds_GetBCond(BCONDS)

#define N_CG          Loads_GetNbOfLoads(LOADS)
#define CG            Loads_GetLoad(LOADS)

#define N_FI          IntFcts_GetNbOfIntFcts(INTFCTS)
#define FI            IntFcts_GetIntFct(INTFCTS)

#define N_POINTS      Points_GetNbOfPoints(POINTS)

#define N_DATES       Dates_GetNbOfDates(DATES)

#define N_OBJ         ObVals_GetNbOfObVals(OBVALS)
#define OBJ           ObVals_GetObVal(OBVALS)


#define PRINT(...) \
        fprintf(stdout,__VA_ARGS__)
        //Message_Direct(__VA_ARGS__)


void DataSet_PrintData(DataSet_t* dataset,std::string const& mot){
  DataSet_PrintData(dataset,mot.c_str());
}
void DataSet_PrintData(DataSet_t* dataset,char const* mot)
{
  static int i_debug=0 ;
  
  if(!strcmp(mot,"\0")) return ;

  PRINT("\n") ;
  PRINT("debug(%d)\n",i_debug++) ;
  PRINT("-----\n") ;
  
  /* File content
   * ------------ */
  if(DataSet_GetDataFile(dataset) && (!strncmp(mot,"data file content",4) || !strncmp(mot,"all",3))) {
    if(DataFile_GetFileContent(DataSet_GetDataFile(dataset))) {
    PRINT("\n") ;
    PRINT("Data file content:\n") ;
    
    PRINT("%s",DataFile_GetFileContent(DataSet_GetDataFile(dataset))) ;
    PRINT("\n") ;
    }
  }

  /* Geometry
   * -------- */
  if(DataSet_GetGeometry(dataset) && (!strncmp(mot,"geometry",4) || !strncmp(mot,"all",3))) {
    PRINT("\n") ;
    Geometry_Print(DataSet_GetGeometry(dataset));
  }

  /* Mesh
   * ---- */
  if(DataSet_GetMesh(dataset) && (!strncmp(mot,"mesh",4) || !strncmp(mot,"all",3))) {
    Nodes_t* nodes = Mesh_GetNodes(MESH) ;
    Elements_t* elts = Mesh_GetElements(MESH) ;
    int c1 = 14 ;
    int c2 = 30 ;
    int c3 = 45 ;
    
    if(nodes && elts) {
    
    PRINT("\n") ;
    PRINT("Mesh:\n") ;

    PRINT("\t Nodes:\n") ;
    PRINT("\t Nb of nodes = %lu\n",N_NO) ;
    
    for(size_t i = 0 ; i < N_NO ; i++) {
      Node_t* node_i = NO + i ;
      int ne = Node_GetNbOfElements(node_i) ;
      int n = PRINT("\t no(%lu)",i) ;
      int j ;
      
      while(n < c1) n += PRINT(" ") ;
      
      n += PRINT(":") ;
      
      for(j = 0 ; j < DIM ; j++) {
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
    PRINT("\t Nb of elements = %lu\n",N_EL) ;
    
    for(size_t i = 0 ; i < N_EL ; i++) {
      Element_t* elt_i = EL + i ;
      int nn = Element_GetNbOfNodes(elt_i) ;
      int n = PRINT("\t el(%lu)",i) ;
      int j ;
      
      while(n < c1) n += PRINT(" ") ;
      
      n += PRINT(":") ;
      
      n += PRINT("  reg(%s)",Element_GetRegionName(elt_i)) ;
      
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
  }

  /* Models
   * --------- */
  if(DataSet_GetModels(dataset) && (!strncmp(mot,"model",5) || !strncmp(mot,"all",3))) {    
    PRINT("\n") ;
    Models_Print(MODELS);
  }

  /* Materials
   * --------- */
  if(DataSet_GetMaterials(dataset) && (!strncmp(mot,"material",3) || !strncmp(mot,"all",3))) {   
    PRINT("\n") ;
    Materials_Print(MATERIALS);
  }

  /* Continuity
   * ---------- */
  if(DataSet_GetMesh(dataset) && (!strncmp(mot,"continuity",3))) {
    Nodes_t* nodes = Mesh_GetNodes(MESH) ;
    Elements_t* elts = Mesh_GetElements(MESH) ;
    
    if(nodes && elts) {

    PRINT("\n") ;
    PRINT("Continuity:\n") ;
    
    PRINT("\t Positions of unknowns and equations at nodes of elements\n") ;
    
    for(size_t i = 0 ; i < N_EL ; i++) {
      Element_t* elt_i = EL + i ;
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
    
    for(size_t i = 0 ; i < N_NO ; i++) {
      Node_t* node_i = NO + i ;
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
  }

  /* Matrix numbering
   * ---------------- */
  if(DataSet_GetMesh(dataset) && !strncmp(mot,"numbering",3)) {
    Nodes_t* nodes = Mesh_GetNodes(MESH) ;
    Elements_t* elts = Mesh_GetElements(MESH) ;
    
    if(nodes && elts) {
    
    PRINT("\n") ;
    PRINT("Matrix numbering:\n") ;
    
    PRINT("\n") ;
    PRINT("\t Matrix indexes of equations and unknowns at nodes:\n") ;
    
    for(size_t i = 0 ; i < N_NO ; i++) {
      Node_t* node_i = NO + i ;
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
  }

  /* Functions
   * --------- */
  if(DataSet_GetFunctions(dataset) && (!strncmp(mot,"function",4) || !strncmp(mot,"all",3))) {
    PRINT("\n") ;
    Functions_Print(DataSet_GetFunctions(dataset));
  }

  /* Fields
   * ------ */
  if(DataSet_GetFields(dataset) && (!strncmp(mot,"field",4) || !strncmp(mot,"all",3))) {    
    PRINT("\n") ;
    Fields_Print(DataSet_GetFields(dataset));
  }

  /* Initial conditions
   * ------------------ */
  if(DataSet_GetIConds(dataset) && (!strncmp(mot,"initialization",3) || !strncmp(mot,"all",3))) {
    char* nom = IConds_GetFileNameOfNodalValues(ICONDS) ;
    int c1 = 14 ;
    
    PRINT("\n") ;
    PRINT("Initial conditions:\n") ;
    
    PRINT("\t Nb of initial condtions = %lu\n",N_IC) ;
    
    if(!nom) {
      FILE*  fic_ini = fopen(nom,"r") ;
      
      if(!fic_ini) {
        arret("DataSet_PrintData: can't open file") ;
      }
    
      PRINT("\n") ;
      PRINT("Initialization of nodal unknowns from %s:\n",nom) ;
    
      for(size_t i = 0 ; i < N_NO ; i++) {
        Node_t* node_i = NO + i ;
        int neq = Node_GetNbOfEquations(node_i) ;
        int n = PRINT("\t no(%lu)",i) ;
      
        while(n < c1) n += PRINT(" ") ;
      
        n += PRINT(":") ;
      
        for(int j = 0 ; j < neq ; j++) {
          double u ;
          
          fscanf(fic_ini,"%le",&u) ;
          n += PRINT(" %d(%e)",j,u) ;
        }
      
        PRINT("\n") ;
        
      }
      
      fclose(fic_ini) ;
    }
    
    IConds_Print(DataSet_GetIConds(dataset));
  }

  /* Boundary conditions
   * ------------------- */
  if(DataSet_GetBConds(dataset) && (!strncmp(mot,"bcondition",4) || !strncmp(mot,"all",3))) {    
    PRINT("\n") ;
    PRINT("Boundary conditions:\n") ;
    
    PRINT("\t Nb of boundary conditions = %lu\n",N_CL) ;
    
    for(size_t i = 0 ; i < N_CL ; i++) {
      char* reg = BCond_GetRegionName(CL + i) ;
      char* name_unk =BCond_GetNameOfUnknown(CL + i) ;
      Field_t* ch = BCond_GetField(CL + i) ;
      Function_t* fn = BCond_GetFunction(CL + i) ;
      
      PRINT("Boundary Condition(%lu):\n",i) ;
      
      //PRINT("\t Region  = %d\n",reg) ;
      PRINT("\t Region  = %s\n",reg) ;
      PRINT("\t Unknown = %s\n",name_unk) ;
      
      if(ch) {
        ptrdiff_t n = ch - CH ;
        
        PRINT("\t Field = %td (type %s)\n",n,Field_GetType(ch)) ;
      } else {
        PRINT("\t Natural boundary condition (null)\n") ;
      }
      
      if(fn) {
        ptrdiff_t n = fn - FN ;
        
        PRINT("\t Function = %td\n",n) ;
      } else {
        PRINT("\t Function unity (f(t) = 1)\n") ;
      }
    }
  }

  /* Loads
   * ----- */
  if(DataSet_GetLoads(dataset) && (!strncmp(mot,"load",4) || !strncmp(mot,"all",3))) {    
    PRINT("\n") ;
    PRINT("Loads:\n") ;
    
    PRINT("\t Nb of loads = %lu\n",N_CG) ;
    
    for(size_t i = 0 ; i < N_CG ; i++) {
      char* reg = Load_GetRegionName(CG + i) ;
      char* name_eqn = Load_GetNameOfEquation(CG + i) ;
      char* type = Load_GetType(CG + i) ;
      Field_t* ch = Load_GetField(CG + i) ;
      Function_t* fn = Load_GetFunction(CG + i) ;
      
      PRINT("Load(%lu):\n",i) ;
      
      //PRINT("\t Region   = %d\n",reg) ;
      PRINT("\t Region   = %s\n",reg) ;
      PRINT("\t Equation = %s\n",name_eqn) ;
      PRINT("\t Type     = %s\n",type) ;
      
      if(ch) {
        ptrdiff_t n = ch - CH ;
        
        PRINT("\t Field = %td (type %s)\n",n,Field_GetType(ch)) ;
        
      } else {
        PRINT("\t Natural load (null)\n") ;
        
      }
      
      if(fn) {
        ptrdiff_t n = fn - FN ;
        
        PRINT("\t Function = %td\n",n) ;
        
      } else {
        PRINT("\t Function unity (f(t) = 1)\n") ;
        
      }
    }
  }

  /* Points
   * ------ */
  if(DataSet_GetPoints(dataset) && (!strncmp(mot,"points",4) || !strncmp(mot,"all",3))) {
    size_t n_points = N_POINTS ;
    Point_t* point = Points_GetPoint(POINTS) ;
    
    PRINT("\n") ;
    PRINT("Points:\n") ;
    
    PRINT("\t Nb of points = %lu\n",n_points) ;

    for(size_t i = 0 ; i < n_points ; i++) {
      double* coor = Point_GetCoordinate(point + i) ;
      double x = (DIM > 0) ? coor[0] : 0. ;
      double y = (DIM > 1) ? coor[1] : 0. ;
      double z = (DIM > 2) ? coor[2] : 0. ;
      Element_t* elt = Point_GetEnclosingElement(point + i) ;
      
      PRINT("\t Point(%lu): ",i) ;
      
      PRINT("(%e,%e,%e)",x,y,z) ;
      
      if(elt) {
        char* reg_el = Element_GetRegionName(elt) ;
        size_t index  = Element_GetElementIndex(elt) ;
        
        PRINT(" in region %s",reg_el) ;
        PRINT(" in element %lu",index) ;
      }
      
      PRINT("\n") ;
    }
  }

  /* Dates
   * ----- */
  if(DataSet_GetDates(dataset) && (!strncmp(mot,"dates",4) || !strncmp(mot,"all",3))) {
    size_t n_dates = N_DATES ;
    Date_t* date = Dates_GetDate(DATES) ;
    
    PRINT("\n") ;
    PRINT("Dates:\n") ;
    
    PRINT("\t Nb of dates = %lu\n",n_dates) ;
    
    for(size_t i = 0 ; i < n_dates ; i++) {
      double t = Date_GetTime(date + i) ;
      
      PRINT("\t Date(%lu): ",i) ;
      
      PRINT("%e\n",t) ;
    }
  }

  /* Time steps
   * ---------- */
  if(DataSet_GetTimeStep(dataset) && (!strncmp(mot,"time",4) || !strncmp(mot,"all",3))) {
    PRINT("\n") ;
    PRINT("Time Step:\n") ;
    PRINT("\t Dtini = %e\n",TimeStep_GetInitialTimeStep(TIMESTEP)) ;
    PRINT("\t Dtmax = %e\n",TimeStep_GetMaximumTimeStep(TIMESTEP)) ;
    PRINT("\t Dtmin = %e\n",TimeStep_GetMinimumTimeStep(TIMESTEP)) ;
    PRINT("\t Max common ratio = %e\n",TimeStep_GetMaximumCommonRatio(TIMESTEP)) ;
    PRINT("\t Reduction factor = %e\n",TimeStep_GetReductionFactor(TIMESTEP)) ;
  }



  /* Iterative process
   * ----------------- */
  if(DataSet_GetIterProcess(dataset) && (!strncmp(mot,"iterations",4) || !strncmp(mot,"all",3))) {
    PRINT("\n") ;
    PRINT("Iterative Process:\n") ;
    PRINT("\t Nb of iterations = %d\n",IterProcess_GetNbOfIterations(ITERPROCESS)) ;
    PRINT("\t Tolerance = %e\n",IterProcess_GetTolerance(ITERPROCESS)) ;
    PRINT("\t Nb of repetitions = %d\n",IterProcess_GetNbOfRepetitions(ITERPROCESS)) ;
  }

  /* Objective variations
   * -------------------- */
  if(DataSet_GetObVals(dataset) && (!strncmp(mot,"obvariations",4) || !strncmp(mot,"all",3))) {    
    PRINT("\n") ;
    PRINT("Objective values:\n") ;
    
    PRINT("\t Nb of objective values = %lu\n",N_OBJ) ;
    
    for(size_t i = 0 ; i < N_OBJ ; i++) {
      PRINT("\t %s = %e",ObVal_GetNameOfUnknown(OBJ + i),ObVal_GetValue(OBJ + i)) ;
      PRINT(" , type = %c",ObVal_GetType(OBJ + i)) ;
      PRINT(" , relaxation factor = %e",ObVal_GetRelaxationFactor(OBJ + i)) ;
      PRINT("\n") ;
    }
  }

  /* Interpolation functions
   * ----------------------- */
  if(DataSet_GetMesh(dataset) && (!strncmp(mot,"interpolation",4))) {
    Nodes_t* nodes = Mesh_GetNodes(MESH) ;
    Elements_t* elts = Mesh_GetElements(MESH) ;
    int i ;
    
    if(nodes && elts) {
    
    PRINT("\n") ;
    PRINT("Interpolation:\n") ;
    
    PRINT("\t Nb of interpolation functions = %d\n",N_FI) ;
    
    for(i = 0 ; i < (int) N_FI ; i++) {
      int np = IntFct_GetNbOfPoints(FI + i) ;
      int nn = IntFct_GetNbOfFunctions(FI + i) ;
      int dim = IntFct_GetDimension(FI + i) ;
      
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
            double* ap = IntFct_GetCoordinatesAtPoint(FI + i,p) ;
            
            PRINT("% e ",ap[k]) ;
          }
        
          PRINT("\n") ;
        }
      }
      
      
      PRINT("\t Weights = ") ;
      
      {
        double* w = IntFct_GetWeight(FI + i) ;
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
            double* hp = IntFct_GetFunctionAtPoint(FI + i,p) ;
            
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
              double* dhp = IntFct_GetFunctionGradientAtPoint(FI + i,p) ;
              
              PRINT("% e ",DHP(l,k)) ;
            }
        
            PRINT("\n") ;
          }
        }
      }
#undef DHJ
    }
  }
  }

  fflush(stdout) ;
}
