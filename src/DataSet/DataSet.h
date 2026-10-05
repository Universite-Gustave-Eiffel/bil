#ifndef DATASET_H
#define DATASET_H

#ifdef __CPLUSPLUS
extern "C" {
#endif


/* Forward declarations */
struct DataSet_t;
struct Options_t;
struct Units_t;
struct DataFile_t;
struct Geometry_t;
struct Mesh_t;
struct Materials_t;
struct Dates_t;
struct Points_t;
struct IConds_t;
struct BConds_t;
struct Loads_t;
struct Functions_t;
struct Fields_t;
struct ObVals_t;
struct TimeStep_t;
struct IterProcess_t;
struct Module_t;
struct Models_t;
struct Context_t;

#include <string>

extern DataSet_t*  (DataSet_New)(std::string const&,Context_t* = nullptr);
extern DataSet_t*  (DataSet_New)       (char const*,Context_t* = nullptr);
extern DataSet_t*  (DataSet_Create)    (char const*,Context_t* = nullptr) ;
extern void        (DataSet_Scan)      (DataSet_t*,DataFile_t*);
extern void        (DataSet_Delete)    (void*) ;
extern void        (DataSet_PrintData) (DataSet_t*,std::string const&) ;
extern void        (DataSet_PrintData) (DataSet_t*,char const*) ;



#define DataSet_GetUnits(DS)          ((DS)->GetUnits())
#define DataSet_GetDataFile(DS)       ((DS)->GetDataFile())
#define DataSet_GetGeometry(DS)       ((DS)->GetGeometry())
#define DataSet_GetMesh(DS)           ((DS)->GetMesh())
#define DataSet_GetMaterials(DS)      ((DS)->GetMaterials())
#define DataSet_GetDates(DS)          ((DS)->GetDates())
#define DataSet_GetPoints(DS)         ((DS)->GetPoints())
#define DataSet_GetIConds(DS)         ((DS)->GetIConds())
#define DataSet_GetBConds(DS)         ((DS)->GetBConds())
#define DataSet_GetLoads(DS)          ((DS)->GetLoads())
#define DataSet_GetFunctions(DS)      ((DS)->GetFunctions())
#define DataSet_GetFields(DS)         ((DS)->GetFields())
#define DataSet_GetObVals(DS)         ((DS)->GetObVals())
#define DataSet_GetModels(DS)         ((DS)->GetModels())
#define DataSet_GetTimeStep(DS)       ((DS)->GetTimeStep())
#define DataSet_GetIterProcess(DS)    ((DS)->GetIterProcess())
#define DataSet_GetOptions(DS)        ((DS)->GetOptions())
#define DataSet_GetModule(DS)         ((DS)->GetModule())

#define DataSet_SetUnits(DS,A)          ((DS)->SetUnits(A))
#define DataSet_SetDataFile(DS,A)       ((DS)->SetDataFile(A))
#define DataSet_SetGeometry(DS,A)       ((DS)->SetGeometry(A))
#define DataSet_SetMesh(DS,A)           ((DS)->SetMesh(A))
#define DataSet_SetMaterials(DS,A)      ((DS)->SetMaterials(A))
#define DataSet_SetDates(DS,A)          ((DS)->SetDates(A))
#define DataSet_SetPoints(DS,A)         ((DS)->SetPoints(A))
#define DataSet_SetIConds(DS,A)         ((DS)->SetIConds(A))
#define DataSet_SetBConds(DS,A)         ((DS)->SetBConds(A))
#define DataSet_SetLoads(DS,A)          ((DS)->SetLoads(A))
#define DataSet_SetFunctions(DS,A)      ((DS)->SetFunctions(A))
#define DataSet_SetFields(DS,A)         ((DS)->SetFields(A))
#define DataSet_SetObVals(DS,A)         ((DS)->SetObVals(A))
#define DataSet_SetModels(DS,A)         ((DS)->SetModels(A))
#define DataSet_SetTimeStep(DS,A)       ((DS)->SetTimeStep(A))
#define DataSet_SetIterProcess(DS,A)    ((DS)->SetIterProcess(A))
#define DataSet_SetOptions(DS,A)        ((DS)->SetOptions(A))
#define DataSet_SetModule(DS,A)         ((DS)->SetModule(A))


#define DataSet_SetInOutBaseName(DS,...)  ((DS)->SetInOutBaseName(__VA_ARGS__))
#define DataSet_Finalize(DS)              ((DS)->Finalize())




#define DataSet_GetSequentialIndex(DS) \
        Module_GetSequentialIndex(DataSet_GetModule(DS))
        
#define DataSet_GetNbOfSequences(DS) \
        Module_GetNbOfSequences(DataSet_GetModule(DS))


#define DataSet_SetSequentialIndex(DS,A) \
        Module_SetSequentialIndex(DataSet_GetModule(DS),A)
        
#define DataSet_SetNbOfSequences(DS,A) \
        Module_SetNbOfSequences(DataSet_GetModule(DS),A)



struct DataSet_t {               /* set of data for the problem to work out */
  private:
  Units_t*       _units ;         /* Units */
  DataFile_t*    _datafile ;      /* data file */
  Geometry_t*    _geometry ;      /* Geometry */
  Mesh_t*        _mesh ;          /* Mesh */
  Materials_t*   _materials ;     /* Materials */
  Dates_t*       _dates ;         /* Dates */
  Points_t*      _points ;        /* Points */
  IConds_t*      _iconds ;        /* Initial Conditions */
  BConds_t*      _bconds ;        /* Boundary Conditions */
  Loads_t*       _loads ;         /* Loadings */
  Functions_t*   _functions ;     /* Functions */
  Fields_t*      _fields ;        /* Fields */
  ObVals_t*      _obvals ;        /* Objective Values */
  Models_t*      _models ;        /* Models */
  TimeStep_t*    _timestep ;      /* time step managing */
  IterProcess_t* _iterprocess ;   /* iterative process */
  Options_t*     _options ;       /* options */
  //Modules_t*     modules ;       /* modules */
  Module_t*      _module ;        /* module */

  public:
  /* The getters */
  Units_t*       GetUnits(){return _units ;}
  DataFile_t*    GetDataFile(){return _datafile ;}
  Geometry_t*    GetGeometry(){return _geometry ;}
  Mesh_t*        GetMesh(){return _mesh ;}
  Materials_t*   GetMaterials(){return _materials ;}
  Dates_t*       GetDates(){return _dates ;}
  Points_t*      GetPoints(){return _points ;}
  IConds_t*      GetIConds(){return _iconds ;}
  BConds_t*      GetBConds(){return _bconds ;}
  Loads_t*       GetLoads(){return _loads ;}
  Functions_t*   GetFunctions(){return _functions ;}
  Fields_t*      GetFields(){return _fields ;}
  ObVals_t*      GetObVals(){return _obvals ;}
  Models_t*      GetModels(){return _models ;}
  TimeStep_t*    GetTimeStep(){return _timestep ;}
  IterProcess_t* GetIterProcess(){return _iterprocess ;}
  Options_t*     GetOptions(){return _options ;}
  Module_t*      GetModule(){return _module ;}

  /* The setters */
  void       SetUnits(Units_t* a){_units = a;}
  void       SetDataFile(DataFile_t* a){_datafile = a;}
  void       SetGeometry(Geometry_t* a){_geometry = a;}
  void       SetMesh(Mesh_t* a){_mesh = a;}
  void       SetMaterials(Materials_t* a){_materials = a;}
  void       SetDates(Dates_t* a){_dates = a;}
  void       SetPoints(Points_t* a){_points = a;}
  void       SetIConds(IConds_t* a){_iconds = a;}
  void       SetBConds(BConds_t* a){_bconds = a;}
  void       SetLoads(Loads_t* a){_loads = a;}
  void       SetFunctions(Functions_t* a){_functions = a;}
  void       SetFields(Fields_t* a){_fields = a;}
  void       SetObVals(ObVals_t* a){_obvals = a;}
  void       SetModels(Models_t* a){_models = a;}
  void       SetTimeStep(TimeStep_t* a){_timestep = a;}
  void       SetIterProcess(IterProcess_t* a){_iterprocess = a;}
  void       SetOptions(Options_t* a){_options = a;}
  void       SetModule(Module_t* a){_module = a;}

  void SetInOutBaseName(char const*);
  void Finalize(void);
} ;


#include "Mesh.h"
#include "Materials.h"
#include "Options.h"
#include "DataFile.h"

  inline void DataSet_t::SetInOutBaseName(char const* filename){
    DataFile_t* df = GetDataFile();
    
    DataFile_Set(df,filename);
  }

  inline void DataSet_t::Finalize(void) {
    Materials_t* materials = GetMaterials() ;
    Mesh_t* mesh = GetMesh() ;
    BConds_t*  bconds = GetBConds() ;
    ObVals_t* obvals = GetObVals();
    Module_t* module = GetModule();

    Mesh_CreateMore(mesh,materials) ;

    Nodes_LinkUpToObVals(Mesh_GetNodes(mesh),obvals);
    Materials_LinkUpToObVals(materials,obvals);
    
    Mesh_GetNbOfMatrices(mesh) = Module_GetNbOfSequences(module) ;
    Mesh_SetMatrixRowColumnIndexes(mesh,bconds) ;
  }



#ifdef __CPLUSPLUS
}
#endif

/* For the macros */
#include "Mry.h"
#include "Module.h"
#endif
