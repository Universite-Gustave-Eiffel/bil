/* compilation:

c++ -std=c++17 ElastWithPBC.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lbil-2.14-Release -DBASENAME=" " -fpermissive

or if petsc and mpi are included in the library, use:
mpic++ -std=c++17 Elast.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lbil-2.13-Debug -DBASENAME=" " -fpermissive -lpetsc_real -g
or
c++ -std=c++17 Elast.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lbil-2.14-Release -DBASENAME=" " -fpermissive -lpetsc_real -lmpi_cxx -lmpi
*/

#include <bil/bil.h>

int main()
{
  DataSet_t* d =  DataSet_New("out") ;
  Geometry_t* geom = DataSet_GetGeometry(d);
  Mesh_t* mesh = DataSet_GetMesh(d);
  Fields_t* fields = DataSet_GetFields(d);
  Functions_t* functions = DataSet_GetFunctions(d);
  IConds_t* iconds = DataSet_GetIConds(d);
  BConds_t* bconds = DataSet_GetBConds(d);
  Loads_t* loads = DataSet_GetLoads(d);
  Dates_t* dates = DataSet_GetDates(d);
  Points_t* points = DataSet_GetPoints(d);
  ObVals_t* obvals = DataSet_GetObVals(d);
  IterProcess_t* iterprocess = DataSet_GetIterProcess(d);
  TimeStep_t* timestep = DataSet_GetTimeStep(d);
  Materials_t* materials = DataSet_GetMaterials(d);
  Models_t* models = DataSet_GetModels(d);
  Module_t* modul = DataSet_GetModule(d);
  Options_t* opt = DataSet_GetOptions(d);
  Periodicities_t* periodicities = Geometry_GetPeriodicities(geom);
  
  Session_Open();

  Geometry_Set(geom,2,"plane") ;
  Mesh_Set(mesh,"composite1.msh");
  
  Mesh_WriteInversePermutation(mesh,"out","hsl");
  
  {
    double v1[3] = {2,0,0};
    double v2[3] = {0,2,0};
    
    Periodicities_EmplaceBack(periodicities,"105","13",v1);
    Periodicities_EmplaceBack(periodicities,"114","125",v1);
    Periodicities_EmplaceBack(periodicities,"115","104",v2);
    Periodicities_EmplaceBack(periodicities,"124","14",v2);
  }
 

  {
    std::vector<double> t = {0,5};
    std::vector<double> f = {0,5};
    
    Functions_EmplaceBack(functions,"piecewiseaffine",t,f);
  }


  BConds_EmplaceBack(bconds,"1","u_1",0,0);
  BConds_EmplaceBack(bconds,"1","u_2",0,0);

  Dates_Set(dates,{0,1,2,3,4,5});
  
  ObVals_EmplaceBack(obvals,"u_1",1.e-4);
  ObVals_EmplaceBack(obvals,"u_2",1.e-4);
  
  IterProcess_Set(iterprocess,10,1.e-4,0);
  
  TimeStep_Set(timestep,0.1,1);
  
  
  {
    Material_t* mat1 = Materials_EmplaceBack(materials,"Elast");
    
    Material_Set(mat1,"gravity",0.);
    Material_Set(mat1,"rho_s",0.);
    Material_Set(mat1,"sig0_11",0.);
    Material_Set(mat1,"sig0_22",0.);
    Material_Set(mat1,"sig0_33",0.);
    Material_Set(mat1,"young",2413.e6);
    Material_Set(mat1,"poisson",0.339);
    Material_Set(mat1,"macro-gradient_12",1.e-3);
    Material_Set(mat1,"macro-gradient_21",1.e-3);
    Material_Set(mat1,"macro-fctindex_12",1);
    Material_Set(mat1,"macro-fctindex_21",1);
    Material_Finalize(mat1);
  }
  
  {
    Material_t* mat2 = Materials_EmplaceBack(materials,"Elast");
    
    Material_Set(mat2,"gravity",0.);
    Material_Set(mat2,"rho_s",0.);
    Material_Set(mat2,"sig0_11",0.);
    Material_Set(mat2,"sig0_22",0.);
    Material_Set(mat2,"sig0_33",0.);
    Material_Set(mat2,"young",24130.e6);
    Material_Set(mat2,"poisson",0.49);
    Material_Set(mat2,"macro-gradient_12",1.e-3);
    Material_Set(mat2,"macro-gradient_21",1.e-3);
    Material_Set(mat2,"macro-fctindex_12",1);
    Material_Set(mat2,"macro-fctindex_21",1);
    Material_Finalize(mat2);
  }
  
  
  DataSet_Finalize(d);

  DataSet_PrintData(d,"all") ;
  
  //Options_Set(opt,"-solver petscksp -ksp_type cg -pc_type sor");
  
  Module_ComputeProblem(modul,d);
  
  {
    PosFilesForGMSH_t* pf4gmsh = PosFilesForGMSH_Create(d);
    
    PosFilesForGMSH_ParsedFileFormat(pf4gmsh);
  }
  
  DataSet_Delete(d) ;
  Mry_Free(d) ;
}
