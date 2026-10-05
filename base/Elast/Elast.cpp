/* compilation:

c++ -std=c++17 Elast.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lbil-2.13-Debug -DBASENAME=" " -fpermissive -g

or if petsc and mpi are included in the library, use:
mpic++ -std=c++17 Elast.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lbil-2.13-Debug -DBASENAME=" " -fpermissive -lpetsc_real -g
or
c++ -std=c++17 Elast.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lbil-2.13-Debug -DBASENAME=" " -fpermissive -lpetsc_real -lmpi_cxx -lmpi -g
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
  
  Session_Open();

  Geometry_Set(geom,2,"axis") ;
  Mesh_Set(mesh,"cylinder.msh");
  
  Mesh_WriteInversePermutation(mesh,"out","hsl");
 
  Fields_EmplaceBack(fields,"affine",1.e6,(double[]){0.,0.,0.},(double[]){0.,0.,0.});

  Functions_EmplaceBack(functions,"piecewiseaffine",(std::vector<double>){0,1,2},(std::vector<double>){1,10,0.1});


  BConds_EmplaceBack(bconds,"80","u_2",0,0);
  
  Loads_EmplaceBack(loads,"10","meca_1","pressure",1,1);
  Loads_EmplaceBack(loads,"20","meca_1","pressure",1,1);
  Loads_EmplaceBack(loads,"30","meca_1","pressure",1,1);
  Loads_EmplaceBack(loads,"50","meca_1","pressure",1,0);
  Loads_EmplaceBack(loads,"60","meca_1","pressure",1,0);
  Loads_EmplaceBack(loads,"70","meca_1","pressure",1,0);
  Loads_EmplaceBack(loads,"80","meca_1","pressure",1,0);

  Points_EmplaceBack(points,{0.15,0.15,0},"600");

  Dates_Set(dates,{0,1,2});
  
  ObVals_EmplaceBack(obvals,"u_1",1.e-4);
  ObVals_EmplaceBack(obvals,"u_2",1.e-4);
  
  IterProcess_Set(iterprocess,5,1.e-4,0);
  
  TimeStep_Set(timestep,1.,100000);
  
  Models_EmplaceBack(models,"Elast");
  
  
  {
    Material_t* mat1 = Materials_EmplaceBack(materials,"Elast");
    
    Material_Set(mat1,"gravity",0.);
    Material_Set(mat1,"rho_s",0.);
    Material_Set(mat1,"sig0_11",-1.e6);
    Material_Set(mat1,"sig0_22",-1.e6);
    Material_Set(mat1,"sig0_33",-1.e6);
    Material_Set(mat1,"young",10.e9);
    Material_Set(mat1,"poisson",0.26);
    Material_Finalize(mat1);
  }
  
  {
    Material_t* mat2 = Materials_EmplaceBack(materials,"Elast");
    
    Material_Set(mat2,"gravity",0.);
    Material_Set(mat2,"rho_s",0.);
    Material_Set(mat2,"sig0_11",-1.e6);
    Material_Set(mat2,"sig0_22",-1.e6);
    Material_Set(mat2,"sig0_33",-1.e6);
    Material_Set(mat2,"young",1.e9);
    Material_Set(mat2,"poisson",0.26);
    Material_Finalize(mat2);
  }
  
  {
    Material_t* mat3 = Materials_EmplaceBack(materials,"Elast");
    
    Material_Set(mat3,"gravity",0.);
    Material_Set(mat3,"rho_s",0.);
    Material_Set(mat3,"sig0_11",-1.e6);
    Material_Set(mat3,"sig0_22",-1.e6);
    Material_Set(mat3,"sig0_33",-1.e6);
    Material_Set(mat3,"young",10.e9);
    Material_Set(mat3,"poisson",0.26);
    Material_Finalize(mat3);
  }
  
  
  DataSet_Finalize(d);

  DataSet_PrintData(d,"all") ;
  
  Options_Set(opt,"-solver petscksp -ksp_type cg -pc_type sor");
  
  Module_ComputeProblem(modul,d);
  
  DataSet_Delete(d) ;
  Mry_Free(d) ;
}
