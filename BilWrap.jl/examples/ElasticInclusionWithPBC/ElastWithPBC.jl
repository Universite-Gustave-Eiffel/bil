"""
   Usage from within this directory:
   
      julia ElastWithPBC.jl
"""

include("../../src/BilWrap.jl")
using .BilWrap

  d =  DataSet_New("out")
  geom = DataSet_GetGeometry(d)
  mesh = DataSet_GetMesh(d)
  fields = DataSet_GetFields(d)
  functions = DataSet_GetFunctions(d)
  iconds = DataSet_GetIConds(d)
  bconds = DataSet_GetBConds(d)
  loads = DataSet_GetLoads(d)
  dates = DataSet_GetDates(d)
  points = DataSet_GetPoints(d)
  obvals = DataSet_GetObVals(d)
  iterprocess = DataSet_GetIterProcess(d)
  timestep = DataSet_GetTimeStep(d)
  materials = DataSet_GetMaterials(d)
  models = DataSet_GetModels(d)
  modul = DataSet_GetModule(d)
  opt = DataSet_GetOptions(d)
  periodicities = Geometry_GetPeriodicities(geom)
  
  Session_Open()

  Geometry_Set(geom,2,"plane")
  Mesh_Set(mesh,"composite1.msh")
  
  Mesh_WriteInversePermutation(mesh,"out","hsl")
  
    Periodicities_EmplaceBack(periodicities,"105","13",[2.,0.,0.])
    Periodicities_EmplaceBack(periodicities,"114","125",[2.,0.,0.])
    Periodicities_EmplaceBack(periodicities,"115","104",[0.,2.,0.])
    Periodicities_EmplaceBack(periodicities,"124","14",[0.,2.,0.])
 

    Functions_EmplaceBack(functions,"piecewiseaffine",[0.,5.],[0.,5.])


  BConds_EmplaceBack(bconds,"1","u_1",0,0)
  BConds_EmplaceBack(bconds,"1","u_2",0,0)

  Dates_Set(dates,[0.,1.,2.,3.,4.,5.])
  
  ObVals_EmplaceBack(obvals,"u_1",1.e-4)
  ObVals_EmplaceBack(obvals,"u_2",1.e-4)
  
  IterProcess_Set(iterprocess,10,1.e-4,0)
  
  TimeStep_Set(timestep,0.1,1)
  
  
    mat1 = Materials_EmplaceBack(materials,"Elast")
    Material_Set(mat1,"gravity",0.)
    Material_Set(mat1,"rho_s",0.)
    Material_Set(mat1,"sig0_11",0.)
    Material_Set(mat1,"sig0_22",0.)
    Material_Set(mat1,"sig0_33",0.)
    Material_Set(mat1,"young",2413.e6)
    Material_Set(mat1,"poisson",0.339)
    Material_Set(mat1,"macro-gradient_12",1.e-3)
    Material_Set(mat1,"macro-gradient_21",1.e-3)
    Material_Set(mat1,"macro-fctindex_12",1)
    Material_Set(mat1,"macro-fctindex_21",1)
    Material_Finalize(mat1)
  
    mat2 = Materials_EmplaceBack(materials,"Elast")
    Material_Set(mat2,"gravity",0.)
    Material_Set(mat2,"rho_s",0.)
    Material_Set(mat2,"sig0_11",0.)
    Material_Set(mat2,"sig0_22",0.)
    Material_Set(mat2,"sig0_33",0.)
    Material_Set(mat2,"young",24130.e6)
    Material_Set(mat2,"poisson",0.49)
    Material_Set(mat2,"macro-gradient_12",1.e-3)
    Material_Set(mat2,"macro-gradient_21",1.e-3)
    Material_Set(mat2,"macro-fctindex_12",1)
    Material_Set(mat2,"macro-fctindex_21",1)
    Material_Finalize(mat2)
  
  
  DataSet_Finalize(d)

  DataSet_PrintData(d,"all")
  
  #Options_Set(opt,"-solver petscksp -ksp_type cg -pc_type sor")
  
  Module_ComputeProblem(modul,d)
  
    pf4gmsh = PosFilesForGMSH_Create(d)
    PosFilesForGMSH_ParsedFileFormat(pf4gmsh)
  
  DataSet_Delete(d)
