/* compilation:
c++ -std=c++17 Richards-2d.cpp -L/usr/local/lib -lbil-2.13-Debug -DBASENAME=" " -fpermissive -g
*/

#include <bil/bil.h>

int main()
{
  DataSet_t* d =  DataSet_New() ;
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

  Geometry_Set(geom,2,"plane") ;
  Mesh_Set(mesh,"columncomposite.msh");
 
  Fields_EmplaceBack(fields,"affine",0.,(double[]){0.,-9810.,0.},(double[]){0.,0.2,0.});

  Functions_EmplaceBack(functions,"piecewiseaffine",(std::vector<double>){0,3600},(std::vector<double>){1,0});

  IConds_EmplaceBack(iconds,"100","p_l",nullptr,1,0);
  IConds_EmplaceBack(iconds,"101","p_l",nullptr,1,0);

  BConds_EmplaceBack(bconds,"11"," ","p_l",1,1);

  Points_EmplaceBack(points,{0.01,0.1,0},"101");
  Points_EmplaceBack(points,{0.01,0.175,0},"100");

  Dates_Set(dates,{0,200,400,600,800,1000,1200,1400,1600,1800,2000,2200,2400,2600,2800,3000});
  
  ObVals_EmplaceBack(obvals,"p_l",1.e3);
  
  IterProcess_Set(iterprocess,20,1.e-6,2);
  
  TimeStep_Set(timestep,1.,1000);
  
  Models_EmplaceBack(models,"Richards");
  
  #if 1
  {
    Material_t* mat1 = Materials_EmplaceBack(materials,"Richards");
    
    Material_Set(mat1,"Gravity",-9.81);
    Material_Set(mat1,"Porosity",0.38);
    Material_Set(mat1,"LiquidMassDensity",1000.);
    Material_Set(mat1,"IntrinsicPermeability",8.9e-12);
    Material_Set(mat1,"ReferenceGasPressure",0.);
    Material_Set(mat1,"LiquidViscosity",0.001);
    Material_Set(mat1,"Curves","billes");
    Material_Finalize(mat1);
  }
  
  {
    Material_t* mat2 = Materials_EmplaceBack(materials,"Richards");
    
    Material_Set(mat2,"Gravity",-9.81);
    Material_Set(mat2,"Porosity",0.38);
    Material_Set(mat2,"LiquidMassDensity",1000.);
    Material_Set(mat2,"IntrinsicPermeability",8.9e-13);
    Material_Set(mat2,"ReferenceGasPressure",0.);
    Material_Set(mat2,"LiquidViscosity",0.001);
    Material_Set(mat2,"Curves","billes");
    Material_Finalize(mat2);
  }
  #endif

  DataSet_PrintData(d,"all") ;
  DataSet_Delete(d) ;
  Mry_Free(d) ;
}
