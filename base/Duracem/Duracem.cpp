/* compilation:

c++ -std=c++17 Duracem.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lbil-2.13-Debug -DBASENAME=" " -fpermissive -g
or if petsc and mpi are included in the library, use:
mpic++ -std=c++17 Duracem.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lbil-2.13-Debug -DBASENAME=" " -fpermissive -lpetsc_real -g
or
c++ -std=c++17 Duracem.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lbil-2.13-Debug -DBASENAME=" " -fpermissive -lpetsc_real -lmpi_cxx -lmpi -g
*/

#include <bil/bil.h>

int main()
{
  DataSet_t* d =  DataSet_New("out") ;
  Options_t* options = DataSet_GetOptions(d);
  Units_t* units = DataSet_GetUnits(d);
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
  
  Session_Open();
  
  Units_EmplaceBack(units,"Length","decimeter");
  Units_EmplaceBack(units,"Mass","hectogram");

  Geometry_Set(geom,1,"plane");
  Mesh_Set(mesh,"3 0. 0. 0.6\n0.0002\n1 100\n1 1");
 
  Fields_EmplaceBack(fields,"affine",1.,(double[]){0.,0.,0.},(double[]){0.,0.,0.});
  Fields_EmplaceBack(fields,"affine",-0.158,(double[]){0.,0.,0.},(double[]){0.,0.,0.});
  Fields_EmplaceBack(fields,"affine",-70.5e6,(double[]){0.,0.,0.},(double[]){0.,0.,0.});
  Fields_EmplaceBack(fields,"affine",0.,(double[]){0.,0.,0.},(double[]){0.,0.,0.});
  Fields_EmplaceBack(fields,"affine",0.03,(double[]){0.,0.,0.},(double[]){0.,0.,0.});
  Fields_EmplaceBack(fields,"affine",1.,(double[]){0.,0.,0.},(double[]){0.,0.,0.});
  Fields_EmplaceBack(fields,"affine",1.,(double[]){0.,0.,0.},(double[]){0.,0.,0.});
  Fields_EmplaceBack(fields,"grid","CN_satb");
  Fields_EmplaceBack(fields,"affine",-1.5,(double[]){0.,0.,0.},(double[]){0.,0.,0.});

  Functions_EmplaceBack(functions,"piecewiseaffine",(std::vector<double>){0,3600},(std::vector<double>){1,0.16});
  Functions_EmplaceBack(functions,"piecewiseaffine",(std::vector<double>){0,60,360,3600,36000,60000,86400,172800},(std::vector<double>){-15,-13.5,-12,-10.5,-6,-4.9,-3,-2.4});

  IConds_EmplaceBack(iconds,"2","logc_co2",1,2);
  IConds_EmplaceBack(iconds,"2","p_l",8,0);
  IConds_EmplaceBack(iconds,"2","psi",0,0);
  IConds_EmplaceBack(iconds,"2","z_si",6,0);
  IConds_EmplaceBack(iconds,"2","z_ca",7,0);
  IConds_EmplaceBack(iconds,"2","logc_oh",9,0);

  BConds_EmplaceBack(bconds,"1","psi",0,0);
  BConds_EmplaceBack(bconds,"1","p_l",3,0);
  BConds_EmplaceBack(bconds,"1","logc_co2",1,2);

  Dates_Set(dates,{0,86400,172800});
  
  ObVals_EmplaceBack(obvals,"logc_co2",0.1);
  ObVals_EmplaceBack(obvals,"p_l",1.e5);
  ObVals_EmplaceBack(obvals,"z_ca",0.1);
  ObVals_EmplaceBack(obvals,"psi",1.);
  ObVals_EmplaceBack(obvals,"logc_na",1.e-1);
  ObVals_EmplaceBack(obvals,"logc_k",1.e-1);
  ObVals_EmplaceBack(obvals,"z_si",1.e-1);
  ObVals_EmplaceBack(obvals,"logc_oh",1.);
  
  IterProcess_Set(iterprocess,20,1.e-3,0);
  
  TimeStep_Set(timestep,10.,3600);
  
  Models_EmplaceBack(models,(std::string)"Duracem",std::vector<std::string>{"carbon","silicon"},std::vector<std::string> {"logc_co2","z_si"});
  
  {
    Material_t* mat1 = Materials_EmplaceBack(materials,"Duracem");
    
    Material_Set(mat1,"InitialPorosity",0.379);
    Material_Set(mat1,"IntrinsicPermeability_liquid",1.4e-17);
    Material_Set(mat1,"InitialContent_portlandite",3.9);
    Material_Set(mat1,"InitialContent_csh",2.4);
    Material_Set(mat1,"InitialConcentration_sodium",0.019);
    Material_Set(mat1,"InitialConcentration_potassium",0.012);
    Material_Set(mat1,"FractionalLengthOfPoreBodies",0.8);
    Material_Set(mat1,"PorosityFractionAtVanishingPermeability",0.7);
    Material_Set(mat1,"Curves","desorbCN");
    Material_Set(mat1,"Curves","relpermCN  s_l = Range{x1 = 0 , x2 = 1 , n = 101} kl_r = Mualem_liq(1){m = 0.45}");
    Material_Set(mat1,"Curves","V_CSH");
    Material_Set(mat1,"Curves_log","csh4t S_CH = Range{x0 = 1.e-20 , x1 = 1 , n = 201} X_CSH = CSHss(1){n = 4 , x1 = 1, y1 = 1.5 , logk1 = 12.5 , x2 = 1.25 , y2 = 1.25 , logk2 = 18.1 , x3 = 1.5 , y3 = 1 , logk3 = 25.3 , x4 = 0 , y4 = 1 , logk4 = -2.7 , logkch = 22.8 , logksh = -2.7} S_SH = CSHss(1){n = 4 , x1 = 1, y1 = 1.5 , logk1 = 12.5 , x2 = 1.25 , y2 = 1.25 , logk2 = 18.1 , x3 = 1.5 , y3 = 1 , logk3 = 25.3 , x4 = 0 , y4 = 1 , logk4 = -2.7 , logkch = 22.8 , logksh = -2.7}");
    Material_Set(mat1,"UseAutodiff",0.);
    Material_Finalize(mat1);
  }
  
  DataSet_Finalize(d);

  //DataSet_PrintData(d,"all") ;
  
  //Module_Set(modul,"Monolithic");
  //Module_Set(modul,"SNIA");
  
  //Options_Set(options,"printlevel","2");
  Module_ComputeProblem(modul,d);
  
  
  DataSet_Delete(d) ;
  Mry_Free(d) ;
}
