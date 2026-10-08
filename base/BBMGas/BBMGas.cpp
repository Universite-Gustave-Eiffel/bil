/* compilation:

c++ -std=c++17 BBMGas.cpp -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lbil-2.14-Release -DBASENAME=" " -fpermissive

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
  
  Session_Open();

  Geometry_Set(geom,2,"axis") ;
  Mesh_Set(mesh,"carre.msh");
  
  Mesh_WriteInversePermutation(mesh,"out","hsl");
 
  {
    double g[3] = {0,0,0};
    double x[3] = {0,0,0};
    
    Fields_EmplaceBack(fields,"affine",-1.e3,g,x);
    Fields_EmplaceBack(fields,"affine",1.e3,g,x);
    Fields_EmplaceBack(fields,"affine",-100.e6,g,x);
    Fields_EmplaceBack(fields,"affine",1.e5,g,x);
  }

  {
    std::vector<double> t1 = {0,1,2,3,4,5,6};
    std::vector<double> f1 = {1,40,1,80,1,160,1};
    std::vector<double> t2 = {0,1.999,2,3.999,4,5.999,6};
    std::vector<double> f2 = {0,0,40,40,80,80,160};
    
    Functions_EmplaceBack(functions,"piecewiseaffine",t1,f1);
    Functions_EmplaceBack(functions,"piecewiseaffine",t2,f2);
  }


  BConds_EmplaceBack(bconds,"14","u_1",0,0);
  BConds_EmplaceBack(bconds,"11","u_2",0,0);
  BConds_EmplaceBack(bconds,"100","p_l",1,2);
  BConds_EmplaceBack(bconds,"100","p_g",0,0);
  
  Loads_EmplaceBack(loads,"13","meca_1","pressure",2,1);
  Loads_EmplaceBack(loads,"12","meca_1","pressure",2,1);

  Points_EmplaceBack(points,{0.5,0.5,0});

  Dates_Set(dates,{0,1,2,3,4,5,6});
  
  ObVals_EmplaceBack(obvals,"u_1",1.e-4);
  ObVals_EmplaceBack(obvals,"u_2",1.e-4);
  ObVals_EmplaceBack(obvals,"p_l",1.e3);
  ObVals_EmplaceBack(obvals,"p_g",1.e3);
  
  IterProcess_Set(iterprocess,10,1.e-6,0);
  
  TimeStep_Set(timestep,1.e-4,1.e-3);
  
  //Models_EmplaceBack(models,"BBMGas");
  
  
  {
    Material_t* mat1 = Materials_EmplaceBack(materials,"BBMGas");
    
    Material_Set(mat1,"gravity",0.);
    Material_Set(mat1,"rho_s",2000.);
    Material_Set(mat1,"slope_of_swelling_line",0.011);
    Material_Set(mat1,"slope_of_virgin_consolidation_line",0.065);
    Material_Set(mat1,"shear_modulus",1.e8);
    Material_Set(mat1,"slope_of_critical_state_line",1.2);
    Material_Set(mat1,"initial_pre-consolidation_pressure",0.04e6);
    Material_Set(mat1,"reference_consolidation_pressure",0.01e6);
    Material_Set(mat1,"kappa_s",0.005);
    Material_Set(mat1,"initial_stress_11",-1000.);
    Material_Set(mat1,"initial_stress_22",-1000.);
    Material_Set(mat1,"initial_stress_33",-1000.);
    Material_Set(mat1,"initial_porosity",0.25);
    Material_Set(mat1,"rho_l",1000.);
    Material_Set(mat1,"kl_int",1e-20);
    Material_Set(mat1,"kg_int",1e-18);
    Material_Set(mat1,"mu_l",0.001);
    Material_Set(mat1,"mu_g",1.81e-5);
    Material_Set(mat1,"suction_cohesion_coefficient",0.8);
    Material_Set(mat1,"poisson",0.3);
    Material_Set(mat1,"vapor_diffusion_coefficient",2.82e-5);

    Material_Set(mat1,"Curves","wrc2   pc = Range{x1 = 0 , x2 = 30.e6, n = 171}  sl = Expressions(1){p0 = 1.e6 ; m = 0.6 ; sl = (1 + (pc/p0)**(1/(1-m)))**(-m)}");
    Material_Set(mat1,"Curves","krc2   pc = Range{x1 = 0 , x2 = 30.e6, n = 171}  kl = Expressions(1){s0 = 30.e6 ; kl = 10**(- 5*pc/s0)} kg = Expressions(1){kg = 1}");
    Material_Set(mat1,"Curves","lc   pc = Range{x1 = 0 , x2 = 1.e6, n = 200}  lc = Expressions(1){l0 = 0.065 ; k = 0.011 ; beta = 20.e-6 ; r = 0.75 ; lc = (l0 - k)/(l0*((1-r)*exp(-beta*pc) + r) - k)}");
    Material_Set(mat1,"Curves","kappa_s_p pnet = Range{x1 = 1.e3 , x2 = 10.e6, n = 400} kappa_s_p = Expressions(1){kappas0 = 0.1; alphas = 0.19; patm = 1.e5; kappa_s_p = kappas0*(1 + alphas*log(pnet/patm));}");
    Material_Set(mat1,"Curves","kappa suction = Range{x1 = 1.e3 , x2 = 10.e6, n = 400} kappa = Expressions(1){kappa0 = 0.011; alpha = 1.727; patm = 1.e5; kappa = kappa0*(1 + alpha*log(suction/patm+1));}");
    Material_Set(mat1,"Curves","kappa_s_s pc = Range{x1 = 1.e3 , x2 = 200.e6, n = 2} kappa_s_s = Expressions(1){kappa_s_s = 1}");
    Material_Finalize(mat1);
  }
  
  
  DataSet_Finalize(d);

  DataSet_PrintData(d,"all") ;
  
  //Options_Set(opt,"-solver petscksp -ksp_type cg -pc_type sor");
  
  Module_ComputeProblem(modul,d);
  
  DataSet_Delete(d) ;
  Mry_Free(d) ;
}
